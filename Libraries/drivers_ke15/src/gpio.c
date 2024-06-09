/*
 * Copyright 2018-2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "common.h"
#include "gpio.h"


#if defined(GPIO_BASE_PTRS)
#define GPIO_BASES  GPIO_BASE_PTRS
#endif

#if defined(PORT_BASE_PTRS)
#define PORT_BASES  PORT_BASE_PTRS
#endif

#ifndef GPIO_BASES
    #if defined(PTA)
    #define GPIO_BASES {PTA, PTB, PTC, PTD, PTE};
    #elif defined(GPIOA)
    #define GPIO_BASES {GPIOA, GPIOB, GPIOC, GPIOD, GPIOE};
    #endif
#endif

#ifndef PORT_BASES
#define PORT_BASES {PORTA, PORTB, PORTC, PORTD, PORTE};
#endif

static GPIO_Type * const GPIOBases[] = GPIO_BASES;
static PORT_Type * const PORTBases[] = PORT_BASES;
#if defined(MKL27Z4)
static const IRQn_Type GPIO_IrqTbl[] = 
{
    (IRQn_Type)PORTA_IRQn,
    (IRQn_Type)0,
    (IRQn_Type)PORTCD_IRQn,
    (IRQn_Type)PORTCD_IRQn,
};
#else
//static const IRQn_Type GPIO_IrqTbl[] = 
//{
//    (IRQn_Type)(PORTA_IRQn + 0),
//    (IRQn_Type)(PORTA_IRQn + 1),
//    (IRQn_Type)(PORTA_IRQn + 2),
//    (IRQn_Type)(PORTA_IRQn + 3),
//    (IRQn_Type)(PORTA_IRQn + 4),
//};
#endif

/**
 * @brief  
 * @note   
 * @param  instance:
 *         @arg HW_GPIOx : GPIOA-E
 * @param  pin : 0-31
 * @param  dir : 
 *         @arg 1 : 
 *         @arg 0 : 
 * @retval None
 */
void GPIO_SetPinDir(uint32_t instance, uint32_t pin, uint32_t dir)
{
    (dir == 1) ? (GPIOBases[instance]->PDDR |= (1 << pin)):
    (GPIOBases[instance]->PDDR &= ~(1 << pin));
}

 /**
 * @brief  
 * @note   
 * @param  instance:
 *         @arg HW_GPIOx : GPIOA-E
 * @param  pin : 0-31
 * @param  mode : 
 *         @arg kGPIO_IFT : 
 *         @arg kGPIO_IPD : 
 *         @arg kGPIO_IPU : 
 *         @arg kGPIO_OOD : 
 *         @arg kGPIO_OPP : 
 * @retval None
 */
uint32_t GPIO_Init(uint32_t instance, uint32_t pin, GPIO_t mode)
{
    switch(mode)
    {
        case kGPIO_IFT:
            SetPinPull(instance, pin, 0xFF);
            GPIO_SetPinDir(instance, pin, 0);
            break;
        case kGPIO_IPD:
            SetPinPull(instance, pin, 0);
            GPIO_SetPinDir(instance, pin, 0);
            break;
        case kGPIO_IPU:
            SetPinPull(instance, pin, 1);
            GPIO_SetPinDir(instance, pin, 0);
            break;
        case kGPIO_OPPH:
        case kGPIO_OPPL:
            SetPinPull(instance, pin, 0xFF);
            (mode == kGPIO_OPPH)? GPIO_PinWrite(instance, pin, 1):GPIO_PinWrite(instance, pin, 0);
            GPIO_SetPinDir(instance, pin, 1);
//        #if defined(PORT_PCR_DSE_MASK)
//            PORTBases[instance]->PCR[pin] |= PORT_PCR_DSE_MASK;
//        #endif
            break;
        default:
            break;					
    }
    /* config pinMux */
    SetPinMux(instance, pin, 1);
    return instance;
}



 /**
 * @brief  
 * @note   01
 * @param  instance:
 *         @arg HW_GPIOx : GPIOA-E
 * @param  pin : 0-31
 * @param  data : 
 *         @arg 0 : 0
 *         @arg 1 : 1
 * @retval None
 */
void GPIO_PinWrite(uint32_t instance, uint32_t pin, uint8_t data)
{
    (data) ? (GPIOBases[instance]->PSOR |= (1 << pin)):
    (GPIOBases[instance]->PCOR |= (1 << pin));
}

 /**
 * @brief  
 * @note   
 * @param  instance:
 *         @arg HW_GPIOx : GPIOA-E
 * @param  pin : 0-31
 * @retval 01
 */
uint32_t GPIO_PinRead(uint32_t instance, uint32_t pin)
{
    return ((GPIOBases[instance]->PDIR >> pin) & 0x01);
}

 /**
 * @brief  
 * @note   None
 * @param  instance:
 *         @arg HW_GPIOx : GPIOx
 * @param  pin : 0-31
 * @retval None
 */
void GPIO_PinToggle(uint32_t instance, uint8_t pin)
{
    GPIOBases[instance]->PTOR |= (1 << pin);
}

 /**
 * @brief  
 * @note   None
 * @param  instance:
 *         @arg HW_GPIOx : GPIOA-E
 * @retval 32
 */
uint32_t GPIO_ReadPort(uint32_t instance)
{
    return (GPIOBases[instance]->PDIR);
}

 /**
 * @brief  
 * @note   None
 * @param  instance:
 *         @arg HW_GPIOx : GPIOA-E
 * @param  data : 32
 * @retval None
 */
void GPIO_WritePort(uint32_t instance, uint32_t data)
{
    GPIOBases[instance]->PDOR = data;
}


/**
 * @brief  
 * @note   None
 * @param  instance:
 *         @arg HW_GPIOx : GPIOA-E
 * @param  pin : 0-31
 * @param  mode : 
 *         @arg kGPIO_DMA_RE : 
 *         @arg kGPIO_DMA_FE 
 *         @arg kGPIO_DMA_EE 
 * @param  val :
 *         @arg true 
 *         @arg false 
 * @retval CH_OK
 */
//int GPIO_SetIntMode(uint32_t instance, uint32_t pin, GPIO_Int_t mode, bool val)
//{
//    PORTBases[instance]->PCR[pin] &= ~PORT_PCR_IRQC_MASK;
//    if(!val)
//    {
//        
//        return CH_OK;
//    }
//    NVIC_EnableIRQ(GPIO_IrqTbl[instance]);
//    
//    switch(mode)
//    {
//        case kGPIO_Int_RE:
//            PORTBases[instance]->PCR[pin] |= PORT_PCR_IRQC(9);
//            break;
//        case kGPIO_Int_FE:
//            PORTBases[instance]->PCR[pin] |= PORT_PCR_IRQC(10);
//            break;
//        case kGPIO_Int_EE:
//            PORTBases[instance]->PCR[pin] |= PORT_PCR_IRQC(11);
//            break;
//    }
//    return CH_OK;
//}

/**
 * @brief  DMA
 * @note   None
 * @param  instance:
 *         @arg HW_GPIOx : GPIOA-E
 * @param  pin : 0-31
 * @param  mode : 
 *         @arg kGPIO_Int_RE : 
 *         @arg kGPIO_Int_FE 
 *         @arg kGPIO_Int_EE 
 * @param  val :
 *         @arg true 
 *         @arg false 
 * @retval CH_OK
 */
int GPIO_SetDMAMode(uint32_t instance, uint32_t pin, GPIO_DMA_t mode, bool val)
{
    if(!val)
    {
        PORTBases[instance]->PCR[pin] &= ~PORT_PCR_IRQC_MASK;
        return CH_OK;
    }
    PORTBases[instance]->PCR[pin] &= ~PORT_PCR_IRQC_MASK;
    switch(mode)
    {
        case kGPIO_DMA_RE:
            PORTBases[instance]->PCR[pin] |= PORT_PCR_IRQC(1);
            break;
        case kGPIO_DMA_FE:
            PORTBases[instance]->PCR[pin] |= PORT_PCR_IRQC(2);
            break;
        case kGPIO_DMA_EE:
            PORTBases[instance]->PCR[pin] |= PORT_PCR_IRQC(3);
            break;
    }
    return CH_OK;
}

void PORT_IRQHandler(uint32_t instance)
{
    PORTBases[instance]->ISFR = 0xFFFFFFFF;
}
//! @}
