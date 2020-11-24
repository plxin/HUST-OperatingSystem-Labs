#include "shared.h"

// 从文件读出数据，写入共享内存
int main()
{
    int i = 0;
    for(; i < 10; i++)
    {
        // 获取key的共享内存
        shmid[i] = shmget((key_t)(1000 + i), 0, 0);
        // 将共享内存映射到地址空间
        buffer[i] = (shared_buf *)shmat(shmid[i], NULL, 0);
    }
    // 获取信号灯
    semid = semget((key_t)1234, 0, 0);

    FILE *fp;
    fp = fopen("./test.txt", "rb");
    i = 0;
    // printf("Reading...\n");
    while(1)
    {
        // 是否可以写
        P(semid, 1);
        // fread返回size_t 对象，表示元素的总数
        printf("reading...\n");
        // 成功读取的元素总数会以 size_t 对象返回，size_t 对象是一个整型数据类型。如果总数与 nmemb 参数不同，则可能发生了一个错误或者到达了文件末尾。
        // fread返回成功读取的元素总数
        /*
            ptr -- 这是指向带有最小尺寸 size*nmemb 字节的内存块的指针。
            size -- 这是要读取的每个元素的大小，以字节为单位。
            nmemb -- 这是元素的个数，每个元素的大小为 size 字节。
            stream -- 这是指向 FILE 对象的指针，该 FILE 对象指定了一个输入流。
        */
        // buffer[i]->size存放从fp文件中读取的字节数
        buffer[i]->size = fread(buffer[i]->data, 1, BUF_LEN, fp);
        // 读完了就结束
        if(buffer[i]->size == 0)
        {
            V(semid, 0);
            break;
        }
        i = (++i) % 10;
        V(semid, 0);
    }
    fclose(fp);

    // 断开共享内存的链接
    for(i = 0; i < 10; i++)
        shmdt(buffer[i]);
    return 0;
}