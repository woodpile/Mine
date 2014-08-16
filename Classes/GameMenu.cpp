#include "GameMenu.h"
#include "game/GameScene.h"
#include "config/Config.h"

USING_NS_CC;

//创建主菜单场景
Scene* GameMenu::createScene()
{
    auto scene = Scene::create();

    auto layer = GameMenu::create();

    scene->addChild(layer);

    return scene;
}
//引擎 初始化图层
bool GameMenu::init()
{
    //引擎
    if ( !Layer::init() )
    {
        return false;
    }

    //取屏幕的逻辑尺寸 逻辑原点
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Point origin = Director::getInstance()->getVisibleOrigin();

    //创建退出游戏的菜单键
    auto closeItem = MenuItemImage::create(CF_F("img_menu_close_n"), CF_F("img_menu_close_s"),
                                           CC_CALLBACK_1(GameMenu::menuCloseCallback, this));
	closeItem->setPosition(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                            origin.y + closeItem->getContentSize().height/2);
    //创建承载退出键的菜单
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(0, 0);
    this->addChild(menu, 1);

    //创建背景
    auto sprite = Sprite::create(CF_F("img_menu_back"));
    sprite->setPosition(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y);
    this->addChild(sprite, 0);

    //创建进入游戏场景的菜单键
    auto enterGameItem = MenuItemImage::create("Start_Game.png", "Start_Game_select.png",
                                           CC_CALLBACK_1(GameMenu::menuGame, this));
    enterGameItem->setPosition(visibleSize.width/2 + origin.x, visibleSize.height * 2 / 5 + origin.y);
    //创建主菜单
    Menu* pItemMenu = Menu::create();
    pItemMenu->addChild(enterGameItem);
    pItemMenu->setPosition(0, 0);
    this->addChild(pItemMenu, 2);

    return true;
}

//进入游戏场景菜单键的回调函数
void GameMenu::menuGame(Ref* pSender)
{
    Director::getInstance()->replaceScene(GameScene::createScene());
}

//退出游戏程序的菜单键的回调函数
void GameMenu::menuCloseCallback(Ref* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
    return;
#endif

    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
