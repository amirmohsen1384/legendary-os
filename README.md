# Operating System Simulator

Course project for **Data Structures**, Faculty of Computer Engineering – University of Isfahan
Winter 2025–2026

## Project Introduction

The **Operating System Simulator** is a single-processor system based on quantum scheduling that operates cyclically and processes commands in batches. The system simulates process execution over discrete time intervals.

In each cycle, the system first receives commands from the input. During the execution phase, the processor is then allocated to processes according to the scheduling algorithms. The simulator is responsible for managing the process hierarchy and a tree-based file system.

The program automatically manages the dependencies between processes and system resources and continues executing cycles until it receives the **SHUTDOWN** command.

## Project Objectives

* Implement and manage process hierarchies using a **General Tree** data structure.
* Understand and practically implement **Priority Queues** using a **Max-Heap**.
* Manage dynamic dependencies between processes and the file system.
* Simulate different process states, including **READY**, **RUNNING**, and **WAITING**.
* Practice **Exception Handling** concepts in the context of an operating system.
* Follow clean coding principles and structured design for system resource management.

## Overall Project Structure

| System Component  | Responsibility                                                       | Data Structure            |
| :---------------- | :------------------------------------------------------------------- | :------------------------ |
| **PCB**           | Stores the identity, priority, and state of each process             | Class / Object            |
| **Process Tree**  | Manages parent-child relationships and hierarchical process deletion | General Tree              |
| **Ready Queue**   | Manages processes that are ready for execution based on priority     | Priority Queue (Max-Heap) |
| **Waiting Queue** | Stores processes waiting for files or capacity availability          | Simple Queue (FIFO)       |
| **File Tree**     | Models the file and directory structure for process access           | General Tree              |


## Features

* Supports **quantum-based scheduling** and fair processor allocation among processes.
* **Automatically updates process priority** after a timeout based on the state of its **siblings**.
* Manages the process hierarchy so that deleting a parent automatically deletes all of its children.
* Performs a thorough check of the required file access permissions before allocating the processor during each time quantum.
* Prevents **starvation** through a gradual priority-increase mechanism known as **Aging**.
* Provides final statistical output, including **CPU utilization** and the **average waiting time** of processes.
* Displays the Linux-style file structure with standard indentation similar to the Linux `tree` command.
* Supports a **graphical user interface (GUI)** for real-time monitoring of queue and process states.

## System Configuration

When the operating system starts, it loads its operational parameters from the following configuration file:

```json
{
  "quantumSize": 20,
  "executionQuantumsPerCycle": 5,
  "inputCommandLimit": 3,
  "Ready_Limit": 5,
  "waitingQueueLimit": 10
}
```

</div>
