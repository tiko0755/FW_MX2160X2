/**********************************************************
filename: max_tester_ui.h
**********************************************************/
#ifndef _MAX_TESTER_UI_H
#define _MAX_TESTER_UI_H

#include "misc.h"
#include "uartdev.h"
#include "app_timer.h"
#include "ui.h"

/*****************************************************************************
 @ typedefs
****************************************************************************/
#pragma pack(push,4)        // push current align bytes, and then set 4 bytes align

#pragma pack(pop)           //recover align bytes from 4 bytes

extern uiDev_T ui;

void  max_tester_ui_initial(
    UartDev_t* uartDev,
		appTmrDev_t* tObj
);

#endif

