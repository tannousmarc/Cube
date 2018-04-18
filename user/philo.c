
#include "philo.h"
#include "waiter.h"


int min(int a, int b){
  return (a < b ? a : b);
}
int max(int a, int b){
  return (a > b ? a : b);
}
void think(int id){
  int write = getDash();
  if(!write){
    writeLine("P",1);
    printNumber(id);
    writeLine(" has started thinking.\n",23);
  }
  resetClock(id);
  waitRead(id,0);
  if(!write){
    writeLine("P",1);
    printNumber(id);
    writeLine(" is done thinking.\n",19);
  }
}

void eat(int id){
  int write = getDash();
  if(!write){
    writeLine("P",1);
    printNumber(id);
    writeLine(" has started eating.\n",21);
  }
  resetClock(id);
  waitRead(id,0);
  if(!write){
    writeLine("P",1);
    printNumber(id);
    writeLine(" is done eating.\n",17);
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
