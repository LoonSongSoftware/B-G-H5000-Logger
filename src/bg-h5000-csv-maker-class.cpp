


#include "bg-h5000-csv-maker-class.h"
#include "bg-observation-class.h"
#include <ctime>
#include <fstream>

BgH5000CsvMaker::BgH5000CsvMaker(int argc, char** argv):
    m_iFile(NULL), m_oFile(NULL)
{
    m_inputFile = "\\\\wfiles\\OtherData\\BulkData\\Sailing\\Logs\\B&G\\202010015 flatlog.log";
//    m_inputFile = argv[1];
}

string MakeCsvHeaders(Json::Value& dataDefs) 
{

    // DataItems is an array of JSON strings, one for each data item
    vector<string> headers(MAX_CSV_COLUMNS);
    size_t nCols = 0;   // Identifies the highest column actually populated
    Json::Value items = dataDefs["DataItems"];
    for (Json::Value::ArrayIndex i = 0; i != items.size(); i++)
    {
        Json::Value item = items[i];
        if (item.isMember("TrackCsv"))
        {
            if (item.isMember("ExpName") && item.isMember("ExpColumn"))
            {
                // All required items are present
                if (item["TrackCsv"].asBool())
                {
                    size_t col = item["ExpColumn"].asUInt();
                    if (col > nCols) nCols = col + 1;
                    string header = item["ExpName"].asString();
                    headers[col-1] = header;

                }
            }
            else
            {
                // todo identify the DataItem and specific missing element
                cout << "Missing elements in BgDataDefs.json" << endl;
                exit(1);
            }
        }
    }
    headers.resize(nCols);
}

int BgH5000CsvMaker::run()
{

    // Open BgDataDefs.json and load into a json object
    std::ifstream file("../../resources/BgDataDefs.json");
    if (file.is_open()) {
        file >> m_bgDataDefs;
        file.close();
    }
    else {
        cerr << "Couldn't open or read BgDataDefs.json" << endl;
        exit(-1);
    }

    string m_csvHeaders = MakeCsvHeaders(m_bgDataDefs);

    cout << m_bgDataDefs.toStyledString() << endl;

    // Open flat log file 
    if (m_iFile == NULL)
    {
        m_iFile = fopen(m_inputFile.c_str(), "r");
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
				cout << "Error reading file" << endl;
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
void BgH5000CsvMaker::ProcessObservation(BgObservation& o) {

    // Select the handling code corresponding to the data id
    switch (o.getId()) {

    case 34:		// UTC (date)
    {
        // Convert the date from Julian Date to an Excel-compatible date (days since Jan 0, 1900)
        m_observations[static_cast<int>(BgH5000CsvMaker::trackedData::UtcDate)] = o.getVal() - 2415019;
        unsigned long int utcdate = static_cast<int>(m_observations[static_cast<int>(BgH5000CsvMaker::trackedData::UtcDate)]);

        // Test to see if this is the first "datestamp" observed
        if (m_rawTimestamp == 0)
        {
            // Test to see if a timestamp has been observed yet
            if (((int)m_observations[static_cast<int>(BgH5000CsvMaker::trackedData::UtcTime)]) != 0)
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

        // Set the date currently being tracked
        //m_date = o.strVal();
        break;
    }

    case 35:		// UTC (time)
    {
        m_observations[static_cast<int>(BgH5000CsvMaker::trackedData::UtcTime)] = o.getVal();
        unsigned long int utctime = static_cast<int>(m_observations[static_cast<int>(BgH5000CsvMaker::trackedData::UtcTime)]);

        // Test to see if this is the first "timestamp" observed
        if (m_rawTimestamp == 0)
        {
            // Test to see if a 'datestamp' has been observed yet
            unsigned long int utcdate = static_cast<int>(m_observations[static_cast<int>(BgH5000CsvMaker::trackedData::UtcDate)]);
            if (utcdate != 0)
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

    case 9:			// COG
        m_observations[static_cast<int>(BgH5000CsvMaker::trackedData::Cog)] = o.getVal();
        break;

    case 37:		// HDG
        m_observations[static_cast<int>(BgH5000CsvMaker::trackedData::Hdg)] = o.getVal();
        break;

    case 39:		// SET
        m_observations[static_cast<int>(BgH5000CsvMaker::trackedData::TideSet)] = o.getVal();
        break;

    case 40:		// TIDE
        m_observations[static_cast<int>(BgH5000CsvMaker::trackedData::TideRate)] = o.getVal();
        break;

    case 41:		// SOG
        m_observations[static_cast<int>(BgH5000CsvMaker::trackedData::Sog)] = o.getVal();
        break;

    case 42:		// BSPD
        m_observations[static_cast<int>(BgH5000CsvMaker::trackedData::Bsp)] = o.getVal();
        break;

    case 46:		// AWS
        m_observations[static_cast<int>(BgH5000CsvMaker::trackedData::Aws)] = o.getVal();
        break;

    case 47:		// TWS
        m_observations[static_cast<int>(BgH5000CsvMaker::trackedData::Tws)] = o.getVal();
        break;

    case 140:		// AWA
        m_observations[static_cast<int>(BgH5000CsvMaker::trackedData::Awa)] = o.getVal();
        break;

    case 141:		// TWA
        m_observations[static_cast<int>(BgH5000CsvMaker::trackedData::Twa)] = o.getVal();
        break;

    case 142:		// TWD
        m_observations[static_cast<int>(BgH5000CsvMaker::trackedData::Twd)] = o.getVal();
        break;

    case 228:		// TGW Target TWA
        m_observations[static_cast<int>(BgH5000CsvMaker::trackedData::TargetTwa)] = o.getVal();
        break;

    case 234:		// TGS Target Boat Speed
        m_observations[static_cast<int>(BgH5000CsvMaker::trackedData::TargetSpd)] = o.getVal();
        break;

    case 235:		// VMG
        m_observations[static_cast<int>(BgH5000CsvMaker::trackedData::Vmg)] = o.getVal();
        break;

    case 240:		// POL Polar Speed
        m_observations[static_cast<int>(BgH5000CsvMaker::trackedData::PolarBsp)] = o.getVal();
        break;

    case 241:		// POL Polar Performance
        m_observations[static_cast<int>(BgH5000CsvMaker::trackedData::PolarPerf)] = o.getVal();
        break;

    case 243:		// WAM Wind Angle to Mast
        m_observations[static_cast<int>(BgH5000CsvMaker::trackedData::MastWa)] = o.getVal();
        break;

    case 331:		// MWA
        m_observations[static_cast<int>(BgH5000CsvMaker::trackedData::Mwa)] = o.getVal();
        break;

    case 332:		// MWS
        m_observations[static_cast<int>(BgH5000CsvMaker::trackedData::Mws)] = o.getVal();
        break;

    case 383:		// TAC TWA Correction
        m_observations[static_cast<int>(BgH5000CsvMaker::trackedData::TwaCorr)] = o.getVal();
        break;

    case 384:		// TWC TWS Correction
        m_observations[static_cast<int>(BgH5000CsvMaker::trackedData::TwsCorr)] = o.getVal();
        break;

    case 403:		// BSC Boat Speed Correction
        m_observations[static_cast<int>(BgH5000CsvMaker::trackedData::BspCorr)] = o.getVal();
        break;

    case 404:		// CWS CorrectedMWS
        m_observations[static_cast<int>(BgH5000CsvMaker::trackedData::CorrMws)] = o.getVal();
        break;

    case 405:		// CWA CorrectedMWA
        m_observations[static_cast<int>(BgH5000CsvMaker::trackedData::CorrMwa)] = o.getVal();
        break;

    case 406:		// TS OrigTWS
        m_observations[static_cast<int>(BgH5000CsvMaker::trackedData::OrigTws)] = o.getVal();
        break;

    case 407:		// TA OrigTWA
        m_observations[static_cast<int>(BgH5000CsvMaker::trackedData::OrigTwa)] = o.getVal();
        break;

    case 408:		// TD OrigTWD
        m_observations[static_cast<int>(BgH5000CsvMaker::trackedData::OrigTwd)] = o.getVal();
        break;

    case 409:		// MBS MeasuredBSP
        m_observations[static_cast<int>(BgH5000CsvMaker::trackedData::MBsp)] = o.getVal();
        break;

    case 437:		// MeasuredBSPPort
        m_observations[static_cast<int>(BgH5000CsvMaker::trackedData::MBspPrt)] = o.getVal();
        break;

    case 438:		// MeasuredBSPStbd
        m_observations[static_cast<int>(BgH5000CsvMaker::trackedData::MBspStbd)] = o.getVal();
        break;

    case 497:		// SpdThruWater
        m_observations[static_cast<int>(BgH5000CsvMaker::trackedData::SpdThruWater)] = o.getVal();
        break;

        // The following data item id's/values will not be written to the .csv file
    case 1:		// ALT
    case 3:		// EPE
    case 4:		// HDOP
    case 5:		// VDOP
    case 7:		// PDOP
    case 8:		// GSEP
    case 10:	// PQU
    case 11:	// PIN
    case 12:	// SATS
    case 13:	// SDG
    case 27:	// LOG
    case 30:	// TP1
    case 31:	// TP1	/??
    case 44:	// AV1
    case 45:	// MX1
    case 77:	// DEP
    case 121:	// ROT
    case 122:	// TRM
    case 125:	// VAR
    case 146:	// RUD
    case 150:	// CMR
    case 157:	// GPF
    case 165:	// DOF
    case 221:	// CRS
    case 224:	// HOT
    case 226:	// LEE
    case 230:	// TMR
    case 309:	// BOW LAT
    case 310:	// BOW LON
    case 317:	// DRB
    case 318:	// DRD
    case 319:	// DPE
    case 320:	// DSE
    case 321:	// DLI
    case 325:	// HEL
    case 336:	// OWA
    case 340:	// SLP LAT
    case 341:	// SLP LON
    case 352:	// SLS LAT
    case 353:	// SLS LON
    case 354:	// SLB
    case 355:	// TP2
    case 356:	// TP2
    case 357:	// VMG
    case 360:	// AV2
    case 361:	// MX1
    case 380:	// BSL
    case 381:	// SLA
    case 382:	// DLI
    case 385:	// RED
    case 386:	// GRN
    case 387:	// BLU
    case 391:	// PERP
    case 392:	// ARR
    case 398:	// AC1
    case 399:	// AC2
    case 400:	// AC3
    case 401:	// AC4
    case 402:	// DES
    case 410:	// ULO
    case 414:	// BLI
    case 415:	// BLIB
    case 420:	// SLA
    case 421:	// GPS LAT
    case 422:	// GPS LON
    case 423:	// BPE
    case 424:	// BSE
    case 425:	// Opposite Tack COG
    case 426:	// Opposite Tack Target Heading
    case 439:	// ATWD
    case 440:	// WPHS
    case 441:	// LIFT
    case 466:	// LEE
    case 467:	// DPE
    case 468:	// DSE
    case 498:	// APM
    case 499:	// GBA
    case 500:	// TWR
    case 501:	// HCP
    case 502:	// NCR
    case 503:	// PTW
    case 504:	// WHL
    case 505:	// AHL
        break;

    default:
        // Print an alert if an unrecognized data item id is sent by the H5000.
        cerr << "Unhandled value: " << o.getId() << endl;
        break;
    }
}


/**
 * @brief Start a new .csv file and re-initialize the collection of observations.
 *
 * @param utcdate_ An integer representation of the number of days since Jan 0, 1900.
*/
void BgH5000CsvMaker::NewDate(unsigned long int utcdate)
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
    string headings = "UtcDate,UtcTime,Cog,Hdg,Sog,Bsp,MBsp,MBspPrt,SpdThruWater,BspCorr,Mws,Aws,Tws,OrigTws,CorrMws,TwsCorr,Mwa,CorrMwa,Awa,Twa,";
    headings += "OrigTwa,TwaCorr,MastWa,Twd,OrigTwd,TideSet,TideRate,TargetTwa,TargetSpd,PolarBsp,PolarPerf,Vmg\n";
//    string headings = "Boat,Utc,BSP,AWA,AWS,TWA,TWS,TWD,RudderFwd,Leeway,Set,Drift,HDG,AirTemp,SeaTemp,Baro,Depth,Heel,Trim,Rudder,Tab,Forestay,Downhaul,";
//    headings += "MastAng,FstayLen,MastButt,Load S,Load P,Rake,Volts,ROT,GpQual,PDOP,GpsNum,GpsAge,Altitude,GeoSep,GpsMode,Lat,Lon,COG,SOG,DiffStn,Error,";
//    headings += "RunnerS,RunnerP,Vang,Trav,Main,KeelAng,KeelHt,Board,Oil P,RPM 1,RPM 2,Board P"
    fputs(headings.c_str(), m_oFile);

    // Clear the stored observations (and zero the time)
    Clear();
    m_observations[static_cast<int>(BgH5000CsvMaker::trackedData::UtcTime)] = 0.0;
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
void BgH5000CsvMaker::NewTime(unsigned long int utctime)
{
    // Create a string with the values in CSV format
    stringstream ss;
    for (int i = 0; i < static_cast<int>(BgH5000CsvMaker::trackedData::MAX_ITEMS); i++)
    {
        ss << m_observations[i] << ",";
    }
    string s = ss.str();
    s.pop_back();				// Remove trailing space
    s += '\n';


    // Write the line to the output file
    fputs(s.c_str(), m_oFile);
    fflush(m_oFile);

    // Clear the stored observations (except date and time)
    Clear();
    m_observations[static_cast<int>(BgH5000CsvMaker::trackedData::UtcTime)] = utctime;

    // Set raw timestamp value
    m_rawTimestamp = static_cast<uint64_t>(m_observations[static_cast<int>(BgH5000CsvMaker::trackedData::UtcTime)]);
}

/**
 * @brief Clear accumulated observation data from the object.
 *
 * This function is called immediately after a timestamp set of values has been
 * written to the .csv file (so intermittent observations/values are dropped
 * and not carried forward).
*/
void BgH5000CsvMaker::Clear()
{
    // Save the date and time most recently observed
    double date = m_observations[static_cast<int>(BgH5000CsvMaker::trackedData::UtcDate)];
    double time = m_observations[static_cast<int>(BgH5000CsvMaker::trackedData::UtcTime)];

    // Clear the entire vector of data observation values
    m_observations.fill(0.0);

    // Restore the date and time values to initialize the new set of observations
    m_observations[static_cast<int>(BgH5000CsvMaker::trackedData::UtcDate)] = date;
    m_observations[static_cast<int>(BgH5000CsvMaker::trackedData::UtcTime)] = time;
}

/**
 * @brief Construct the .csv file name, including the date of the observations.
 *
 * @param utcdate_ An integer representing the number of days since Jan 0, 1900.
 * @return A string with the new file name.
*/
string BgH5000CsvMaker::MakeFileName(unsigned long int utcdate)
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
