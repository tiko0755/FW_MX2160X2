#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "ui_def_instance.h"
#include "ui_cb_instance.h"
#include "ui_machine.h"
#include "user_log.h"
#include "thread_delay.h"

// set the position #0
void ui_onEdit_ps0(int argc, ...){
//    log("<%s >",__func__);
    float f;
    va_list ap;
    char*p;
    if(argc == 1){
        va_start(ap, argc);
        p = va_arg(ap, char*);
        va_end(ap);
        if(sscanf(p, "\"%f\"", &f)==1){
            posR[0] = f*1000; 
            if(posR[0]%10 >= 5){
                posR[0]/=10;
                posR[0]++;
            }
            else{
                posR[0]/=10;
            }
            ui.Set(&ui.rsrc, PG_MAIN, MAIN_PS0, UI_ATTR_TXT, "%d.%d", posR[0]/100, posR[0]%100);
            uiInstanceSaveConf();
        }
    }
//    log("</%s >",__func__);
}

// set the position #0
void ui_onEdit_ps0_act(int argc, ...){
//    log("<%s >",__func__);
    float f;
    va_list ap;
    char*p;
    if(argc == 1){
        va_start(ap, argc);
        p = va_arg(ap, char*);
        va_end(ap);
        if(sscanf(p, "\"%f\"", &f)==1){
            posY[0] = f*1000; 
            if(posY[0]%10 >= 5){
                posY[0]/=10;
                posY[0]++;
            }
            else{
                posY[0]/=10;
            }
            ui.Set(&ui.rsrc, PG_MAIN, MAIN_PS0_ACT, UI_ATTR_TXT, "%d.%d", posY[0]/100, posY[0]%100);
            uiInstanceSaveConf();
        }
    }
//    log("</%s >",__func__);
}

void ui_onEdit_ps1(int argc, ...){
//    log("<%s >",__func__);
    float f;
    va_list ap;
    char*p;
    if(argc == 1){
        va_start(ap, argc);
        p = va_arg(ap, char*);
        va_end(ap);
        if(sscanf(p, "\"%f\"", &f)==1){
//            log("<%s f:%f posR[1]:%d >",__func__, f, posR[1]);
            thread_delay(1000);
            posR[1] = f*1000;
            if(posR[1]%10 >= 5){
                posR[1]/=10;
                posR[1]++;
            }
            else{
                posR[1]/=10;
            }
            ui.Set(&ui.rsrc, PG_MAIN, MAIN_PS1, UI_ATTR_TXT, "%d.%d", posR[1]/100, posR[1]%100);
            uiInstanceSaveConf();
        }
    }
//    log("</%s >",__func__);
}
void ui_onEdit_ps1_act(int argc, ...){
//    log("<%s >",__func__);
    float f;
    va_list ap;
    char*p;
    if(argc == 1){
        va_start(ap, argc);
        p = va_arg(ap, char*);
        va_end(ap);
        if(sscanf(p, "\"%f\"", &f)==1){
            posY[1] = f*1000; 
            if(posY[1]%10 >= 5){
                posY[1]/=10;
                posY[1]++;
            }
            else{
                posY[1]/=10;
            }
            ui.Set(&ui.rsrc, PG_MAIN, MAIN_PS1_ACT, UI_ATTR_TXT, "%d.%d", posY[1]/100, posY[1]%100);
            uiInstanceSaveConf();
        }
    }
//    log("</%s >",__func__);
}

void ui_onEdit_ps2(int argc, ...){
//    log("<%s >",__func__);
    float f;
    va_list ap;
    char*p;
    if(argc == 1){
        va_start(ap, argc);
        p = va_arg(ap, char*);
        va_end(ap);
        if(sscanf(p, "\"%f\"", &f)==1){
            posR[2] = f*1000; 
            if(posR[2]%10 >= 5){
                posR[2]/=10;
                posR[2]++;
            }
            else{
                posR[2]/=10;
            }
            ui.Set(&ui.rsrc, PG_MAIN, MAIN_PS2, UI_ATTR_TXT, "%d.%d", posR[2]/100, posR[2]%100);
            uiInstanceSaveConf();
        }
    }
//    log("</%s >",__func__);
}
void ui_onEdit_ps2_act(int argc, ...){
//    log("<%s >",__func__);
    float f;
    va_list ap;
    char*p;
    if(argc == 1){
        va_start(ap, argc);
        p = va_arg(ap, char*);
        va_end(ap);
        if(sscanf(p, "\"%f\"", &f)==1){
            posY[2] = f*1000; 
            if(posY[2]%10 >= 5){
                posY[2]/=10;
                posY[2]++;
            }
            else{
                posY[2]/=10;
            }
            ui.Set(&ui.rsrc, PG_MAIN, MAIN_PS2_ACT, UI_ATTR_TXT, "%d.%d", posY[2]/100, posY[2]%100);
            uiInstanceSaveConf();
        }
    }
//    log("</%s >",__func__);
}

void ui_onEdit_ps3(int argc, ...){
//    log("<%s >",__func__);
    float f;
    va_list ap;
    char*p;
    if(argc == 1){
        va_start(ap, argc);
        p = va_arg(ap, char*);
        va_end(ap);
        if(sscanf(p, "\"%f\"", &f)==1){
            posR[3] = f*1000; 
            if(posR[3]%10 >= 5){
                posR[3]/=10;
                posR[3]++;
            }
            else{
                posR[3]/=10;
            }
            ui.Set(&ui.rsrc, PG_MAIN, MAIN_PS3, UI_ATTR_TXT, "%d.%d", posR[3]/100, posR[3]%100);
            uiInstanceSaveConf();
        }
    }
//    log("</%s >",__func__);
}
void ui_onEdit_ps3_act(int argc, ...){
//    log("<%s >",__func__);
    float f;
    va_list ap;
    char*p;
    if(argc == 1){
        va_start(ap, argc);
        p = va_arg(ap, char*);
        va_end(ap);
        if(sscanf(p, "\"%f\"", &f)==1){
            posY[3] = f*1000; 
            if(posY[3]%10 >= 5){
                posY[3]/=10;
                posY[3]++;
            }
            else{
                posY[3]/=10;
            }
            ui.Set(&ui.rsrc, PG_MAIN, MAIN_PS3_ACT, UI_ATTR_TXT, "%d.%d", posY[3]/100, posY[3]%100);
            uiInstanceSaveConf();
        }
    }
//    log("</%s >",__func__);
}

void ui_onEdit_ps4(int argc, ...){
//    log("<%s >",__func__);
    float f;
    va_list ap;
    char*p;
    if(argc == 1){
        va_start(ap, argc);
        p = va_arg(ap, char*);
        va_end(ap);
        if(sscanf(p, "\"%f\"", &f)==1){
            posR[4] = f*1000; 
            if(posR[4]%10 >= 5){
                posR[4]/=10;
                posR[4]++;
            }
            else{
                posR[4]/=10;
            }
            ui.Set(&ui.rsrc, PG_MAIN, MAIN_PS4, UI_ATTR_TXT, "%d.%d", posR[4]/100, posR[4]%100);
            uiInstanceSaveConf();
        }
    }
//    log("</%s >",__func__);
}
void ui_onEdit_ps4_act(int argc, ...){
//    log("<%s >",__func__);
    float f;
    va_list ap;
    char*p;
    if(argc == 1){
        va_start(ap, argc);
        p = va_arg(ap, char*);
        va_end(ap);
        if(sscanf(p, "\"%f\"", &f)==1){
            posY[4] = f*1000; 
            if(posY[4]%10 >= 5){
                posY[4]/=10;
                posY[4]++;
            }
            else{
                posY[4]/=10;
            }
            ui.Set(&ui.rsrc, PG_MAIN, MAIN_PS4_ACT, UI_ATTR_TXT, "%d.%d", posY[4]/100, posY[4]%100);
            uiInstanceSaveConf();
        }
    }
//    log("</%s >",__func__);
}

void ui_onChanged_main_tbSpdY(int argc, ...){
    s32 x;
    float f;
    va_list ap;
    char*p;
    if(argc == 1){
        va_start(ap, argc);
        p = va_arg(ap, char*);
        va_end(ap);
        spd_y = atoi(p);
        log("<%s spd_y:%d >", __func__, spd_y);
    }
}

// set the thickness of product
void ui_onEdit_thickness(int argc, ...){
    float f;
    va_list ap;
    char*p;
    if(argc == 1){
        va_start(ap, argc);
        p = va_arg(ap, char*);
        va_end(ap);
        if(sscanf(p, "\"%f\"", &f)==1){
            thickNess = f*100;
            ui.Set(&ui.rsrc, PG_MAIN, MAIN_THICKNESS, UI_ATTR_TXT, "%d.%d", thickNess/100, thickNess%100);
            uiInstanceSaveConf();
        }
    }
}

// set the angle/rev of R axis 
void ui_onEdit_r_rev(int argc, ...){
    float f;
    va_list ap;
    char*p;
    if(argc == 1){
        va_start(ap, argc);
        p = va_arg(ap, char*);
        va_end(ap);
        if(sscanf(p, "\"%f\"", &f)==1){
            perRev_r = f*100; 
            ui.Set(&ui.rsrc, PG_MAIN, MAIN_R_REV, UI_ATTR_TXT, "%d.%d", perRev_r/100, perRev_r%100);
            uiInstanceSaveConf();
        }
    }
}

// set the distance/rev of Y axis 
void ui_onEdit_y_rev(int argc, ...){
    float f;
    va_list ap;
    char*p;
    if(argc == 1){
        va_start(ap, argc);
        p = va_arg(ap, char*);
        va_end(ap);
        if(sscanf(p, "\"%f\"", &f)==1){
            perRev_y = f*100; 
            ui.Set(&ui.rsrc, PG_MAIN, MAIN_Y_REV, UI_ATTR_TXT, "%d.%d", perRev_y/100, perRev_y%100);
            uiInstanceSaveConf();
        }
    }
}

void ui_onClick_psX_rst(int argc, ...){
    log("<%s >", __func__);
    s32 pos;
    mPlayer.stop(&mPlayer.rsrc);
    stprRamp[1].gohome(&stprRamp[1].rsrc, 64500);
    while(stprRamp[1].rsrc.posCur){
        thread_delay(50);
    }
    log("</%s >", __func__);
}

static void ui_onClick_psX_act(s32 dist){
    log("<%s dist:%d >", __func__, dist);
    s32 pos;
    ui_onClick_psX_rst(1, NULL);
    pos = distanceToMStep(dist);
    stprRamp[1].rsrc.spdMax = 64500;
    stprRamp[1].moveTo(&stprRamp[1].rsrc, pos);
    while(pos != stprRamp[1].rsrc.posCur){
        thread_delay(50);
    }
    log("</%s >", __func__);
}

static void ui_onClick_psX(u32 angle){
    log("<%s angle:%d >", __func__, angle);
    s32 pos;
    ui_onClick_psX_rst(1, NULL);
    pos = angleToMStep(angle);
    stprRamp[0].rsrc.spdMax = 65000;
    stprRamp[0].moveTo(&stprRamp[0].rsrc, pos);
    while(pos != stprRamp[0].rsrc.posCur){
        thread_delay(50);
    }
    log("</%s >", __func__);
}



void ui_onClick_ps0(int argc, ...){    ui_onClick_psX(posR[0]); }
void ui_onClick_ps1(int argc, ...){    ui_onClick_psX(posR[1]); }
void ui_onClick_ps2(int argc, ...){    ui_onClick_psX(posR[2]); }
void ui_onClick_ps3(int argc, ...){    ui_onClick_psX(posR[3]); }
void ui_onClick_ps4(int argc, ...){    ui_onClick_psX(posR[4]); }

void ui_onClick_ps0_act(int argc, ...){ ui_onClick_psX_act(posY[0]);    }
void ui_onClick_ps1_act(int argc, ...){ ui_onClick_psX_act(posY[1]);    }
void ui_onClick_ps2_act(int argc, ...){ ui_onClick_psX_act(posY[2]);    }
void ui_onClick_ps3_act(int argc, ...){ ui_onClick_psX_act(posY[3]);    }
void ui_onClick_ps4_act(int argc, ...){ ui_onClick_psX_act(posY[4]);    }





void ui_onClick_main_bAuto(int argc, ...){
}
