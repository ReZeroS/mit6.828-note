#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"


//struct stat {
//    int dev;     // 文件系统设备号
//    uint ino;    // Inode 值
//    short type;  // 文件类型
//    short nlink; // 文件被链接数
//    uint64 size; // 文件大小
//};

#define STDDER_FILENO 2
#define O_RDONLY 0

char *fmtname(char *path) {
    static char buf[DIRSIZ + 1];
    char *p;

    // Find first character after last slash.
    for (p = path + strlen(path); p >= path && *p != '/'; p--);
    p++;

    // Return blank-padded name.
    if (strlen(p) >= DIRSIZ)
        return p;
    memmove(buf, p, strlen(p));
    buf[strlen(p)] = 0;
    return buf;
}


void find(char *path, char *file) {
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, O_RDONLY)) < 0) {
        fprintf(STDDER_FILENO, "ls: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0) {
        fprintf(STDDER_FILENO, "ls: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch (st.type) {
        case T_FILE:
            // printf("%s ### %s\n", path, file);
            if(!strcmp(fmtname(path), file)) {
                printf("%s\n", path);
            }
            break;

        case T_DIR:
            if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
                printf("ls: path too long\n");
                break;
            }
            // 添加路径符 /
            strcpy(buf, path);
            p = buf + strlen(buf);
            *p++ = '/';

            while (read(fd, &de, sizeof(de)) == sizeof(de)) {
                if (de.inum == 0)
                    continue;
                memmove(p, de.name, DIRSIZ);
                p[DIRSIZ] = 0;

                if (!strcmp(de.name, ".") || !strcmp(de.name, "..")) continue;

                // recursive call find
                find(buf, file);
            }
            break;
    }
    close(fd);
}

int
main(int argc, char *argv[]) {

    if (argc < 3) {
        printf("Error input arguments, should be find path fileName");
        exit(1);
    }
    find(argv[1], argv[2]);
    exit(0);
}
