#ifndef __GAMEMENU_SCENE_H__
#define __GAMEMENU_SCENE_H__

#include "cocos2d.h"

//游戏主菜单类
class GameMenu : public cocos2d::Layer
{
public:
    //创建主菜单场景
    static cocos2d::Scene* createScene();
    
    //引擎 初始化
    virtual bool init();
    
    //菜单回调函数
    void menuCloseCallback(cocos2d::Ref* pSender);
    void menuGame(cocos2d::Ref* pSender);
    void menuNetGame(cocos2d::Ref* pSender);

    //引擎
    CREATE_FUNC(GameMenu);
};

#endif // __GAMEMENU_SCENE_H__
