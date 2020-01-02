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
  if (*data){
    strcpy(data, "");
    shmdt(data);
  }
  //file
  fd = open("story.txt", O_CREAT | O_TRUNC, 0644);
  if (fd != -1){
    close(fd);
  }
}

void view_story(){
  printf("Viewing story...\n");
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
  view_story();
  printf("Removing story...\n");
  int semd, shmd;
  //remove semaphore
  semd = semget(SEM_KEY, 1, 0);
  struct sembuf sb;
  sb.sem_num = 0;
  sb.sem_op = -1;
  semop(semd, &sb, 1); //wait for semaphore to be available
  semctl(semd, IPC_RMID, 0);
  //remove shared memory
  shmd = shmget(MEM_KEY, SEG_SIZE, IPC_CREAT | 0644);
  shmctl(shmd, IPC_RMID, 0);
  //remove file
  remove("story.txt");
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
