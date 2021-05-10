//
// Created by User on 2021/5/10.
//

#include "infra/ConfigFile.h"
#include <fstream>

using namespace std;

string trim(string const &source, char const *delims = " \t\r\n"){
    string result(source);
    auto index = result.find_last_not_of(delims);
    if (index != string::npos){
        result.erase(++index);
    }

    index = result.find_first_not_of(delims);
    if (index != string::npos){
        result.erase(0, index);
    } else{
        result.erase();
    }
    return result;
}

ConfigFile::ConfigFile(const std::string &configFile) {
    ifstream file(configFile.c_str());

    string line, name, value, inSection;
    int posEqual;
    while (std::getline(file, line)) {

        if (!line.length()) continue;

        if (line[0] == '#') continue;
        if (line[0] == ';') continue;

        if (line[0] == '[') {
            inSection = trim(line.substr(1, line.find(']') - 1));
            continue;
        }

        posEqual = line.find('=');
        name = trim(line.substr(0, posEqual));
        value = trim(line.substr(posEqual + 1));

        _content_[inSection + '/' + name] = value;
    }
}

string const& ConfigFile::Value(const std::string &section, const std::string &entry) const {
    auto ci = _content_.find(section + '/' + entry);
    if (ci == _content_.end()) {
        throw invalid_argument("not exist");
    }
    return ci->second;
}