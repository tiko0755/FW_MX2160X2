#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "ui_def_instance.h"
#include "ui_cb_instance.h"
#include "ui_machine.h"
#include "user_log.h"

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

// set the position #0
void ui_onEdit_ps0(int argc, ...){
    float f;
    va_list ap;
    char*p;
    if(argc == 1){
        va_start(ap, argc);
        p = va_arg(ap, char*);
        va_end(ap);
        if(sscanf(p, "\"%f\"", &f)==1){
            posR[0] = f*100; 
            ui.Set(&ui.rsrc, PG_MAIN, MAIN_PS0, UI_ATTR_TXT, "%d.%d", posR[0]/100, posR[0]%100);
            uiInstanceSaveConf();
        }
    }
}

// set the position #0
void ui_onEdit_ps0_act(int argc, ...){
    float f;
    va_list ap;
    char*p;
    if(argc == 1){
        va_start(ap, argc);
        p = va_arg(ap, char*);
        va_end(ap);
        if(sscanf(p, "\"%f\"", &f)==1){
            posY[0] = f*100; 
            ui.Set(&ui.rsrc, PG_MAIN, MAIN_PS0_ACT, UI_ATTR_TXT, "%d.%d", posY[0]/100, posY[0]%100);
            uiInstanceSaveConf();
        }
    }
}

void ui_onEdit_ps1(int argc, ...){
    float f;
    va_list ap;
    char*p;
    if(argc == 1){
        va_start(ap, argc);
        p = va_arg(ap, char*);
        va_end(ap);
        if(sscanf(p, "\"%f\"", &f)==1){
            posR[1] = f*100; 
            ui.Set(&ui.rsrc, PG_MAIN, MAIN_PS1, UI_ATTR_TXT, "%d.%d", posR[1]/100, posR[1]%100);
            uiInstanceSaveConf();
        }
    }
}
void ui_onEdit_ps1_act(int argc, ...){
    float f;
    va_list ap;
    char*p;
    if(argc == 1){
        va_start(ap, argc);
        p = va_arg(ap, char*);
        va_end(ap);
        if(sscanf(p, "\"%f\"", &f)==1){
            posY[1] = f*100; 
            ui.Set(&ui.rsrc, PG_MAIN, MAIN_PS1_ACT, UI_ATTR_TXT, "%d.%d", posY[1]/100, posY[1]%100);
            uiInstanceSaveConf();
        }
    }
}

void ui_onEdit_ps2(int argc, ...){
    float f;
    va_list ap;
    char*p;
    if(argc == 1){
        va_start(ap, argc);
        p = va_arg(ap, char*);
        va_end(ap);
        if(sscanf(p, "\"%f\"", &f)==1){
            posR[2] = f*100; 
            ui.Set(&ui.rsrc, PG_MAIN, MAIN_PS2, UI_ATTR_TXT, "%d.%d", posR[2]/100, posR[2]%100);
            uiInstanceSaveConf();
        }
    }
}
void ui_onEdit_ps2_act(int argc, ...){
    float f;
    va_list ap;
    char*p;
    if(argc == 1){
        va_start(ap, argc);
        p = va_arg(ap, char*);
        va_end(ap);
        if(sscanf(p, "\"%f\"", &f)==1){
            posY[2] = f*100; 
            ui.Set(&ui.rsrc, PG_MAIN, MAIN_PS2_ACT, UI_ATTR_TXT, "%d.%d", posY[2]/100, posY[2]%100);
            uiInstanceSaveConf();
        }
    }
}

void ui_onEdit_ps3(int argc, ...){
    float f;
    va_list ap;
    char*p;
    if(argc == 1){
        va_start(ap, argc);
        p = va_arg(ap, char*);
        va_end(ap);
        if(sscanf(p, "\"%f\"", &f)==1){
            posR[3] = f*100; 
            ui.Set(&ui.rsrc, PG_MAIN, MAIN_PS3, UI_ATTR_TXT, "%d.%d", posR[3]/100, posR[3]%100);
            uiInstanceSaveConf();
        }
    }
}
void ui_onEdit_ps3_act(int argc, ...){
    float f;
    va_list ap;
    char*p;
    if(argc == 1){
        va_start(ap, argc);
        p = va_arg(ap, char*);
        va_end(ap);
        if(sscanf(p, "\"%f\"", &f)==1){
            posY[3] = f*100; 
            ui.Set(&ui.rsrc, PG_MAIN, MAIN_PS3_ACT, UI_ATTR_TXT, "%d.%d", posY[3]/100, posY[3]%100);
            uiInstanceSaveConf();
        }
    }
}

void ui_onEdit_ps4(int argc, ...){
    float f;
    va_list ap;
    char*p;
    if(argc == 1){
        va_start(ap, argc);
        p = va_arg(ap, char*);
        va_end(ap);
        if(sscanf(p, "\"%f\"", &f)==1){
            posR[4] = f*100; 
            ui.Set(&ui.rsrc, PG_MAIN, MAIN_PS4, UI_ATTR_TXT, "%d.%d", posR[4]/100, posR[4]%100);
            uiInstanceSaveConf();
        }
    }
}
void ui_onEdit_ps4_act(int argc, ...){
    float f;
    va_list ap;
    char*p;
    if(argc == 1){
        va_start(ap, argc);
        p = va_arg(ap, char*);
        va_end(ap);
        if(sscanf(p, "\"%f\"", &f)==1){
            posY[4] = f*100; 
            ui.Set(&ui.rsrc, PG_MAIN, MAIN_PS4_ACT, UI_ATTR_TXT, "%d.%d", posY[4]/100, posY[4]%100);
            uiInstanceSaveConf();
        }
    }
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

void ui_onClick_main_bPos1(int argc, ...){
    
}

void ui_onClick_main_bPos2(int argc, ...){

}

void ui_onClick_main_bPos3(int argc, ...){

}

void ui_onClick_main_bPos4(int argc, ...){

}

void ui_onClick_main_bRst1(int argc, ...){

}

void ui_onClick_main_bRst2(int argc, ...){

}

void ui_onClick_main_bRst3(int argc, ...){
    
}

void ui_onClick_main_bRst4(int argc, ...){

}

void ui_onClick_main_bAct1(int argc, ...){
    
}

void ui_onClick_main_bAct2(int argc, ...){

}

void ui_onClick_main_bAct3(int argc, ...){

}

void ui_onClick_main_bAct4(int argc, ...){

}

void ui_onClick_main_bAuto(int argc, ...){

}


