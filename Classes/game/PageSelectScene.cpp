//
//  PageSelectScene.cpp
//  Mine
//
//  Created by woodpile on 14-8-25.
//
//

#include "PageSelectScene.h"
#include "MPage.h"
#include "GameSceneNet.h"
#include "../config/Config.h"
#include "../GameMenu.h"
#include "../util/UtilNet.h"

USING_NS_CC;

PageSelectScene* PageSelectScene::_pPageSelectInstance = nullptr;

PageSelectScene::PageSelectScene(void)
: _page_h(0), _page_w(0), _bLogin(false), _bWaitNet(false)
{
}

PageSelectScene::~PageSelectScene(void)
{
    _pPageSelectInstance = nullptr;
}

//创建承载本图层的场景
Scene* PageSelectScene::createScene()
{
    auto scene = Scene::create();

    auto layer = PageSelectScene::create();
    _pPageSelectInstance = layer;

    scene->addChild(layer);

    return scene;
}

PageSelectScene* PageSelectScene::getInstance()
{
    return PageSelectScene::_pPageSelectInstance;
}

//引擎 加载图层时调用的方法
void PageSelectScene::onEnter()
{
    //引擎
    Layer::onEnter();

    //触摸监听
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    //引擎 向触摸监听注册响应函数
    listener->onTouchBegan = CC_CALLBACK_2(PageSelectScene::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(PageSelectScene::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(PageSelectScene::onTouchEnded, this);
    //激活触摸监听
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    //发送登录游戏请求
    UtilNet::sendUserLogin();
    _bWaitNet = true;
}
//引擎 移除图层时调用的方法
void PageSelectScene::onExit()
{
    this->unscheduleAllSelectors();

    //引擎
    Layer::onExit();
}
//引擎 初始化
bool PageSelectScene::init()
{
    //引擎
    if ( !Layer::init() )
    {
        return false;
    }

    //取屏幕逻辑尺寸
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Point origin = Director::getInstance()->getVisibleOrigin();

    //创建背景
    _backgroud = Sprite::create("back1.png");
    _backgroud->setPosition(visibleSize.width / 2, visibleSize.height / 2 + origin.y);
    this->addChild(_backgroud);

    //创建退出菜单键
    auto pExitItem = MenuItemImage::create("Return.png", "Return_big.png",
                                           CC_CALLBACK_1(PageSelectScene::menuCloseCallback, this));
    auto exitSize = pExitItem->getContentSize();
    pExitItem->setPosition(0 - exitSize.width / 2, exitSize.height / 2);
    //创建重新开始菜单键
    auto pRestartItem = MenuItemImage::create("Restart.png", "Restart_big.png",
                                              CC_CALLBACK_1(PageSelectScene::menuRestartCallback, this));
    auto restartSize = pRestartItem->getContentSize();
    pRestartItem->setPosition(0 - exitSize.width - restartSize.width / 2, restartSize.height / 2);
    //创建菜单
    Menu* pItemMenu = Menu::create();
    pItemMenu->addChild(pExitItem);
    pItemMenu->addChild(pRestartItem);
    pItemMenu->setPosition(visibleSize.width + origin.x, origin.y);
    this->addChild(pItemMenu);

    //初始化有关资源
    Director::getInstance()->getTextureCache()->addImage("white.png");
    Director::getInstance()->getTextureCache()->addImage("blue.png");

    return true;
}
//退出菜单键的回调函数
void PageSelectScene::menuCloseCallback(Ref* pSender)
{
    //返回游戏主菜单
    Director::getInstance()->replaceScene(GameMenu::createScene());
}
//重新开始菜单键的回调函数
void PageSelectScene::menuRestartCallback(Ref* pSender)
{
}

//是否处于等待通信状态
bool PageSelectScene::isWaitNet(void)
{
    return _bWaitNet;
}

//引擎 触摸行为按下
bool PageSelectScene::onTouchBegan(Touch* touch, Event* event)
{
    //是否处于可滑动状态
    if (true == _bWaitNet)
    {
        return false;
    }

    return true;
}
//引擎 触摸行为移动
void PageSelectScene::onTouchMoved(Touch* touch, Event* event)
{
    //是否处于可滑动状态
    if (true == _bWaitNet)
    {
        return;
    }

    //滑动处理
    auto n_wheel = _wheel->getPosition() + (touch->getLocation() - touch->getPreviousLocation());
    if (n_wheel.x < _edge00.x)
    {
        n_wheel.x = _edge00.x;
    }
    else if (n_wheel.x > _edgeMM.x)
    {
        n_wheel.x = _edgeMM.x;
    }
    if (n_wheel.y < _edge00.y)
    {
        n_wheel.y = _edge00.y;
    }
    else if (n_wheel.y > _edgeMM.y)
    {
        n_wheel.y = _edgeMM.y;
    }

    _wheel->setPosition(n_wheel.x, n_wheel.y);

    this->refreshShowPages();

    return;
}
//引擎 触摸行为松开
void PageSelectScene::onTouchEnded(Touch* touch, Event* event)
{
    return;
}

//返回登录信息
void PageSelectScene::callbackUserLogin(void)
{
    _bLogin = true;
    _bWaitNet = false;
    this->loadBaseGround();
}

//加载游戏环境
void PageSelectScene::loadBaseGround(void)
{
    //创建承载页面格子的底板
    _wheel = Sprite::create();
    _wheel->setPosition(_backgroud->getContentSize().width / 2,
                        _backgroud->getContentSize().height / 2);
    _backgroud->addChild(_wheel);
    _wheel_base_pos = _wheel->getPosition();

    UtilNet::sendGetMapInfo();
}

//返回地图的基本信息
void PageSelectScene::callbackGetMapInfo(int mapw, int maph)
{
    auto pagesize = Director::getInstance()->getTextureCache()->getTextureForKey("blue.png")->getContentSize();

    _map_max_w = mapw;
    _map_max_h = maph;

    _center_w = (_map_max_w + 0) / 2;
    _center_h = (_map_max_h + 0) / 2;

    _page_start.x = 0 - ((pagesize.width * (_map_max_w + 1) / 2) - (pagesize.width / 2));
    _page_start.y = 0 - ((pagesize.height * (_map_max_h + 1) / 2) - (pagesize.height / 2));

    _edge00.x = _wheel_base_pos.x + _page_start.x + (2 * pagesize.width);
    _edge00.y = _wheel_base_pos.y + _page_start.y + (2 * pagesize.height);
    _edgeMM.x = _wheel_base_pos.x + _page_start.x + (_map_max_w - 2) * pagesize.width;
    _edgeMM.y = _wheel_base_pos.y + _page_start.y + (_map_max_h - 2) * pagesize.height;

    UtilNet::sendShowPages(_center_w, _center_h);
}

//展示格子的返回信息
void PageSelectScene::callbackShowPages(NetPageAttr apages[], int arrlen)
{
    _bWaitNet = false;

    this->updatePages(apages, arrlen);

    this->schedule(schedule_selector(PageSelectScene::timeShowPages), 5);
}

//定时发送显示当前页面信息的定时器函数
void PageSelectScene::timeShowPages(float dt)
{
    UtilNet::sendShowPages(_center_w, _center_h);
}

//更新页面信息
void PageSelectScene::updatePages(NetPageAttr apages[], int arrlen)
{
    log("update %d page", arrlen);
    auto pagesize = Director::getInstance()->getTextureCache()->getTextureForKey("blue.png")->getContentSize();

    for (int i = 0; i < arrlen; i++)
    {
        auto pageid = PageSelectScene::BASE_PAGE_ID + apages[i].hight * (_map_max_w + 1) + apages[i].width;
        auto page = (MPage*)(_wheel->getChildByTag(pageid));
        if (nullptr == page)
        {
            page = MPage::create();
            page->setPosition(_page_start.x + pagesize.width * apages[i].width,
                              _page_start.y + pagesize.height * apages[i].hight);
            _wheel->addChild(page, 1, pageid);

            page->setPagePos(apages[i].width, apages[i].hight);
            page->setBasePosition(page->getPosition());
        }

        page->setPageOwn(apages[i].hasown);
        page->setOpenPercent(apages[i].openprecent);

        if (apages[i].width - _center_w > 3 ||
            apages[i].width - _center_w < -3 ||
            apages[i].hight - _center_h > 3 ||
            apages[i].hight - _center_h < -3)
        {
            page->hideFromScene();
        }
    }

    this->refreshShowPages();
}

//页面格子触发选择页面
void PageSelectScene::clickSelectPage(int w, int h)
{
    if (true == _bWaitNet)
    {
        return;
    }

    auto pageid = PageSelectScene::BASE_PAGE_ID + h * (_map_max_w + 1) + w;
    auto page = (MPage*)(_wheel->getChildByTag(pageid));
    if (true == page->getPageOwn())
    {
        return;
    }

    _bWaitNet = true;

    UtilNet::sendSelectPage(w, h);
    _page_w = w;
    _page_h = h;
}

//返回页面选择信息
void PageSelectScene::callbackSelectPage(bool ret, int maxw, int maxh)
{
    _bWaitNet = false;

    if (false == ret)
    {
        _page_w = _page_h = 0;
        return;
    }

    auto pageid = PageSelectScene::BASE_PAGE_ID + _page_h * (_map_max_w + 1) + _page_w;
    auto page = (MPage*)(_wheel->getChildByTag(pageid));
    if (nullptr == page)
    {
        return;
    }

    _box_max_w = maxw;
    _box_max_h = maxh;

    auto abig = ScaleTo::create(0.5f, 2);
    auto afunc = CallFuncN::create(CC_CALLBACK_1(PageSelectScene::selectActionCallback, this));
    page->runAction(Sequence::create(abig, afunc, NULL));
}

//选中动画后的回调函数
void PageSelectScene::selectActionCallback(cocos2d::Node* sender)
{
    this->unscheduleAllSelectors();
    Director::getInstance()->replaceScene(GameSceneNet::createScene(_page_w, _page_h, _box_max_w, _box_max_h));
}

//返回连接错误信息
void PageSelectScene::callbackErrConnection(void)
{
    _bWaitNet = true;

    auto errBoard = Sprite::create("Err_board.png");
    errBoard->setPosition(_backgroud->getContentSize().width / 2,
                          _backgroud->getContentSize().height / 2);
    _backgroud->addChild(errBoard, 2000);

    //创建退出菜单键
    auto pExitItem = MenuItemImage::create("Return.png", "Return_big.png",
                                           CC_CALLBACK_1(PageSelectScene::menuCloseCallback, this));
    Menu* pItemMenu = Menu::create();
    pItemMenu->addChild(pExitItem);
    pItemMenu->setPosition(errBoard->getContentSize().width / 2,
                           errBoard->getContentSize().height / 4);
    errBoard->addChild(pItemMenu);

    auto label = Label::createWithTTF("connect server\n    failed\n please return", CF_F("font_hei"), 36);
    label->setColor(Color3B::BLACK);
    label->setPosition(errBoard->getContentSize().width / 2,
                       errBoard->getContentSize().height * 2.7 / 4);
    errBoard->addChild(label);
}

//刷新页面格子的显示情况
void PageSelectScene::refreshShowPages(void)
{
    auto pagesize = Director::getInstance()->getTextureCache()
                    ->getTextureForKey("blue.png")->getContentSize();

    auto centerid = PageSelectScene::BASE_PAGE_ID + _center_h * (_map_max_w + 1) + _center_w;
    auto dpos = _wheel->getChildByTag(centerid)->getPosition() + _wheel->getPosition() - _wheel_base_pos;

    int new_c_w = _center_w;
    int new_c_h = _center_h;

    if (dpos.x > (pagesize.width / 2)) {
        new_c_w -= 1;
    } else if (dpos.x < -(pagesize.width / 2)) {
        new_c_w += 1;
    }
    if (dpos.y > (pagesize.height / 2)) {
        new_c_h -= 1;
    } else if (dpos.y < -(pagesize.height / 2)) {
        new_c_h += 1;
    }

    if (new_c_w < 2) {
        new_c_w = 2;
    } else if (new_c_w > (_map_max_w - 2)) {
        new_c_w = _map_max_w - 2;
    }
    if (new_c_h < 2) {
        new_c_h = 2;
    } else if (new_c_h > (_map_max_h - 2)) {
        new_c_h = _map_max_h - 2;
    }

    for (int h = -3; h <= 3; h++) {
        for (int w = -3; w <= 3; w++) {
            if (_center_w + w < 0 || _center_w + w > _map_max_w || _center_h + h < 0 || _center_h + h > _map_max_h) {
                continue;
            }

            auto pPage = (MPage*)(_wheel->getChildByTag(PageSelectScene::BASE_PAGE_ID + (_center_h + h) * (_map_max_w + 1) + _center_w + w));
            if (nullptr == pPage) {
                UtilNet::sendShowPages(_center_w + 2 * w, _center_h + 2 * h);
                _bWaitNet = true;
                return;
            }

            auto offPoint = pPage->getBasePosition() + _wheel->getPosition() - _wheel_base_pos;
            if (offPoint.x >= (3 * pagesize.width) || offPoint.x <= (-3 * pagesize.width)) {
                pPage->hideFromScene();
                pPage->setPositionX(pPage->getBasePosition().x);
                pPage->setScaleX(1);
            } else if (offPoint.y < (3 * pagesize.height) && offPoint.y > (-3 * pagesize.height)) {
                pPage->showFromScene();
            }
            if (offPoint.x >= (2 * pagesize.width) && offPoint.x < (3 * pagesize.width)) {
                auto offx = offPoint.x - (2 * pagesize.width);
                pPage->setPositionX(pPage->getBasePosition().x - (offx / 2));
                pPage->setScaleX(1 - offx / pagesize.width);
            } else if (offPoint.x <= (-2 * pagesize.width) && offPoint.x > (-3 * pagesize.width)) {
                auto offx = offPoint.x + (2 * pagesize.width);
                pPage->setPositionX(pPage->getBasePosition().x - (offx / 2));
                pPage->setScaleX(1 + offx / pagesize.width);
            } else if (offPoint.x < (2 * pagesize.width) && offPoint.x > (-2 * pagesize.width)) {
                pPage->setPositionX(pPage->getBasePosition().x);
                pPage->setScaleX(1);
            }

            if (offPoint.y >= (3 * pagesize.height) || offPoint.y <= (-3 * pagesize.height)) {
                pPage->hideFromScene();
                pPage->setPositionY(pPage->getBasePosition().y);
                pPage->setScaleY(1);
            } else if (offPoint.x < (3 * pagesize.width) && offPoint.x > (-3 * pagesize.width)) {
                pPage->showFromScene();
            }
            if (offPoint.y >= (2 * pagesize.height) && offPoint.y < (3 * pagesize.height)) {
                auto offy = offPoint.y - (2 * pagesize.height);
                pPage->setPositionY(pPage->getBasePosition().y - (offy / 2));
                pPage->setScaleY(1 - offy / pagesize.height);
            } else if (offPoint.y <= (-2 * pagesize.height) && offPoint.y > (-3 * pagesize.height)) {
                auto offy = offPoint.y + (2 * pagesize.height);
                pPage->setPositionY(pPage->getBasePosition().y - (offy / 2));
                pPage->setScaleY(1 + offy / pagesize.height);
            } else if (offPoint.y < (2 * pagesize.height) && offPoint.y > (-2 * pagesize.height)) {
                pPage->setPositionY(pPage->getBasePosition().y);
                pPage->setScaleY(1);
            }
        }
    }

    _center_w = new_c_w;
    _center_h = new_c_h;

    return;
}
