#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include "header.h"

#define INIT_SCHED() ({char var; top_stack = &var;})

#define NPROC_MAX 6
#define BEGIN_PROC 2
#define INIT_PROC 0
#define SCHED_PROC 1

#define SRUN 1
#define SVOID 0

char* top_stack;

int elu = BEGIN_PROC;

struct Tproc{
  char state;
  jmp_buf buf;
  char stack[64536];
  int stack_size;
} proc[NPROC_MAX];



int mysetjmp(int idx){
  char tmp;
  char* bottom = &tmp;
  proc[idx].stack_size = top_stack - bottom;

  memcpy(proc[idx].stack, 
	 bottom,
	 proc[idx].stack_size);
    
  if(setjmp(proc[idx].buf)==0){ 
    return 0;
  }
  else{
    memcpy(top_stack - proc[elu].stack_size,
	   proc[elu].stack,
	   proc[elu].stack_size);
    return 1;
  }
}


void mylongjmp(int idx){
  elu = idx;
  longjmp(proc[idx].buf, 1);
}

void f(int arg){
  int n = 0;
  while(n<6){
    printf("Execute processus num %d : n = %d \n",arg,n++);
    sleep(1);
  }
  return;
}


void g(int arg){
  int n = 0;
  while(n<10){
    printf("Execute processus num %d : n = %d \n",arg,n++);
    sleep(1);
  }
  return;
}



void new_proc(void( *fun)(int), int arg){

  sigset_t sig_proc;
  int i;

  for(i=BEGIN_PROC;i<NPROC_MAX && proc[i].state != SVOID; i++);
  
  if(i<NPROC_MAX){
    proc[i].state = SRUN;
    if(mysetjmp(i)==0){
      return;
    }
    else{
      fun(arg);

      sigemptyset(&sig_proc);
      sigaddset (&sig_proc, SIGALRM);
      sigprocmask(SIG_BLOCK,&sig_proc, NULL);

      printf("--------------------\n\n Tâche numéro %d terminé \n\n ---------------",arg);
      proc[i].state = SVOID;
      
      mylongjmp(SCHED_PROC);

    }
  }

  sigprocmask(SIG_UNBLOCK,&sig_proc, NULL);

}



int election(){

  int cpt = 0;
  int i = elu + 1;

  if(i>=NPROC_MAX){
    i=BEGIN_PROC;
  }

  while(i<=NPROC_MAX){
    if(proc[i].state == SRUN){
      return i;
    }
    else{
      i++;
      if(i>=NPROC_MAX){
	i = BEGIN_PROC;
      }
      cpt++;
      if(cpt == NPROC_MAX ){
	// Cas où l'on a plus de processus
	longjmp(proc[INIT_PROC].buf,1);
      }
    }
  }
}


void commut(int no){
  
  int p = election();  

  printf("\n Commutation :  election %d | courant %d \n\n",p,elu);
  
  signal(SIGALRM,&commut);
  alarm(3);
  
  if(mysetjmp(elu) == 0) {                                                                                                                         
    mylongjmp(p);
  }
  else{
    return;
  }
  
}                                                                                                                                                                   



void start_sched(){
  
  INIT_SCHED();
  
  // Mettre peut être le signal() après les new_proc selon la dernière question ? 
  signal(SIGALRM,&commut);
  
  if(mysetjmp(SCHED_PROC)==0){
    
    new_proc(f,BEGIN_PROC);
    new_proc(g,BEGIN_PROC+1);
    new_proc(f,BEGIN_PROC+2);
    new_proc(g,BEGIN_PROC+3);    


    alarm(1);
    int p = election();
    mylongjmp(p);
  }
  else{
    // Fin d'un processus    
    int p = election();
    mylongjmp(p);
  }
  
  
}


int main(int argc,char* argv[]){
    
  if(setjmp(proc[INIT_PROC].buf) == 0) {
    start_sched();
  }
  else {
    printf("\n\n\n ****************** Retour au main, fin du programme ***************** \n\n\n");
  }

  return EXIT_SUCCESS;
}
