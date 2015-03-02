
extern int MSGSIZE;
extern int N;
extern char *fifo;
extern char *fifo2;
extern char *server_file_name;

void jobExecutorServer();
void jobCommander(char**, int);
void exit_operation();
void parse(char*, char**);
void handler(int);
void send_response(char**);
void response(char*);
