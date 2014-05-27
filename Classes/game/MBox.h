#ifndef __M_BOX_H__
#define __M_BOX_H__

#include "cocos2d.h"


class MBox : public cocos2d::Sprite
{
public:
    MBox();
    ~MBox();

    static MBox* create(void);
    static MBox* createWithTexture(cocos2d::Texture2D *pTexture);
    virtual void onEnter(void);
    virtual void onExit(void);

    virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    virtual void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);

private:
    void slideCover(cocos2d::Touch* touch);
    void drawPath(cocos2d::Touch* touch);
    void showTag(bool show);
    void slideCoverBackCallback(cocos2d::Node* sender);

    void coverClick(void);
    void coverDrag(cocos2d::Touch* touch);
    void coverDel(void);
    void coverToBigger(void);
    void coverToNoBigger(void);
    void coverSlideBack(void);
    void matchTune(void);

    UINT _box_x;
    UINT _box_y;

    cocos2d::Sprite* _tag;

    cocos2d::Sprite* _cover;
    cocos2d::Point _basePos;
    cocos2d::Point _moveStartPos;
    char _tune[100];

    bool _bigger;
};

#endif // __M_BOX_H__
