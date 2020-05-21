#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <sys/types.h> 
#include <unistd.h>

int main(int argc, char *argv[]){

    pid_t pid;
    
    pid  = fork();

    if(pid<0)
    {
        perror("fork");
        exit(1);
    }

    if(pid == 0)
    {
        execv("/Users/hui_yang/Desktop/3120/A2/test",NULL);
    }
    else  
    {   
        wait(NULL);
        printf("Parent process is terminating...\n");
        return 0;
    }
}




