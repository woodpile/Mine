#include "MBox.h"
#include "GameScene.h"
#include "../config/Config.h"

USING_NS_CC;

enum {
    TAG_TAG_NUM = 100,
    TAG_TAG_BOMB,
};

MBox::MBox(void)
: _box_x(0), _box_y(0), _basePos(Point(0, 0)), _moveStartPos(Point(0, 0)),
_tag(nullptr), _cover(nullptr), _box_id(0), _game(nullptr)
{
    return;
}

MBox::~MBox(void)
{
    return;
}

//创建默认格子
MBox* MBox::create(void)
{
    auto *pBox = new MBox();
    auto pSpriteText = Director::getInstance()->getTextureCache()->getTextureForKey("blue.png");
    if (nullptr == pSpriteText)
    {
        pBox->initWithFile("blue.png");
    }
    else
    {
        pBox->initWithTexture(pSpriteText);
    }
    pBox->autorelease();

    return pBox;
}

//根据纹理撞见格子
MBox* MBox::createWithTexture(Texture2D *pTexture)
{
    auto *pBox = new MBox();
    pBox->initWithTexture(pTexture);
    pBox->autorelease();

    return pBox;
}

//引擎 加载格子时调用的方法 初始化
void MBox::onEnter(void)
{
    //引擎
    Sprite::onEnter();
    
    //触摸监听
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    //引擎 向触摸监听注册响应函数
    listener->onTouchBegan = CC_CALLBACK_2(MBox::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(MBox::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(MBox::onTouchEnded, this);
    //激活触摸监听
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    //读取格子内容尺寸
    auto cs = this->getContentSize();
    //生成覆盖层
    auto pSpriteText = Director::getInstance()->getTextureCache()->getTextureForKey("white.png");
    _cover = Sprite::createWithTexture(pSpriteText);
    _cover->setPosition(cs.width / 2, cs.height / 2);
    this->addChild(_cover, 2);
    
    //记录格子的锚点
    _basePos = this->getAnchorPointInPoints();

    return;
}

//引擎 移除格子时调用的方法 去初始化
void MBox::onExit(void)
{
    //引擎
    Sprite::onExit();

    return;
}

//设置格子所处的游戏场景
void MBox::setGame(GameScene* pGame)
{
    _game = pGame;
}
//外部设置格子ID
void MBox::setBoxId(int boxid)
{
    _box_id = boxid;
}
//外部设置逻辑坐标
void MBox::setMPos(int x, int y)
{
    _box_x = x;
    _box_y = y;
}
//外部设置格子属性
void MBox::setAttr(int n, MBombType bt)
{
    _attribe.num = n;
    _attribe.bombtype = bt;
}

//引擎 触摸行为按下
bool MBox::onTouchBegan(Touch* touch, Event* event)
{
    //覆盖层已删除的情况下不处理触摸行为
    if (nullptr == _cover)
    {
        return false;
    }
    //检查触摸点是否处于本格子的范围内
    Point point = _cover->convertTouchToNodeSpaceAR(touch);
    auto s = _cover->getContentSize();
    bool ret = Rect(-s.width / 2, -s.height / 2, s.width, s.height).containsPoint(point);
    //触摸生效的处理
    if (true == ret)
    {
        //提高格子的Z向高度
        this->setLocalZOrder(1000);
        //记录按下时的触摸点，用于计算增量位移
        _moveStartPos = touch->getLocation();
    }

    return ret;
}
//引擎 触摸行为移动
void MBox::onTouchMoved(Touch* touch, Event* event)
{
    //滑动处理
    slideCover(touch);

    return;
}
//引擎 触摸行为松开
void MBox::onTouchEnded(Touch* touch, Event* event)
{
    //无覆盖层时不需处理 安全判断
    if (nullptr == _cover)
    {
        return;
    }
    //位移极小的时候，判断为点击
    //否则按照滑动处理
    if (3 > touch->getLocation().getDistance(_moveStartPos))
    {
        coverClick();
    }
    else
    {
        coverDrag(touch);
    }
    //复原格子的Z向高度
    this->setLocalZOrder(1);

    return;
}

//覆盖层滑动 随滑动改变形状
void MBox::slideCover(Touch* touch)
{
    //取触摸点坐标
    auto tp = touch->getLocation();
    //向上移动不处理
    if (tp.y > _moveStartPos.y)
    {
        return;
    }
    //移动边界检查 收敛
    auto h = _cover->getContentSize().height * 0.75;
    if (tp.y < _moveStartPos.y - h)
    {
        tp.y = _moveStartPos.y - h;
    }
    //计算移动增量
    auto dy = tp.y - _moveStartPos.y;
    //根据增量改变覆盖层形状 修改覆盖层坐标
    _cover->setScaleY((2 * h - abs(dy)) / (2 * h));
    _cover->setPositionY(dy + _basePos.y);

    return;
}
//覆盖层滑动返回后的回调函数
void MBox::slideCoverBackCallback(cocos2d::Node* sender)
{
    //隐藏标记
    this->showTag(false);
}
//覆盖层点击
void MBox::coverClick(void)
{
    log("clickCover %d %d", _box_x, _box_y);
    
    //反应到游戏场景
    _game->boxBeClick(_box_id);
    
    //阐述覆盖层
    coverDel();
    //删除后处理
    doAttrRet();
}
//覆盖层滑动松开后处理
void MBox::coverDrag(Touch* touch)
{
    //完全滑出边界时删除覆盖层 删除后处理
    //否则滑动返回
    if (_moveStartPos.y - touch->getLocation().y > _cover->getContentSize().height * 0.75)
    {
        //阐述覆盖层
        coverDel();
        //标记为炸弹
        flagToBomb();
        
        //反映到游戏场景
        _game->boxBeFlag(_box_x, _box_y);
    }
    else
    {
        coverSlideBack();
    }
}
//覆盖层删除
void MBox::coverDel(void)
{
    if (nullptr == _cover)
    {
        return;
    }
    //删除覆盖层
    _cover->removeFromParent();
    _cover = nullptr;
}
//覆盖层恢复
void coverRevocer(void)
{
    
}
//覆盖层滑动返回
void MBox::coverSlideBack(void)
{
    //覆盖层移动返回基础层锚点
    _cover->runAction(MoveTo::create(0.2f, _basePos));
    //覆盖层恢复形状 恢复后处理
    auto action = Sequence::create(
        ScaleTo::create(0.2f, 1),
        CallFuncN::create(CC_CALLBACK_1(MBox::slideCoverBackCallback, this)),
        nullptr);
    _cover->runAction(action);
}

//处理标记显示或隐藏
void MBox::showTag(bool show)
{
    //显示标记
    if (true == show && nullptr == _tag)
    {
        //创建显示标记的底板
        _tag = Sprite::create();
        _tag->setPosition(getContentSize().width / 2, getContentSize().height / 2);
        this->addChild(_tag, 1);
        //显示属性标记
        showAttrTag();
    }
    //隐藏标记
    else if (false == show)
    {
        //删除标记底板 连带删除底板上的标记
        if (nullptr != _tag)
        {
            _tag->removeFromParent();
            _tag = nullptr;
        }
    }
}
//显示属性标记
void MBox::showAttrTag(void)
{
    if (nullptr == _tag)
    {
        return;
    }

    return;
}
//处理属性标记效果
void MBox::doAttrRet(void)
{
    if (nullptr == _tag)
    {
        //创建显示标记的底板
        _tag = Sprite::create();
        _tag->setPosition(getContentSize().width / 2, getContentSize().height / 2);
        this->addChild(_tag, 1);
    }
    //处理炸弹属性
    if (MBombType::NOBOMB != _attribe.bombtype)
    {
        auto tb = Sprite::create("lock.png");
        tb->setPosition(0, 0);
        _tag->addChild(tb);
        this->setColor(Color3B::RED);
        
        return;
    }
    //处理数字属性
    if (0 != _attribe.num)
    {
        auto sn = String::createWithFormat("%d", _attribe.num);
        auto tn = Label::createWithTTF(sn->getCString() , CF_F("font_hei"), 27);
        tn->setPosition(0, 0);
        _tag->addChild(tn, 1, TAG_TAG_NUM);
    }
}
//玩家标记为炸弹
void MBox::flagToBomb(void)
{
    if (nullptr == _tag)
    {
        //创建显示标记的底板
        _tag = Sprite::create();
        _tag->setPosition(getContentSize().width / 2, getContentSize().height / 2);
        this->addChild(_tag, 1);
    }
    //创建玩家标记
    auto flag = Sprite::create("flag_bomb.png");
    flag->setPosition(0, 0);
    _tag->addChild(flag, 1);
}

//重置格子的参数
bool MBox::resetAttr(int n, MBombType bt)
{
    //判断是否有变化
    if (_attribe.num != n || _attribe.bombtype != bt)
    {
        _attribe.num = n;
        _attribe.bombtype = bt;
        return true;
    }
    return false;
}
//是否已掀开
bool MBox::isOpened(void)
{
    if (nullptr == _cover)
    {
        return true;
    }
    return false;
}
//掀开格子并显示属性
void MBox::openAndDoAtrrib(void)
{
    //阐述覆盖层
    coverDel();
    //删除后处理
    doAttrRet();
}
//重新合上格子
void MBox::closeAndRecover(void)
{
    if (nullptr != _cover)
    {
        return;
    }
    auto as = ScaleTo::create(0.1f, 1, 0);
    auto asb = ScaleTo::create(0.1f, 1, 1);
    auto af = CallFuncN::create(CC_CALLBACK_1(MBox::closeInMiddleCallback, this));
    this->runAction(Sequence::create(as, af, asb, NULL));
}
//重新合上格子的中间阶段的回调函数
void MBox::closeInMiddleCallback(cocos2d::Node* sender)
{
    //删除所有标记
    this->showTag(false);
    //恢复覆盖层
    auto pSpriteText = Director::getInstance()->getTextureCache()->getTextureForKey("white.png");
    _cover = Sprite::createWithTexture(pSpriteText);
    _cover->setPosition(this->getContentSize().width / 2, this->getContentSize().height / 2);
    this->addChild(_cover, 2);
}

