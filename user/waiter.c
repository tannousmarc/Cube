
#include "waiter.h"


// phil_t players[4];

extern void main_philo();




void main_waiter() {

  while(getPhiloNo()<PHIL_NO){
    fork();
    exec(&main_philo);
  }

  initPipes();
  exit( EXIT_SUCCESS );
}
