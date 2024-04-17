// must compile with: gcc  -std=c99 -Wall -o oslab oslab.c
// finished

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/* WE will use the three-state model */
#define READY   0
#define RUNNING 1
#define BLOCKED 2
#define MAXNUM 9999 //pid must be less than this number

/* string of process states */
char states[3][10] = {"ready", "running", "blocked"};

int num_completed_processes = 0;
/* Information about each process */
/* Important: Feel free to add any new fields you want. But do not remove any of the cited one. */
struct process_info{
	int pid; // process ID, cannot be negative
	int state; //state of the process: ready, running, blocked
	int arrival_time; // time the process arrives
	int complete_time; //time the process finishes
	int turnaround_time; // complete_time - arrival_time
	int cpu1; //first computing interval
	int io; // io interval
	int cpu2; // second computing interval
	int done; // This flag is initialized to 0 and is set to 1 when the process is done
	int remaining_cpu1;
	int remaining_cpu2;
	int remaining_io;
};

/**** Global Variables ***/

struct process_info * process_list; // array containing all processes info
int num_processes = 0;  // total number of processes
int running_process_index = -1; // index of the process currently on the cpu
int cpu_active = 0; // Incremented each cycle the CPU is busy

/* You will implement the following functions */
/* Feel free to add more functions if you need. */

void process_file(FILE *); // Reads the input file and fill the process_list

int scheduler(int); // Returns the next process to be scheduled for execution (i.e. its state becomes RUNNING)
int update_running_process(int, int);
void update_blocked_processes(int); //decrement the io field in all blocked processes. 
void print_processes(FILE *, int); //print blocked/ready processes into the output file
// void print_processes(FILE *output, int time, int running_process_index);

/**********************************************************************/

int main(int argc, char *argv[]){
  
FILE * fp; //for creating the output file
FILE * output; // output file
char filename[100]=""; // the output file name
int time; //The clock
int i;

// Check that the command line is correct
if(argc != 2){

  printf("usage:  ./oslab filename\n");
  printf("filename: the processese information file\n");
  exit(1);
}

// Process the input command line. 

// Check that the file specified by the user exists and open it
if( !(fp = fopen(argv[1],"r")))
{
  printf("Cannot open file %s\n", argv[1]);
  exit(1);
}

// process input file
process_file(fp);

// form the output file name: original name + number of processes + .out
strcat(filename,argv[1]); 
sprintf(filename,"%s%d",filename, num_processes );
if( !(output = fopen(filename,"w")))
{
  printf("Cannot open file %s\n", filename);
  exit(1);
}

// The main loop
time = 0;
while(num_completed_processes < num_processes){
    running_process_index = scheduler(time); //using the scheduler to choose the current running index
    running_process_index = update_running_process(running_process_index, time);
    print_processes(output, time);
    update_blocked_processes(time);

    for (i = 0; i < num_processes; i++) {
		// we're first checking to see if we can mark the process as completed 
        if (process_list[i].remaining_cpu2 == 0 && process_list[i].done == 0) {
            // we can now mark the process as "done" and note down the respective times
            process_list[i].done = 1;
			process_list[i].complete_time = time;
			process_list[i].turnaround_time = process_list[i].complete_time - process_list[i].arrival_time;

			process_list[i].state = READY;
            
			//we also need to keep track of the completed processes, so we should increment this variable by one
            num_completed_processes= num_completed_processes+1;

            // resetting the running process index
            if (process_list[i].pid == running_process_index) {
                running_process_index = -1;
            }

        } else if (process_list[i].remaining_cpu1 == 0 && process_list[i].remaining_io > 0) {
            // if the first CPU running time has been completed, we now need to put the process into io and set its state as blocked
            process_list[i].state = BLOCKED;

            if (process_list[i].pid == running_process_index) {
                running_process_index = -1;
            }
        }
    }
    time = time+1; // time increases
}


printf("num processes = %d\n", num_processes);
printf("CPU utilization = %f\n", (float)cpu_active/(float)time);
printf("total time = %d\n", time);
for(i = 0; i < num_processes; i++)
	printf("process %d: turnaround time = %d\n", process_list[i].pid, process_list[i].turnaround_time );

// close the processes file
fclose(fp);
fclose(output);
free(process_list);

return 0;
}

int scheduler(int time) {
    if (running_process_index != -1) {
        for (int i = 0; i < num_processes; i++) {
            if (process_list[i].pid == running_process_index && process_list[i].state == RUNNING) {
                process_list[i].state = READY;
                break; // there can only be one runnng process, so we can just break if we find it
            }
        }
    }

    int nextPID = -1; //no suitable process found yet
    for (int i = 0; i < num_processes; i++) {
        if (process_list[i].arrival_time <= time && process_list[i].done == 0 && process_list[i].state == READY) {
            if (nextPID == -1 || process_list[i].pid < process_list[nextPID].pid) {
                nextPID = i;
            }
        }
    }

    if (nextPID != -1) {
        process_list[nextPID].state = RUNNING;
        return process_list[nextPID].pid; 
    }
    return -1;
}

void update_blocked_processes(int time) {
    int i = 0; 
    while (i < num_processes) { // we continue to loop as long as i is less than num_processes.
        if (process_list[i].state == BLOCKED) {
            process_list[i].remaining_io--;
            if (process_list[i].remaining_io == 0) { //this means that the io is finished (can be put back to ready state now)
                process_list[i].state = READY;
            }
        }
        i = i+1;
    }
}

int update_running_process(int pid, int time) {
    for (int i = 0; i < num_processes; i++) {
        if (process_list[i].pid == pid) {
            if (process_list[i].remaining_cpu1 > 0) {
                process_list[i].remaining_cpu1--;
            } else if (process_list[i].remaining_cpu2 > 0) { 
                process_list[i].remaining_cpu2--;
            }
            cpu_active= cpu_active+1;
            break;
        }
    }
    return pid;
}


void print_processes(FILE *fp, int time) {
    fprintf(fp, "time %d:\n", time);
    int i = 0; 
    while (i < num_processes) { // we continue looping as long as i is less than num_processes.
        if (process_list[i].done == 0 && time >= process_list[i].arrival_time) {
            fprintf(fp, "Process: %d : %s\n", process_list[i].pid, states[process_list[i].state]);
        }
        i = i+1; 
    }
}

/**********************************************************************/
/* The following function does the following:
- Reads the number of process from the input file and save it in the global variable: num_processes
- Allocates space for the processes list: process_list
- Read the file and fill up the info in the process_info array
- Keeps reading the file and fill up the information in the process_list 
*/
void process_file(FILE * fp){

int i = 0;
fscanf(fp, "%d", &num_processes);	

if((process_list = malloc(num_processes * sizeof(struct process_info) ))== NULL){
	printf("Failure to allocate process list of %d processes\n", num_processes);
	exit(1);
}

while( fscanf(fp,"%d %d %d %d %d", &process_list[i].pid, &process_list[i].cpu1, &process_list[i].io, &process_list[i].cpu2, &process_list[i].arrival_time) == 5){ 
	
	process_list[i].done = 0;
	process_list[i].remaining_cpu1 = process_list[i].cpu1;
	process_list[i].remaining_cpu2 = process_list[i].cpu2;
	process_list[i].remaining_io = process_list[i].io;
	process_list[i].state = READY;
	i++;
   }	
}

/**********************************************************************/
