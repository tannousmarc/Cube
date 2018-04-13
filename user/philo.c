
#include "philo.h"

void main_philo() {
  int id = getPhiloId();
  // printNumber(id);
  // write(STDOUT_FILENO, "PHILOINIT\n", 10);
  waitWrite(id, 0, id);
  waitWrite(id, 0, 17);
  waitWrite(id, 0, 18);
  writeLine("PHILODONE\n");
  // get current philosopher's place
  //int id = getID();
  // generatePipe(1,2);
  // writePipe(0, 0, 1);
  // readPipe(0, 0);
  // writePipe(0, 0, 12);
  // readPipe(0, 0);
  // generatePipe(3,4);
  // writePipe(1, 1, 4);
  // readPipe(1, 1);
  // readPipe(1, 0);
  exit( EXIT_SUCCESS );
}
