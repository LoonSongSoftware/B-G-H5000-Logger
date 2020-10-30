


#include "bg-flat-output-class.h"
#include "bg-observation-class.h"
#include <ctime>


BgFlatOutput::BgFlatOutput(string& outDir):
    m_outDir(outDir)
{
}

/**
 * @brief Write an observation to the flat log.
 *
 * @param o A BgObservation object with the new data.
*/
void BgFlatOutput::ProcessObservation(BgObservation& o) 
{
    // If the flat log has not been opened, open it.
    if (m_oFile == NULL)
    {
        string filename = "flatlog.log";
        m_oFile = fopen(filename.c_str(), "a");
    }

    // Write the string representation of the observation to the file
    fputs(o.str().c_str(), m_oFile);

    // Since we may not exit the application gracefully, flush the write buffer 
    // every change in timestamp
    if (o.getId() == 35)
        fflush(m_oFile);
}


/**
 * @brief Start a new .log file.
 *
 * @param utcdate_ An integer representation of the number of days since Jan 0, 1900.
*/
void BgFlatOutput::NewDate(unsigned long int utcdate)
{
    if (m_oFile != NULL) {
        fclose(m_oFile);
    }

    // Open a new .log output file for observations from this new date
    m_oFile = fopen(MakeFileName(utcdate).c_str(), "w");
}


/**
 * @brief Construct the .csv file name, including the date of the observations.
 *
 * @param utcdate_ An integer representing the number of days since Jan 0, 1900.
 * @return A string with the new file name.
*/
string BgFlatOutput::MakeFileName(unsigned long int utcdate)
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
