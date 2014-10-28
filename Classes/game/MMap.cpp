//
//  MMap.cpp
//  Mine
//
//  Created by woodpile on 14-8-8.
//
//

#include "stdlib.h"
#include "time.h"
#include "MMap.h"

using std::vector;
using std::pair;

MMap::MMap(int width, int heigh)
: _width(width), _heigh(heigh), _bomb_prob(5000), _bomb_prob_inc(false), _min_bomb_num(4)
{
    vector<MMapData> tmp(_width * _heigh);
    _mapdata = tmp;

    return;
}

MMap::~MMap(void)
{
    return;
}

//随机重刷地图
void MMap::randRefresh(void)
{
    srand((unsigned int)time(nullptr));
    
    int max = _width * _heigh;
    for (int i = 0; i < _mapdata.size(); i++)
    {
        _mapdata[i].bomb = 0;
    }
    for (int n = 0; n < _min_bomb_num;)
    {
        auto sd = rand();
        int i = sd % max;
        
        if (0 != _mapdata[i].bomb)
        {
            continue;
        }
        _mapdata[i].bomb = 1;
        n += 1;
    }
    this->proMapNum();
    
//    this->printMap();
}

//计算地图格子的数字
void MMap::proMapNum(void)
{
    for (int i = 0; i < _width * _heigh; i++)
    {
        int h = i / _width;
        int w = i % _width;
        _mapdata[i].num = this->proOneNum(w, h);
    }
}

//计算一个格子的数字
int MMap::proOneNum(int w, int h)
{
    int num = 0;
    
    //计算上一行
    if (0 < h && 0 < w && 0 < _mapdata[_width * (h - 1) + (w - 1)].bomb)
    {
        num += 1;
    }
    if (0 < h && 0 < _mapdata[_width * (h - 1) + (w)].bomb)
    {
        num += 1;
    }
    if (0 < h && w < (_width - 1) && 0 < _mapdata[_width * (h - 1) + (w + 1)].bomb)
    {
        num += 1;
    }
    //计算当前行
    if (0 < w && 0 < _mapdata[_width * (h) + (w - 1)].bomb)
    {
        num += 1;
    }
    if (w < (_width - 1) && 0 < _mapdata[_width * (h) + (w + 1)].bomb)
    {
        num += 1;
    }
    //计算下一行
    if (h < (_heigh - 1) && 0 < w && 0 < _mapdata[_width * (h + 1) + (w - 1)].bomb)
    {
        num += 1;
    }
    if (h < (_heigh - 1) && 0 < _mapdata[_width * (h + 1) + (w)].bomb)
    {
        num += 1;
    }
    if (h < (_heigh - 1) && w < (_width - 1) && 0 < _mapdata[_width * (h + 1) + (w + 1)].bomb)
    {
        num += 1;
    }
    
    
    return num;
}

//设置炸弹出现的概率
void MMap::setBombProbability(int p, bool inc)
{
    _bomb_prob = p;
    _bomb_prob_inc = inc;
}

//取地图上一个点的属性
MMapData MMap::getDate(int w, int h)
{
    auto pos = h * _width + w;
    return _mapdata[pos];
}

//重新生成一个格子的内容
void MMap::reduceOne(int index)
{
    auto sd = rand();
    if ((sd % 10000) < _bomb_prob)
    {
        _mapdata[index].bomb = 1;
    }
    else{
        _mapdata[index].bomb = 0;
    }
}
//由一个格子引起地图变动
void MMap::changeMap(int w, int h, int ex_num)
{
    //炸弹出现概率增加
    //_bomb_prob += 1000;
    
    //重新生成目标格子
    int i = w + h * _width;
    this->reduceOne(i);
    
    //随机寻找被牵动的格子并重新生成
    int max = _width * _heigh;
    for (int t = 0; t < ex_num; t++)
    {
        auto sd = rand();
        i = sd % max;
        this->reduceOne(i);
    }
    
    //补足炸弹的数量
    this->fillEnoughBomb();
    
    //重新计算格子数字
    this->proMapNum();
    
//    this->printMap();
}
//补充炸弹数量
void MMap::fillEnoughBomb(void)
{
    //统计当前炸弹的数量
    int len = _width * _heigh;
    int num = 0;
    for (int i = 0; i < len; i++)
    {
        if (0 != _mapdata[i].bomb)
        {
            num += 1;
        }
    }
    //补充炸弹数量到下限
    while (num < _min_bomb_num)
    {
        auto sd = rand();
        int i = sd % len;
        
        if (0 != _mapdata[i].bomb)
        {
            continue;
        }
        _mapdata[i].bomb = 1;
        num += 1;
    }
}

//在日志中打印地图内容
//void MMap::printMap(void)
//{
//    //打印bomb
//    printf("bomb\n");
//    for (int i = 0; i < _heigh; i++)
//    {
//        for (int j = 0; j < _width; j++)
//        {
//            int pos = i * _width + j;
//            printf("%d ", _mapdata[pos].bomb);
//        }
//        printf("\n");
//    }
//    
//    //打印num
//    printf("num\n");
//    for (int i = 0; i < _heigh; i++)
//    {
//        for (int j = 0; j < _width; j++)
//        {
//            int pos = i * _width + j;
//            printf("%d ", _mapdata[pos].num);
//        }
//        printf("\n");
//    }
//}
