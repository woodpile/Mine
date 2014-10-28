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
#include "../GameMenu.h"
#include "../util/UtilNet.h"

USING_NS_CC;

PageSelectScene* PageSelectScene::_pPageSelectInstance = nullptr;

PageSelectScene::PageSelectScene(void)
: _page_h(0), _page_w(0), _bLogin(false)
{
}

PageSelectScene::~PageSelectScene(void)
{
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

    UtilNet::sendUserLogin();

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

//返回登录信息
void PageSelectScene::callbackUserLogin(void)
{
    _bLogin = true;
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

    UtilNet::sendShowPages();
}

//展示格子的返回信息
void PageSelectScene::callbackShowPages(int mapw, int maph, NetPageAttr apages[], int arrlen)
{
    auto pagesize = Director::getInstance()->getTextureCache()->getTextureForKey("blue.png")->getContentSize();

    _map_max_w = mapw;
    _map_max_h = maph;

    _page_start.x = 0 - ((pagesize.width * (_map_max_w + 1) / 2) - (pagesize.width / 2));
    _page_start.y = 0 - ((pagesize.height * (_map_max_h + 1) / 2) - (pagesize.height / 2));

    this->updatePages(apages, arrlen);

    this->schedule(schedule_selector(PageSelectScene::timeShowPages), 5);
}

//定时发送显示当前页面信息的定时器函数
void PageSelectScene::timeShowPages(float dt)
{
    UtilNet::sendShowPages();
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
        }

        page->setPageOwn(apages[i].hasown);
        page->setOpenPercent(apages[i].openprecent);
    }
}

//页面格子触发选择页面
void PageSelectScene::clickSelectPage(int w, int h)
{
    auto pageid = PageSelectScene::BASE_PAGE_ID + h * (_map_max_w + 1) + w;
    auto page = (MPage*)(_wheel->getChildByTag(pageid));
    if (true == page->getPageOwn())
    {
        return;
    }

    UtilNet::sendSelectPage(w, h);
    _page_w = w;
    _page_h = h;
}

//返回页面选择信息
void PageSelectScene::callbackSelectPage(bool ret, int maxw, int maxh)
{
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
