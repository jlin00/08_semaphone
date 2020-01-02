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

int main() {
  int semd, shmd, fd;
  char *data;
  char input[SEG_SIZE];
  //check semaphore availability
  semd = semget(SEM_KEY, 1, 0);
  if (semd == -1){
    printf("%s\n", strerror(errno));
  }
  struct sembuf sb;
  sb.sem_num = 0;
  sb.sem_op = -1;
  semop(semd, &sb, 1);
  //access shared memory
  shmd = shmget(MEM_KEY, SEG_SIZE, 0);
  if (shmd == -1){
    printf("%s\n", strerror(errno));
  }
  data = shmat(shmd, 0, 0);
  //let user add to story
  printf("Last addition: %s\n", data); //access last addition from shared mem
  printf("Your addition: ");
  fgets(input, SEG_SIZE, stdin); //ask for user input
  strcpy(data, input);
  *strchr(data, '\n') = 0;
  shmdt(data); //update shared memory
  //update file
  fd = open("story.txt", O_WRONLY| O_APPEND, 0644);
  write(fd, input, strlen(input)); //add line to story
  close(fd);
  //done with semaphore
  sb.sem_op = 1;
  semop(semd, &sb, 1);
  return 0;
}
