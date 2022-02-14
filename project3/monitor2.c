#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/mman.h>
#include <dirent.h>
#include <signal.h>
#include <sys/sysmacros.h>
#include <sys/stat.h>

int child_process(){
    char input[1000];
    printf("\033[0;34m");
    printf("monitor2");
    printf("\033[0;m");
    printf("$ ");
    //alarm(3);
    //T = time(NULL);
    //*tm = *localtime(&T);
    //*old_sec = tm->tm_sec;
    //printf("Local time is: \t\t%d:%d:%d \n", tm->tm_hour, tm->tm_min, tm->tm_sec);
    fgets(input, 1000, stdin);
    //alarm(0);
    //T = time(NULL);
    //*tm = *localtime(&T);
    if((strlen(input) > 0) && (input[strlen(input) - 1] == '\n')){
        input[strlen(input) - 1] = '\0';
    }
    /* end program */
    if(strcmp(input, "q") == 0){
        printf("Program finished successfully. \n");
        return 0;
    }
    /* list cwd */
    else if(strcmp(input, "list") == 0){
        DIR *dir;
        dir = opendir(".");
        if(!dir){
            perror("opendir() failed");
            return -1;
        }
        struct dirent *entry;
        printf("cwd files: \n");
        while((entry = readdir(dir)) != NULL){
            printf("%s \n", entry->d_name);
        }
        putchar('\n');
        closedir(dir);
        if(child_process() == 0){
            return 0;
        }
    }
    /* file stats */
    struct stat sb;
    int ret = stat(input, &sb);
    if(ret != 0){
        perror(__FUNCTION__);
        putchar('\n');
        if(child_process() == 0){
            return 0;
        }
    }
    printf("ID of containing device:  [%lx,%lx]\n", (long)major(sb.st_dev), (long)minor(sb.st_dev));
            printf("File type:                ");
                switch(sb.st_mode & S_IFMT){
                    case S_IFBLK:   printf("block device \n");          break;
                    case S_IFCHR:   printf("character device \n");      break;
                    case S_IFDIR:   printf("directory\n");              break;
                    case S_IFIFO:   printf("FIFO/pipe\n");              break;
                    case S_IFLNK:   printf("symlink\n");                break;
                    case S_IFREG:   printf("regular file\n");           break;
                    case S_IFSOCK:  printf("socket\n");                 break;
                    default:        printf("unknown?\n");               break;
                }

                printf("I-node number:            %ld\n", (long) sb.st_ino);
                printf("Mode:                     %lo (octal)\n", (unsigned long) sb.st_mode);
                printf("Link count:               %ld\n", (long) sb.st_nlink);
                printf("Ownership:                UID=%ld   GID=%ld\n", (long) sb.st_uid, (long) sb.st_gid);
                
                printf("Preferred I/O block size: %ld bytes\n", (long) sb.st_blksize);
                printf("File size:                %lld bytes\n", (long long) sb.st_size);
                printf("Blocks allocated:         %lld\n", (long long) sb.st_blocks);
                
                printf("Last status change:       %s", ctime(&sb.st_ctime));
                printf("Last file access:         %s", ctime(&sb.st_atime));
                printf("Last file modification:   %s", ctime(&sb.st_mtime));

    putchar('\n');
    if(child_process() == 0){
        return 0;
    }
    return 0;
}

void signalhandler1(int sig){
    printf("Termination not possible \n");
}
void signalhandler2(int sig){
    printf("in here \n");
}

int main(){
    for(int sig = 1; sig < 65; sig++){
        signal(sig, signalhandler1);
    }
    //signal(SIGALRM, signalhandler2);
    //int parent_pid = getpid();
    //printf("Parent pid: \t\t%d \n", parent_pid);
    int *child_pid = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0);
    //time_t T = time(NULL);
    //struct tm *tm = (struct tm *)mmap(NULL, sizeof(struct tm), PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0);
    //int *old_sec = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0);

    if(fork() == 0){
        if(child_process() == 0){
            kill(*child_pid, SIGKILL);
        }
    }
    else{
        wait(0);
        sleep(10);
        main();
    }

    munmap(child_pid, sizeof(int));
    //munmap(tm, sizeof(struct tm));
    //munmap(old_sec, sizeof(int));
    return 0;
}