//
// Created by User on 2021/5/10.
//

#pragma once

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <boost/bind.hpp>
#include <mutex>
#include <thread>
#include <string>
#include <iostream>
#include <condition_variable>

namespace boost_ip = boost::asio::ip;
using IpAddress = boost_ip::address;
using tcp = boost_ip::tcp;
using namespace std;

typedef unsigned char byte;

