/**********************************************************
filename: max_tester_ui.c
**********************************************************/

/************************����ͷ�ļ�***************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "uartDev.h"
#include "max_tester_ui.h"
#include "board.h"

#define COLOR_BACKGROUND    ("61277")     // background color
#define COLOR_RED           ("63488")     // red color
#define COLOR_GREEN         ("2016")      // green color

#define ITEM_LINES          (9)         // the test items count
#define MAX_TEST_POLLING_TIM    (16)

/**********************************************************
 Private variable
**********************************************************/


//static uiPage_t pg00;
//static uiPage_t pg10;
//static uiPage_t pg20;
//static uiPage_t pgxx;


uiDev_T ui;
appTmrDev_t* maxTest_tmr;

static void print_lcd(const char* FORMAT_ORG, ...);
static void maxTest_pg00_updateLine(u8 lineIndx, const char* DESC, const char* LOWER, const char* UPPER, const char* UNIT, const char* RSLT, const char* TIM, const char* BG_COLOR);
static void maxTest_pg00_updateLineX(u8 lineIndx, const char* RSLT, const char* TIM, const char* BG_COLOR);
static void maxTest_initPg00(void);

static u16 maxTest_squ = 0;
static u16 maxTest_tick = 0;
static void maxTest_task(void* e);

/**********************************************************
 Private function
**********************************************************/


/**********************************************************
 Public function
**********************************************************/
void  max_tester_ui_initial(
    UartDev_t* uartDev,
    appTmrDev_t* tObj
){
    s32 i;
    char buff[16];
    
    uiSetup(&ui, uartDev, print_lcd);
    maxTest_tmr = tObj;

    // new pages
    ui.NewPage(&ui.rsrc, "pg00");
    ui.NewPage(&ui.rsrc, "pg10");
    ui.NewPage(&ui.rsrc, "pg20");
    ui.NewPage(&ui.rsrc, "pgxx");

    ui.PlaceTxtBx(&ui.rsrc, "pgxx", "t_about", 0);

    // setup pg00
    ui.PlaceTxtBx(&ui.rsrc, "pg00", "t_title", 48);    // project's name
    ui.PlaceTxtBx(&ui.rsrc, "pg00", "t_total", 8);    // total test counter
    ui.PlaceTxtBx(&ui.rsrc, "pg00", "t_fail", 8);    // fail counter
    ui.PlaceTxtBx(&ui.rsrc, "pg00", "t_percent", 8);    // pass rate in percent
    ui.PlaceTxtBx(&ui.rsrc, "pg00", "t_run", 8);        // run time in second
    
    // create test itmes table lines 
    for(i=1;i<=ITEM_LINES;i++){
        // test item line's description
        memset(buff,0,16);
        sprintf(buff, "t%d_desc", i);
        ui.PlaceTxtBx(&ui.rsrc, "pg00", buff, 0);
        // test item line's lower
        memset(buff,0,16);
        sprintf(buff, "t%d_lower", i);
        ui.PlaceTxtBx(&ui.rsrc, "pg00", buff, 0);
        // test item line's upper
        memset(buff,0,16);
        sprintf(buff, "t%d_upper", i);
        ui.PlaceTxtBx(&ui.rsrc, "pg00", buff, 0);
        // test item line's unit
        memset(buff,0,16);
        sprintf(buff, "t%d_unit", i);
        ui.PlaceTxtBx(&ui.rsrc, "pg00", buff, 0);
        // test item line's result
        memset(buff,0,16);
        sprintf(buff, "t%d_rslt", i);
        ui.PlaceTxtBx(&ui.rsrc, "pg00", buff, 0);
        // test item line's time
        memset(buff,0,16);
        sprintf(buff, "t%d_tim", i);
        ui.PlaceTxtBx(&ui.rsrc, "pg00", buff, 0);
    }
    
    maxTest_squ = 0;
    maxTest_tmr->start(&maxTest_tmr->rsrc, MAX_TEST_POLLING_TIM, POLLING_REPEAT, maxTest_task, NULL);
    
}

static void maxTest_task(void* e){
    ui.Polling(&ui.rsrc, MAX_TEST_POLLING_TIM);

    maxTest_tick += MAX_TEST_POLLING_TIM;
    if(maxTest_tick < 64){
        return;
    }
    maxTest_tick = 0;
    
    switch(maxTest_squ){
        case 0: {
            if(ui.rsrc.hasLoaded){
                maxTest_initPg00();
                maxTest_squ++;
            }
            break;            
        }
       
        case 1:{
            break;
        }
    }
}

static void maxTest_initPg00(void){
    // inital pg00
    ui.Set(&ui.rsrc, "pg00", "t_title", "txt", "USIXXX PCBA TESTER");
    ui.Set(&ui.rsrc, "pg00", "t_total", "txt", "9999");
    ui.Set(&ui.rsrc, "pg00", "t_fail", "txt", "0000");
    ui.Set(&ui.rsrc, "pg00", "t_percent", "txt", "100");
    ui.Set(&ui.rsrc, "pg00", "t_run", "txt", "123.4S");

    // inital lines
    maxTest_pg00_updateLine(1, "1. Test Volt", "2.0","3.2","V","--", "--", COLOR_BACKGROUND);
    maxTest_pg00_updateLine(2, "2. Test Volt", "2.1","3.2","V","--", "--", COLOR_BACKGROUND);
    maxTest_pg00_updateLine(3, "3. Test Volt", "2.2","3.2","V","--", "--", COLOR_BACKGROUND);
    maxTest_pg00_updateLine(4, "4. Test Volt", "2.3","3.2","V","--", "--", COLOR_BACKGROUND);
    maxTest_pg00_updateLine(5, "5. Test Volt", "2.4","3.2","V","--", "--", COLOR_BACKGROUND);
    maxTest_pg00_updateLine(6, "6. Test Volt", "2.5","3.2","V","--", "--", COLOR_BACKGROUND);
    maxTest_pg00_updateLine(7, "7. Test Volt", "2.6","3.2","V","--", "--", COLOR_BACKGROUND);
    maxTest_pg00_updateLine(8, "8. Test Volt", "2.7","3.2","V","--", "--", COLOR_BACKGROUND);
    maxTest_pg00_updateLine(9, "9. Test Volt", "2.8","3.2","V","--", "--", COLOR_BACKGROUND);

}

static void maxTest_pg00_updateLine(u8 lineIndx, const char* DESC, const char* LOWER, const char* UPPER, const char* UNIT, const char* RSLT, const char* TIM, const char* BG_COLOR){
    char objName[16];
    if(lineIndx > ITEM_LINES){    return;   } 
    // description
    memset(objName,0,16);
    sprintf(objName, "t%d_desc", lineIndx);
    ui.Set(&ui.rsrc, "pg00", objName, "txt", DESC);
    ui.Set(&ui.rsrc, "pg00", objName, "bco", BG_COLOR);
    ui.Polling(&ui.rsrc, 999);
    // lower
    memset(objName,0,16);
    sprintf(objName, "t%d_lower", lineIndx);
    ui.Set(&ui.rsrc, "pg00", objName, "txt", LOWER);
    ui.Set(&ui.rsrc, "pg00", objName, "bco", BG_COLOR);
    ui.Polling(&ui.rsrc, 999);
    // upper
    memset(objName,0,16);
    sprintf(objName, "t%d_upper", lineIndx);
    ui.Set(&ui.rsrc, "pg00", objName, "txt", UPPER);
    ui.Set(&ui.rsrc, "pg00", objName, "bco", BG_COLOR);
    ui.Polling(&ui.rsrc, 999);
    // unit
    memset(objName,0,16);
    sprintf(objName, "t%d_unit", lineIndx);
    ui.Set(&ui.rsrc, "pg00", objName, "txt", UNIT);
    ui.Set(&ui.rsrc, "pg00", objName, "bco", BG_COLOR);
    ui.Polling(&ui.rsrc, 999);
    // result
    memset(objName,0,16);
    sprintf(objName, "t%d_rslt", lineIndx);
    ui.Set(&ui.rsrc, "pg00", objName, "txt", RSLT);
    ui.Set(&ui.rsrc, "pg00", objName, "bco", BG_COLOR);
    ui.Polling(&ui.rsrc, 999);
    // time
    memset(objName,0,16);
    sprintf(objName, "t%d_tim", lineIndx);
    ui.Set(&ui.rsrc, "pg00", objName, "txt", TIM);
    ui.Set(&ui.rsrc, "pg00", objName, "bco", BG_COLOR);
    ui.Polling(&ui.rsrc, 999);
}

static void maxTest_pg00_updateLineX(u8 lineIndx, const char* RSLT, const char* TIM, const char* BG_COLOR){
    char objName[16];
    if(lineIndx >= ITEM_LINES){    return;   } 
    // description
    memset(objName,0,16);
    sprintf(objName, "t%d_desc", lineIndx);
    ui.Set(&ui.rsrc, "pg00", objName, "bco", BG_COLOR);
    ui.Polling(&ui.rsrc, 999);
    // lower
    memset(objName,0,16);
    sprintf(objName, "t%d_lower", lineIndx);
    ui.Set(&ui.rsrc, "pg00", objName, "bco", BG_COLOR);
    ui.Polling(&ui.rsrc, 999);
    // upper
    memset(objName,0,16);
    sprintf(objName, "t%d_upper", lineIndx);
    ui.Set(&ui.rsrc, "pg00", objName, "bco", BG_COLOR);
    ui.Polling(&ui.rsrc, 999);
    // unit
    memset(objName,0,16);
    sprintf(objName, "t%d_unit", lineIndx);
    ui.Set(&ui.rsrc, "pg00", objName, "bco", BG_COLOR);
    ui.Polling(&ui.rsrc, 999);
    // result
    memset(objName,0,16);
    sprintf(objName, "t%d_rslt", lineIndx);
    ui.Set(&ui.rsrc, "pg00", objName, "txt", RSLT);
    ui.Set(&ui.rsrc, "pg00", objName, "bco", BG_COLOR);
    ui.Polling(&ui.rsrc, 999);
    // time
    memset(objName,0,16);
    sprintf(objName, "t%d_tim", lineIndx);
    ui.Set(&ui.rsrc, "pg00", objName, "txt", TIM);
    ui.Set(&ui.rsrc, "pg00", objName, "bco", BG_COLOR);
    ui.Polling(&ui.rsrc, 999);
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
    if(bytes>0)    {
        buf[bytes++] = 0xff; 
        buf[bytes++] = 0xff; 
        buf[bytes++] = 0xff; 
        ui.rsrc.pUartDev->Send(&ui.rsrc.pUartDev->rsrc, (u8*)buf, bytes);
    }
}


/**********************************************************
 == THE END ==
**********************************************************/

