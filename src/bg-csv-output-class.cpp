


#include "bg-csv-output-class.h"
#include "bg-observation-class.h"
#include <ctime>
#include <fstream>
#include <iomanip>

BgCsvOutput::BgCsvOutput(string& outDir, string& inputFlatLog):
    m_iFile(NULL), m_oFile(NULL), m_inputFile(inputFlatLog), m_outDir(outDir)
{
}

bool WriteToCsvTest(Json::Value& item)
{
    if (item.isMember("CsvTrack"))
        if (item["CsvTrack"].asBool() == true)
            return true;
    if (item.isMember("CsvRequired"))
    {
        if (item["CsvRequired"].asBool() == true)
            return true;
    }
    return false;
}

/**
 * @brief Construct an array of items to be tracked in the CSV file.
 * 
 * This function will examine the BgDataDefs.json file, identifying DataItem
 * elements that have a "CsvTrack" member equal to TRUE. It will populate a
 * member variables (m_csvTrackedItems) with the names of the items/columns
 * to be written.
 * @return True if successful; otherwise, false.
*/
bool BgCsvOutput::AnalyzeDataDefs()
{

    // DataItems is an array of JSON strings, one for each data item
    Json::Value items = m_bgDataDefs["DataItems"];

    // Construct a vector of the items to be logged to the CSV file
    // (a vector of strings with the Expedition name of the variable, in the
    // position corresponding to the CSV file column)
    m_csvTrackedItems.resize(MAX_CSV_COLUMNS);
    m_precisions.resize(MAX_CSV_COLUMNS);
    size_t cols = 0;
    for (Json::Value::ArrayIndex i = 0; i != items.size(); i++)
    {
        Json::Value item = items[i];
        if (WriteToCsvTest(item))
        {

            if (!item.isMember("CsvHeader"))
            {
                cout << "CsvColumn is missing for item ID: " << item["ID"].asUInt() << endl;
                continue;
            }
            if (!item.isMember("CsvColumn"))
            {
                cout << "CsvHeader is missing for item ID: " << item["ID"].asUInt() << endl;
                continue;
            }

            // All required items are present
            // Determine which CSV column this is and extend the total count if necessary
            size_t col = item["CsvColumn"].asUInt();
            if (col > cols) cols = col + 1;

            string header = item["CsvHeader"].asString();
            m_csvTrackedItems[col-1] = header;
            m_ExpNameToCsvCol[item["CsvHeader"].asString()] = item["CsvColumn"].asUInt();
            m_CsvColtoExpName[item["CsvColumn"].asUInt()] = item["CsvHeader"].asString();
            m_precisions[item["CsvColumn"].asUInt() - 1] = (unsigned char)item["Decimals"].asUInt();

            // Add this item to the B&G ID to CSV column map
            cout << item.toStyledString() << endl;
            if (item.isMember("ID"))
            {
                m_bgToCsvMap[item["ID"].asUInt()] = item["CsvColumn"].asUInt();
                m_BgIDToBgName[item["ID"].asUInt()] = item["Name"].asString();
                m_BgNameToBgID[item["Name"].asString()] = item["ID"].asUInt();
            }

        }
    }
    m_csvTrackedItems.resize(cols);
    m_precisions.resize(cols);
    m_observations.resize(cols);
    m_obsSeen.resize(cols);
    return true;
}

bool BgCsvOutput::LoadDataDefs()
{
    std::ifstream file("BgDataDefs.json");
    if (file.is_open()) {
        file >> m_bgDataDefs;
        file.close();
    }
    else {
        cerr << "Couldn't open or read BgDataDefs.json" << endl;
        exit(-1);
    }

    if (!AnalyzeDataDefs())
    {
        cerr << "Error analyzing the BgDataDefs.json file" << endl;
        exit(-1);
    }

    return true;
}

int BgCsvOutput::ProcessFlatLog()
{

    // Open BgDataDefs.json and load into a json object
    LoadDataDefs();

    // Open flat log file 
    if (m_iFile == NULL)
    {
        m_iFile = fopen(m_inputFile.c_str(), "r");
        if (m_iFile == NULL)
        {
            cout << "Failed to open file: " << m_inputFile << endl;
            exit(-1);
        }
    }

    // Read each line in the file, passing it to BgH5000CsvMaker::ProcessObservation
	char buffer[200];
	while (true) {

		// Get the next flat log line from the input file
		if (fgets(buffer, sizeof(buffer), m_iFile) == NULL) {
			if (feof(m_iFile)) {
				cout << "ok" << endl;
			}
			else {
				cerr << "Error reading file" << endl;
			}
			break;
		}
		else {

            string s = buffer;
			BgObservation o(s);
            ProcessObservation(o);
		}
	}

    
    // Close input and output files and exit
    if (m_iFile)
		fclose(m_iFile);
	if (m_oFile)
		fclose(m_oFile);

    return 0;
}


/**
 * @brief Identify the type of an incoming observation and handle it accordingly.
 *
 * After a BgObservation object is created from an incoming JSON message, this
 * function "handles" it.
 *
 * The key elements are UtcData (id=34) and UtcTime (id=35), which trigger new
 * output files (in the case of UtcData) and a new collection of observations
 * (in the case of UtcTime, if the new time is more than 1.5 seconds after the
 * most recent time previously observed).
 *
 * Other observation id's are either used to update tracked data or ignored.
 * (There are dozens of id's returned by an H5000 system with a lot of
 * connected devices, so it doesn't necessarily make sense to track them all.
 * But it is possible with some changes to the code in this function.)
 *
 * @param o A BgObservation object with the new data.
*/
void BgCsvOutput::ProcessObservation(BgObservation& o) {

    unsigned int csvColT, csvColD, csvColO;

    //cout << o.getId() << " [" << m_BgIDToBgName[o.getId()] << "]: " << o.getVal() << endl;

    // Select the handling code corresponding to the data id
    switch (o.getId()) {

    case 34:		// UTC (date)
    {
        try {
            csvColD = m_bgToCsvMap.at(o.getId()) - 1;
            csvColT = m_bgToCsvMap.at(35) - 1;
        }
        catch (out_of_range &e) {
            cout << "out of range: " << e.what() << endl;
            break;
        }

        // Convert the date from Julian Date to an Excel-compatible date (days since Jan 0, 1900)
        m_observations[csvColD] = o.getVal() - 2415019;
        m_obsSeen[csvColD] = true;
        unsigned long int utcdate = static_cast<int>(m_observations[csvColD]);

        // Test to see if this is the first "datestamp" observed
        if (m_rawTimestamp == 0)
        {
            // Test to see if a timestamp has been observed yet
//            if (((int)m_observations[csvColT]) != 0)
            if (m_obsSeen[csvColT])
            {
                // OK. We've just observed the first datestamp and have 
                // received a timestamp, so start a new output file for the 
                // new date
                NewDate(utcdate);
            }
        }
        else {
            // Test to see if the UTC Date has rolled over to the next day
            // todo Build the test to see if the UTC date has changed
            if (false) {
                NewDate(utcdate);
            }
        }

        break;
    }

    case 35:		// UTC (time)
    {
        try {
            csvColT = m_bgToCsvMap.at(o.getId()) - 1;
            csvColD = m_bgToCsvMap.at(34) - 1;
        }
        catch (out_of_range &e) {
            cout << "out of range 2: " << e.what() << endl;
            break;
        }
        m_observations[csvColT] = o.getVal();
        m_obsSeen[csvColT] = true;
        unsigned long int utctime = static_cast<int>(m_observations[csvColT]);

        // Test to see if this is the first "timestamp" observed
        if (m_rawTimestamp == 0)
        {
            // Test to see if a 'datestamp' has been observed yet
            unsigned long int utcdate = static_cast<int>(m_observations[csvColD]);
            if (m_obsSeen[csvColD])
            {
                // OK. We've just observed the first timestamp and have 
                // previously seen a 'datestamp,' so start a new output file 
                // for the new date
                NewDate(utcdate);

                // Set the rawTimestamp
                m_rawTimestamp = static_cast<uint64_t>(o.getVal());
            }
        }
        else
        {
            // Test to see if this time is within 2 seconds of the current stamp
            if (o.getVal() - m_rawTimestamp > 1.5)
            {
                NewTime(utctime);
            }
        }

        // Set the time currently being tracked
        //m_time = o.strVal();
        break;
    }

    default:
        try {
            csvColO = m_bgToCsvMap.at(o.getId()) - 1;
        }
        catch (out_of_range &e) {
            (void)e;        // silence compiler warnings about unused variables
            break;
        }
        m_observations[csvColO] = o.getVal();
        m_obsSeen[csvColO] = true;
        break;
    }
}


/**
 * @brief Start a new .csv file and re-initialize the collection of observations.
 *
 * @param utcdate_ An integer representation of the number of days since Jan 0, 1900.
*/
void BgCsvOutput::NewDate(unsigned long int utcdate)
{
    // Write the accumulated information for the most recent timestamp to 
    // the .csv file, then close it (if the file was open in the first place)
    if (m_oFile != NULL) {
        // todo check/fix date rollover 
        NewTime(0);     // Cause the latest data to be written to the file
        fclose(m_oFile);
    }

    // Open a new .csv output file for observations from this new date
    m_oFile = fopen(MakeFileName(utcdate).c_str(), "w");

    // Write a line of column headings
    string headings = MakeHeaderString() + '\n';
    fputs(headings.c_str(), m_oFile);

    // Clear the stored observations (and zero the time)
    Clear();
    unsigned int csvColT;
    try {
        csvColT = m_bgToCsvMap.at(35) - 1;
    }
    catch (out_of_range &e) {
        cerr << "Error in NewDate: " << e.what() << endl;
        exit(-1);
    }
    m_obsSeen[csvColT] = false;
}

/**
 * @brief Write the accumulated observations for the old timestamp to the file.
 *
 * When a new timestamp is observed, the existing observations are written to
 * the .csv output file (identified with the old timestamp), the tracked data
 * values are cleared, and a new collection of observations is started.
 *
 * @param utctime_ An integer representation of the number of seconds since midnight UTC.
*/
void BgCsvOutput::NewTime(unsigned long int utctime)
{
    unsigned int csvColT, csvColD, csvColCombo, csvBoat;
    try {
        csvColT = m_bgToCsvMap.at(35) - 1;
        csvColD = m_bgToCsvMap.at(34) - 1;
        csvColCombo = m_ExpNameToCsvCol["Utc"] - 1;
        csvBoat = m_ExpNameToCsvCol["Boat"] - 1;
    }
    catch (out_of_range &e) {
        cerr << "Error in NewDate: " << e.what() << endl;
        exit(-1);
    }

    // Create an Excel-compatible "Utc" field and populate the "Boat" field with 0
    double UtcDateTime = m_observations[csvColD] + (m_observations[csvColT] / (24.0 * 60.0 * 60.0));
    cout << setprecision(10) << UtcDateTime << endl;
    m_observations[csvColCombo] = UtcDateTime;
    m_obsSeen[csvColCombo] = true;
    m_observations[csvBoat] = 0;
    m_obsSeen[csvBoat] = true;


    // Create a string with the values in CSV format
    stringstream ss;
    ss << fixed;
    vector<double>::const_iterator it;
    vector<unsigned char>::const_iterator jt;
    vector<bool>::const_iterator kt;
    for (it = m_observations.cbegin(), jt = m_precisions.cbegin(), kt = m_obsSeen.cbegin(); 
        it != m_observations.cend(); it++, jt++, kt++)
    {
        if (*kt)
            ss << setprecision(*jt) << *it;
        ss << ",";

    }
    string s = ss.str();
    s.pop_back();				// Remove trailing space
    s += '\n';


    // Write the line to the output file
    fputs(s.c_str(), m_oFile);
    fflush(m_oFile);

    // Clear the stored observations (except date and time)
    Clear();
    m_observations[csvColT] = utctime;

    // Set raw timestamp value
    m_rawTimestamp = static_cast<uint64_t>(m_observations[csvColT]);
}

/**
 * @brief Clear accumulated observation data from the object.
 *
 * This function is called immediately after a timestamp set of values has been
 * written to the .csv file (so intermittent observations/values are dropped
 * and not carried forward).
*/
void BgCsvOutput::Clear()
{
    // Save the date and time most recently observed
    unsigned int csvColD, csvColT;
    try {
        csvColD = m_bgToCsvMap.at(34) - 1;
        csvColT = m_bgToCsvMap.at(35) - 1;
    }
    catch (out_of_range &e) {
        cerr << "Error in Clear: " << e.what() << endl;
        exit(-1);
    }

    double date = m_observations[csvColD];
    double time = m_observations[csvColT];

    // Clear the entire vector of data observation values
    fill(m_observations.begin(), m_observations.end(), 0.0);

    // Restore the date and time values to initialize the new set of observations
    m_observations[csvColD] = date;
    m_observations[csvColT] = time;
}

/**
 * @brief Construct the .csv file name, including the date of the observations.
 *
 * @param utcdate_ An integer representing the number of days since Jan 0, 1900.
 * @return A string with the new file name.
*/
string BgCsvOutput::MakeFileName(unsigned long int utcdate)
{
    // Convert the Excel-format date to a tm struct (in UTC time)
    double dateExcel = utcdate;
    time_t tsUnix = (time_t)(dateExcel - 25569) * 86400;
    struct tm* dateTm = gmtime(&tsUnix);

    // Create the filename buffer, including the date
    char buffer[100];
    strftime(buffer, 100, "%Y%m%d-h5000-cpu-data.csv", dateTm);

    return string(buffer);
}

string BgCsvOutput::MakeHeaderString()
{
    // Iterate through the array to construct the header
    string headerStr;
    for (vector<string>::const_iterator it = m_csvTrackedItems.cbegin(); it != m_csvTrackedItems.cend(); it++)
    {
        headerStr += *it;
        headerStr += ',';
    }

    // Drop the trailing comma and return the string
    headerStr = headerStr.substr(0, headerStr.size() - 2);
    return headerStr;

}
