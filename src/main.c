#include <stdint.h>
#include <curses.h>
#include <stdlib.h>
#include "cpu.h"
#include "bus.h"

int main(int argc, char** argv)
{

  if (argv[1] == NULL)
  {
    fprintf(stderr, "Usage: ./6502Emu <path_to_program>\n");
    return -1;
  }

  char* filename = argv[1];

  CPU* cpu;
  Bus* bus;
  int c;

  initscr();

  cpu = cpu_create();
  bus = bus_create();

  cpu_connect_bus(cpu, bus);

  FILE* file = fopen(filename, "rb");

  fread(bus->memory, 1, 0x10000, file);

  fclose(file);
 
  cpu_reset(cpu);

  char* instr = malloc(sizeof(char) * 16);

  if (instr == NULL) goto exit;


  while(1)
  {
    c = wgetch(stdscr);
    erase();

    switch (c)
    {
      case ' ':
        do
        {
          cpu_clock(cpu);
        } while (!cpu_complete(cpu));
        break;
      
      case 'r':
        cpu_reset(cpu);
        break;

      case 'i':
        cpu_irq(cpu);
        break;

      case 'q':
        goto exit;
      
      default:
        break;

    }

    instr = cpu_disassemble(cpu, instr);

    mvprintw(1, 1, "PC: $%04x", cpu->pc);
    mvprintw(2, 1, "X: $%02x", cpu->x);
    mvprintw(3, 1, "Y: $%02x", cpu->y);
    mvprintw(4, 1, "A: $%02x", cpu->a);

    mvprintw(6, 1, "NVUBDIZC");
    mvprintw(7, 1, "%d", cpu_flag_get(cpu, N));
    mvprintw(7, 2, "%d", cpu_flag_get(cpu, V));
    mvprintw(7, 3, "%d", cpu_flag_get(cpu, U));
    mvprintw(7, 4, "%d", cpu_flag_get(cpu, B));
    mvprintw(7, 5, "%d", cpu_flag_get(cpu, D));
    mvprintw(7, 6, "%d", cpu_flag_get(cpu, I));
    mvprintw(7, 7, "%d", cpu_flag_get(cpu, Z));
    mvprintw(7, 8, "%d", cpu_flag_get(cpu, C));

    mvprintw(1, 20, "%s", instr);

    refresh();

  }

exit:
  free(instr);
  endwin();
  cpu_destroy(cpu);

}
