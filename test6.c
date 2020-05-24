#include  <fcntl.h>                              
#include  <stdio.h>                              
#include  <stdlib.h>                             
#include  <string.h>                             
#include  <sys/types.h>                          
#include  <sys/wait.h>                           
#include  <sys/stat.h>                          
#include  <termios.h>                            
#include  <unistd.h>                             
                                                                          
                                                
int main(int argc, char* argv[])                 
{                                                
     pid_t pid;
    int fd[2];

    pipe(fd);
    pid = fork();

    if(pid==0)
    {
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        close(fd[1]);
        execlp("ls", "ls", "-l", (char*) NULL);
       
        exit(1);
    }
    else
    { 
        pid=fork();

        if(pid==0)
        {
            dup2(fd[0], STDIN_FILENO);
            close(fd[1]);
            close(fd[0]);
            execlp("wc", "wc", "-l",(char*) NULL);
            exit(1);
        }
        else
        {
            int status;
            close(fd[1]);
            close(fd[0]);
            waitpid(pid, &status, 0);
        }
}                                         //
}