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
extern u8 isClamp;

extern u16 repoCount[5];
extern u16 maxCount;


extern u8 uiInstanceSqu;

extern mPath2Dev_t mPlayer;
extern mPath2PosNode* mPath;

extern uiInstPos_t* uiInstRunPrg[UI_INSTANCE_POS_COUNT];





//s32 uiInstanceSaveConf(void){
//    u32 checkSum = 0xa5;
//    s32 i;
//    u16 addr = uiInst_ioBase + 4;
//    if(uiInst_ioWrite==NULL){
//        return -1;
//    }
//    
//    uiInst_ioWrite(addr, (const u8*)&slotDepth, 2);
//    checkSum += slotDepth;  
//    addr += 2;
//    thread_delay(5);
//    
//    uiInst_ioWrite(addr, (const u8*)&thickNess, 2);
//    checkSum += thickNess;  
//    addr += 2;
//    thread_delay(5);
//    
//    uiInst_ioWrite(addr, (const u8*)&perRev_r, 2);   
//    checkSum += perRev_r;  
//    addr += 2;
//    thread_delay(5);
//    
//    uiInst_ioWrite(addr, (const u8*)&perRev_y, 2);   
//    checkSum += perRev_y;  
//    addr += 2;
//    thread_delay(5);
//    
//    uiInst_ioWrite(addr, (const u8*)&spd_r, 4);   
//    checkSum += spd_r;  
//    addr += 4;
//    thread_delay(5);
//    
//    uiInst_ioWrite(addr, (const u8*)&spd_y, 4);   
//    checkSum += spd_y;  
//    addr += 4;
//    thread_delay(5);
//    
//    uiInst_ioWrite(addr, (const u8*)&cur_r, 2);   
//    checkSum += cur_r;  
//    addr += 2;
//    thread_delay(5);
//    
//    uiInst_ioWrite(addr, (const u8*)&cur_y, 2);   
//    checkSum += cur_y;  
//    addr += 2;
//    thread_delay(5);

//    for(i=0;i<5;i++){
//        uiInst_ioWrite(addr, (u8*)&posR[i], 4);
//        checkSum += posR[i];  
//        addr += 4;
//        thread_delay(5);
//    }
//    
//    for(i=0;i<5;i++){
//        uiInst_ioWrite(addr, (u8*)&posY[i], 4);
//        checkSum += posY[i];  
//        addr += 4;
//        thread_delay(5);
//    }
//    
//    uiInst_ioWrite(addr, (const u8*)&statusBits, 2);
//    checkSum += statusBits;  
//    addr += 2;
//    thread_delay(5);
//    
//    for(i=0;i<5;i++){
//        uiInst_ioWrite(addr, (u8*)&repoCount[i], 2);
//        checkSum += repoCount[i];
//        addr += 2;
//        thread_delay(5);
//    }
//    
//    uiInst_ioWrite(addr, (const u8*)&maxCount, 2);
//    checkSum += maxCount;  
//    addr += 2;
//    thread_delay(5);

//    uiInst_ioWrite(addr, (const u8*)&spd_r, 4);   
//    checkSum += spd_r;  
//    addr += 4;
//    thread_delay(5);
//    
//    uiInst_ioWrite(addr, (const u8*)&spd_y, 4);   
//    checkSum += spd_y;  
//    addr += 4;
//    thread_delay(5);
//    
//    uiInst_ioWrite(uiInst_ioBase, (const u8*)&checkSum, 4);  
//    return 0;
//}















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

s32 angleToMStep(u32 degree);    // angle in 1/100 degree
s32 distanceToMStep(u32 dist);  // dist in 1/100 mm
    
// move to target workplace
//s32 uiInstGoto_prc(u32 targetR, u32 targetY);    
    
    
#endif

