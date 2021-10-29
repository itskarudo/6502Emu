#include "bus.h"
#include <stdlib.h>

Bus* bus_create(void)
{
  Bus* bus = malloc(sizeof(Bus));  
  if (bus == NULL)
  {
    fprintf(stderr, "Failed to allocate memory, abort.\n");
    exit(-1);
  }

  bus->memory = malloc(64 * 1024);
  if (bus->memory == NULL)
  {
    fprintf(stderr, "Failed to allocate memory, abort.\n");
    exit(-1);
  }

  return bus;
}

void bus_destroy(Bus* bus)
{
  free(bus->memory);
  free(bus);
}

uint8_t bus_read(Bus* bus, uint16_t address)
{
  return bus->memory[address];
}

void bus_write(Bus* bus, uint16_t address, uint8_t value)
{
  bus->memory[address] = value;
}
