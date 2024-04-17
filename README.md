# OS Process Scheduler Simulation

This C program simulates a basic operating system process scheduler using a simple three-state process model (READY, RUNNING, BLOCKED). The scheduler implements functionalities for managing process states based on CPU and I/O operations.

## Compilation

Compile the program with the following command:

```bash
gcc -std=c99 -Wall -o oslab oslab.c
```

Ensure that you use the `-std=c99` flag to compile the program as it uses C99 standard features.

## Usage

```bash
./oslab filename
```

- `filename`: This is the name of the file containing the process information which will be used by the scheduler.

## Program Flow

1. **Initialization**: The program starts by checking the command line arguments and opens the specified input file containing process data.
2. **Process File**: It reads the input file to populate the process details into a list.
3. **Scheduling Loop**: The main loop runs until all processes are completed. In each cycle:
   - It chooses a process to run based on the scheduler function.
   - Updates the state of the running process and blocked processes.
   - Prints the current state of processes.
   - Increments the time.
4. **Completion**: Outputs final statistics including CPU utilization and turnaround time for each process.

## Input File Format

The input file should contain the number of processes and details for each process in the following format:

```
<number_of_processes>
<pid> <cpu1> <io> <cpu2> <arrival_time>
...
```

- `pid`: Process ID.
- `cpu1`: Time for the first CPU burst.
- `io`: Time for the I/O burst.
- `cpu2`: Time for the second CPU burst.
- `arrival_time`: Time at which the process arrives in the scheduler.

## Output

The program outputs the process state at each time unit into a file named `<inputfilename><number_of_processes>.out`.

## Function Descriptions

- `void process_file(FILE *)`: Reads process data from the input file.
- `int scheduler(int)`: Determines the next process to be scheduled.
- `int update_running_process(int, int)`: Updates the state of the currently running process.
- `void update_blocked_processes(int)`: Updates blocked processes based on I/O time.
- `void print_processes(FILE *, int)`: Prints the current state of all processes.

## Notes

- Do not remove any provided fields in the `process_info` struct.
- Feel free to add additional fields or functions as necessary for your specific scheduling needs.

## Contributing

Contributions to improve the simulation or add new features are welcome. Please ensure to maintain the coding standards and provide comments wherever necessary.
