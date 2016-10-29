#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

int main(int argc, char* argv[])
{
    pid_t pid = 0;
    pid_t sid = 0;
    FILE *fp= NULL;
    int i = 0;
    pid = fork();

    if (pid < 0)
    {
        printf("fork failed!\n");
        exit(1);
    }

    if (pid > 0)
    {
    	// in the parent
       printf("pid of child process %d \n", pid);
       exit(0); 
    }

    umask(0);

	// open a log file
    fp = fopen ("logfile.log", "w+");
    if(!fp){
    	printf("cannot open log file");
    }
    
    // create new process group -- don't want to look like an orphan
    sid = setsid();
    if(sid < 0)
    {
    	fprintf(fp, "cannot create new process group");
        exit(1);
    }
    
    /* Change the current working directory */ 
    if ((chdir("/")) < 0) {
      printf("Could not change working directory to /\n");
      exit(1);
    }		
	
	// close standard fds
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    while (i < 20)
    {
        sleep(1);
        fprintf(fp, "%d", i);
        fflush(fp);
        i++;
    }

    fclose(fp);
  
    return (0);
}
