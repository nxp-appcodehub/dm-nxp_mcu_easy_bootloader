/*
* Copyright 2023 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/
#include "common.h"
#include "gpio.h"

/* gloabl vars */
#ifndef GPIO_BASES
#define GPIO_BASES {PTA, PTB, PTC, PTD, PTE}
#endif

static GPIO_Type * const GPIO_IPTbl[] = GPIO_BASES;
static PORT_Type * const PORT_IPTbl[] = PORT_BASES;

static const Reg_t CLKTbl[] =
{
    {(void*)&(SIM->SCGC5), SIM_SCGC5_PORTA_MASK, SIM_SCGC5_PORTA_SHIFT},
    {(void*)&(SIM->SCGC5), SIM_SCGC5_PORTB_MASK, SIM_SCGC5_PORTB_SHIFT},
    {(void*)&(SIM->SCGC5), SIM_SCGC5_PORTC_MASK, SIM_SCGC5_PORTC_SHIFT},
    {(void*)&(SIM->SCGC5), SIM_SCGC5_PORTD_MASK, SIM_SCGC5_PORTD_SHIFT},
    {(void*)&(SIM->SCGC5), SIM_SCGC5_PORTE_MASK, SIM_SCGC5_PORTE_SHIFT},
};

#if defined(MKL27Z4)
static const IRQn_Type GPIO_IrqTbl[] = 
{
    (IRQn_Type)PORTA_IRQn,
    (IRQn_Type)0,
    (IRQn_Type)PORTCD_IRQn,
    (IRQn_Type)PORTCD_IRQn,
};
#else
static const IRQn_Type GPIO_IrqTbl[] = 
{
    (IRQn_Type)(PORTA_IRQn+0),
    (IRQn_Type)(PORTA_IRQn+1),
    (IRQn_Type)(PORTA_IRQn+2),
    (IRQn_Type)(PORTA_IRQn+3),
    (IRQn_Type)(PORTA_IRQn+4),
};
#endif


//! @defgroup CHKinetis
//! @{


//! @defgroup GPIO
//! @brief GPIO API functions
//! @{


 /**
 * @brief    
 * @note   GPIO
 * @code
 *      // PORTB3
 *      GPIO_PinConfig(HW_GPIOB, 3, kInpput);
 * @endcode
 * @param  instance: GPIO
 *         @arg HW_GPIOA :PORTA
 *         @arg HW_GPIOB :PORTB
 *         @arg HW_GPIOC :PORTC
 *         @arg HW_GPIOD :PORTD
 *         @arg HW_GPIOE :PORTE
 * @param  pinIndex  : 0~31
 * @param  mode      :
 *         @arg kInpput  :
 *         @arg kOutput  :
 * @retval None
 */
void inline GPIO_SetPinDir(uint32_t instance, uint32_t pin, uint32_t dir)
{
    CLK_EN(CLKTbl, instance);
    (dir == 1) ? (GPIO_IPTbl[instance]->PDDR |= (1 << pin)):
    (GPIO_IPTbl[instance]->PDDR &= ~(1 << pin));
}

 /**
 * @brief  init a pin
 * @note   can also init mutiple pin
 * @param  instance:
 *         @arg HW_GPIOx : GPIO moudle x
 * @param  pin : pin number from 0-31
 * @param  mode : pin operation mode
 *         @arg kGPIO_Mode_IFT : input floating
 *         @arg kGPIO_Mode_IPD : pull down
 *         @arg kGPIO_Mode_IPU : pull up
 *         @arg kGPIO_Mode_OOD : open drain
 *         @arg kGPIO_Mode_OPP : push pull
 * @retval None
 */
uint32_t GPIO_Init(uint32_t instance, uint32_t pin, GPIO_Mode_t mode)
{
    int i;
    if(pin < 32)
    {
        switch(mode)
        {
            case kGPIO_Mode_IFT:
                SetPinPull(instance, pin, 0xFF);
                GPIO_SetPinDir(instance, pin, 0);
                break;
            case kGPIO_Mode_IPD:
                SetPinPull(instance, pin, 0);
                GPIO_SetPinDir(instance, pin, 0);
                break;
            case kGPIO_Mode_IPU:
                SetPinPull(instance, pin, 1);
                GPIO_SetPinDir(instance, pin, 0);
                break;
            case kGPIO_Mode_OPP:
                SetPinPull(instance, pin, 0xFF);
                GPIO_SetPinDir(instance, pin, 1);
                break;
            default:
                break;					
        }
        /* config pinMux */
        SetPinMux(instance, pin, 1);
    }
    else
    {
        for(i=0; i<31; i++)
        {
            if(pin & (1<<i))
            {
                switch(mode)
                {
                    case kGPIO_Mode_IFT:
                        SetPinPull(instance, i, 0xFF);
                        GPIO_SetPinDir(instance, i, 0);
                        break;
                    case kGPIO_Mode_IPD:
                        SetPinPull(instance, i, 0);
                        GPIO_SetPinDir(instance, i, 0);
                        break;
                    case kGPIO_Mode_IPU:
                        SetPinPull(instance, i, 1);
                        GPIO_SetPinDir(instance, i, 0);
                        break;
                    case kGPIO_Mode_OPP:
                        SetPinPull(instance, i, 0xFF);
                        GPIO_SetPinDir(instance, i, 1);
                        break;
                    default:
                        break;					
                }
            }
            SetPinMux(instance, i, 1);
        }
    }
    return instance;
}


 /**
 * @brief  
 * @note   
 * @code
 *      //PORTB10
 *      GPIO_WriteBit(HW_GPIOB, 10, 1);
 * @endcode
 * @param  instance: GPIO
 *         @arg HW_GPIOA :PORTA
 *         @arg HW_GPIOB :PORTB
 *         @arg HW_GPIOC :PORTC
 *         @arg HW_GPIOD :PORTD
 *         @arg HW_GPIOE :PORTE
 * @param  pinIndex  : 0~31
 * @param  data   :   
 *         @arg 0 :  
 *         @arg 1 : 
 * @retval None
 */
void inline GPIO_PinWrite(uint32_t instance, uint32_t pin, uint8_t data)
{
    (data) ? (GPIO_IPTbl[instance]->PSOR |= (1 << pin)):
    (GPIO_IPTbl[instance]->PCOR |= (1 << pin));
}
 /**
 * @brief  
 * @code
 *      //PORTB10
 *      uint8_t status ; //
 *      status = GPIO_ReadBit(HW_GPIOB, 10); //status
 * @endcode
 * @param  instance: GPIO
 *         @arg HW_GPIOA :PORTA
 *         @arg HW_GPIOB :PORTB
 *         @arg HW_GPIOC :PORTC
 *         @arg HW_GPIOD :PORTD
 *         @arg HW_GPIOE :PORTE
 * @param  pinIndex  : 0~31
 * @retval 
 *         @arg 0 : 
 *         @arg 1 : 
 */
uint32_t GPIO_PinRead(uint32_t instance, uint32_t pin)
{
    return ((GPIO_IPTbl[instance]->PDIR >> pin) & 0x01);
}

 /**
 * @brief  
 * @code
 *      //PORTB10
 *      GPIO_ToggleBit(HW_GPIOB, 10); 
 * @endcode
 * @param  instance: GPIO
 *         @arg HW_GPIOA :PORTA
 *         @arg HW_GPIOB :PORTB
 *         @arg HW_GPIOC :PORTC
 *         @arg HW_GPIOD :PORTD
 *         @arg HW_GPIOE :PORTE
 * @param  pinIndex  : 0~31
 * @retval None
 */
void GPIO_PinToggle(uint32_t instance, uint8_t pin)
{
    GPIO_IPTbl[instance]->PTOR |= (1 << pin);
}

/**
 * @brief  32
 * @code
 *      //PORTB
 *      uint32_t status ; //
 *      status = GPIO_ReadPort(HW_GPIOB); //status
 * @endcode
 * @param  instance: GPIO
 *         @arg HW_GPIOA :PORTA
 *         @arg HW_GPIOB :PORTB
 *         @arg HW_GPIOC :PORTC
 *         @arg HW_GPIOD :PORTD
 *         @arg HW_GPIOE :PORTE
 * @retval 32
 */
uint32_t GPIO_ReadPort(uint32_t instance)
{
    return (GPIO_IPTbl[instance]->PDIR);
}
/**
 * @brief  32
 * @code
 *      //PORTB0xFFFFFFFF
 *      GPIO_WriteByte(HW_GPIOB, 0xFFFFFFFF); 
 * @endcode
 * @param  instance: GPIO
 *         @arg HW_GPIOA :PORTA
 *         @arg HW_GPIOB :PORTB
 *         @arg HW_GPIOC :PORTC
 *         @arg HW_GPIOD :PORTD
 *         @arg HW_GPIOE :PORTE
 * @param  data  :32
 * @retval None
 */
void GPIO_WritePort(uint32_t instance, uint32_t data)
{
    GPIO_IPTbl[instance]->PDOR = data;
}

/**
 * @brief  set pin interrupt mode
 * @retval None
 */
int GPIO_SetIntMode(uint32_t instance, uint32_t pin, GPIO_Int_t mode, bool val)
{
    CLK_EN(CLKTbl, instance);
    if(!val)
    {
        PORT_IPTbl[instance]->PCR[pin] &= ~PORT_PCR_IRQC_MASK;
        return 0;
    }
    NVIC_EnableIRQ(GPIO_IrqTbl[instance]);
    PORT_IPTbl[instance]->PCR[pin] &= ~PORT_PCR_IRQC_MASK;
    switch(mode)
    {
        case kGPIO_Int_RE:
            PORT_IPTbl[instance]->PCR[pin] |= PORT_PCR_IRQC(9);
            break;
        case kGPIO_Int_FE:
            PORT_IPTbl[instance]->PCR[pin] |= PORT_PCR_IRQC(10);
            break;
        case kGPIO_Int_EE:
            PORT_IPTbl[instance]->PCR[pin] |= PORT_PCR_IRQC(11);
            break;
    }
    return 0;
}


//! @}
