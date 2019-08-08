#ifndef __DEBUGDEMO_H__
#define __DEBUGDEMO_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define  OSD_DEBUG_WIDTH	80
#define  OSD_DEBUG_LINES	12

typedef enum
{
	OSD_SPEC_0 = 0,
	OSD_VERSION,
	OSD_SPEC_2,
	OSD_CAR,
	OSD_SPEC_4,
	OSD_KEY,
	OSD_SPEC_6,
	OSD_IOINFO,
	OSD_SPEC_8,
	OSD_IO2INFO,
	OSD_SPEC_10,
	OSD_T123INFO,
	OSD_SPEC_MAX
}eOSD;

extern void DebugDemoInit(void);
extern void DebugDemoProc(void);
extern void DebugDemoIpcEventRegister(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __DEBUGDEMO_H__ */
