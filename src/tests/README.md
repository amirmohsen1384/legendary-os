# LegendaryOS Tests

This directory contains GUI tests for the LegendaryOS project.

## Available Tests

### SimpleTest
A basic GUI test that:
- Creates multiple top-level tasks with random priorities and burst times
- Creates child tasks under each top-level task
- Does NOT create any agents
- Does NOT set agent dependencies on tasks

### DependencyTest
An advanced GUI test that:
- Creates a hierarchical agent tree structure with:
  - System (CPU, Memory)
  - Network (Ethernet, WiFi)  
  - Storage (Disk, Cache)
- Creates tasks with agent dependencies:
  - Compute Task → depends on /System/CPU
  - Memory Task → depends on /System/Memory
  - Network Task → depends on /Network/Ethernet
  - Storage Task → depends on /Storage/Disk
- Creates child tasks with their own agent dependencies

## Running Tests

To run a test, you need to:

1. Create an instance of MainPanel
2. Create an instance of the test class, passing the MainPanel
3. Call the `run()` method

Example:
```cpp
#include "src/panels/mainpanel.h"
#include "src/tests/simpletest.h"
#include "src/tests/dependencytest.h"

// Setup Qt application...
auto settings = Settings::load();
MainPanel panel(settings);

// Run SimpleTest
SimpleTest simpleTest(&panel);
simpleTest.run();

// Or run DependencyTest
DependencyTest depTest(&panel);
depTest.run();
```

## Test Structure

Both tests use the Qt Test framework and follow a similar pattern:

1. Show and activate the main panel window
2. Get action references (e.g., actionInsertTask, actionInsertAgent)
3. Define lambda functions to automate dialog interactions
4. Trigger actions and fill in dialog forms programmatically
5. Wait for operations to complete using QTest::qWaitFor()
6. Verify results with QVERIFY assertions

## What DependencyTest Adds

The DependencyTest was created to meet the requirement:
> "create some tests which not only create some tasks with some dependency to an agent, but also create some agents in the agent tree model"

This test demonstrates:
- How to programmatically create agents in the agent tree
- How to create hierarchical agent structures (parent-child relationships)
- How to create tasks that depend on specific agents
- How to set the agent path for a task using the format "/Parent/Child"

## Implementation Details

### Creating Agents
The test uses the `createAgentViaDialog` lambda which:
1. Triggers the insertAgent action
2. Waits for AgentEdit dialog to appear
3. Fills in name and description fields
4. Optionally selects a parent agent in the tree
5. Clicks OK to create the agent

### Creating Tasks with Dependencies
The test uses the `createTaskViaDialog` lambda which:
1. Triggers the insertTask action
2. Waits for TaskEdit dialog to appear
3. Fills in task properties (name, priority, burst time)
4. Sets the agent path in the agentEdit field (e.g., "/System/CPU")
5. Optionally selects a parent task in the tree
6. Clicks OK to create the task

### Agent Path Format
Agent dependencies are specified using a Unix-like path format:
- `/System` - root level agent
- `/System/CPU` - child agent under System
- `/Network/Ethernet` - child agent under Network

This format matches how the AgentModel's `toString()` method represents agent paths.
