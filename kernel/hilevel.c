/* Copyright (C) 2017 Daniel Page <csdsp@bristol.ac.uk>
*
* Use of this source code is restricted per the CC BY-NC-ND license, a copy of
* which can be found via http://creativecommons.org (and should be included as
* LICENSE.txt within the associated archive or repository).
*/

#include "hilevel.h"
#include "libc.h"
/* Since we *know* there will be 2 processes, stemming from the 2 user
* programs, we can
*
* - allocate a fixed-size process table (of PCBs), and then maintain
*   an index into it for the currently executing process,
* - employ a fixed-case of round-robin scheduling: no more processes
*   can be created, and neither is able to terminate.
*/
#define MAX_PRIORITY (5)

extern void     main_console();
extern uint32_t tos_console;
extern void     main_P3();
extern uint32_t tos_P3;
extern void     main_P4();
extern uint32_t tos_P4;
extern void     main_P5();
extern uint32_t tos_P5;
extern uint32_t tos_PX;

pcb_t pcb[ 32 ];
int executing = 0;
int processes = -1;
int deadProcesses = 0;
int next = 0;

void timer(){
  TIMER0->Timer1Load  = 0x00100000; // select period = 2^20 ticks ~= 1 sec
  TIMER0->Timer1Ctrl  = 0x00000002; // select 32-bit   timer
  TIMER0->Timer1Ctrl |= 0x00000040; // select periodic timer
  TIMER0->Timer1Ctrl |= 0x00000020; // enable          timer interrupt
  TIMER0->Timer1Ctrl |= 0x00000080; // enable          timer

  GICC0->PMR          = 0x000000F0; // unmask all            interrupts
  GICD0->ISENABLER1  |= 0x00000010; // enable timer          interrupt
  GICC0->CTLR         = 0x00000001; // enable GIC interface
  GICD0->CTLR         = 0x00000001; // enable GIC distributor
}

void scheduler( ctx_t* ctx ) {
  // if(processes > 0){
  //   int aux = executing;
  //   do{
  //     aux++;
  //     next = aux % (processes + 1);
  //   }
  //   while(pcb[next].status == STATUS_TERMINATED);
  // }
  // else next = executing;
  //
  // memcpy( &pcb [ executing ].ctx, ctx, sizeof( ctx_t )); // preserve currently executing
  // if(pcb[ executing ].status != STATUS_TERMINATED){
  //   pcb[ executing ].status = STATUS_READY;
  // }
  // memcpy( ctx, &pcb[ next ].ctx, sizeof( ctx_t ) ); // restore next process
  // pcb[ next ].status = STATUS_EXECUTING;
  // executing = next;
  int bestPriority = 0;
  next = executing;
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



void hilevel_handler_irq(ctx_t* ctx){
  // read interrupt id
  uint32_t id = GICC0->IAR;

  // handle interrupt, then reset
  if( id == GIC_SOURCE_TIMER0 ) {
    PL011_putc( UART0, 'T', true );
    // increase priority of each process ready to be run
    for(int i = 0; i <= processes; i++){
      if(pcb[i].status == STATUS_READY
         && pcb[i].priority < MAX_PRIORITY){
        pcb[i].priority++;
        // printNumber(i);
        // PL011_putc( UART0, '=', true );
        // printNumber(pcb[i].priority);
      }

      // make sure no 👑 rules forever
      else if(pcb[i].priority == MAX_PRIORITY
              && pcb[i].status == STATUS_EXECUTING){
        pcb[i].priority -= ( processes - deadProcesses );
      }
    }
    scheduler(ctx);
    TIMER0->Timer1IntClr = 0x01;
  }

  // write the interrupt identifier to signal we're done.
  GICC0->EOIR = id;

  return;
}
void generateProcess(uint32_t main, int priority){
  processes++;
  memset( &pcb[ processes ], 0, sizeof( pcb_t ) );

  pcb[ processes ].pid          = processes;
  pcb[ processes ].status       = STATUS_READY;
  pcb[ processes ].priority     = priority;
  pcb[ processes ].ctx.cpsr     = 0x50;
  pcb[ processes ].ctx.pc       = ( uint32_t )( main );
  pcb[ processes ].ctx.sp       = ( uint32_t )( &tos_PX + 0x00001000*processes );
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
      pcb[ processes ].priority = 1;
      memcpy( &pcb[processes].ctx, ctx, sizeof(ctx_t) );

      // Make sure the top of stack offsets are equal
      uint32_t childTos = ( uint32_t )( &tos_PX + processes*0x00001000 );
      uint32_t parentTos = ( uint32_t )(&tos_PX + (pcb[executing].pid*0x00001000));
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
      deadProcesses++;
      pcb[executing].status = STATUS_TERMINATED;
      scheduler(ctx);
      break;
    }
    case 0x05 :{ // EXEC
      pcb[processes].ctx.pc = ctx -> gpr[0];
      break;
    }
    case 0x06 :{ // KILL
      deadProcesses++;
      int lookingFor = ctx -> gpr[0];
      for(int i = 0; i <= processes; i++){
        if(pcb[i].pid == lookingFor){
          pcb[ i ].status = STATUS_TERMINATED;
        }
      }
      break;
    }
    case 0x10 :{ // CLEAR SCREEN
      write(STDOUT_FILENO, "\033c", 5);
      break;
    }
    default   : { // 0x?? => unknown/unsupported
      // write(STDOUT_FILENO, " UNKNH ", 7 );
      break;
    }
  }

  return;
}
