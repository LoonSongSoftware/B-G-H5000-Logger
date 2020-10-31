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

#ifndef __BG_CSV_WRITER_H
#define __BG_CSV_WRITER_H

#include <bg-observation-class.h>
#include "json/json.h"

#define MAX_CSV_COLUMNS 500
using namespace std;

class BgCsvWriter
{
public:
    BgCsvWriter(string& outDir);
    void WriteObservations(vector<double>& observations, vector<unsigned char>& precisions, vector<bool>& seen);
    void NewFile(unsigned long utcdate, vector<string>& trackedItems);
private:
    string MakeFileName(unsigned long int utcdate);

private:
    string MakeHeaderString(vector<string>& trackedItems);
    string m_outputFile;
    string m_outDir;
    FILE* m_oFile;
};

#endif  // __BG_CSV_WRITER_H
