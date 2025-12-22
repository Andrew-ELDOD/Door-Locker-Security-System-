#include "eeprom.h"
#include "tm4c123gh6pm.h"

/* Wait for EEPROM operation to complete */
static uint8_t EEPROM_WaitDone(void)
{
    uint32_t timeout = 1000000;
    while ((EEPROM_EEDONE_R & EEPROM_EEDONE_WORKING) && timeout--) ;
    if(timeout==0) return EEPROM_TIMEOUT;
    if(EEPROM_EEDONE_R & (EEPROM_EEDONE_INVPL | EEPROM_EEDONE_NOPERM)) return EEPROM_ERROR;
    return EEPROM_SUCCESS;
}

uint8_t EEPROM_Init(void)
{
    uint32_t timeout = 1000000;
    SYSCTL_RCGCEEPROM_R |= 0x01;
    while((SYSCTL_RCGCEEPROM_R & 0x01)==0 && timeout--) ;
    if(timeout==0) return EEPROM_ERROR;

    for(timeout=0;timeout<6;timeout++);

    timeout=1000000;
    while((EEPROM_EEDONE_R & EEPROM_EEDONE_WORKING) && timeout--) ;
    if(timeout==0) return EEPROM_ERROR;

    if(EEPROM_EESUPP_R & 0x0C) return EEPROM_ERROR;
    EEPROM_EEBLOCK_R = 0;
    EEPROM_EEOFFSET_R = 0;
    return EEPROM_SUCCESS;
}

uint8_t EEPROM_WriteWord(uint32_t block, uint32_t offset, uint32_t data)
{
    if(block>=EEPROM_TOTAL_BLOCKS || offset>=EEPROM_BLOCK_SIZE) return EEPROM_ERROR;
    EEPROM_EEBLOCK_R = block;
    EEPROM_EEOFFSET_R = offset;
    EEPROM_EERDWR_R = data;
    return EEPROM_WaitDone();
}

uint8_t EEPROM_ReadWord(uint32_t block, uint32_t offset, uint32_t *data)
{
    if(block>=EEPROM_TOTAL_BLOCKS || offset>=EEPROM_BLOCK_SIZE || data==0) return EEPROM_ERROR;
    EEPROM_EEBLOCK_R = block;
    EEPROM_EEOFFSET_R = offset;
    *data = EEPROM_EERDWR_R;
    return EEPROM_SUCCESS;
}

uint8_t EEPROM_WriteBuffer(uint32_t block, uint32_t offset, const uint8_t *buffer, uint32_t length)
{
    if(buffer==0 || length==0) return EEPROM_ERROR;
    uint32_t i=0, word, current_block=block, current_offset=offset;
    uint8_t result;
    while(i<length)
    {
        word = 0xFFFFFFFF;
        for(uint8_t b=0; b<4; b++)
        {
            if(i+b<length)
            {
                word &= ~(0xFF << (b*8));
                word |= ((uint32_t)buffer[i+b] << (b*8));
            }
        }
        result=EEPROM_WriteWord(current_block,current_offset,word);
        if(result!=EEPROM_SUCCESS) return result;
        current_offset++;
        if(current_offset>=EEPROM_BLOCK_SIZE)
        {
            current_offset=0;
            current_block++;
            if(current_block>=EEPROM_TOTAL_BLOCKS) return EEPROM_ERROR;
        }
        i+=4;
    }
    return EEPROM_SUCCESS;
}

uint8_t EEPROM_ReadBuffer(uint32_t block, uint32_t offset, uint8_t *buffer, uint32_t length)
{
    if(buffer==0 || length==0) return EEPROM_ERROR;
    uint32_t i=0, word, current_block=block, current_offset=offset;
    uint8_t result;
    while(i<length)
    {
        result=EEPROM_ReadWord(current_block,current_offset,&word);
        if(result!=EEPROM_SUCCESS) return result;
        for(uint8_t b=0; b<4 && i<length; b++) buffer[i++] = (word >> (b*8)) & 0xFF;
        current_offset++;
        if(current_offset>=EEPROM_BLOCK_SIZE)
        {
            current_offset=0;
            current_block++;
            if(current_block>=EEPROM_TOTAL_BLOCKS) return EEPROM_ERROR;
        }
    }
    return EEPROM_SUCCESS;
}

uint8_t EEPROM_MassErase(void)
{
    for(uint32_t block=0; block<EEPROM_TOTAL_BLOCKS; block++)
    {
        for(uint32_t offset=0; offset<EEPROM_BLOCK_SIZE; offset++)
        {
            if(EEPROM_WriteWord(block,offset,0xFFFFFFFF)!=EEPROM_SUCCESS) return EEPROM_ERROR;
        }
    }
    return EEPROM_SUCCESS;
}
