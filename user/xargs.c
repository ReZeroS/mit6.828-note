#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

#define STDIN_FILENO 0
#define MAX_LINE_LENGTH 1024

// 0< 是标准输入（stdin） 1> 是标准输出（stdout），2> 是标准错误输出（stderr）

int main(int argc, char *argv[]){

    // 读一行命令最多的字符
    char line[MAX_LINE_LENGTH];
    // save xargs param and exec arguments
    char* params[MAXARG];

    // init args cmd
    char* cmd_in_xargs = argv[1];

    int n, args_index = 0;

    int i;
    for (i = 1; i < argc; i++) {
        params[args_index++] = argv[i];
    }

    while(( n = read(STDIN_FILENO, line, MAX_LINE_LENGTH) ) > 0) {
//        printf("whole line: %s, %d\n", line, n);
        //echo a    b   ccss a | xargs echo ddd aaaa xxx
        //whole line: a b ccss a
        // the line will not include the unused blank symbol
        if(fork() == 0) { // child process
            // arg init as the whole args
            char *arg = (char*) malloc(sizeof(line));

            int index = 0;
            for (i = 0; i < n; i++){
                // every time after parsed a complete arg
                // the line will not include the unused blank symbol
                // \n always the last symbol while ' ' is used to split the args
                if (line[i] == ' ' || line[i] == '\n'){
                    // read the arg done
                    arg[index] = 0;
                    // append the arg to the final params
                    params[args_index++] = arg;
                    // reset the arg and read the next arg
                    index = 0;
                    // malloc new memory since the last arg will saved into params
//                    printf("arg*: %s, %lu\n", arg, sizeof(line));
                    arg = (char*) malloc(sizeof(line));
                } else {
                    // try save single arg
                    arg[index++] = line[i];
                }
            }
            arg[index] = 0;
            params[args_index] = 0;
            exec(cmd_in_xargs, params);
        } else {
            wait((int*)0); // wait child process
        }
    }

    exit(0);

}