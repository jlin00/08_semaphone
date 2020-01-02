#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/shm.h>

#define SEM_KEY 10282
#define MEM_KEY 24602

//union semun {
//  int              val;    /* Value for SETVAL */
//  struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
//  unsigned short  *array;  /* Array for GETALL, SETALL */
//  struct seminfo  *__buf;  /* Buffer for IPC_INFO
//                              (Linux-specific) */
//};

void create_story(){
  printf("Creating story...\n");
  int semd;
  int v, r;
  semd = semget(SEM_KEY, 1, IPC_CREAT | IPC_EXCL | 0644);
  if (semd == -1){ //already exists
    semd = semget(SEM_KEY, 1, 0);
    v = semctl(semd, 0, GETVAL, 0);
    printf("Story has already been created!\n");
  }
  else {
    //semaphore
    union semun us;
    us.val = 1;
    r = semctl(semd, 0, SETVAL, us);
    //shared memory

    //file
    
    printf("Story successfully created!\n");
  }
}

void remove_story(){
  printf("Removing story...\n");
}

void view_story(){
  printf("Viewing story...\n");
}

int main(int argc, char *argv[]) {
  if (argc > 1){
    if (!strcmp(argv[1],"-c")){
      create_story();
    }
    else if (!strcmp(argv[1],"-r")){
      remove_story();
    }
    else if (!strcmp(argv[1],"-v")){
      view_story();
    }
    else {
      printf("Please run ./control with -c, -r, or -v.\n");
    }
  }
  else {
    printf("Please run ./control with -c, -r, or -v.\n");
  }
  return 0;
}
