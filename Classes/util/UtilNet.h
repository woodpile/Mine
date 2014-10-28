//
//  UtilNet.h
//  Mine
//
//  Created by woodpile on 14-8-26.
//
//

#ifndef __M_UtilNet__
#define __M_UtilNet__

#include "cocos2d.h"
#include "HttpClient.h"

class UtilNet
{
public:
    typedef std::function<bool(const char*)> recvCallback;
    static bool regRecvList(void);

    static bool sendUserLogin(void);
    static bool sendShowPages(void);
    static bool sendSelectPage(int w, int h);
    static bool sendReleasePage(void);
    static bool sendLoadSubPage(int pw, int ph, int w, int h);
    static bool sendBoxClick(int pw, int ph, int w, int h);
    static bool sendBoxFlag(int pw, int ph, int w, int h);
private:
    static bool recvUserLogin(const char* pRes);
    static bool recvShowPages(const char* pRes);
    static bool recvSelectPage(const char* pRes);
    static bool recvReleasePage(const char* pRes);
    static bool recvLoadSubPage(const char* pRes);
    static bool recvBoxClick(const char* pRes);
    static bool recvBoxFlag(const char* pRes);
    
    static bool cryptoData(int len, const unsigned char* pIn, unsigned char* pOut);
    static bool sendData(const char* pMethod, const char* pData, const char* pTag);
    static bool sendMsg(const char* pMsg, int len, const char* pTag);
    
    static void recvMsg(cocos2d::network::HttpClient* c, cocos2d::network::HttpResponse* r);
    static void recvData(cocos2d::network::HttpResponse* r);
};

class UtilNetRecv : public cocos2d::Ref
{
public:
    static UtilNetRecv* create(const UtilNet::recvCallback& h);
    UtilNet::recvCallback _h;
};

#endif /* defined(__M_UtilNet__) */
