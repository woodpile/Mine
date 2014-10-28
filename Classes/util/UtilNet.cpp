//
//  UtilNet.cpp
//  Mine
//
//  Created by woodpile on 14-8-26.
//
//

#include "UtilNet.h"
#include "json/rapidjson.h"
#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"
#include "UtilRc4.h"
#include "../game/GameSceneNet.h"
#include "../game/PageSelectScene.h"

USING_NS_CC;
using namespace cocos2d::network;

std::string _g_username = "";

std::string g_strHost = "http://114.215.193.215:9991/mine";

bool UtilNet::sendUserLogin(void)
{
    _g_username = "";
    return UtilNet::sendData(".UserLogin", "", "userlogin");
}

bool UtilNet::recvUserLogin(const char* pRes)
{
    rapidjson::Document da;
    da.Parse<rapidjson::kParseDefaultFlags>(pRes);
    if (false == da.HasMember("User"))
    {
        return false;
    }
    if (false == da["User"].IsString())
    {
        log("recv type wrong");
        return false;
    }
    log("recv type correct");

    _g_username = std::string(da["User"].GetString());
    
    PageSelectScene::getInstance()->callbackUserLogin();
    return true;
}

bool decodeNetPageAttr(rapidjson::Document::ValueIterator it, NetPageAttr* outAttr)
{
    if (false == it->IsObject())
    {
        log("recv refresh sub type wrong");
        return false;
    }
    if (false == it->HasMember("Width") ||
        false == it->HasMember("Height") ||
        false == it->HasMember("HasOwn") ||
        false == it->HasMember("OpenPercent"))
    {
        log("recv refresh sub type member wrong");
        return false;
    }
    outAttr->width = (*it)["Width"].GetInt();
    outAttr->hight = (*it)["Height"].GetInt();
    outAttr->hasown = (*it)["HasOwn"].GetBool();
    outAttr->openprecent = (*it)["OpenPercent"].GetDouble();

    return true;
}

bool UtilNet::sendShowPages(void)
{
    return UtilNet::sendData("Matrix.ShowPages", "", "showpages");
}

bool UtilNet::recvShowPages(const char* pRes)
{    rapidjson::Document da;
    da.Parse<rapidjson::kParseDefaultFlags>(pRes);
    if (false == da.HasMember("MapW") ||
        false == da.HasMember("MapH") ||
        false == da.HasMember("Pages"))
    {
        return false;
    }
    if (false == da["MapW"].IsInt() ||
        false == da["MapH"].IsInt() ||
        false == da["Pages"].IsArray())
    {
        log("recv type wrong");
        return false;
    }
    log("recv type correct");

    int mapw = da["MapW"].GetInt();
    int maph = da["MapH"].GetInt();
    int size = da["Pages"].Size();
    if (0 == size)
    {
        PageSelectScene::getInstance()->callbackShowPages(mapw, maph);
        return true;
    }

    NetPageAttr pagearr[size];
    int index = 0;
    for (auto it = da["Pages"].onBegin(); it != da["Pages"].onEnd(); it++)
    {
        if (false == decodeNetPageAttr(it, &(pagearr[index])))
        {
            continue;
            size -= 1;
        }
        index++;
    }

    PageSelectScene::getInstance()->callbackShowPages(mapw, maph, pagearr, size);

    return true;
}

bool UtilNet::sendSelectPage(int w, int h)
{
    rapidjson::Document d;
    d.SetObject();
    rapidjson::Document::AllocatorType& al = d.GetAllocator();
    d.AddMember("PageW", w, al);
    d.AddMember("PageH", h, al);
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> wt(sb);
    d.Accept(wt);

    return UtilNet::sendData("Matrix.SelcetPage", sb.GetString(), "selectpage");
}

bool UtilNet::recvSelectPage(const char* pRes)
{
    rapidjson::Document da;
    da.Parse<rapidjson::kParseDefaultFlags>(pRes);
    if (false == da.HasMember("Ret") ||
        false == da.HasMember("MaxW") ||
        false == da.HasMember("MaxH"))
    {
        return false;
    }
    if (false == da["Ret"].IsBool() ||
        false == da["MaxW"].IsInt() ||
        false == da["MaxH"].IsInt())
    {
        log("recv type wrong");
        return false;
    }
    log("recv type correct");

    auto bret = da["Ret"].GetBool();
    auto maxw = da["MaxW"].GetInt();
    auto maxh = da["MaxH"].GetInt();

    PageSelectScene::getInstance()->callbackSelectPage(bret, maxw, maxh);

    return true;
}

bool UtilNet::sendReleasePage(void)
{
    return UtilNet::sendData("Matrix.ReleasePage", "", "releaspage");
}

bool UtilNet::recvReleasePage(const char* pRes)
{
    return true;
}

bool decodeNetBoxAttr(rapidjson::Document::ValueIterator it, NetBoxAttr* outAttr)
{
    if (false == it->IsObject())
    {
        log("recv refresh sub type wrong");
        return false;
    }
    if (false == it->HasMember("Width") ||
        false == it->HasMember("Height") ||
        false == it->HasMember("Num") ||
        false == it->HasMember("IsOpen") ||
        false == it->HasMember("Bomb") ||
        false == it->HasMember("IsFlag"))
    {
        log("recv refresh sub type member wrong");
        return false;
    }
    outAttr->width = (*it)["Width"].GetInt();
    outAttr->hight = (*it)["Height"].GetInt();
    outAttr->num = (*it)["Num"].GetInt();
    outAttr->isopen = (*it)["IsOpen"].GetBool();
    outAttr->bomb = (*it)["Bomb"].GetInt();
    outAttr->isflag = (*it)["IsFlag"].GetBool();
    return true;
}

bool UtilNet::sendBoxClick(int pw, int ph, int w, int h)
{
    rapidjson::Document d;
    d.SetObject();
    rapidjson::Document::AllocatorType& al = d.GetAllocator();
    d.AddMember("PageW", pw, al);
    d.AddMember("PageH", ph, al);
    d.AddMember("width", w, al);
    d.AddMember("height", h, al);
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> wt(sb);
    d.Accept(wt);
    
    return UtilNet::sendData("Matrix.ClickBox", sb.GetString(), "clickbox");
}

bool UtilNet::recvBoxClick(const char* pRes)
{
    rapidjson::Document da;
    da.Parse<rapidjson::kParseDefaultFlags>(pRes);
    if (false == da.HasMember("Num") ||
        false == da.HasMember("Bomb") ||
        false == da.HasMember("Refresh"))
    {
        return false;
    }
    if (false == da["Num"].IsInt() ||
        false == da["Bomb"].IsInt() ||
        false == da["Refresh"].IsArray())
    {
        log("recv type wrong");
        return false;
    }
    log("recv type correct");

    int num = da["Num"].GetInt();
    int bomb = da["Bomb"].GetInt();
    int size = da["Refresh"].Size();
    if (0 == size)
    {
        GameSceneNet::getInstance()->callbackClickBox(num, bomb);
        return true;
    }

    NetBoxAttr boxarr[size];
    int index = 0;
    for (auto it = da["Refresh"].onBegin(); it != da["Refresh"].onEnd(); it++)
    {
        if (false == decodeNetBoxAttr(it, &(boxarr[index])))
        {
            continue;
            size -= 1;
        }
        index++;
    }

    GameSceneNet::getInstance()->callbackClickBox(num, bomb, boxarr, size);
    
    return true;
}

bool UtilNet::sendBoxFlag(int pw, int ph, int w, int h)
{
    rapidjson::Document d;
    d.SetObject();
    rapidjson::Document::AllocatorType& al = d.GetAllocator();
    d.AddMember("PageW", pw, al);
    d.AddMember("PageH", ph, al);
    d.AddMember("width", w, al);
    d.AddMember("height", h, al);
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> wt(sb);
    d.Accept(wt);

    return UtilNet::sendData("Matrix.FlagBox", sb.GetString(), "flagbox");
}

bool UtilNet::recvBoxFlag(const char* pRes)
{

    rapidjson::Document da;
    da.Parse<rapidjson::kParseDefaultFlags>(pRes);
    if (false == da.HasMember("Num") ||
        false == da.HasMember("Bomb") ||
        false == da.HasMember("Refresh"))
    {
        return false;
    }
    if (false == da["Num"].IsInt() ||
        false == da["Bomb"].IsInt() ||
        false == da["Refresh"].IsArray())
    {
        log("recv type wrong");
        return false;
    }
    log("recv type correct");

    int num = da["Num"].GetInt();
    int bomb = da["Bomb"].GetInt();
    int size = da["Refresh"].Size();
    if (0 == size)
    {
        GameSceneNet::getInstance()->callbackFlagBox(num, bomb);
        return true;
    }

    NetBoxAttr boxarr[size];
    int index = 0;
    for (auto it = da["Refresh"].onBegin(); it != da["Refresh"].onEnd(); it++)
    {
        if (false == decodeNetBoxAttr(it, &(boxarr[index])))
        {
            continue;
            size -= 1;
        }
        index++;
    }

    GameSceneNet::getInstance()->callbackFlagBox(num, bomb, boxarr, size);

    return true;
}

bool UtilNet::sendLoadSubPage(int pw, int ph, int w, int h)
{
    rapidjson::Document d;
    d.SetObject();
    rapidjson::Document::AllocatorType& al = d.GetAllocator();
    d.AddMember("PageW", pw, al);
    d.AddMember("PageH", ph, al);
    d.AddMember("width", w, al);
    d.AddMember("height", h, al);
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> wt(sb);
    d.Accept(wt);
    
    return UtilNet::sendData("Matrix.LoadSubPage", sb.GetString(), "loadsubpage");
}

bool UtilNet::recvLoadSubPage(const char* pRes)
{
    rapidjson::Document da;
    da.Parse<rapidjson::kParseDefaultFlags>(pRes);
    
    if (false == da.HasMember("PageW") ||
        false == da.HasMember("PageH") ||
        false == da.HasMember("Refresh"))
    {
        return false;
    }
    if (false == da["PageW"].IsInt() ||
        false == da["PageH"].IsInt() ||
        false == da["Refresh"].IsArray())
    {
        log("recv type wrong");
        return false;
    }
    log("recv type correct");
    
    int pw = da["PageW"].GetInt();
    int ph = da["PageH"].GetInt();
    int size = da["Refresh"].Size();
    NetBoxAttr boxarr[size];
    int index = 0;
    for (auto it = da["Refresh"].onBegin(); it != da["Refresh"].onEnd(); it++)
    {
        if (false == decodeNetBoxAttr(it, &(boxarr[index])))
        {
            continue;
            size -= 1;
        }
        index++;
    }
    
    GameSceneNet::getInstance()->updateBoxs(pw, ph, boxarr, size);
    
    return true;
}

//---------Recv Registe
UtilNetRecv* UtilNetRecv::create(const UtilNet::recvCallback& h)
{
    auto p = new UtilNetRecv();
    p->_h = h;
    return p;
}

Map<std::string, UtilNetRecv*> pMapRecv;

bool UtilNet::regRecvList(void)
{
    pMapRecv.insert("userlogin", UtilNetRecv::create(UtilNet::recvUserLogin));
    pMapRecv.insert("showpages", UtilNetRecv::create(UtilNet::recvShowPages));
    pMapRecv.insert("selectpage", UtilNetRecv::create(UtilNet::recvSelectPage));
    pMapRecv.insert("releaspage", UtilNetRecv::create(UtilNet::recvReleasePage));
    pMapRecv.insert("clickbox", UtilNetRecv::create(UtilNet::recvBoxClick));
    pMapRecv.insert("flagbox", UtilNetRecv::create(UtilNet::recvBoxFlag));
    pMapRecv.insert("loadsubpage", UtilNetRecv::create(UtilNet::recvLoadSubPage));
    return true;
}

//---------Net Framework

bool UtilNet::sendData(const char* pMethod, const char* pData, const char* pTag)
{
    rapidjson::Document d;
    d.SetObject();
    rapidjson::Document::AllocatorType& al = d.GetAllocator();
    log("send user %s", _g_username.c_str());
    d.AddMember("user", _g_username.c_str(), al);
    d.AddMember("time", "20140827100000", al);
    d.AddMember("method", pMethod, al);
    d.AddMember("data", pData, al);
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> wt(sb);
    d.Accept(wt);
    
    return UtilNet::sendMsg(sb.GetString(), (int)strlen(sb.GetString()), pTag);
}

bool UtilNet::sendMsg(const char* pMsg, int len, const char* pTag)
{
    char pCryMsg[len + 1];
    memset(pCryMsg, 0, len + 1);
    UtilNet::cryptoData(len, (unsigned char*)pMsg, (unsigned char*)pCryMsg);
    
    HttpRequest* r = new HttpRequest();
    r->setUrl(g_strHost.c_str());
    r->setRequestType(HttpRequest::Type::POST);
    
    r->setRequestData(pCryMsg, len);
    r->setTag(pTag);
    
    r->setResponseCallback(UtilNet::recvMsg);
    
    HttpClient::getInstance()->send(r);
    
    return true;
}

bool UtilNet::cryptoData(int len, const unsigned char* pIn, unsigned char* pOut)
{
    unsigned char k[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    int kl = 8;
	mineRC4_KEY key;
    mineRC4_set_key(&key, kl, k);
    mineRC4(&key, len, pIn, (unsigned char*)pOut);
    
    return true;
}

void UtilNet::recvMsg(HttpClient* c, HttpResponse* r)
{
    if (false == r->isSucceed())
    {
        log("error buffer: %s", r->getErrorBuffer());
        return;
    }
    
    log("response tag %s", r->getHttpRequest()->getTag());
    
    UtilNet::recvData(r);
    
    r->getHttpRequest()->release();
    
    return;
}

void UtilNet::recvData(cocos2d::network::HttpResponse* r)
{
    auto rData = r->getResponseData();
    int len = (int)rData->size();
    if (0 == len)
    {
        return;
    }
    
    char acBuf[len + 1];
    memset(acBuf, 0, len + 1);
    
    UtilNet::cryptoData(len, (unsigned char*)rData->data(), (unsigned char*)acBuf);
    
    log("recv %s", acBuf);
    
    rapidjson::Document d;
    d.Parse<rapidjson::kParseDefaultFlags>(acBuf);
    for (auto it = d.MemberonBegin(); it != d.MemberonEnd(); it++)
    {
        log("key %s, value %s", it->name.GetString(), it->value.GetString());
    }
    
    if (false == d.HasMember("Err") ||
        false == d.HasMember("Time") ||
        false == d.HasMember("Data"))
    {
        log("recv invalid msg : %s", d.GetString());
        return;
    }
    
    auto err = __String::create(d["Err"].GetString());
    if (0 != err->compare("ok"))
    {
        log("return error : %s", err->getCString());
        return;
    }
    
    auto c = pMapRecv.at(r->getHttpRequest()->getTag());
    if (nullptr != c)
    {
        c->_h(d["Data"].GetString());
    }
    
    return;
}
