/**
    h5000-logger-class.h

    The main application class

    Copyright (c) 2020 D. Scott Miller. All rights reserved.
    This code is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License as published by the Free
    Software Foundation; either version 2 of the License, or (at your option)
    any later version.
*/

#ifndef __H5000_LOGGER_CLASS_H
#define __H5000_LOGGER_CLASS_H

#include "bg-observation-class.h"
#include "bg-websocket-session-class.h"
#include "bg-csv-output-class.h"
#include "bg-flat-output-class.h"
#include "json/json.h"
#include <string>
#include <vector>

using namespace std;

// Output progress to console only if in test mode
#define DEBUGOUT(x) if (m_testFlag || m_debugFlag) cout << x << endl;

class H5000Logger {

    friend class BgWebsocketSession;

public:
    H5000Logger(int argc_, char** argv_);
    int run();

protected:
    void RequestAllValues();

private:
    // Main communication/logging routines
    void RequestDataValues(vector<int> items);
    void RequestDataInfo(vector<int> items);
    void handleData(Json::Value& root);
    void handleResponse(Json::Value& root);
    void handleResponse(string const& s);
    void handleMany(Json::Value& root);

    // Log output functions
    void WriteFlatLog(BgObservation& o);
    void WriteCsv(BgObservation& o);

    // Helper functions
    void ProcessCommandLine(int argc, char** argv);
    void AddDataItemArray(vector<int> array, string& str);
    void AddIntArray(vector<int> array, string& str);
    void CloseLogFiles();
    Json::Value ConstructJson(string sJson);


private:
    // command line flags
    bool m_hostFlag;            // host ip specified?
    bool m_portFlag;            // communication port specified?
    bool m_outDirFlag;          // output directory specified?
    bool m_debugFlag;           // debug (console) output?
    bool m_inputLogFlag;        // process "flatfile" input?
    bool m_testFlag;            // just test connection?
    bool m_csvFlag;             // produce CSV file output?
    bool m_flatFlag;            // produce "flatfile" output?

    // corresponding argument values
    string m_host;
    string m_port;
    string m_outDir;
    string m_inputLogFile;

    BgCsvOutput* m_csvFile;
    BgFlatOutput* m_flatFile;
    FILE* m_fpLog;											// pointer to the .log output file (if any)

    // Core actions
    void ProcessObservation(BgObservation& o);
    void ProcessObservation(const char* buffer);

    // Helper functions
    void NewDate(unsigned long int utcdate);
    void Clear();
    void NewTime(unsigned long int utctime);
    string MakeFileName(unsigned long int utcdate);
    Json::Value ReadJsonFile(const char* fname);

public:
    uint64_t m_rawTimestamp = 0;
    tm m_tmTimestamp = { 0,0,0,0 };
    string m_date;
    string m_time;
    array<double, 100> m_observations;
    FILE* m_fout;
    shared_ptr<BgWebsocketSession> m_session;

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

#endif	// __H5000_LOGGER_CLASS_H
