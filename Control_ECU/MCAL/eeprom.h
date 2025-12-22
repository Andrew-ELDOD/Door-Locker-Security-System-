#ifndef EEPROM_H
#define EEPROM_H

#include <stdint.h>

#define EEPROM_TOTAL_BLOCKS   64
#define EEPROM_BLOCK_SIZE     64

#define EEPROM_SUCCESS        1
#define EEPROM_ERROR          0
#define EEPROM_TIMEOUT        2

uint8_t EEPROM_Init(void);
uint8_t EEPROM_WriteWord(uint32_t block, uint32_t offset, uint32_t data);
uint8_t EEPROM_ReadWord(uint32_t block, uint32_t offset, uint32_t *data);
uint8_t EEPROM_WriteBuffer(uint32_t block, uint32_t offset, const uint8_t *buffer, uint32_t length);
uint8_t EEPROM_ReadBuffer(uint32_t block, uint32_t offset, uint8_t *buffer, uint32_t length);
uint8_t EEPROM_MassErase(void);

#endif
