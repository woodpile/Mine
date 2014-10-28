//
//  HelpBoard.h
//  Mine
//
//  Created by woodpile on 14-8-25.
//
//

#ifndef __M_HelpBoard__
#define __M_HelpBoard__

#include "cocos2d.h"

//帮助面板类
class HelpBoard : public cocos2d::Sprite
{
public:
    HelpBoard();
    ~HelpBoard();
    
    //创建格子
    static HelpBoard* create(void);
    static HelpBoard* createNet(void);
    
    //引擎 初始化与去初始化
    virtual void onEnter(void);
    virtual void onExit(void);
    //引擎 触摸行为注册函数
    virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    virtual void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
    
    //设置面板能否移动
    void setBoardCanMove(bool bCanMove);
    
private:
    //成员对象 面板的基础位置
    cocos2d::Point _basePos;
    //成员数据 能否移动
    bool _bCanMove;
    //成员数据 是否处于打开状态
    bool _bOpen;
};

#endif /* defined(__M_HelpBoard__) */
