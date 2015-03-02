#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "list.h"
#include "jobs.h"

Running_List running_jobs_list = NULL;
Queued_List queued_jobs_list = NULL;
int N = 1;

void jobExecutorServer() {
	int fd, fd2, unknown_command = 1, i, jobID = 0, id, nwrite, stopped, length;
	char msgbuf[MSGSIZE + 1], *operation, *parameter, *args[64], response_string[100];
	/*make fifo to send response back to commander*/
	if (mkfifo (fifo2 , 0666) == -1){
		if (errno != EEXIST ) {
			perror ("mkfifo failed") ;
			exit (6) ;
		}
	}
	
	for (;;) {

		for (i=0 ; i<64 ; i++)
			args[i] = NULL;

		if ( (fd=open(fifo, O_RDWR)) < 0) {
			perror("server : fifo open problem"); 
			exit(3);	
		}

		if (read(fd, msgbuf, MSGSIZE + 1) < 0) {
			perror("problem in reading");exit(5);
		}
		close(fd);	
		/*split the string read in the first space met*/
		/*the first argument will be the operation chosen*/
		/*and the second will be the command*/
		operation = strtok(msgbuf, " ");
	
		/*action dependind on operation chosen*/
		if (strcmp(operation, "issuejob") == 0) {
			unknown_command = 0;
			parameter = strtok(NULL, "");
			if (parameter == NULL)
				printf("\"issuejob\" : missing argument\n");
			else {
				issuejob(parameter, &running_jobs_list, &queued_jobs_list, jobID, 1);
				jobID++;
			}
		}
		if (strcmp(operation, "setConcurrency") == 0) {
			unknown_command = 0;
			parameter = strtok(NULL, " ");
			N = atoi(parameter);
			sprintf(response_string, "%s %d", "Concurrency changed to :", N);
			response(response_string);
			/*function to update the lists used*/
			update_running();
		}
		if (strcmp(operation, "stop") == 0) {
			unknown_command = 0;
			parameter = strtok(NULL, " ");
			id = atoi(parameter);
			if (!stop(&running_jobs_list, id)) {
			/*the job is not running*/
				if (!stop_queued(&queued_jobs_list, id)) {
				/*the job is neither queued*/
					sprintf(response_string, "%s", "No job found to stop");
					response(response_string);
				}
				else {
				/*the job is queued*/
					stopped = atoi(parameter);
					sprintf(response_string, "%s %d", "Job stopped :", stopped);
					response(response_string);
				}
			}
			else {
			/*the job is running*/
				stopped = atoi(parameter);
				sprintf(response_string, "%s %d", "Job stopped :", stopped);
				response(response_string);
			}
		}
		if (strcmp(operation, "poll") == 0) {
			unknown_command = 0;
			parameter = strtok(NULL, " ");
			if (strcmp(parameter, "running") == 0) {
				print_running(running_jobs_list, args);
				send_response(args);
			}
			if (strcmp(parameter, "queued") == 0) {
				print_queued(queued_jobs_list, args);
				send_response(args);
			}
			i = 0;
			while (args[i] != NULL) {

				free(args[i]);
				args[i] = NULL;
				i++;
			}
		}
		if (strcmp(operation, "exit") == 0) {
			unknown_command = 0;
			exit_operation(fd);
		}

		fflush(stdout);
		if (unknown_command == 1) {
			sprintf(response_string, "%s", "Operation selected : unknown\n");
			response(response_string);
		}

		fflush(stdout);	
		
	}
}

/*write response to commander*/
/*repsonse is just a string*/
void response(char *args) {
	int fd2, nwrite, i;
	char msgbuf[MSGSIZE+1];

	if( (fd2=open(fifo2, O_WRONLY)) < 0) {
		perror("server writing: fifo open error");
		exit(1);
	}

	i = 0;
	strcpy(msgbuf, args);
	if ((nwrite=write(fd2, msgbuf, MSGSIZE+1)) == -1) {
		perror("server : Error in Writing");
		exit(2);
	}
	msgbuf[0] = '\0';
	strcpy(msgbuf, "exit");
	if ((nwrite=write(fd2, msgbuf, MSGSIZE+1)) == -1) {
			perror("server : Error in Writing");
			exit(2);
	}
	close(fd2);


}

/*write response to commander*/
/*response is an array of strings*/
void send_response(char **args) {
	int fd2, nwrite, i;
	char msgbuf[MSGSIZE+1];

	if( (fd2=open(fifo2, O_WRONLY)) < 0) {
		perror("commander writing: fifo open error");
		exit(1);
	}

	i = 0;
	while (args[i] != NULL) {
		strcpy(msgbuf, args[i]);
		if ((nwrite=write(fd2, msgbuf, MSGSIZE+1)) == -1) {
			perror("Commander : Error in Writing");
			exit(2);
		}
		i++;
	}
	msgbuf[0] = '\0';
	strcpy(msgbuf, "exit");
	if ((nwrite=write(fd2, msgbuf, MSGSIZE+1)) == -1) {
			perror("Commander : Error in Writing");
			exit(2);
	}
	close(fd2);

}

void exit_operation() {
	char response_string[100];
	unlink(server_file_name);
	sprintf(response_string, "%s", "Server is exiting...");
	response(response_string);
	exit(0);
}
