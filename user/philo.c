
#include "philo.h"
#include "waiter.h"

void waitFork(int id){
  id += PHIL_NO;
  waitRead(id, 0);
  waitWrite(id, 0, -1);
}
void downFork(int id){
  id += PHIL_NO;
  waitWrite(id, 0, 0);
}

int min(int a, int b){
  return (a < b ? a : b);
}
int max(int a, int b){
  return (a > b ? a : b);
}
void think(int id){
  int write = getDash();
  if(!write){
    writeLine("P");
    printNumber(id);
    writeLine(" has started thinking.\n");
  }
  resetClock(id);
  waitRead(id,0);
  if(!write){
    writeLine("P");
    printNumber(id);
    writeLine(" is done thinking.\n");
  }
}

void eat(int id){
  int write = getDash();
  if(!write){
    writeLine("P");
    printNumber(id);
    writeLine(" has started eating.\n");
  }
  resetClock(id);
  waitRead(id,0);
  if(!write){
    writeLine("P");
    printNumber(id);
    writeLine(" is done eating.\n");
  }
}


void main_philo() {
  int id = getPhiloId();
  // printNumber(id);
  // write(STDOUT_FILENO, "PHILOINIT\n", 10);
  // waitWrite(id, 0, id);
  // waitWrite(id, 0, 17);
  // waitWrite(id, 0, 18);
  while(true){
    think(id);
    waitFork(min(id, (id+1) % PHIL_NO));
    waitFork(max(id, (id+1) % PHIL_NO));
    eat(id);
    downFork((id+1) % PHIL_NO);
    downFork(id);
  }
  exit( EXIT_SUCCESS );
}
