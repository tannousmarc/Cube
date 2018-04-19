/* Copyright (C) 2017 Daniel Page <csdsp@bristol.ac.uk>
*
* Use of this source code is restricted per the CC BY-NC-ND license, a copy of
* which can be found via http://creativecommons.org (and should be included as
* LICENSE.txt within the associated archive or repository).
*/

#include "hilevel.h"
#include "libc.h"
#include "GUI.h"

#include <stdlib.h>

extern void     main_philo();
extern void     main_waiter();
extern void     main_console();
extern uint32_t tos_console;
extern void     main_P3();
extern uint32_t tos_P3;
extern void     main_P4();
extern uint32_t tos_P4;
extern void     main_P5();
extern uint32_t tos_P5;
extern uint32_t tos_PX;

pcb_t pcb[ 256 ];
pipe_t pipe[ 256 ];
int clocks[ 256 ];

// Scheduler utils
int executing = 0;
int next = 0;

// GUI utils
int selected = 0;
int level = 0;

// Counters for serial initialisation
int processes = -1;
int pipes     = -1;

// Flags
int philInit = 0;
int toggleDash = true;

// Retrieves keyboard input from launch-qemu terminal
void getQEMU( char* x, int n ) {
  for( int i = 0; i < n; i++ ) {
    x[ i ] = PL011_getc( UART0, true );

    if( x[ i ] == '\x0A' ) {
      x[ i ] = '\x00'; break;
    }
  }
}

// PC to Process Name
char* getName(uint32_t main){
  if(main == (uint32_t)main_console) return "CNSL";
  else if(main == (uint32_t)main_P3) return "PRG3";
  else if(main == (uint32_t)main_P4) return "PRG4";
  else if(main == (uint32_t)main_P5) return "PRG5";
  else if(main == (uint32_t)main_philo) return "PHIL";
  else if(main == (uint32_t)main_waiter) return "DOOR";
  return "";
}


void generateProcess(uint32_t main, int priority){
  processes++;
  memset( &pcb[ processes ], 0, sizeof( pcb_t ) );
  pcb[processes].name = getName(main);

  pcb[ processes ].pid          = processes;
  pcb[ processes ].status       = STATUS_READY;
  pcb[ processes ].priority     = priority;
  pcb[ processes ].ctx.cpsr     = 0x50;
  pcb[ processes ].ctx.pc       = ( uint32_t )( main );
  pcb[ processes ].ctx.sp       = ( uint32_t )( &tos_PX + 0x000010000*processes );
}

void timer(){
  TIMER0->Timer1Load  = 0x00020000; // select period = 2^20 ticks ~= 1 sec
  TIMER0->Timer1Ctrl  = 0x00000002; // select 32-bit   timer
  TIMER0->Timer1Ctrl |= 0x00000040; // select periodic timer
  TIMER0->Timer1Ctrl |= 0x00000020; // enable          timer interrupt
  TIMER0->Timer1Ctrl |= 0x00000080; // enable          timer

  GICC0->PMR          = 0x000000F0; // unmask all            interrupts
  GICD0->ISENABLER1  |= 0x00000010; // enable timer          interrupt
  GICC0->CTLR         = 0x00000001; // enable GIC interface
  GICD0->CTLR         = 0x00000001; // enable GIC distributor
}

// Generates pipes to be used by philosophers
void initPipes(){
  // These are used for sending timer data (for eat and think)
  for(int i = 0; i < PHIL_NO; i++){
    generatePipe(-2,i);
  }
  // These are used for sending fork status
  for(int i = 0; i < PHIL_NO; i++){
    generatePipe(-3, i);
    writePipe(PHIL_NO + i, 0, 0);
  }
  // Philosophers have been initialised, set flag
  philInit = 1;
}

void scheduler( ctx_t* ctx ) {
  int bestPriority = 0;
  next = executing;
  // find next
  for(int i = 0; i <= processes; i++){
    if(pcb[i].priority > bestPriority && pcb[i].status == STATUS_READY){
      bestPriority = pcb[i].priority;
      next = i;
    }
  }
  memcpy( &pcb [ executing ].ctx, ctx, sizeof( ctx_t )); // preserve currently executing
  if(pcb[ executing ].status != STATUS_TERMINATED){
    pcb[ executing ].status = STATUS_READY;
  }
  memcpy( ctx, &pcb[ next ].ctx, sizeof( ctx_t ) ); // restore next process
  pcb[ next ].status = STATUS_EXECUTING;
  executing = next;
  return;
}

void handleInput(char* string, ctx_t ctx){
  if(strcmp(string," ") == 0){
    // EXECUTE
    if(level == 0){
      switch(selected){
        case 0: {
          generateProcess((uint32_t) main_P3, 1);
        break;
        }
        case 1: {
          generateProcess((uint32_t) main_P4, 1);
        break;
        }
        case 2: {
          generateProcess((uint32_t) main_P5, 1);
        break;
        }
        case 3: {
          generateProcess((uint32_t) main_waiter, 1);
        break;
        }
      }
    }
    // RESET
    else if(level == 1 && selected >= 2){
      pcb[0].priority = 0;
      for(int i = 1; i <= processes; i++){
        pcb[ processes ].status       = STATUS_TERMINATED;
        pcb[ processes ].priority     = 0;
      }
      for(int i = 0; i <= pipes; i++){
        writePipe(i, 0, -1);
      }
      for(int i = 0; i <= PHIL_NO; i++){
        clocks[i] = 0;
      }
      processes = 0;
      pipes = -1;
      executing = 0;
      next = 0;
      scheduler(&ctx);
    }
    // TERMINATE
    else if(level == 1 && selected < 2){
      for(int i = 1; i <= processes; i++){
        kill( i, 0 );
      }
    }
    // TOGGLE DASH
    else if(level == 2){
      toggleDashboard();
    }
  }
  // HANDLE ARROW KEYS
  else if(strcmp(string,"w") == 0){
    if(level == 0)
      level = 2;
    else level--;
  }
  else if(strcmp(string,"s") == 0){
    level = (level + 1) % 3;
  }
  else if(strcmp(string,"a") == 0 || strcmp(string,"a ") == 0){
    if(level == 1 && selected > 1)
      selected = 1;
    else if(level == 1 && selected <= 1)
      selected = 3;
    else if(selected == 0)
      selected = 3;

    else selected--;
    string = "";
  }
  else if(strcmp(string,"d") == 0 || strcmp(string,"d ") == 0){
    if(level == 1 && selected < 2)
      selected = 2;
    else if(level == 1 && selected >= 2)
      selected = 0;

    selected =(selected + 1) % 4;
    string = "";
  }
}

void hilevel_handler_irq(ctx_t* ctx){
  // read interrupt id
  uint32_t id = GICC0->IAR;

  // handle interrupt, then reset
  if( id == GIC_SOURCE_TIMER0 ) {
    char* string = "";
    getQEMU(string, 1);
    handleInput(string, *ctx);

    // display GUI
    write(STDOUT_FILENO, "\033c", 4);
    if(toggleDash)
    dashboard();
    options(selected, level, toggleDash);

    if(philInit){
      // tick down philosopher timers
      for(int i = 0; i < PHIL_NO; i++){
        clocks[i]--;
        if(clocks[i] == 0){
          waitWrite(i,0,0);
        }
      }
    }

    // age/deage processes
    for(int i = 0; i <= processes; i++){

      if(pcb[i].status == STATUS_READY
        && pcb[i].priority < MAX_PRIORITY){
          pcb[i].priority++;
        }

        // make sure no king rules forever
        else if(pcb[i].priority >= MAX_PRIORITY
          && pcb[i].status == STATUS_EXECUTING){
            pcb[i].priority -= processes;
            if(pcb[i].priority < 0)
            pcb[i].priority = 0;
          }

        }

        scheduler(ctx);
        TIMER0->Timer1IntClr = 0x01;
      }

      // write the interrupt identifier to signal we're done.
      GICC0->EOIR = id;

      return;
    }

    void hilevel_handler_rst( ctx_t* ctx              ) {
      // initialise console process
      generateProcess((uint32_t) main_console, 1);

      // execute console
      memcpy( ctx, &pcb[ 0 ].ctx, sizeof( ctx_t ) );
      pcb[ 0 ].status = STATUS_EXECUTING;
      executing = 0;

      timer();
      int_enable_irq();

      return;
    }

    void hilevel_handler_svc( ctx_t* ctx, uint32_t id ) {

      switch( id ) {
        case 0x00 : { // 0x00 => yield()

          break;
        }

        case 0x01 : { // 0x01 => write( fd, x, n )
          int   fd = ( int   )( ctx->gpr[ 0 ] );
          char*  x = ( char* )( ctx->gpr[ 1 ] );
          int    n = ( int   )( ctx->gpr[ 2 ] );

          for( int i = 0; i < n; i++ ) {
            PL011_putc( UART0, *x++, true );
          }

          ctx->gpr[ 0 ] = n;
          break;
        }

        case 0x03 :{ // FORK
          processes++;
          memset( &pcb[processes], 0, sizeof(pcb_t) );
          pcb[ processes ].pid      = processes;
          pcb[ processes ].name     = pcb[executing].name;
          pcb[ processes ].priority = 1;
          memcpy( &pcb[processes].ctx, ctx, sizeof(ctx_t) );

          // Make sure the top of stack offsets are equal
          uint32_t childTos = ( uint32_t )( &tos_PX + processes*0x000010000 );
          uint32_t parentTos = ( uint32_t )(&tos_PX + (pcb[executing].pid*0x000010000));
          pcb[processes].ctx.sp   = childTos + (pcb[executing].ctx.sp - parentTos);

          // copy stack
          memcpy(&childTos,&parentTos,4);

          pcb[processes].status   = STATUS_READY;

          // return child pid to parent and 0 to child
          pcb[executing].ctx.gpr[0] = pcb[processes].pid;
          pcb[processes].ctx.gpr[0] = 0;
          break;
        }
        case 0x04 :{ // EXIT
          pcb[executing].status = STATUS_TERMINATED;
          pcb[executing].priority = -1;
          scheduler(ctx);
          break;
        }
        case 0x05 :{ // EXEC
          pcb[processes].ctx.pc = ctx -> gpr[0];
          pcb[processes].name = getName(ctx -> gpr[0]);
          break;
        }
        case 0x06 :{ // KILL
          int lookingFor = ctx -> gpr[0];
          for(int i = 0; i <= processes; i++){
            if(pcb[i].pid == lookingFor){
              pcb[ i ].status = STATUS_TERMINATED;
              pcb[ i ].priority = -1;
            }
          }
          break;
        }
        case 0x10 :{ // CLEAR SCREEN
          write(STDOUT_FILENO, "\033c", 4);
          break;
        }
        case 0x11 :{ // DASHBOARD
          displayDashboard(processes, pcb);
          break;
        }

        case 0x12 :{ // TOGGLE DASHBOARD
          toggleDash = !toggleDash;
          write(STDOUT_FILENO, "\033c", 4);
          write(STDOUT_FILENO, "\033[0m", 5);
          break;
        }
        case 0x13 :{ // get dash status
          ctx->gpr[0] = toggleDash;
          break;
        }
        case 0x20:{ // LET'S BUILD A PIPE
        pipes++;
        pipe[pipes].player1 = ctx -> gpr[0];
        pipe[pipes].player2 = ctx -> gpr[1];
        pipe[pipes].write   = -1;
        pipe[pipes].read    = -1;
        break;
      }
      case 0x21:{ // READ FROM A PIPE
        int id = ctx->gpr[0];
        int direction = ctx->gpr[1];
        // 0 = read, 1 = write
        if(direction == 0){
          ctx->gpr[0] = pipe[id].read;
        }
        else if(direction == 1){
          ctx->gpr[0] = pipe[id].write;
        }
        break;
      }
      case 0x22:{ // WRITE ON A PIPE
        int id = ctx->gpr[0];
        int direction = ctx->gpr[1];
        int data = ctx->gpr[2];
        // 0 = read, 1 = write
        if(direction == 0)
          pipe[id].read = data;
        else if(direction == 1)
          pipe[id].write = data;
        break;
      }
      case 0x25:{ // RUN PHILOSOPHERS
        generateProcess((uint32_t) main_waiter, 1);
        break;
      }
      case 0x26:{ // GET PHILOSOPHERS ID
        int curr = 0;
        for(int i = 0; i <= processes; i++){
          if(i == executing){
            ctx->gpr[0] = curr;
            break;
          }
          if(0 == strcmp( pcb[i].name, "PHIL" ))
          curr++;
        }
        break;
      }
      case 0x27:{ // GET PHILOSOPHERS NO
        int curr = 0;
        for(int i = 0; i <= processes; i++){
          if(0 == strcmp( pcb[i].name, "PHIL" ) && pcb[i].status != STATUS_TERMINATED)
          curr++;
        }
        ctx->gpr[0] = curr;
        break;
      }
      case 0x28:{ // RESET A CLOCK
        int curr = ctx->gpr[0];
        int value = rand() % 5 + 1;
        clocks[curr] = value*value;
      }
      default   : { // 0x?? => unknown/unsupported
        break;
      }
    }

    return;
  }
