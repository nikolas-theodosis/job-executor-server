
typedef struct r_listnode *Running_List;

struct r_listnode{
	char job[100];
	int jobID;
	int pid;
	Running_List next;
};

typedef struct q_listnode *Queued_List;

struct q_listnode{
	char job[100];	
	int jobID;
	Queued_List next;
};

extern Running_List running_jobs_list;
extern Queued_List queued_jobs_list;

int count(Running_List *);
int count_queued(Queued_List*);
void addToRunning(Running_List*, int, char*, int);
void addToQueued(Queued_List*, int, char*);
void print_running(Running_List, char**);
void print_queued(Queued_List, char**);
int stop(Running_List*, int);
int remove_running(int, Running_List*);
int remove_queued();
int stop_queued(Queued_List*, int);
