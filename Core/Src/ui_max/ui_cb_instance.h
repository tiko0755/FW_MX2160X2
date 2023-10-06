#ifndef _UI_CB_INSTANCE_H_
#define _UI_CB_INSTANCE_H_

#include "misc.h"
#include <stdarg.h>

void ui_onEdit_thickness(int argc, ...);
void ui_onEdit_r_rev(int argc, ...);
void ui_onEdit_y_rev(int argc, ...);

void ui_onEdit_ps0(int argc, ...);
void ui_onEdit_ps0_act(int argc, ...);

void ui_onEdit_ps1(int argc, ...);
void ui_onEdit_ps1_act(int argc, ...);

void ui_onEdit_ps2(int argc, ...);
void ui_onEdit_ps2_act(int argc, ...);

void ui_onEdit_ps3(int argc, ...);
void ui_onEdit_ps3_act(int argc, ...);

void ui_onEdit_ps4(int argc, ...);
void ui_onEdit_ps4_act(int argc, ...);


void ui_onClick_ps0(int argc, ...);
void ui_onClick_ps1(int argc, ...);
void ui_onClick_ps2(int argc, ...);
void ui_onClick_ps3(int argc, ...);
void ui_onClick_ps4(int argc, ...);

void ui_onClick_ps0_act(int argc, ...);
void ui_onClick_ps1_act(int argc, ...);
void ui_onClick_ps2_act(int argc, ...);
void ui_onClick_ps3_act(int argc, ...);
void ui_onClick_ps4_act(int argc, ...);

void ui_onClick_psX_rst(int argc, ...);




//void ui_onChanged_main_tbUnitY(int argc, ...);

//void ui_onChanged_main_tbSpdR(int argc, ...);
//void ui_onChanged_main_tbSpdY(int argc, ...);


void ui_onClick_main_bAuto(int argc, ...);
void ui_onClick_emergence(int argc, ...);
void ui_onClick_homing(int argc, ...);
void ui_onClick_clamp(int argc, ...);
//void ui_onClick_auto(int argc, ...);

void ui_onEdit_repo0(int argc, ...);
void ui_onEdit_repo1(int argc, ...);
void ui_onEdit_repo2(int argc, ...);
void ui_onEdit_repo3(int argc, ...);
void ui_onEdit_repo4(int argc, ...);



#endif
