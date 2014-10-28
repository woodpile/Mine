//
//  HelpBoard.cpp
//  Mine
//
//  Created by woodpile on 14-8-25.
//
//

#include "HelpBoard.h"

USING_NS_CC;

HelpBoard::HelpBoard(void)
: _basePos(Point(0, 0)), _bCanMove(false), _bOpen(false)
{
}

HelpBoard::~HelpBoard(void)
{
}

//创建帮助面板
HelpBoard* HelpBoard::create(void)
{
    auto *pBoard = new HelpBoard();
    pBoard->initWithFile("Help_back.png");
    pBoard->autorelease();
    
    return pBoard;
}
//创建帮助面板网络版
HelpBoard* HelpBoard::createNet(void)
{
    auto *pBoard = new HelpBoard();
    pBoard->initWithFile("Help_back_net.png");
    pBoard->autorelease();

    return pBoard;
}

//引擎 加载面板时调用的方法 初始化
void HelpBoard::onEnter(void)
{
    //引擎
    Sprite::onEnter();
    
    if (false == _bCanMove)
    {
        return;
    }
    
    //触摸监听
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    //引擎 向触摸监听注册响应函数
    listener->onTouchBegan = CC_CALLBACK_2(HelpBoard::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(HelpBoard::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(HelpBoard::onTouchEnded, this);
    //激活触摸监听
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    //记录面板的基本位置
    _basePos = this->getPosition();
    
    return;
}

//引擎 移除面板时调用的方法 去初始化
void HelpBoard::onExit(void)
{
    //引擎
    Sprite::onExit();
    
    return;
}

//引擎 触摸行为按下
bool HelpBoard::onTouchBegan(Touch* touch, Event* event)
{
    //检查触摸点是否处于本格子的范围内
    auto point = this->convertTouchToNodeSpaceAR(touch);
    auto s = this->getContentSize();
    bool ret = Rect(-s.width / 2, -s.height / 2, s.width, s.height).containsPoint(point);
    
    return ret;
}
//引擎 触摸行为移动
void HelpBoard::onTouchMoved(Touch* touch, Event* event)
{
    return;
}
//引擎 触摸行为松开
void HelpBoard::onTouchEnded(Touch* touch, Event* event)
{
    if (false == _bOpen)
    {
        auto ac = MoveTo::create(0.2f, Point(_basePos.x, _basePos.y + 142));
        this->runAction(ac);
        _bOpen = true;
    }
    else
    {
        auto ac = MoveTo::create(0.2f, _basePos);
        this->runAction(ac);
        _bOpen = false;
    }
    return;
}

//设置面板能否移动
void HelpBoard::setBoardCanMove(bool bCanMove)
{
    this->_bCanMove = bCanMove;
}
