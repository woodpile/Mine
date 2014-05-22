#include "Config.h"

USING_NS_CC;
using namespace std;

map<string, string> Config::_mFiles;
map<string, string> Config::_mStrings;
map<string, int> Config::_mFontSize;
map<string, float> Config::_mTimer;
map<string, float> Config::_mPos;

Config::Config()
{
}

Config::~Config()
{
}

bool Config::init()
{
    FileUtils::getInstance()->addSearchResolutionsOrder("config");

    Config::configFilePath();
    Config::loadFilename();

    return true;
}

bool Config::configFilePath()
{
    ValueVector txt_vec = FileUtils::getInstance()->getValueVectorFromFile("path.xml");
    for (auto& value : txt_vec)
    {
        FileUtils::getInstance()->addSearchResolutionsOrder(value.asString());
        log("add path to FileUtils : %s.", value.asString());
    }

    return true;
}

bool Config::loadFilename()
{
    ValueVector txt_vec = FileUtils::getInstance()->getValueVectorFromFile("filename.xml");
    for (auto& value : txt_vec)
    {
        auto text_map = value.asValueMap();
        for (auto& text_map_pair : text_map)
        {
            Config::_mFiles[text_map_pair.first] = text_map_pair.second.asString();
            log("load filename : key %s, string %s.", text_map_pair.first.c_str(), text_map_pair.second.asString().c_str());
        }
    }

    return true;
}

string Config::getFilename(string strIndex)
{
    return Config::_mFiles[strIndex];
}
