/**
    bg-flat-output-class.h

    A class which writes 'data item' observations received from an H5000 CPU
    to a "flat" log file.

    See: "GoFree Web Interface" / "Navico GoFree Tier 2 Websocket" documents for
    more detail (available for download from the B&G website) about data items.

    Copyright (c) 2020 D. Scott Miller. All rights reserved.
    This code is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License as published by the Free
    Software Foundation; either version 2 of the License, or (at your option)
    any later version.
*/

#ifndef __BG_FLAT_WRITER_H
#define __BG_FLAT_WRITER_H

#include <bg-observation-class.h>

using namespace std;

class BgFlatWriter
{
public:
    BgFlatWriter(string& m_outDir);
    void ProcessObservation(BgObservation& o);

private:
    void NewDate(unsigned long int utcdate);
    string MakeFileName(unsigned long int utcdate);

    FILE* m_oFile;
    string m_outDir;

};

#endif  // __BG_FLAT_WRITER_H
