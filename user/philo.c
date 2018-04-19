
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
    writeLine("\n  P");
    printNumber(id);
    writeLine(" has started thinking.");
  }
  resetClock(id);
  waitRead(id,0);
  if(!write){
    writeLine("\n  P");
    printNumber(id);
    writeLine(" is done thinking.");
  }
}

void eat(int id){
  int write = getDash();
  if(!write){
    writeLine("\n  P");
    printNumber(id);
    writeLine(" has started eating.");
  }
  resetClock(id);
  waitRead(id,0);
  if(!write){
    writeLine("\n  P");
    printNumber(id);
    writeLine(" is done eating.");
  }
}


void main_philo() {
  int id = getPhiloId();

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
