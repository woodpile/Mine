#ifndef __GAMEMENU_SCENE_H__
#define __GAMEMENU_SCENE_H__

#include "cocos2d.h"

class GameMenu : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();

    void menuCloseCallback(cocos2d::Ref* pSender);
    void menuGame(cocos2d::Ref* pSender);

    CREATE_FUNC(GameMenu);
};

#endif // __GAMEMENU_SCENE_H__
