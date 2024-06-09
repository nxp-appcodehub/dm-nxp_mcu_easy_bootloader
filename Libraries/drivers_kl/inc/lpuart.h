/*
* Copyright 2023 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef __CH_LIB_LPUART_H__
#define __CH_LIB_LPUART_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
     
#define     LPUART0_RX_E21_TX_E20   (0x0000A920U)
#define     LPUART0_RX_B16_TX_B17   (0x0000A0C8U)
#define     LPUART0_RX_A01_TX_A02   (0x00008280U)
#define     LPUART1_RX_A18_TX_A19   (0x0000A4C1U)
#define     LPUART1_RX_C03_TX_C04   (0x000086D1U)
#define     LPUART0_RX_D06_TX_D07   (0x00008CD8U)

/*!< LPUART instance */
#define HW_LPUART0  (0x00U)
#define HW_LPUART1  (0x01U)
     
/*!< parity  */
typedef enum
{
    kLPUART_ParityDisabled = 0x0,
    kLPUART_ParityEven     = 0x2,
    kLPUART_ParityOdd      = 0x3,
} LPUART_ParityMode_t;

/*!< bit count */
typedef enum 
{
    kLPUART_8BitsPerChar  = 0,
    kLPUART_9BitsPerChar  = 1,
} LPUART_BitPerChar_t;


typedef void (*LPUART_CallBackTxType)(void* pram);

/*!< Interrupts and DMA */
typedef enum
{
    kUART_IT_Tx,                //  
    kUART_DMA_Tx,               // DMA 
    kUART_IT_Rx,                //  
    kUART_DMA_Rx,               // DMA 
}LPUART_ITDMAConfig_Type;


typedef struct
{
    uint32_t                srcClock;
    uint8_t                 instance;
    uint32_t                baudrate;
    LPUART_ParityMode_t     parityMode;
    LPUART_BitPerChar_t     bitPerChar;
}LPUART_InitTypeDef;
     
/*!< API functions */
uint32_t LPUART_QuickInit(uint32_t MAP, uint32_t baudrate);
void LPUART_Init(LPUART_InitTypeDef * UART_InitStruct);
void LPUART_WriteByte(uint32_t instance, char ch);
uint8_t LPUART_ReadByte(uint32_t instance, uint8_t *ch);
void LPUART_ITDMAConfig(uint32_t instance, LPUART_ITDMAConfig_Type config, bool status);
//void LPUART_SendByte()
//uint32_t LPUART_GetByte()
//void LPUART_AdvConfig
//void LPUART_IntConfig

#ifdef __cplusplus
}
#endif

#endif

