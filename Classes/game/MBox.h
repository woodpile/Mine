#ifndef __M_BOX_H__
#define __M_BOX_H__

#include "cocos2d.h"
#include "GameScene.h"

//炸弹类型定义
enum MBombType {
    NOBOMB,
    NORMAL,
}; 
//格子属性定义
struct MBoxAttr {
    int num;
    MBombType bombtype;
};
//格子类
class MBox : public cocos2d::Sprite
{
public:
    MBox();
    ~MBox();
    
    //创建格子
    static MBox* create(void);
    static MBox* createWithTexture(cocos2d::Texture2D *pTexture);
    //设置格子所处的游戏场景
    void setGame(GameScene* pGame);
    //格子的参数设置
    void setBoxId(int boxid);
    void setMPos(int x, int y);
    void setAttr(int n, MBombType bt);
    //引擎 初始化与去初始化
    virtual void onEnter(void);
    virtual void onExit(void);
    //引擎 触摸行为注册函数
    virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    virtual void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
    
    //重置格子的参数
    bool resetAttr(int n, MBombType bt);
    //是否已掀开
    bool isOpened(void);
    //掀开格子并显示属性
    void openAndDoAtrrib(void);
    //重新合上格子
    void closeAndRecover(void);

private:
    //覆盖层滑动 随滑动改变形状
    void slideCover(cocos2d::Touch* touch);
    //覆盖层点击
    void coverClick(void);
    //覆盖层滑动松开后处理
    void coverDrag(cocos2d::Touch* touch);
    //覆盖层删除
    void coverDel(void);
    //覆盖层滑动返回
    void coverSlideBack(void);
    //覆盖层滑动返回后的回调函数
    void slideCoverBackCallback(cocos2d::Node* sender);
    //覆盖层恢复
    void coverRevocer(void);
    
    //重新合上格子的中间阶段的回调函数
    void closeInMiddleCallback(cocos2d::Node* sender);
    
    //处理标记显示或隐藏
    void showTag(bool show);
    //显示属性标记
    void showAttrTag(void);
    //处理属性标记效果
    void doAttrRet(void);
    //玩家标记为炸弹
    void flagToBomb(void);

    //成员数据 格子的ID
    int _box_id;
    //成员数据 格子的外在逻辑坐标
    int _box_x;
    int _box_y;
    //成员数据 格子的逻辑属性
    MBoxAttr _attribe;
    //成员对象 标记底板
    cocos2d::Sprite* _tag;
    //成员对象 覆盖层
    cocos2d::Sprite* _cover;
    //成员对象 格子基层锚点
    cocos2d::Point _basePos;
    //成员对象 触摸移动的起始点 活动数据
    cocos2d::Point _moveStartPos;
    
    //成员对象 所处游戏场景
    GameScene* _game;
};

#endif // __M_BOX_H__
