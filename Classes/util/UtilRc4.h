//
//  UtilRc4.h
//  Mine
//
//  Created by woodpile on 14-8-27.
//  From openssl rc4
//

#ifndef _M_UtilRc4_
#define _M_UtilRc4_

#include <stddef.h>

#define mineRC4_INT unsigned char
#define mineRC4_CHUNK unsigned long

#ifdef  __cplusplus
extern "C" {
#endif
    
    typedef struct rc4_key_st
	{
        mineRC4_INT x,y;
        mineRC4_INT data[256];
	} mineRC4_KEY;
    
    void mineRC4_set_key(mineRC4_KEY *key, int len, const unsigned char *data);
    void private_mineRC4_set_key(mineRC4_KEY *key, int len, const unsigned char *data);
    void mineRC4(mineRC4_KEY *key, size_t len, const unsigned char *indata,
             unsigned char *outdata);
    
#ifdef  __cplusplus
}
#endif


#endif
