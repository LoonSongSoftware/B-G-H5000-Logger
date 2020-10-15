/**
    bg-websocket-session-class.h
    Code to establish, maintain, etc. the websocket connection.

    Copyright (c) 2020 D. Scott Miller. All rights reserved.
    This code is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License as published by the Free
    Software Foundation; either version 2 of the License, or (at your option)
    any later version.
*/

#ifndef __BG_WEBSOCKET_SESSION_CLASS_H
#define __BG_WEBSOCKET_SESSION_CLASS_H

// forward class definition to appease gcc when referenced in include file(s)
class BgWebsocketSession; 

#include "h5000-logger-class.h"
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/strand.hpp>
#include <iostream>

//------------------------------------------------------------------------------
// Preamble
namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
class H5000Logger;

//------------------------------------------------------------------------------
// Class definition
class BgWebsocketSession : public std::enable_shared_from_this<BgWebsocketSession>
{
    H5000Logger* m_app;                                     // pointer to the application object (contains message handlers)
    std::string m_host;										// hostname (fqdn) or ip address of the websocket server
    bool m_testMode;

    tcp::resolver m_resolver;								// for resolving fqdn to ip address
    websocket::stream<beast::tcp_stream> m_ws;				// the websocket communication object
    beast::flat_buffer m_buffer;								// a buffer to hold websocket i/o
    std::vector<std::shared_ptr<std::string const>> m_queue;	// a queue of messages to be sent to the h5000 websocket

public:
    explicit BgWebsocketSession(H5000Logger* pApp, net::io_context& ioc, bool testMode);
    ~BgWebsocketSession();
    void run(char const* host, char const* port);
    void send(shared_ptr<string const> const& ss);

private:
    // Websocket async handlers
    void on_resolve(beast::error_code ec, tcp::resolver::results_type results);
    void on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type ep);
    void on_handshake(beast::error_code ec);
    void on_read(beast::error_code ec, std::size_t bytes_transferred);
    void on_write(beast::error_code ec, std::size_t bytes_transferred);
    void on_close(beast::error_code ec);
    void fail(beast::error_code ec, char const* what);
};

#endif	// __BG_WEBSOCKET_SESSION_CLASS_H
