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


/*count running jobs*/
int count(Running_List *jobs_list) {
	int count = 0;
	Running_List templist;
	templist = *jobs_list;
	if (templist == NULL)
		return 0;
	while (templist != NULL) {
		count++;
		templist = templist->next;
	}
	return count;
}

/*count queued jobs*/
int count_queued(Queued_List *list) {
	int count = 0;
	Queued_List templist;
	templist = *list;
	if (templist == NULL)
		return 0;
	while (templist != NULL) {
		count++;
		templist = templist->next;
	}
	return count;
}

/*add job to running jobs list*/
void addToRunning(Running_List *running_jobs_list, int jobID, char *job, int pid) {
	Running_List templist;
	templist = *running_jobs_list;
	/*first running job*/
	if (templist == NULL) {
		*running_jobs_list = malloc(sizeof(struct r_listnode));
		(*running_jobs_list)->jobID = jobID;
		(*running_jobs_list)->pid = pid;
		strcpy((*running_jobs_list)->job, job);
		(*running_jobs_list)->next = templist; 
	}
	/*not the first job in the list*/
	else {
		while (*running_jobs_list != NULL)
			running_jobs_list = &((*running_jobs_list)->next);
		*running_jobs_list = malloc(sizeof(struct r_listnode));
		(*running_jobs_list)->jobID = jobID;
		(*running_jobs_list)->pid = pid;
		strcpy((*running_jobs_list)->job, job);
		(*running_jobs_list)->next = NULL; 
	}
}

/*add job to queued jobs list*/
void addToQueued(Queued_List *list, int jobID, char *job) {
	Queued_List templist;
	templist = *list;
	/*first running job*/
	if (templist == NULL) {
		*list = malloc(sizeof(struct q_listnode));
		(*list)->jobID = jobID;
		strcpy((*list)->job, job);
		(*list)->next = templist; 
	}
	/*not the first job in the list*/
	else {
		while (*list != NULL)
		list = &((*list)->next);
		*list = malloc(sizeof(struct q_listnode));
		(*list)->jobID = jobID;
		strcpy((*list)->job, job);
		(*list)->next = NULL; 
	}
}

/*print running jobs*/
void print_running(Running_List list, char **args) {
	int i = 0;
	char buffer[500];
	
	while (list != NULL) {
		sprintf(buffer, "%d\t%s", list->jobID, list->job);
		args[i] = malloc(sizeof(char)*(strlen(buffer)+1));
		strcpy(args[i], buffer);
		i++;
		list = list->next;
	}
	args[i] = NULL;
	
}

/*print queued jobs*/
void print_queued(Queued_List list, char **args) {
	int i;
	char buffer[500];
	i = 0;
	while (list != NULL) {
		sprintf(buffer, "%d\t%s", list->jobID, list->job);
		args[i] = malloc(sizeof(char)*(strlen(buffer)+1));
		strcpy(args[i], buffer);
		i++;
		list = list->next;
	}
	args[i] = NULL;
}

/*stop running job*/
int stop(Running_List *list, int jobID) {
	Running_List templist;
	while ((*list) != NULL) {
		if ((*list)->jobID == jobID) {
			templist = *list;
			kill((*list)->pid, SIGKILL);
			*list = (*list)->next;
			free(templist);
			return 1;
		}
		else
			list = &((*list)->next);
	}
	return 0;
}

/*stop queued job*/
int stop_queued(Queued_List *list, int jobID) {
	Queued_List templist;
	while ((*list) != NULL) {
		if ((*list)->jobID == jobID) {
			templist = *list;
			*list = (*list)->next;
			free(templist);
			return 1;
		}
		else
			list = &((*list)->next);
	}
	return 0;
}

/*remove running job from list*/
int remove_running(int pid, Running_List *list) {
	Running_List templist;
	while (*list != NULL) {
		if ((*list)->pid == pid) {
			templist = *list;
			*list = (*list)->next;
			free(templist);
			return 1;
		}
		else
			list = &((*list)->next);
	}
	return 0;
}

/*remove queued job from list*/
int remove_queued() {
	Queued_List templist;
	if ((queued_jobs_list) != NULL) {
		templist = queued_jobs_list;
		queued_jobs_list = (queued_jobs_list)->next;
		free(templist);
		return 1;
	}
	return 0;
}
