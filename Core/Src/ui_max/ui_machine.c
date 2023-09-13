/**********************************************************
filename: max_tester_ui.c
**********************************************************/

/************************����ͷ�ļ�***************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "uartDev.h"
#include "ui_cb_instance.h"
#include "ui_def_instance.h"
#include "ui_machine.h"
#include "user_log.h"
#include "thread_delay.h"


#define ITEM_LINES          (9)         // the test items count
#define UI_INST_INTERVAL    (10)

#define MIN_SPEED_MM_PER_SEC    (1)
#define MAX_SPEED_MM_PER_SEC    (400)

/**********************************************************
 Private variable
**********************************************************/

uiDev_T ui;

u16 slotDepth = 60000;  // total slot depth
u16 thickNess = 1250;   // in 1/100 mm
u16 perRev_r = 7200;  // in 1/100 degree
u16 perRev_y = 2400;  // in 1/100 mm

u32 spd_r = 340; // in mm/sec
u32 spd_y = 300; // in mm/sec
u32 posR[5] = {6000, 12000, 18000, 24000, 30000};
u32 posY[5] = {61250, 61261, 61252, 61273, 61284};    // about 600mm, in 1/100 mm
//s16 slotCount[5] = {0};

u8 isAuto = 0;
u8 isEmerg = 0;
u8 isClamp = 0;

u16 repoCount_0 = 0;
s8 repoCount_1 = 0;
s8 repoCount_2 = 0;
s8 repoCount_3 = 0;
s8 repoCount_4 = 0;
u16 maxCount = 999;

u8 uiInstanceSqu = 0;

uiInstPos_t* uiInstRunPrg[UI_INSTANCE_POS_COUNT];

u8 uiInstanceEvntMannual;

/**********************************************************
 Private function
**********************************************************/
static void print_lcd(const char* FORMAT_ORG, ...);
static void maxTest_pg00_updateLine(u8 lineIndx, const char* DESC, const char* LOWER, const char* UPPER, const char* UNIT, const char* RSLT, const char* TIM, const char* BG_COLOR);
static void maxTest_pg00_updateLineX(u8 lineIndx, const char* RSLT, const char* TIM, const char* BG_COLOR);
static void machine_initPg00(void);
static void maxTest_task(void* e);

// R axis motion process
static u16 r_motion_tick;
static u16 r_motion_tim;
static u8 r_motion_squ;
static void r_motion(u16 tick);

mPath2Dev_t mPlayer;
mPath2PosNode* mPath;
static s32 uiMotPath2_goto(u8 axis, u16 spd, s32 pos);
static s32 uiMotPath_getPos(u8 axis, s32* pos);
static s32 uiMotPath_stop(u8 axis);

static s32 uiInstanceReadConf(void);
static void defaultConf();

static uiInstPos_t positions[UI_INSTANCE_POS_COUNT] = {
    {0,0,0}, // zero
    {0,0,0}, // pos1 reset
    {0,0,0},  // pos1 act
    {0,0,0},  // pos2 reset
    {0,0,0},  // pos2 act
    {0,0,0},  // pos3 reset
    {0,0,0},  // pos3 act
    {0,0,0},  // pos4 reset
    {0,0,0},  // pos4 act
    {0,0,0},  // pos reset
    {0,0,0},
    {0,0,0},
    {0,0,0}, 
    {0,0,0}, 
    {0,0,0}, 
    {0,0,0}, 
};

static appTmrDev_t* maxTest_tmr;
static u16 maxTest_tick = 0;

static u16 uiInst_ioBase;
static s32 (*uiInst_ioWrite)(u16 addr, const u8 *pDat, u16 nBytes);
static s32 (*uiInst_ioRead)(u16 addr, u8 *pDat, u16 nBytes);


/**********************************************************
 Public function
**********************************************************/
void  uiInstance_setup(
    UartDev_t* uartDev,
    appTmrDev_t* tObj,
    u16 ioBase,
    s32 (*ioWrite)(u16 addr, const u8 *pDat, u16 nBytes),
    s32 (*ioRead)(u16 addr, u8 *pDat, u16 nBytes)
){    
    uiSetup(&ui, uartDev, print_lcd);
    maxTest_tmr = tObj;

    uiInst_ioBase = ioBase;
    uiInst_ioWrite = ioWrite;
    uiInst_ioRead = ioRead;
    
    if(mPath_setup(&mPlayer, uiMotPath2_goto, uiMotPath_getPos, uiMotPath_stop) <0){
        log("<%s err@mPath_setup >", __func__);
    }
    
    // new MAIN page, and place components
    ui.NewPage(&ui.rsrc, PG_MAIN);   // 
    if(ui.PlaceComponent(&ui.rsrc, PG_MAIN, MAIN_THICKNESS, 8) == NULL){  log("<%s !!MAIN_THICKNESS>");      }
    if(ui.PlaceComponent(&ui.rsrc, PG_MAIN, MAIN_R_REV, 8) == NULL){  log("<%s !!MAIN_R_REV>");      }
    if(ui.PlaceComponent(&ui.rsrc, PG_MAIN, MAIN_Y_REV, 8) == NULL){  log("<%s !!MAIN_Y_REV>");      }
    
    if(ui.PlaceComponent(&ui.rsrc, PG_MAIN, MAIN_PS0, 8) == NULL){  log("<%s !!MAIN_PS0>");      }
    if(ui.PlaceComponent(&ui.rsrc, PG_MAIN, MAIN_PS0_RST, 0) == NULL){  log("<%s !!MAIN_PS0_RST>");      }
    if(ui.PlaceComponent(&ui.rsrc, PG_MAIN, MAIN_PS0_ACT, 8) == NULL){  log("<%s !!MAIN_PS0_ACT>");      }
    
    if(ui.PlaceComponent(&ui.rsrc, PG_MAIN, MAIN_PS1, 8) == NULL){  log("<%s !!MAIN_PS1>");      }
    if(ui.PlaceComponent(&ui.rsrc, PG_MAIN, MAIN_PS1_ACT, 8) == NULL){  log("<%s !!MAIN_PS1_ACT>");      }
    
    if(ui.PlaceComponent(&ui.rsrc, PG_MAIN, MAIN_PS2, 8) == NULL){  log("<%s !!MAIN_PS2>");      }
    if(ui.PlaceComponent(&ui.rsrc, PG_MAIN, MAIN_PS2_ACT, 8) == NULL){  log("<%s !!MAIN_PS2_ACT>");      }
    
    if(ui.PlaceComponent(&ui.rsrc, PG_MAIN, MAIN_PS3, 8) == NULL){  log("<%s !!MAIN_PS3>");      }
    if(ui.PlaceComponent(&ui.rsrc, PG_MAIN, MAIN_PS3_ACT, 8) == NULL){  log("<%s !!MAIN_PS3_ACT>");      }
    
    if(ui.PlaceComponent(&ui.rsrc, PG_MAIN, MAIN_PS4, 8) == NULL){  log("<%s !!MAIN_PS4>");      }
    if(ui.PlaceComponent(&ui.rsrc, PG_MAIN, MAIN_PS4_ACT, 8) == NULL){  log("<%s !!MAIN_PS4_ACT>");      }
    
    if(ui.PlaceComponent(&ui.rsrc, PG_MAIN, MAIN_HOMING, 0) == NULL){  log("<%s !!MAIN_HOMING>");      }
    if(ui.PlaceComponent(&ui.rsrc, PG_MAIN, MAIN_AUTO, 0) == NULL){  log("<%s !!MAIN_AUTO>");      }
    if(ui.PlaceComponent(&ui.rsrc, PG_MAIN, MAIN_EMERGENCY, 4) == NULL){  log("<%s !!MAIN_EMERGENCY>");      }
    if(ui.PlaceComponent(&ui.rsrc, PG_MAIN, MAIN_CLAMP, 4) == NULL){  log("<%s !!MAIN_CLAMP>");      }
    if(ui.PlaceComponent(&ui.rsrc, PG_MAIN, MAIN_PG_SWITCH, 0) == NULL){  log("<%s !!MAIN_PG_SWITCH>");      }

    if(ui.PlaceComponent(&ui.rsrc, PG_MAIN, MAIN_REPO0, 0) == NULL){  log("<%s !!MAIN_REPO0>");      }
    if(ui.PlaceComponent(&ui.rsrc, PG_MAIN, MAIN_REPO1, 0) == NULL){  log("<%s !!MAIN_REPO1>");      }
    if(ui.PlaceComponent(&ui.rsrc, PG_MAIN, MAIN_REPO2, 0) == NULL){  log("<%s !!MAIN_REPO2>");      }
    if(ui.PlaceComponent(&ui.rsrc, PG_MAIN, MAIN_REPO3, 0) == NULL){  log("<%s !!MAIN_REPO3>");      }
    if(ui.PlaceComponent(&ui.rsrc, PG_MAIN, MAIN_REPO4, 0) == NULL){  log("<%s !!MAIN_REPO4>");      }

    uiInstanceSqu = 0;  // initial
}

static void uiRestartMPath(s32 sta, void* e){
    mPlayer.start(&mPlayer.rsrc, mPath, uiRestartMPath);
}

void  uiInstance_initial(){
    maxTest_tmr->start(&maxTest_tmr->rsrc, UI_INST_INTERVAL, POLLING_REPEAT, maxTest_task, NULL);
    if(uiInstanceReadConf() < 0){
         uiInstanceSaveConf();
    }
    if(thickNess == 0){
        maxCount = 999;
        log("<%s fetal_error >", __func__);
    }
    else{   maxCount = slotDepth/thickNess;   }
    
    machine_initPg00();
    
    mPlayer.addTo(&mPlayer.rsrc, &mPath, 10000, 11000, 63000,63000, 5000);
    mPlayer.addTo(&mPlayer.rsrc, &mPath, 20000, 21000, 63000,63000, 5000);
    mPlayer.addTo(&mPlayer.rsrc, &mPath, 30000, 31000, 63000,63000, 5000);
    mPlayer.addTo(&mPlayer.rsrc, &mPath, 40000, 41000, 63000,63000, 5000);
    mPlayer.addTo(&mPlayer.rsrc, &mPath, 50000, 51000, 63000,63000, 5000);
    mPlayer.addTo(&mPlayer.rsrc, &mPath, 40000, 41000, 63000,63000, 5000);
    mPlayer.addTo(&mPlayer.rsrc, &mPath, 30000, 31000, 63000,63000, 5000);
    mPlayer.addTo(&mPlayer.rsrc, &mPath, 20000, 21000, 63000,63000, 5000);
    thread_delay(100);
    
//    mPlayer.start(&mPlayer.rsrc, mPath, uiRestartMPath);
}



static void maxTest_task(void* e){
    ui.Polling(&ui.rsrc, UI_INST_INTERVAL);
    
    mPlayer.polling(&mPlayer.rsrc, UI_INST_INTERVAL);
    
    r_motion(UI_INST_INTERVAL);
}

static void machine_initPg00(void){
    // inital pg10
    ui.Set(&ui.rsrc, PG_MAIN, MAIN_PS0, UI_ATTR_TXT, "%d.%d", posR[0]/100, posR[0]%100);
    ui.Set(&ui.rsrc, PG_MAIN, MAIN_PS0_ACT, UI_ATTR_TXT, "%d.%d", posY[0]/100, posY[0]%100);
    
    ui.Set(&ui.rsrc, PG_MAIN, MAIN_PS1, UI_ATTR_TXT, "%d.%d", posR[1]/100, posR[1]%100);
    ui.Set(&ui.rsrc, PG_MAIN, MAIN_PS1_ACT, UI_ATTR_TXT, "%d.%d", posY[1]/100, posY[1]%100);
    thread_delay(50);
    
    ui.Set(&ui.rsrc, PG_MAIN, MAIN_PS2, UI_ATTR_TXT, "%d.%d", posR[2]/100, posR[2]%100);
    ui.Set(&ui.rsrc, PG_MAIN, MAIN_PS2_ACT, UI_ATTR_TXT, "%d.%d", posY[2]/100, posY[2]%100);
    
    ui.Set(&ui.rsrc, PG_MAIN, MAIN_PS3, UI_ATTR_TXT, "%d.%d", posR[3]/100, posR[3]%100);
    ui.Set(&ui.rsrc, PG_MAIN, MAIN_PS3_ACT, UI_ATTR_TXT, "%d.%d", posY[3]/100, posY[3]%100);
    
    ui.Set(&ui.rsrc, PG_MAIN, MAIN_PS4, UI_ATTR_TXT, "%d.%d", posR[4]/100, posR[4]%100);
    ui.Set(&ui.rsrc, PG_MAIN, MAIN_PS4_ACT, UI_ATTR_TXT, "%d.%d", posY[4]/100, posY[4]%100);

    thread_delay(50);
    ui.Set(&ui.rsrc, PG_MAIN, MAIN_THICKNESS, UI_ATTR_TXT, "%d.%d", thickNess/100, thickNess%100);
    ui.Set(&ui.rsrc, PG_MAIN, MAIN_R_REV, UI_ATTR_TXT, "%d.%d", perRev_r/100, perRev_r%100);
    ui.Set(&ui.rsrc, PG_MAIN, MAIN_Y_REV, UI_ATTR_TXT, "%d.%d", perRev_y/100, perRev_y%100);
    
    ui.Set(&ui.rsrc, PG_MAIN, MAIN_AUTO, UI_ATTR_VAL, "1");
    ui.Set(&ui.rsrc, PG_MAIN, MAIN_EMERGENCY, UI_ATTR_VAL, "0");
    ui.Set(&ui.rsrc, PG_MAIN, MAIN_CLAMP, UI_ATTR_VAL, "0");
    
    ui.Set(&ui.rsrc, PG_MAIN, MAIN_AUTO, UI_ATTR_VAL, "%d", isAuto);
    ui.Set(&ui.rsrc, PG_MAIN, MAIN_EMERGENCY, UI_ATTR_VAL, "%d", isEmerg);
    ui.Set(&ui.rsrc, PG_MAIN, MAIN_CLAMP, UI_ATTR_VAL, "%d", isClamp);

//u16 repoCount_0 = 0;
//s8 repoCount_1 = 0;
//s8 repoCount_2 = 0;
//s8 repoCount_3 = 0;
//s8 repoCount_4 = 0;
//u16 maxCount = 999;
    ui.Set(&ui.rsrc, PG_MAIN, MAIN_REPO0, UI_ATTR_TXT, "%d", repoCount_0);
    ui.Set(&ui.rsrc, PG_MAIN, MAIN_REPO0, UI_ATTR_TXT, "%d", abs(repoCount_1));
    ui.Set(&ui.rsrc, PG_MAIN, MAIN_REPO0, UI_ATTR_TXT, "%d", abs(repoCount_2));
    ui.Set(&ui.rsrc, PG_MAIN, MAIN_REPO0, UI_ATTR_TXT, "%d", abs(repoCount_3));
    ui.Set(&ui.rsrc, PG_MAIN, MAIN_REPO0, UI_ATTR_TXT, "%d", abs(repoCount_4));

    
    
    
//    // bind on event
    ui.Bind(&ui.rsrc, PG_MAIN, MAIN_THICKNESS, UI_EVNT_EDIT_STR, ui_onEdit_thickness);
    ui.Bind(&ui.rsrc, PG_MAIN, MAIN_R_REV, UI_EVNT_EDIT_STR, ui_onEdit_r_rev);
    ui.Bind(&ui.rsrc, PG_MAIN, MAIN_Y_REV, UI_EVNT_EDIT_STR, ui_onEdit_y_rev);
    
    ui.Bind(&ui.rsrc, PG_MAIN, MAIN_PS0, UI_EVNT_EDIT_STR, ui_onEdit_ps0);
    ui.Bind(&ui.rsrc, PG_MAIN, MAIN_PS1, UI_EVNT_EDIT_STR, ui_onEdit_ps1);    
    ui.Bind(&ui.rsrc, PG_MAIN, MAIN_PS2, UI_EVNT_EDIT_STR, ui_onEdit_ps2);    
    ui.Bind(&ui.rsrc, PG_MAIN, MAIN_PS3, UI_EVNT_EDIT_STR, ui_onEdit_ps3);    
    ui.Bind(&ui.rsrc, PG_MAIN, MAIN_PS4, UI_EVNT_EDIT_STR, ui_onEdit_ps4);   
    
    ui.Bind(&ui.rsrc, PG_MAIN, MAIN_PS0_ACT, UI_EVNT_EDIT_STR, ui_onEdit_ps0_act);
    ui.Bind(&ui.rsrc, PG_MAIN, MAIN_PS1_ACT, UI_EVNT_EDIT_STR, ui_onEdit_ps1_act);
    ui.Bind(&ui.rsrc, PG_MAIN, MAIN_PS2_ACT, UI_EVNT_EDIT_STR, ui_onEdit_ps2_act);
    ui.Bind(&ui.rsrc, PG_MAIN, MAIN_PS3_ACT, UI_EVNT_EDIT_STR, ui_onEdit_ps3_act);
    ui.Bind(&ui.rsrc, PG_MAIN, MAIN_PS4_ACT, UI_EVNT_EDIT_STR, ui_onEdit_ps4_act);
    
    ui.Bind(&ui.rsrc, PG_MAIN, MAIN_PS0, UI_EVNT_CLICK, ui_onClick_ps0);
    ui.Bind(&ui.rsrc, PG_MAIN, MAIN_PS1, UI_EVNT_CLICK, ui_onClick_ps1);    
    ui.Bind(&ui.rsrc, PG_MAIN, MAIN_PS2, UI_EVNT_CLICK, ui_onClick_ps2);    
    ui.Bind(&ui.rsrc, PG_MAIN, MAIN_PS3, UI_EVNT_CLICK, ui_onClick_ps3);    
    ui.Bind(&ui.rsrc, PG_MAIN, MAIN_PS4, UI_EVNT_CLICK, ui_onClick_ps4);   
    
    ui.Bind(&ui.rsrc, PG_MAIN, MAIN_PS0_ACT, UI_EVNT_CLICK, ui_onClick_ps0_act);
    ui.Bind(&ui.rsrc, PG_MAIN, MAIN_PS1_ACT, UI_EVNT_CLICK, ui_onClick_ps1_act);
    ui.Bind(&ui.rsrc, PG_MAIN, MAIN_PS2_ACT, UI_EVNT_CLICK, ui_onClick_ps2_act);
    ui.Bind(&ui.rsrc, PG_MAIN, MAIN_PS3_ACT, UI_EVNT_CLICK, ui_onClick_ps3_act);
    ui.Bind(&ui.rsrc, PG_MAIN, MAIN_PS4_ACT, UI_EVNT_CLICK, ui_onClick_ps4_act);
    
    ui.Bind(&ui.rsrc, PG_MAIN, MAIN_PS0_RST, UI_EVNT_CLICK, ui_onClick_psX_rst);

//    ui.Bind(&ui.rsrc, PG_MAIN, MAIN_tbUnitY, UI_EVNT_CHANGE, ui_onChanged_main_tbUnitY);
//    ui.Bind(&ui.rsrc, PG_MAIN, MAIN_tbSpdR, UI_EVNT_CHANGE, ui_onChanged_main_tbSpdR);
//    ui.Bind(&ui.rsrc, PG_MAIN, MAIN_tbSpdY, UI_EVNT_CHANGE, ui_onChanged_main_tbSpdY);

}

//static u16 r_motion_tick;
//static u16 r_motion_tim;
//static u8 r_motion_squ;
static u16 r_motion_target;
static void r_motion(u16 tick){
    r_motion_tick += tick;
    
    switch(r_motion_squ){
        case 0:{    break;  }
        // move to target posion
        case 1:{
            
            break;
        }
    }
}


static void print_lcd(const char* FORMAT_ORG, ...){
    va_list ap;
    char buf[64] = {0};
    s16 bytes;
    //take string
    va_start(ap, FORMAT_ORG);
    bytes = vsnprintf(buf, 64, FORMAT_ORG, ap);
    va_end(ap);
    //send out
    if(bytes>0){
        buf[bytes++] = 0xff; 
        buf[bytes++] = 0xff; 
        buf[bytes++] = 0xff; 
        ui.rsrc.pUartDev->Send(&ui.rsrc.pUartDev->rsrc, (u8*)buf, bytes);
    }
}

s32 uiInstanceSaveConf(void){
    u32 checkSum = 0xa5;
    s32 i;
    u16 addr = uiInst_ioBase + 4;
    if(uiInst_ioWrite==NULL){
        return -1;
    }
    
    uiInst_ioWrite(addr, (const u8*)&slotDepth, sizeof(slotDepth));
    checkSum += slotDepth;  
    addr += sizeof(slotDepth);
    thread_delay(5);
    
    uiInst_ioWrite(addr, (const u8*)&thickNess, sizeof(thickNess));
    checkSum += thickNess;  
    addr += sizeof(thickNess);
    thread_delay(5);
    
    uiInst_ioWrite(addr, (const u8*)&perRev_r, sizeof(perRev_r));   
    checkSum += perRev_r;  
    addr += sizeof(perRev_r);
    thread_delay(5);
    
    uiInst_ioWrite(addr, (const u8*)&perRev_y, sizeof(perRev_y));   
    checkSum += perRev_y;  
    addr += sizeof(perRev_y);
    thread_delay(5);
    
    uiInst_ioWrite(addr, (const u8*)&spd_r, sizeof(spd_r));   
    checkSum += spd_r;  
    addr += sizeof(spd_r);
    thread_delay(5);
    
    uiInst_ioWrite(addr, (const u8*)&spd_y, sizeof(spd_y));   
    checkSum += spd_y;  
    addr += sizeof(spd_y);
    thread_delay(5);
    
    uiInst_ioWrite(addr, (const u8*)&cur_r, sizeof(cur_r));   
    checkSum += cur_r;  
    addr += sizeof(cur_r);
    thread_delay(5);
    
    uiInst_ioWrite(addr, (const u8*)&cur_y, sizeof(cur_y));   
    checkSum += cur_y;  
    addr += sizeof(cur_y);
    thread_delay(5);

    for(i=0;i<5;i++){
        uiInst_ioWrite(addr, (u8*)&posR[i], sizeof(posR));
        checkSum += posR[i];  
        addr += sizeof(posR);
        thread_delay(5);
    }
    
    for(i=0;i<5;i++){
        uiInst_ioWrite(addr, (u8*)&posY[i], sizeof(posY));
        checkSum += posY[i];  
        addr += sizeof(posY);
        thread_delay(5);
    }
    
    uiInst_ioWrite(addr, (const u8*)&statusBits, sizeof(statusBits));
    checkSum += statusBits;  
    addr += sizeof(statusBits);
    thread_delay(5);
    
    for(i=0;i<5;i++){
        uiInst_ioWrite(addr, (u8*)&repoCount[i], sizeof(repoCount));
        checkSum += repoCount[i];
        addr += sizeof(repoCount);
        thread_delay(5);
    }
    
    uiInst_ioWrite(addr, (const u8*)&maxCount, sizeof(maxCount));
    checkSum += maxCount;  
    addr += sizeof(maxCount);
    thread_delay(5);
    
    uiInst_ioWrite(uiInst_ioBase, (const u8*)&checkSum, 4);  
    return 0;
}

static s32 uiInstanceReadConf(void){
    u32 checkSum = 0xa5, checkCode;
    s32 i;
    u16 addr = uiInst_ioBase+4;
    if(uiInst_ioRead==NULL){
        return -1;
    }
    
    uiInst_ioRead(uiInst_ioBase, (u8*)&checkCode, sizeof checkCode);
    addr = uiInst_ioBase + sizeof(checkCode);
//    log("<%s slotDepth:%08x >", __func__, slotDepth);
    
    uiInst_ioRead(addr, (u8*)&slotDepth, sizeof(slotDepth));
    addr += sizeof(slotDepth);  
    checkSum += slotDepth;
//    log("<%s slotDepth:%08x >", __func__, slotDepth);
    
    uiInst_ioRead(addr, (u8*)&thickNess, sizeof(thickNess));
    addr += sizeof(thickNess);  
    checkSum += thickNess;
//    log("<%s thickNess:%08x >", __func__, thickNess);
    
    uiInst_ioRead(addr, (u8*)&perRev_r, sizeof(perRev_r));
    addr += sizeof(perRev_r);  
    checkSum += perRev_r;
//    log("<%s perRev_r:%08x >", __func__, perRev_r);
    
    uiInst_ioRead(addr, (u8*)&perRev_y, sizeof(perRev_y));
    addr += sizeof(perRev_y);  
    checkSum += perRev_r;
//    log("<%s perRev_y:%08x >", __func__, perRev_y);

    uiInst_ioRead(addr, (u8*)&spd_r, sizeof(spd_r));
    addr += sizeof(spd_r);  
    checkSum += spd_r;
//    log("<%s spd_r:%08x >", __func__, spd_r);

    uiInst_ioRead(addr, (u8*)&spd_y, sizeof(spd_y));
    addr += sizeof(spd_y);  
    checkSum += spd_y;
//    log("<%s spd_y:%08x >", __func__, spd_y);
    
    uiInst_ioRead(addr, (u8*)&cur_r, sizeof(cur_r));
    addr += sizeof(cur_r);  
    checkSum += cur_r;
//    log("<%s cur_r:%08x >", __func__, cur_r);
    
    uiInst_ioRead(addr, (u8*)&cur_y, sizeof(cur_y));
    addr += sizeof(cur_y);  
    checkSum += cur_y;
//    log("<%s cur_y:%08x >", __func__, cur_y);

    for(i=0;i<5;i++){
        uiInst_ioRead(addr, (u8*)&posR[i], sizeof(posR[i]));
        addr += sizeof(posR[i]);  
        checkSum += posR[i];
//        log("<%s posR[%d]:%08x >", __func__, i, posR[i]);
    }
    
    for(i=0;i<5;i++){
        uiInst_ioRead(addr, (u8*)&posY[i], sizeof(posY[i]));
        addr += sizeof(posY[i]);  
        checkSum += posY[i];
//        log("<%s posY[%d]:%08x >", __func__, i, posY[i]);
    }
    
    uiInst_ioRead(addr, (u8*)&statusBits, sizeof(statusBits));
    addr += sizeof(statusBits);
    checkSum += statusBits;
//    log("<%s statusBits:%08x >", __func__, statusBits);
    
    for(i=0;i<5;i++){
        uiInst_ioRead(addr, (u8*)&repoCount[i], sizeof(repoCount[i]));
        addr += sizeof(repoCount[i]);  
        checkSum += repoCount[i];
//        log("<%s repoCount[%d]:%08x >", __func__, i, repoCount[i]);
    }
    
    uiInst_ioRead(addr, (u8*)&maxCount, sizeof(maxCount));
    addr += sizeof(maxCount);
    checkSum += maxCount;
//    log("<%s maxCount:%08x >", __func__, maxCount);
    
    if(checkCode != checkSum){
//        log("<%s checkCode:0x%08x checkSum:0x%08x >", __func__,checkCode,checkSum);
        defaultConf();
        return -2;
    }
    return 0;
}

static void defaultConf(){
    thickNess = 1250;   // in 1/100 mm
    perRev_r = 7200;  // in 1/100 degree
    perRev_y = 2400;  // in 1/100 mm

    spd_r = 340; // in mm/sec
    spd_y = 300; // in mm/sec
    
    posR[0] = 1400;
    posR[1] = 2800;
    posR[2] = 4200;
    posR[3] = 5600;
    posR[4] = 7000;
    
    posY[0] = 61250;
    posY[1] = 61261;
    posY[2] = 61252;
    posY[3] = 61273;
    posY[4] = 61284;
    
}

// angle in 1/100 degree
s32 angleToMStep(u32 degree){
    log("<%s degree:%d mStep:%d >", __func__, degree, stprRamp[1].rsrc.mStep);
    if(perRev_r == 0){
        return -1;
    }
    s32 mstep = degree*200*stprRamp[1].rsrc.mStep/perRev_r;
    log("</%s mstep:%d >", __func__, mstep);
    return (mstep);

}

// dist in 1/100 mm
s32 distanceToMStep(u32 dist){
    if(perRev_y == 0){
        return -1;
    }
    s32 mstep = dist*200*stprRamp[1].rsrc.mStep/perRev_y;
    log("<%s mstep:%d >", __func__, mstep);
    return mstep;
}
    


//static u32 wrkPlc_targetR;
//static u32 wrkPlc_targetY;
//static u8 wrkPlc_squ = 0;
//static u16 wrkPlc_tick;
//static u16 wrkPlc_tim;
//static void wrkPlc_task(u16 tick);
#define WP_SQU_Y

//s32 uiInstGoto_prc(u32 targetR, u32 targetY){
//    // test if busy
//    if(wrkPlc_squ){
//        return -1;
//    }
//    switch(wrkPlc_squ){
//        case 1:{
//            break;
//        }
//
//    
//    }
//}

static s32 uiMotPath2_goto(u8 axis, u16 spd, s32 pos){
    if(axis>=2){
        return -1;
    }
    stprRamp[axis].rsrc.spdMax = spd;
    stprRamp[axis].moveTo(&stprRamp[axis].rsrc, pos);
    return 0;
}

static s32 uiMotPath_getPos(u8 axis, s32* pos){
    if((axis>=2) || (pos==NULL)){
        return -1;
    }
    *pos = stprRamp[axis].rsrc.posCur;
    return 0;
}

static s32 uiMotPath_stop(u8 axis){
    if(axis>=2){
        return -1;
    }
    stprRamp[axis].stop(&stprRamp[axis].rsrc);
    return 0;
}


/**********************************************************
 == THE END ==
**********************************************************/

