#include "stdlib.h"
#include "matrix.h"

#include "cocos2d.h"

char mb[100] = { 0 };

void showMatrix(char *m, int l)
{
    for (int i = 0; i < l; i++)
    {
        for (int j = 0; j < l; ++j)
        {
            //cocos2d::log("pos %d %d -> %d.", i, j, *(m + i * l + j));
        }
    }
}

void Matrix::addM(char *m, int l, float w, float x, float y)
{
	float dw = w / l;
	int ix = (int)(x / dw);
	int iy = (int)((w - y) / dw);
	*(m + iy * l + ix) = 1;
}

float Matrix::match(char *m, int l)
{
	if (0 != (l % 2))
	{
		return false;
	}

	int lb = l / 2;

    memset(mb, 0, 100);
	showMatrix(m, l);
	Matrix::compress2(m, l, mb, lb);
	showMatrix(mb, lb);

	auto r = Matrix::withT(mb, lb);

    return r;
}

void Matrix::compress2(char *m, int l, char *mb, int lb)
{
	for (int i = 0; i < l; i += 2)
	{
		for (int j = 0; j < l; j += 2)
		{
			if (0 != *(m + i * l + j) ||
				0 != *(m + i * l + j + 1) ||
				0 != *(m + i * l + l + j) ||
				0 != *(m + i * l + l + j + 1))
			{
				*(mb + i / 2* lb + j / 2) = 1;
			}
            else
            {
                *(mb + i / 2 * lb + j / 2) = 0;
            }
		}
	}
}

float Matrix::withT(char *mb, int lb)
{
	int T[5][5] = {
		0, 0, 0, 0, 0,
		1, 1, 1, 1, 1,
		0, 0, 1, 0, 0,
		0, 0, 1, 0, 0,
		0, 0, 1, 0, 0
	};

	int nm = 0;
	int ns = 8;
	for (int i = 0; i < lb; ++i)
	{
		for (int j = 0; j < lb; ++j)
		{
			if (1 == *(mb + i * lb + j) && 1 == T[i][j])
			{
				nm += 1;
			}
			if (*(mb + i * lb + j) != T[i][j])
			{
				ns += 1;
			}
		}
	}

	return (float)(nm) / (float)(ns);
}
