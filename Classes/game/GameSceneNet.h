//
//  GameSceneNet.h
//  Mine
//
//  Created by woodpile on 14-8-25.
//
//

#ifndef __M_GameSceneNet__
#define __M_GameSceneNet__

#include "GameScene.h"
#include "MBox.h"

struct NetBoxAttr
{
    int width;
    int hight;
    int num;
    bool isopen;
    int bomb;
    bool isflag;
};

class GameSceneNet: public GameScene
{
public:
    GameSceneNet();
    ~GameSceneNet();
    
    //生成游戏场景
    static cocos2d::Scene* createScene(int w, int h, int maxboxw, int maxboxh);
    //获取游戏场景实例
    static GameSceneNet* getInstance();

    //退出菜单键的回调函数
    virtual void menuCloseCallback(cocos2d::Ref* pSender);
    
    //格子被点击
    virtual bool boxBeClick(int w, int h);
    //格子被标记
    virtual void boxBeFlag(int w, int h);

    //更新格子信息
    void updateBoxs(int pw, int ph, NetBoxAttr aboxs[], int arrlen);
    //点击格子的返回信息
    void callbackClickBox(int num, int bomb, NetBoxAttr aboxs[] = nullptr, int arrlen = 0);
    //插旗格子的返回信息
    void callbackFlagBox(int num, int bomb, NetBoxAttr aboxs[] = nullptr, int arrlen = 0);

    //重新扩散一些盒子
    virtual void reDiffusion(void);
    
    //引擎 触摸行为注册函数
    virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    virtual void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
    
    //引擎
    CREATE_FUNC(GameSceneNet);
    
protected:
    //创建一个新的游戏
    virtual void createNewGame(void);
    
    //初始化帮助信息界面
    virtual void initHelpInfoBoard(void);
    //初始化分数面板
    virtual void initScoreInfoBoard(void);

private:
    static GameSceneNet* _pNetGameInstance;

    //设置地图页面的值
    void setPagePos(int w, int h);
    
    //加载游戏环境
    void loadGameGround(void);
    //刷新需要进入显示和进入隐藏的格子
    void refreshShowBoxes(void);

    //本页的页面坐标
    int _page_w, _page_h;
    
    //当前作为已显示区域中心格子的地图坐标
    int _center_w, _center_h;
    //本页地图的尺寸
    int _page_maxw, _page_maxh;
    
    //是否可以滑动的标记
    bool _bCanTouch;

    //当前点击的格子tagid
    int _boxid_beClicked;

    //坐标为0,0的格子的position
    cocos2d::Point _box_start;
    //坐标为M,M的格子的position
    cocos2d::Point _box_end;
    
    cocos2d::Point _edge00, _edgeMM;

    cocos2d::Sprite* _wheel;
    cocos2d::Point _wheel_base_pos;
    cocos2d::Point _wheel_moveStartPos;
    //cocos2d::Map<int, MBox*> _map;
};


#endif /* defined(__M_GameSceneNet__) */
