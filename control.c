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

void create_story(){ //on hold, should create wipe contents?
  int semd, shmd, fd;
  char *data;
  semd = semget(SEM_KEY, 1, IPC_CREAT | IPC_EXCL | 0644);
  //semaphore
  if (semd != -1){ //new semaphore
    union semun us;
    us.val = 1;
    semctl(semd, 0, SETVAL, us);
    printf("semaphore created\n");
    //shared memory
    shmd = shmget(MEM_KEY, SEG_SIZE, IPC_CREAT | 0644);
    if (shmd == -1){
      printf("%s\n", strerror(errno));
    }
    data = shmat(shmd, 0, 0);
    if (errno != 0){
      printf("%s\n", strerror(errno));
    }
    printf("shared memory created\n");
    //file
    fd = open("story.txt", O_CREAT | O_TRUNC, 0644);
    if (fd != -1){
      printf("file created\n");
      close(fd);
    }
    else{
      printf("%s\n", strerror(errno));
    }
  }
}

void view_story(){
  printf("The story so far:\n");
  FILE * fp = fopen("story.txt", "r");
  if (errno != 0){
    printf("%s\n", strerror(errno));
  }
  char line[SEG_SIZE];
  while (fgets(line, SEG_SIZE, fp)){
    printf("%s", line);
  }
  fclose(fp);
}

void remove_story(){
  int semd, shmd;
  //remove semaphore
  printf("trying to get in\n");
  semd = semget(SEM_KEY, 1, 0);
  if (semd == -1){
    printf("%s\n", strerror(errno));
  }
  struct sembuf sb;
  sb.sem_num = 0;
  sb.sem_op = -1;
  semop(semd, &sb, 1); //wait for semaphore to be available
  if (errno != 0){
      printf("%s\n", strerror(errno));
  }
  view_story();
  //remove shared memory
  shmd = shmget(MEM_KEY, SEG_SIZE, 0);
  if (shmd == -1){
    printf("%s\n", strerror(errno));
  }
  shmctl(shmd, IPC_RMID, 0);
  if (errno != 0){
      printf("%s\n", strerror(errno));
  }
  printf("shared memory removed\n");
  //remove file
  remove("story.txt");
  if (errno != 0){
      printf("%s\n", strerror(errno));
  }
  printf("file removed\n");
  //remove semaphore
  semctl(semd, IPC_RMID, 0);
  if (errno != 0){
      printf("%s\n", strerror(errno));
  }
  printf("semaphore removed\n");
}

int main(int argc, char *argv[]){
  if (argc > 1){
    if (!strcmp(argv[1],"-c")){
      create_story();
    }
    if (!strcmp(argv[1],"-r")){
      remove_story();
    }
    if (!strcmp(argv[1],"-v")){
      view_story();
    }
  }
  return 0;
}
