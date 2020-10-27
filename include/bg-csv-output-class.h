/**
    bg-csv-output-class.h

    A class which holds a single 'data item' observation received from an
    H5000 CPU.

    See: "GoFree Web Interface" / "Navico GoFree Tier 2 Websocket" documents for
    more detail (available for download from the B&G website) about data items.

    Copyright (c) 2020 D. Scott Miller. All rights reserved.
    This code is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License as published by the Free
    Software Foundation; either version 2 of the License, or (at your option)
    any later version.
*/

#ifndef __BG_CSV_OUTPUT_H
#define __BG_CSV_OUTPUT_H

#include <bg-observation-class.h>
#include "json/json.h"

#define MAX_CSV_COLUMNS 500
using namespace std;

class BgCsvOutput
{
public:
    BgCsvOutput(int argc, char** argv);
    int run();
    bool LoadDataDefs();
    void ProcessObservation(BgObservation& o);

private:
    void NewDate(unsigned long int utcdate);
    void NewTime(unsigned long int utctime);
    void Clear();
    bool AnalyzeDataDefs();
    string MakeFileName(unsigned long int utcdate);
    string MakeHeaderString();

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
};

#endif  // BG_CSV_MAKER
