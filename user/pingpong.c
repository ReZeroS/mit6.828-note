#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


//
// 管道是半双工的，数据只能向一个方向流动
// 并且只存在于内存中
// 写入的内容每次都添加在管道缓冲区的末尾，并且每次都是从缓冲区的头部读出数据
// 只要两个进程中存在亲缘关系，这里的亲缘关系指 的是具有共同的祖先，都可以采用管道方式来进行通信
// 固定写死的，即一端只能用于读，由描述字fd[0]表示，称其为管道读端；另 一端则只能用于写，由描述字fd[1]来表示，称其为管道写端。
int main(int argc, char *argv[]) {
    int parent_fd[2], child_fd[2];
    pipe(parent_fd);
    pipe(child_fd);
    char buf[64];

    if (fork()) {
        // Parent
        write(parent_fd[1], "ping", strlen("ping"));
        read(child_fd[0], buf, 4);
        printf("%d: received %s\n", getpid(), buf);
    } else {
        // Child
        read(parent_fd[0], buf, 4);
        printf("%d: received %s\n", getpid(), buf);
        write(child_fd[1], "pong", strlen("pong"));
    }

    exit(0);
}
