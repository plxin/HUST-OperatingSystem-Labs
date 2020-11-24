#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/sem.h>
#include <unistd.h>

#define COMPUTE 0
#define PRINT_ODD 1
#define PRINT_EVEN 2
// semctl函数的第四个参数
union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

// 和
int sum = 0;

// 信号量
int semid;
// 循环变量
int i=1;
// 标志退出
int flag = 0;

void *compute();
void *print_odd();
void *print_even();

void P(int semid, int index);
void V(int semid, int index);

int main()
{
    // 0:compute  1:print_odd  2:print_even
    union semun semun0, semun1, semun2;
    // compute, print_odd, print_even信号灯的初值
    semun0.val = 1;
    semun1.val = 0;
    semun2.val = 0;

    // 创建一个信号量
    semid = semget(IPC_PRIVATE, 3, IPC_CREAT|0666);
    if(semid == -1)
        exit(1);

    if(semctl(semid, 0, SETVAL, semun0) == -1 || semctl(semid, 1, SETVAL, semun1) == -1 || semctl(semid, 2, SETVAL, semun2) == -1)
        exit(1);
    // 创建三个线程
    pthread_t p1, p2, p3;

    pthread_create(&p1, NULL, compute, NULL);
    pthread_create(&p2, NULL, print_odd, NULL);
    pthread_create(&p3, NULL, print_even, NULL);

    // wait
    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
    pthread_join(p3, NULL);

    // 删除信号量
    semctl(semid, IPC_RMID, 0);
    return 0;
}

/* compute */
void *compute()
{
    for(;i<=100;i++)
    {
        P(semid, COMPUTE);
        sum += i;
        // 偶数     
        if(sum % 2 == 0)
            V(semid, PRINT_EVEN);
        else V(semid, PRINT_ODD);
        
    }
    // 线程退出
    // 等打印完了再退出
    sleep(1);
    flag = 1;
    V(semid, PRINT_ODD);
    V(semid, PRINT_EVEN);
    // printf("i = %d\n", i);
    pthread_exit(NULL);
    return NULL;
}
/* 打印奇数 */
void *print_odd()
{
    while(1)
    {
        // printf("[debug]print_odd0\n");
        P(semid, PRINT_ODD);
        if(flag)
            break;
        printf("odd: %d\n", sum);
        V(semid, COMPUTE);
        // printf("[debug]print_odd1\n");
    }
    // 线程退出
    // printf("[debug]print_odd2\n");
    pthread_exit(NULL);
    return NULL;
}

/* 打印偶数 */
void *print_even()
{
    while(1)
    {
        // printf("[debug]print_even0\n");
        P(semid, PRINT_EVEN);
        if(flag)
            break;
        printf("even: %d\n", sum);
        V(semid, COMPUTE);
        // printf("[debug]print_even1\n");
    }
    // 线程退出
    // printf("[debug]print_even2\n");
    pthread_exit(NULL);
    return NULL;
}
/* P操作实现 */
void P(int id, int index)
{
    struct sembuf sem;
    sem.sem_num = index;
    sem.sem_op = -1;
    sem.sem_flg = SEM_UNDO;
    semop(id, &sem, 1);  // 1:表示执行命令的个数
}
/* V操作实现 */
void V(int id, int index)
{
    struct sembuf sem;
    sem.sem_num = index;
    sem.sem_op = 1;
    sem.sem_flg = SEM_UNDO;
    semop(id, &sem, 1);
}