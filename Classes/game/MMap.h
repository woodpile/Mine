//
//  MMap.h
//  Mine
//
//  Created by woodpile on 14-8-8.
//
//

#ifndef __M_MMap_H_
#define __M_MMap_H_

#include <vector>
#include <utility>

struct MMapData {
    int num;
    int bomb;
};

class MMap {
public:
    MMap(int width, int heigh);
    ~MMap(void);
    
    //随机重刷地图
    void randRefresh(void);
    //设置炸弹出现的概率
    void setBombProbability(int p, bool inc = false);
    
    //取地图上一个点的属性
    MMapData getDate(int posw, int posh);
    
    //由一个格子引起地图变动
    void changeMap(int w, int h, int ex_num);
    
    //成员数据 地图的长宽高
    int _width;
    int _heigh;
private:
    //在日志中打印地图内容
    void printMap(void);
    
    //计算地图格子的数字
    void proMapNum(void);
    //计算一个格子的数字
    int proOneNum(int w, int h);
    //重新生成一个格子的内容
    void reduceOne(int index);
    //补充炸弹数量
    void fillEnoughBomb(void);
    
    //成员数据 炸弹数量的下限
    int _min_bomb_num;
    //成员数据 地图数据
    std::vector<MMapData> _mapdata;
    //成员数据 炸弹出现的概率
    int _bomb_prob;
    bool _bomb_prob_inc;
};
#endif /* defined(__M_MMap_H_) */
