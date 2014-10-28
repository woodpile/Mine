//
//  UtilRc4.c
//  Mine
//
//  Created by woodpile on 14-8-27.
//  From openssl rc4
//

#include "UtilRc4.h"

void mineRC4_set_key(mineRC4_KEY *key, int len, const unsigned char *data)
{
	private_mineRC4_set_key(key, len, data);
}

void private_mineRC4_set_key(mineRC4_KEY *key, int len, const unsigned char *data)
{
    register mineRC4_INT tmp;
    register int id1,id2;
    register mineRC4_INT *d;
    unsigned int i;
    
    d= &(key->data[0]);
    key->x = 0;
    key->y = 0;
    id1=id2=0;
    
#define SK_LOOP(d,n) { \
tmp=d[(n)]; \
id2 = (data[id1] + tmp + id2) & 0xff; \
if (++id1 == len) id1=0; \
d[(n)]=d[id2]; \
d[id2]=tmp; }
    
	for (i=0; i < 256; i++) d[i]=i;
	for (i=0; i < 256; i+=4)
    {
		SK_LOOP(d,i+0);
		SK_LOOP(d,i+1);
		SK_LOOP(d,i+2);
		SK_LOOP(d,i+3);
    }
}


void mineRC4(mineRC4_KEY *key, size_t len, const unsigned char *indata,
	     unsigned char *outdata)
{
    register mineRC4_INT *d;
    register mineRC4_INT x,y,tx,ty;
	size_t i;
    
    x=key->x;
    y=key->y;
    d=key->data;
    
#if defined(mineRC4_CHUNK)
    
# define mineRC4_STEP	( \
x=(x+1) &0xff,	\
tx=d[x],	\
y=(tx+y)&0xff,	\
ty=d[y],	\
d[y]=tx,	\
d[x]=ty,	\
(mineRC4_CHUNK)d[(tx+ty)&0xff]\
)
    
	if ( ( ((size_t)indata  & (sizeof(mineRC4_CHUNK)-1)) |
          ((size_t)outdata & (sizeof(mineRC4_CHUNK)-1)) ) == 0 )
    {
		mineRC4_CHUNK ichunk,otp;
		const union { long one; char little; } is_endian = {1};
        
		if (!is_endian.little)
        {	/* BIG-ENDIAN CASE */
# define BESHFT(c)	(((sizeof(mineRC4_CHUNK)-(c)-1)*8)&(sizeof(mineRC4_CHUNK)*8-1))
			for (;len&(0-sizeof(mineRC4_CHUNK));len-=sizeof(mineRC4_CHUNK))
            {
				ichunk  = *(mineRC4_CHUNK *)indata;
				otp  = mineRC4_STEP<<BESHFT(0);
				otp |= mineRC4_STEP<<BESHFT(1);
				otp |= mineRC4_STEP<<BESHFT(2);
				otp |= mineRC4_STEP<<BESHFT(3);
				if (sizeof(mineRC4_CHUNK)==8)
                {
					otp |= mineRC4_STEP<<BESHFT(4);
					otp |= mineRC4_STEP<<BESHFT(5);
					otp |= mineRC4_STEP<<BESHFT(6);
					otp |= mineRC4_STEP<<BESHFT(7);
                }
				*(mineRC4_CHUNK *)outdata = otp^ichunk;
				indata  += sizeof(mineRC4_CHUNK);
				outdata += sizeof(mineRC4_CHUNK);
            }
			if (len)
            {
				mineRC4_CHUNK mask=(mineRC4_CHUNK)-1, ochunk;
                
				ichunk = *(mineRC4_CHUNK *)indata;
				ochunk = *(mineRC4_CHUNK *)outdata;
				otp = 0;
				i = BESHFT(0);
				mask <<= (sizeof(mineRC4_CHUNK)-len)<<3;
				switch (len&(sizeof(mineRC4_CHUNK)-1))
                {
					case 7:	otp  = mineRC4_STEP<<i, i-=8;
					case 6:	otp |= mineRC4_STEP<<i, i-=8;
					case 5:	otp |= mineRC4_STEP<<i, i-=8;
					case 4:	otp |= mineRC4_STEP<<i, i-=8;
					case 3:	otp |= mineRC4_STEP<<i, i-=8;
					case 2:	otp |= mineRC4_STEP<<i, i-=8;
					case 1:	otp |= mineRC4_STEP<<i, i-=8;
					case 0: ; /*
                               * it's never the case,
                               * but it has to be here
                               * for ultrix?
                               */
                }
				ochunk &= ~mask;
				ochunk |= (otp^ichunk) & mask;
				*(mineRC4_CHUNK *)outdata = ochunk;
            }
			key->x=x;
			key->y=y;
			return;
        }
		else
        {	/* LITTLE-ENDIAN CASE */
# define LESHFT(c)	(((c)*8)&(sizeof(mineRC4_CHUNK)*8-1))
			for (;len&(0-sizeof(mineRC4_CHUNK));len-=sizeof(mineRC4_CHUNK))
            {
				ichunk  = *(mineRC4_CHUNK *)indata;
				otp  = mineRC4_STEP;
				otp |= mineRC4_STEP<<8;
				otp |= mineRC4_STEP<<16;
				otp |= mineRC4_STEP<<24;
				if (sizeof(mineRC4_CHUNK)==8)
                {
					otp |= mineRC4_STEP<<LESHFT(4);
					otp |= mineRC4_STEP<<LESHFT(5);
					otp |= mineRC4_STEP<<LESHFT(6);
					otp |= mineRC4_STEP<<LESHFT(7);
                }
				*(mineRC4_CHUNK *)outdata = otp^ichunk;
				indata  += sizeof(mineRC4_CHUNK);
				outdata += sizeof(mineRC4_CHUNK);
            }
			if (len)
            {
				mineRC4_CHUNK mask=(mineRC4_CHUNK)-1, ochunk;
                
				ichunk = *(mineRC4_CHUNK *)indata;
				ochunk = *(mineRC4_CHUNK *)outdata;
				otp = 0;
				i   = 0;
				mask >>= (sizeof(mineRC4_CHUNK)-len)<<3;
				switch (len&(sizeof(mineRC4_CHUNK)-1))
                {
					case 7:	otp  = mineRC4_STEP,    i+=8;
					case 6:	otp |= mineRC4_STEP<<i, i+=8;
					case 5:	otp |= mineRC4_STEP<<i, i+=8;
					case 4:	otp |= mineRC4_STEP<<i, i+=8;
					case 3:	otp |= mineRC4_STEP<<i, i+=8;
					case 2:	otp |= mineRC4_STEP<<i, i+=8;
					case 1:	otp |= mineRC4_STEP<<i, i+=8;
					case 0: ; /*
                               * it's never the case,
                               * but it has to be here
                               * for ultrix?
                               */
                }
				ochunk &= ~mask;
				ochunk |= (otp^ichunk) & mask;
				*(mineRC4_CHUNK *)outdata = ochunk;
            }
			key->x=x;
			key->y=y;
			return;
        }
    }
#endif
#define LOOP(in,out) \
x=((x+1)&0xff); \
tx=d[x]; \
y=(tx+y)&0xff; \
d[x]=ty=d[y]; \
d[y]=tx; \
(out) = d[(tx+ty)&0xff]^ (in);
    
#ifndef mineRC4_INDEX
#define mineRC4_LOOP(a,b,i)	LOOP(*((a)++),*((b)++))
#else
#define mineRC4_LOOP(a,b,i)	LOOP(a[i],b[i])
#endif
    
	i=len>>3;
	if (i)
    {
		for (;;)
        {
			mineRC4_LOOP(indata,outdata,0);
			mineRC4_LOOP(indata,outdata,1);
			mineRC4_LOOP(indata,outdata,2);
			mineRC4_LOOP(indata,outdata,3);
			mineRC4_LOOP(indata,outdata,4);
			mineRC4_LOOP(indata,outdata,5);
			mineRC4_LOOP(indata,outdata,6);
			mineRC4_LOOP(indata,outdata,7);
#ifdef mineRC4_INDEX
			indata+=8;
			outdata+=8;
#endif
			if (--i == 0) break;
        }
    }
	i=len&0x07;
	if (i)
    {
		for (;;)
        {
			mineRC4_LOOP(indata,outdata,0); if (--i == 0) break;
			mineRC4_LOOP(indata,outdata,1); if (--i == 0) break;
			mineRC4_LOOP(indata,outdata,2); if (--i == 0) break;
			mineRC4_LOOP(indata,outdata,3); if (--i == 0) break;
			mineRC4_LOOP(indata,outdata,4); if (--i == 0) break;
			mineRC4_LOOP(indata,outdata,5); if (--i == 0) break;
			mineRC4_LOOP(indata,outdata,6); if (--i == 0) break;
        }
    }
	key->x=x;
	key->y=y;
}

