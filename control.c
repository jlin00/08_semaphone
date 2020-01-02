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
#define SEG_SIZE 200

//union semun {
//  int              val;    /* Value for SETVAL */
//  struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
//  unsigned short  *array;  /* Array for GETALL, SETALL */
//  struct seminfo  *__buf;  /* Buffer for IPC_INFO
//                              (Linux-specific) */
//};

void create_story(){
  printf("Creating story...\n");
  int semd, shmd, fd;
  char *data;
  semd = semget(SEM_KEY, 1, IPC_CREAT | IPC_EXCL | 0644);
  //semaphore
  if (semd != -1){
    union semun us;
    us.val = 1;
    semctl(semd, 0, SETVAL, us);
  }
  //shared memory
  shmd = shmget(MEM_KEY, SEG_SIZE, IPC_CREAT | 0644);
  data = shmat(shmd, 0, 0);
  //file
  fd = open("story.txt", O_CREAT | O_TRUNC, 0644);
  if (fd != -1){
    close(fd);
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
