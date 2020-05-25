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

#define LENGTH_LIMIT 80;
int main(int argc, char *argv[])
{
    pid_t pid;
    int status;
    int fd[2];

    /* The while loop will looping and print prompt requesting user to input some command.
     * The first fork in the while loop will create a process A(parent) and B(child 1);  
     * Process A will send the line of command to Process B(child 1) through pipe and then wait for Prcess B to be terminated.
     * 
     * The Process B will split the line of command into several single linux executable command.
     * Process B will use fork inside a for loop to create several child processes to execute the single command.
     * The procecss B itself will keep looping the for loop if one child process terminate.
     * 
     * When all of child processes to executie single linux command terminate, the for loop is over and Process B will exit by itself. 
     *
     * Then the control of program return to Process A, it will print out $ and wait for user to input.
     *  
     */
    while (1)
    {

        pid_t main_id = getpid();
        // printf("while Id is %d\n", main_id);
        int return_id = waitpid(-1, &status, WNOHANG);

        // if (return_id > 0)
        // {
        //     printf("id is %d\n", return_id);
        // }
        // else
        // {
        //     printf("id is %d\n", return_id);
        // }

        printf("$");

        char *line_input;

        line_input = (char *)malloc(sizeof(char));    
        fgets(line_input,100,stdin);
        strtok(line_input, "\n");
    
        if(strlen(line_input)>80){
            printf("Your input is too long. Please try again\n");
            continue;
        }
        
        //Exit the program if user input exit;
        if (strcmp(line_input, "exit") == 0)
        {
            break;
        }

        
        if (pipe(fd) == -1)
        {
            fprintf(stderr, "can't pipe\n");
            return EXIT_FAILURE;
        }

        
        /* Fork here
         * Create Process A (send the line of command B through pipe and execute the while loop ) 
         * and    Process B(receive the line of command and split it in to several command, using a for loop to execute all of them.)
         *        Process B will terminate if all commands have been executed.
         */
        pid = fork();

        if (pid < 0)
        {
            perror("fork");
            exit(1);
        }
        else if (pid == 0)
        {

            /* This is process B, you will see a for loop inside it.
             * Inside the for loop, Process B will fork.
             * Create several child process(c1, c2, c3...) to execute each of single command.
             * Every child process will terminate if the command has been executed.
             */
            pid_t id;
            char buf[400] = {'\0'};

            close(fd[1]);
            read(fd[0], buf, 400);

            char *copy = strdup(buf);

            // array is the array of pointer used to hold all the single commands.
            char *array[10]; 
            
            
            char *token = strtok(buf, ";&");
            char delimiter;
            
            /* b_job is an array used to record which of single command 
             * inside the array[] will be executed in the background.
             */
            int b_job[20] = {999};
            int b_count = 0;

            int i = 0;
            while (token != NULL)
            {

                delimiter = copy[token - buf + strlen(token)];
                // printf("delimiter is %c\n",delimiter);

                if (delimiter == '&' && copy[token - buf + strlen(token) + 1] != '&')
                {
                    // printf("enter\n");
                    b_job[b_count] = i;
                    b_count++;
                }
                array[i++] = token;

                token = strtok(NULL, ";&");
            }
            // printf("job is %d\n",b_job[0]);
            // for(int j =0 ; j <b_count;j++){
            // printf("background job is %d\n",b_job[j]);
            // }

            int fd2[2];

            if (pipe(fd2) == -1)
            {
                fprintf(stderr, "can't pipe\n");
                return EXIT_FAILURE;
            }

            // b_job_index is an integer used to loop b_job[]
            int b_job_index = 0;

            //For loop here to execute single command                        
            for (int k = 0; k < i; ++k)
            {

                if (pipe(fd2) == -1)
                {
                    fprintf(stderr, "can't pipe\n");
                    return EXIT_FAILURE;
                }


                /* Porcess B will fork here 
                 * and create child process c1, c2, c3...
                 */
                id = fork();

                if (id < 0)
                {
                    perror("fork");
                    exit(1);
                }
                else if (id == 0)
                {

                    /*Child Processes c1, c2, c3... to execute a single command.
                     */
                    char single_command[40];
                    close(fd2[1]);
                    read(fd2[0], single_command, 40);
                    // printf("single coomand:%s\n",single_command);

                    char *contain;
                    contain = strchr(single_command, '|');

                    /*If a single command contain "|", which means a single command use pipe technique.
                     * We have to use belowing snippet of code to execute the single command.
                     * 
                     * The belowing part of code use two pipe: fd3[2] and fd4[2];
                     * fd3 will be used for directing STDOUT_FILENO to fd3[1] 
                     * and for direct standard input from fd3[0].
                     * 
                     * fd4[2] will be used to send seoncd command after | to child process 
                     *  
                     */
                    if (contain != NULL)
                    {

                        char *pipe_token = strtok(single_command, "|");
                        char *pipe_content[3];

                        pipe_content[0] = pipe_token;
                        pipe_token = strtok(NULL, "|");
                        pipe_content[1] = pipe_token;

                        int fd3[2];
                        int fd4[2];
                        pipe(fd3);
                        pipe(fd4);
                        // printf("content 1 is %s\n",pipe_content[0]);
                        // printf("content 2 is %s\n",pipe_content[1]);
                        
                        
                        // fork here, create two process to execute pipe
                        pid_t pipe_pid = fork();
                        if (pipe_pid == 0)
                        {

                            dup2(fd3[1], STDOUT_FILENO);
                            close(fd3[0]);
                            close(fd3[1]);
                            char first_command[20];
                            close(fd4[1]);
                            read(fd4[0], first_command, 20);
                            // printf("first command is %s", first_command);
                            

                            /*break single command into tokens.
                             */
                            char *token_3 = strtok(first_command, " ");
                            char *single_command_component[10];
                            int j = 0;
                            while (token_3 != NULL)
                            {
                                printf("%s\n", token_3);
                                single_command_component[j++] = token_3;
                                token_3 = strtok(NULL, " ");
                            }
                            execvp(single_command_component[0], single_command_component);

                            exit(1);
                        }
                        else
                        {

                            close(fd4[0]);
                            write(fd4[1], pipe_content[0], 20);
                            dup2(fd3[0], STDIN_FILENO);
                            close(fd3[0]);
                            close(fd3[1]);
                            
                            /*break single command into tokens. 
                             */
                            char *token_2 = strtok(pipe_content[1], " ");
                            char *single_command_component[10];
                            int j = 0;
                            
                            while (token_2 != NULL)
                            {
                                printf("%s", token_2);
                                single_command_component[j++] = token_2;
                                token_2 = strtok(NULL, " ");
                            }
                            execvp(single_command_component[0], single_command_component);
                            exit(1);
                        }
                    }
                    /*if the single command does not contain "|"
                     * we just execute it in the parent process.
                     */
                    else
                    {   
                        /*break single command into tokens. 
                         */
                        char *token_2 = strtok(single_command, " ");
                        char *single_command_component[10];
                        int j = 0;
                        while (token_2 != NULL)
                        {
                            single_command_component[j++] = token_2;
                            token_2 = strtok(NULL, " ");
                        }


                        execvp(single_command_component[0], single_command_component);
                    }
                }//Child process c1, c2, c2... terminate here.
                else
                {

                    /*Process B here, sending single command to Processes c1, c2, c3....
                     *if single command end with &, Process B will not use waitpid();
                     */
                    close(fd2[0]);
                    write(fd2[1], array[k], 40);

                    if (k == b_job[b_job_index])
                    {
                        // printf("k is %d\n",k);
                        // printf("b job is %d\n",b_job[b_job_index]);

                        // int wait_id = waitpid(-1, &status, WNOHANG);
                        // printf("wait id is %d\n",wait_id);

                        b_job_index++;
                    }
                    else
                    {
                        int wait_id = waitpid(id, &status, 0);
                    }

                    // printf("one command component complete\n");
                }
            }//For loop terminate

            // printf("for loop finish\n");
            exit(1);// Process B exit
        }// Process B terminate here.
        else
        {
            //Process A send the line of command to Process B;
            close(fd[0]);
            write(fd[1], line_input, 400);
            waitpid(pid, &status, 0);
            // printf("one line command compete\n");
        }

        free(line_input);

    }// While loop terminate.
    return 0;
}

