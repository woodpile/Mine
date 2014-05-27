#ifndef __M_GAMESCENE_H__
#define __M_GAMESCENE_H__

#include "cocos2d.h"

class GameScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    virtual void onEnter();
    virtual void onExit();

    void menuCloseCallback(cocos2d::Ref* pSender);

    virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    virtual void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);

    CREATE_FUNC(GameScene);

private:
    void createBoxes(void);

    cocos2d::Sprite* _backgroud;
    cocos2d::Point _startPos;

    std::vector<std::vector<unsigned int> > _vecMatrix;
};

#endif // __M_GAMESCENE_H__
