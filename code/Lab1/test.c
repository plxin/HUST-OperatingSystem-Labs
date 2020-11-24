#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

void myFunc(int sigNo)
{
    if(sigNo == SIGUSR1)
        printf("Receive SIGUSR1.\n");
    if(sigNo == SIGUSR2)
        printf("Receive SIGUSR2.\n");
    if(sigNo == SIGINT)
    {
        printf("Receive SIGINT.\n");
        exit(0);
    }
}



int main()
{
    if(signal(SIGUSR1, myFunc)==SIG_ERR)
        printf("can't catch SIGUSR1.\n");
    if(signal(SIGUSR2, myFunc)==SIG_ERR)
        printf("can't catch SIGUSR2.\n");
    if(signal(SIGINT, myFunc)==SIG_ERR)
        printf("can't catch SIGINT.\n");
    kill(getpid(), SIGINT);
    while(1);
    return 0;
}