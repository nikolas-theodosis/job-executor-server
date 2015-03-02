#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#include "list.h"
#include "jobs.h"

#define RUNNING "RUNNING"
#define QUEUED  "QUEUED"

void issuejob(char*, Running_List*, Queued_List*, int, int);

void issuejob(char *command, Running_List *running_jobs_list, Queued_List *queued_jobs_list, int jobID, int flag) {	
	char *args[64], job[strlen(command)], response_string[100];
	int i, running;
	pid_t pid;
	/*save job because the string will be split later*/
	strcpy(job, command); 
	/*split the command in arguments*/
	parse(command, args);
	signal(SIGCHLD, handler);
	/*count the running jobs*/
	running = count(running_jobs_list);
	/*there is space for running jobs*/
	if (running < N) {
	/*flag=1 means the function was called because*/
        /*another running process has ended and a queued will be transfered*/
        /*changing the answer the commander will receive*/
		if (flag == 1) {
			sprintf(response_string, "%d\t%s\t%s", jobID, job, "RUNNING");
			response(response_string);
		}
		else
			printf("%d\t%s\t%s\n", jobID, job, "RUNNING");
			/*create another process for the job*/
			if ((pid = fork()) < 0) { 
				perror("fork");
				exit(1);
			}
			if (pid == 0) { 
				/*execute the command*/
				execvp(*args, args); 
				perror(*args);
				exit(1);
			}
			else 
				/*add the job to the running ones*/
				addToRunning(running_jobs_list, jobID, job, pid);
		
	}
	/*there is no space left, so add the job to the queued ones*/
	else {
		sprintf(response_string, "%d\t%s\t%s", jobID, job, "QUEUED");
		response(response_string);
		addToQueued(queued_jobs_list, jobID, job);
	}
}

/*handler for terminated jobs*/
void handler(int sig) {
	int running, queued, i;
	char job[100];
	int jobID, status;
	pid_t pid;
	Queued_List list;

	/*find terminated children*/
	/*even if they terminate at the same time*/
	while ((pid = waitpid((pid_t)(-1), &status, WNOHANG)) > 0)
		remove_running(pid, &running_jobs_list);
		
   	 /*count the running jobs*/
	running = count(&running_jobs_list);
    	/*there is space in the list*/
	if (running < N) {
        	/*try to run as many jobs as the running list can take*/
		for (i=0 ; i<abs(N-running) ; i++) {
            	/*as long as there are queued jobs*/
			queued = count_queued(&queued_jobs_list);
			if (queued > 0) {
				list = queued_jobs_list;
				jobID = list->jobID;
				strcpy(job, list->job);
                		/*remove the oldest queued job*/
				remove_queued();
				issuejob(job, &running_jobs_list, &queued_jobs_list, jobID, 0);
			}
		}
	}
	
}

/*this function is called when concurrency is changed*/
/*checks if queued jobs can be executed*/
/*works almost like the handler above*/
void update_running() {
	int running, queued, i;
	char job[100];
	int jobID;
	Queued_List list;
	
	running = count(&running_jobs_list);
	if (running < N) {
		for (i=0 ; i<abs(N-running) ; i++) {
			queued = count_queued(&queued_jobs_list);
			if (queued > 0) {
				list = queued_jobs_list;
				jobID = list->jobID;
				strcpy(job, list->job);
				remove_queued();
				issuejob(job, &running_jobs_list, &queued_jobs_list, jobID, 0);
			}
		}
	}
	
}

/*splitting the command into arguments*/
void parse(char *buf, char **args) {
	while (*buf != '\0') {
		while ((*buf == ' ') || (*buf == '\t') || (*buf == '\n'))
			*buf++ = '\0'; 
		*args++ = buf;
		while ((*buf != '\0') && (*buf != ' ') && (*buf != '\t') && (*buf != '\n'))
			buf++; 
		*args = NULL;
	}
	
}
