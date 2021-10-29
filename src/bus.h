#ifndef __BUS_H__
#define __BUS_H__

#include <stdint.h>

typedef struct
{
  uint8_t* memory; // pointer to 64Kb memory array
} Bus;

Bus* bus_create(void);
void bus_destroy(Bus* bus);

uint8_t bus_read(Bus* bus, uint16_t address);
void    bus_write(Bus* bus, uint16_t address, uint8_t value);

#endif /* __BUS_H__ */
