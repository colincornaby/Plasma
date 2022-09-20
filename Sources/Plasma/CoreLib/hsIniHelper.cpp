//
//  hsIniHelper.cpp
//  CoreLib
//
//  Created by Colin Cornaby on 8/29/22.
//

#include "hsIniHelper.h"
#include "hsStringTokenizer.h"

hsIniEntry::hsIniEntry(ST::string line):
fCommand(), fComment() {
    if(line.size() == 0) {
        fType = kBlankLine;
    } else if(line[0] == '#') {
        fType = kComment;
        fComment = line.after_first('#');
    } else if(line == "\n") {
        fType = kBlankLine;
    } else {
        fType = kCommandValue;
        hsStringTokenizer tokenizer = hsStringTokenizer(line.c_str(), " ");
        char *str;
        int i = 0;
        while((str = tokenizer.next())) {
            if (i==0) {
                fCommand = str;
            } else {
                fValues.push_back(str);
            }
            i++;
        }
    }
}

void hsIniEntry::setValue(size_t idx, ST::string value) {
    if (fValues.size() >= idx) {
        fValues[idx] = value;
    } else {
        for (int i=fValues.size(); i<idx; i++) {
            fValues.push_back("");
        }
        fValues.push_back(value);
    }
}

std::optional<ST::string> hsIniEntry::getValue(size_t idx) {
    if (fValues.size() < idx) {
        return std::optional<ST::string>();
    } else {
        return fValues[idx];
    }
}


hsIniFile::hsIniFile(plFileName filename) {
    
    this->filename = filename;
    readFile();
}


hsIniFile::hsIniFile(hsStream& stream) {
    readStream(stream);
}

void hsIniFile::readStream(hsStream& stream) {
    ST::string line;
    while(stream.ReadLn(line)) {
        std::shared_ptr<hsIniEntry> entry = std::make_shared<hsIniEntry>(line);
        fEntries.push_back(entry);
    }
}

void hsIniFile::writeFile() {
    hsAssert(filename.GetSize() > 0, "writeFile requires contructor with filename");
    
    hsBufferedStream s;
    s.Open(filename, "w");
    writeStream(s);
    s.Close();
}

void hsIniFile::readFile() {
    hsAssert(filename.GetSize() > 0, "writeFile requires contructor with filename");
    
    fEntries.clear();
    
    hsBufferedStream s;
    s.Open(filename);
    readStream(s);
    s.Close();
}

void hsIniFile::writeFile(plFileName filename) {
    hsBufferedStream s;
    s.Open(filename, "w");
    writeStream(s);
    s.Close();
}

void hsIniFile::writeStream(hsStream& stream) {
    for (std::shared_ptr<hsIniEntry> entry: fEntries) {
        switch (entry->fType) {
            case hsIniEntry::kBlankLine:
                stream.WriteSafeString("\n");
                break;
            case hsIniEntry::kComment:
                stream.WriteSafeString("#" + entry.get()->fComment + "\n");
                break;
            case hsIniEntry::kCommandValue:
                ST::string line = entry->fCommand;
                for (ST::string value: entry->fValues) {
                    line += " " + value;
                }
                line += "\n";
                stream.WriteString(line);
                break;
        }
    }
}

std::shared_ptr<hsIniEntry> hsIniFile::findByCommand(ST::string command) {
    for (std::shared_ptr<hsIniEntry> entry: fEntries) {
        if(entry->fCommand == command) {
            return entry;
        }
    }
    return std::shared_ptr<hsIniEntry>(nullptr);
}
