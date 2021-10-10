#include <stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>


void runCD(char** cdargs){};
char path[] = {"/bin"};

//change method
char **get_input(char *input) {
    char **command = malloc(8 * sizeof(char *));
    char *separator = " ";
    char *parsed;
    int index = 0;

    parsed = strtok(input, separator);
    while (parsed != NULL) {
        command[index] = parsed;
        index++;

        parsed = strtok(NULL, separator);
    }

    command[index] = NULL;
    return command;
}

void runLS(){
     int status;
    char *args[2];

    args[0] = "/bin";        
    args[1] = NULL;           

    if ( fork() == 0 )
        execv( args[0], args ); 
    else
        wait( &status );        
}


void runLSwithArgs(char** lsargs){
    int status;
    char *binaryPath = "/bin/ls";       
    char *args[] = {binaryPath,lsargs[1],lsargs[2], NULL};


    if ( fork() == 0 ){
        printf("%s\n\n",lsargs[1]);
        if(strlen(lsargs[1])>0){
             printf("We have an argument");
        }
        execv( binaryPath, args ); 
        }
    else
        wait( &status );        
}

void sanitizeInput(char* input){\
    printf("Sanitizing input\n");
    char *token;
    char *pointer;
    pointer = input;
    char* dict[10] = {0};
    int i=0;

    token = strtok(pointer, " \t\n");
    if (token == NULL)
        return;
    do
    {
        //fprintf(stdout, "%s\n", token);
        dict[i]=token;
        printf("%s",dict[i]);
        pointer = NULL;
        i++;
    } while ((token = strtok(NULL, " \t\n")) != NULL);

    //if(strcmp(dict[0],"ls")==0 && (dict[1] || dict[2] )) { runLSwithArgs(dict); return; }
    
    if(strcmp(dict[0],"cd" ) == 0  ){
        printf("We got a cd\n");
        int rc = fork();
        int stat;
        if (rc < 0) {
            fprintf(stderr, "fork failed\n");
            exit(1);
        } else if (rc == 0) {
            printf("try to chdir\n");
            int ret = chdir(dict[1]);
            printf("%d\n",ret);
            

        
        }
        else {
        int rc_wait = wait(&stat);
        char s[100];
        printf("%s\n", getcwd(s, 100));
        }
    }

    else { runLS(); return; }




}

void parseInput(char* input){
    printf("%s",input);
    
    sanitizeInput(input);  
}



int main(int argc, char* argv[]){

    char* buffer;
    size_t bufsize = 32;
    char *found;
    char* dict[10];
    int i=0;
    FILE* fptr;
    char* line=NULL;
    size_t len = 0;
    ssize_t read;

   
    if(argv[1]!=NULL){

        char* filename;
        filename = (char *)malloc(50 * sizeof(char));
        if( filename == NULL)
        {
            perror("Unable to allocate buffer");
            exit(1);
        }
        filename = argv[1];
        fptr = fopen(filename,"r+");

        if(fptr == NULL)
        {
            exit(1);             
        } 

        while ((read = getline(&line, &len, fptr)) != -1) {
                if(read>1){
                while( (found = strsep(&line,",")) != NULL ){
                dict[i] = found;
                printf("%s",dict[i]);
                i++;
        }
        }
        }
        
    }
    else{

        // buffer = (char *)malloc(bufsize * sizeof(char));
        // if( buffer == NULL)
        // {
        //     perror("Unable to allocate buffer");
        //     exit(1);
        // }

        // while(1){
        //     printf(">wish ");
        //     getline(&buffer,&bufsize,stdin);
        //     //printf("%s",buffer);
        //     parseInput(buffer);
        //     buffer[strcspn(buffer, "\n")] = 0;
        //     if(strcmp(buffer,"exit")==0){
        //         exit(0);
        //     }


   char **command;
    char *input;
    pid_t child_pid;
    int stat_loc;

    while (1) {
        input = readline("unixsh> ");
        command = get_input(input);

        if (!command[0]) {      /* Handle empty commands */
            free(input);
            free(command);
            continue;
        }

        child_pid = fork();
        if (child_pid == 0) {
            /* Never returns if the call is successful */
            execvp(command[0], command);
            printf("This won't be printed if execvp is successul\n");
        } else {
            waitpid(child_pid, &stat_loc, WUNTRACED);
        }

        free(input);
        free(command);
    }

 
    }
return 0;
}



