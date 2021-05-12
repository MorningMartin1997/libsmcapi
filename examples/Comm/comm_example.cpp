//
// Created by User on 2021/5/10.
//

#include <boost/thread/thread.hpp>
#include <comm/Comm.hpp>
#include <utility>
#include <infra/ConfigFile.h>


struct CommConfig {
    string party_1_ip;
    string party_2_ip;
    int party_1_port;
    int party_2_port;
    string certificateChainFile;
    string password;
    string privateKeyFile;
    string tmpDHFile;
    string clientVerifyFile;
    string classType;
    CommConfig(string party_1_ip, string party_2_ip, int party_1_port, int party_2_port,
               string certificateChainFile, string password, string privateKeyFile, string tmpDHFile,
               string clientVerifyFile, string classType) {
        this->party_1_ip = std::move(party_1_ip);
        this->party_2_ip = std::move(party_2_ip);
        this->party_1_port = party_1_port;
        this->party_2_port = party_2_port;
        this->certificateChainFile = certificateChainFile;
        this->password = password;
        this->privateKeyFile = privateKeyFile;
        this->tmpDHFile = tmpDHFile;
        this->clientVerifyFile = clientVerifyFile;
        this->classType = classType;
    }
};

CommConfig readCommConfig(string config_file){
    ConfigFile configFile(config_file);
#ifdef _WIN32
    string os = "windows";
#else
    string os = "Linux";
#endif
    int party_1_port = stoi(configFile.Value("", "party_1_port"));
    int party_2_port = stoi(configFile.Value("", "party_2_port"));
    string party_1_ip = configFile.Value("", "party_1_ip");
    string party_2_ip = configFile.Value("", "party_2_ip");
    string certificateChainFile = configFile.Value(os, "certificateChainFile");
    string password = configFile.Value(os, "password");
    string privateKeyFile = configFile.Value(os, "privateKeyFile");
    string tmpDHFile = configFile.Value(os, "tmpDHFile");
    string clientVerifyFile = configFile.Value(os, "clientVerifyFile");
    string classType = configFile.Value("", "classType");
    return CommConfig(party_1_ip, party_2_ip, party_1_port, party_2_port,certificateChainFile, password, privateKeyFile, tmpDHFile, clientVerifyFile, classType);
}

CommParty* getCommParty(const CommConfig& commConfig, const string& partyNumber, boost::asio::io_service& io_service) {
    string myIp = (partyNumber == "1") ? commConfig.party_1_ip : commConfig.party_2_ip;
    string otherIp = (partyNumber == "1") ? commConfig.party_2_ip : commConfig.party_1_ip;
    int myPort = (partyNumber == "1") ? commConfig.party_1_port : commConfig.party_2_port;
    int otherPort = (partyNumber == "1") ? commConfig.party_2_port : commConfig.party_1_port;
    SocketPartyData me(IpAddress::from_string(myIp), myPort);
    SocketPartyData other(IpAddress::from_string(otherIp), otherPort);
    cout << "trying to connect to: " << otherIp << " port: " << otherPort << endl;
    if (commConfig.classType == "CommPartyTCPSynced")
    {
        cout << "Running Communication Example With CommPartyTCPSynced Class" << endl;
        return new CommPartyTCPSynced(io_service, me, other);
    }
    throw invalid_argument("Got unsupported class type in config file");
}

void print_send_message(const string &s, int i){
    cout << "sending message number" << i << "message: " << s <<endl;
}

void print_recv_message(const string &s, int i) {
    cout << "receievd message number " << i << " message: " << s << endl;
}

void send_messages(CommParty *commParty, string *messages, int start, int end){
    for (int i = start; i < end; i++){
        auto s = messages[i];
        print_send_message(s, i);
        commParty->write((const byte *)s.c_str(), s.size(), -1, -1);
    }
}

void recv_messages(CommParty *commParty, string *messages, int start,
                   int end, byte *buffer, int expectedSize){
    commParty->read(buffer, expectedSize, -1, -1);
    // the size of all strings is 2. Parse the message to get the original strings.
    for (int i = start, j = 0; i < end; ++i, ++j){
        auto s = string(reinterpret_cast<char const *>(buffer+j*2), 2);
        print_recv_message(s, i);
        messages[i] = s;
    }
}


int Comm(string partyNumber, string filePath) {
    try {
        if (partyNumber != "0" && partyNumber != "2"){
            cerr << "Usage: ./libscapi_example comm <party_number(0|2)> <config_file_path>";
            return 0;
        }
        auto commConfig = readCommConfig(filePath);

        boost::asio::io_service ioService;
        CommParty *commParty = getCommParty(commConfig, partyNumber, ioService);
        boost::thread t([ObjectPtr = &ioService] { ObjectPtr->run(); });
        commParty->join(500, 5000);

        string sendMessages[6] =  { "s0", "s1", "s2", "s3", "s4", "s5" };
        string recvMessages[6];
        byte buffer[100];

        // send 3 messages, get 3. send additional 2 get 2. send 1 get 1.
        send_messages(commParty, sendMessages, 0, 3);
        recv_messages(commParty, recvMessages, 0, 3, buffer, 6);
        send_messages(commParty, sendMessages, 3, 5);
        recv_messages(commParty, recvMessages, 3, 5, buffer, 4);
        send_messages(commParty, sendMessages, 5, 6);
        recv_messages(commParty, recvMessages, 5, 6, buffer, 2);

        ioService.stop();
        t.join();
        delete commParty;
    }
    catch (std::exception& e){
        std::cerr << "Exception: " << e.what() << "\n";
    }
    return 0;
}