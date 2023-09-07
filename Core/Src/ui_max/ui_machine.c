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
#define MAX_TEST_POLLING_TIM    (10)

#define MIN_SPEED_MM_PER_SEC    (1)
#define MAX_SPEED_MM_PER_SEC    (400)

/**********************************************************
 Private variable
**********************************************************/

uiDev_T ui;

u32 thickNess = 1250;   // in 1/100 mm
u32 perRev_r = 7200;  // in 1/100 degree
u32 perRev_y = 2400;  // in 1/100 mm

u32 spd_r = 340; // in mm/sec
u32 spd_y = 300; // in mm/sec
u32 posR[5] = {1400, 2800, 4200, 5600, 7000};
u32 posY[5] = {61250, 61261, 61252, 61273, 61284};    // about 600mm, in 1/100 mm
u16 slotCount[5] = {0};
u8 uiInstanceSqu = 0;

uiInstancePos_t* uiInstRunPrg[UI_INSTANCE_POS_COUNT];

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

static void uiInstanceReadConf(void);
static void defaultConf();


static uiInstancePos_t positions[UI_INSTANCE_POS_COUNT] = {
    {0,     0}, // zero
    {0,0}, // pos1 reset
    {0,0},  // pos1 act
    {0,0},  // pos2 reset
    {0,0},  // pos2 act
    {0,0},  // pos3 reset
    {0,0},  // pos3 act
    {0,0},  // pos4 reset
    {0,0},  // pos4 act
    {0,0},  // pos reset
    {0,0},
    {0,0},
    {0,0}, 
    {0,0}, 
    {0,0}, 
    {0,0}, 
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
    // new MAIN page, and place components
    ui.NewPage(&ui.rsrc, PG_MAIN);   // 
    ui.PlaceComponent(&ui.rsrc, PG_MAIN, MAIN_THICKNESS, 8);
    ui.PlaceComponent(&ui.rsrc, PG_MAIN, MAIN_R_REV, 8);
    ui.PlaceComponent(&ui.rsrc, PG_MAIN, MAIN_Y_REV, 8);
    ui.PlaceComponent(&ui.rsrc, PG_MAIN, MAIN_PS0, 8);          // position0, R axis distance
    ui.PlaceComponent(&ui.rsrc, PG_MAIN, MAIN_PS0_RST, 0);      // position0, Y axis home
    ui.PlaceComponent(&ui.rsrc, PG_MAIN, MAIN_PS0_ACT, 8);      // position0, Y axis action position
    ui.PlaceComponent(&ui.rsrc, PG_MAIN, MAIN_PS1, 8);
    ui.PlaceComponent(&ui.rsrc, PG_MAIN, MAIN_PS1_RST, 0);
    ui.PlaceComponent(&ui.rsrc, PG_MAIN, MAIN_PS1_ACT, 8);
    ui.PlaceComponent(&ui.rsrc, PG_MAIN, MAIN_PS2, 8);
    ui.PlaceComponent(&ui.rsrc, PG_MAIN, MAIN_PS2_RST, 0);
    ui.PlaceComponent(&ui.rsrc, PG_MAIN, MAIN_PS2_ACT, 8);
    ui.PlaceComponent(&ui.rsrc, PG_MAIN, MAIN_PS3, 8);
    ui.PlaceComponent(&ui.rsrc, PG_MAIN, MAIN_PS3_RST, 0);
    ui.PlaceComponent(&ui.rsrc, PG_MAIN, MAIN_PS3_ACT, 8);
    ui.PlaceComponent(&ui.rsrc, PG_MAIN, MAIN_PS4, 8);
    ui.PlaceComponent(&ui.rsrc, PG_MAIN, MAIN_PS4_RST, 0);
    ui.PlaceComponent(&ui.rsrc, PG_MAIN, MAIN_PS4_ACT, 8);
    
    ui.PlaceComponent(&ui.rsrc, PG_MAIN, MAIN_HOMING, 0);
    ui.PlaceComponent(&ui.rsrc, PG_MAIN, MAIN_EMERGENCY, 4);
    ui.PlaceComponent(&ui.rsrc, PG_MAIN, MAIN_CLAMP, 4);
    
    uiInstanceSqu = 0;  // initial
}

void  uiInstance_initial(){
    maxTest_tmr->start(&maxTest_tmr->rsrc, MAX_TEST_POLLING_TIM, POLLING_REPEAT, maxTest_task, NULL);
    uiInstanceReadConf();
    machine_initPg00();
}

static void maxTest_task(void* e){
    ui.Polling(&ui.rsrc, MAX_TEST_POLLING_TIM);

    r_motion(MAX_TEST_POLLING_TIM);

}

static void machine_initPg00(void){
    log("<%s >", __func__);
    // inital pg10
    ui.Set(&ui.rsrc, PG_MAIN, MAIN_PS0, UI_ATTR_TXT, "%d.%d", posR[0]/100, posR[0]%100);
    ui.Set(&ui.rsrc, PG_MAIN, MAIN_PS0_ACT, UI_ATTR_TXT, "%d.%d", posY[0]/100, posY[0]%100);
    
    ui.Set(&ui.rsrc, PG_MAIN, MAIN_PS1, UI_ATTR_TXT, "%d.%d", posR[1]/100, posY[1]%100);
    ui.Set(&ui.rsrc, PG_MAIN, MAIN_PS1_ACT, UI_ATTR_TXT, "%d.%d", posY[1]/100, posY[1]%100);
    thread_delay(50);
    
    ui.Set(&ui.rsrc, PG_MAIN, MAIN_PS2, UI_ATTR_TXT, "%d.%d", posR[2]/100, posY[2]%100);
    ui.Set(&ui.rsrc, PG_MAIN, MAIN_PS2_ACT, UI_ATTR_TXT, "%d.%d", posY[2]/100, posY[2]%100);
    
    ui.Set(&ui.rsrc, PG_MAIN, MAIN_PS3, UI_ATTR_TXT, "%d.%d", posR[3]/100, posY[3]%100);
    ui.Set(&ui.rsrc, PG_MAIN, MAIN_PS3_ACT, UI_ATTR_TXT, "%d.%d", posY[3]/100, posY[3]%100);
    
    ui.Set(&ui.rsrc, PG_MAIN, MAIN_PS4, UI_ATTR_TXT, "%d.%d", posR[4]/100, posY[4]%100);
    ui.Set(&ui.rsrc, PG_MAIN, MAIN_PS4_ACT, UI_ATTR_TXT, "%d.%d", posY[4]/100, posY[4]%100);

    thread_delay(50);
    ui.Set(&ui.rsrc, PG_MAIN, MAIN_THICKNESS, UI_ATTR_TXT, "%d.%d", thickNess/100, thickNess%100);
    ui.Set(&ui.rsrc, PG_MAIN, MAIN_R_REV, UI_ATTR_TXT, "%d.%d", perRev_r/100, perRev_r%100);
    ui.Set(&ui.rsrc, PG_MAIN, MAIN_Y_REV, UI_ATTR_TXT, "%d.%d", perRev_y/100, perRev_y%100);
    
//    // bind on event
    ui.Bind(&ui.rsrc, PG_MAIN, MAIN_THICKNESS, UI_EVNT_EDIT_STR, ui_onEdit_thickness);
    ui.Bind(&ui.rsrc, PG_MAIN, MAIN_R_REV, UI_EVNT_EDIT_STR, ui_onEdit_r_rev);
    ui.Bind(&ui.rsrc, PG_MAIN, MAIN_Y_REV, UI_EVNT_EDIT_STR, ui_onEdit_y_rev);
    
    ui.Bind(&ui.rsrc, PG_MAIN, MAIN_PS0, UI_EVNT_EDIT_STR, ui_onEdit_ps0);
    ui.Bind(&ui.rsrc, PG_MAIN, MAIN_PS0_ACT, UI_EVNT_EDIT_STR, ui_onEdit_ps0_act);
    
    ui.Bind(&ui.rsrc, PG_MAIN, MAIN_PS1, UI_EVNT_EDIT_STR, ui_onEdit_ps1);
    ui.Bind(&ui.rsrc, PG_MAIN, MAIN_PS1_ACT, UI_EVNT_EDIT_STR, ui_onEdit_ps1_act);
    
    ui.Bind(&ui.rsrc, PG_MAIN, MAIN_PS2, UI_EVNT_EDIT_STR, ui_onEdit_ps2);
    ui.Bind(&ui.rsrc, PG_MAIN, MAIN_PS2_ACT, UI_EVNT_EDIT_STR, ui_onEdit_ps2_act);
    
    ui.Bind(&ui.rsrc, PG_MAIN, MAIN_PS3, UI_EVNT_EDIT_STR, ui_onEdit_ps3);
    ui.Bind(&ui.rsrc, PG_MAIN, MAIN_PS3_ACT, UI_EVNT_EDIT_STR, ui_onEdit_ps3_act);
    
    ui.Bind(&ui.rsrc, PG_MAIN, MAIN_PS4, UI_EVNT_EDIT_STR, ui_onEdit_ps4);
    ui.Bind(&ui.rsrc, PG_MAIN, MAIN_PS4_ACT, UI_EVNT_EDIT_STR, ui_onEdit_ps4_act);
    
    
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


void uiInstanceSaveConf(void){
    u32 checkSum = 0xa5;
    s32 i;
    u16 addr = uiInst_ioBase + 4;
    if(uiInst_ioWrite==NULL){
        return;
    }
    uiInst_ioWrite(addr, (const u8*)&thickNess, 4);
    checkSum += thickNess;  
    addr += 4;
    thread_delay(5);    
    
    uiInst_ioWrite(addr, (const u8*)&perRev_r, 4);   
    checkSum += perRev_r;  
    addr += 4;
    thread_delay(5);
    
    uiInst_ioWrite(addr, (const u8*)&perRev_y, 4);   
    checkSum += perRev_y;  
    addr += 4;
    thread_delay(5);
    
    for(i=0;i<5;i++){
        uiInst_ioWrite(addr, (const u8*)&posR[i], 4);   
        checkSum += posR[i];  
        addr += 4;
        thread_delay(5);
    }
    
    for(i=0;i<5;i++){
        uiInst_ioWrite(addr, (const u8*)&posY[i], 4);   
        checkSum += posY[i];  
        addr += 4;
        thread_delay(5);
    }
    
    for(i=0;i<5;i++){
        uiInst_ioWrite(addr, (const u8*)&slotCount[i], 2);   
        checkSum += slotCount[i];  
        addr += 2;
        thread_delay(5);
    }
    
    uiInst_ioWrite(addr, (const u8*)&spd_r, 4);   
    checkSum += spd_r;  
    addr += 4;
    thread_delay(5);
    
    uiInst_ioWrite(addr, (const u8*)&spd_y, 4);   
    checkSum += spd_y;  
    addr += 4;
    thread_delay(5);
    
    uiInst_ioWrite(uiInst_ioBase, (const u8*)&checkSum, 4);  
}

static void uiInstanceReadConf(void){
    u32 checkSum = 0xa5, checkCode;
    s32 i;
    u16 addr = uiInst_ioBase+4;
    if(uiInst_ioRead==NULL){
        return;
    }
    
    uiInst_ioRead(uiInst_ioBase, (u8*)&checkCode, 4);  
    
    uiInst_ioRead(addr, (u8*)&thickNess, 4);
    checkSum += thickNess;  
    addr += 4;  
    
    uiInst_ioRead(addr, (u8*)&perRev_r, 4);   
    checkSum += perRev_r;  
    addr += 4;
    
    uiInst_ioRead(addr, (u8*)&perRev_y, 4);   
    checkSum += perRev_y;  
    addr += 4;
    
    for(i=0;i<5;i++){
        uiInst_ioRead(addr, (u8*)&posR[i], 4);   
        checkSum += posR[i];  
        addr += 4;
    }
    
    for(i=0;i<5;i++){
        uiInst_ioRead(addr, (u8*)&posY[i], 4);   
        checkSum += posY[i];  
        addr += 4;
    }
    
    for(i=0;i<5;i++){
        uiInst_ioRead(addr, (u8*)&slotCount[i], 2);   
        checkSum += slotCount[i];  
        addr += 2;
    }
    
    uiInst_ioRead(addr, (u8*)&spd_r, 4);   
    checkSum += spd_r;  
    addr += 4;
    
    uiInst_ioRead(addr, (u8*)&spd_y, 4);   
    checkSum += spd_y;  
    addr += 4;
    
    if(checkCode != checkSum){
        log("<%s checksum_fail >", __func__);
        defaultConf();
        uiInstanceSaveConf();
    }

}

static void defaultConf(){
    thickNess = 1250;   // in 1/100 mm
    perRev_r = 7200;  // in 1/100 degree
    perRev_y = 2400;  // in 1/100 mm

    spd_r = 340; // in mm/sec
    spd_y = 300; // in mm/sec
    
    posR[0] = 1400;
    posR[0] = 2800;
    posR[0] = 4200;
    posR[0] = 5600;
    posR[0] = 7000;
    
    posY[0] = 61250;
    posY[0] = 61261;
    posY[0] = 61252;
    posY[0] = 61273;
    posY[0] = 61284;
    
    memset(slotCount,0,sizeof(u16)*5);

}

/**********************************************************
 == THE END ==
**********************************************************/

