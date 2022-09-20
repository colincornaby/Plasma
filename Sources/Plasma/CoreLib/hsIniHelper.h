//
//  hsIniHelper.hpp
//  CoreLib
//
//  Created by Colin Cornaby on 8/29/22.
//

#ifndef hsIniHelper_hpp
#define hsIniHelper_hpp

#include <stdio.h>
#include <string_theory/string>
#include <vector>
#include <optional>
#include "plFileSystem.h"
#include "hsStream.h"

#endif /* hsIniHelper_hpp */

class hsIniFile;

class hsIniEntry {
    friend hsIniFile;
public:
    
    enum Type {
        kBlankLine = 0,
        kComment,
        kCommandValue
    };
    
    Type fType;
    ST::string fCommand;
    ST::string fComment;
    std::vector<ST::string> fValues;
    void setValue(size_t idx, ST::string value);
    std::optional<ST::string> getValue(size_t idx);
    hsIniEntry(ST::string line);
};

class hsIniFile {
public:
    std::vector<std::shared_ptr<hsIniEntry>> fEntries;
    
    hsIniFile(plFileName filename);
    hsIniFile(hsStream& stream);
    void writeFile(plFileName filename);
    void writeFile();
    void writeStream(hsStream& stream);
    std::shared_ptr<hsIniEntry> findByCommand(ST::string command);
    void readFile();
private:
    void readStream(hsStream& stream);
    plFileName filename;
};
