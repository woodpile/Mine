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
    virtual void menuCloseCallback(cocos2d::Ref* pSender);
    //重新开始菜单键的回调函数
    void menuRestartCallback(cocos2d::Ref* pSender);
    
    //格子被点击
    virtual bool boxBeClick(int w, int h);
    //格子被标记
    virtual void boxBeFlag(int w, int h);
    //重新扩散一些盒子
    virtual void reDiffusion(void);

    //引擎
    CREATE_FUNC(GameScene);
    
protected:
    const int BASE_BOX_ID = 1000;
    
    //创建一个新的游戏
    virtual void createNewGame(void);

    //初始化帮助信息界面
    virtual void initHelpInfoBoard(void);
    //初始化分数面板
    virtual void initScoreInfoBoard(void);

    //成员对象 背景精灵
    cocos2d::Sprite* _backgroud;
    //成员对象 分数标签
    cocos2d::Label* _score_label;

private:
    //处理一个有炸弹的格子被点击
    void procClickBoxBang(int w, int h);
    //处理一个安全的格子被点击
    void procClickBoxSafe(int w, int h);
    //扩散一个格子
    void procDiffusionBox(int w, int h, bool force = false);
    //改变一些格子
    void changeSomeBoxes(void);
    
    //增加分数
    void addScore(int n);
    //重置分数
    void resetScore(void);
    
    //成员数据 已标记的格子
    std::vector<std::pair<int, int>> _flaged_box;
    //成员数据 已重新扩散标记
    bool _flag_reDifusion;
    //成员对象 地图信息
    MMap* _map;
    //成员对象 分数
    int _score;
};

#endif // __M_GAMESCENE_H__
