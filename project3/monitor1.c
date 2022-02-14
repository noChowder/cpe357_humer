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

int main(){
    char input[1000];
    printf("\033[0;34m");
    printf("stat_prog");
    printf("\033[0;m");
    printf("$ ");
    scanf("%s", input);

    struct stat sb;
    int ret = stat(input, &sb);
    if(ret != 0){
        perror(__FUNCTION__);
        main();
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

    return 0;
}