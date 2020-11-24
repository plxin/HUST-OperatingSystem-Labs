#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUF_SIZE 30

int pid1, pid2;

/*
 * pipe(int *pfd)
 * pfd[0]: read
 * pfd[1]: write
 */

/*
 * sigint_handler - handle SIGINT, kill child pid
 */
void sigint_handler(int sig) {
  printf("\n");
  kill(pid1, SIGUSR1);
  kill(pid2, SIGUSR2);
}

/*
 * sigusr_handler - handle SIGUER1/2, end child pid
 */
void sigusr_handler(int sig) {
  if (sig == SIGUSR1) {
    printf("Child Process 1 is Killed by Parent!\n");
    exit(0);
  }
  else if (sig == SIGUSR2) {
    printf("Child Process 2 is Killed by Parent!\n");
    exit(0);
  }
}

int main(void) {
  int pfd[2];
  char read_buf[BUF_SIZE];
  int read_num;
  int wait_tmp;
  signal(SIGINT, sigint_handler);

  if (pipe(pfd) == -1)
    printf("Pipe");
  
  switch (pid1 = fork()) {
    case -1:
      printf("First fork");
    
    /* Child 1 - write data */
    case 0:
      /* Ignore SIGINT */
      signal(SIGINT, SIG_IGN);
      /* Catch SIGUSR2 */
      signal(SIGUSR1, sigusr_handler);
      if (close(pfd[0]) == -1)
        printf("Child 1 close read fd");
      
      int count = 1;
      char write_buf[BUF_SIZE];
      for (;;) {
        sprintf(write_buf, "I send you %d times.\n", count);
        if (write(pfd[1], write_buf, strlen(write_buf)) != strlen(write_buf))
          printf("Child 1 write");
        sleep(1);
        count++;
      }
    
    /* Parent pid */
    default:
      switch (pid2 = fork()) {
        case -1:
          printf("Second fork");
        
        /* Child 2 - read data */
        case 0:
          /* Ignore SIGINT */
          signal(SIGINT, SIG_IGN);
          /* Catch SIGUSR2 */
          signal(SIGUSR2, sigusr_handler);
          if (close(pfd[1] == -1))
            printf("Child 2 close write fd");
          
          for (;;) {
            read_num = read(pfd[0], read_buf, BUF_SIZE);
            if (read_num == -1)
              printf("Child 2 read");
            if (read_num == 0)
              continue;
            if (write(STDOUT_FILENO, read_buf, read_num) != read_num)
              printf("Child 2 write");
          }

        /* Parent pid */
        default:
          if (close(pfd[0]) == -1)
            printf("Parent close pfd[0]");
          if (close(pfd[1]) == -1)
            printf("Parent close pfd[1]");
          /* Wait for child pid */
          waitpid(pid1, &wait_tmp, 0);
          waitpid(pid1, &wait_tmp, 0);
          printf("Parent Process is Killed!\n");
          exit(0);
      }
  }
}