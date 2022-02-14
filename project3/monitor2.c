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
    printf("stat_prog");
    printf("\033[0;m");
    printf("$ ");
    fgets(input, 1000, stdin);
    if((strlen(input) > 0) && (input[strlen(input) - 1] == '\n')){
        input[strlen(input) - 1] = '\0';
    }
    /* end program */
    if(strcmp(input, "q") == 0){
        printf("\nProgram finished successfully. \n");
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

int main(){
    if(fork() == 0){
        child_process();
    }
    else{
        wait(0);
    }

    return 0;
}