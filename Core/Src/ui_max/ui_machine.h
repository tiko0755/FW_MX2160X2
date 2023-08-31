/**********************************************************
filename: ui_machine.h
**********************************************************/
#ifndef _UI_MACHINE_H_
#define _UI_MACHINE_H_

#include "misc.h"
#include "uartdev.h"
#include "app_timer.h"
#include "ui.h"

#define UI_INSTANCE_POS_COUNT   (16)

/*****************************************************************************
 @ typedefs
****************************************************************************/
#pragma pack(push,4)    // push current align bytes, and then set 4 bytes align
typedef struct{
    s32 r;
    s32 y;
}uiInstancePos_t;

#pragma pack(pop)       //recover align bytes from 4 bytes

extern uiDev_T ui;
extern u16 mmPerRev_r;  // in um
extern u16 mmPerRev_y;  // in um
extern u16 spd_r;       // in mm/sec
extern u16 spd_y;       // in mm/sec
extern u32 posR[4];
extern u32 posY[4];
extern u8 uiInstanceSqu;

extern uiInstancePos_t* uiInstRunPrg[UI_INSTANCE_POS_COUNT];

// delegate for manual operate
extern u8 uiInstanceEvntMannual;

void  uiInstance_initial(
    UartDev_t* uartDev,
    appTmrDev_t* tObj
);

#endif

