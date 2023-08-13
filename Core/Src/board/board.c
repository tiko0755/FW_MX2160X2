/******************** (C) COPYRIGHT 2015 INCUBECN *****************************
* File Name          : board.c
* Author             : Tiko Zhong
* Date First Issued  : 11/18/2021
* Description        : 
*                      
********************************************************************************
* History:
* Apr22,2021: V0.2
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include "board.h"
#include "main.h"
#include "at24cxx.h"
#include "app_timer.h"
#include "gpioDecal.h"

#include "led_flash.h"
#include "outputCmd.h"
#include "inputCmd.h"
#include "rampCmd.h"
#include "tmc2160a_cmd.h"

#include "max_tester_ui.h"

#define NOUSED_PIN_INDX 255

/* import handle from main.c variables ---------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
const char ABOUT[] = {"MB2160X2-1.0.0"};
const char COMMON_HELP[] = {
    "Commands:"
    "\n help()"
    "\n about()"
    "\n restart()"
    "\n reg.write(addr,val)"
    "\n reg.read(addr)"
    "\n baud.set(bHost,bBus)"
    "\n baud.get()"
    "\n ipconf.setip(ip0,..,ip3)"
    "\n   |-ipconf.setip(12.34.56.78:port)"
    "\n ipconf.setmask(msk0,..,msk3)"
    "\n ipconf.setgw(gw0,..,gw3)"
    "\n ipconf.info()"
    "\n"
};
char g_addrPre[4] = {0};    //addr precode
u8 g_initalDone = 0;
u8 g_baudHost = 4;    //BAUD[4]=115200
u8 g_baud485 = 4;
u32 g_errorCode;// = 0;
/**********************************************
*  PINs Define
**********************************************/
const PIN_T RUNNING = {SWD_LED_GPIO_Port, SWD_LED_Pin};

/**********************************************
*  static Devices
**********************************************/
// =============================================
#define RX_POOL_LEN    (MAX_CMD_LEN)
#define TX_POOL_LEN    (MAX_CMD_LEN)
#define    RX_BUF_LEN    (128)
// uart device
static u8 uartRxPool[RX_POOL_LEN] = {0};
static u8 uartRxBuf[2*RX_BUF_LEN] = {0};
static u8 uartTxPool[TX_POOL_LEN] = {0};
UartDev_t console;

#define UI_RX_POOL_LEN 	(256)
#define UI_TX_POOL_LEN 	(128)
#define UI_RX_BUFF_LEN 	(64)

static u8 uiRxPool[UI_RX_POOL_LEN] = {0};
static u8 uiRxBuf[2*UI_RX_BUFF_LEN] = {0};
static u8 uiTxPool[UI_TX_POOL_LEN] = {0};
static UartDev_t uiUartDev;

//// rs485 device
//const PIN_T DE = {DE_GPIO_Port, DE_Pin};
//const PIN_T DET = {DET_GPIO_Port, DET_Pin};
//static u8 rs485RxPool[RX_POOL_LEN] = {0};
//static u8 rs485RxBuf[2*RX_BUF_LEN] = {0};
//static u8 rs485TxPool[TX_POOL_LEN] = {0};
//static s8 rs485AfterSend_1(UART_HandleTypeDef *huart);
//static s8 rs485BeforeSend_1(void);
//Rs485Dev_t rs485;

// rs485 device


// app timer 
appTmrDev_t tmr[APP_TIMER_COUNT] = {0};

// commander comsumer
cmdConsumerDev_t cmdConsumer;

OUTPUT_DEV_T g_output;
const PIN_T OUTPUT_PIN[] = {
    {OUT0_GPIO_Port, OUT0_Pin},
    {OUT1_GPIO_Port, OUT1_Pin},
    {OUT2_GPIO_Port, OUT2_Pin},
    {OUT3_GPIO_Port, OUT3_Pin},
};

INPUT_DEV_T g_input;
const PIN_T INPUT_PIN[] = {
    {IN0_GPIO_Port, IN0_Pin},
    {IN1_GPIO_Port, IN1_Pin},
    {IN2_GPIO_Port, IN2_Pin},
    {IN3_GPIO_Port, IN3_Pin},
    {IN4_GPIO_Port, IN4_Pin}
};


// stepper
const PIN_T M0_DIR = {M0_DIR_GPIO_Port, M0_DIR_Pin};
const PIN_T M0_REFL = {REFL0_EXT15_GPIO_Port, REFL0_EXT15_Pin};
const PIN_T M0_REFR = {REFR0_EXT0_GPIO_Port, REFR0_EXT0_Pin};

const PIN_T M1_DIR = {M1_DIR_GPIO_Port, M1_DIR_Pin};
const PIN_T M1_REFL = {REFL1_EXT7_GPIO_Port, REFL1_EXT7_Pin};
const PIN_T M1_REFR = {REFR1_EXT12_GPIO_Port, REFR1_EXT12_Pin};

rampDev_t stprRamp[2];

const PIN_T DRV0_CS = {SPI_CS0_GPIO_Port, SPI_CS0_Pin};
const PIN_T DRV0_EN = {M0_EN_GPIO_Port, M0_EN_Pin};
const PIN_T DRV0_DIA0 = {M0_DIA0_GPIO_Port, M0_DIA0_Pin};
const PIN_T DRV0_DIA1 = {M0_DIA1_GPIO_Port, M0_DIA1_Pin};

const PIN_T DRV1_CS = {SPI_CS1_GPIO_Port, SPI_CS1_Pin};
const PIN_T DRV1_EN = {M1_EN_GPIO_Port, M1_EN_Pin};
const PIN_T DRV1_DIA0 = {M1_DIA0_GPIO_Port, M1_DIA0_Pin};
const PIN_T DRV1_DIA1 = {M1_DIA1_GPIO_Port, M1_DIA1_Pin};

TMC2160A_dev_t stprDrv[2];

// =============================================
AT24CXX_Dev_T erom;
const PIN_T SCL = {SCL_GPIO_Port, SCL_Pin};
const PIN_T SDA = {SDA_GPIO_Port, SDA_Pin};
// define app eeprom size
#define EEPROM_SIZE_USR            (6*1024)
#define EEPROM_SIZE_REG            (1*1024)
#define EEPROM_SIZE_NET            (1*1024)
// define app eeprom base address
#define EEPROM_BASE_USER        0
#define EEPROM_BASE_REG            (EEPROM_BASE_USER + EEPROM_SIZE_USR)
#define EEPROM_BASE_NET            (EEPROM_BASE_REG + EEPROM_SIZE_NET)

static s8 configWrite(void);
static s8 configRead(void);
static u8 brdCmdU8(void* d, u8* CMDu8, u8 len, void (*xprint)(const char* FORMAT_ORG, ...));
static void forwardToBus(u8* BUFF, u16 len);

/* Private function prototypes -----------------------------------------------*/
// after GPIO initial, excute this function to enable
void boardPreInit(void){
    AT24CXX_Setup(&erom, SCL, SDA, AT24C64, 0X00);
    configRead();
}

void boardInit(void){
		u8 trmIdx = 0;
    // setup app timers
    for(trmIdx=0;trmIdx<APP_TIMER_COUNT;trmIdx++){
        setup_appTmr(&tmr[trmIdx]);
    }
		trmIdx = 0;
    
    //read board addr
    setupUartDev(&console, &huart1, &tmr[trmIdx++], uartTxPool, TX_POOL_LEN, uartRxPool, RX_POOL_LEN, uartRxBuf, RX_BUF_LEN, 4);
    memset(g_addrPre,0,4);
    strFormat(g_addrPre, 4, "%d.", g_boardAddr);
    print("%sabout(\"%s\")\r\n", g_addrPre, ABOUT);

    printS("setup lcd...");
    setupUartDev(&uiUartDev, &huart2, &tmr[trmIdx++], uiTxPool, UI_TX_POOL_LEN, uiRxPool, UI_RX_POOL_LEN, uiRxBuf, UI_RX_BUFF_LEN, 8);
    max_tester_ui_initial(&uiUartDev, &tmr[trmIdx++]);
    printS("ok\r\n");
		
    // application initial
    printS("setup led_flash...");
    led_flash_init(&tmr[trmIdx++], &RUNNING, 100);     // now, it can run itself
    printS("ok\r\n");
   
    printS("setup gpio driver...");
    outputDevSetup(&g_output, OUTPUT_PIN, 4, 0x00000000);
    InputDevSetup(&g_input, OUTPUT_PIN, 5);
    printS("ok\r\n");
    
    TMC2160A_dev_Setup(
        &stprDrv[0], 
        "drv0",
        &DRV0_CS,
        &DRV0_EN,
        &DRV0_DIA0,
        &DRV0_DIA1,
        &hspi1, 
        0,
        EEPROM_BASE_USER,
        NULL,   //s8 (*ioWrite)(u16 addr, const u8 *pDat, u16 nBytes),
        NULL    //s8 (*ioRead)(u16 addr, u8 *pDat, u16 nBytes)
    );
    
    TMC2160A_dev_Setup(
        &stprDrv[1], 
        "drv1",
        &DRV1_CS,
        &DRV1_EN,
        &DRV1_DIA0,
        &DRV1_DIA1,
        &hspi1, 
        1,
        EEPROM_BASE_USER,
        NULL,   //s8 (*ioWrite)(u16 addr, const u8 *pDat, u16 nBytes),
        NULL    //s8 (*ioRead)(u16 addr, u8 *pDat, u16 nBytes)
    );

    printS("setup ramp...");
    // setup ramp
    rampSetup(
        &stprRamp[0],
        "m1",
        &htim3,
        TIM_CHANNEL_1,
        &M0_DIR,
        &M0_REFL,
        &M0_REFR,
        64
    );
    
    HAL_GPIO_WritePin(M0_DIR.GPIOx, M0_DIR.GPIO_Pin, GPIO_PIN_SET);
    
    // setup ramp
    rampSetup(
        &stprRamp[1],
        "m2",
        &htim1,
        TIM_CHANNEL_4,
        &M1_DIR,
        &M1_REFL,
        &M1_REFR,
        64
    );
    printS("ok\r\n");
    
    printS("setup cmdConsumer...");
    setup_cmdConsumer(&cmdConsumer, 
        &console.rsrc.rxRB,     // command line in a ringbuffer
        fetchLineFromRingBufferU8, // fetchLine method  
        print,                  // print out 
        forwardToBus,
        &tmr[trmIdx++],
        10                     // unit in ms, polling rb each interval
    );
    cmdConsumer.append(&cmdConsumer.rsrc, NULL, brdCmdU8);
    cmdConsumer.append(&cmdConsumer.rsrc, &stprRamp[0], rampCmdU8);
    cmdConsumer.append(&cmdConsumer.rsrc, &stprRamp[1], rampCmdU8);
    cmdConsumer.append(&cmdConsumer.rsrc, &g_output, outputCmdU8);
    cmdConsumer.append(&cmdConsumer.rsrc, &stprDrv[0], tmc2160aCmdU8);
    cmdConsumer.append(&cmdConsumer.rsrc, &stprDrv[1], tmc2160aCmdU8);
    printS("ok\r\n");
    
    // get ready, start to work
    console.StartRcv(&console.rsrc);
		
//    rs485.rsrc.uartdev.StartRcv(&rs485.rsrc.uartdev.rsrc);
//    HAL_GPIO_WritePin(rs485.rsrc.DE.GPIOx, rs485.rsrc.DE.GPIO_Pin, GPIO_PIN_RESET);

    g_initalDone = 1;
    print("%d timers have been used\r\n", trmIdx);
    printS("initial complete, type \"help\" for help\n");      
}

void printS(const char* STRING){
    console.Send(&console.rsrc, (const u8*)STRING, strlen(STRING));
}

void print(const char* FORMAT_ORG, ...){
    va_list ap;
    char buf[MAX_CMD_LEN] = {0};
    s16 bytes;
    //take string
    va_start(ap, FORMAT_ORG);
    bytes = vsnprintf(buf, MAX_CMD_LEN, FORMAT_ORG, ap);
    va_end(ap);
    //send out
    if(bytes>0)    console.Send(&console.rsrc, (u8*)buf, bytes);
}

void printS485(const char* STRING){
//    rs485.Send(&rs485.rsrc, (const u8*)STRING, strlen(STRING));
}

void print485(const char* FORMAT_ORG, ...){
//    va_list ap;
//    char buf[MAX_CMD_LEN] = {0};
//    s16 bytes;
//    //take string
//    va_start(ap, FORMAT_ORG);
//    bytes = vsnprintf(buf, MAX_CMD_LEN, FORMAT_ORG, ap);
//    va_end(ap);
//    //send out
//    if(bytes>0)    rs485.Send(&rs485.rsrc, (u8*)buf, bytes);
}

void printSUDP(const char* STRING){
//    handler_udp->send(&handler_udp->rsrc, (u8*)STRING, strlen(STRING));
}

void printUDP(const char* FORMAT_ORG, ...){
//    va_list ap;
//    char buf[MAX_CMD_LEN] = {0};
//    s16 bytes;
//    //take string
//    va_start(ap, FORMAT_ORG);
//    bytes = vsnprintf(buf, MAX_CMD_LEN, FORMAT_ORG, ap);
//    va_end(ap);
//    //send out
//    if(bytes>0)    handler_udp->send(&handler_udp->rsrc, (u8*)buf, bytes);
}

//s8 ioWrite(u16 addr, const u8 *pDat, u16 nBytes){
//    erom.Write(&erom.rsrc, EEPROM_BASE_USER + addr, pDat, nBytes);
//    return 0;
//}
//
//s8 ioRead(u16 addr, u8 *pDat, u16 nBytes){
//    erom.Read(&erom.rsrc, EEPROM_BASE_USER+addr, pDat, nBytes);
//  return 0;
//}


static void forwardToBus(u8* BUFF, u16 len){
    print("<%s BUFF:%s >", __func__, (char*)BUFF);
}

s8 ioReadReg(u16 addr, s32 *val){
    return(erom.Read(&erom.rsrc, EEPROM_BASE_REG+addr*4, (u8*)val, 4));
}

s8 ioWriteReg(u16 addr, s32 val){
    return(erom.Write(&erom.rsrc, EEPROM_BASE_REG+addr*4, (u8*)&val, 4));
}

static s8 configWrite(void){
    u8 buff[32]={0};
    buff[14] = g_baudHost;
    buff[15] = g_baud485;
    buff[16] = HAL_GetTick()&0xff;            // mac[3]
    buff[17] = (HAL_GetTick()>>8)&0xff;        // mac[4]
    buff[18] = (HAL_GetTick()>>16)&0xff;    // mac[5]
    buff[31] = 0xaa;
    erom.Write(&erom.rsrc, EEPROM_BASE_NET, buff, 32);
    return 0;
}

static s8 configRead(void){
    u8 buff[32] = {0};
    erom.Read(&erom.rsrc, EEPROM_BASE_NET, buff, 32);
    if(buff[31] == 0xaa){
        g_baudHost = buff[14];
        g_baud485 = buff[15];

        if(g_baudHost >= 7)    g_baudHost = 4;    // 4@115200
        if(g_baud485 >= 7)     g_baud485 = 4;    // 4@115200
    }
    return 0;
}

static u8 brdCmdU8(void* d, u8* CMDu8, u8 len, void (*xprint)(const char* FORMAT_ORG, ...)){
    return(brdCmd((const char*)CMDu8, xprint));
}

u8 brdCmd(const char* CMD, void (*xprint)(const char* FORMAT_ORG, ...)){
    s32 i,j,k;
    u8 brdAddr = g_boardAddr;
    // common
    if(strncmp(CMD, "about", strlen("about")) == 0){
        xprint("+ok@%d.about(\"%s\")\r\n", brdAddr, ABOUT);
        led_flash_answer(50, 30);
        return 1;
    }
    else if(strncmp(CMD, "help", strlen("help")) == 0){
        xprint("%s +ok@%d.help()\r\n", COMMON_HELP, brdAddr);
        return 1;
    }
    else if(strncmp(CMD, "restart ", strlen("restart ")) == 0){
        HAL_NVIC_SystemReset();
        return 1;
    }

    else if(sscanf(CMD, "reg.write %d %d ", &i, &j)==2){
        if(i>=EEPROM_SIZE_REG/4)    {
            xprint("+err@%d.reg.write(\"address[0..%d]\")\r\n", brdAddr, EEPROM_SIZE_REG/4);
            return 1;
        }
        if(ioWriteReg(i,j) == 0)    xprint("+ok@%d.reg.write(%d,%d)\r\n", brdAddr, i, j);
        else xprint("+err@%d.reg.write(%d,%d)\r\n", brdAddr, i, j);
        return 1;
    }
    else if(sscanf(CMD, "reg.read %d ", &i)==1){
        if(i>=EEPROM_SIZE_REG/4){
            xprint("+err@%d.reg.read(\"address[0..%d]\")\r\n", brdAddr, EEPROM_SIZE_REG/4);
            return 1;
        }
        ioReadReg(i,&j);
        xprint("+ok@%d.reg.read(%d,%d)\r\n", brdAddr, i, j);
//        if(ioReadReg(i,&j) == 0)
//            xprint("+ok@%d.reg.read(%d,%d)\r\n", brdAddr, i, j);
//        else xprint("+err@%d.reg.read(%d,%d)\r\n", brdAddr, i, j);
        return 1;
    }

    else if(sscanf(CMD, "baud.set %d %d", &i,&j)==2){
        for(k=0;k<7;k++){
            g_baudHost = k;
            if(i==BAUD[g_baudHost])    break;
        }
        for(k=0;k<7;k++){
            g_baud485 = k;
            if(j==BAUD[g_baud485])    break;
        }
        configWrite();
        xprint("+ok@%d.baud.set(%d,%d)\r\n", brdAddr, BAUD[g_baudHost], BAUD[g_baud485]);
        return 1;
    }
    else if(strncmp(CMD, "baud.get ", strlen("baud.get "))==0){
        configRead();
        xprint("+ok@%d.baud.get(%d,%d)\r\n", brdAddr, BAUD[g_baudHost], BAUD[g_baud485]);
        return 1;
    }

    return 0;
}

//static s8 rs485BeforeSend_1(void){
//    if(g_initalDone == 0)    return 0;
//    if(HAL_GPIO_ReadPin(rs485.rsrc.DET.GPIOx, rs485.rsrc.DET.GPIO_Pin)==GPIO_PIN_SET){
//        return -1;
//    }
//    HAL_GPIO_WritePin(rs485.rsrc.DE.GPIOx, rs485.rsrc.DE.GPIO_Pin, GPIO_PIN_SET);
//    while(1){
//        if(HAL_GPIO_ReadPin(rs485.rsrc.DET.GPIOx, rs485.rsrc.DET.GPIO_Pin)==GPIO_PIN_SET){
//            break;
//        }
//    }
//    return 0;
//}

//static s8 rs485AfterSend_1(UART_HandleTypeDef *huart){
//    if(g_initalDone == 0)    return 0;
//    if(huart->Instance == rs485.rsrc.uartdev.rsrc.huart->Instance){
//        HAL_GPIO_WritePin(rs485.rsrc.DE.GPIOx, rs485.rsrc.DE.GPIO_Pin, GPIO_PIN_RESET);
//        rs485.rsrc.uartdev.rsrc.flag |= BIT(0);
////        while(1){
////            if(HAL_GPIO_ReadPin(rs485.rsrc.DET.GPIOx, rs485.rsrc.DET.GPIO_Pin)==GPIO_PIN_RESET){
////                break;
////            }
////        }
//    }
//    return 0;
//}

void tmc2160_readWriteArray(uint8_t channel, uint8_t *data, size_t len){
    stprDrv[channel].readWriteArray(&stprDrv[channel].rsrc, data,len);
}

/**
  * @brief EXTI line detection callbacks
  * @param GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
    
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
    if(g_initalDone == 0){  return; }
//	if(htim->Instance == htim1.Instance){
//        stprRamp[1].isr(&stprRamp[1].rsrc, &htim1);
//	}
//    else if(htim->Instance == htim3.Instance){
//        stprRamp[0].isr(&stprRamp[0].rsrc, &htim3);
//    }
}



/**
  * @brief  Conversion complete callback in non blocking mode
  * @param  AdcHandle : ADC handle
  * @note   This example shows a simple way to report end of conversion
  *         and get conversion result. You can add your own implementation.
  * @retval None
  */
// void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *AdcHandle){}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
    if(g_initalDone==0)    return;
//    rs485.rsrc.uartdev.rsrc.afterSend(huart);
    if(huart->Instance == console.rsrc.huart->Instance){
        console.rsrc.flag |= BIT(0);
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
}

/**
  * @brief  TxRx Transfer completed callback.
  * @param  hspi: SPI handle
  * @note   This example shows a simple way to report end of DMA TxRx transfer, and
  *         you can add your own implementation.
  * @retval None
  */
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi){
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi){
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi){
}

/**
  * @brief  SPI error callbacks.
  * @param  hspi: SPI handle
  * @note   This example shows a simple way to report transfer error, and you can
  *         add your own implementation.
  * @retval None
  */
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi){
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
