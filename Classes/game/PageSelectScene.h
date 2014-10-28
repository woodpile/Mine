//
//  GameSceneNet.h
//  Mine
//
//  Created by woodpile on 14-8-25.
//
//

#ifndef __M_PageSelectScene__
#define __M_PageSelectScene__

#include "cocos2d.h"

struct NetPageAttr
{
    int width;
    int hight;
    bool hasown;
    double openprecent;
};

class PageSelectScene: public cocos2d::Layer
{
public:
    PageSelectScene();
    ~PageSelectScene();

    //生成游戏场景
    static cocos2d::Scene* createScene();
    //获取游戏场景实例
    static PageSelectScene* getInstance();

    //引擎
    CREATE_FUNC(PageSelectScene);
    
    //引擎 初始化和去初始化
    virtual bool init();
    virtual void onEnter();
    virtual void onExit();

    //退出菜单键的回调函数
    void menuCloseCallback(cocos2d::Ref* pSender);
    //重新开始菜单键的回调函数
    void menuRestartCallback(cocos2d::Ref* pSender);

    //页面格子触发选择页面
    void clickSelectPage(int w, int h);

    //返回登录信息
    void callbackUserLogin(void);
    //展示格子的返回信息
    void callbackShowPages(int mapw, int maph, NetPageAttr apages[] = nullptr, int arrlen = 0);
    //返回页面选择信息
    void callbackSelectPage(bool ret, int maxw, int maxh);
//protected:

private:
    const int BASE_PAGE_ID = 1000;

    static PageSelectScene* _pPageSelectInstance;

    //加载游戏环境
    void loadBaseGround(void);

    //定时发送显示当前页面信息的定时器函数
    void timeShowPages(float dt);

    //更新页面信息
    void updatePages(NetPageAttr apages[], int arrlen);

    //选中动画后的回调函数
    void selectActionCallback(cocos2d::Node* sender);

    //成员对象 是否已经登录
    bool _bLogin;
    //成员对象 选择的页面坐标
    int _page_w, _page_h;
    //成员对象 选择的页面内的最大格子坐标
    int _box_max_w, _box_max_h;

    //成员对象 总页数的长宽
    int _map_max_w;
    int _map_max_h;
    //成员对象 页面格子的排布起始坐标,即页面0,0的起始坐标,相对于wheel
    cocos2d::Point _page_start;

    //成员对象 承载页面格子的底板
    cocos2d::Sprite* _wheel;
    //成员对象 底板的初始坐标
    cocos2d::Point _wheel_base_pos;
    
    //成员对象 背景精灵
    cocos2d::Sprite* _backgroud;
};


#endif /* defined(__M_PageSelectScene__) */
