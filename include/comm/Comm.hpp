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
#include <utility>

namespace boost_ip = boost::asio::ip;
using IpAddress = boost_ip::address;
using boost::asio::io_service;
using tcp = boost_ip::tcp;
using namespace std;


typedef unsigned char byte;

/// A marker interface. Each type of party should have a concrete class that implement this interface.
class PartyData{};

/*
 * This class holds the data of a party in a communication layer.
 * It should be used in case the user wants to use the regular mechanism of communication using tcp sockets.
 */
class SocketPartyData : public PartyData{
private:
    IpAddress ipAddress;
    int port;
    int compare(const SocketPartyData &other) const;

public:
    SocketPartyData(IpAddress ip, int port){
        this->ipAddress = ip;
        this->port = port;
    }

    IpAddress getIpAddress() const{
        return ipAddress;
    }

    int getPort() const{
        return port;
    }

    string toLogString() {
        return ipAddress.to_string() + "|" + to_string(port);
    }

    /**
	* Compares two parties.
	*<0 if this party's string is smaller than the otherParty's string representation.
	*>0 if this party's string is larger than the otherParty's string representation.
	*/
    bool operator==(const SocketPartyData &other) const { return (compare(other) == 0); };
    bool operator!=(const SocketPartyData &other) const { return (compare(other) != 0); };
    bool operator<=(const SocketPartyData &other) const { return (compare(other) <= 0); };
    bool operator>=(const SocketPartyData &other) const { return (compare(other) >= 0); };
    bool operator>(const SocketPartyData &other) const { return (compare(other) > 0); };
    bool operator<(const SocketPartyData &other) const { return (compare(other) < 0); };
};


class CommParty {
public:
    /// This method setups a double edge connection with another party. It connects to the other
    /// party and also accepts connections from it.
    /// The method blocks until both sides are connected to each other.
    /// \param sleep_between_attempts
    /// \param timeout
    /// \param first
    /// \return
    virtual int join(int sleep_between_attempts, int timeout) = 0;

    /// Write data to the other party
    /// \param data
    /// \param size
    /// \param peer
    /// \param protocol
    /// \return
    virtual size_t write(const byte* data, size_t size, int peer, int protocol) = 0;

    virtual size_t read(byte *data, int size2Read, int peer, int protocol) = 0;

    virtual ~CommParty() = default;
};

class CommPartyTCPSynced : public CommParty{
private:
    io_service &_ioServiceServer;
    io_service &_ioServiceClient;
    tcp::acceptor _acceptor;
    tcp::socket _serverSocket;
    tcp::socket _clientSocket;
    SocketPartyData _me;
    SocketPartyData _other;
    int _role;

    void setSocketOptions();
    tcp::socket & socketForRead() {
        if (_role == 1)
            return _clientSocket;
        return _serverSocket;
    }

    tcp::socket & socketForWrite() {
        if (_role == 0)
            return _serverSocket;
        return _clientSocket;
    }

public:
    CommPartyTCPSynced(boost::asio::io_service& ioService, const SocketPartyData& me, SocketPartyData other, int role = 2):
    _ioServiceServer(ioService),
    _ioServiceClient(ioService),
    _acceptor(ioService, tcp::endpoint(tcp::v4(), (role == 1) ? 10000 + me.getPort() : me.getPort())),
    _serverSocket(ioService),
    _clientSocket(ioService),
    _me(me),
    _other(std::move(other)),
    _role(role)
    {};

    // accumulate the number of send/receive bytes
    unsigned long bytesIn = 0;
    unsigned long bytesOut = 0;

    int join(int sleepBetweenAttempts, int time_out) override;
    size_t write(const byte* data, size_t size, int peer, int protocol) override;
    size_t read(byte *data, int size2Read, int peer, int protocol) override;
};

int CommPartyTCPSynced::join(int sleepBetweenAttempts, int time_out) {
   int totalSleep = 0;
   bool isAccepted = (_role == 1);
   bool isConnected = (_role == 0);

   // establish connections
   while (!isConnected || !isAccepted){
       try {
           if (!isConnected){
               tcp::resolver resolver(_ioServiceClient);
               tcp::resolver::query query(_other.getIpAddress().to_string(), to_string(_other.getPort()));
               tcp::resolver::iterator endpointIterator = resolver.resolve(query);
               boost::asio::connect(_clientSocket, endpointIterator);
               isConnected = true;
           }
       }
       catch (const boost::system::system_error& ex){
           if (totalSleep > time_out){
               cerr << "Failed to connect after timeout, aborting!";
               throw ex;
           }
           cout << "Failed to connect. Sleeping for " << sleepBetweenAttempts <<
           " milliseconds, " << ex.what() <<endl;
           this_thread::sleep_for(chrono::milliseconds(sleepBetweenAttempts));
           totalSleep += sleepBetweenAttempts;
       }
       if (!isAccepted){
           boost::system::error_code ec;
           cout << "accepting..." <<endl;
           _acceptor.accept(_serverSocket, ec);
           isAccepted = true;
       }
   }
   setSocketOptions();
   return 0;
}

size_t CommPartyTCPSynced::write(const byte *data, size_t size, int peer, int protocol) {
    boost::system::error_code ec;
    bytesOut += size;
    return boost::asio::write(socketForWrite(), boost::asio::buffer(data, size),
                              boost::asio::transfer_all(), ec);
}

void CommPartyTCPSynced::setSocketOptions() {
    boost::asio::ip::tcp::no_delay option(true);
    if (_role != 1){
        _serverSocket.set_option(option);
    } else if (_role != 0){
        _clientSocket.set_option(option);
    }
}

size_t CommPartyTCPSynced::read(byte *data, int size2Read, int peer, int protocol) {
    bytesIn += size2Read;
    return boost::asio::read(socketForRead(), boost::asio::buffer(data, size2Read));
}




