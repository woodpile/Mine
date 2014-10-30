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
#include "ErrNet.h"
#include "../game/GameSceneNet.h"
#include "../game/PageSelectScene.h"

USING_NS_CC;
using namespace cocos2d::network;

std::string _g_username = "";

//std::string g_strHost = "http://114.215.193.215:9991/mine";
std::string g_strHost = "http://127.0.0.1:9991/mine";

bool UtilNet::dispatchErrCode(int errcode)
{
    if (errcode == ERR_NET_NOLOGIN) {
        _g_username = "";
        auto p = GameSceneNet::getInstance();
        if (nullptr != p)
        {
            p->callbackErrNoLogin();
        }
    }

    return true;
}

bool UtilNet::sendUserLogin(void)
{
    return UtilNet::sendData(".UserLogin", "", "userlogin");
}

bool UtilNet::recvUserLogin(int errcode, const char* pRes)
{
    if (ERR_NET_NOERR != errcode)
    {
        _g_username = "";
        return false;
    }
    
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

    auto p = PageSelectScene::getInstance();
    if (nullptr != p)
    {
        p->callbackUserLogin();
    }
    return true;
}

bool UtilNet::sendGetMapInfo(void)
{
    return UtilNet::sendData("Matrix.GetMapInfo", "", "getmapinfo");
}

bool UtilNet::recvGetMapInfo(int errcode, const char* pRes)
{
    if (ERR_NET_NOERR != errcode)
    {
        return false;
    }

    rapidjson::Document da;
    da.Parse<rapidjson::kParseDefaultFlags>(pRes);
    if (false == da.HasMember("MapW") ||
        false == da.HasMember("MapH"))
    {
        return false;
    }
    if (false == da["MapW"].IsInt() ||
        false == da["MapH"].IsInt())
    {
        log("recv type wrong");
        return false;
    }
    log("recv type correct");

    int mapw = da["MapW"].GetInt();
    int maph = da["MapH"].GetInt();
    auto p = PageSelectScene::getInstance();
    if (nullptr != p)
    {
        p->callbackGetMapInfo(mapw, maph);
    }
    
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

bool UtilNet::sendShowPages(int w, int h)
{
    rapidjson::Document d;
    d.SetObject();
    rapidjson::Document::AllocatorType& al = d.GetAllocator();
    d.AddMember("Width", w, al);
    d.AddMember("Height", h, al);
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> wt(sb);
    d.Accept(wt);

    return UtilNet::sendData("Matrix.ShowPages", sb.GetString(), "showpages");
}

bool UtilNet::recvShowPages(int errcode, const char* pRes)
{
    if (ERR_NET_NOERR != errcode)
    {
        return false;
    }

    rapidjson::Document da;
    da.Parse<rapidjson::kParseDefaultFlags>(pRes);
    if (false == da.HasMember("Pages"))
    {
        return false;
    }
    if (false == da["Pages"].IsArray())
    {
        log("recv type wrong");
        return false;
    }
    log("recv type correct");

    int size = da["Pages"].Size();

    NetPageAttr pagearr[size];

    if (0 != size)
    {
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
    }

    auto p = PageSelectScene::getInstance();
    if (nullptr != p)
    {
        p->callbackShowPages(pagearr, size);
    }

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

bool UtilNet::recvSelectPage(int errcode, const char* pRes)
{
    if (ERR_NET_NOERR != errcode)
    {
        return false;
    }

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

    auto p = PageSelectScene::getInstance();
    if (nullptr != p)
    {
        p->callbackSelectPage(bret, maxw, maxh);
    }

    return true;
}

bool UtilNet::sendReleasePage(void)
{
    return UtilNet::sendData("Matrix.ReleasePage", "", "releaspage");
}

bool UtilNet::recvReleasePage(int errcode, const char* pRes)
{
    if (ERR_NET_NOERR != errcode)
    {
        return false;
    }


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

bool UtilNet::recvBoxClick(int errcode, const char* pRes)
{
    if (ERR_NET_NOERR != errcode)
    {
        return UtilNet::dispatchErrCode(errcode);
    }

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

    NetBoxAttr boxarr[size];
    if (0 != size)
    {
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
    }

    auto p = GameSceneNet::getInstance();
    if (nullptr != p)
    {
        p->callbackClickBox(num, bomb, boxarr, size);
    }
    
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

bool UtilNet::recvBoxFlag(int errcode, const char* pRes)
{
    if (ERR_NET_NOERR != errcode)
    {
        return UtilNet::dispatchErrCode(errcode);
    }

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

    NetBoxAttr boxarr[size];
    if (0 != size)
    {
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
    }

    auto p = GameSceneNet::getInstance();
    if (nullptr != p)
    {
        p->callbackFlagBox(num, bomb, boxarr, size);
    }

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

bool UtilNet::recvLoadSubPage(int errcode, const char* pRes)
{
    if (ERR_NET_NOERR != errcode)
    {
        return UtilNet::dispatchErrCode(errcode);
    }

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

    auto p = GameSceneNet::getInstance();
    if (nullptr != p)
    {
        p->updateBoxs(pw, ph, boxarr, size);
    }
    
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
    pMapRecv.insert("getmapinfo", UtilNetRecv::create(UtilNet::recvGetMapInfo));
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
        UtilNet::connectErr(r);
    }
    else{
        log("response tag %s", r->getHttpRequest()->getTag());
        
        UtilNet::recvData(r);
    }
    
    r->getHttpRequest()->release();
    
    return;
}

void UtilNet::recvData(HttpResponse* r)
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
    
//    log("recv(%d) %s", len, acBuf);

    rapidjson::Document d;
    d.Parse<rapidjson::kParseDefaultFlags>(acBuf);
    for (auto it = d.MemberonBegin(); it != d.MemberonEnd(); it++)
    {
        if (true == it->value.IsString())
        {
            log("key %s, value %s", it->name.GetString(), it->value.GetString());
        }
        else if (true == it->value.IsInt())
        {
            log("key %s, value %d", it->name.GetString(), it->value.GetInt());
        }
    }
    
    if (false == d.HasMember("Err") ||
        false == d.HasMember("Time") ||
        false == d.HasMember("Data") ||
        false == d.HasMember("ErrCode"))
    {
        log("recv invalid msg : %s", d.GetString());
        return;
    }
    
    auto err = __String::create(d["Err"].GetString());
    if (0 != err->compare("ok"))
    {
        log("return error(code %d) : %s", d["ErrCode"].GetInt(), err->getCString());
    }
    
    auto c = pMapRecv.at(r->getHttpRequest()->getTag());
    if (nullptr != c)
    {
        c->_h(d["ErrCode"].GetInt(), d["Data"].GetString());
    }
    
    return;
}

void UtilNet::connectErr(HttpResponse* r)
{
    if (nullptr != GameSceneNet::getInstance())
    {
        GameSceneNet::getInstance()->callbackErrConnection();
    }
    else if (nullptr != PageSelectScene::getInstance())
    {
        PageSelectScene::getInstance()->callbackErrConnection();
    }
}
