#include "MBox.h"
#include "GameScene.h"
#include "config/Config.h"
#include "util/matrix.h"

USING_NS_CC;

enum {
    TAG_TAG_NUM = 100,
    TAG_TAG_BOMB,
};

MBox::MBox(void)
: _box_x(0), _box_y(0), _basePos(Point(0, 0)), _moveStartPos(Point(0, 0)),
_bigger(false), _tag(nullptr), _cover(nullptr)
{
    return;
}

MBox::~MBox(void)
{
    return;
}

MBox* MBox::create(void)
{
    auto *pBox = new MBox();
    pBox->initWithFile("blue.png");
    pBox->autorelease();

    return pBox;
}

MBox* MBox::createWithTexture(Texture2D *pTexture)
{
    auto *pBox = new MBox();
    pBox->initWithTexture(pTexture);
    pBox->autorelease();

    return pBox;
}

void MBox::onEnter(void)
{
    Sprite::onEnter();

    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);

    listener->onTouchBegan = CC_CALLBACK_2(MBox::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(MBox::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(MBox::onTouchEnded, this);

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    auto cs = this->getContentSize() / 2;

    auto pSpriteText = Director::getInstance()->getTextureCache()->getTextureForKey("white.png");
    _cover = Sprite::createWithTexture(pSpriteText);
    _cover->setPosition(cs.width, cs.height);
    this->addChild(_cover, 2);

    return;
}

void MBox::onExit(void)
{
    Sprite::onExit();

    return;
}

bool MBox::onTouchBegan(Touch* touch, Event* event)
{
    if (nullptr == _cover)
    {
        return false;
    }
    Point point = _cover->convertTouchToNodeSpaceAR(touch);
    auto s = _cover->getContentSize();
    bool ret = Rect(-s.width / 2, -s.height / 2, s.width, s.height).containsPoint(point);

    if (true == ret)
    {
        this->setLocalZOrder(1000);
        _moveStartPos = touch->getLocation();
        _basePos = this->getAnchorPointInPoints();
    }

    return ret;
}

void MBox::onTouchMoved(Touch* touch, Event* event)
{
    if (false == _bigger)
    {
        slideCover(touch);
    }
    else
    {
        drawPath(touch);
    }

    return;
}

void MBox::onTouchEnded(Touch* touch, Event* event)
{
    if (nullptr == _cover)
    {
        return;
    }

    if (3 > touch->getLocation().getDistance(_moveStartPos))
    {
        coverClick();
    }
    else
    {
        coverDrag(touch);
    }
    this->setLocalZOrder(1);

    return;
}

void MBox::slideCover(Touch* touch)
{
    auto tp = touch->getLocation();
    auto h = _cover->getContentSize().height * 0.75;
    if (tp.y > _moveStartPos.y + h)
    {
        tp.y = _moveStartPos.y + h;
    }
    else if (tp.y < _moveStartPos.y - h)
    {
        tp.y = _moveStartPos.y - h;
    }

    auto dy = tp.y - _moveStartPos.y;

    _cover->setScaleY((2 * h - abs(dy)) / (2 * h));
    _cover->setPositionY(dy + _basePos.y);

    if (dy == h || dy == -h)
    {
        showTag(true);
    }

    return;
}

void MBox::drawPath(Touch* touch)
{
    auto p = _cover->convertTouchToNodeSpace(touch);
    auto s = _cover->getContentSize();
    if (p.x < 0 || p.y < 0 || p.x > s.width || p.y > s.height)
    {
        return;
    }

    auto d = Sprite::create("pro_green.png");
    d->setScale(0.75);
    d->setPosition(p);
    _cover->addChild(d);

    Matrix::addM(_tune, 10, _cover->getContentSize().width, p.x, p.y);
}

void MBox::showTag(bool show)
{
    if (true == show && nullptr == _tag)
    {
        _tag = Sprite::create();
        _tag->setPosition(getContentSize().width / 2, getContentSize().height / 2);
        this->addChild(_tag, 1);
        showAttrTag();
    }
    else if (false == show)
    {
        if (nullptr != _tag)
        {
            _tag->removeFromParent();
            _tag = nullptr;
        }
    }
}

void MBox::slideCoverBackCallback(cocos2d::Node* sender)
{
    this->showTag(false);
}

void MBox::coverClick(void)
{
    if (false == _bigger)
    {
        coverToBigger();
    }
    else
    {
        coverToNoBigger();
    }
}

void MBox::coverDrag(Touch* touch)
{
    _cover->runAction(MoveTo::create(0.2f, _basePos));

    if (false == _bigger)
    {
        if (_moveStartPos.y - touch->getLocation().y > _cover->getContentSize().height * 0.75)
        {
            coverDel();
        }
        else
        {
            coverSlideBack();
        }
    }
    else
    {
        matchTune();
    }
}

void MBox::coverDel(void)
{
    _cover->removeFromParent();
    _cover = nullptr;
    doAttrRet();
}

void MBox::coverToBigger(void)
{
    memset(_tune, 0, 100);
    _bigger = true;
    _cover->runAction(ScaleTo::create(0.2f, 4));
}

void MBox::coverToNoBigger(void)
{
    _bigger = false;
    _cover->removeAllChildren();
    _cover->runAction(ScaleTo::create(0.2f, 1));
}

void MBox::coverSlideBack(void)
{
    auto action = Sequence::create(
        ScaleTo::create(0.2f, 1),
        CallFuncN::create(CC_CALLBACK_1(MBox::slideCoverBackCallback, this)),
        nullptr);
    _cover->runAction(action);
}

void MBox::matchTune(void)
{
    auto rf = Matrix::match(_tune, 10);
    log("rf -----> %f.", rf);
    if (rf < 0.8)
    {
        memset(_tune, 0, 100);
        _cover->removeAllChildren();
    }
}


//属性有关的操作
void MBox::setMPos(int x, int y)
{
    _box_x = x;
    _box_y = y;
}

void MBox::setAttr(int n, MBombType bt)
{
    _attribe.num = n;
    _attribe.bombtype = bt;
}

void MBox::showAttrTag(void)
{
    if (nullptr == _tag)
    {
        return;
    }

    if (0 != _attribe.num)
    {
        auto sn = String::createWithFormat("%d", _attribe.num);
        auto tn = Label::createWithTTF(sn->getCString() , CF_F("font_hei"), 27);
        tn->setPosition(0, 0);
        _tag->addChild(tn, 1, TAG_TAG_NUM);
    }

    return;
}

void MBox::doAttrRet(void)
{
    if (MBombType::NO != _attribe.bombtype)
    {
        _tag->removeChildByTag(TAG_TAG_NUM);
        auto tb = Sprite::create("lock.png");
        tb->setPosition(0, 0);
        _tag->addChild(tb);
        this->setColor(Color3B::RED);
    }
}

