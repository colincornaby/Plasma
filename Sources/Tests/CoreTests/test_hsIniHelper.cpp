//
//  test_hsIniHelper.cpp
//  test_CoreLib
//
//  Created by Colin Cornaby on 8/29/22.
//


#include <gtest/gtest.h>

#include "hsIniHelper.h"
#include <stdio.h>
#include <string_theory/string>


TEST(hsIniHelper, entry_comment)
{
    hsIniEntry entry("#This is a comment");
    EXPECT_EQ(entry.fType, hsIniEntry::Type::kComment);
    EXPECT_STREQ(entry.fComment.c_str(), "This is a comment");
    EXPECT_EQ(entry.fValues.size(), 0);
    EXPECT_STREQ(entry.fCommand.c_str(), "");
}

TEST(hsIniHelper, entry_blankLine)
{
    hsIniEntry entry("\n");
    EXPECT_EQ(entry.fType, hsIniEntry::Type::kBlankLine);
    EXPECT_STREQ(entry.fComment.c_str(), "");
    EXPECT_EQ(entry.fValues.size(), 0);
    EXPECT_STREQ(entry.fCommand.c_str(), "");
}

TEST(hsIniHelper, entry_command)
{
    hsIniEntry entry("Graphics.Height 1024");
    EXPECT_EQ(entry.fType, hsIniEntry::Type::kCommandValue);
    EXPECT_STREQ(entry.fComment.c_str(), "");
    EXPECT_EQ(entry.fValues.size(), 1);
    EXPECT_STREQ(entry.fValues[0].c_str(), "1024");
    EXPECT_STREQ(entry.fCommand.c_str(), "Graphics.Height");
}

TEST(hsIniHelper, entry_command_quoted)
{
    hsIniEntry entry("Graphics.Height \"1024 1024\"");
    EXPECT_EQ(entry.fType, hsIniEntry::Type::kCommandValue);
    EXPECT_STREQ(entry.fComment.c_str(), "");
    EXPECT_EQ(entry.fValues.size(), 1);
    EXPECT_STREQ(entry.fValues[0].c_str(), "1024 1024");
    EXPECT_STREQ(entry.fCommand.c_str(), "Graphics.Height");
}

TEST(hsIniHelper, entry_stream_parse)
{
    hsRAMStream s;
    std::string line = "Graphics.Height 1024\n";
    s.Write(line.length(), line.data());
    line = "Graphics.Width 768";
    s.Write(line.length(), line.data());
    s.Rewind();
    
    hsIniFile file(s);
    
    std::shared_ptr<hsIniEntry> heightEntry = file.findByCommand("Graphics.Height");
    EXPECT_NE(heightEntry, nullptr);
    EXPECT_EQ(heightEntry->fType, hsIniEntry::kCommandValue);
    EXPECT_EQ(heightEntry->fCommand, "Graphics.Height");
    EXPECT_EQ(heightEntry->fValues, std::vector<ST::string>({"1024"}));
    
    std::shared_ptr<hsIniEntry> widthEntry = file.findByCommand("Graphics.Width");
    EXPECT_NE(widthEntry, nullptr);
    EXPECT_EQ(widthEntry->fType, hsIniEntry::kCommandValue);
    EXPECT_EQ(widthEntry->fCommand, "Graphics.Width");
    EXPECT_EQ(widthEntry->fValues, std::vector<ST::string>({"768"}));
    
    std::shared_ptr<hsIniEntry> notAnEntry = file.findByCommand("NotACommand");
    EXPECT_EQ(notAnEntry, nullptr);
}

TEST(hsIniHelper, entry_stream_mutate)
{
    hsRAMStream s;
    std::string line = "Graphics.Height 1024\n";
    s.Write(line.length(), line.data());
    s.Rewind();
    
    hsIniFile file(s);
    
    std::shared_ptr<hsIniEntry> heightEntry = file.findByCommand("Graphics.Height");
    EXPECT_NE(heightEntry, nullptr);
    EXPECT_EQ(heightEntry->fType, hsIniEntry::kCommandValue);
    EXPECT_EQ(heightEntry->fCommand, "Graphics.Height");
    EXPECT_EQ(heightEntry->fValues, std::vector<ST::string>({"1024"}));
    
    heightEntry->setValue(0, "2048");
    
    heightEntry = file.findByCommand("Graphics.Height");
    EXPECT_EQ(heightEntry->fValues, std::vector<ST::string>({"2048"}));
}
