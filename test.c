#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <unistd.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
void trim_leading(char *str);
int main(int argc, char *argv[]){
    
   
    pid_t pid;

    int status;
    
    int fd[2];
    
    
    
    while(1){
    
        char *p;
        printf("$");
        size_t bufsize = 100;
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
            close(fd[1]);
            char buf[100];
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
                    
                    char *begin = malloc(sizeof(char));

                    char singe_command[20];
                    
                    close(fd2[1]);

                    read(fd2[0],singe_command,20);


                    if(singe_command[0]==' '){
                        
                        trim_leading(singe_command);
                    }
                
                    char *token_2 = strtok(singe_command," ");
                    char *singe_command_component[10];
                    int j = 0;
                    while(token_2!=NULL){    
                        singe_command_component[j++]=token_2;
                        token_2 =strtok(NULL," ");
                    }
                    

                    execvp(singe_command,singe_command_component);


                }else{        
                
                    close(fd2[0]);
                    write(fd2[1],array[k],20);
                    waitpid(id,&status2,0);
                }


            }    
        


        }else{

            close(fd[0]);
            write(fd[1],p,100);
            waitpid(pid,&status,0);
        }

        free(p);
    
    }
    return 0;
}


void trim_leading(char *str){

    char *p;
    int i=0;    
    while(str[i]==' '){
        i++;
    }
    int k=i;
    for(int j = k;j<sizeof(str);j++){
        str[j-i]=str[j];
        str[j]='\0';
    }

}