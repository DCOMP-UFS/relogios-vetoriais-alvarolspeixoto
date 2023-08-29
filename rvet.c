/**
 * Código base (incompleto) para implementação de relógios vetoriais.
 * Meta: implementar a interação entre três processos ilustrada na figura
 * da URL: 
 * 
 * https://people.cs.rutgers.edu/~pxk/417/notes/images/clocks-vector.png
 * 
 * Compilação: mpicc -o rvet rvet.c
 * Execução:   mpiexec -n 3 ./rvet
 */
 
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <mpi.h>     


typedef struct Clock { 
   int p[3];
} Clock;

int Max(int a, int b) {
   if (a > b) return a;
   else return b;
}

void Event(int pid, Clock *clock){
   clock->p[pid]++;
   printf("Process: %d, Clock: (%d, %d, %d)\n", pid, clock->p[0], clock->p[1], clock->p[2]);
}


void Send(int myid, int pid, Clock *clock){
   clock->p[myid]++;
   MPI_Send(clock, sizeof(Clock), MPI_BYTE, pid, 0, MPI_COMM_WORLD);
   printf("Process: %d, Clock: (%d, %d, %d)\n", myid, clock->p[0], clock->p[1], clock->p[2]);
}

void Receive(int myid, int pid, Clock *clock){
   clock->p[myid]++;
   Clock recv_clock;
   MPI_Recv(&recv_clock, sizeof(Clock), MPI_BYTE, pid, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
   clock->p[0] = Max(recv_clock.p[0], clock->p[0]);
   clock->p[1] = Max(recv_clock.p[1], clock->p[1]);
   clock->p[2] = Max(recv_clock.p[2], clock->p[2]);
   printf("Process: %d, Clock: (%d, %d, %d)\n", myid, clock->p[0], clock->p[1], clock->p[2]);

}

// Representa o processo de rank 0
void process0(){
   Clock clock = {{0,0,0}};
   Event(0, &clock);
   
   Send(0, 1, &clock);
   
   Receive(0, 1, &clock);
   
   Send(0, 2, &clock);
   
   Receive(0, 2, &clock);
   
   Send(0, 1, &clock);
   
   Event(0, &clock);
   
   printf("Final result - Process: %d, Clock: (%d, %d, %d)\n", 0, clock.p[0], clock.p[1], clock.p[2]);

}

// Representa o processo de rank 1
void process1(){
   Clock clock = {{0,0,0}};

   Send(1, 0, &clock);
   
   Receive(1, 0, &clock);
   
   Receive(1, 0, &clock);
   
   printf("Final result - Process: %d, Clock: (%d, %d, %d)\n", 1, clock.p[0], clock.p[1], clock.p[2]);
}

// Representa o processo de rank 2
void process2(){
   Clock clock = {{0,0,0}};
   
   Event(2, &clock);
   
   Send(2, 0, &clock);
   
   Receive(2, 0, &clock);
   
   printf("Final result - Process: %d, Clock: (%d, %d, %d)\n", 2, clock.p[0], clock.p[1], clock.p[2]);
   
}

int main(void) {
   int my_rank;               

   MPI_Init(NULL, NULL); 
   MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); 

   if (my_rank == 0) { 
      process0();
   } else if (my_rank == 1) {  
      process1();
   } else if (my_rank == 2) {  
      process2();
   }

   /* Finaliza MPI */
   MPI_Finalize(); 

   return 0;
}  /* main */
