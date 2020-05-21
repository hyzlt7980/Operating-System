#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <unistd.h>
#include <string.h>
#include <unistd.h>


int main(int argc, char *argv[]){
    
   
    pid_t pid;

    int status;
    
    int fd[2];
    
    int fd2[2];
    
    while(1){
    
        char *p;
        printf("$");
        size_t bufsize = 20;
        p = (char *)malloc(bufsize * sizeof(char));
        getline(&p,&bufsize,stdin);
        strtok(p, "\n");
        if(strcmp(p,"exit")==0){
            break;
        }
        if(pipe(fd)==-1){
            fprintf(stderr, "can't pipe\n");
            return  EXIT_FAILURE;
        }

        pid  = fork();
        
        if(pid<0)
        {
            perror("fork");
            exit(1);
        }else if(pid==0){
            printf("test1\n");
            close(fd[1]);
            char buf[20];
            read(fd[0],buf,20);
            strtok(buf, "\n");
            char *token = strtok(buf," ");
            char *array[20];
            int i=0;
            
            while(token!=NULL){    
                array[i++]=token;
                token =strtok(NULL," ");
            }
            
            execvp(buf,array);
            printf("test2");
        }else{
        
        
            write(fd[1],p,20);
            waitpid(pid,&status,0);
                
        }
    
    }
    return 0;
}


