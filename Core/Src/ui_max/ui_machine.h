/**********************************************************
filename: ui_machine.h
**********************************************************/
#ifndef _UI_MACHINE_H_
#define _UI_MACHINE_H_

#include "misc.h"
#include "uartdev.h"
#include "app_timer.h"
#include "ui.h"
#include "motion_path_2axis.h"
#include "board.h"


#define UI_INSTANCE_POS_COUNT   (16)

/*****************************************************************************
 @ typedefs
****************************************************************************/
#pragma pack(push,4)    // push current align bytes, and then set 4 bytes align
typedef struct{
    s32 r;
    s32 y;
    u32 delay;
}uiInstPos_t;

#pragma pack(pop)       //recover align bytes from 4 bytes

extern uiDev_T ui;

extern u16 slotDepth;  // total slot depth
extern u16 thickNess;   // in 1/100 mm
extern u16 perRev_r;  // in 1/100 degree
extern u16 perRev_y;  // in 1/100 mm

extern u32 spd_r;       // in mm/sec
extern u32 spd_y;       // in mm/sec
extern u16 cur_r;
extern u16 cur_y;

extern u32 posR[5];
extern u32 posY[5];

extern u16 statusBits;
extern u8 isAuto;
extern u8 isEmerg;
extern u8 isHoming;
extern u8 isClamp;

extern volatile s16 repoCount[5];
extern u16 maxCount;

extern u8 uiInstanceSqu;
extern u8 uiAutoSqu,uiAutoSquNxt;

extern mPath2Dev_t mPlayer;
extern mPath2PosNode* mPath;

extern uiInstPos_t* uiInstRunPrg[UI_INSTANCE_POS_COUNT];

// delegate for manual operate
extern u8 uiInstanceEvntMannual;

void  uiInstance_setup(
    UartDev_t* uartDev,
    appTmrDev_t* tObj,
    u16 ioBase,
    s32 (*ioWrite)(u16 addr, const u8 *pDat, u16 nBytes),
    s32 (*ioRead)(u16 addr, u8 *pDat, u16 nBytes)
);

void uiInstance_initial();
s32 uiInstanceSaveConf(void);
s32 uiInstanceReadConf(void);
void defaultConf();
s32 angleToMStep(u32 degree);    // angle in 1/100 degree
s32 distanceToMStep(u32 dist);  // dist in 1/100 mm
    
// move to target workplace
//s32 uiInstGoto_prc(u32 targetR, u32 targetY);    
    
#endif

