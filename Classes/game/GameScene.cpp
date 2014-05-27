#include "../GameMenu.h"
#include "GameScene.h"
#include "config/Config.h"
#include "MBox.h"

USING_NS_CC;

Scene* GameScene::createScene()
{
    auto scene = Scene::create();

    auto layer = GameScene::create();

    scene->addChild(layer);

    return scene;
}

void GameScene::onEnter()
{
    Layer::onEnter();

    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);

    listener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(GameScene::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(GameScene::onTouchEnded, this);

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void GameScene::onExit()
{
    Layer::onExit();
}

bool GameScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Point origin = Director::getInstance()->getVisibleOrigin();
    
    _backgroud = Sprite::create("back1.png", Rect(0, 0, 800, 600));
    _backgroud->setPosition(visibleSize.width / 2, visibleSize.height / 2);
    addChild(_backgroud);

    createBoxes();

    return true;
}

void GameScene::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->replaceScene(GameMenu::createScene());
}

bool GameScene::onTouchBegan(Touch* touch, Event* event)
{
    Point point = _backgroud->convertTouchToNodeSpaceAR(touch);
    auto s = _backgroud->getContentSize();
    auto ret = Rect(-s.width / 2, -s.height / 2, s.width, s.height).containsPoint(point);
    if (true == ret)
    {
        _startPos = touch->getLocation();
    }
    return ret;
}

void GameScene::onTouchMoved(Touch* touch, Event* event)
{
    auto p = touch->getLocation();
    auto dp = p - _startPos;

    auto vecN = _backgroud->getChildren();
    for (auto& pNode : vecN)
    {
        auto px = pNode->getPositionX();
        pNode->setPositionX(px + dp.x);
    }
    _startPos.x = p.x;

    return;
}

void GameScene::onTouchEnded(Touch* touch, Event* event)
{
    return;
}

void GameScene::createBoxes(void)
{
    auto s = _backgroud->getContentSize();
    Director::getInstance()->getTextureCache()->addImage("white.png");
    auto pSpriteText = Director::getInstance()->getTextureCache()->addImage("blue.png");

    auto ms = pSpriteText->getContentSize();
    int lx = (int)(s.width - ms.width) / (int)(ms.width);
    int ly = (int)(s.height - ms.height) / (int)(ms.height);
    int ex = ((int)(s.width - lx * ms.width) / 2) + (int)(ms.width) / 2;
    int ey = ((int)(s.height - ly * ms.height) / 2) + (int)(ms.height) / 2;


    MBox* box = nullptr; 
    unsigned int tagBox = 1000;
    for (int i = 0; i < lx; i++)
    {
        std::vector<unsigned int> vx;
        for (int j = 0; j < ly; j++)
        {
            box = MBox::createWithTexture(pSpriteText);
            box->setPosition(ex + ms.width * i, ey + ms.height * j);

            tagBox += 1;
            _backgroud->addChild(box, 1, tagBox);
            vx.push_back(tagBox);
        }
        _vecMatrix.push_back(vx);
    }
}
