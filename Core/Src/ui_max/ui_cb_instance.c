#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "ui_def_instance.h"
#include "ui_cb_instance.h"
#include "ui_machine.h"
#include "user_log.h"

void ui_onChanged_main_tbUnitR(int argc, ...){
	s32 x;
    float f;
	va_list ap;
	char*p;
	if(argc == 1){
		va_start(ap, argc);
		p = va_arg(ap, char*);
		va_end(ap);
		f = atof(p);
        mmPerRev_r = f*1000;
    }
}

void ui_onChanged_main_tbUnitY(int argc, ...){
	s32 x;
    float f;
	va_list ap;
	char*p;
	if(argc == 1){
		va_start(ap, argc);
		p = va_arg(ap, char*);
		va_end(ap);
		f = atof(p);
        mmPerRev_y = f*1000;
    }
}

void ui_onChanged_main_tbSpdR(int argc, ...){
	s32 x;
    float f;
	va_list ap;
	char*p;
	if(argc == 1){
		va_start(ap, argc);
		p = va_arg(ap, char*);
		va_end(ap);
		spd_r = atoi(p);
        log("<%s spd_r:%d >", __func__, spd_r);
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


