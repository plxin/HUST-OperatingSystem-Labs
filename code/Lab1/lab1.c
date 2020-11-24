#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int child1(int *file_pipes);
int child2(int *file_pipes);
void handleSon1();
void handleSon2();
void handleParent();

pid_t son1;
pid_t son2;
int pipefd[2];
int cnt1 = 0;
int cnt2 = 0;

int main()
{
    char buffer[128];
    int cnt = 0;    // 记录信息总数
    int i=1;
    int status;
    signal(SIGINT, handleParent);
    if(pipe(pipefd) == 0)
    {
        son1 = fork();
        son2 = fork();

        if(son1 == -1)
        {
            printf("Fork failure!\n");
            exit(EXIT_FAILURE);
        }
        if(son1 == 0)
        {
            child1(pipefd);
        }
        
        if(son2 == -1)
        {
            printf("Fork failure!\n");
            exit(EXIT_FAILURE);
        }
        if(son2 == 0)
        {
            child2(pipefd);
        }
        // 父进程写入到管道
        if(son1 > 0 && son2 > 0)
        {
            close(pipefd[0]);
            while(1)
            {     
                sprintf(buffer, "I send you %d times.\n", i);
                write(pipefd[1], buffer, strlen(buffer));
                i++;
                sleep(1);   // 休眠1s
            }
            waitpid(son1, &status, 0);
            waitpid(son2, &status, 0);
            close(pipefd[0]);
            close(pipefd[1]);
            free(pipefd);
            printf("Parent total send %d times.\n", i);
            printf("parent process is killed.\n");
            exit(0);
        }

    }
    exit(1);
}   


int child1(int *file_pipes)
{
    signal(SIGINT, SIG_IGN);
    signal(SIGUSR1, handleSon1);
    char buffer1[128];
    close(file_pipes[1]);
    while(1)
    {
        read(file_pipes[0], buffer1, sizeof(buffer1));
        printf("%s", buffer1);
        cnt1++;
    }
    close(file_pipes[0]);
}
int child2(int *file_pipes)
{
    signal(SIGINT, SIG_IGN);    // 子进程忽略SIGINT
    signal(SIGUSR2, handleSon1); // 子进程接受SIGUSR2
    char buffer2[128];
    close(file_pipes[1]);
    while(1)
    {
        read(file_pipes[0], buffer2, sizeof(buffer2));
        printf("%s", buffer2);
        cnt2++;
    }
    close(file_pipes[0]);
}

void handleSon1(int sig)
{
    if(sig == SIGUSR1)
    {
        printf("son1 was killed by parent.\n");
        printf("son1 total received %d times from parent.\n", cnt1);
        exit(0);
    }
    else if(sig == SIGUSR2)
    {
        printf("son2 was killed by parent.\n");
        printf("son2 total received %d times from parent.\n", cnt2);
        exit(0);
    }
}
void handleParent(int sig)
{
    printf("\n");
    kill(son1, SIGUSR1);
    kill(son2, SIGUSR2);
}