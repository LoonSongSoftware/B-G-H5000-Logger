/**
    bg-observation-class.cpp

    A class which holds a single 'data item' observation received from an 
    H5000 CPU.

    See "GoFree Web Interface" / "Navico GoFree Tier 2 Websocket" documents for
    more detail (available for download from the B&G website) about data items.

    Copyright (c) 2020 D. Scott Miller. All rights reserved.
    This code is free software; you can redistribute it and/or modify it under 
    the terms of the GNU General Public License as published by the Free 
    Software Foundation; either version 2 of the License, or (at your option) 
    any later version.
*/

#include "bg-observation-class.h"
#include <iomanip>

/**
 * @brief Constructor (creates an Observation object from a Json::Value object).

 * The B&G websocket server provides messages with one or more observations
 * combined in a JSON object. This function takes the JSON associated with a 
 * single observation and creates an Observation object from it.
 * 
 * @param data A Json::Value object with the observation data (in B&G websocket format).
*/
BgObservation::BgObservation(Json::Value data) :
    m_val(0), m_sysVal(0), m_inst(0), m_damped(false), m_dampedVal(0)
{
    // 'id' and 'valid' are always present
    m_id = data["id"].asInt();
    m_valid = data["valid"].asBool();

    // Other members may or may not be present in the JSON
    if (data.isMember("val"))
        m_val = data["val"].asDouble();
    if (data.isMember("sysVal"))
        m_sysVal = data["sysVal"].asDouble();
    if (data.isMember("dampedVal"))
        m_dampedVal = data["dampedVal"].asDouble();
    if (data.isMember("valStr"))
        m_valStr = data["valStr"].asString();
    if (data.isMember("inst"))
        m_inst = data["inst"].asInt();
    if (data.isMember("damped"))
        m_damped = data["damped"].asBool();
}

/**
 * @brief Constructor (creates an Observation object from a string object).

 * When the application is used off the boat (away from a live websocket
 * session), the observation can be constructed from a line in the flatlog
 * file.  This function takes the string associated with a single observation/
 * single line in the file and creates an Observation object from it.
 *
 * @param data A string with the observation data (in flatlog format).
*/
BgObservation::BgObservation(string data) :
    m_val(0), m_sysVal(0), m_inst(0), m_damped(false), m_dampedVal(0)
{

    size_t idx;
    size_t start = 0;
    m_id = stoul(data,&idx);
    start += idx + 1;
    m_valid = (stoul(data.substr(start), &idx) == 1);
    start += idx + 1;
    m_val = stod(data.substr(start), &idx);
    start += idx + 1;
    m_sysVal = stod(data.substr(start), &idx);
    start += idx + 1;
    m_dampedVal = stod(data.substr(start), &idx);
    start += idx + 1;
    m_inst = stoul(data.substr(start), &idx);
    start += idx + 1;
    m_damped = (stoul(data.substr(start), &idx) == 1);
}

/**
 * @brief Accessor for the 'id' value.
 * @return The data id for the observation.
*/
int BgObservation::getId() {
    return m_id;
}

/**
 * @brief  Accessor for the 'valid' value.
 * @return True if the data is valid.
*/
bool BgObservation::isValid() {
    return m_valid;
}

/**
 * @brief Return the string representation of the value.
 * @return A string representing the value.
*/
string BgObservation::strVal()
{
    return m_valStr;
}

/**
 * @brief Accessor for the 'value' value.
 * @return A double containing the value.
*/
double BgObservation::getVal()
{
    return m_val;
}

/**
 * @brief Get a string representation of the Observation data members.
 * @return A string with the comma-separated data values.
*/
string BgObservation::str() {
    stringstream s;
    s << m_id << "," << m_valid << "," << m_inst << "," << m_damped << "," << setprecision(10) << m_val << "," 
        << m_sysVal << "," << m_dampedVal << endl;
    return s.str();
}
