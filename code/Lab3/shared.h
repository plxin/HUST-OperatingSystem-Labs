#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
// 缓冲区长度
#define BUF_LEN 1024
// 缓冲区数量
#define BUF_NUM 10

// 信号灯定义
#define READ 0
#define WRITE 1
#define MUTEX 2


typedef struct shared_buf
{
    size_t size;           // 缓冲区大小
    char data[BUF_LEN];    // 缓冲区长度
} shared_buf;

// 共享内存标识
int shmid[10];
// 指向共享内存块
shared_buf *buffer[10];

// 信号灯
int semid = 0;

// P操作实现
void P(int id, int index)
{
    struct sembuf sem;
    sem.sem_num = index;
    sem.sem_op = -1;
    sem.sem_flg = SEM_UNDO;
    semop(id, &sem, 1);  // 1:表示执行命令的个数
    return;
}
// V操作实现
void V(int id, int index)
{
    struct sembuf sem;
    sem.sem_num = index;
    sem.sem_op = 1;
    sem.sem_flg = SEM_UNDO;
    semop(id, &sem, 1);
    return;
}