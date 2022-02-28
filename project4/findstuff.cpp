#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <dirent.h> 
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <libgen.h>
#include <fcntl.h> 
#include <strings.h>
#include <sys/time.h> 
typedef struct FileObj
{
    char name[256];
    char path[1000];
    int size;
}FileObj;

typedef struct DirObj
{
    char name[256];
    char path[1000];
}DirObj;

typedef struct Queue
{
    DirObj *q[1000000];
    int size;
    int startIndex;
    int endIndex;
}Queue;

void addToQueue(Queue *Q,DirObj *dir)
{
    Q->q[Q->endIndex] = dir;
    Q->endIndex = ((Q->endIndex)+1)%1000000;
    Q->size = Q->size + 1;
}

DirObj *popFromQueue(Queue *Q)
{
    if(Q->size == 0)
    {
        return NULL;
    }
    DirObj *dir;
    dir = Q->q[Q->startIndex];
    Q->q[Q->startIndex] = NULL;
    Q->startIndex = ((Q->startIndex)+1)%1000000;
    Q->size = Q->size - 1;
    return dir;
}

Queue *Queue_init()
{
    Queue *Q;
    Q = (Queue *)malloc(sizeof(Queue));
    memset((Q->q),NULL,(sizeof(DirObj *))*1000000);
    Q->startIndex = 0;
    Q->endIndex = 0;
    Q->size = 0;
    return Q;
}
int get_queue_size(Queue *Q)
{
    return Q->size;
}

void get_children(DIR *d, dirent *dir,char *path, FileObj* file_list[],int *fileListIndex, DirObj* dir_list[1000], int *dirListIndex);
void get_child_dirs(DIR *d, dirent *dir,char *path, DirObj* dir_list[1000],int *dirListIndex);
void get_child_files(DIR *d, dirent *dir,char *path, FileObj* file_list[1000],int *fileListIndex);
void local_text_search(FileObj* file_list[],int fileListIndex, char* input_text,FileObj* result_File_list[], int *result_file_index);
void local_file_search(FileObj* file_list[],int fileListIndex, char* inputFileName,FileObj* result_File_list[], int *result_file_index);
void local_text_search_helper(char *fileName,char *file_text);
int bfs_search_file(char *inputFileName,DirObj *rootdir,DIR *d, dirent *dir, FileObj* file_list[],int *fileListIndex, DirObj* dir_list[1000], int *dirListIndex,FileObj* result_File_list[], int *result_file_index);
int bfs_search_text(char *inputText,DirObj *rootdir,DIR *d, dirent *dir, FileObj* file_list[],int *fileListIndex, DirObj* dir_list[1000], int *dirListIndex,FileObj* result_File_list[], int *result_file_index);
void get_child_files_fflag(DIR *d, dirent *dir,char *path, FileObj* file_list[1000],int *fileListIndex,char *fileSpecifier);
void get_children_fflag(DIR *d, dirent *dir,char *path, FileObj* file_list[],int *fileListIndex, DirObj* dir_list[1000], int *dirListIndex,char *fileSpecifier);
int bfs_search_file_fflag(char *inputFileName,DirObj *rootdir,DIR *d, dirent *dir, FileObj* file_list[],int *fileListIndex, DirObj* dir_list[1000], int *dirListIndex,FileObj* result_File_list[], int *result_file_index,char *fileSpecifier);
int bfs_search_text_fflag(char *inputText,DirObj *rootdir,DIR *d, dirent *dir, FileObj* file_list[],int *fileListIndex, DirObj* dir_list[1000], int *dirListIndex,FileObj* result_File_list[], int *result_file_index,char *fileSpecifier);
typedef struct ProcessList
{
    int numProcesses;
    int pids[10];
    char *tasks[10];
}ProcessList;
ProcessList *ProcessList_init_()
{
    ProcessList *p;
    p = (ProcessList *)mmap(NULL, (sizeof(ProcessList)), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    p->numProcesses = 0;
    memset(p->pids,-1,(10*sizeof(int)));
    return p;
}
void *ProcessList_init_(ProcessList *p)
{
    //ProcessList *p;
    p = (ProcessList *)mmap(NULL, (sizeof(ProcessList)), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    p->numProcesses = 0;
    memset(p->pids,NULL,(10*sizeof(int)));
    return p;
}
int addProcess(ProcessList *p,int childPid,char *childTask)
{
    int currIndex;
    if(p->numProcesses == 10)
    {
        return -1;
    }
    else if(p->numProcesses ==0||p->pids[p->numProcesses] ==-1) 
    {
        currIndex = p->numProcesses;
        p->pids[p->numProcesses] = childPid;
        p->tasks[p->numProcesses] = childTask;
        p->numProcesses = p->numProcesses +1;
        return currIndex;
    }
    else
    {
        for(int i =0;i<10; i++)
        {
            if(p->pids[i]== -1)
            {
                p->pids[i] = childPid;
                p->tasks[i]  = childTask;
                currIndex = i;
                p->numProcesses = p->numProcesses +1;
                return currIndex;
            }
        }
    }
    return -1;
}
int getNumProcesses(ProcessList *p)
{
    return p->numProcesses;
}
int removeProcess(ProcessList *p,int serialNum)
{
    if(p->numProcesses ==0 ||serialNum < 0|| serialNum >=10)
    {
        return -1;
    }
    int currPid = p->pids[serialNum];
    p->pids[serialNum] = -1;
    p->tasks[serialNum] = NULL;
    p->numProcesses = p->numProcesses - 1;
    return currPid;
}
int removeProcessPID(ProcessList *p,int pid)
{
    for(int i = 0; i < 10; i++)
    {
        if(p->pids[i] == pid)
        {
            int serialNum = i + 1;
            p->pids[i] = -1;
            p->tasks[i] = NULL;
            p->numProcesses = p->numProcesses - 1;
            return serialNum;
        }
    }
    return -1;
}
/*typedef struct childProcess
{
    int serialNum;
    int pid;
    char task[100];
    FileObj *result_list[1000];

}*/
struct Argument
{
    char arg1[100];
    char arg2[100];
    char arg3[100];
    char arg4[100];
};

/*bool file_exists_in_path(char *path, struct stat *sb, char *fileList)
{
    int retStat = stat(path,sb);
    if(retStat == 0)
    {
        // add to file
        return true;
    }
    return false;
}*/
ProcessList *process_list;
int fd[2];
int savestdin;
int realstdin;
void childPrint(int i)
{
    dup2(fd[0], STDIN_FILENO);
    //exit(0);
}
void restorestdin(int i)
{
    dup2(savestdin, STDIN_FILENO);
    wait(0);
    //exit(0);
}
/*void childwait(int i)
{
    wait(0);
}*/


bool get_argument(char* line, int argn, char* result)
	{
	//firstly: remove all spaces at the front
	char temp[1000];
	int start_space = 1;
	for (int i = 0, u = 0; i <= strlen(line); i++)
		if (line[i] == ' ' && start_space == 1) continue;
		else 
			{
			temp[u++] = line[i]; 
			start_space = 0;
			}
	//now remove an double or tripple spaces
	char temp2[1000];
	int space_on = 0;
	for (int i = 0, u = 0; i <= strlen(temp); i++)
		{
		if (space_on == 1 && temp[i] == ' ') continue;
		temp2[u++] = temp[i];
		if (temp[i] == ' ') space_on = 1;
		else space_on = 0;
		}
	//finally extract the arguments
	int start, end;
	start = end = 0;
	int count = 0;
	int quote = 0;

	for (int i = 0; i <= strlen(temp2); i++)
		if (temp2[i] == '\"') quote = !quote;
		else if (quote == 0 &&(temp2[i] == ' ' || temp2[i] == 0 || temp2[i] == '\n'))
			{
			end = i;
			if (argn == count)
				{
				int length = end - start;
				strncpy(result, temp2 + start, length);
				result[length] = 0;
				return 1;
				}
			start = end + 1;
			count++;
			}
	return 0;
	}

 
 
bool sFlagCheck(Argument *args)
{
    if((strcmp("-s",(char*)(args->arg3)) == 0)||(strcmp("-s",(char*)(args->arg4)) == 0))
    {
        //write(STDOUT_FILENO,"-s flag raised\n",16);
        fsync(fd[1]);
        fflush(stdout);
        return true;
    }
    return false;
}
    
bool fFlagCheck(Argument *args, char *fileTypeSpecifier)
{
    if((strncmp("-f:.",(char*)(args->arg3),4) == 0))//  this good
    {
        char output[100];
        strncpy(fileTypeSpecifier,(((char*)(args->arg3))+3),4);// this good
        //write(STDOUT_FILENO,"f flag raised\n",15);
        //sprintf(output,"file specifier = %s\n",fileTypeSpecifier);
        //write(STDOUT_FILENO,output,strlen(output))

        fflush(stdout);
        return true;
    }
    if((strncmp("-f:.",(char*)(args->arg4),4) == 0))//  this good
    {

        strncpy(fileTypeSpecifier,(((char*)(args->arg4))+3),4);// this good
        //printf("f flag raised\n");
        //printf("file specifier = %s\n",fileTypeSpecifier);
        //fflush(stdout);
        return true;
    }
    return false;   
}

bool textSearchCheck(Argument *args, char *stringToSearch)
{
    if((strncmp("\"",(char*)(args->arg2),1)==0)) // fixed
    {
        //printf("text search\n");
        //printf("str len of text: %lu\n",strlen(((char*)args->arg2)));
        //stringToSearch = strtok(((char*)(args->arg2)),"\"");
        strncpy(stringToSearch,(((char*)(args->arg2))+1),(strlen(((char*)args->arg2))-2));
        //printf("string to search is \"%s\"\n", stringToSearch);
        fflush(stdout);
        return true;
    }
    return false;
}

bool fileSearchCheck(Argument *args, char *fileName)
{
    if((strncmp("\"",(char*)(args->arg2),1)==0)) 
    {
        return false;
    }
    //printf("file search\n");
    strcpy(fileName,((char *)args->arg2));
    //printf("file to be found is \"%s\"\n",fileName);
    //fflush(stdout);
    return true;
}


char *childTask;



int main()
{
    //int bfs_search_file_fflag(char *inputFileName,DirObj *rootdir,DIR *d, dirent *dir, FileObj* file_list[],int *fileListIndex, DirObj* dir_list[1000], int *dirListIndex,FileObj* result_File_list[], int *result_file_index,char *fileSpecifier);
    //realstdin = fileno(stdin);
    //printf("realstdin = %d\n",realstdin);
    char rootpath[256];
    strcpy(rootpath,"/Users/tanmayvegunta/Desktop/cpe357test/");//please enter root dir here, add "/" at the end
    DirObj *root;
    root = (DirObj*)malloc(sizeof(DirObj));
    char cwd[256];
    getcwd(cwd,256);
    strcat(cwd,"/");
    strcpy(root->path,rootpath);
    DIR *d;
    dirent *dir;
    FileObj* file_list[10000];
    DirObj* dir_list[10000];
    int *file_index;
    file_index = (int *)malloc(sizeof(int));
    int *dir_index;
    dir_index = (int *)malloc(sizeof(int));
    FileObj* result_file_list[10000];
    int *result_index;
    result_index = (int *)malloc(sizeof(int));
    
    int *childProcessDone;
    childProcessDone = (int *)mmap(NULL, 4, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    *childProcessDone = 0;
    pipe(fd);
    process_list = ProcessList_init_();
    struct stat *sb;
    struct Argument *args;
    args  = (Argument *)malloc(sizeof(Argument));
    char *command;
    command = (char *)malloc(1073741824);
    char *child_output;
    child_output = (char *)malloc((sizeof(char)*10000000));
    char cmdcpy[1000];
    char tempResult[1000];
    pid_t childPids[10];
    int numArgs;
    char *token;
    bool fileSearch = false;
    bool textSearch = false;
    //bool findString = false;
    bool sIsTrue = false;
    bool fIsTrue = false;
    char *fileName;
    char *stringToSearch;
    char fileTypeSpecifier[5];
    stringToSearch  =(char *)malloc((100*sizeof(char)));
    fileName =(char *)malloc((100*sizeof(char)));
    savestdin = dup(STDIN_FILENO);
    dup2(savestdin,STDIN_FILENO);
    signal(SIGUSR1, childPrint);
    signal(SIGUSR2, restorestdin);
    //printf("we out\n");
    //fflush(stdout);
    int mainPid = getpid();
    while(1)
    {
        //dup2(savestdin,STDIN_FILENO);
        dup2(savestdin,STDIN_FILENO);
        (args->arg1)[0] = 0;
        (args->arg2)[0] = 0;
        (args->arg3)[0] = 0;
        (args->arg4)[0] = 0;

    
        numArgs = 0;
        write(STDOUT_FILENO,"\n\033[0;34mstat findstuff\033[0m$ ",29);
        fsync(fd[1]);
        fflush(stdout);
        memset(command,0,1073741824);
        read(STDIN_FILENO,command,1000);
        fflush(0);
        if(*childProcessDone == 1)
        {
            printf("%s\n",command);
            *childProcessDone = 0;
        }
        
        
        
        bool arg1bool = get_argument(command,0,((char*)(args->arg1)));
        bool arg2bool = get_argument(command,1,((char*)(args->arg2)));
        bool arg3bool = get_argument(command,2,((char*)(args->arg3)));
        bool arg4bool = get_argument(command,3,((char*)(args->arg4)));
        
        
        if((strcmp("q",(char*)(args->arg1)) == 0) || (strcmp("quit",(char*)(args->arg1)) == 0))
        {
            //printf("quitting\n");
            //quit
            if(getNumProcesses(process_list) == 0)
            {
                munmap(process_list,sizeof(ProcessList));
                munmap(childProcessDone,sizeof(int));

                return 0;
            }
            else
            {
                for(int l = 0;l<10;l++)
                {
                    if(process_list->pids[l]!= -1)
                    {
                        kill(process_list->pids[l],SIGUSR1);
                    }
                }
                munmap(process_list,sizeof(ProcessList));
                munmap(childProcessDone,sizeof(int));
                return 0;
            }
        }
        else if((strcmp("find",(char*)(args->arg1))) == 0)
        {
            if(strlen((char *)(args->arg2)) == 0)
            {
                write(STDOUT_FILENO,"usage: find filename/'\"keyword \" (-s) (-f:.XYZ)\n",49);
                fsync(fd[1]);
                fflush(stdout);
            }
            else if(getNumProcesses(process_list) >= 10) 
            {
                write(STDOUT_FILENO,"too many processes, please kill a process to make more finds.\n",63);
                fsync(fd[1]);
                fflush(stdout);
            }
            else 
            {
                //printf("find command\n");
                fflush(stdout);
                sIsTrue = sFlagCheck(args);
                fIsTrue = fFlagCheck(args,fileTypeSpecifier);
                textSearch = textSearchCheck(args, stringToSearch);
                fileSearch = fileSearchCheck(args,fileName);
            }
            childTask = (char *)malloc(((sizeof(char))*100));
            if(fileSearch==true)
            {
                strcat(childTask,"file search ");
                strcat(childTask,fileName);
                strcat(childTask," ");
            }
            if(textSearch == true)
            {
                strcat(childTask,"text Search ");
                strcat(childTask,stringToSearch);
                strcat(childTask," ");
            }
            if(sIsTrue == true)
            {
                strcat(childTask,"global search ");
            }
            if(sIsTrue == false)
            {
                strcat(childTask,"local search");
            }
            if(fIsTrue == true)
            {
                strcat(childTask,"file type specified ");
                strcat(childTask,fileTypeSpecifier);
            }
            //here see if text search or file search are true and fork.
            if(fileSearch == true || textSearch == true)
            {
                //printf("%s\n",root->path);
                fflush(stdout);
                if(fork() == 0)
                {
                    clock_t stop, start;
                    start = clock();
                    time_t T = time(NULL);
                    struct tm tm = *localtime(&T);
                    //printf("fork!!!!!\n\n");
                    strcpy(child_output,childTask);
                    addProcess(process_list,getpid(),childTask);
                    if(fileSearch == true)
                    {
                        if(sIsTrue == true)
                        {
                            if (fIsTrue == true)
                            {
                                bfs_search_file_fflag(fileName,root,d,dir,file_list,file_index,dir_list,dir_index,result_file_list,result_index,fileTypeSpecifier);
                            }
                            else if(fIsTrue == false)
                            {
                                bfs_search_file(fileName,root,d,dir,file_list,file_index,dir_list,dir_index,result_file_list,result_index);

                            }
                            
                        }
                        else if(sIsTrue == false)
                        {
                            
                            if (fIsTrue == true)
                            {
                                get_children_fflag(d,dir,root->path,file_list,file_index,dir_list,dir_index,fileTypeSpecifier);// replace root->path with char *cwd if needed
                                local_file_search(file_list,*file_index,fileName,result_file_list,result_index);
                            }
                            else if(fIsTrue == false)
                            {
                                get_children(d,dir,root->path,file_list,file_index,dir_list,dir_index);                        // replace root->path with char *cwd if needed
                                local_file_search(file_list,*file_index,fileName,result_file_list,result_index);
                            }
                            
                        }
                    }
                    else if(textSearch == true)
                    {
                        if(sIsTrue == true)
                        {
                            if (fIsTrue == true)
                            {
                                bfs_search_text_fflag(stringToSearch,root,d,dir,file_list,file_index,dir_list,dir_index,result_file_list,result_index,fileTypeSpecifier);
                            }
                            else if(fIsTrue == false)
                            {
                                bfs_search_text(stringToSearch,root,d,dir,file_list,file_index,dir_list,dir_index,result_file_list,result_index);

                            }
                            
                        }
                        else if(sIsTrue == false)
                        {
                            if (fIsTrue == true)
                            {
                                get_children_fflag(d,dir,root->path,file_list,file_index,dir_list,dir_index,fileTypeSpecifier); // replace root->path with char *cwd if needed
                                local_text_search(file_list,*file_index,stringToSearch,result_file_list,result_index);
                            }
                            else if(fIsTrue == false)
                            {
                                get_children(d,dir,root->path,file_list,file_index,dir_list,dir_index);
                                local_text_search(file_list,*file_index,stringToSearch,result_file_list,result_index); // replace root->path with char *cwd if needed
                            }
                            
                        }
                    }
                    if(*result_index == 0)
                    {
                        strcat(child_output,"\n<no files found>\n");
                    }
                    else if(*result_index > 0)
                    {
                        strcat(child_output,"\n<files found>\n");
                        for(int i = 0;i< *result_index;i++)
                        {
                            strcat(child_output,(char *)(result_file_list[i]->path));
                            strcat(child_output,"\n");
                        }
                    }
                    //printf("output = %s\n",child_output);
                    stop = clock();
                    double timetaken = ((double)(stop - start))/CLOCKS_PER_SEC;
                    int ms = timetaken * 1000;
                    char timestr[100];
                    strcpy(timestr, "");
                    sprintf(timestr, "\nTime taken: %d:%d:%d:%d", ms/(1000*60*60), (ms%(1000*60*60))/(1000*60), ((ms%(1000*60*60))%(1000*60))/1000, ((ms%(1000*60*60))%(1000*60))%1000);
                    strcat(child_output,timestr);
                    close(fd[0]);
                    write(fd[1],child_output,strlen(child_output));
                    fsync(fd[1]);
                    close(fd[1]);
                    fsync(STDOUT_FILENO);
                    fflush(stdout);
                    close(fd[1]);
                    fsync(STDOUT_FILENO);
                    *childProcessDone = 1;
                    removeProcessPID(process_list,getpid());
                    kill(mainPid,SIGUSR1);
                    kill(mainPid,SIGUSR2);
                    return 0;
                }
            }

        }
        else if((strcmp("list",(char*)(args->arg1))) == 0)
        {
            char reportBuffer[10000];
            //printf("list command\n");
            //fflush(stdout);
            if(getNumProcesses(process_list) == 0)
            {
                write(STDOUT_FILENO,"< No active processes >\n",25);
                fflush(stdout);
            }
            //list child pid + process numbers
            //printf("list command\n");
            else
            {
                //printf("we here\n");
                for(int i =0;i<10;i++)
                {
                    if(process_list->pids[i] == -1)
                    {
                        sprintf(reportBuffer,"serial num %d <no process>\n",(i+1));
                        write(STDOUT_FILENO,reportBuffer,strlen(reportBuffer));
                        fflush(0);
                    }
                    else if (process_list->pids[i]!= -1)
                    {
                        sprintf(reportBuffer,"serial num %d <pid = %d > <task: %s >\n",(i+1),process_list->pids[i],process_list->tasks[i]);
                        write(STDOUT_FILENO,reportBuffer,strlen(reportBuffer));
                        fflush(0);
                    }
                    
                }
            }

        }
        else if((strcmp("kill",(char*)(args->arg1))) == 0)
        {
            char killfailtext[100];
            int killPid;
            int serialNum = atoi(args->arg2);
            // kill child pid
            //printf("kill command\n");
            fflush(0);
            if(strlen(args->arg2)==0)
            {
                write(STDOUT_FILENO,"usage kill < serial number(1-10) >\n",36);
                fflush(0);
            }
            else if(getNumProcesses(process_list) ==0)
            {
                write(STDOUT_FILENO,"no active processes to kill\n",29);
                fflush(0);
            }
            else if(serialNum == 0)
            {
                write(STDOUT_FILENO,"invalid serial Number\n",23);
                fflush(0);
            }
            else if((serialNum > 0) && (serialNum <= 10))
            {
                killPid = removeProcess(process_list,(serialNum - 1));
                if(killPid == -1)
                {
                    sprintf(killfailtext,"failed to kill process with bad serial number %d\n",serialNum);
                    write(STDOUT_FILENO,killfailtext,strlen(killfailtext));
                    fflush(0);
                }
                else if(killPid != -1)
                {
                    kill(killPid,SIGUSR1);
                    waitpid(killPid,0,0);
                    printf("process with serial number %d and pid %d killed successfully\n",serialNum,killPid);
                    fflush(0);
                }
            }
        }
        else 
        {
            // invalid command printf
            //printf("invalid command\n");
            //fflush(stdout);
        }
        fileSearch = false;
        textSearch = false;
        sIsTrue = false;
        fIsTrue= false;
        memset(fileName,0,100);
        memset(stringToSearch,0,100);
        memset((fileTypeSpecifier),0,5);
        memset(command,0,1000);
        memset((char*)(args->arg1),0,100);
        memset((char*)(args->arg2),0,100);
        memset((char*)(args->arg3),0,100);
        memset((char*)(args->arg4),0,100);
        memset(fileName,0,100);
        memset(stringToSearch,0,100);
    }
    return 0;
}
void local_text_search(FileObj* file_list[],int fileListIndex, char* input_text,FileObj* result_File_list[], int *result_file_index)
{
    char fileText[1000000];
    FILE *fp;
    memset(fileText,0,sizeof(fileText));
    for(int i = 0; i < fileListIndex; i++)
    {
        if((file_list[i])!=NULL)
        {
            local_text_search_helper((char *)(file_list[i]->path),fileText);
            
            if(strstr(fileText,input_text)!=NULL)
            {
                
                result_File_list[*result_file_index] = file_list[i];
                *result_file_index = *result_file_index +1;
            }
                
        }
    }
    
}

void local_text_search_helper(char *fileName,char *file_text)
{
    memset(file_text,0,sizeof(char)*1000000);
    FILE *fp;
    char eofCheck[2];
    fp = fopen(fileName,"r");

    if(fp!= NULL)
    {

        while( ((int)(eofCheck[0]= getc(fp))) != EOF) 
        {
            strcat(file_text, eofCheck);
            
        }
    } 
    else
    {
        perror("error");
    }

    fclose(fp);
}
int bfs_search_file(char *inputFileName,DirObj *rootdir,DIR *d, dirent *dir, FileObj* file_list[],int *fileListIndex, DirObj* dir_list[1000], int *dirListIndex,FileObj* result_File_list[], int *result_file_index)
{
    *result_file_index = 0;
    DirObj *currDir;
    Queue *Q;
    Q = Queue_init();
    addToQueue(Q,rootdir);
    while (get_queue_size(Q)>0)
    {
        currDir = popFromQueue(Q);
        get_children(d,dir,currDir->path,file_list,fileListIndex,dir_list,dirListIndex);
        local_file_search(file_list,*fileListIndex,inputFileName,result_File_list,result_file_index);
        for(int i = 0; i< *dirListIndex;i++)
        {
            addToQueue(Q,dir_list[i]);
        }
        *fileListIndex = 0;
        *dirListIndex = 0;
    }

    return *result_file_index;
      
}

int bfs_search_text(char *inputText,DirObj *rootdir,DIR *d, dirent *dir, FileObj* file_list[],int *fileListIndex, DirObj* dir_list[1000], int *dirListIndex,FileObj* result_File_list[], int *result_file_index)
{
    *result_file_index = 0;
    DirObj *currDir;
    Queue *Q;
    Q = Queue_init();
    addToQueue(Q,rootdir);
    while (get_queue_size(Q)>0)
    {
        currDir = popFromQueue(Q);
        get_children(d,dir,currDir->path,file_list,fileListIndex,dir_list,dirListIndex);
        local_text_search(file_list,*fileListIndex,inputText,result_File_list,result_file_index);
        for(int i = 0; i< *dirListIndex;i++)
        {
            addToQueue(Q,dir_list[i]);
        }
        *fileListIndex = 0;
        *dirListIndex = 0;
    }

    return *result_file_index;
      
}
void get_child_files_fflag(DIR *d, dirent *dir,char *path, FileObj* file_list[1000],int *fileListIndex,char *fileSpecifier)
{
    *fileListIndex = 0;
    struct FileObj *currFile;
    struct stat sb;
    int retStat;
    char currName[256];
    char tempPath[1000];
    strcpy(tempPath,path);
    
    d = opendir(tempPath);
    if (d) 
    {   
        
        while ((dir = readdir(d)) != NULL) 
        {
            strcpy(tempPath,path);
            strcpy(currName,dir->d_name);
            strcat(tempPath,currName);
            retStat = stat((const char *)tempPath,&sb);
            
            if(retStat == 0)
            {
                if((sb.st_mode & S_IFMT) == S_IFREG && (strstr(currName,fileSpecifier)!=0))
                {
                    currFile = (FileObj *)malloc(sizeof(FileObj));
                    //currFile = new FileObj();
                    strcpy((char *)(currFile->name),(const char *)currName); // set_name() method to be implemented
                    //printf("in file loop\n");

                    //strcat(tempPath,currName);
                    strcpy((char *)(currFile->path),(const char *)tempPath); // set_path() method to be implemented
                    currFile->size = sb.st_size;
                    file_list[*fileListIndex] = currFile;
                    *fileListIndex  = *fileListIndex +1;
                    //printf("currFile = %s, index = %d\n", (char *)(currFile->path), fileListIndex);

                }
            }
        
        }
        closedir(d);
    }
}
void get_children_fflag(DIR *d, dirent *dir,char *path, FileObj* file_list[],int *fileListIndex, DirObj* dir_list[1000], int *dirListIndex,char *fileSpecifier)
{
    get_child_dirs(d,dir,(char *)path,dir_list,dirListIndex);
    get_child_files_fflag(d,dir,(char *)path,file_list,fileListIndex,fileSpecifier);
}
int bfs_search_file_fflag(char *inputFileName,DirObj *rootdir,DIR *d, dirent *dir, FileObj* file_list[],int *fileListIndex, DirObj* dir_list[1000], int *dirListIndex,FileObj* result_File_list[], int *result_file_index,char *fileSpecifier)
{
    *result_file_index = 0;
    DirObj *currDir;
    Queue *Q;
    Q = Queue_init();
    addToQueue(Q,rootdir);
    while (get_queue_size(Q)>0)
    {
        currDir = popFromQueue(Q);
        get_children_fflag(d,dir,currDir->path,file_list,fileListIndex,dir_list,dirListIndex,fileSpecifier);
        local_file_search(file_list,*fileListIndex,inputFileName,result_File_list,result_file_index);
        for(int i = 0; i< *dirListIndex;i++)
        {
            addToQueue(Q,dir_list[i]);
        }
        *fileListIndex = 0;
        *dirListIndex = 0;
    }

    return *result_file_index;
      
}
int bfs_search_text_fflag(char *inputText,DirObj *rootdir,DIR *d, dirent *dir, FileObj* file_list[],int *fileListIndex, DirObj* dir_list[1000], int *dirListIndex,FileObj* result_File_list[], int *result_file_index,char *fileSpecifier)
{
    *result_file_index = 0;
    DirObj *currDir;
    Queue *Q;
    Q = Queue_init();
    addToQueue(Q,rootdir);
    while (get_queue_size(Q)>0)
    {
        currDir = popFromQueue(Q);
        get_children_fflag(d,dir,currDir->path,file_list,fileListIndex,dir_list,dirListIndex,fileSpecifier);
        local_text_search(file_list,*fileListIndex,inputText,result_File_list,result_file_index);
        for(int i = 0; i< *dirListIndex;i++)
        {
            addToQueue(Q,dir_list[i]);
        }
        *fileListIndex = 0;
        *dirListIndex = 0;
    }

    return *result_file_index;
      
}
void get_child_dirs(DIR *d, dirent *dir,char *path, DirObj* dir_list[1000],int *dirListIndex)
{
    *dirListIndex = 0;
    struct DirObj *currDir;
    struct stat sb;
    int retStat;
    char currName[256];
    char tempPath[1000];
    strcpy(tempPath,path);
    
    d = opendir(tempPath);
    if (d) 
    {   
        
        while ((dir = readdir(d)) != NULL) 
        {
            strcpy(tempPath,path);
            strcpy(currName,dir->d_name);
            strcat(tempPath,currName);
            retStat = stat((const char *)tempPath,&sb);
            
            if(retStat == 0)
            {
                if(((sb.st_mode & S_IFMT) == S_IFDIR) && (strcmp(currName,".")!=0) && (strcmp(currName,"..")!=0))
                {
                    currDir = (DirObj *)malloc(sizeof(DirObj));
                    strcpy((char *)(currDir->name),(const char *)currName); // set_name() method to be implemented
                    strcat(tempPath,"/");
                    strcpy((char *)(currDir->path),(const char *)tempPath); // set_path() method to be implemented
                    dir_list[*dirListIndex] = currDir;
                    *dirListIndex = *dirListIndex +1;
                    

                }
            }
        
        }
        closedir(d);
    }
}
void get_child_files(DIR *d, dirent *dir,char *path, FileObj* file_list[1000],int *fileListIndex)
{
    *fileListIndex = 0;
    struct FileObj *currFile;
    struct stat sb;
    int retStat;
    char currName[256];
    char tempPath[1000];
    strcpy(tempPath,path);
    
    d = opendir(tempPath);
    if (d) 
    {   
        
        while ((dir = readdir(d)) != NULL) 
        {
            strcpy(tempPath,path);
            strcpy(currName,dir->d_name);
            strcat(tempPath,currName);
            ////printf("\n\n\n----temp path = %s\n",tempPath);
           //printf("-> %s\n", currName);
            retStat = stat((const char *)tempPath,&sb);
            //retStat = stat(,sb);
            
            if(retStat == 0)
            {
                if((sb.st_mode & S_IFMT) == S_IFREG)
                {
                    currFile = (FileObj *)malloc(sizeof(FileObj));
                    //currFile = new FileObj();
                    strcpy((char *)(currFile->name),(const char *)currName); // set_name() method to be implemented
                    //printf("in file loop\n");

                    //strcat(tempPath,currName);
                    strcpy((char *)(currFile->path),(const char *)tempPath); // set_path() method to be implemented
                    currFile->size = sb.st_size;
                    file_list[*fileListIndex] = currFile;
                    *fileListIndex  = *fileListIndex +1;
                    //printf("currFile = %s, index = %d\n", (char *)(currFile->path), fileListIndex);

                }
            }
        
        }
        closedir(d);
    }
}
void get_children(DIR *d, dirent *dir,char *path, FileObj* file_list[],int *fileListIndex, DirObj* dir_list[1000], int *dirListIndex)
{
    get_child_dirs(d,dir,(char *)path,dir_list,dirListIndex);
    get_child_files(d,dir,(char *)path,file_list,fileListIndex);
}
void local_file_search(FileObj* file_list[],int fileListIndex, char* inputFileName,FileObj* result_File_list[], int *result_file_index)
{
    //currFile = (FileObj *)malloc(sizeof(FileObj));
    struct FileObj *currFile;
    //currFile = (FileObj *)malloc(sizeof(FileObj));
    for(int i = 0; i < fileListIndex; i++)
    {
        if((file_list[i])!=NULL)
        {
            if(strcmp((const char *)(file_list[i]->name),inputFileName) == 0)
            {
                result_File_list[*result_file_index] = file_list[i];
                *result_file_index = *result_file_index +1;
            }
        }




        // currFile = (FileObj *)malloc(sizeof(FileObj));
        // //currFile = file_list[i];
        // if((file_list[i])!=NULL)
        // {
        //     strcpy((char *)(currFile->name),(const char *)(file_list[i]->name));
        //     strcpy((char *)(currFile->path),(const char *)(file_list[i]->path));
        // }
        // if(strcmp(((char*)(currFile->name)),inputFileName) == 0)
        // {
        //     result_File_list[i] = currFile;
        //     *result_file_index = *result_file_index +1;
        // }
    }
}
