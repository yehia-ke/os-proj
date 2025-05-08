# OS-Project

## Overview
The **OS-Project** simulates key operating system functionalities, focusing on process management, scheduling, and synchronization. Written in C, the project implements multiple scheduling algorithms, process synchronization mechanisms, and a graphical user interface (GUI) for real-time interaction and visualization.

## Features
- **Dynamic Scheduling Algorithms**:
  - **First-Come, First-Serve (FCFS)**: Processes are executed in the order they arrive.
  - **Round Robin (RR)**: Time-sliced scheduling for fair CPU sharing.
  - **Multi-Level Feedback Queue (MLFQ)**: Dynamically adjusts process priorities based on behavior.
- **Manual and Automatic Clock Execution**:
  - **Manual Clock**: Step through each cycle one at a time.
  - **Automatic Clock**: Executes continuously with a defined interval.
- **Process Synchronization**:
  - Mutexes (`userInput`, `userOutput`, `file`) with `semWait` and `semSignal` operations for resource management.
- **Graphical User Interface (GUI)**:
  - Visualize process states, memory allocation, and resource usage.
  - Interact with the system by adding processes, selecting schedulers, and monitoring clock cycles.
- **Memory Management**:
  - Allocate and track memory usage for each process.

## Project Structure
### Key Components:
1. **Process Management**:
   - Processes are represented as **Process Control Blocks (PCBs)**, storing metadata like memory allocation, program counter, and state.

2. **Scheduling**:
   - **FCFS Scheduler**: Executes processes in the order they arrive. No preemption is involved.
   - **RR Scheduler**: Uses a fixed time quantum for each process before moving to the next.
   - **MLFQ Scheduler**: Adjusts process priorities dynamically and handles both CPU-bound and I/O-bound processes efficiently.

3. **Synchronization**:
   - Mutexes ensure safe access to shared resources.
   - Implements `semWait` to lock a resource and `semSignal` to release it.

4. **Interpreter**:

   The table below lists all the commands that the interpreter can handle. Any command outside of this table will not be executed by the interpreter.

      | Command         | Arguments                 | Description                                                                                          |
      |-----------------|--------------------------|------------------------------------------------------------------------------------------------------|
      | `semWait`       | `userInput`, `userOutput`, `file` | Acquires the specified mutex.                                                                       |
      | `semSignal`     | `userInput`, `userOutput`, `file` | Releases the specified mutex.                                                                       |
      | `assign`        | `a input`, `b input`     | Assigns user input to variable `a` or `b`.                                                          |
      |                 | `b readFile`            | Reads data from a file into variable `b`.                                                           |
      |                 | `a b`, `b a`            | Assigns the value of one variable to another.                                                       |
      | `print`         | `a`, `b`, `c`           | Prints the value of the specified variable (`a`, `b`, or `c`).                                       |
      | `printFromTo`   | `start`, `end`          | Prints all numbers from `start` to `end`.                                                           |
      | `writeFile`     | `filename`, `data`      | Writes `data` to the specified `filename`.                                                          |
      | `exit`          | None                    | Terminates the current process.                                                                     |

   ## Notes:
   - Commands like `semWait` and `semSignal` are used for synchronization to manage access to shared resources.
   - The `assign` command supports variable assignments, including reading from files.
   - The `printFromTo` command allows printing a range of numbers, either ascending or descending.
    
   For further details, refer to the implementation in [interpreter.c](https://github.com/yehia-ke/os-proj/blob/efc55d4efde3d3f6d6d5c6d6596d2a6aacd3da96/src/interpreter/interpreter.c).


5. **GUI**:
   - Built using GTK, the GUI provides an interactive platform for managing and monitoring the system.

6. **Clock Modes**:
   - **Manual Clock**: Allows step-by-step execution, controlled by the user.
   - **Automatic Clock**: Executes cycles automatically at 1-second intervals.

7. **Programs**:
   - Sample programs in the `programs/` directory demonstrate the system's capabilities.

### Directory Structure:
- **`src/`**: Contains the source code for different modules.
  - `main.c`: Entry point and GUI-related functions.
  - `fcfs/`: First-Come, First-Serve scheduler implementation.
  - `rr/`: Round Robin scheduler implementation.
  - `mlfq/`: Multi-Level Feedback Queue scheduler implementation.
  - `interpreter/`: Code for interpreting and executing process instructions.
  - `pcb/`: Process Control Block implementation.
  - `scheduler/`: Scheduler management and integration.
- **`programs/`**: Sample programs for process simulation.
- **`Makefile`**: Build and clean automation.

## Getting Started

### Prerequisites
- GCC compiler
- GTK library for the GUI (install via `sudo apt install libgtk-3-dev` on Linux)

### Installation
1. Clone the repository:
   ```bash
   git clone https://github.com/yehia-ke/os-proj.git
   ```
2. Navigate to the project directory:
   ```bash
   cd os-proj
   ```
3. Compile the project:
   ```bash
   make
   ```

### Usage
1. Run the program:
   ```bash
   make run
   ```
2. Use the GUI to:
   - Add processes by specifying their arrival time and program file.
   - Select a scheduling algorithm (FCFS, RR, or MLFQ).
   - Switch between Manual and Automatic clock modes.
   - Monitor process execution, memory allocation, and mutex states.

### Debugging
To debug the program using GDB:
```bash
make debug
```

## Example Programs
Sample programs are located in the `programs/` directory. Here's an example:
```text
semWait userInput
assign a input
assign b input
semSignal userInput
semWait userOutput
printFromTo a b
semSignal userOutput
```

## Contributors
Special thanks to all contributors for their valuable input and support:

- [RemoteShift](https://github.com/RemoteShift)
- [yehia-ke](https://github.com/yehia-ke)
- [YusufZoair](https://github.com/YusufZoair)
- [Dohamaged2004](https://github.com/Dohamaged2004)
- [KhaledKorayem](https://github.com/KhaledKorayem)
- [Hassan]()
- [Loujain]()
