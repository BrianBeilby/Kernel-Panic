# Kernel Panic - Simple Operating System

## Overview
Kernel Panic is a lightweight, educational operating system designed for academic purposes within the CSC159 - Operating System Pragmatics course at Sacramento State University. This OS provides a hands-on approach to understanding the lower-level workings of modern operating systems.

## Special Environment
Kernel Panic is developed and tested exclusively in SpedeVM with Oracle [VirtualBox](https://www.virtualbox.org/), a specialized virtual machine environment (64-bit x86). This environment may have unique configurations and dependencies that could affect Kernel Panic's performance or functionality in other settings.

### Caution
- **Environment Specific**: Kernel Panic is tailored for SpedeVM and might not function as intended in other environments.
- **Compatibility Issues**: There may be compatibility issues when attempting to run this OS in environments other than SpedeVM due to specific hardware and software expectations.

## Features
- **Kernel Management**: Core functionalities managed within the kernel module.
- **Concurrency**: Implements basic process management, mutexes, and semaphores.
- **Device Drivers**: Includes drivers for keyboard, VGA, and timer.

## Project Structure
- `include/`: Contains all the header files necessary for the OS.
- `src/`: Contains the implementation files (.c and .S) for the OS functionalities.

### Key Components
- **Kernel**: Heart of the operating system (`kernel.c`).
- **Scheduler**: Manages process scheduling (`scheduler.c`).
- **Syscalls**: Interface for system calls (`syscall.c`).
- **Interrupt Handling**: Manages hardware and software interrupts, crucial for responding to events like hardware signals and system calls (`interrupts.c`).

## Getting Started
### Prerequisites
- To run Kernel Panic, you will need a virtual machine that can emulate an x86 architecture. While Kernel Panic was specifically developed for SpedeVM, it may be possible to adapt it for use in other VM environments with some adjustments.

### Building the OS
1. **Clone the repository**: `git clone (https://github.com/BrianBeilby/Kernel-Panic.git)`
2. **Navigate to the project directory**: Change into the directory where Kernel Panic has been cloned.
3. **Build the project**: Run `make` to compile the OS. Ensure you have a GCC cross-compiler set up for x86 development.

### Running the OS
- To run Kernel Panic, you will need to load the compiled binary in your VM setup. Specific instructions can vary based on the VM you are using.
