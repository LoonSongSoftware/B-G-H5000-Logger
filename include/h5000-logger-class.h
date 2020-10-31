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
#include "bg-csv-writer-class.h"
#include "bg-flat-writer-class.h"
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
    bool AnalyzeDataDefs();
    bool LoadDataDefs();
    void handleMany(Json::Value& root);

    // Helper functions
    void ProcessCommandLine(int argc, char** argv);
    void AddDataItemArray(vector<int> array, string& str);
    void AddIntArray(vector<int> array, string& str);
    Json::Value ConstructJson(string sJson);
    void Usage();

    int ProcessFlatLog();


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
    string m_exePath;

    BgCsvWriter* m_csvWriter;
    BgFlatWriter* m_flatWriter;

    // Core actions
    void ProcessObservation(BgObservation& o);
    bool WriteToCsvTest(Json::Value& item);

    // Helper functions
    void NewDate(unsigned long int utcdate);
    void Clear();
    void NewTime(unsigned long int utctime);

public:
    shared_ptr<BgWebsocketSession> m_session;





    map<int, int> m_bgToCsvMap;     // Map for translating B&G DataItem IDs to Expedition CSV columns
    vector<double> m_observations;  // Vector of observations for this timestamp (overwritten for each new time)
    map<string, int> m_ExpNameToCsvCol; // Map to translate an Expedition column name to the corresponding column number
    map<int, string> m_CsvColtoExpName; // Map to translate a column number to the corresponding Expedition column name
    map<int, string> m_BgIDToBgName;    // Map to translate an ID value to the corresponding B&G DataItem name
    vector<bool> m_obsSeen;         // Elements are set to True when the corresponding observation has been seen
    uint64_t m_rawTimestamp = 0;
    Json::Value m_bgDataDefs;
    vector<string> m_csvTrackedItems;
    vector<unsigned char> m_precisions; // Vector with precision (after decimal point) for each data item
    map<string, int> m_BgNameToBgID;    // Map to translate a B&G DataItem name to its corrsponding ID value

    FILE* m_iFile;
};

#endif	// __H5000_LOGGER_CLASS_H
