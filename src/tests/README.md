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

### StressTest
A comprehensive stress and edge-case test that:
- **Exercises task and agent lifecycle management** under extreme conditions
- **Tests bulk operations**: Multiple agents/tasks created and removed in succession
- **Edge case testing**: Removes agents while tasks depend on them
- **Cascade operations**: Removes parent agents/tasks to verify children are properly handled
- **System recovery**: Creates new entities after bulk removals to verify system integrity
- **Dependency integrity**: Tests complex dependency chains and their invalidation
- **Verification**: Ensures the program remains functional after all stress scenarios

The test is structured in 9 distinct phases:
1. **Bulk Agent Creation**: Creates multiple root agents with nested hierarchies
2. **Bulk Task Creation**: Creates tasks with various agent dependencies
3. **Edge Case - Remove Agent with Dependencies**: Removes agents that have active dependent tasks
4. **Edge Case - Cascade Agent Removal**: Removes parent agents with nested children
5. **Edge Case - Cascade Task Removal**: Removes parent tasks with active children
6. **Bulk Removal Operations**: Sequential removal of multiple entities
7. **System Recovery**: Creates new agents and tasks after removals
8. **Dependency Integrity**: Tests complex dependency chains and their invalidation
9. **Final Validation**: Verifies system functionality and consistency

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
#include "src/tests/stresstest.h"

// Setup Qt application...
auto settings = Settings::load();
MainPanel panel(settings);

// Run SimpleTest
SimpleTest simpleTest(&panel);
simpleTest.run();

// Or run DependencyTest
DependencyTest depTest(&panel);
depTest.run();

// Or run StressTest
StressTest stressTest(&panel);
stressTest.run();
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

## What StressTest Adds

The StressTest was created to meet the requirement:
> "Create a new stress and edge-case test that extends the same testing philosophy, just like DependencyTest or SimpleTest."

This test demonstrates:
- **Stress testing**: Bulk additions and removals of tasks and agents
- **Edge case handling**: Removing entities with active dependencies
- **Cascade operations**: Parent removal triggering child cleanup
- **State transitions**: Tasks moving between states (Ready → WaitingForAgent)
- **System resilience**: Recovery and continued functionality after invalidation
- **Deterministic behavior**: All operations are predictable and reproducible
- **Loud failures**: Uses QVERIFY assertions that fail immediately if invariants break

Key differences from other tests:
- More aggressive testing with bulk operations
- Explicit edge case scenarios (removal during active dependencies)
- System recovery validation
- Focus on lifecycle management rather than just creation

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

### Handling Confirmation Dialogs (StressTest)
The StressTest includes removal operations, which require handling confirmation dialogs:

The test uses the `handleConfirmDialog` lambda which:
1. Waits for a QMessageBox with objectName "confirmdialog" to appear
2. Locates the "Yes" button in the confirmation dialog
3. Clicks the "Yes" button to confirm the deletion
4. Waits for the dialog to close

This is essential because:
- The `removeTask()` and `removeAgent()` actions in MainPanel show confirmation dialogs
- Without clicking "Yes", the removal operations don't actually execute
- The confirmation dialogs ask "Are you sure to delete this task/agent?"
- This ensures the test actually performs the removals it intends to test

### Agent Path Format
Agent dependencies are specified using a Unix-like path format:
- `/System` - root level agent
- `/System/CPU` - child agent under System
- `/Network/Ethernet` - child agent under Network

This format matches how the AgentModel's `toString()` method represents agent paths.
