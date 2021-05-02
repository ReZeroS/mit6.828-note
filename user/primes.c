#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


#define STDDER_FILENO 2

#define READEND 0
#define WRITEEND 1

typedef int pid_t;
//https://blog.csdn.net/readlnh/article/details/102762315
//
// 父进程调用pipe函数创建管道，得到两个文件描述符fd[0]、fd[1]指向管道的读端和写端。
//
//父进程调用fork创建子进程，那么子进程也有两个文件描述符指向同一管道。
//
//父进程关闭管道读端，子进程关闭管道写端。父进程可以向管道中写入数据，子进程将管道中的数据读出。
// 由于管道是利用环形队列实现的，数据从写端流入管道，从读端流出，这样就实现了进程间通信。

int main(int argc, char *argv[]) {
    int numbers[36], fd[2];
    int i, index = 0;
    pid_t pid;
    for (i = 2; i <= 35; i++) {
        numbers[index++] = i;
    }
    while (index > 0) {
        pipe(fd);
        if ((pid = fork()) < 0) { // error detect
            fprintf(STDDER_FILENO, "fork error\n");
            exit(0);
        } else if (pid > 0) { // parent process
            close(fd[READEND]); // parent close reader end
            for (i = 0; i < index; i++) { // then write 2-35 to pipe
                write(fd[WRITEEND], &numbers[i], sizeof(numbers[i]));
            }
            close(fd[WRITEEND]); // close write end after write done
            // 如果所有指向管道写端的文件描述符都关闭了（管道写端引用计数为0），而仍然有进程从管道的读端读数据，
            // 那么管道中剩余的数据都被读取后，再次read会返回0，就像读到文件末尾一样。
            wait((int *) 0); // wait child response
            exit(0); // exit indicate success
        } else { // child process
            close(fd[WRITEEND]); // child close write end
            int prime = 0; // this process base prime
            int temp = 0;
            index = -1;

            // 如果有指向管道写端的文件描述符没关闭（管道写端引用计数大于0），
            // 而持有管道写端的进程也没有向管道中写数据，这时有进程从管道读端读数据，那么管道中剩余的数据都被读取后，
            // 再次read会阻塞，直到管道中有数据可读了才读取数据并返回
            // 这个循环第一次会直到读完35为止 因为父进程写完所有数后才执行close(fd[WRITEEND]) ，在关闭之前子进程read的话会阻塞
            while (read(fd[READEND], &temp, sizeof(temp)) != 0) { //then read one integer from pipe
                // the first number must be prime
                // printf("[index:%d] [temp:%d] [prime:%d]\n", index, temp, prime);
                if (index < 0) prime = temp, index++;
                else {
                    if (temp % prime != 0) {
                        numbers[index++] = temp;
                    }
                }
            }
            printf("prime %d\n", prime);
            // fork again until no prime
            close(fd[READEND]);
        }
    }
    exit(0);

}


//primes
//[index:-1] [temp:2] [prime:0]
//[index:0] [temp:3] [prime:2]
//[index:1] [temp:4] [prime:2]
//[index:1] [temp:5] [prime:2]
//[index:2] [temp:6] [prime:2]
//[index:2] [temp:7] [prime:2]
//[index:3] [temp:8] [prime:2]
//[index:3] [temp:9] [prime:2]
//[index:4] [temp:10] [prime:2]
//[index:4] [temp:11] [prime:2]
//[index:5] [temp:12] [prime:2]
//[index:5] [temp:13] [prime:2]
//[index:6] [temp:14] [prime:2]
//[index:6] [temp:15] [prime:2]
//[index:7] [temp:16] [prime:2]
//[index:7] [temp:17] [prime:2]
//[index:8] [temp:18] [prime:2]
//[index:8] [temp:19] [prime:2]
//[index:9] [temp:20] [prime:2]
//[index:9] [temp:21] [prime:2]
//[index:10] [temp:22] [prime:2]
//[index:10] [temp:23] [prime:2]
//[index:11] [temp:24] [prime:2]
//[index:11] [temp:25] [prime:2]
//[index:12] [temp:26] [prime:2]
//[index:12] [temp:27] [prime:2]
//[index:13] [temp:28] [prime:2]
//[index:13] [temp:29] [prime:2]
//[index:14] [temp:30] [prime:2]
//[index:14] [temp:31] [prime:2]
//[index:15] [temp:32] [prime:2]
//[index:15] [temp:33] [prime:2]
//[index:16] [temp:34] [prime:2]
//[index:16] [temp:35] [prime:2]
//prime 2
//[index:-1] [temp:3] [prime:0]
//[index:0] [temp:5] [prime:3]
//[index:1] [temp:7] [prime:3]
//[index:2] [temp:9] [prime:3]
//[index:2] [temp:11] [prime:3]
//[index:3] [temp:13] [prime:3]
//[index:4] [temp:15] [prime:3]
//[index:4] [temp:17] [prime:3]
//[index:5] [temp:19] [prime:3]
//[index:6] [temp:21] [prime:3]
//[index:6] [temp:23] [prime:3]
//[index:7] [temp:25] [prime:3]
//[index:8] [temp:27] [prime:3]
//[index:8] [temp:29] [prime:3]
//[index:9] [temp:31] [prime:3]
//[index:10] [temp:33] [prime:3]
//[index:10] [temp:35] [prime:3]
//prime 3
//[index:-1] [temp:5] [prime:0]
//[index:0] [temp:7] [prime:5]
//[index:1] [temp:11] [prime:5]
//[index:2] [temp:13] [prime:5]
//[index:3] [temp:17] [prime:5]
//[index:4] [temp:19] [prime:5]
//[index:5] [temp:23] [prime:5]
//[index:6] [temp:25] [prime:5]
//[index:6] [temp:29] [prime:5]
//[index:7] [temp:31] [prime:5]
//[index:8] [temp:35] [prime:5]
//prime 5
//[index:-1] [temp:7] [prime:0]
//[index:0] [temp:11] [prime:7]
//[index:1] [temp:13] [prime:7]
//[index:2] [temp:17] [prime:7]
//[index:3] [temp:19] [prime:7]
//[index:4] [temp:23] [prime:7]
//[index:5] [temp:29] [prime:7]
//[index:6] [temp:31] [prime:7]
//prime 7
//[index:-1] [temp:11] [prime:0]
//[index:0] [temp:13] [prime:11]
//[index:1] [temp:17] [prime:11]
//[index:2] [temp:19] [prime:11]
//[index:3] [temp:23] [prime:11]
//[index:4] [temp:29] [prime:11]
//[index:5] [temp:31] [prime:11]
//prime 11
//[index:-1] [temp:13] [prime:0]
//[index:0] [temp:17] [prime:13]
//[index:1] [temp:19] [prime:13]
//[index:2] [temp:23] [prime:13]
//[index:3] [temp:29] [prime:13]
//[index:4] [temp:31] [prime:13]
//prime 13
//[index:-1] [temp:17] [prime:0]
//[index:0] [temp:19] [prime:17]
//[index:1] [temp:23] [prime:17]
//[index:2] [temp:29] [prime:17]
//[index:3] [temp:31] [prime:17]
//prime 17
//[index:-1] [temp:19] [prime:0]
//[index:0] [temp:23] [prime:19]
//[index:1] [temp:29] [prime:19]
//[index:2] [temp:31] [prime:19]
//prime 19
//[index:-1] [temp:23] [prime:0]
//[index:0] [temp:29] [prime:23]
//[index:1] [temp:31] [prime:23]
//prime 23
//[index:-1] [temp:29] [prime:0]
//[index:0] [temp:31] [prime:29]
//prime 29
//[index:-1] [temp:31] [prime:0]
//prime 31
