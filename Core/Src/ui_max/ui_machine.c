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
#include "board.h"

#define ITEM_LINES          (9)         // the test items count
#define MAX_TEST_POLLING_TIM    (10)

#define MIN_SPEED_MM_PER_SEC    (1)
#define MAX_SPEED_MM_PER_SEC    (400)

/**********************************************************
 Private variable
**********************************************************/
uiDev_T ui;

u16 mmPerRev_r = 45000;  // in um
u16 mmPerRev_y = 35500;  // in um

u16 spd_r = 340; // in mm/sec
u16 spd_y = 300; // in mm/sec
u32 posR[4];
u32 posY[4];
u8 uiInstanceSqu = 0;

uiInstancePos_t* uiInstRunPrg[UI_INSTANCE_POS_COUNT];

u8 uiInstanceEvntMannual;  

/**********************************************************
 Private function
**********************************************************/
static void print_lcd(const char* FORMAT_ORG, ...);
static void maxTest_pg00_updateLine(u8 lineIndx, const char* DESC, const char* LOWER, const char* UPPER, const char* UNIT, const char* RSLT, const char* TIM, const char* BG_COLOR);
static void maxTest_pg00_updateLineX(u8 lineIndx, const char* RSLT, const char* TIM, const char* BG_COLOR);
static void machine_initPg10(void);
static void maxTest_task(void* e);

// R axis motion process
static u16 r_motion_tick;
static u16 r_motion_tim;
static u8 r_motion_squ;
static void r_motion(u16 tick);



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


/**********************************************************
 Public function
**********************************************************/
void  uiInstance_initial(
    UartDev_t* uartDev,
    appTmrDev_t* tObj
){
    s32 i;
    char buff[16];
    
    uiSetup(&ui, uartDev, print_lcd);
    maxTest_tmr = tObj;

    // new pages
    ui.NewPage(&ui.rsrc, PG_MAIN);   // 

    // setup pg00
    ui.PlaceTxtBx(&ui.rsrc, PG_MAIN, MAIN_tbUnitR, 10);    // R axis mm/rev
    ui.PlaceTxtBx(&ui.rsrc, PG_MAIN, MAIN_tbUnitY, 10);    // Y axis mm/rev
    
    ui.PlacePic(&ui.rsrc, PG_MAIN, MAIN_hSpdR);
    ui.PlacePic(&ui.rsrc, PG_MAIN, MAIN_hSpdY);
    
    ui.PlacePic(&ui.rsrc, PG_MAIN, MAIN_bPos1);
    ui.PlacePic(&ui.rsrc, PG_MAIN, MAIN_bPos2);
    ui.PlacePic(&ui.rsrc, PG_MAIN, MAIN_bPos3);
    ui.PlacePic(&ui.rsrc, PG_MAIN, MAIN_bPos4);
    
    ui.PlacePic(&ui.rsrc, PG_MAIN, MAIN_bRst1);
    ui.PlacePic(&ui.rsrc, PG_MAIN, MAIN_bRst2);
    ui.PlacePic(&ui.rsrc, PG_MAIN, MAIN_bRst3);
    ui.PlacePic(&ui.rsrc, PG_MAIN, MAIN_bRst4);
    
    ui.PlacePic(&ui.rsrc, PG_MAIN, MAIN_bAct1);
    ui.PlacePic(&ui.rsrc, PG_MAIN, MAIN_bAct2);
    ui.PlacePic(&ui.rsrc, PG_MAIN, MAIN_bAct3);
    ui.PlacePic(&ui.rsrc, PG_MAIN, MAIN_bAct4);
    
    ui.PlacePic(&ui.rsrc, PG_MAIN, MAIN_bAuto);
    ui.PlaceTxtBx(&ui.rsrc, PG_MAIN, MAIN_bStat, 32);
    ui.PlaceTxtBx(&ui.rsrc, PG_MAIN, MAIN_tbSpdR, 8);
    ui.PlaceTxtBx(&ui.rsrc, PG_MAIN, MAIN_tbSpdY, 8);
    
    uiInstanceSqu = 0;  // initial
    maxTest_tmr->start(&maxTest_tmr->rsrc, MAX_TEST_POLLING_TIM, POLLING_REPEAT, maxTest_task, NULL);
        
}

static void maxTest_task(void* e){
    ui.Polling(&ui.rsrc, MAX_TEST_POLLING_TIM);

    r_motion(MAX_TEST_POLLING_TIM);

}

static void machine_initPg10(void){
    print("<%s >", __func__);
    s32 i,y;
    // inital pg10
    ui.Set(&ui.rsrc, PG_MAIN, MAIN_tbUnitR, "txt", "%d.%d", mmPerRev_r/1000, (mmPerRev_r%1000)/100);
    ui.Set(&ui.rsrc, PG_MAIN, MAIN_tbUnitY, "txt", "%d.%d", mmPerRev_y/1000, (mmPerRev_y%1000)/100);
    
    ui.Set(&ui.rsrc, PG_MAIN, MAIN_hSpdR, "minval", "%d", MIN_SPEED_MM_PER_SEC);
    ui.Set(&ui.rsrc, PG_MAIN, MAIN_hSpdY, "minval", "%d", MIN_SPEED_MM_PER_SEC);
    ui.Set(&ui.rsrc, PG_MAIN, MAIN_hSpdR, "maxval", "%d", MAX_SPEED_MM_PER_SEC);
    ui.Set(&ui.rsrc, PG_MAIN, MAIN_hSpdY, "maxval", "%d", MAX_SPEED_MM_PER_SEC);
    ui.Set(&ui.rsrc, PG_MAIN, MAIN_hSpdR, "val", "%d", spd_r);
    ui.Set(&ui.rsrc, PG_MAIN, MAIN_hSpdY, "val", "%d", spd_y);
    
    ui.Set(&ui.rsrc, PG_MAIN, MAIN_bStat, "txt", " ");
    ui.Set(&ui.rsrc, PG_MAIN, MAIN_tbSpdR, "txt", "%d", spd_r);
    ui.Set(&ui.rsrc, PG_MAIN, MAIN_tbSpdY, "txt", "%d", spd_y);

    // bind on event
    ui.Bind(&ui.rsrc, PG_MAIN, MAIN_tbUnitR, UI_EVNT_CHANGE, ui_onChanged_main_tbUnitR);
    ui.Bind(&ui.rsrc, PG_MAIN, MAIN_tbUnitY, UI_EVNT_CHANGE, ui_onChanged_main_tbUnitY);
    ui.Bind(&ui.rsrc, PG_MAIN, MAIN_tbSpdR, UI_EVNT_CHANGE, ui_onChanged_main_tbSpdR);
    ui.Bind(&ui.rsrc, PG_MAIN, MAIN_tbSpdY, UI_EVNT_CHANGE, ui_onChanged_main_tbSpdY);
    
    
    
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







/**********************************************************
 == THE END ==
**********************************************************/

