#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>

void write_to_file(char *argv[], int index, int number){
  char filename[32];
  snprintf(filename, sizeof(char) * 32, "%i.txt", index);
  FILE *f = fopen(filename, "w");

	if (f == NULL)
	{
	    printf("Error opening file!\n");
	    exit(1);
	}
  int counter = 0;
  for(; counter < number; counter++){
    fprintf(f, "%s\n", argv[index]);
  }
  fclose(f);
}

int main(int argc, char **argv){

  int i = 0;
  for (i = 0; i < argc; i++)
		printf("%s\n", argv[i]);

  int number = atoi(argv[argc - 1]);

  for (i = 1; i < argc - 1; i++){
    printf("Forking new process for word %s, %d times.\n", argv[i], number);
    pid_t pid = fork();
    if (pid < 0) exit(1);
    switch(pid){
      case 0:
        printf("Successfully forked one child.\n");
      default:
        printf("Calling write to file from %d.\n", getpid());
        write_to_file(argv, i, number);
        printf("Successfully wrote to file... breaking.\n.");
        break;
    }
  }
  wait(NULL);
  return 0;
}
