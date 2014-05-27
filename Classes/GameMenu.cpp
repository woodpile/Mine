#include "GameMenu.h"
#include "game/GameScene.h"
#include "config/Config.h"

USING_NS_CC;

Scene* GameMenu::createScene()
{
    auto scene = Scene::create();

    auto layer = GameMenu::create();

    scene->addChild(layer);

    return scene;
}

bool GameMenu::init()
{
    if ( !Layer::init() )
    {
        return false;
    }

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Point origin = Director::getInstance()->getVisibleOrigin();

    auto closeItem = MenuItemImage::create(CF_F("img_menu_close_n"), CF_F("img_menu_close_s"),
                                           CC_CALLBACK_1(GameMenu::menuCloseCallback, this));

	closeItem->setPosition(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                            origin.y + closeItem->getContentSize().height/2);

    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(0, 0);
    this->addChild(menu, 1);

    auto label = Label::createWithTTF("Hello World", CF_F("font_hei"), 24);

    label->setPosition(origin.x + visibleSize.width/2,
                        origin.y + visibleSize.height - label->getContentSize().height);

    this->addChild(label, 1);

    auto sprite = Sprite::create(CF_F("img_menu_back"));

    sprite->setPosition(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y);

    this->addChild(sprite, 0);

    auto labelLan = Label::createWithTTF("Game Start", CF_F("font_hei"), 24);
    MenuItemLabel* pMenuItemLanTest = MenuItemLabel::create(labelLan, CC_CALLBACK_1(GameMenu::menuGame, this));
    pMenuItemLanTest->setPosition(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y);

    Menu* pItemMenu = Menu::create();
    pItemMenu->addChild(pMenuItemLanTest);
    pItemMenu->setPosition(0, 0);
    this->addChild(pItemMenu, 2);

    return true;
}

void GameMenu::menuGame(Ref* pSender)
{
    Director::getInstance()->replaceScene(GameScene::createScene());
}


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
