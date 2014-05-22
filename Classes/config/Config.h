#ifndef __M_CONFIG_H__
#define __M_CONFIG_H__

#include "cocos2d.h"

#define CF_F(str) Config::getFilename(str)
//#define CF_S(str) Config::getString(str)

class Config
{
public:
	Config();
	~Config();

	static bool init();

	static std::string getFilename(std::string strIndex);
    //static std::string getString(std::string strIndex);
    static int getFont(std::string strIndex);
    //static float getTimer(std::string strIndex);
    //static float getPos(std::string strIndex);

private:
    static bool configFilePath();
    static bool loadFilename();

    static std::map<std::string, std::string> _mFiles;
    static std::map<std::string, std::string> _mStrings;
    static std::map<std::string, int> _mFontSize;
    static std::map<std::string, float> _mTimer;
    static std::map<std::string, float> _mPos;
};

#endif // __M_CONFIG_H__
