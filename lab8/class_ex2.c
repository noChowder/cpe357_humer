#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/times.h>
#include <time.h>
#include <fcntl.h>

// Exercise: Two-Way Pipes
// a. Parent sends message to Child ("Hi Kid") and
//    Child sends Message to parent ("Hi Mom & Dad").
//    Use pipes!

/*int main(){
    int fd[2];
    pipe(fd);

    if(fork() == 0){
        close(fd[0]);
        write(fd[1], "Hi Mom & Dad", 20);
        write(fd[1], "Hi Kid", 10);
        close(fd[1]);
        return 0;
    }
    else{
        close(fd[0]);
        char text[100];
        read(fd[0], text, 20);
        printf("%s \n", text);
        text[0] = 0;
        read(fd[0], text, 10);
        printf("%s \n", text);
        close(fd[1]);
        wait(0);
    }

    return 0;
}*/

int main(){
    int pipeToParent[2];
    int pipeToChild[2];

    pipe(pipeToChild);
    pipe(pipeToParent);

    int pid = fork();

    if(pid == 0){
        char message[100];
        close(pipeToChild[1]);
        read(pipeToChild[0], message, 20);
        close(pipeToChild[0]);

        close(pipeToParent[0]);
        write(pipeToParent[1], "Hello Mom & Dad", 100);
        close(pipeToParent[1]);
        return 0;
    }
    else{
        sleep(5);
        close(pipeToChild[0]);
        write(pipeToChild[1], "Hello Kid!", 100);
        close(pipeToChild[1]);

        char message[100];
        close(pipeToParent[1]);
        read(pipeToParent[0], message, 100);
        close(pipeToParent[0]);
    }
}