#ifndef __M_GAMESCENE_H__
#define __M_GAMESCENE_H__

#include "cocos2d.h"
#include "MMap.h"

class GameScene : public cocos2d::Layer
{
public:
    //生成游戏场景
    static cocos2d::Scene* createScene();

    //引擎 初始化和去初始化
    virtual bool init();
    virtual void onEnter();
    virtual void onExit();

    //退出菜单键的回调函数
    void menuCloseCallback(cocos2d::Ref* pSender);
    //重新开始菜单键的回调函数
    void menuRestartCallback(cocos2d::Ref* pSender);
    
    //格子被点击
    void boxBeClick(int boxid);
    //格子被标记
    void boxBeFlag(int w, int h);

    //引擎
    CREATE_FUNC(GameScene);

private:
    const int BASE_BOX_ID = 1000;
    
    //创建一个新的游戏
    void createNewGame(void);
    //初始化格子们
    void initBoxes(void);
    
    //处理一个有炸弹的格子被点击
    void procClickBoxBang(int w, int h);
    //处理一个安全的格子被点击
    void procClickBoxSafe(int w, int h);
    //扩散一个格子
    void procDiffusionBox(int w, int h);
    //改变一些格子
    void changeSomeBoxes(void);
    
    //增加分数
    void addScore(int n);
    //重置分数
    void resetScore(void);
    
    //成员数据 已标记的格子
    std::vector<std::pair<int, int>> _flaged_box;
    //成员对象 背景精灵
    cocos2d::Sprite* _backgroud;
    //成员对象 分数标签 分数
    cocos2d::Label* _score_label;
    int _score;
    //成员对象 地图信息
    MMap* _map;
};

#endif // __M_GAMESCENE_H__
