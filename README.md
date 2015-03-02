# job-executor-server
jobs (commands) being executed by server while controlling the flow control of the jobs, based on a given concurrency

Makefile is included to compile the jobCommander program.
Run the jobCommander with the following parameters:

* ./jobCommander issuejob <job>  
e.g ./jobCommander issuejob ls -l  
(Executes the <job command>)

* ./jobCommander setConcurrency <N>
e.g ./jobCommander setConcurrency 4  
(Changes the concurrency of the jobs that can be run simultaneously)

* ./jobCommander stop <jobID>
e.g ./jobCommander stop 2  
(Stops the job with the <jobID> assigned)

* ./jobCommander poll [running,queued]  
e.g ./jobCommander poll running  
(Prints the pattern <jobID, job> of every running or queued job)

* ./jobCommander exit  
(The jobExecutorServer exits)  


Also two bash scripts are included.  
You can send jobs to jobExecutorServer from files including jobs.  

Execute:  
./multijob.sh file1.txt file2.txt  
to run the jobs included in the two text files  

Finally, you can stop all the running and queued jobs by executing:  
./allJobsStop.sh
