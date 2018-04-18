
#include "waiter.h"


// phil_t players[4];

extern void main_philo();




void main_waiter() {
  // write(STDOUT_FILENO,"WAITERINIT\n",11);
  int cnt = 0;
  for(int i = 0; i < PHIL_NO; i++){
    if(getPhiloNo()<PHIL_NO){

      fork();
      exec(&main_philo);
    }
    else{
      exit(EXIT_SUCCESS);
    }
    // printNumber(cnt);
    // write(STDOUT_FILENO,"HATZ\n",5);
    // cnt++;
  }
  initPipes();

  // for(int i = 0; i < PHIL_NO; i++){
  //   generatePipe(-1,i);
  // }
  //
  // for(int i = 0; i < PHIL_NO; i++){
  //   int a = waitRead(i,0);
  //   printNumber(a);
  //   writeLine(" ");
  //   printNumber(i);
  //   writeLine("\n");
  // }
  //
  // for(int i = 0; i < PHIL_NO; i++){
  //   int a = waitRead(i,0);
  //   printNumber(a);
  //   writeLine(" ");
  //   printNumber(i);
  //   writeLine("\n");
  // }
  //
  // for(int i = 0; i < PHIL_NO; i++){
  //   int a = waitRead(i,0);
  //   printNumber(a);
  //   writeLine(" ");
  //   printNumber(i);
  //   writeLine("\n");
  // }
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
