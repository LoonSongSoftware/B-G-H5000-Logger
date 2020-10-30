/**
    h5000-logger-class.cpp

    The main application object (instantiated and called from main()).

    Copyright (c) 2020 D. Scott Miller. All rights reserved.
    This code is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License as published by the Free
    Software Foundation; either version 2 of the License, or (at your option)
    any later version.
*/

#include "h5000-logger-class.h"
#include "getopt/getopt.h"
#include <fstream>

/// ///////////////////////////////////////////////////////////////////////////
// Public Member Functions
/// ///////////////////////////////////////////////////////////////////////////

#if (true)

/**
 * @brief Constructor.
 * 
 * Save command line arguments in class member variables.
 * 
 * @param argc Number of arguments supplied on the command line.
 * @param argv Arguments supplied on the command line.
*/
H5000Logger::H5000Logger(int argc, char** argv) : 
    m_csvFile(NULL), m_flatFile(NULL), m_fpLog(NULL), m_testFlag(false), m_fout(NULL)
{
    // Check command line arguments and populate member variables.
    ProcessCommandLine(argc, argv);
}

/**
 * @brief Start execution of the logger program.
 * 
 * The logger will attempt to connect to the specified webserver and, if
 * successful, will canvass the server for certain data item values and
 * save the resulting values in log files.
 * 
 * @return ERROR_SUCCESS if successful; otherwise, error code.
*/
int H5000Logger::run()
{

    // Create and initialize the CSV output class (if desired)
    if (m_csvFlag)
    {
        m_csvFile = new BgCsvOutput(m_outDir, m_inputLogFile);
        m_csvFile->LoadDataDefs();

        // Special handling if the input should come from a flat log file,
        // instead of live websocket data
        if (m_inputLogFlag)
        {
            int nRet = m_csvFile->ProcessFlatLog();

            // Exit after processing the flat log file
            delete m_csvFile;
            exit(nRet);
        }
    }

    // Create and initialize the FLAT output class (if desired)
    if (m_flatFlag)
    {
        m_flatFile = new BgFlatOutput(m_outDir);
    }

    // The io_context is required for all I/O
    net::io_context ioc;
    m_session = make_shared<BgWebsocketSession>(this, ioc, m_testFlag);
    m_session ->run(m_host.c_str(), m_port.c_str());

    // Run the I/O service. The call will return when the socket is closed.
    ioc.run();

    return EXIT_SUCCESS;
}

#endif


/// ///////////////////////////////////////////////////////////////////////////
// Outgoing websocket message constructors
/// ///////////////////////////////////////////////////////////////////////////

#if (true)

/**
 * @brief Request all (relevant) values from the H5000 CPU.
 *
 * This function is called when the websocket handshake is successfully 
 * completed to start the stream of data values coming from the websocket 
 * server.
 *
 * @see on_handshake()
*/
void H5000Logger::RequestAllValues() {

    // todo Remove hardwired value ids and replace with those "advertised" by the CPU.
    // todo Periodically request a list of "advertised" value from the CPU (e.g., if new devices have appeared)

    DEBUGOUT("Requesting dataitem info and values.")

    vector<int> dataitems = { 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 17, 18, 19, 21, 23, 27, 30, 31, 32, 33, 34, 35, 37, 39, 40, 41, 42, 44, 45, 46 };
    RequestDataInfo(dataitems);
    if (m_testFlag)     // Just ask for one "batch" of values if in test mode
        return;
    RequestDataValues(dataitems);
    
    dataitems = { 47, 48, 49, 65, 66, 77, 118, 119, 121, 122, 125, 140, 141, 142, 146, 150, 157, 165, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 230, 231, 232 };
    RequestDataInfo(dataitems);
    RequestDataValues(dataitems);
    dataitems = { 233, 234, 235, 236, 238, 240, 241, 243, 304, 305, 306, 309, 310, 311, 312, 313, 314, 315, 316, 317, 318, 319, 320, 321, 322, 323, 324, 325, 326, 327 };
    RequestDataInfo(dataitems);
    RequestDataValues(dataitems);
    dataitems = { 328, 329, 330, 331, 332, 333, 334, 335, 336, 337, 338, 339, 340, 341, 342, 343, 344, 345, 346, 347, 348, 349, 350, 351, 352, 353, 354, 355, 356, 357 };
    RequestDataInfo(dataitems);
    RequestDataValues(dataitems);
    dataitems = { 358, 359, 360, 361, 362, 363, 364, 365, 366, 367, 368, 369, 370, 371, 372, 373, 374, 375, 376, 377, 378, 379, 380, 381, 382, 383, 384, 385, 386, 387, 388 };
    RequestDataInfo(dataitems);
    RequestDataValues(dataitems);
    dataitems = { 389, 390, 391, 392, 393, 394, 395, 396, 397, 398, 399, 400, 401, 402, 403, 404, 405, 406, 407, 408, 409, 410, 411, 412, 413, 414, 415, 416, 417, 418, 419 };
    RequestDataInfo(dataitems);
    RequestDataValues(dataitems);
    dataitems = { 420, 421, 422, 423, 424, 425, 426, 427, 428, 429, 432, 433, 434, 435, 436, 437, 438, 439, 440, 441, 442, 443, 444, 445, 446, 447, 448, 449, 450, 451, 452, 453 };
    RequestDataInfo(dataitems);
    RequestDataValues(dataitems);
    dataitems = { 454, 455, 456, 457, 458, 459, 460, 461, 462, 463, 464, 465, 466, 467, 468, 469, 470, 471, 472, 473, 474, 475, 476, 477, 478, 479, 480, 481, 482, 483, 484, 485 };
    RequestDataInfo(dataitems);
    RequestDataValues(dataitems);
    dataitems = { 486, 487, 488, 489, 490, 491, 492, 493, 494, 495, 496, 497, 498, 499, 500, 501, 502, 503, 504, 505, 506, 507, 508, 509, 510, 511, 512, 513, 514, 515 };
    RequestDataInfo(dataitems);
    RequestDataValues(dataitems);
}

/**
 * @brief Construct and enqueue a "DataReq" message.
 *
 * The B&G websocket interface provides a "DataReq" message, to query the
 * values and state of one or more data values. This function constructs the
 * outgoing message string, then enqueues it to be sent to the websocket
 * server.
 *
 * The constructed message requests updates of each value ("repeat: true")
 * and queries only instance=0.
 *
 * @param items_ A vector of integers, each representing a data id to be 
 *      queried.
*/
void H5000Logger::RequestDataValues(vector<int> items_) {

    // Construct the outgoing message string
    string sQuery = "{\"DataReq\":[";
    AddDataItemArray(items_, sQuery);
    sQuery += "]}";
    m_session->send(make_shared<string>(sQuery));
}

/**
 * @brief Construct and enqueue a "DataInfoReq" message.
 *
 * The B&G websocket interface provides a "DataInfoReq" message to get static
 * information about one or more data values (long name, short name, units,
 * etc.).
 *
 * This function constructs a JSON string representing such a message for each
 * data id value in the items_ parameter vector and enqueues it to be sent to
 * the websocket server.
 *
 * @param items_ An array of integers representing the data ids of values for
 * which the information is requested.
*/
void H5000Logger::RequestDataInfo(vector<int> items_) {

    // Construct the outgoing message string
    string sQuery = "{\"DataInfoReq\":[";
    AddIntArray(items_, sQuery);
    sQuery += "]}";
    m_session->send(make_shared<string>(sQuery));
}

#endif

/// //////////////////////////////////////////////////////////////
// Incoming websocket message handlers
/// //////////////////////////////////////////////////////////////

#if (true)
/**
 * @brief Handles an incoming message by converting it to JSON, then calling the handler.
 *
 * @param s A incoming message string.
*/
void H5000Logger::handleResponse(string const& s)
{
    // If we're in "test" mode, we're done--the response has already been displayed
    if (m_testFlag) {
        DEBUGOUT("Test complete.")
        exit(0);
    }

    Json::Value root = ConstructJson(s);
    handleResponse(root);
}

/**
 * @brief Process an incoming JSON message.
 *
 * At this point, the program only "handles" "Data" and "Many" messages,
 * although the B&G websocket server can send many other types.
 *
 * Based on the type of message, a handleXXX function is called to process the
 * specific message type.
 *
 * @param root_ A JSON value representing the message to be handled.
 * @see handleResponse(string&)
*/
void H5000Logger::handleResponse(Json::Value& root_)
{
    if (root_.isMember("Data") != Json::nullValue) {
        handleData(root_["Data"]);
    }
    if (root_.isMember("Many") != Json::nullValue) {
        handleData(root_["Many"]);
    }

}

/**
 * @brief Handle a "Data" JSON message.
 *
 * This function creates an Observation object for each data id in the message
 * (there can be more than one) then, depending on the log output options
 * specified when the BgWebsocketSession 'run' function was called, will add
 * the observation to the log output stream.
 *
 * @param root_ The root of the "Data" JSON message object.
*/
void H5000Logger::handleData(Json::Value& root_)
{
    for (Json::Value::ArrayIndex idx = 0; idx != root_.size(); idx++) {

        // Construct a BgObservation object from the JSON data
        BgObservation o(root_[idx]);

        // Process the data if the value is valid
        if (o.isValid())
        {
            ProcessObservation(o);
/*            if (m_flatFile != NULL) {
                m_flatFile->ProcessObservation(o);
            }
            if (m_csvFile != NULL) {
                m_csvFile->ProcessObservation(o);
            }*/
        }
    }
}

void H5000Logger::ProcessObservation(BgObservation& o)
{

}

/**
 * @brief Handle an incoming "Many" message.
 *
 * The B&G websocket server can encapsulate many messages into one string. This
 * function processes such a "Many" message by accessing each of the
 * encapsulated messages and separately handling each of them.
 *
 * @param root_ The root of the JSON object representing the message.
*/
void H5000Logger::handleMany(Json::Value& root_)
{
    // root_ is an array of instructions
    for (Json::Value::ArrayIndex idx = 0; idx != root_.size(); idx++) {
        handleResponse(root_[idx]);
    }
}

#endif

/// //////////////////////////////////////////////////////////////
// Helper routines
/// //////////////////////////////////////////////////////////////

#if (true)

void H5000Logger::ProcessCommandLine(int argc, char** argv)
{

    char opts[] = "h:p:o:dl:tcf";
    int opt;
    while ((opt = getopt(argc, argv, opts)) != -1)
    {
        switch (opt)
        {
        case 'h':     // specify host ip (required for live logging)
            m_host = optarg;
            m_hostFlag = true;
            break;
        case 'p':
            m_port = optarg;
            m_portFlag = true;
            break;
        case 'o':
            m_outDir = optarg;
            m_outDirFlag = true;
            break;
        case 'd':
            m_debugFlag = true;
            break;
        case 'l':
            m_inputLogFile = optarg;
            m_inputLogFlag = true;
            break;
        case 't':
            m_testFlag = true;
            break;
        case 'c':
            m_csvFlag = true;
            break;
        case 'f':
            m_flatFlag = true;
            break;
        default:
            std::cerr <<
              "\nUsage: h5000-logger <host> <port>\n" <<
              "Example:\n" <<
              "    h5000-logger 192.168.0.2 2053\n\n";
            exit(-1);
        }
    }
}

/**
 * @brief Add an array of integers to a string that will become a portion of a DataItem request message.
 *
 * The B&G websocket interface accepts DataItem messages which can specify more
 * than one data id to be returned. Since this application requests lots of
 * data ids, this helper function is intended to help construct the JSON string
 * to request each data id.
 *
 * At this point, the constructed JSON query will request repeated updates of
 * each value and only anticipates values with instance = 0.
 *
 * @param array An array of ints representing the data ids to be queried.
 * @param str A JSON string into which the generated JSON text is to be inserted.
*/
void H5000Logger::AddDataItemArray(vector<int> array, string& str) {

    // iterate through value ids, inserting them into the target string
    for (vector<int>::const_iterator it = array.begin(); it != array.end(); it++) {
        str += "{\"id\":";
        str += to_string((*it));
        str += ",\"repeat\":true,\"inst\":0";
        str += "}";
        str += ",";
    }

    // get rid of trailing comma to maintain valid JSON
    if (!str.empty())
        str.pop_back();
}

/**
 * @brief Add an array of ints to a string, comma-separated.
 *
 * The B&G websocket interface's DataInfo message expects a comma-separated
 * array of integers representing the ids of the data items for which the
 * information is requested. This function creates that string from an array
 * of int's.
 *
 * @param array Array of ints (representing the data ids to be queried).
 * @param str A JSON string into which the generated text is to be inserted.
*/
void H5000Logger::AddIntArray(vector<int> array, string& str) {

    // iterate through value ids, inserting them into the target string
    for (vector<int>::const_iterator it = array.begin(); it != array.end(); it++) {
        str += to_string(*it);
        str += ",";
    }
    if (!str.empty())
        str.pop_back();
}

/**
 * @brief Close the output log files (if they're open).
*/
void H5000Logger::CloseLogFiles() {
    if (m_fpLog != NULL)
    {
        fclose(m_fpLog);
    }
}

/**
 * @brief Create a JSON value from a JSON-formatted string.
 *
 * @param sJson A string with valid JSON
 * @return The new JSON value.
*/
Json::Value H5000Logger::ConstructJson(string sJson)
{
    Json::CharReaderBuilder builder;
    Json::CharReader* reader = builder.newCharReader();

    Json::Value json;
    string errors;
    if (!reader->parse(sJson.c_str(), sJson.c_str() + sJson.size(), &json, &errors))
    {
        cerr << "Failed to parse the following string:" << endl << sJson << endl;
        cerr << "Error(s): " << errors << endl;
        return Json::nullValue;
    }
    return json;
}

#endif
