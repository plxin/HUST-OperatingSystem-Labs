#include "shared.h"

// 从共享内存中读取数据然后往文件中写数据

int main()
{
    int i = 0;
    for(; i < 10; i++)
    {
        // 获取key的共享内存
        shmid[i] = shmget((key_t)(1000 + i), 0, 0);
        // 将共享内存映射到地址空间,返回指向共享内存第一个字节的指针
        buffer[i] = (shared_buf *)shmat(shmid[i], NULL, 0);
    }
    // 获取信号灯
    semid = semget((key_t)1234, 0, 0);

    FILE *fp;
    fp = fopen("./testCopy.txt", "wb");

    i = 0;
    // printf("Writing...\n");
    while(1)
    {
        // 
        P(semid, 0);
        printf("writing...\n");
        if(buffer[i]->size == 0)
        {
            V(semid, 1);
            break;
        }
        /*
            fwrite()
            第一个参数是buf的内存地址
            第二个参数是写多少个单位
            第三个参数是每个单位的大小
            第四个参数是fopen返回的文件指针
        */
        // 把buffer[i]中数据 写入fp指向的文件中
        // fwrite(buffer[i]->data, buffer[i]->size, 1, fp);
        // 以buffer[i]->size大小的数据为一个单位，每次写一个，可以执行
        fwrite(buffer[i]->data, 1, buffer[i]->size, fp);

        i = (++i) % 10;
        V(semid, 1);
    }
    fclose(fp);
    // 断开共享内存的链接
    for(i = 0; i < 10; i++)
        shmdt(buffer[i]);
    return 0;
}
