




#pragma once
#include <bg-observation-class.h>
#include "json/json.h"

using namespace std;

class BgH5000CsvMaker
{
public:
    BgH5000CsvMaker(int argc, char** argv);
    int run();
    void ProcessObservation(BgObservation& o);

    void NewDate(unsigned long int utcdate);

    void NewTime(unsigned long int utctime);

    void Clear();

    string MakeFileName(unsigned long int utcdate);

private:
    string m_inputFile;
    string m_outputFile;
    FILE* m_iFile;
    FILE* m_oFile;

    uint64_t m_rawTimestamp = 0;
    tm m_tmTimestamp = { 0,0,0,0 };
    Json::Value m_bgDataDefs;

    array<double, 100> m_observations;

    enum class trackedData {
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

