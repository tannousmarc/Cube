
#include "philo.h"


void main_philo() {
  generatePipe(1,2);
  writePipe(0, 0, 1);
  readPipe(0, 0);
  writePipe(0, 0, 12);
  readPipe(0, 0);
  generatePipe(3,4);
  writePipe(1, 1, 4);
  readPipe(1, 1);
  readPipe(1, 0);
  exit( EXIT_SUCCESS );
}
