#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/times.h>
#include <time.h>
#include <fcntl.h>

int main(){
    int status;
    int pid[2];
    pid[0] = fork();
    
    if(pid[0] == 0){
        sleep(10);
        return 0;
    }
    else{
        pid[1] = fork();
        if(pid[1] == 0){
            sleep(4);
            return 0;
        }
        else{
            while(1){
                for(int i = 0; i < 2; i++){
                    int result = waitpid(pid[i], &status, WNOHANG);
                    if(result > 0){
                        printf("Child %d complete \n", i);
                    }
                }
            }
            return 0;
        }
        return 0;
    }
}