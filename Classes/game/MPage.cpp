#include "MPage.h"
#include "PageSelectScene.h"
#include "../config/Config.h"

USING_NS_CC;

MPage::MPage(void)
: _page_w(0), _page_h(0), _hasown(false), _open_precent(0.0), _label_precent(nullptr)
{
    return;
}

MPage::~MPage(void)
{
    return;
}

//创建默认格子
MPage* MPage::create(void)
{
    auto *pPage = new MPage();
    auto pSpriteText = Director::getInstance()->getTextureCache()->getTextureForKey("blue.png");
    if (nullptr == pSpriteText)
    {
        pPage->initWithFile("blue.png");
    }
    else
    {
        pPage->initWithTexture(pSpriteText);
    }
    pPage->autorelease();

    return pPage;
}

//引擎 加载格子时调用的方法 初始化
void MPage::onEnter(void)
{
    //引擎
    Sprite::onEnter();

    //创建百分比显示标签
    _label_precent = Label::createWithTTF("0%", CF_F("font_hei"), 60);
    _label_precent->setColor(Color3B::WHITE);
    _label_precent->setPosition(this->getContentSize().width / 2, this->getContentSize().height / 2);
    this->addChild(_label_precent);
    
    //触摸监听
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    //引擎 向触摸监听注册响应函数
    listener->onTouchBegan = CC_CALLBACK_2(MPage::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(MPage::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(MPage::onTouchEnded, this);
    //激活触摸监听
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    return;
}

//引擎 移除格子时调用的方法 去初始化
void MPage::onExit(void)
{
    //引擎
    Sprite::onExit();

    return;
}

//引擎 触摸行为按下
bool MPage::onTouchBegan(Touch* touch, Event* event)
{
    //检查触摸点是否处于本格子的范围内
    Point point = this->convertTouchToNodeSpaceAR(touch);
    auto s = this->getContentSize();
    bool ret = Rect(-s.width / 2, -s.height / 2, s.width, s.height).containsPoint(point);
    //触摸生效的处理
    if (true == ret)
    {
        //提高格子的Z向高度
        this->setLocalZOrder(1000);
    }

    return ret;
}
//引擎 触摸行为移动
void MPage::onTouchMoved(Touch* touch, Event* event)
{
    return;
}
//引擎 触摸行为松开
void MPage::onTouchEnded(Touch* touch, Event* event)
{
    //检查触摸点是否处于本格子的范围内
    Point point = this->convertTouchToNodeSpaceAR(touch);
    auto s = this->getContentSize();
    bool ret = Rect(-s.width / 2, -s.height / 2, s.width, s.height).containsPoint(point);

    if (true == ret)
    {
        //复原格子的Z向高度
        this->setLocalZOrder(1);
        PageSelectScene::getInstance()->clickSelectPage(_page_w, _page_h);
    }

    return;
}


//设置页面的逻辑坐标
void MPage::setPagePos(int w, int h)
{
    _page_w = w;
    _page_h = h;
}

//查询页面是否已经有人占用
bool MPage::getPageOwn(void)
{
    return _hasown;
}

//设置页面是否已经有人占用
void MPage::setPageOwn(bool hasown)
{
    if (hasown == _hasown)
    {
        return;
    }

    _hasown = hasown;

    if (true == hasown)
    {
        _label_precent->setColor(Color3B::RED);
    }
    else
    {
        _label_precent->setColor(Color3B::WHITE);
    }
}

//设置页面的进度百分比
void MPage::setOpenPercent(double p)
{
    if (p == _open_precent)
    {
        return;
    }

    _open_precent = p;

    auto pn = String::createWithFormat("%d%%", (int)p);
    _label_precent->setString(pn->getCString());
}
