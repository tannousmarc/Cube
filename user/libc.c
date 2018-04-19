/* Copyright (C) 2017 Daniel Page <csdsp@bristol.ac.uk>
 *
 * Use of this source code is restricted per the CC BY-NC-ND license, a copy of
 * which can be found via http://creativecommons.org (and should be included as
 * LICENSE.txt within the associated archive or repository).
 */

#include "libc.h"
void writeLine(char* line){
  int size = strlen(line);
  for( int i = 0; i < size; i++ )
     PL011_putc( UART0, *line++, true );
}

int waitRead(int id, int dir){
  int x = readPipe(id,dir);
  while(x == -1){
    x = readPipe(id,dir);
  }
  writePipe(id,dir,-1);
  return x;
}
void waitWrite(int id, int dir, int data){
  int x = readPipe(id,dir);
  while(x != -1){
    x = readPipe(id,dir);
  }
  writePipe(id, dir, data);
}
// used exclusively for debugging
void printDigit(int x){
  char* string;
  switch (x) {
    case 0 :  string ="0"; break;
    case 1 :  string ="1"; break;
    case 2 :  string ="2"; break;
    case 3 :  string ="3"; break;
    case 4 :  string ="4"; break;
    case 5 :  string ="5"; break;
    case 6 :  string ="6"; break;
    case 7 :  string ="7"; break;
    case 8 :  string ="8"; break;
    case 9 :  string ="9"; break;
  }
  write(STDOUT_FILENO, string, 1);
}
void printNumber(int x){
  if(x<0){
    write(STDOUT_FILENO, "--", 2);
  }
  else if(x>9){
    printDigit(x/10);
    printDigit(x - ((x/10)*10));
  }
  else{
    write(STDOUT_FILENO, " ", 1);
    printDigit(x);
  }
}
int  atoix( char* x        ) {
  char* p = x; bool s = false; int r = 0;

  if     ( *p == '-' ) {
    s =  true; p++;
  }
  else if( *p == '+' ) {
    s = false; p++;
  }

  for( int i = 0; *p != '\x00'; i++, p++ ) {
    r = s ? ( r * 10 ) - ( *p - '0' ) :
            ( r * 10 ) + ( *p - '0' ) ;
  }

  return r;
}

void itoax( char* r, int x ) {
  char* p = r; int t, n;

  if( x < 0 ) {
     p++; t = -x; n = t;
  }
  else {
          t = +x; n = t;
  }

  do {
     p++;                    n /= 10;
  } while( n );

    *p-- = '\x00';

  do {
    *p-- = '0' + ( t % 10 ); t /= 10;
  } while( t );

  if( x < 0 ) {
    *p-- = '-';
  }

  return;
}
void generatePipe(int p1, int p2){
  asm volatile( "mov r0, %1 \n" // assign r0 =  p1
                "mov r1, %2 \n" // assign r1 =  p2
                "svc %0     \n" // make system call PIPE_INIT
              :
              : "I" (PIPE_INIT), "r" (p1), "r" (p2)
              : "r0", "r1" );
  return;
}
int readPipe(int id, int direction){
  int r;
  asm volatile( "mov r0, %2 \n" // assign r0 =  id
                "mov r1, %3 \n" // assign r1 =  direction
                "svc %1     \n" // make system call PIPE_READ
                "mov %0, r0 \n" // assign r  = r0
              : "=r" (r)
              : "I" (PIPE_READ), "r" (id), "r" (direction)
              : "r0", "r1" );
  return r;
}
void writePipe(int id, int direction, int data){
  // write(STDOUT_FILENO,"AICIWRITE1",10);
  asm volatile( "mov r0, %1 \n" // assign r0 = id
                "mov r1, %2 \n" // assign r1 = direction
                "mov r2, %3 \n" // assign r2 = data
                "svc %0     \n" // make system call PIPE_WRITE
              :
              : "I" (PIPE_WRITE),  "r" (id), "r" (direction), "r" (data)
              : "r0", "r1", "r2" );
  // write(STDOUT_FILENO,"AICIWRITE2",10);
}

void runPhilo() {
  asm volatile( "svc %0     \n" // make system call RUN_PHILO
              :
              : "I" (RUN_PHILO)
              : );

  return;
}
int getDash() {
  int r;

  asm volatile( "svc %1     \n" // make system call GET_DASH
                "mov %0, r0 \n" // assign r  = r0
              : "=r" (r)
              : "I" (GET_DASH)
              : "r0");
  return r;
}
int getPhiloId() {
  int r;

  asm volatile( "svc %1     \n" // make system call SYS_WRITE
                "mov %0, r0 \n" // assign r  = r0
              : "=r" (r)
              : "I" (PHILO_ID)
              : "r0");
  return r;
}
int getPhiloNo() {
  int r;

  asm volatile( "svc %1     \n" // make system call SYS_WRITE
                "mov %0, r0 \n" // assign r  = r0
              : "=r" (r)
              : "I" (PHILO_NO)
              : "r0");
  return r;
}
void yield() {
  asm volatile( "svc %0     \n" // make system call SYS_YIELD
              :
              : "I" (SYS_YIELD)
              : );

  return;
}


void clear() {
  asm volatile( "svc %0     \n" // make system call SYS_CLEAR
              :
              : "I" (SYS_CLEAR)
              : );

  return;
}
void dashboard() {
  asm volatile( "svc %0     \n" // make system call SYS_DSHBR
              :
              : "I" (SYS_DSHBR)
              : );

  return;
}
void toggleDashboard() {
  asm volatile( "svc %0     \n" // make system call SYS_DSHBR
              :
              : "I" (TGL_DSHBR)
              : );
  return;
}
int write( int fd, const void* x, size_t n ) {
  int r;

  asm volatile( "mov r0, %2 \n" // assign r0 = fd
                "mov r1, %3 \n" // assign r1 =  x
                "mov r2, %4 \n" // assign r2 =  n
                "svc %1     \n" // make system call SYS_WRITE
                "mov %0, r0 \n" // assign r  = r0
              : "=r" (r)
              : "I" (SYS_WRITE), "r" (fd), "r" (x), "r" (n)
              : "r0", "r1", "r2" );

  return r;
}

int  read( int fd,       void* x, size_t n ) {
  int r;

  asm volatile( "mov r0, %2 \n" // assign r0 = fd
                "mov r1, %3 \n" // assign r1 =  x
                "mov r2, %4 \n" // assign r2 =  n
                "svc %1     \n" // make system call SYS_READ
                "mov %0, r0 \n" // assign r  = r0
              : "=r" (r)
              : "I" (SYS_READ),  "r" (fd), "r" (x), "r" (n)
              : "r0", "r1", "r2" );

  return r;
}

int  fork() {
  int r;

  asm volatile( "svc %1     \n" // make system call SYS_FORK
                "mov %0, r0 \n" // assign r  = r0
              : "=r" (r)
              : "I" (SYS_FORK)
              : "r0" );

  return r;
}

void exit( int x ) {
  asm volatile( "mov r0, %1 \n" // assign r0 =  x
                "svc %0     \n" // make system call SYS_EXIT
              :
              : "I" (SYS_EXIT), "r" (x)
              : "r0" );

  return;
}

void resetClock( int x ) {
  asm volatile( "mov r0, %1 \n" // assign r0 =  x
                "svc %0     \n" // make system call PHILO_CL
              :
              : "I" (PHILO_CL), "r" (x)
              : "r0" );

  return;
}


void exec( const void* x ) {
  asm volatile( "mov r0, %1 \n" // assign r0 = x
                "svc %0     \n" // make system call SYS_EXEC
              :
              : "I" (SYS_EXEC), "r" (x)
              : "r0" );

  return;
}

int  kill( int pid, int x ) {
  int r;

  asm volatile( "mov r0, %2 \n" // assign r0 =  pid
                "mov r1, %3 \n" // assign r1 =    x
                "svc %1     \n" // make system call SYS_KILL
                "mov %0, r0 \n" // assign r0 =    r
              : "=r" (r)
              : "I" (SYS_KILL), "r" (pid), "r" (x)
              : "r0", "r1" );

  return r;
}

void nice( int pid, int x ) {
  asm volatile( "mov r0, %1 \n" // assign r0 =  pid
                "mov r1, %2 \n" // assign r1 =    x
                "svc %0     \n" // make system call SYS_NICE
              :
              : "I" (SYS_NICE), "r" (pid), "r" (x)
              : "r0", "r1" );

  return;
}
