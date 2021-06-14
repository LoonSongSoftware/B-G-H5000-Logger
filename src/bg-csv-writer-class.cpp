


#include "bg-csv-writer-class.h"
#include "bg-observation-class.h"
#include <ctime>
#include <fstream>
#include <iomanip>

BgCsvWriter::BgCsvWriter(string& outDir):
    m_outDir(outDir), m_oFile(NULL)
{
std::cout<<"BgCsvWriter constructor"<<std::endl;
}

void BgCsvWriter::WriteObservations(vector<double>& observations, vector<unsigned char>& precisions, vector<bool>& seen)
{
    // Only write observations if a file is already open
    if (m_oFile)
    {
        // Create a string with the values in CSV format
        stringstream ss;
        ss << fixed;
        vector<double>::const_iterator it;
        vector<unsigned char>::const_iterator jt;
        vector<bool>::const_iterator kt;
        for (it = observations.cbegin(), jt = precisions.cbegin(), kt = seen.cbegin();
            it != observations.cend(); it++, jt++, kt++)
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
    }
}


void BgCsvWriter::NewFile(unsigned long utcdate, vector<string>& trackedItems)
{
    // Close existing file (if any)
    if (m_oFile)
        fclose(m_oFile);

    // Open a new .csv output file for observations from this new date
    m_oFile = fopen(MakeFileName(utcdate).c_str(), "a");

    // Write a line of column headings
    string headings = MakeHeaderString(trackedItems) + '\n';
    fputs(headings.c_str(), m_oFile);
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
/*void BgCsvWriter::ProcessObservation(BgObservation& o) {

}
*/

/**
 * @brief Construct the .csv file name, including the date of the observations.
 *
 * @param utcdate_ An integer representing the number of days since Jan 0, 1900.
 * @return A string with the new file name.
*/
string BgCsvWriter::MakeFileName(unsigned long int utcdate)
{
    // Convert the Excel-format date to a tm struct (in UTC time)
    double dateExcel = utcdate;
    time_t tsUnix = (time_t)(dateExcel - 25569) * 86400;
    struct tm* dateTm = gmtime(&tsUnix);

    // Create the filename buffer, including the date
    char buffer[100];
    strftime(buffer, 100, "%Y%m%d-h5000-cpu-data.csv", dateTm);

    string filename = m_outDir + string(buffer);
    return filename;
}

string BgCsvWriter::MakeHeaderString(vector<string>& trackedItems)
{
    // Iterate through the array to construct the header
    string headerStr;
    for (vector<string>::const_iterator it = trackedItems.cbegin(); it != trackedItems.cend(); it++)
    {
        headerStr += *it;
        headerStr += ',';
    }

    // Drop the trailing comma and return the string
    headerStr = headerStr.substr(0, headerStr.size() - 2);
    return headerStr;

}
