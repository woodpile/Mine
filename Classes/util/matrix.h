#ifndef __M_UTIL_MATRIX_H__
#define __M_UTIL_MATRIX_H__

class Matrix
{
public:
    static void addM(char *m, int l, float w, float x, float y);
    static float match(char *m, int l);
    static void compress2(char *m, int l, char *mb, int lb);
    static float withT(char *mb, int lb);
};

#endif // __M_UTIL_MATRIX_H__
