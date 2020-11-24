#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
// 递归打印目录信息
void print_dir(char *dir, int depth);
// 打印具体信息
void print_info(struct stat *statbuf, char *filename, int depth);

int main(int argc, char *argv[])
{
    char dir[257];
    // 只有一个参数，打印当前目录
    if(argc < 2)
    {
        strcpy(dir, ".");
    }
    else 
    {
        strcpy(dir, argv[1]);
    }
    int depth = 0;
    print_dir(dir, depth);
    return 0;
}


void print_dir(char *dir, int depth)
{
    DIR *dp;
    // 目录项结构
    struct dirent *entry;
    // 文件项信息
    struct stat statbuf;
    
    if((dp = opendir(dir)) == NULL)
    {
        // fprintf(stderr, "opendir error\n");
        perror("1");
        return;
        // exit(EXIT_FAILURE);
    }
    // 设置dir为当前目录
    chdir(dir);

    while((entry = readdir(dp)) != NULL)
    {
        // 如果读取到符号链接则读取符号链接本身的信息
        lstat(entry->d_name, &statbuf);  
        // 如果是目录则打印当前目录信息，然后递归打印下一个目录的信息
        if(S_ISDIR(statbuf.st_mode))
        {
            if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            // 如果不是.或者.. 则打印目录信息
            print_info(&statbuf, entry->d_name, depth);

            // 新目录名为当前目录名/子目录名
            char *new_dir=(char *)malloc(strlen(dir)+strlen(entry->d_name)+2);
            strcpy(new_dir,dir);
            // 如果当前目录最后一个字符不是'/'则需要加上一个'/'
            if(dir[strlen(dir)-1]!='/'){
                strcat(new_dir,"/");
            }
            strcat(new_dir,entry->d_name);

            print_dir(new_dir, depth + 4);
            free(new_dir);
        }
        // 不是目录则直接打印文件信息
        else
            print_info(&statbuf, entry->d_name, depth);
    }
    // 返回父目录
    chdir("..");
    // 关闭目录
    closedir(dp);
    
}


void print_info(struct stat *statbuffer, char *filename, int depth)
{
    int i = 0;
    // 打印缩进
    for(i = 0; i < depth / 4 ; i++)
        printf("\t");
    struct stat statbuf = *statbuffer;
    // 打印具体信息
    // 1.打印文件类型
    if(S_ISDIR(statbuf.st_mode))
        printf("d");
    else if(S_ISLNK(statbuf.st_mode))
        printf("l");
    else if(S_ISFIFO(statbuf.st_mode))
        printf("p");
    else
        printf("-");
    // 2.打印权限(user-group-other)
    //           rwx  rwx   rwx
    if((S_IRUSR & statbuf.st_mode) == S_IRUSR)
        printf("r");
    else printf("-");
    if((S_IWUSR & statbuf.st_mode) == S_IWUSR)
        printf("w");
    else printf("-");
    if((S_IXUSR & statbuf.st_mode) == S_IXUSR)
        printf("x");
    else printf("-");

    if((S_IRGRP & statbuf.st_mode) == S_IRGRP)
        printf("r");
    else printf("-");
    if((S_IRGRP & statbuf.st_mode) == S_IRGRP)
        printf("w");
    else printf("-");
    if((S_IRGRP & statbuf.st_mode) == S_IRGRP)
        printf("x");
    else printf("-");

    if((S_IROTH & statbuf.st_mode) == S_IROTH)
        printf("r");
    else printf("-");
    if((S_IROTH & statbuf.st_mode) == S_IROTH)
        printf("w");
    else printf("-");
    if((S_IROTH & statbuf.st_mode) == S_IROTH)
        printf("x");
    else printf("-");

    printf(" ");

    // 3.打印文件硬链接数量
    printf("%ld ", statbuf.st_nlink);

    // 4.打印文件拥有着和所属组名
    // 存放用户信息的结构
    struct passwd *user;
    // 存放组信息的结构
    struct group *grp;
    user = getpwuid(statbuf.st_uid);
    if(user != NULL)
        printf("%s ", user->pw_name);
    grp = getgrgid(statbuf.st_gid);
    if(grp != NULL)
        printf("%s ", grp->gr_name);

    // 5.打印文件大小
    printf("%d ", (int)statbuf.st_size);

    // 6.打印文件创建时间
    char timebuf[128];
    struct tm time;
    // 设置时区
    tzset();
    // 转化为本地时间
    localtime_r(&(statbuf.st_mtime), &time);
    strftime(timebuf, 128, "%Y-%m-%d %H:%M", &time);
    printf("%s ", timebuf);

    // 7.打印文件名
    printf("%s ", filename);

    // 8.打印深度
    printf("%d", depth);
    printf("\n");

}