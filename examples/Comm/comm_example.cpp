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

}