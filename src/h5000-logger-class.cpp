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
#include <fstream>

/**
 * @brief Constructor.
 * 
 * Save command line arguments in class member variables.
 * 
 * @param argc Number of arguments supplied on the command line.
 * @param argv Arguments supplied on the command line.
*/
H5000Logger::H5000Logger(int argc, char** argv) : m_fpLog(NULL), m_testMode(false), m_fout(NULL)
{
	// Check command line arguments.
	if (argc < 3)
	{
		std::cerr <<
			"\nUsage: h5000-logger <host> <port>\n" <<
			"Example:\n" <<
			"    h5000-logger 192.168.0.2 2053\n\n";
		return;
	}

#   ifdef TEST
    m_testMode = true;
#   endif

	m_host = argv[1];
	m_port = argv[2];
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
	// The io_context is required for all I/O
	net::io_context ioc;

	// Launch the asynchronous operation
	// todo interpret command line output options
    m_session = make_shared<BgWebsocketSession>(this, ioc, m_testMode);
    m_session ->run(m_host.c_str(), m_port.c_str());

	// Run the I/O service. The call will return when the socket is closed.
	ioc.run();

	return EXIT_SUCCESS;
}


/////////////////////////////////////////////////////////////////
// Outgoing websocket message constructors
/////////////////////////////////////////////////////////////////

/**
 * @brief Request all (relevant) values from the H5000 CPU.
 *
 * This function is called when the websocket handshake is successfully completed to start
 * the stream of data values coming from the websocket server.
 *
 * @see on_handshake()
*/
void H5000Logger::RequestAllValues() {

    TESTOUT("Requesting dataitem values.")

    vector<int> dataitems = { 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 17, 18, 19, 21, 23, 27, 30, 31, 32, 33, 34, 35, 37, 39, 40, 41, 42, 44, 45, 46 };
    RequestDataInfo(dataitems);
    if (m_testMode)     // Just ask for one "batch" of values if in test mode
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
 * @param items_ A vector of integers, each representing a data id to be queried.
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


/// //////////////////////////////////////////////////////////////
// Incoming websocket message handlers
/// //////////////////////////////////////////////////////////////

/**
 * @brief Handles an incoming message by converting it to JSON, then calling the handler.
 *
 * @param s A incoming message string.
*/
void H5000Logger::handleResponse(string const& s)
{
    // If we're in "test" mode, we're done--the response has already been displayed
    if (m_testMode) {
        TESTOUT("Test complete.")
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
        BgObservation o(root_[idx]);
        if (o.isValid())
        {
            if (m_logFile) {
                WriteFlatLog(o);
            }
            if (m_csvFile) {
                WriteCsv(o);
            }
        }
    }
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


/// //////////////////////////////////////////////////////////////
// Log output functions
/// //////////////////////////////////////////////////////////////

/**
 * @brief Write an observation to the "flat" output log.
 *
 * @param o The observation to be written.
*/
void H5000Logger::WriteFlatLog(BgObservation& o) {

    // If the flat log has not been opened, open it.
    if (m_fpLog == NULL)
    {
        string filename = "flatlog.log";
        m_fpLog = fopen(filename.c_str(), "w");
    }

    // Write the string representation of the observation to the file
    fputs(o.str().c_str(), m_fpLog);

    // Since we may not exit the application gracefully, flush the write buffer 
    // every change in timestamp
    if (o.getId() == 35)
        fflush(m_fpLog);
}

/**
 * @brief Add an observation to the timestamp-based output log.
 *
 * @param o The observation to be added.
*/
void H5000Logger::WriteCsv(BgObservation& o) {
    ProcessObservation(o);
}


/// //////////////////////////////////////////////////////////////
// Helper routines
/// //////////////////////////////////////////////////////////////

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


/**
 * @brief Identify the type of an incoming observation and handle it accordingly.
 *
 * After a BgObservation object is created from an incoming JSON message, this
 * function "handles" it.
 *
 * The key elements are UtcData (id=34) and UtcTime (id=35), which trigger new
 * output files (in the case of UtcData) and a new collection of observations
 * (in the case of UtcTime, if the new time is more than 1.5 seconds after the
 * most recent time previously observed).
 *
 * Other observation id's are either used to update tracked data or ignored.
 * (There are dozens of id's returned by an H5000 system with a lot of
 * connected devices, so it doesn't necessarily make sense to track them all.
 * But it is possible with some changes to the code in this function.)
 *
 * @param o A BgObservation object with the new data.
*/
void H5000Logger::ProcessObservation(BgObservation& o) {

    // Select the handling code corresponding to the data id
    switch (o.getId()) {

    case 34:		// UTC (date)
    {
        // Convert the date from Julian Date to an Excel-compatible date (days since Jan 0, 1900)
        m_observations[static_cast<int>(H5000Logger::trackedData::UtcDate)] = o.getVal() - 2415019;
        unsigned long int utcdate = static_cast<int>(m_observations[static_cast<int>(H5000Logger::trackedData::UtcDate)]);

        // Test to see if this is the first "datestamp" observed
        if (m_rawTimestamp == 0)
        {
            // Test to see if a timestamp has been observed yet
            if (((int)m_observations[static_cast<int>(H5000Logger::trackedData::UtcTime)]) != 0)
            {
                // OK. We've just observed the first datestamp and have 
                // received a timestamp, so start a new output file for the 
                // new date
                NewDate(utcdate);
            }
        }
        else {
            // Test to see if the UTC Date has rolled over to the next day
            // todo Build the test to see if the UTC date has changed
            if (false) {
                NewDate(utcdate);
            }
        }

        // Set the date currently being tracked
        m_date = o.strVal();
        break;
    }

    case 35:		// UTC (time)
    {
        m_observations[static_cast<int>(H5000Logger::trackedData::UtcTime)] = o.getVal();
        unsigned long int utctime = static_cast<int>(m_observations[static_cast<int>(H5000Logger::trackedData::UtcTime)]);

        // Test to see if this is the first "timestamp" observed
        if (m_rawTimestamp == 0)
        {
            // Test to see if a 'datestamp' has been observed yet
            unsigned long int utcdate = static_cast<int>(m_observations[static_cast<int>(H5000Logger::trackedData::UtcDate)]);
            if (utcdate != 0)
            {
                // OK. We've just observed the first timestamp and have 
                // previously seen a 'datestamp,' so start a new output file 
                // for the new date
                NewDate(utcdate);

                // Set the rawTimestamp
                m_rawTimestamp = static_cast<uint64_t>(o.getVal());
            }
        }
        else
        {
            // Test to see if this time is within 2 seconds of the current stamp
            if (o.getVal() - m_rawTimestamp > 1.5)
            {
                NewTime(utctime);
            }
        }

        // Set the time currently being tracked
        m_time = o.strVal();
        break;
    }

    case 9:			// COG
        m_observations[static_cast<int>(H5000Logger::trackedData::Cog)] = o.getVal();
        break;

    case 37:		// HDG
        m_observations[static_cast<int>(H5000Logger::trackedData::Hdg)] = o.getVal();
        break;

    case 39:		// SET
        m_observations[static_cast<int>(H5000Logger::trackedData::TideSet)] = o.getVal();
        break;

    case 40:		// TIDE
        m_observations[static_cast<int>(H5000Logger::trackedData::TideRate)] = o.getVal();
        break;

    case 41:		// SOG
        m_observations[static_cast<int>(H5000Logger::trackedData::Sog)] = o.getVal();
        break;

    case 42:		// BSPD
        m_observations[static_cast<int>(H5000Logger::trackedData::Bsp)] = o.getVal();
        break;

    case 46:		// AWS
        m_observations[static_cast<int>(H5000Logger::trackedData::Aws)] = o.getVal();
        break;

    case 47:		// TWS
        m_observations[static_cast<int>(H5000Logger::trackedData::Tws)] = o.getVal();
        break;

    case 140:		// AWA
        m_observations[static_cast<int>(H5000Logger::trackedData::Awa)] = o.getVal();
        break;

    case 141:		// TWA
        m_observations[static_cast<int>(H5000Logger::trackedData::Twa)] = o.getVal();
        break;

    case 142:		// TWD
        m_observations[static_cast<int>(H5000Logger::trackedData::Twd)] = o.getVal();
        break;

    case 228:		// TGW Target TWA
        m_observations[static_cast<int>(H5000Logger::trackedData::TargetTwa)] = o.getVal();
        break;

    case 234:		// TGS Target Boat Speed
        m_observations[static_cast<int>(H5000Logger::trackedData::TargetSpd)] = o.getVal();
        break;

    case 235:		// VMG
        m_observations[static_cast<int>(H5000Logger::trackedData::Vmg)] = o.getVal();
        break;

    case 240:		// POL Polar Speed
        m_observations[static_cast<int>(H5000Logger::trackedData::PolarBsp)] = o.getVal();
        break;

    case 241:		// POL Polar Performance
        m_observations[static_cast<int>(H5000Logger::trackedData::PolarPerf)] = o.getVal();
        break;

    case 243:		// WAM Wind Angle to Mast
        m_observations[static_cast<int>(H5000Logger::trackedData::MastWa)] = o.getVal();
        break;

    case 331:		// MWA
        m_observations[static_cast<int>(H5000Logger::trackedData::Mwa)] = o.getVal();
        break;

    case 332:		// MWS
        m_observations[static_cast<int>(H5000Logger::trackedData::Mws)] = o.getVal();
        break;

    case 383:		// TAC TWA Correction
        m_observations[static_cast<int>(H5000Logger::trackedData::TwaCorr)] = o.getVal();
        break;

    case 384:		// TWC TWS Correction
        m_observations[static_cast<int>(H5000Logger::trackedData::TwsCorr)] = o.getVal();
        break;

    case 403:		// BSC Boat Speed Correction
        m_observations[static_cast<int>(H5000Logger::trackedData::BspCorr)] = o.getVal();
        break;

    case 404:		// CWS CorrectedMWS
        m_observations[static_cast<int>(H5000Logger::trackedData::CorrMws)] = o.getVal();
        break;

    case 405:		// CWA CorrectedMWA
        m_observations[static_cast<int>(H5000Logger::trackedData::CorrMwa)] = o.getVal();
        break;

    case 406:		// TS OrigTWS
        m_observations[static_cast<int>(H5000Logger::trackedData::OrigTws)] = o.getVal();
        break;

    case 407:		// TA OrigTWA
        m_observations[static_cast<int>(H5000Logger::trackedData::OrigTwa)] = o.getVal();
        break;

    case 408:		// TD OrigTWD
        m_observations[static_cast<int>(H5000Logger::trackedData::OrigTwd)] = o.getVal();
        break;

    case 409:		// MBS MeasuredBSP
        m_observations[static_cast<int>(H5000Logger::trackedData::MBsp)] = o.getVal();
        break;

    case 437:		// MeasuredBSPPort
        m_observations[static_cast<int>(H5000Logger::trackedData::MBspPrt)] = o.getVal();
        break;

    case 438:		// MeasuredBSPStbd
        m_observations[static_cast<int>(H5000Logger::trackedData::MBspStbd)] = o.getVal();
        break;

    case 497:		// SpdThruWater
        m_observations[static_cast<int>(H5000Logger::trackedData::SpdThruWater)] = o.getVal();
        break;

        // The following data item id's/values will not be written to the .csv file
    case 1:		// ALT
    case 3:		// EPE
    case 4:		// HDOP
    case 5:		// VDOP
    case 7:		// PDOP
    case 8:		// GSEP
    case 10:	// PQU
    case 11:	// PIN
    case 12:	// SATS
    case 13:	// SDG
    case 27:	// LOG
    case 30:	// TP1
    case 31:	// TP1	/??
    case 44:	// AV1
    case 45:	// MX1
    case 77:	// DEP
    case 121:	// ROT
    case 122:	// TRM
    case 125:	// VAR
    case 146:	// RUD
    case 150:	// CMR
    case 157:	// GPF
    case 165:	// DOF
    case 221:	// CRS
    case 224:	// HOT
    case 226:	// LEE
    case 230:	// TMR
    case 309:	// BOW LAT
    case 310:	// BOW LON
    case 317:	// DRB
    case 318:	// DRD
    case 319:	// DPE
    case 320:	// DSE
    case 321:	// DLI
    case 325:	// HEL
    case 336:	// OWA
    case 340:	// SLP LAT
    case 341:	// SLP LON
    case 352:	// SLS LAT
    case 353:	// SLS LON
    case 354:	// SLB
    case 355:	// TP2
    case 356:	// TP2
    case 357:	// VMG
    case 360:	// AV2
    case 361:	// MX1
    case 380:	// BSL
    case 381:	// SLA
    case 382:	// DLI
    case 385:	// RED
    case 386:	// GRN
    case 387:	// BLU
    case 391:	// PERP
    case 392:	// ARR
    case 398:	// AC1
    case 399:	// AC2
    case 400:	// AC3
    case 401:	// AC4
    case 402:	// DES
    case 410:	// ULO
    case 414:	// BLI
    case 415:	// BLIB
    case 420:	// SLA
    case 421:	// GPS LAT
    case 422:	// GPS LON
    case 423:	// BPE
    case 424:	// BSE
    case 425:	// Opposite Tack COG
    case 426:	// Opposite Tack Target Heading
    case 439:	// ATWD
    case 440:	// WPHS
    case 441:	// LIFT
    case 466:	// LEE
    case 467:	// DPE
    case 468:	// DSE
    case 498:	// APM
    case 499:	// GBA
    case 500:	// TWR
    case 501:	// HCP
    case 502:	// NCR
    case 503:	// PTW
    case 504:	// WHL
    case 505:	// AHL
        break;

    default:
        // Print an alert if an unrecognized data item id is sent by the H5000.
        cerr << "Unhandled value: " << o.getId() << endl;
        break;
    }
}

/**
 * @brief Create a BgObservation object from a JSON string, then process that observation.
 *
 * @param buffer_ A string containing JSON, representing an H5000 data observation.
*/
void H5000Logger::ProcessObservation(const char* buffer)
{
    // todo this may not work
    string s = buffer;
    BgObservation o(s);
    ProcessObservation(o);
}

/**
 * @brief Start a new .csv file and re-initialize the collection of observations.
 *
 * @param utcdate_ An integer representation of the number of days since Jan 0, 1900.
*/
void H5000Logger::NewDate(unsigned long int utcdate)
{
    // Write the accumulated information for the most recent timestamp to 
    // the .csv file, then close it (if the file was open in the first place)
    if (m_fout != NULL) {
        // todo check/fix date rollover 
        NewTime(0);     // Cause the latest data to be written to the file
        fclose(m_fout);
    }

    // Open a new .csv output file for observations from this new date
    m_fout = fopen(MakeFileName(utcdate).c_str(), "w");

    // Write a line of column headings
    //string headings = "UtcDate,UtcTime,Cog,Hdg,Sog,Bsp,MBsp,MBspPrt,SpdThruWater,BspCorr,Mws,Aws,Tws,OrigTws,CorrMws,TwsCorr,Mwa,CorrMwa,Awa,Twa,";
    //headings += "OrigTwa,TwaCorr,MastWa,Twd,OrigTwd,TideSet,TideRate,TargetTwa,TargetSpd,PolarBsp,PolarPerf,Vmg\n";
    string headings = "Boat,Utc,BSP,AWA,AWS,TWA,TWS,TWD,RudderFwd,Leeway,Set,Drift,HDG,AirTemp,SeaTemp,Baro,Depth,Heel,Trim,Rudder,Tab,Forestay,Downhaul,";
    headings += "MastAng,FstayLen,MastButt,Load S,Load P,Rake,Volts,ROT,GpQual,PDOP,GpsNum,GpsAge,Altitude,GeoSep,GpsMode,Lat,Lon,COG,SOG,DiffStn,Error,";
    headings += "RunnerS,RunnerP,Vang,Trav,Main,KeelAng,KeelHt,Board,Oil P,RPM 1,RPM 2,Board P";
    fputs(headings.c_str(), m_fout);

    // Clear the stored observations (and zero the time)
    Clear();
    m_observations[static_cast<int>(H5000Logger::trackedData::UtcTime)] = 0.0;
}

/**
 * @brief Write the accumulated observations for the old timestamp to the file.
 *
 * When a new timestamp is observed, the existing observations are written to
 * the .csv output file (identified with the old timestamp), the tracked data
 * values are cleared, and a new collection of observations is started.
 *
 * @param utctime_ An integer representation of the number of seconds since midnight UTC.
*/
void H5000Logger::NewTime(unsigned long int utctime)
{
    // Create a string with the values in CSV format
    stringstream ss;
    for (int i = 0; i < static_cast<int>(H5000Logger::trackedData::MAX_ITEMS); i++)
    {
        ss << m_observations[i] << ",";
    }
    string s = ss.str();
    s.pop_back();				// Remove trailing space
    s += '\n';


    // Write the line to the output file
    fputs(s.c_str(), m_fout);
    fflush(m_fout);

    // Clear the stored observations (except date and time)
    Clear();
    m_observations[static_cast<int>(H5000Logger::trackedData::UtcTime)] = utctime;

    // Set raw timestamp value
    m_rawTimestamp = static_cast<uint64_t>(m_observations[static_cast<int>(H5000Logger::trackedData::UtcTime)]);
}

/**
 * @brief Clear accumulated observation data from the object.
 *
 * This function is called immediately after a timestamp set of values has been
 * written to the .csv file (so intermittent observations/values are dropped
 * and not carried forward).
*/
void H5000Logger::Clear()
{
    // Save the date and time most recently observed
    double date = m_observations[static_cast<int>(H5000Logger::trackedData::UtcDate)];
    double time = m_observations[static_cast<int>(H5000Logger::trackedData::UtcTime)];

    // Clear the entire vector of data observation values
    m_observations.fill(0.0);

    // Restore the date and time values to initialize the new set of observations
    m_observations[static_cast<int>(H5000Logger::trackedData::UtcDate)] = date;
    m_observations[static_cast<int>(H5000Logger::trackedData::UtcTime)] = time;
}

/**
 * @brief Construct the .csv file name, including the date of the observations.
 *
 * @param utcdate_ An integer representing the number of days since Jan 0, 1900.
 * @return A string with the new file name.
*/
string H5000Logger::MakeFileName(unsigned long int utcdate)
{
    // Convert the Excel-format date to a tm struct (in UTC time)
    double dateExcel = utcdate;
    time_t tsUnix = (time_t)(dateExcel - 25569) * 86400;
    struct tm* dateTm = gmtime(&tsUnix);

    // Create the filename buffer, including the date
    char buffer[100];
    strftime(buffer, 100, "%Y%m%d-h5000-cpu-data.csv", dateTm);

    return string(buffer);
}
