#ifndef __M_MPAGE_H__
#define __M_MPAGE_H__

#include "cocos2d.h"
#include "GameScene.h"

//页面类
class MPage : public cocos2d::Sprite
{
public:
    MPage();
    ~MPage();
    
    //创建页面
    static MPage* create(void);
    
    //引擎 初始化与去初始化
    virtual void onEnter(void);
    virtual void onExit(void);
    //引擎 触摸行为注册函数
    virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    virtual void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);

    //设置页面的逻辑坐标
    void setPagePos(int w, int h);

    //设置页面的进度百分比
    void setOpenPercent(double p);
    //设置页面是否已经有人占用
    void setPageOwn(bool hasown);
    //查询页面是否已经有人占用
    bool getPageOwn(void);

private:
    //成员对象 页面的逻辑坐标
    int _page_w, _page_h;
    //成员对象 页面是否被占用
    bool _hasown;
    //成员对象 页面进度百分比
    double _open_precent;

    //成员对象 百分比显示标签
    cocos2d::Label* _label_precent;
};

#endif // __M_MPAGE_H__
