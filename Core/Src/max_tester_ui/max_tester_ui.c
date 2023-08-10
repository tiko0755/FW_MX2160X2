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


/**********************************************************
 Private variable
**********************************************************/


//static uiPage_t pg00;
//static uiPage_t pg10;
//static uiPage_t pg20;
//static uiPage_t pgxx;


uiDev_T ui;

static void print_lcd(const char* FORMAT_ORG, ...);



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
		uiSetup(&ui, uartDev, print_lcd, tObj);

		// new pages
		ui.NewPage(&ui.rsrc, "pg00");
		ui.NewPage(&ui.rsrc, "pg10");
		ui.NewPage(&ui.rsrc, "pg20");
		ui.NewPage(&ui.rsrc, "pgxx");
	
		ui.PlaceTxtBx(&ui.rsrc, "pgxx", "t_about");
		ui.PlaceTxtBx(&ui.rsrc, "pg00", "t_title");
	
//		textbox_t* txb = ui.GetTxtBx(&ui.rsrc, "pg00", "t_title");
//		txb->set(&txb->rsrc, "txt", "hello, Tiko");
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

