#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <unistd.h>

union semun {
  int val;
  struct semid_ds *buf;
  unsigned short *array;
#if defined(__linux__)
  struct seminfo *__buf;
#endif // __linux__
};

static int set_semval(void);
static void del_semval(void);
static int semp(void);
static int semv(void);

static int sem_id;

int main(int argc, char *argv[])
{
    int i;
    int ptime;
    char opch = 'o';
    srand((unsigned int)getpid());

    sem_id = semget((key_t)1234, 1, 0666|IPC_CREAT);
    if(argc > 1)
    {
        if(!set_semval())
        {
            fprintf(stderr, "Failed to initialize semaphore\n");
            exit(EXIT_FAILURE);
        }
        opch = 'x';
        sleep(2);
    }

    for(int i=0; i<10; i++)
    {
        if(!semp())
            exit(1);
        printf("%c", opch);
        fflush(stdout);
        ptime = rand() % 3;
        sleep(ptime);
        printf("%c", opch);
        fflush(stdout);

        if(!semv())
            exit(1);
        ptime = rand() % 2;
        sleep(ptime);
    }

    printf("\n%d - finished\n", getpid());

    if(argc > 1)
    {
        sleep(10);
        del_semval();
    }
    exit(0);
}

static int set_semval(void)
{
    union semun sem_union;
    sem_union.val = 1;
    if(semctl(sem_id, 0, SETVAL, sem_union) == -1)
        return 0;
    return 1;
}
static void del_semval(void)
{
    union semun sem_union;
    if(semctl(sem_id, 0, IPC_RMID, sem_union) == -1)
        fprintf(stderr, "Failed to delete semaphore\n");
}
static int semp(void)
{
    struct sembuf semb;

    semb.sem_num = 0;
    semb.sem_op = -1;
    semb.sem_flg = SEM_UNDO;
    if(semop(sem_id, &semb, 1) == -1)
    {
        fprintf(stderr, "semaphore_p failed\n");
        return 0;
    }
    return 1;
}
static int semv(void)
{
    struct sembuf semb;

    semb.sem_num = 0;
    semb.sem_op = 1;
    semb.sem_flg = SEM_UNDO;
    if(semop(sem_id, &semb, 1) == -1)
    {
        fprintf(stderr, "semaphore_v failed\n");
        return 0;
    }
    return 1;
}

