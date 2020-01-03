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

union semun {
  int              val;    /* Value for SETVAL */
  struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
  unsigned short  *array;  /* Array for GETALL, SETALL */
  struct seminfo  *__buf;  /* Buffer for IPC_INFO
                              (Linux-specific) */
};

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

int main(int argc, char *argv[]){
  int semd, shmd, fd;
  char *data;
  if (argc > 1){
    //create story
    if (!strcmp(argv[1], "-c")){
      semd = semget(SEM_KEY, 1, IPC_CREAT | IPC_EXCL | 0644);
      //semaphore
      if (semd != -1){
        //create semaphore
        union semun us;
        us.val = 1;
        semctl(semd, 0, SETVAL, us);
        printf("semaphore created\n");
        //create shared memory
        shmd = shmget(MEM_KEY, SEG_SIZE, IPC_CREAT | 0644);
        if (shmd == -1){
          printf("%s\n", strerror(errno));
        }
        data = shmat(shmd, 0, 0);
        if (errno != 0){
          printf("%s\n", strerror(errno));
        }
        printf("shared memory created\n");
        //create file
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
    //view story
    if (!strcmp(argv[1], "-v")){
      view_story();
    }
    //remove story
    if (!strcmp(argv[1],  "-r")){
      //remove semaphore
      semd = semget(SEM_KEY, 1, 0);
      if (semd == -1){
        printf("%s\n", strerror(errno));
      }
      struct sembuf sb;
      sb.sem_num = 0;
      sb.sem_op = -1;
      printf("trying to get in\n");
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
  }
  return 0;
}
