#include "w_io.h" //silly name for a file
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//research on dup3 pipe 2 and O_CLOSEXEC flag needed along fcntl function



void restore_std_fd()
{
    redir_in_to(STD_IN_DUP);
    redir_out_to(STD_OUT_DUP);
    redir_err_to(STD_ERR_DUP);//since these redir will create more dup s
    close(STD_IN_DUP); //https://stackoverflow.com/questions/43316836/do-duplicated-descriptor-files-get-closed-after-closing-the-original-file-descri
    close(STD_OUT_DUP);//this step is so damn important here otherwise program will keep hanging since duplicates will be open in 
    close(STD_ERR_DUP);//in background this was causing the internal piped to external to hang at one point
    STD_IN_DUP =0;//0
    STD_OUT_DUP =1;//1
    STD_ERR_DUP =2;//2

}

int redir_in_to(int fd) // used by pipes 
{
    
    STD_IN_DUP = dup(0);
    dup2(fd,0);

    return 0;

}

int redir_out_to(int fd) // 1 >& fd or >& fd
{
    
    STD_OUT_DUP = dup(1);
    dup2(fd,1);

    return 0;
}



int redir_err_to(int fd)
{
    STD_ERR_DUP = dup(2);
    dup2(fd,2);
    return 0;
}



int out_to_file(const char * filepath)// cmd > filename or > filename or 1 > filename
{
    int file_fd= open(filepath,O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH); //added mode because of O_CREAT flag 
    STD_OUT_DUP = dup(1);
    dup2(file_fd,1);

    return file_fd;
}

int append_to_file(const char * filepath)// cmd >> filename or >> filename or 1 >> filename
{
    int file_fd= open(filepath,O_WRONLY|O_CREAT|O_APPEND,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);//added mode because of O_CREAT flag 
    STD_OUT_DUP = dup(1);
    dup2(file_fd,1);

    return file_fd;
}

int err_to_file(const char *filepath)
{
    int file_fd= open(filepath,O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);//added mode because of O_CREAT flag 
    STD_ERR_DUP = dup(2);
    dup2(file_fd,2);

    return file_fd;
}


int append_err_to_file(const char *filepath)
{
    int file_fd= open(filepath,O_WRONLY|O_CREAT|O_APPEND,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);//added mode because of O_CREAT flag 
    STD_ERR_DUP = dup(2);
    dup2(file_fd,2);

    return file_fd;
}


int truncate_file(const char* filepath) // : > filename
{
    int file_fd= open(filepath,O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);//added mode because of O_CREAT flag
    close(file_fd);     
    return 0;
}


int redir_to_file(int fd,const char* filepath) // fd > filename
{
    int orig_fd= dup(fd);
    int file_fd = open(filepath,O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH); //added mode because of O_CREAT flag 
    dup2(file_fd,fd);
    return orig_fd;
}


int redir(int M,int N) // M >& N
{
    int orig_M = dup(M);
    dup2(N,M);
    return orig_M;
}

int rdir_err_out_to_file(const char* filename) // &> filename //new to BASH
{
    int file_fd = open(filename,O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
    STD_OUT_DUP = dup(1);
    STD_ERR_DUP = dup(2);
    dup2(file_fd,1);
    dup2(file_fd,2);
    return file_fd;
}



