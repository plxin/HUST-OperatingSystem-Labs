#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFSIZE 128

pid_t son1;
pid_t son2;
int pipefd[2];
int i=1;
int cnt1 = 0;
int cnt2 = 0;
int flag1 = 0;
int flag2 = 0;
void handleSon1(int sig);
void handleParent(int sig);

int main()
{ 
    char buffer[BUFSIZE];
    memset(buffer, '\0', sizeof(buffer));
    
    int status;
    signal(SIGINT, handleParent);
    if(pipe(pipefd) == 0)
    {
        son1 = fork();
        switch(son1)
        {
        case -1:
            printf("fork failure\n");
            exit(1);
        case 0:
            // 子进程1
            signal(SIGINT, SIG_IGN);    // 忽略SIGINT
            signal(SIGUSR1, handleSon1);    // catchSIGUSR1
            char buffer1[BUFSIZE];
            int tmp1;   // 判断是否还在写
            close(pipefd[1]);
            while(1)
            {
                tmp1 = read(pipefd[0], buffer1, sizeof(buffer1));
                if(tmp1>0)
                {
                    printf("%s", buffer1);
                    cnt1++;
                }
                // 没有写就退出
                else
                {
                    printf("send over, child 1 is killed.\n");
                    printf("child 1 receiced %d times.\n", cnt1);
                    exit(0);
                } 
            }
        // 父进程
        default:
            son2 = fork();
            switch(son2)
            {
            case -1:
                printf("fork failure\n");
                exit(1);
            // 子进程2
            case 0:
                signal(SIGINT, SIG_IGN);        // 子进程忽略SIGINT
                signal(SIGUSR2, handleSon1);    // 子进程catchSIGUSR2
                char buffer2[BUFSIZE];
                close(pipefd[1]);               // 管道只支持单向通信
                int tmp;
                while(1)
                {
                    tmp = read(pipefd[0], buffer2, sizeof(buffer2));
                    if(tmp > 0)
                    {
                        
                        printf("%s", buffer2);
                        cnt2++;
                    }
                    // 没有写就退出
                    else 
                    {
                        printf("send over, child 2 is killed.\n");
                        printf("child 2 receiced %d times.\n", cnt2);
                        exit(0);
                    }
                }
            default:
                // 父进程
                close(pipefd[0]);
                while(i<11)
                {     
                    sprintf(buffer, "I send you %d times.\n", i);
                    write(pipefd[1], buffer, strlen(buffer));
                    i++;
                    sleep(1);   // 休眠1s
                }
                
            }
            
        }
        
    }
}


void handleSon1(int sig)
{
    if(sig == SIGUSR1)
    {
        printf("son1 was killed by parent.\n");
        printf("son1 total received %d times from parent.\n", cnt1);
        // son1 退出
        flag1 = 1;
        exit(0);
        
    }
    else if(sig == SIGUSR2)
    {
        printf("son2 was killed by parent.\n");
        printf("son2 total received %d times from parent.\n", cnt2);
        // son2 退出
        exit(0);
    }
}
void handleParent(int sig)
{
    printf("\n");
    kill(son1, SIGUSR1);
    kill(son2, SIGUSR2);
    close(pipefd[0]);
    close(pipefd[1]);
    waitpid(son1, NULL, 0);
    waitpid(son2, NULL, 0);
    printf("parent process is killed.\n");
    printf("Parent total send %d times.\n", i-1);
    exit(0);
}
