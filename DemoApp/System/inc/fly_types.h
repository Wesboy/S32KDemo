#ifndef __FLY_TYPES_H__
#define __FLY_TYPES_H__

#include "Lpc_types.h"


// MIN/MAX/ABS macros
#define SMALLER(a, b)			(((a) < (b))?(a):(b))
#define LARGER(a, b)			(((a) > (b))?(a):(b))
#define ABS(x)					((x > 0)?(x):(-x))


typedef Bool BOOL;

typedef unsigned char  BYTE;
typedef unsigned int   UINT;
typedef unsigned short WORD;
typedef unsigned short UINT16;
typedef unsigned int  UINT32;
typedef unsigned int  DWORD;
typedef unsigned int  ULONG;

typedef unsigned char  U8;
typedef unsigned char  u8;
typedef unsigned short U16;
typedef unsigned short u16;
typedef unsigned int  U32;
typedef unsigned int  u32;

typedef unsigned char  Uint8;
typedef unsigned short Uint16;
typedef unsigned int  Uint32;

typedef unsigned char  uint8;
typedef unsigned short uint16;
typedef unsigned int  uint32;

typedef unsigned char  uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;

typedef unsigned int event_t;

typedef enum {ACC_OFF = 0, ACC_ON = !ACC_OFF}ACC;


#endif
