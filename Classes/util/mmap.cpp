#include "mmap.h"

using namespace std;

vector<vector<BoxAttr> > Mmap::getMmap(int n)
{
    char mn[3][3] = {
        {1, 1, 1},
        {1, 1, 1},
        {1, 1, 1}
    };
    char mb[3][3] = {
        { 0, 0, 0 },
        { 0, 1, 0 },
        { 0, 0, 0 }
    };
    int lx = 3;
    int ly = 3;

    vector<vector<BoxAttr> > m;
    for (int i = 0; i < lx; i++)
    {
        vector<BoxAttr> vb;
        for (int j = 0; j < ly; j++)
        {
            BoxAttr b;
            b.num = mn[i][j];
            b.bombtype = mb[i][j];
            vb.push_back(b);
        }
        m.push_back(vb);
    }

    return m;
}
