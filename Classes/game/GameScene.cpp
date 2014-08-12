#include "../GameMenu.h"
#include "GameScene.h"
#include "../config/Config.h"
#include "MBox.h"

USING_NS_CC;

//创建承载本图层的场景
Scene* GameScene::createScene()
{
    auto scene = Scene::create();

    auto layer = GameScene::create();

    scene->addChild(layer);

    return scene;
}
//引擎 加载图层时调用的方法
void GameScene::onEnter()
{
    //引擎
    Layer::onEnter();
}
//引擎 移除图层时调用的方法
void GameScene::onExit()
{
    //引擎
    Layer::onExit();
}
//引擎 初始化
bool GameScene::init()
{
    //引擎
    if ( !Layer::init() )
    {
        return false;
    }
    
    //取屏幕逻辑尺寸
    Size visibleSize = Director::getInstance()->getVisibleSize();
    
    //创建背景
    _backgroud = Sprite::create("back1.png", Rect(0, 0, 800, 600));
    _backgroud->setPosition(visibleSize.width / 2, visibleSize.height / 2);
    addChild(_backgroud);
    
    //创建退出菜单键
    auto labelExit = Label::createWithTTF("return to menu", CF_F("font_hei"), 24);
    MenuItemLabel* pExitItem = MenuItemLabel::create(labelExit, CC_CALLBACK_1(GameScene::menuCloseCallback, this));
    auto exitSize = pExitItem->getContentSize();
    pExitItem->setPosition(exitSize.width / 2, 0 - exitSize.height / 2);
    //创建重新开始菜单键
    auto labelRestart = Label::createWithTTF("restart game", CF_F("font_hei"), 24);
    MenuItemLabel* pRestartItem = MenuItemLabel::create(labelRestart, CC_CALLBACK_1(GameScene::menuRestartCallback, this));
    auto restartSize = pRestartItem->getContentSize();
    pRestartItem->setPosition(exitSize.width / 2, 0 - exitSize.height - restartSize.height / 2);
    //创建菜单
    Menu* pItemMenu = Menu::create();
    pItemMenu->addChild(pExitItem);
    pItemMenu->addChild(pRestartItem);
    pItemMenu->setPosition(0, visibleSize.height);
    this->addChild(pItemMenu);
    
    //初始化有关资源
    Director::getInstance()->getTextureCache()->addImage("white.png");
    Director::getInstance()->getTextureCache()->addImage("blue.png");
    
    //初始化游戏参数
    _flaged_box.clear();
    _score = 0;
    
    //初始化游戏格子
    this->createNewGame();

    return true;
}
//退出菜单键的回调函数
void GameScene::menuCloseCallback(Ref* pSender)
{
    //返回游戏主菜单
    Director::getInstance()->replaceScene(GameMenu::createScene());
}
//重新开始菜单键的回调函数
void GameScene::menuRestartCallback(Ref* pSender)
{
    //删除现有的格子
    _backgroud->removeAllChildren();
    //重置游戏参数
    _flaged_box.clear();
    //创建新的游戏
    this->createNewGame();
}

//创建一个新的游戏
void GameScene::createNewGame(void)
{
    int width = 6, heigh = 6;
    
    //初始化游戏地图
    if (nullptr != _map)
    {
        delete _map;
        _map = nullptr;
    }
    _map = new MMap(width, heigh);
    _map->setBombProbability(2000);
    _map->randRefresh();
    
    //计算地图边界
    auto mapsize = _backgroud->getContentSize();
    auto boxsize = Director::getInstance()->getTextureCache()->getTextureForKey("blue.png")->getContentSize();
    int edgew = ((int)(mapsize.width - width * boxsize.width) / 2) + (int)(boxsize.width) / 2;
    int edgeh = ((int)(mapsize.height - heigh * boxsize.height) /2) - (int)(boxsize.height) / 2 + heigh * boxsize.height;
    
    //根据地图画出格子
    for (int h = 0; h < heigh; h++)
    {
        for (int w = 0; w < width; w++)
        {
            //创建一个格子
            auto box = MBox::create();
            box->setPosition(edgew + boxsize.width * w, edgeh - boxsize.height * h);
            auto boxid = GameScene::BASE_BOX_ID + h * width + w;
            _backgroud->addChild(box, 1, boxid);
            //设置格子的属性
            box->setBoxId(boxid);
            box->setMPos(w, h);
            auto boxAttrib = _map->getDate(w, h);
            box->setAttr(boxAttrib.num, (MBombType)boxAttrib.bomb);
            //设置关联
            box->setGame(this);
        }
    }
    
    //创建分数标签
    _score_label = Label::createWithTTF("SCORE 0", CF_F("font_hei"), 48);
    _score_label->setColor(Color3B::BLACK);
    _score_label->setPosition(_backgroud->getContentSize().width / 2,
                              _backgroud->getContentSize().height - _score_label->getContentSize().height / 2);
    _backgroud->addChild(_score_label);
    
    return;
}

//格子被点击
void GameScene::boxBeClick(int boxid)
{
    //取格子坐标
    auto w = (boxid - GameScene::BASE_BOX_ID) % _map->_width;
    auto h = (boxid - GameScene::BASE_BOX_ID) / _map->_heigh;
    
    log("box %d %d be clicked", w, h);
    
    //取格子属性
    if (MBombType::NORMAL == _map->getDate(w, h).bomb)
    {
        this->procClickBoxBang(w, h);
    }
    else
    {
        this->procClickBoxSafe(w, h);
    }
}

//处理一个格子爆炸
void GameScene::procClickBoxBang(int w, int h)
{
    log("box big bang !!!");
    
    //处理所有的格子
    for (auto i = GameScene::BASE_BOX_ID; i < (GameScene::BASE_BOX_ID + _map->_width * _map->_heigh); i++)
    {
        //对所有的格子做掀开的处理
        auto it = (MBox*)_backgroud->getChildByTag(i);
        it->openAndDoAtrrib();
    }
}
//处理一个安全的格子被点击
void GameScene::procClickBoxSafe(int w, int h)
{
    log("box safe");
    
    //处理num为0的格子
    if (0 == _map->getDate(w, h).num)
    {
        this->procDiffusionBox(w, h);
    }
}
//扩散一个格子
void GameScene::procDiffusionBox(int w, int h)
{
    //异常检查
    if (0 != _map->getDate(w, h).bomb)
    {
        return;
    }
    
    //打开本格子
    auto box = (MBox*)_backgroud->getChildByTag(GameScene::BASE_BOX_ID + h * _map->_width + w);
    if (true == box->isOpened())
    {
        return;
    }
    box->openAndDoAtrrib();
    
    //增加分数
    this->addScore(10);
    
    //递归处理周围的格子
    if (0 == _map->getDate(w, h).num)
    {
        if (0 < h)
        {
            if (0 < w)
            {
                this->procDiffusionBox(w-1, h-1);
            }
            this->procDiffusionBox(w, h-1);
            if (w < _map->_width - 1)
            {
                this->procDiffusionBox(w+1, h-1);
            }
        }
        if (0 < w)
        {
            this->procDiffusionBox(w-1, h);
        }
        if (w < _map->_width - 1)
        {
            this->procDiffusionBox(w+1, h);
        }
        if (h < _map->_heigh - 1)
        {
            if (0 < w)
            {
                this->procDiffusionBox(w-1, h+1);
            }
            this->procDiffusionBox(w, h+1);
            if (w < _map->_width - 1)
            {
                this->procDiffusionBox(w+1, h+1);
            }
        }
    }
}

//格子被标记
void GameScene::boxBeFlag(int w, int h)
{
    log("box be flag");
    
    //不是炸弹格子被标记，按爆炸处理
    if (0 == _map->getDate(w, h).bomb)
    {
        this->procClickBoxBang(w, h);
        return;
    }
    
    //记录标记
    _flaged_box.push_back(std::make_pair(w, h));
    //标记数量达到时，触发格子变化
    if (2 == _flaged_box.size())
    {
        this->changeSomeBoxes();
        _flaged_box.clear();
    }
    
    //增加分数
    this->addScore(50);
}
//改变一些格子
void GameScene::changeSomeBoxes(void)
{
    //触发地图变化
    for (auto it = _flaged_box.begin(); it != _flaged_box.end(); it++)
    {
        _map->changeMap(it->first, it->second, 1);
    }
    //处理产生变化的格子
    for (auto it = _flaged_box.begin(); it != _flaged_box.end(); it++)
    {
        auto box = (MBox*)_backgroud->getChildByTag(GameScene::BASE_BOX_ID + it->second * _map->_width + it->first);
        box->resetAttr(_map->getDate(it->first, it->second).num,
                       (MBombType)_map->getDate(it->first, it->second).bomb);
        box->closeAndRecover();
    }
    //处理其他格子
    for (int th = 0; th < _map->_heigh; th++)
    {
        for (int tw = 0; tw < _map->_width; tw++)
        {
            auto box = (MBox*)_backgroud->getChildByTag(GameScene::BASE_BOX_ID + th * _map->_width + tw);
            if (true == box->resetAttr(_map->getDate(tw, th).num, (MBombType)_map->getDate(tw, th).bomb))
            {
                box->closeAndRecover();
            }
        }
    }
}

//增加分数
void GameScene::addScore(int n)
{
    //增加分数
    _score += n;
    //重写分数标签
    auto s = String::createWithFormat("SCORE %d", _score);
    _score_label->setString(s->getCString());
}
