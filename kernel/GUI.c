#include "libc.h"
#include "hilevel.h"

void options(int selected, int level, int toggleDash){
  writeLine("\n");
  writeLine("  ");
  for(int x = 0; x <4; x++){
    if(x == selected && level == 0){
      write(STDOUT_FILENO, "\033[1;30m", 8);
      writeLine("▛▀▀▀▀▀▀▀▀▀▀▀▀▀▀▜");
      write(STDOUT_FILENO, "\033[0m", 5);
    }
    else{
      writeLine("╔══════════════╗");
    }
    writeLine(" ");
  }
  writeLine("\n");
  writeLine("  ");
  for(int x = 0; x <4; x++){
    if(x == selected && level == 0){
      write(STDOUT_FILENO, "\033[1;30m", 8);
      writeLine("▌");
      write(STDOUT_FILENO, "\033[0m", 5);
    }
    else{
      writeLine("║");
    }
    switch(x){
      case 0:{
        writeLine("  EXECUTE P3  ");
        break;
      }
      case 1:{
        writeLine("  EXECUTE P4  ");
        break;
      }
      case 2:{
        writeLine("  EXECUTE P5  ");
        break;
      }
      case 3:{
        writeLine(" PHILOSOPHERS ");
        break;
      }
    }
    if(x == selected && level == 0){
      write(STDOUT_FILENO, "\033[1;30m", 8);
      writeLine("▐");
      write(STDOUT_FILENO, "\033[0m", 5);
    }
    else{
      writeLine("║");
    }
    writeLine(" ");
  }
  writeLine("\n");
  writeLine("  ");
  for(int x = 0; x <4; x++){
    if(x == selected && level == 0){
      write(STDOUT_FILENO, "\033[1;30m", 8);
      writeLine("▙▄▄▄▄▄▄▄▄▄▄▄▄▄▄▟");
      write(STDOUT_FILENO, "\033[0m", 5);
    }
    else{
      writeLine("╚══════════════╝");
    }
    writeLine(" ");
  }
  writeLine("\n");
  if(level == 1 && selected >= 2){
    write(STDOUT_FILENO, "\033[1;31m", 8);
    writeLine("  ╔═══════════════════════════════╗ ▛▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▜\n");
    writeLine("  ║         TERMINATE ALL         ║ ▌             RESET             ▐\n");
    writeLine("  ╚═══════════════════════════════╝ ▙▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▟\n");
    write(STDOUT_FILENO, "\033[0m", 5);
  }
  else if(level == 1 && selected < 2){
    write(STDOUT_FILENO, "\033[1;31m", 8);
    writeLine("  ▛▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▜ ╔═══════════════════════════════╗\n");
    writeLine("  ▌         TERMINATE ALL         ▐ ║             RESET             ║\n");
    writeLine("  ▙▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▟ ╚═══════════════════════════════╝\n");
    write(STDOUT_FILENO, "\033[0m", 5);
  }
  if(level != 1){
    write(STDOUT_FILENO, "\033[1;31m", 8);
    writeLine("  ╔═══════════════════════════════╗ ╔═══════════════════════════════╗\n");
    writeLine("  ║         TERMINATE ALL         ║ ║             RESET             ║\n");
    writeLine("  ╚═══════════════════════════════╝ ╚═══════════════════════════════╝\n");
    write(STDOUT_FILENO, "\033[0m", 5);
  }
  if(level != 2){
    if(toggleDash == true)
    write(STDOUT_FILENO, "\033[1;32m", 8);
    writeLine("  ╔═════════════════════════════════════════════════════════════════╗\n");
    writeLine("  ║                           TOGGLE DASH                           ║\n");
    writeLine("  ╚═════════════════════════════════════════════════════════════════╝");
    write(STDOUT_FILENO, "\033[0m", 5);
  }
  else if(level == 2){
    if(toggleDash == true)
    write(STDOUT_FILENO, "\033[1;32m", 8);
    writeLine("  ▛▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▜\n");
    writeLine("  ▌                           TOGGLE DASH                           ▐\n");
    writeLine("  ▙▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▟");
    write(STDOUT_FILENO, "\033[0m", 5);
  }
  writeLine("\n  ");
}
void displayDashboard(int processes, pcb_t pcb[]){
  write(STDOUT_FILENO, "\033[1;29m", 8);
  writeLine("                ╔═══════╦════════╦════════╦═══════════╗\n");
  writeLine("                ║  PID  ║  PRIO  ║  NAME  ║   STATE   ║\n");
  writeLine("                ╠═══════╩════════╩════════╩═══════════╣\n");
  for(int i = 0; i <= processes; i++){
    writeLine("                ║  ");
    printNumber(pcb[i].pid);
    writeLine("       ");
    printNumber(pcb[i].priority);
    writeLine("      ");
    write(STDOUT_FILENO, "\033[0m", 5);
    write(STDOUT_FILENO, "\033[1;30m", 8);
    write(STDOUT_FILENO, pcb[i].name, strlen(pcb[i].name));
    write(STDOUT_FILENO, "\033[0m", 5);
    switch(pcb[i].status){
      case STATUS_READY :{
        write(STDOUT_FILENO, "\033[1;32m", 8);
        writeLine("      READY   ");
        write(STDOUT_FILENO, "\033[0m", 5);
        writeLine("║\n");
        break;
      }
      case STATUS_TERMINATED :{
        write(STDOUT_FILENO, "\033[1;31m", 8);
        writeLine("    COMPLETED ");
        write(STDOUT_FILENO, "\033[0m", 5);
        writeLine("║\n");
        break;
      }
      case STATUS_EXECUTING :{
        write(STDOUT_FILENO, "\033[1;34m", 8);
        writeLine("    EXECUTING ");
        write(STDOUT_FILENO, "\033[0m", 5);
        writeLine("║\n");
        break;
      }
    }
    if(i<processes)
    writeLine("                ╟─────────────────────────────────────╢\n");
  }
  writeLine("                ╚═════════════════════════════════════╝\n");
}
