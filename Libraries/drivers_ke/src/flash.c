/*
 * Copyright 2018-2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "flash.h"
#include "common.h"


#define SECTOR_SIZE     (512)

#define FTMRH_FCLKDIV_FDIVLD_MASK                0x80u
#define FTMRH_FSTAT_CCIF_MASK                    0x80u
#define FTMRH_FSTAT_ACCERR_MASK                  0x20u
#define FTMRH_FSTAT_FPVIOL_MASK                  0x10u
#define FTMRH_FSTAT_MGSTAT_MASK                  0x3u
#define FTMRH_ERROR                              (FTMRH_FSTAT_ACCERR_MASK | FTMRH_FSTAT_FPVIOL_MASK | FTMRH_FSTAT_MGSTAT_MASK)
#define FTFx_SSD_FSTAT_FPVIOL                   (8)


#ifdef FTMRE_FSEC_KEYEN_MASK
#define FTMRX   FTMRE
#elif FTMRH_FSEC_KEYEN_MASK
#define FTMRX   FRMRH
#endif

 /**
 * @brief  
 * @note   None
 * @param  None
 * @retval Flash
 */
uint32_t FLASH_GetSectorSize(void)
{
    return SECTOR_SIZE;
}


 /**
 * @brief  Flash
 * @note   None
 * @param  None
 * @retval Flash
 */
void FLASH_Init(void)
{
    FTMRX->FCLKDIV = 0x1F;
    MCM->PLACR = 0x1BC00; /* Disable flash cache */
}


uint8_t FLASH_EEP_EraseSector(uint32_t addr)
{
    // Clear error flags
    FTMRX->FSTAT = 0x30;
    
    // Write index to specify the command code to be loaded
    FTMRX->FCCOBIX = 0;
    // Write command code and memory address bits[23:16]	
    FTMRX->FCCOBHI = 0x12;

    FTMRX->FCCOBLO = (uint8_t)((addr >> 16)&0x007f);   // memory address bits[17:16]
    // Write index to specify the lower byte memory address bits[15:0] to be loaded
    FTMRX->FCCOBIX = 0x1;
    // Write the lower byte memory address bits[15:0]
    //FTMRH_FCCOB = (uint16_t)adr;
    FTMRX->FCCOBHI = (uint8_t)(addr >>  8);
    FTMRX->FCCOBLO = (uint8_t)(addr);

    // Launch the command
    FTMRX->FSTAT = 0x80;
    
    // Wait till command is completed
    while (!(FTMRX->FSTAT & FTMRH_FSTAT_CCIF_MASK));

    if (FTMRX->FSTAT & FTMRH_FSTAT_FPVIOL_MASK)  return(FTFx_SSD_FSTAT_FPVIOL);
    if (FTMRX->FSTAT & FTMRH_ERROR)              return(1);
    return (0);
}

uint8_t FLASH_EraseSector(uint32_t addr)
{
    // Clear error flags
    FTMRX->FSTAT = 0x30;
    
    // Write index to specify the command code to be loaded
    FTMRX->FCCOBIX = 0;
    // Write command code and memory address bits[23:16]	
    FTMRX->FCCOBHI = 0x0A;                            // Flash Sector Erase command

    FTMRX->FCCOBLO = (uint8_t)((addr >> 16)&0x007f);   // memory address bits[17:16]
    // Write index to specify the lower byte memory address bits[15:0] to be loaded
    FTMRX->FCCOBIX = 0x1;
    // Write the lower byte memory address bits[15:0]
    //FTMRH_FCCOB = (uint16_t)adr;
    FTMRX->FCCOBHI = (uint8_t)(addr >>  8);
    FTMRX->FCCOBLO = (uint8_t)(addr);

    // Launch the command
    FTMRX->FSTAT = 0x80;
    
    // Wait till command is completed
    while (!(FTMRX->FSTAT & FTMRH_FSTAT_CCIF_MASK));

    if (FTMRX->FSTAT & FTMRH_FSTAT_FPVIOL_MASK)  return(FTFx_SSD_FSTAT_FPVIOL);
    if (FTMRX->FSTAT & FTMRH_ERROR)              return(1);
    return (0);
}


uint8_t FLASH_EEP_WriteSector(uint32_t addr, const uint8_t *buf, uint32_t len)
{
  int  i;

  for (i = 0; i < ((len+1)/2); i++)  {
    // Clear error flags
    FTMRX->FSTAT = 0x30;
    
    // Write index to specify the command code to be loaded
    FTMRX->FCCOBIX = 0;
    // Write command code and memory address bits[17:16]	
    FTMRX->FCCOBHI = 0x11;// program EEPROM command
    FTMRX->FCCOBLO = (uint8_t) ((addr >> 16));// Addr[22:16] always 0
    // Write index to specify the lower byte memory address bits[15:0] to be loaded
    FTMRX->FCCOBIX = 0x1;
    // Write the lower byte memory address bits[15:0]
    FTMRX->FCCOBHI = (uint8_t)(addr >>  8);
    FTMRX->FCCOBLO = (uint8_t)(addr);

    // Write index to specify the word0 (MSB word) to be programmed
    FTMRX->FCCOBIX = 0x2;
    FTMRX->FCCOBLO =  buf[0];
    // Write index to specify the word1 (LSB word) to be programmed
    FTMRX->FCCOBIX = 0x3;
    FTMRX->FCCOBLO =  buf[1];
    
    // Launch the command
    FTMRX->FSTAT = 0x80;
    // Wait till command is completed
    while (!(FTMRX->FSTAT & FTMRH_FSTAT_CCIF_MASK))
     ;
    // Check error status
    if (FTMRX->FSTAT & FTMRH_ERROR) return(1);
    buf += 2;
    addr += 2;

  }
  return (0);                                  // Finished without Errors
}

uint8_t FLASH_WriteSector(uint32_t addr, const uint8_t *buf, uint32_t len)
{
    int  i;

    for (i = 0; i < ((len+7)/8); i++)
    {
        // Clear error flags
        FTMRX->FSTAT = 0x30;
    
        // Write index to specify the command code to be loaded
        FTMRX->FCCOBIX = 0;
        // Write command code and memory address bits[17:16]	
        FTMRX->FCCOBHI = 0x06;// program P-FLASH command
        FTMRX->FCCOBLO = (uint8_t) ((addr >> 16));// Addr[22:16] always 0
        // Write index to specify the lower byte memory address bits[15:0] to be loaded
        FTMRX->FCCOBIX = 0x1;
        // Write the lower byte memory address bits[15:0]
        FTMRX->FCCOBHI = (uint8_t)(addr >>  8);
        FTMRX->FCCOBLO = (uint8_t)(addr);

        // Write index to specify the word0 (MSB word) to be programmed
        FTMRX->FCCOBIX = 0x2;
        // Write the word0
        FTMRX->FCCOBHI =  buf[1];
        FTMRX->FCCOBLO =  buf[0];
        // Write index to specify the word1 (LSB word) to be programmed
        FTMRX->FCCOBIX = 0x3;
        // Write the word1
        FTMRX->FCCOBHI =  buf[3];
        FTMRX->FCCOBLO =  buf[2];
    
        // Write the word2
		FTMRX->FCCOBIX = 0x4;
        FTMRX->FCCOBHI =  buf[5];
        FTMRX->FCCOBLO =  buf[4];

        // Write the word3
        FTMRX->FCCOBIX = 0x5;
        FTMRX->FCCOBHI =  buf[7];
        FTMRX->FCCOBLO =  buf[6];
    
        // Launch the command
        FTMRX->FSTAT = 0x80;
        // Wait till command is completed
        while (!(FTMRX->FSTAT & FTMRH_FSTAT_CCIF_MASK));
        // Check error status
        if (FTMRX->FSTAT & FTMRH_ERROR) return(1);
        buf += 8;
        addr += 8;

    }
    return (0);                                  // Finished without Errors
}

 /**
 * @brief  
 * @note   None
 * @param  addr: 
 * @retval CH_OKCH_ERR
 */
static uint32_t FLASH_SetcorTest(uint32_t addr)
{
    uint32_t ret, i, j;
    uint8_t *p;
    ALIGN(8) uint8_t buf[32];
    
    LIB_TRACE("program addr:0x%X(%dKB) ...", addr, addr/1024);
    ret = FLASH_EraseSector(addr);
    
    for(i=0; i<sizeof(buf); i++)
    {
        buf[i] = i % 0xFF;
    }
    
    for(i=0; i<(SECTOR_SIZE/sizeof(buf)); i++)
    {
        ret += FLASH_WriteSector(addr + sizeof(buf)*i, buf, sizeof(buf));  
        if(ret)
        {
            LIB_TRACE("issue command failed\r\n");
            return CH_ERR;
        }
    }
    
    LIB_TRACE("varify addr:0x%X ...", addr);
    for(i=0; i<(SECTOR_SIZE/sizeof(buf)); i++)
    {
        p = (uint8_t*)(addr + sizeof(buf)*i);
        for(j=0; j<sizeof(buf); j++)
        {
            if(p[j] != (j%0xFF))
            {
                ret++;
                LIB_TRACE("ERR:[%d]:0x%02X ", i, *p); 
            }
        }
    }
    
    if(ret == 0)
    {
        LIB_TRACE("OK\r\n"); 
    }
    return ret;
}

 /**
 * @brief  
 * @note   None
 * @param  addr: 
 * @retval CH_OKCH_ERR
 */
static uint32_t FLASH_SetcorSizeTest(uint32_t addr)
{
    int i;
    uint8_t *p;
    FLASH_SetcorTest(addr);
    FLASH_SetcorTest(addr + SECTOR_SIZE);
    FLASH_EraseSector(addr);
    p = (uint8_t*)(addr);
    
    for(i=0; i<SECTOR_SIZE*2; i++)
    {
        /* if (all content = 0xFF), then there is a error */
        if(*p++ != 0xFF)
        {
            break;
        }
    }
    
    if(i == SECTOR_SIZE*2)
    {
        LIB_TRACE("Flash SectorTest Error in %d\r\n", i);
        return CH_ERR;
    }
    
    return CH_OK;
}

 /**
 * @brief  Flash
 * @note   
 * @param  addr: 
 * @retval CH_OKCH_ERR
 */
uint32_t FLASH_Test(uint32_t addr, uint32_t len)
{
    int i, ret;
    FLASH_Init();
    FLASH_SetcorSizeTest(addr);
    for(i=0; i<(len/SECTOR_SIZE); i++)
    {
        ret = FLASH_SetcorTest(addr + i*SECTOR_SIZE);
        if(ret != CH_OK)
        {
            return ret;
        }
    }
    return ret;
}
