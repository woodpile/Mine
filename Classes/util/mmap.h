#ifndef __M_UTIL_MMAP_H__
#define __M_UTIL_MMAP_H__

#include <vector>

struct BoxAttr {
    int num;
    int bombtype;
};

class Mmap
{
public:
    static std::vector<std::vector<BoxAttr> > getMmap(int n);
};

#endif // __M_UTIL_MMAP_H__