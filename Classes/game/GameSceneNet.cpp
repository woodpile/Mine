//
//  GameSceneNet.cpp
//  Mine
//
//  Created by woodpile on 14-8-25.
//
//

#include "../config/Config.h"
#include "GameSceneNet.h"
#include "HelpBoard.h"
#include "PageSelectScene.h"
#include "../util/UtilNet.h"

USING_NS_CC;

GameSceneNet* GameSceneNet::_pNetGameInstance = nullptr;

GameSceneNet::GameSceneNet(void)
: _bCanTouch(false), _boxid_beClicked(0), _page_maxw(0), _page_maxh(0), _page_w(0), _page_h(0)
{
}

GameSceneNet::~GameSceneNet(void)
{
}

//创建承载本图层的场景
Scene* GameSceneNet::createScene(int w, int h, int maxboxw, int maxboxh)
{
    log("create GameSceneNet, page(%d, %d), max box (%d, %d)", w, h, maxboxw, maxboxh);
    auto scene = Scene::create();
    
    auto layer = GameSceneNet::create();
    GameSceneNet::_pNetGameInstance = layer;
    layer->setPagePos(w, h);
    layer->_page_maxw = maxboxw;
    layer->_page_maxh = maxboxh;
    
    scene->addChild(layer);
    
    return scene;
}

GameSceneNet* GameSceneNet::getInstance()
{
    return GameSceneNet::_pNetGameInstance;
}

//退出菜单键的回调函数
void GameSceneNet::menuCloseCallback(Ref* pSender)
{
    log("net close callback");
    UtilNet::sendReleasePage();
    //返回游戏主菜单
    Director::getInstance()->replaceScene(PageSelectScene::createScene());
}

//设置地图页面的值
void GameSceneNet::setPagePos(int w, int h)
{
    _page_w = w;
    _page_h = h;
}

//初始化帮助信息界面
void GameSceneNet::initHelpInfoBoard(void)
{
    //取屏幕逻辑尺寸
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Point origin = Director::getInstance()->getVisibleOrigin();

    auto helpBoard = HelpBoard::createNet();
    auto sh = helpBoard->getContentSize();
    if (visibleSize.height >= 960)
    {
        helpBoard->setPosition(sh.width / 2 + origin.x, sh.height / 2 + origin.y);
    }
    else
    {
        helpBoard->setPosition(sh.width / 2 + origin.x,
                               sh.height / 2 + origin.y - 142);
        helpBoard->setBoardCanMove(true);
    }
    this->addChild(helpBoard);
}

//初始化分数面板
void GameSceneNet::initScoreInfoBoard(void)
{
    //取屏幕逻辑尺寸
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Point origin = Director::getInstance()->getVisibleOrigin();

    //创建分数底板
    auto score_board = Sprite::create("Scoreboard.png");
    score_board->setPosition(visibleSize.width / 2 + origin.x,
                             visibleSize.height + origin.y - score_board->getContentSize().height / 2);
    this->addChild(score_board);

    //创建分数标签
    auto s = String::createWithFormat("page ( %d , %d )\nwidth %d height %d",
                                      _page_w, _page_h, _page_maxw + 1, _page_maxh + 1);
    _score_label = Label::createWithTTF(s->getCString(), CF_F("font_hei"), 36);
    _score_label->setColor(Color3B::BLACK);
    _score_label->setPosition(score_board->getContentSize().width / 2, score_board->getContentSize().height / 2);
    score_board->addChild(_score_label);
}

//格子被点击
bool GameSceneNet::boxBeClick(int w, int h)
{
    log("net box be click");

    if (0 == _page_maxw)
    {
        return false;
    }

    UtilNet::sendBoxClick(_page_w, _page_h, w, h);
    _boxid_beClicked = GameScene::BASE_BOX_ID + h * (_page_maxw + 1) + w;
    
    return false;
}

//格子被标记
void GameSceneNet::boxBeFlag(int w, int h)
{
    log("net box be flag");

    if (0 == _page_maxw)
    {
        return;
    }

    UtilNet::sendBoxFlag(_page_w, _page_h, w, h);
    _boxid_beClicked = GameScene::BASE_BOX_ID + h * (_page_maxw + 1) + w;
}


//创建一个新的游戏
void GameSceneNet::createNewGame(void)
{
    this->loadGameGround();
}

//加载游戏环境
void GameSceneNet::loadGameGround(void)
{
    //创建承载网络格子的底板
    _wheel = Sprite::create();
    _wheel->setPosition(_backgroud->getContentSize().width / 2,
                        _backgroud->getContentSize().height / 2);
    _backgroud->addChild(_wheel);
    _wheel_base_pos = _wheel->getPosition();

    _center_w = (_page_maxw + 0) / 2;
    _center_h = (_page_maxh + 0) / 2;

    auto boxsize = Director::getInstance()->getTextureCache()->getTextureForKey("blue.png")->getContentSize();
    log("boxsize (%f, %f)", boxsize.width, boxsize.height);

    _box_start.x = 0 - ((boxsize.width * (_page_maxw + 1) / 2) - (boxsize.width / 2));
    _box_start.y = 0 - ((boxsize.height * (_page_maxh + 1) / 2) - (boxsize.height / 2));
    _box_end.x = 0 + ((boxsize.width * (_page_maxw + 1) / 2) - (boxsize.width / 2));
    _box_end.y = 0 + ((boxsize.height * (_page_maxh + 1) / 2) - (boxsize.height / 2));

    _edge00.x = _wheel_base_pos.x + _box_start.x + (2 * boxsize.width);
    _edge00.y = _wheel_base_pos.y + _box_start.y + (2 * boxsize.height);
    _edgeMM.x = _wheel_base_pos.x + _box_end.x - (2 * boxsize.width);
    _edgeMM.y = _wheel_base_pos.y + _box_end.y - (2 * boxsize.height);

    //触摸监听
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    //引擎 向触摸监听注册响应函数
    listener->onTouchBegan = CC_CALLBACK_2(GameSceneNet::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(GameSceneNet::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(GameSceneNet::onTouchEnded, this);
    //激活触摸监听
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    //发送请求加载页面上的格子信息
    UtilNet::sendLoadSubPage(_page_w, _page_h, _center_w, _center_h);

    return;
}

void GameSceneNet::updateBoxs(int pw, int ph, NetBoxAttr aboxs[], int arrlen)
{
    auto boxsize = Director::getInstance()->getTextureCache()->getTextureForKey("blue.png")->getContentSize();
    
    for (int i = 0; i < arrlen; i++)
    {
        auto boxid = GameScene::BASE_BOX_ID + aboxs[i].hight * (_page_maxw + 1) + aboxs[i].width;
        auto box = (MBox*)(_wheel->getChildByTag(boxid));
        if (nullptr == box)
        {
            box = MBox::create();
            box->setPosition(_box_start.x + boxsize.width * aboxs[i].width,
                             _box_start.y + boxsize.height * aboxs[i].hight);
            _wheel->addChild(box, 1, boxid);
        
            box->setMPos(aboxs[i].width, aboxs[i].hight);
            box->setOutPosition(box->getPosition());
            box->setGame(this);
        }

        if (true == aboxs[i].isopen)
        {
            auto bomb = MBombType::NOBOMB;
            if (0 < aboxs[i].bomb)
            {
                bomb = MBombType::NORMAL;
            }
            box->setAttr(aboxs[i].num, bomb);
            box->openAndDoAtrrib();
        }
        else if (true == aboxs[i].isflag)
        {
            box->openAndFlag();
        }
        else
        {
            box->setAttr(0, MBombType::NOBOMB);
            box->closeAndRecover();
        }
        
        if (aboxs[i].width - _center_w > 3 ||
            aboxs[i].width - _center_w < -2 ||
            aboxs[i].hight - _center_h > 3 ||
            aboxs[i].hight - _center_h < -3)
        {
            box->hideFromGame();
        }
    }

    _bCanTouch = true;

    this->refreshShowBoxes();
}
//点击格子的返回信息
void GameSceneNet::callbackClickBox(int num, int bomb, NetBoxAttr aboxs[], int arrlen)
{
    if (0 == _boxid_beClicked)
    {
        return;
    }

    auto bombtype = MBombType::NOBOMB;
    if (0 < bomb)
    {
        bombtype = MBombType::NORMAL;
    }

    auto box = (MBox*)(_wheel->getChildByTag(_boxid_beClicked));
    if (nullptr == box)
    {
        return;
    }

    box->setAttr(num, bombtype);
    box->openAndDoAtrrib();

    if (0 == bomb)
    {
        return;
    }

    //刷新由引爆导致的格子重置
    for (int i = 0; i < arrlen; i++)
    {
        auto boxid = GameScene::BASE_BOX_ID + aboxs[i].hight * (_page_maxw + 1) + aboxs[i].width;

        auto box = (MBox*)(_wheel->getChildByTag(boxid));
        if (nullptr == box)
        {
            continue;
        }

        if (true == aboxs[i].isopen)
        {
            if (true == box->resetAttr(aboxs[i].num, MBombType::NOBOMB))
            {
                box->refreshNumTag();
            }
        }
        else
        {
            box->setAttr(0, MBombType::NOBOMB);
            box->closeAndRecover();
        }
    }

    _boxid_beClicked = 0;

    refreshShowBoxes();

    box->runAction(Sequence::create(ScaleTo::create(0.3f, 1.3 * box->getScaleX(), 1.3 * box->getScaleY()),
                                    ScaleTo::create(0.3f, 1 * box->getScaleX(), 1 * box->getScaleY()), NULL));

    return;
}
//点击格子的返回信息
void GameSceneNet::callbackFlagBox(int num, int bomb, NetBoxAttr aboxs[], int arrlen)
{
    if (0 == _boxid_beClicked)
    {
        return;
    }

    if (0 < bomb)
    {
        return;
    }

    auto box = (MBox*)(_wheel->getChildByTag(_boxid_beClicked));
    if (nullptr == box)
    {
        return;
    }

    //刷新由标记导致的格子重置
    for (int i = 0; i < arrlen; i++)
    {
        auto boxid = GameScene::BASE_BOX_ID + aboxs[i].hight * (_page_maxw + 1) + aboxs[i].width;

        auto box = (MBox*)(_wheel->getChildByTag(boxid));
        if (nullptr == box)
        {
            continue;
        }

        if (true == aboxs[i].isopen)
        {
            if (true == box->resetAttr(aboxs[i].num, MBombType::NOBOMB))
            {
                box->refreshNumTag();
            }
        }
        else
        {
            box->setAttr(0, MBombType::NOBOMB);
            box->closeAndRecover();
        }
    }

    _boxid_beClicked = 0;

    refreshShowBoxes();

    box->runAction(Sequence::create(ScaleTo::create(0.3f, 1.3 * box->getScaleX(), 1.3 * box->getScaleY()),
                                    ScaleTo::create(0.3f, 1 * box->getScaleX(), 1 * box->getScaleY()), NULL));

    return;
}

//重新扩散一些格子
void GameSceneNet::reDiffusion(void)
{
    //暂无需要,仅用于重载单机模式
    return;
}

//引擎 触摸行为按下
bool GameSceneNet::onTouchBegan(Touch* touch, Event* event)
{
    //是否处于可滑动状态
    if (false == _bCanTouch)
    {
        return false;
    }
    
    return true;
}
//引擎 触摸行为移动
void GameSceneNet::onTouchMoved(Touch* touch, Event* event)
{
    //是否处于可滑动状态
    if (false == _bCanTouch)
    {
        return;
    }
    
    //滑动处理
    auto t = touch->getLocation();
    auto p = touch->getPreviousLocation();
    auto n = _wheel->getPosition();
    
    auto n_wheel = n + (t - p);
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
    
    //刷新需要进入显示和进入隐藏的格子
    refreshShowBoxes();
    
    return;
}
//引擎 触摸行为松开
void GameSceneNet::onTouchEnded(Touch* touch, Event* event)
{
    return;
}

//刷新需要进入显示和进入隐藏的格子
void GameSceneNet::refreshShowBoxes(void)
{
    auto boxsize = Director::getInstance()->getTextureCache()->getTextureForKey("blue.png")->getContentSize();
    
    auto boxid = GameScene::BASE_BOX_ID + _center_h * (_page_maxw + 1) + _center_w;
    auto oldcenter = _wheel->getChildByTag(boxid);
    auto oldp = oldcenter->getPosition() + _wheel->getPosition();
    
    auto dp = oldp - _wheel_base_pos;
    
    int new_w = _center_w;
    int new_h = _center_h;
    if (dp.x > (boxsize.width / 2))
    {
        new_w -= 1;
    }
    else if (dp.x < -(boxsize.width / 2))
    {
        new_w += 1;
    }
    
    if (dp.y > (boxsize.height / 2))
    {
        new_h -= 1;
    }
    else if (dp.y < -(boxsize.height / 2))
    {
        new_h += 1;
    }
    
    if (new_w < 2)
    {
        new_w = 2;
    }
    else if (new_w > (_page_maxw - 2))
    {
        new_w = _page_maxw - 2;
    }
    if (new_h < 2)
    {
        new_h = 2;
    }
    else if (new_h > (_page_maxh - 2))
    {
        new_h = _page_maxh - 2;
    }

    for (int i = -3; i <= 3; i++)
    {
        for (int j = -3; j <= 3; j++)
        {
            if (_center_w + i < 0 || _center_w + i > _page_maxw ||
                _center_h + j < 0 || _center_h + j > _page_maxh)
            {
                continue;
            }

            auto bBox = (MBox*)(_wheel->getChildByTag(GameScene::BASE_BOX_ID + (_center_h + j) * (_page_maxw + 1) + _center_w + i));
            if (nullptr == bBox)
            {
                _bCanTouch = false;
                UtilNet::sendLoadSubPage(_page_w, _page_h, _center_w + i, _center_h + j);
                return;
            }

            auto offPoint = bBox->getOutPosition() + _wheel->getPosition() - _wheel_base_pos;
            if (offPoint.x >= (3 * boxsize.width) || offPoint.x <= (-3 * boxsize.width))
            {
                bBox->hideFromGame();
                bBox->setPositionX(bBox->getOutPosition().x);
                bBox->setScaleX(1);
            }
            else if (offPoint.y < (3 * boxsize.height) && offPoint.y > (-3 * boxsize.height))
            {
                bBox->showFromGame();
            }
            if (offPoint.x >= (2 * boxsize.width) && offPoint.x < (3 * boxsize.width))
            {
                auto offx = offPoint.x - (2 * boxsize.width);
                bBox->setPositionX(bBox->getOutPosition().x - (offx / 2));
                bBox->setScaleX(1 - offx / boxsize.width);
            }
            else if (offPoint.x > (-3 * boxsize.width) && offPoint.x <= (-2 * boxsize.width))
            {
                auto offx = offPoint.x + (2 * boxsize.width);
                bBox->setPositionX(bBox->getOutPosition().x - (offx / 2));
                bBox->setScaleX(1 + offx / boxsize.width);
            }
            else if (offPoint.x < (2 * boxsize.width) && offPoint.x > (-2 * boxsize.width))
            {
                bBox->setPositionX(bBox->getOutPosition().x);
                bBox->setScaleX(1);
            }

            if (offPoint.y >= (3 * boxsize.height) || offPoint.y <= (-3 * boxsize.height))
            {
                bBox->hideFromGame();
                bBox->setPositionY(bBox->getOutPosition().y);
                bBox->setScaleY(1);
            }
            else if (offPoint.x < (3 * boxsize.width) && offPoint.x > (-3 * boxsize.width))
            {
                bBox->showFromGame();
            }
            if (offPoint.y >= (2 * boxsize.height) && offPoint.y < (3 * boxsize.height))
            {
                auto offy = offPoint.y - (2 * boxsize.height);
                bBox->setPositionY(bBox->getOutPosition().y - (offy / 2));
                bBox->setScaleY(1 - offy / boxsize.width);
            }
            else if (offPoint.y > (-3 * boxsize.height) && offPoint.y <= (-2 * boxsize.height))
            {
                auto offy = offPoint.y + (2 * boxsize.height);
                bBox->setPositionY(bBox->getOutPosition().y - (offy / 2));
                bBox->setScaleY(1 + offy / boxsize.height);
            }
            else if (offPoint.y < (2 * boxsize.height) && offPoint.y > (-2 * boxsize.height))
            {
                bBox->setPositionY(bBox->getOutPosition().y);
                bBox->setScaleY(1);
            }
        }
    }

    _center_w = new_w;
    _center_h = new_h;

    return;
}