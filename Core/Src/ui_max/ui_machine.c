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
u16 thickNess = 4200;   // in 1/100 mm
u16 perRev_r = 7200;  // in 1/100 degree
u16 perRev_y = 2400;  // in 1/100 mm

u32 spd_r = 340; // in mm/sec
u32 spd_y = 300; // in mm/sec
u16 cur_r = 4000;   // mA
u16 cur_y = 4000;   // mA
u32 posR[5] = {4000, 8000, 12000, 16000, 20000};
u32 posY[5] = {45000, 55000, 55000, 55000, 55000};    // about 600mm, in 1/100 mm
//s16 slotCount[5] = {0};

u8 isAuto = 0;
u8 isEmerg = 0;
u8 isHoming = 0;
u8 isClamp = 0;
u16 statusBits;
volatile s16 repoCount[5] = {0};
u8 repoIndx = 0;

//u16 repoCount_0 = 0;
//s8 repoCount_1 = 0;
//s8 repoCount_2 = 0;
//s8 repoCount_3 = 0;
//s8 repoCount_4 = 0;
u16 maxCount = 999;

u8 uiInstanceSqu = 0;
u8 uiAutoSqu = 0;
u8 uiAutoSquNxt = 0;

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

static s32 uiHoming_blocking(u16 timeout);
static s32 uiXHoming_blocking(rampDev_t* stpr, u16 spd, u16 timeout);
static s32 uiYAxisHoming_blocking(u16 spd, u16 timeout);
static s32 uiRAxisHoming_blocking(u16 spd, u16 timeout);

static s32 getRepoDepth(u8 repoIndx, u32* length);

mPath2Dev_t mPlayer;
mPath2PosNode* mPath;
static s32 uiMotPath2_goto(u8 axis, u16 spd, s32 pos);
static s32 uiMotPath_getPos(u8 axis, s32* pos);
static s32 uiMotPath_stop(u8 axis);

//static void defaultConf();

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
    if(uiInstanceReadConf() < 0){
        defaultConf();
        uiInstanceSaveConf();
    }
    if(thickNess == 0){
        maxCount = 999;
        log("<%s fetal_error >", __func__);
    }
    else{   maxCount = slotDepth/thickNess;   }
//    log("<%s maxCount:%d >", __func__, maxCount);
    
    maxTest_tmr->start(&maxTest_tmr->rsrc, UI_INST_INTERVAL, POLLING_REPEAT, maxTest_task, NULL);
    machine_initPg00();

    log("</%s uiInstance_initial_done >", __func__);
    
//    mPlayer.addTo(&mPlayer.rsrc, &mPath, 10000, 11000, 63000,63000, 5000);
//    mPlayer.addTo(&mPlayer.rsrc, &mPath, 20000, 21000, 63000,63000, 5000);
//    mPlayer.addTo(&mPlayer.rsrc, &mPath, 30000, 31000, 63000,63000, 5000);
//    mPlayer.addTo(&mPlayer.rsrc, &mPath, 40000, 41000, 63000,63000, 5000);
//    mPlayer.addTo(&mPlayer.rsrc, &mPath, 50000, 51000, 63000,63000, 5000);
//    mPlayer.addTo(&mPlayer.rsrc, &mPath, 40000, 41000, 63000,63000, 5000);
//    mPlayer.addTo(&mPlayer.rsrc, &mPath, 30000, 31000, 63000,63000, 5000);
//    mPlayer.addTo(&mPlayer.rsrc, &mPath, 20000, 21000, 63000,63000, 5000);
//    thread_delay(100);
    
//    mPlayer.start(&mPlayer.rsrc, mPath, uiRestartMPath);
}

static s32 uiHoming_blocking(u16 timeout){
    if(uiYAxisHoming_blocking(65000, timeout)<0){
        log("<%s err:fetal_err uiAutoSqu:%d >", __func__, uiAutoSqu);
        return -1;
    }
    if(uiRAxisHoming_blocking(62000, timeout)<0){
        log("<%s err:fetal_err uiAutoSqu:%d >", __func__, uiAutoSqu);
        return -2;
    }
    return 0;
}

static s32 uiXHoming_blocking(rampDev_t* stpr, u16 spd, u16 timeout){
    u16 tick = 0;
    
    stpr->homing(&stpr->rsrc, spd);   // Y axis homing
    while(stpr->isHoming(&stpr->rsrc)){
        thread_delay(50);
        ui.Polling(&ui.rsrc, 50);   // allow to poll other task
        mPlayer.polling(&mPlayer.rsrc, 50);
        tick += 50;
        if(tick >= timeout){
            stpr->stop(&stpr->rsrc);
            return -99;
        }
    }
    return 0;
}

static s32 uiYAxisHoming_blocking(u16 spd, u16 timeout){
    log("<%s >", __func__);
    return(uiXHoming_blocking(&stprRamp[1],spd,timeout));
}

static s32 uiRAxisHoming_blocking(u16 spd, u16 timeout){
    log("<%s >", __func__);
    return(uiXHoming_blocking(&stprRamp[0],spd,timeout));
}

static s32 uiRAxisGotoRepo_blocking(u8 repo, u16 timeout){
    u32 pos;
    u16 tick = 0;
    
    pos = angleToMStep(posR[repo]);
    log("<%s pos:%d >", __func__, pos);
    stprRamp[0].rsrc.spdMax = 63000;
    stprRamp[0].moveTo(&stprRamp[0].rsrc, pos);
    while(pos != stprRamp[0].rsrc.posCur){
        thread_delay(50);
        ui.Polling(&ui.rsrc, 50);   // allow to poll other task
        mPlayer.polling(&mPlayer.rsrc, 50);
        if(stprRamp[0].isRotating(&stprRamp[0].rsrc) == 0){  break;  }
        tick += 50;
        if(tick >= timeout){
            stprRamp[0].stop(&stprRamp[0].rsrc);
            return -99;
        }
    }
    return 0;
}

static s32 uiYAxisGotoRst_blocking(u16 timeout){
    u16 tick = 0;
    
    stprRamp[1].homing(&stprRamp[1].rsrc, 65300);
    
//    stprRamp[1].gohome(&stprRamp[1].rsrc, 65300);
    while(0 != stprRamp[1].rsrc.posCur){
        thread_delay(50);
        ui.Polling(&ui.rsrc, 50);   // allow to poll other task
        mPlayer.polling(&mPlayer.rsrc, 50);
        if(stprRamp[1].isRotating(&stprRamp[1].rsrc) == 0){  break;  }
        tick += 50;
        if(tick >= timeout){
            stprRamp[1].stop(&stprRamp[1].rsrc);
            return -99;
        }
    }
    return 0;
}

static s32 uiYAxisGotoHalf_blocking(u16 timeout){
    u32 dist, pos;
    u16 tick = 0;
    
    if(getRepoDepth(0, &dist) < 0){
        log("<%s 'fetal_erro@getRepoDepth' >", __func__);
        return -1;
    }
    dist -= 10000;  // in 1/100 mm
    pos = distanceToMStep(dist);
    log("<%s 'y godown to pick' dist:%d pos:%d >", __func__, dist, pos);
    stprRamp[1].rsrc.spdMax = 65300;
    stprRamp[1].moveTo(&stprRamp[1].rsrc, pos);
    while(pos != stprRamp[1].rsrc.posCur){
        thread_delay(50);
        ui.Polling(&ui.rsrc, 50);   // allow to poll other task
        mPlayer.polling(&mPlayer.rsrc, 50);
        if(stprRamp[1].isRotating(&stprRamp[1].rsrc) == 0){  break;  }
        tick += 50;
        if(tick >= timeout){
            stprRamp[1].stop(&stprRamp[1].rsrc);
            return -99;
        }
    }
    return 0;
}

static s32 uiYAxisGotoAct_blocking(u8 repo, u16 timeout){
    u32 dist, pos;
    u16 tick = 0;
    
    if(getRepoDepth(repo, &dist) < 0){
        log("<%s 'fetal_erro@getRepoDepth' >", __func__);
        return -1;
    }
    pos = distanceToMStep(dist);
    log("<%s 'y godown to pick' dist:%d pos:%d >", __func__, dist, pos);
    stprRamp[1].rsrc.spdMax = 65300;
    stprRamp[1].moveTo(&stprRamp[1].rsrc, pos);
    while(pos != stprRamp[1].rsrc.posCur){
        thread_delay(100);
        ui.Polling(&ui.rsrc, 100);   // allow to poll other task
        mPlayer.polling(&mPlayer.rsrc, 100);
        if(stprRamp[1].isRotating(&stprRamp[1].rsrc) == 0){  break;  }
        tick += 100;
        if(tick >= timeout){
            stprRamp[1].stop(&stprRamp[1].rsrc);
            return -99;
        }
    }
    return 0;
}


static u8 workFlow = 0;
static void maxTest_task(void* e){
    s32 i;
    u32 dist,pos;
    ui.Polling(&ui.rsrc, UI_INST_INTERVAL);
    
    mPlayer.polling(&mPlayer.rsrc, UI_INST_INTERVAL);
    
//    r_motion(UI_INST_INTERVAL);
    switch (uiAutoSqu){
        case 0:{    // stop
            break;
        }
        case 1:{
            // auto mode initial
            if(uiHoming_blocking(50000)<0){
                log("<%s err:fetal_err uiAutoSqu:%d >", __func__, uiAutoSqu);
                uiAutoSqu = 0;
                break;
            }
            uiAutoSqu++;
            uiAutoSquNxt = 0xff;
            log("<%s uiAutoSqu:%d >", __func__, uiAutoSqu);
            break;
        }
        // standby
        case 2:{
            // wait AUTO mode exit
            if(uiAutoSquNxt == 0){
                ui.Set(&ui.rsrc, PG_MAIN, MAIN_AUTO, UI_ATTR_VAL, "0");
                uiAutoSqu = 0;
                uiAutoSquNxt = 0xff;
                break;
            }
            
            if((workFlow & BIT(0)) == 0){
                uiAutoSqu = 10;   // 从发料仓取料放在加工仓
            }
            else{
                uiAutoSqu = 20;   // 从加工仓取料放在收料仓
            }
            break;
        }

        // 从发料仓取料放在加工仓
        case 10:{
            g_output.WritePin(&g_output.rsrc,1,0);
            stprRamp[0].rsrc.spdMax = 63000;
            stprRamp[1].rsrc.spdMax = 65000;
            repoIndx = 0;
            for(i=1;i<=4;i++){
                if(repoCount[i]<0){
                    repoIndx = i;
                    break;
                }
            }
            // if CanNOT find a repo, go home
            if(repoIndx == 0){
                log("<%s err:fetal_err uiAutoSqu:%d >", __func__, uiAutoSqu);
                if(uiHoming_blocking(50000)<0){
                    log("<%s err:uiHoming_blocking uiAutoSqu:%d >", __func__, uiAutoSqu);
                    break;
                }
                uiAutoSqu = 0;
                break;     
            }
            log("<%s repoIndx:%d >", __func__, repoIndx);
            // Y axis go home first, in case of
            if(uiYAxisGotoRst_blocking(50000) < 0){
                log("<%s 'err01@uiYAxisGotoRst_blocking' uiAutoSqu:%d >", __func__,uiAutoSqu);
            }
            // 走到对应仓位
            if(uiRAxisGotoRepo_blocking(repoIndx, 50000) < 0){
                log("<%s 'err02@uiRAxisGotoRepo_blocking' uiAutoSqu:%d >", __func__,uiAutoSqu);
            }
            
            // 下去取料
            if(uiYAxisGotoAct_blocking(repoIndx, 50000) < 0){
                log("<%s 'err03@uiYAxisGotoAct_blocking' uiAutoSqu:%d >", __func__,uiAutoSqu);
            }
            g_output.WritePin(&g_output.rsrc, 0, 1); // close
            // 走回来
            if(uiYAxisGotoRst_blocking(50000) < 0){
                log("<%s 'err04@uiYAxisGotoRst_blocking' uiAutoSqu:%d >", __func__,uiAutoSqu);
            }
            // 走到加工仓位
            if(uiRAxisGotoRepo_blocking(0, 50000) < 0){
                log("<%s 'err05@uiRAxisGotoRepo_blocking' uiAutoSqu:%d >", __func__,uiAutoSqu);
            }
            
            // 走到中间位置standby
            if(uiYAxisGotoHalf_blocking(50000) < 0){
                log("<%s 'err07@uiYAxisGotoHalf_blocking' uiAutoSqu:%d >", __func__,uiAutoSqu);
            }
            uiAutoSqu++;
            log("<%s uiAutoSqu:%d >", __func__, uiAutoSqu);
            break;
        }
        // Wait empty event
        case 11:{
            // 处理退出自动模式事件
            if(uiAutoSquNxt == 0){
                ui.Set(&ui.rsrc, PG_MAIN, MAIN_AUTO, UI_ATTR_VAL, "0");
                uiAutoSqu = 0;
                uiAutoSquNxt = 0xff;
                break;
            }
            if(g_input.ReadPin(&g_input.rsrc,0) == 0 ){
                break;
            }            
            
            // 走下去
            if(uiYAxisGotoAct_blocking(0, 50000) < 0){
                log("<%s 'err08@uiYAxisGotoAct_blocking' uiAutoSqu:%d >", __func__,uiAutoSqu);
            }
            // 放料
            g_output.WritePin(&g_output.rsrc, 0, 0); // open
            
            // 走回来
            if(uiYAxisGotoHalf_blocking(50000) < 0){
                log("<%s 'err09@uiYAxisGotoHalf_blocking' uiAutoSqu:%d >", __func__,uiAutoSqu);
            }
            // send signal to app
            g_output.WritePin(&g_output.rsrc,1,1);
            
            workFlow |= BIT(0); // next flow
            uiAutoSqu = 2;
            uiAutoSquNxt = 0xff;    // 期间的退出自动事件忽略
            break;            
        }

        // 从加工仓取料放在收料仓
        case 20:{
            // 处理退出自动模式事件
            if(uiAutoSquNxt == 0){
                ui.Set(&ui.rsrc, PG_MAIN, MAIN_AUTO, UI_ATTR_VAL, "0");
                uiAutoSqu = 0;
                uiAutoSquNxt = 0xff;
                break;
            }
            // 等待信号
            if(g_input.ReadPin(&g_input.rsrc,0) == 0 ){
                break;
            }
            // 查找可用的收料仓
            repoIndx = 0;
            for(i=1;i<=4;i++){
                if(repoCount[i]>0 && repoCount[i]<posY[i]/thickNess){
                    repoIndx = i;
                    break;
                }
            }
            // if cannot find a repo, go home
            if(repoIndx == 0){
                log("<%s err:fetal_err uiAutoSqu:%d >", __func__, uiAutoSqu);
                if(uiHoming_blocking(50000)<0){
                    log("<%s err:uiHoming_blocking uiAutoSqu:%d >", __func__, uiAutoSqu);
                    break;
                }
                uiAutoSqu = 0;
                break;     
            }
            // 走下去
            if(uiYAxisGotoAct_blocking(0, 50000) < 0){
                log("<%s 'err010@uiYAxisGotoAct_blocking' uiAutoSqu:%d >", __func__,uiAutoSqu);
            }
            // 取料
            g_output.WritePin(&g_output.rsrc, 0, 1); // close
            // 走回来
            if(uiYAxisGotoRst_blocking(50000) < 0){
                log("<%s 'err11@uiYAxisGotoRst_blocking' uiAutoSqu:%d >", __func__,uiAutoSqu);
            }

            // R走到收料仓位
            if(uiRAxisGotoRepo_blocking(repoIndx, 50000) < 0){
                log("<%s 'err12' uiAutoSqu:%d >", __func__,uiAutoSqu);
            }
            // 走下去
            if(uiYAxisGotoAct_blocking(0, 50000) < 0){
                log("<%s err13 uiAutoSqu:%d >", __func__,uiAutoSqu);
            }
            // 放料
            g_output.WritePin(&g_output.rsrc, 0, 0); 
            // 走回来
            if(uiYAxisGotoRst_blocking(50000) < 0){
                log("<%s err14 uiAutoSqu:%d >", __func__,uiAutoSqu);
            }
            // reset the flow
            workFlow = 0;
            uiAutoSqu = 2;
            uiAutoSquNxt = 0xff;    // ignore the AUTO Mod change request
            break;
        }
    }
    
    
    
}

static void machine_initPg00(void){
//    log("<%s >", __func__);
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

    ui.Set(&ui.rsrc, PG_MAIN, MAIN_REPO0, UI_ATTR_VAL, "%d", repoCount[0]);
    ui.Set(&ui.rsrc, PG_MAIN, MAIN_REPO1, UI_ATTR_VAL, "%d", repoCount[1]);
    ui.Set(&ui.rsrc, PG_MAIN, MAIN_REPO2, UI_ATTR_VAL, "%d", repoCount[2]);
    ui.Set(&ui.rsrc, PG_MAIN, MAIN_REPO3, UI_ATTR_VAL, "%d", repoCount[3]);
    ui.Set(&ui.rsrc, PG_MAIN, MAIN_REPO4, UI_ATTR_VAL, "%d", repoCount[4]);
    
    ui.Page(&ui.rsrc, PG_MAIN);

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

    ui.Bind(&ui.rsrc, PG_MAIN, MAIN_EMERGENCY, UI_EVNT_CLICK, ui_onClick_emergence);
    ui.Bind(&ui.rsrc, PG_MAIN, MAIN_HOMING, UI_EVNT_CLICK, ui_onClick_homing);
    ui.Bind(&ui.rsrc, PG_MAIN, MAIN_CLAMP, UI_EVNT_CLICK, ui_onClick_clamp);
    ui.Bind(&ui.rsrc, PG_MAIN, MAIN_AUTO, UI_EVNT_CLICK, ui_onClick_main_bAuto);

    ui.Bind(&ui.rsrc, PG_MAIN, MAIN_REPO0, UI_EVNT_EDIT_STR, ui_onEdit_repo0);
    ui.Bind(&ui.rsrc, PG_MAIN, MAIN_REPO1, UI_EVNT_EDIT_STR, ui_onEdit_repo1);
    ui.Bind(&ui.rsrc, PG_MAIN, MAIN_REPO2, UI_EVNT_EDIT_STR, ui_onEdit_repo2);
    ui.Bind(&ui.rsrc, PG_MAIN, MAIN_REPO3, UI_EVNT_EDIT_STR, ui_onEdit_repo3);
    ui.Bind(&ui.rsrc, PG_MAIN, MAIN_REPO4, UI_EVNT_EDIT_STR, ui_onEdit_repo4);


//    log("</%s >", __func__);
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
//    log("<%s >", __func__);
    u32 checkSum = 0xa5;
    s32 i,j;
    u16 addr = uiInst_ioBase + sizeof(checkSum);
    if(uiInst_ioWrite==NULL){
        return -1;
    }

    j = uiInst_ioWrite(addr, (const u8*)&slotDepth, sizeof(slotDepth));
    checkSum += slotDepth;  
//    log("<%s j:%d addr:0x%08x slotDepth:%d checkSum:%d >", __func__, j, addr,slotDepth,checkSum);
    addr += sizeof(slotDepth);
    thread_delay(5);

    uiInst_ioWrite(addr, (const u8*)&thickNess, sizeof(thickNess));
    checkSum += thickNess;
//    log("<%s j:%d addr:0x%08x thickNess:%d checkSum:%d >", __func__, j, addr,thickNess,checkSum);
    addr += sizeof(thickNess);
    thread_delay(5);
    
    uiInst_ioWrite(addr, (const u8*)&perRev_r, sizeof(perRev_r));   
    checkSum += perRev_r;  
//    log("<%s j:%d addr:0x%08x perRev_r:%d checkSum:%d >", __func__, j, addr,perRev_r,checkSum);
    addr += sizeof(perRev_r);
    thread_delay(5);
    
    uiInst_ioWrite(addr, (const u8*)&perRev_y, sizeof(perRev_y));
    checkSum += perRev_y;    
//    log("<%s j:%d addr:0x%08x perRev_y:%d checkSum:%d >", __func__, j, addr,perRev_y,checkSum);
    addr += sizeof(perRev_y);
    thread_delay(5);

    uiInst_ioWrite(addr, (const u8*)&spd_r, sizeof(spd_r));   
    checkSum += spd_r;
//    log("<%s j:%d addr:0x%08x spd_r:%d checkSum:%d >", __func__, j, addr,spd_r,checkSum);
    addr += sizeof(spd_r);
    thread_delay(5);

    uiInst_ioWrite(addr, (const u8*)&spd_y, sizeof(spd_y));   
    checkSum += spd_y;  
//    log("<%s j:%d addr:0x%08x spd_y:%d checkSum:%d >", __func__, j, addr,spd_y,checkSum);
    addr += sizeof(spd_y);
    thread_delay(5);
    
    uiInst_ioWrite(addr, (const u8*)&cur_r, sizeof(cur_r));  
    checkSum += cur_r;  
//    log("<%s j:%d addr:0x%08x cur_r:%d checkSum:%d >", __func__, j, addr,cur_r,checkSum);
    addr += sizeof(cur_r);
    thread_delay(5);

    uiInst_ioWrite(addr, (const u8*)&cur_y, sizeof(cur_y));   
    checkSum += cur_y;  
//    log("<%s j:%d addr:0x%08x cur_y:%d checkSum:%d >", __func__, j, addr,cur_y,checkSum);
    addr += sizeof(cur_y);
    thread_delay(5);

    for(i=0;i<5;i++){
        uiInst_ioWrite(addr, (u8*)&posR[i], sizeof(posR[i]));
        checkSum += posR[i]; 
//        log("<%s j:%d addr:0x%08x posR[%d]:%d checkSum:%d >", __func__, j, addr,i,posR[i],checkSum);
        addr += sizeof(posR[i]);
        thread_delay(5);
    }
    
    for(i=0;i<5;i++){
        uiInst_ioWrite(addr, (u8*)&posY[i], sizeof(posY[i]));
        checkSum += posY[i]; 
//        log("<%s j:%d addr:0x%08x posY[%d]:%d checkSum:%d >", __func__, j, addr, i, posY[i],checkSum);
        addr += sizeof(posY[i]);
        thread_delay(5);
    }
    
    uiInst_ioWrite(addr, (const u8*)&statusBits, sizeof(statusBits));
    checkSum += statusBits;  
//    log("<%s j:%d addr:0x%08x statusBits:%d checkSum:%d >", __func__, j, addr, statusBits,checkSum);
    addr += sizeof(statusBits);
    thread_delay(5);

    for(i=0;i<5;i++){
        uiInst_ioWrite(addr, (u8*)&repoCount[i], sizeof(repoCount[i]));
        checkSum += repoCount[i];
//        log("<%s j:%d addr:0x%08x repoCount[%d]:%d checkSum:%d >", __func__, j, addr, i, repoCount[i],checkSum);
        addr += sizeof(repoCount[i]);
        thread_delay(5);
    }
    
    uiInst_ioWrite(uiInst_ioBase, (const u8*)&checkSum, sizeof(checkSum));
    thread_delay(5);
//    log("</%s checkSum:0x%08x >", __func__, checkSum);
    return 0;
}

s32 uiInstanceReadConf(void){
    u32 checkSum = 0xa5, checkCode;
    s32 i;
    u16 addr;
    if(uiInst_ioRead==NULL){
        return -1;
    }
    
    addr = uiInst_ioBase;
    uiInst_ioRead(addr, (u8*)&checkCode, sizeof(checkCode));
//    log("<%s addr:0x%08x checkCode:%08x >", __func__, addr,checkCode);
    addr += sizeof(checkCode);
    
    uiInst_ioRead(addr, (u8*)&slotDepth, sizeof(slotDepth));
    checkSum += slotDepth;
//    log("<%s addr:0x%08x slotDepth:%d checkSum:%d >", __func__, addr,slotDepth,checkSum);
    addr += sizeof(slotDepth);  
    

    uiInst_ioRead(addr, (u8*)&thickNess, sizeof(thickNess));
    checkSum += thickNess;
//    log("<%s addr:0x%08x thickNess:%d checkSum:%d >", __func__, addr,thickNess,checkSum);
    addr += sizeof(thickNess);  
    

    uiInst_ioRead(addr, (u8*)&perRev_r, sizeof(perRev_r));
    checkSum += perRev_r;
//    log("<%s addr:0x%08x perRev_r:%d checkSum:%d >", __func__, addr,perRev_r,checkSum);
    addr += sizeof(perRev_r);  
    

    uiInst_ioRead(addr, (u8*)&perRev_y, sizeof(perRev_y));
    checkSum += perRev_y;
//    log("<%s addr:0x%08x perRev_y:%d checkSum:%d >", __func__, addr,perRev_y,checkSum);
    addr += sizeof(perRev_y);  
    
    
    uiInst_ioRead(addr, (u8*)&spd_r, sizeof(spd_r));
    checkSum += spd_r;
//    log("<%s addr:0x%08x spd_r:%d checkSum:%d >", __func__,addr, spd_r,checkSum);
    addr += sizeof(spd_r);  
    
    
    uiInst_ioRead(addr, (u8*)&spd_y, sizeof(spd_y));
    checkSum += spd_y;
//    log("<%s addr:0x%08x spd_y:%d checkSum:%d >", __func__, addr, spd_y,checkSum);
    addr += sizeof(spd_y);  
    
    
    uiInst_ioRead(addr, (u8*)&cur_r, sizeof(cur_r));
    checkSum += cur_r;
//    log("<%s addr:0x%08x cur_r:%d checkSum:%d >", __func__,addr, cur_r,checkSum);
    addr += sizeof(cur_r);  
    
    
    uiInst_ioRead(addr, (u8*)&cur_y, sizeof(cur_y));
    checkSum += cur_y;
//    log("<%s addr:0x%08x cur_y:%d checkSum:%d >", __func__,addr, cur_y,checkSum);
    addr += sizeof(cur_y);  
    
    for(i=0;i<5;i++){
        uiInst_ioRead(addr, (u8*)&posR[i], sizeof(posR[i]));
        checkSum += posR[i];
//        log("<%s addr:0x%08x posR[%d]:%d checkSum:%d >", __func__, addr,i, posR[i],checkSum);
        addr += sizeof(posR[i]);  
        
    }
    
    for(i=0;i<5;i++){
        uiInst_ioRead(addr, (u8*)&posY[i], sizeof(posY[i]));
        checkSum += posY[i];
//        log("<%s addr:0x%08x posY[%d]:%d checkSum:%d >", __func__, addr,i, posY[i],checkSum);
        addr += sizeof(posY[i]);  
        
    }
    
    uiInst_ioRead(addr, (u8*)&statusBits, sizeof(statusBits));
    checkSum += statusBits;
//    log("<%s addr:0x%08x statusBits:%04x checkSum:%d >", __func__, addr,statusBits,checkSum);
    addr += sizeof(statusBits);
    

    for(i=0;i<5;i++){
        repoCount[i] = 0x1234;
        uiInst_ioRead(addr, (u8*)&repoCount[i], sizeof(repoCount[i]));
        checkSum += repoCount[i];
//        log("<%s addr:0x%08x repoCount[%d]:%d checkSum:%d >", __func__, addr,i, repoCount[i],checkSum);
        addr += sizeof(repoCount[i]);  
    }
    
    if(checkCode != checkSum){
//        log("<%s checkSum:0x%08x >", __func__, checkSum);
        return -2;
    }
    return 0;
}

void defaultConf(){
    
    slotDepth = 60000;  // total slot depth
    cur_r = 4000;   // mA
    cur_y = 4000;   // mA
    
    thickNess = 4200;   // in 1/100 mm
    perRev_r = 7200;  // in 1/100 degree
    perRev_y = 2400;  // in 1/100 mm

    maxCount = slotDepth/thickNess;
    
    spd_r = 340; // in mm/sec
    spd_y = 300; // in mm/sec
    
    posR[0] = 4000;
    posR[1] = 8000;
    posR[2] = 12000;
    posR[3] = 16000;
    posR[4] = 20000;
    
    posY[0] = 45000;
    posY[1] = 55000;
    posY[2] = 55000;
    posY[3] = 55000;
    posY[4] = 55000;
    
    repoCount[0] = 1;
    repoCount[1] = 3;
    repoCount[2] = 4;
    repoCount[3] = -5;
    repoCount[4] = -6;
    
}

// angle in 1/100 degree
s32 angleToMStep(u32 degree){
//    log("<%s degree:%d unint:%d >", __func__, degree, stprRamp[1].rsrc.mStep);
    if(perRev_r == 0){
        return -1;
    }
    s32 mstep = degree*200*stprRamp[1].rsrc.mStep/perRev_r;
//    log("</%s mstep:%d >", __func__, mstep);
    return (mstep);

}

// dist in 1/100 mm
s32 distanceToMStep(u32 dist){
    if(perRev_y == 0){
        return -1;
    }
    s32 mstep = dist*200*stprRamp[1].rsrc.mStep/perRev_y;
//    log("<%s mstep:%d >", __func__, mstep);
    return mstep;
}

static s32 getRepoDepth(u8 repoIndx, u32* length){
    s32 rtn = 0;
    
    *length = posY[repoIndx] - abs(repoCount[repoIndx])*thickNess;

    return rtn;
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

