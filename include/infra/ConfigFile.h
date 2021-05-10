//
// Created by User on 2021/5/10.
//

#ifndef LIBSMCAPI_CONFIGFILE_H
#define LIBSMCAPI_CONFIGFILE_H

#pragma once

#include <string>
#include <map>

class ConfigFile {
protected:
    std::map<std::string, std::string> _content_;

public:
    ConfigFile(std::string const &configFile);
    std::string const &Value(std::string const &section, std::string const &entry) const;
};
#endif //LIBSMCAPI_CONFIGFILE_H
