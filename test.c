#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <unistd.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
void trim_leading(char *str);

int main(int argc, char *argv[]){
    pid_t pid;
    int status;
    int fd[2];
    while(1){
        pid_t cp=getpid();
        printf("pid is %d\n",cp);
        printf("$");
        

        size_t bufsize = 100;
        char *p;
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
           
            int status2;
            pid_t id;
            char buf[100];

            close(fd[1]);
            read(fd[0],buf,100);
            
            char *array[10];
            int i = 0;
            char *token = strtok(buf, ";&");
            
            while (token!= NULL)
            {                
                array[i++] = token;
                token = strtok(NULL, ";&");

            }
            
            int fd2[2];
            
            if(pipe(fd2)==-1){
                fprintf(stderr, "can't pipe\n");
                return  EXIT_FAILURE;
            }
            
  
            for (int k = 0; k < i; ++k){
          
                if(pipe(fd2)==-1){
                    fprintf(stderr, "can't pipe\n");
                    return  EXIT_FAILURE;
                } 
                
                id =fork();

                if(id<0){
                    perror("fork");
                    exit(1);
                }else if(id==0){
                    char singe_command[20];
                    close(fd2[1]);
                    read(fd2[0],singe_command,20);
                    printf("single coomand:%s\n",singe_command);  

                    char *token_2 = strtok(singe_command," ");
                    char *singe_command_component[10];
                    int j = 0;
                    while(token_2!=NULL){    
                        singe_command_component[j++]=token_2;
                        token_2 =strtok(NULL," ");
                    }
                    execvp(singe_command_component[0],singe_command_component);
               
    
                }else{  
                         
                    close(fd2[0]);
                    write(fd2[1],array[k],20);
                    waitpid(id,&status2,0);
                    printf("child process in for loop terminate\n");

                }
                
            }  
            exit(1);

        }else{

            close(fd[0]);
            write(fd[1],p,100);
            waitpid(pid,&status,0);
            printf(" terminated\n");
        }

        free(p);
    
    }
    return 0;
}


void trim_leading(char *str){

    int size_of_whitespace=0;    
    while(str[size_of_whitespace]==' '){
        size_of_whitespace++;
    }
    printf("size of whitespace is %d\n",size_of_whitespace);

    

}