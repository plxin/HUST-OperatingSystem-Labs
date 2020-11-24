#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/sem.h>
#include <unistd.h>

// semctl函数的第四个参数
union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

// 总票数
int total = 10;

// 已售
int sold = 0;

// 
int semid;
union semun semun1;
void *subp0();
void *subp1();
void *subp2();

void P(int semid, int index);
void V(int semid, int index);

int main()
{
    // 互斥信号，初始值设为1
    semun1.val = 1;
    // 创建一个信号量
    semid = semget(IPC_PRIVATE, 1, IPC_CREAT|0666);
    if(semid == -1)
        exit(1);
    // 控制信号量信息， SETVAL用于将信号量初始化为一个已知的值semun1.val
    if(semctl(semid, 0, SETVAL, semun1) == -1)
        exit(1);
    // 创建 两个线程
    pthread_t p1, p2, p3;

    pthread_create(&p1, NULL, subp0, NULL);
    pthread_create(&p2, NULL, subp1, NULL);
    pthread_create(&p3, NULL, subp2, NULL);
    // wait
    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
    pthread_join(p3, NULL);

    // 删除信号灯
    semctl(semid, IPC_RMID, 0);
    printf("\nTotal sold %d\n", sold);
    exit(0);
}


void *subp0()
{
    int i = 0;
    while(1)
    {
        P(semid, 0);
        if(sold == total)
        {
            V(semid, 0);
            break;
        }
        i++;
        printf("thread1 sold %d\n", i);
        sold++;
        V(semid, 0);
    }
    printf("thread1 total sold %d\n", i);
    // 线程退出
    pthread_exit(NULL);
}

void *subp1()
{
    int j = 0;
    while(1)
    {
        P(semid, 0);
        if(sold == total)
        {
            V(semid, 0);
            break;
        }
            
        j++;
        printf("thread2 sold %d\n", j);
        sold++;
        V(semid, 0);
    }
    printf("thread2 total sold %d\n", j);
    // 线程退出
    pthread_exit(NULL);
}

void *subp2()
{
    int k = 0;
    while(1)
    {
        P(semid, 0);
        if(sold == total)
        {
            V(semid, 0);
            break;
        }
        k++;
        printf("thread3 sold %d\n", k);
        sold++;
        V(semid, 0);
    }
    printf("thread3 total sold %d\n", k);
    // 线程退出
    pthread_exit(NULL);
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