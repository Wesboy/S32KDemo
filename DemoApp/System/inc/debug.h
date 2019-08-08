#ifndef __DEBUG_H__
#define __DEBUG_H__

typedef int (*DBGPrint)(const char *fmt, ...);

int DebugPrintToAndroidFile(const char *fmt, ...);
int DebugPrintToAndroid(const char *fmt, ...);

#endif
