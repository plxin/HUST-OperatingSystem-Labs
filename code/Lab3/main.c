#include "shared.h"

int main()
{
    //创建两个信号灯。
    semid = semget((key_t)1234, 2, IPC_CREAT | 0666);

    // 信号灯0表示可读初值为0， 信号灯1表示可写即有空的缓存区初值为10，共享内存区的数量
    semctl(semid, 0, SETVAL, 0);
    semctl(semid, 1, SETVAL, 10);

    int i = 0;
    // 创建共享内存
    for(; i < 10; i++)
        shmid[i] = shmget(1000 + i, 1024, IPC_CREAT | 0666);
    
    pid_t get, put;
    if((get = fork()) == 0)
    {
        printf("Get created!\n");
        execl("./get", "get", NULL);
        printf("Get finished!\n");
        exit(EXIT_SUCCESS);
    }
    else
    {
        if((put = fork()) == 0)
        {
            printf("Put created!\n");
            execl("./put", "put", NULL);
            printf("Put finished!\n");
            exit(EXIT_SUCCESS);
        }
    }
    // 等待子进程结束
    waitpid(get, NULL, 0);
    waitpid(put, NULL, 0);
    // 删除信号灯
    semctl(semid, 0, IPC_RMID);
    // 删除共享内存
    for(i = 0; i < 10; i++)
        shmctl(shmid[i], IPC_RMID, 0);
    
    return 0;
    
}
