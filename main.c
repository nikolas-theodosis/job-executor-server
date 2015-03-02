#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#include "jobs.h"

char *fifo = "myfifo";
char *fifo2 = "myfifo2";
char *server_file_name = "server_file";
int MSGSIZE = 512;

int main(int argc, char *argv[]) {
	pid_t server_pid;
	FILE *server_file;
	FILE *commander_file;
	int fd, pid;

	server_file = fopen(server_file_name, "r");
        /*the file does not exists so i need to create the server*/
	if (server_file == NULL) {
		server_pid = fork();
		if (server_pid == -1) {
			perror("Fork failed");
			exit(1);
		}

		if (server_pid == 0) {
	                /*server : write pid to file*/
			server_file = fopen(server_file_name, "w");
			fprintf(server_file, "%d\n", getpid());
			fclose(server_file);
			jobExecutorServer();
		}
		else {
			pid = server_pid;
                        /*create fifo - named pipe*/
			if (mkfifo (fifo , 0666) == -1){
				if (errno != EEXIST ) {
					perror ("mkfifo failed") ;
					exit (6) ;
				}
			}
		}
	}
/*file exists so i just copy the pid from it*/
	else {
		fscanf(server_file, "%d", &pid);
		fclose(server_file);
	}
	
	if (argc == 1) {
		printf("missing arguments...\n");
		exit(1);
	}

	jobCommander(argv, pid);

}
