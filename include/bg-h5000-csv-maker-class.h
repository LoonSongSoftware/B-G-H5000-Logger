




#pragma once
#include <bg-observation-class.h>
#include "json/json.h"

#define MAX_CSV_COLUMNS 500
using namespace std;

class BgH5000CsvMaker
{
public:
    BgH5000CsvMaker(int argc, char** argv);
    int run();
private:
    void ProcessObservation(BgObservation& o);
    void NewDate(unsigned long int utcdate);
    void NewTime(unsigned long int utctime);
    void Clear();
    bool AnalyzeDataDefs();
    string MakeFileName(unsigned long int utcdate);
    string MakeHeaderString();

private:
    string m_inputFile;
    string m_outputFile;
    FILE* m_iFile;
    FILE* m_oFile;

    uint64_t m_rawTimestamp = 0;
    tm m_tmTimestamp = { 0,0,0,0 };
    Json::Value m_bgDataDefs;

    vector<double> m_observations;  // Vector of observations for this timestamp (overwritten for each new time)
    map<int, int> m_bgToCsvMap;     // Map for translating B&G DataItem IDs to Expedition CSV columns
    map<string, int> m_ExpNameToCsvCol; // Map to translate an Expedition column name to the corresponding column number
    map<string, int> m_BgNameToBgID;    // Map to translate a B&G DataItem name to its corrsponding ID value
    map<int, string> m_CsvColtoExpName; // Map to translate a column number to the corresponding Expedition column name
    map<int, string> m_BgIDToBgName;    // Map to translate an ID value to the corresponding B&G DataItem name
    vector<bool> m_obsSeen;         // Elements are set to True when the corresponding observation has been seen
    vector<unsigned char> m_precisions; // Vector with precision (after decimal point) for each data item

    vector<string> m_csvTrackedItems;

    enum class ztrackedData {
        // Date/time
        UtcDate, UtcTime,

        // Course
        Cog, Hdg,

        // Boat Speed
        Sog, Bsp, MBsp, MBspPrt, MBspStbd, SpdThruWater, BspCorr,

        // Wind Speed
        Mws, Aws, Tws, OrigTws, CorrMws, TwsCorr,

        // Wind Angle
        Mwa, CorrMwa, Awa, Twa, OrigTwa, TwaCorr, MastWa,

        // Wind Direction
        Twd, OrigTwd,

        // Tidal/current flow
        TideSet, TideRate,

        // Performance
        TargetTwa, TargetSpd, PolarBsp, PolarPerf, Vmg,

        // END
        MAX_ITEMS
    };


};

