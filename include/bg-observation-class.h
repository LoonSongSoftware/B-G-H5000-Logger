/**
    bg-observation-class.h

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

#ifndef __BG_OBSERVATION_H
#define __BG_OBSERVATION_H

#include <iostream>
#include <json/json.h>

using namespace std;

class BgObservation {

public:
    // Member functions
    BgObservation(Json::Value data);	// Constructor (from JSON received from the H5000)
    BgObservation(string data);         // Constructor (from string in flat log file)
    
    int getId();					// Accessor for the observation's id
    bool isValid();					// Accessor for the 'valid' member variable
    string strVal();				// Accessor for the 'valStr' member variable
    double getVal();				// Accessor for the 'val' member variable

    string str();					// Provide a string representation of the observation
                                    // (this is what will be written to the flat log file)

private:
    // Member variables
    int m_id;				// id of the data item
    double m_val;			// current value
    string m_valStr;		// value represented as a string
    double m_sysVal;		// system value
    int m_inst;			// instance
    bool m_valid;			// true if the data is valid
    bool m_damped;		// true if the data is damped
    double m_dampedVal;	// damped valued
};

#endif	// __BG_OBSERVATION_H
