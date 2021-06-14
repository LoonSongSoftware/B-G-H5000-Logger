/**
    bg-websocket-session-class.cpp

    A class which encapsulates a websocket session--establishing the 
    connection, requesting a data stream, and logging the incoming data.

    Copyright (c) 2020 D. Scott Miller. All rights reserved.
    This code is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License as published by the Free
    Software Foundation; either version 2 of the License, or (at your option)
    any later version.

    "Boost" code based on websocket_client_async
    Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
*/

#include "bg-websocket-session-class.h"
#include "h5000-logger-class.h"

using namespace std;

/// //////////////////////////////////////////////////////////////
// Public member functions
/// //////////////////////////////////////////////////////////////

/**
 * @brief Constructor--initialize variables.
 * 
 * This constructor simply initializes member variables. To actually begin the 
 * session, use the 'run' member.
 * 
 * @param ioc An io_context object to provide asio I/O
 * @see run()
*/
BgWebsocketSession::BgWebsocketSession(H5000Logger* pApp, net::io_context& ioc, bool debugFlag, bool testFlag)
    : m_app(pApp), m_debugFlag(debugFlag), m_testFlag(testFlag),
	m_resolver(net::make_strand(ioc)), m_ws(net::make_strand(ioc))
{
    DEBUGOUT("Constructed session.")
}

/**
 * @brief Destructor.
 * 
 * Empty for now.
 */
BgWebsocketSession::~BgWebsocketSession()
{
}

/**
 * @brief Start a websocket session.
 * 
 * This code begins the cascade of asynchronous websocket connection activities.
 * 
 * @param host		The websocket server fqdn or IP address
 * @param port		The websocket port
 * @param logFile	If true, create a "flat" logfile with a line for each observation
 * @param csvFile	if true, create "timestamp" logfile with a line for each time interval
*/
void BgWebsocketSession::run(char const* host, char const* port)
{
    DEBUGOUT("Running session.")

    // Save these for later
    m_host = host;

    // Look up the domain name and call on_resolve when complete
    m_resolver.async_resolve(
        m_host,
        port,
        beast::bind_front_handler(
            &BgWebsocketSession::on_resolve,
            shared_from_this()));
    DEBUGOUT("::Run exiting.")
}


/// //////////////////////////////////////////////////////////////
// Private member functions: async communication with the websocket
/// //////////////////////////////////////////////////////////////

/**
 * @brief Called when the session hostname has been resolved (or not).
 * 
 * If the hostname was successfully resolved, this function will initiate an attempt
 * to connect to the host on the specified port.
 * 
 * @param ec Error code (0 if successful)
 * @param results An object with the results of the resolve() call
*/
void BgWebsocketSession::on_resolve(beast::error_code ec, tcp::resolver::results_type results)
{
    if (ec)
        return fail(ec, "resolve");

    DEBUGOUT("Session resolved.")

    // Set the timeout for the operation
    beast::get_lowest_layer(m_ws).expires_after(chrono::seconds(5));

    // Make the connection on the IP address we get from a lookup
    beast::get_lowest_layer(m_ws).async_connect(
        results,
        beast::bind_front_handler(
            &BgWebsocketSession::on_connect,
            shared_from_this()));
    DEBUGOUT("::on_resolve exiting.")
}

/**
 * @brief Executes when the connection is made (or not).
 * 
 * This function is called when the session connection has been established (or 
 * failed with an error code). Assuming it succeeded, proceed with the 
 * websocket "handshake" to change the connection from standard HTTP to 
 * WebSocket protocol.
 * 
 * @param ec Error code (0 if successful)
 * @param ep The endpoint type (including port number).
*/
void BgWebsocketSession::on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type ep)
{
    if (ec)
        return fail(ec, "connect");

    DEBUGOUT("Session connected.")

    // Turn off the timeout on the tcp_stream, because
    // the websocket stream has its own timeout system.
    beast::get_lowest_layer(m_ws).expires_never();

    // Set suggested timeout settings for the websocket
    m_ws.set_option(
        websocket::stream_base::timeout::suggested(
            beast::role_type::client));

    // Set a decorator to change the User-Agent of the handshake
    m_ws.set_option(websocket::stream_base::decorator(
        [](websocket::request_type& req)
    {
        req.set(http::field::user_agent,
            string(BOOST_BEAST_VERSION_STRING) +
            " h5000-logger");
    }));

    // Update the host_ string. This will provide the value of the
    // Host HTTP header during the WebSocket handshake.
    // See https://tools.ietf.org/html/rfc7230#section-5.4
    m_host += ':' + to_string(ep.port());

    // Perform the websocket handshake
    m_ws.async_handshake(m_host, "/",
        beast::bind_front_handler(
            &BgWebsocketSession::on_handshake,
            shared_from_this()));
    DEBUGOUT("::on_connect exiting.")
}

/**
 * @brief After the websocket handshake is complete, request values from the server.
 * 
 * @param ec Error code (0 if successful).
*/
void BgWebsocketSession::on_handshake(beast::error_code ec)
{
    if (ec)
        return fail(ec, "handshake");

    DEBUGOUT("Session handshake complete.")

    // Subscribe to all "interesting" data with continuous refresh
    m_app->RequestAllValues();

    // Read the first message into our buffer (subsequent reads will occur via on_read)
    m_ws.async_read(
        m_buffer,
        beast::bind_front_handler(
            &BgWebsocketSession::on_read,
            shared_from_this()));
    DEBUGOUT("::on_handshake exiting.")
}

/**
 * @brief Read a message sent by the websocket server (B&G H5000 CPU).
 * 
 * @param ec Error code (0 if successful)
 * @param bytes_transferred Number of bytes in the incoming message
*/
void BgWebsocketSession::on_read(beast::error_code ec, size_t bytes_transferred)
{
    boost::ignore_unused(bytes_transferred);

    if (ec)
        return fail(ec, "read");

    stringstream testout;
    testout << "Message read:    ";
    DEBUGOUT(testout.str())

    stringstream ss;
    ss << beast::make_printable(m_buffer.data());
    //DEBUGOUT(ss.str());
    m_app->handleResponse(ss.str());

    // Clear the buffer
    m_buffer.consume(m_buffer.size());

    // Read another message
    m_ws.async_read(
        m_buffer,
        beast::bind_front_handler(
            &BgWebsocketSession::on_read,
            shared_from_this()));
    DEBUGOUT("::on_read exiting.")
}

/**
 * @brief Called when a write operation is complete.
 * 
 * This function will remove the completed message from the outgoing message queue then,
 * if there are more messages to be sent, will send the next message to the server.
 * 
 * @param ec Error code (0 if successful).
 * @param bytes_transferred Number of bytes written to the server.
*/
void BgWebsocketSession::on_write(beast::error_code ec, size_t bytes_transferred)
{
    boost::ignore_unused(bytes_transferred);

    if (ec)
        return fail(ec, "write");

    DEBUGOUT("Message written.")

    // Remove the message from the queue
    m_queue.erase(m_queue.begin());

    // Send the next message (if any)
    if (!m_queue.empty()) {
        stringstream testout;
        testout << "Writing message:\n" << *m_queue.front();
        DEBUGOUT(testout.str())
        m_ws.async_write(
            net::buffer(*m_queue.front()),
            beast::bind_front_handler(&BgWebsocketSession::on_write,
                shared_from_this()));
    }
    DEBUGOUT("::on_write exiting.")
}

/**
 * @brief Called when the websocket connection is closed.
 * 
 * This function will close the log files, to ensure that all data is written.
 * 
 * @param ec Error code (0 if successful)
*/
void BgWebsocketSession::on_close(beast::error_code ec)
{
    if (ec)
        return fail(ec, "close");

    DEBUGOUT("Session closed.")

    // If we get here then the connection is closed gracefully
    //m_app->CloseLogFiles();
    DEBUGOUT("::on_close exiting.")
}

/**
 * @brief Write a Beast error message to the console.
 * 
 * @param ec Error code
 * @param what A string identifying where this error occurred.
*/
void BgWebsocketSession::fail(beast::error_code ec, char const* what)
{
    cerr << what << ": " << ec.message() << "\n";
    DEBUGOUT("FAIL::")
}

/**
 * @brief Enqueue a message to be sent to the websocket server.
 * 
 * This function will add a message to the outgoing queue and, if no other message
 * is currently being written, will initiate the "write."
 * 
 * @param ss A string with the message to be sent.
*/
void BgWebsocketSession::send(shared_ptr<string const> const& ss)
{
    // Always add to queue
    m_queue.push_back(ss);

    // Are we already writing?
    if (m_queue.size() > 1)
        return;

    // We are not currently writing, so send this immediately
    stringstream testout;
    testout << "Writing message:\n" << *m_queue.front();
    DEBUGOUT(testout.str())
    m_ws.async_write(
        net::buffer(*m_queue.front()),
        beast::bind_front_handler(&BgWebsocketSession::on_write,
            shared_from_this()));
    DEBUGOUT("::send exiting.")
}
