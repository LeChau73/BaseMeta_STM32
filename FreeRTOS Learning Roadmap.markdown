# FreeRTOS Learning Roadmap

This roadmap outlines a step-by-step guide to mastering the FreeRTOS Real-Time Kernel based on the provided document, "Mastering the FreeRTOS Real-Time Kernel." It covers key concepts, features, and practical applications, organized by the document's chapters and sections. Each step focuses on understanding and applying specific FreeRTOS components and techniques.

---

## Step 1: Understand FreeRTOS Fundamentals
- **Objective**: Gain a foundational understanding of FreeRTOS and its role in embedded systems.
- **Tasks**:
  - Read Section 1.1: *Multitasking in Small Embedded Systems*.
    - Understand the concept of multitasking in embedded systems.
    - Learn the difference between hard and soft real-time requirements.
  - Study Section 1.1.1: *About the FreeRTOS Kernel*.
    - Explore the history and development of FreeRTOS.
    - Understand its suitability for microcontrollers and small microprocessors.
  - Review Section 1.1.2: *Value Proposition*.
    - Learn why FreeRTOS is widely used (professional development, MIT license, AWS stewardship).
  - Study Section 1.1.3: *A Note About Terminology*.
    - Familiarize yourself with FreeRTOS terminology (e.g., tasks vs. threads).
  - Read Section 1.1.4: *Why Use an RTOS?*.
    - Understand the benefits of using an RTOS (e.g., timing abstraction, modularity, efficiency).
  - Explore Section 1.1.5: *FreeRTOS Kernel Features*.
    - List and understand key features (e.g., preemptive/cooperative scheduling, queues, semaphores).
  - Review Section 1.1.6: *Licensing, and The FreeRTOS, OpenRTOS, and SafeRTOS Family*.
    - Understand the MIT license and differences between FreeRTOS, OpenRTOS, and SafeRTOS.

---

## Step 2: Set Up the FreeRTOS Environment
- **Objective**: Learn how to obtain, organize, and build FreeRTOS projects.
- **Tasks**:
  - Read Section 2.1: *Introduction*.
    - Understand the structure of the FreeRTOS distribution.
  - Study Section 2.2: *Understanding the FreeRTOS Distribution*.
    - Learn about FreeRTOS ports (Section 2.2.1).
    - Understand how to build FreeRTOS (Section 2.2.2).
    - Study the role of *FreeRTOSConfig.h* (Section 2.2.3) and its configuration constants.
    - Explore official distributions (Section 2.2.4) and source files (Sections 2.2.6–2.2.7).
    - Learn about include paths and header files (Sections 2.2.8–2.2.9).
  - Review Section 2.3: *Demo Applications*.
    - Understand the purpose of demo applications and how to use them as references.
  - Study Section 2.4: *Creating a FreeRTOS Project*.
    - Learn how to adapt a demo project (Section 2.4.1).
    - Explore creating a new project from scratch (Section 2.4.2).
  - Read Section 2.5: *Data Types and Coding Style Guide*.
    - Understand FreeRTOS data types, variable/function naming, formatting, and macro conventions.
  - **Practical**:
    - Download the FreeRTOS zip file from https://www.FreeRTOS.org/Documentation/code.
    - Set up a development environment (e.g., Visual Studio Community Edition for the Windows port).
    - Build and run a demo application to verify setup.

---

## Step 3: Master Heap Memory Management
- **Objective**: Understand memory allocation schemes in FreeRTOS.
- **Tasks**:
  - Read Section 3.1: *Introduction*.
    - Understand the prerequisites and scope of memory management.
    - Learn about static vs. dynamic memory allocation (Section 3.1.3).
  - Study Section 3.2: *Example Memory Allocation Schemes*.
    - Explore *Heap_1* to *Heap_5* schemes (Sections 3.2.1–3.2.5).
    - Learn how to initialize *Heap_5* using *vPortDefineHeapRegions()* (Section 3.2.6).
  - Review Section 3.3: *Heap Related Utility Functions and Macros*.
    - Study functions like *xPortGetFreeHeapSize()*, *xPortGetMinimumEverFreeHeapSize()*, and *vPortGetHeapStats()*.
    - Understand malloc failed hook functions and per-task heap usage statistics.
  - Explore Section 3.4: *Using Static Memory Allocation*.
    - Learn how to enable static memory allocation (Section 3.4.1).
    - Understand static internal kernel memory for timer and idle tasks (Section 3.4.2).
  - **Practical**:
    - Implement a project using *Heap_4* and monitor free heap size with *xPortGetFreeHeapSize()*.
    - Experiment with static memory allocation for a task and verify its behavior.

---

## Step 4: Learn Task Management
- **Objective**: Master task creation, prioritization, and state management.
- **Tasks**:
  - Read Section 4.1: *Introduction* and Section 4.2: *Task Functions*.
    - Understand task functions and their structure.
  - Study Section 4.3: *Top Level Task States*.
    - Learn about task states (Running, Ready, Blocked, Suspended).
  - Explore Section 4.4: *Task Creation*.
    - Study the *xTaskCreate()* API function (Section 4.4.1).
  - Review Section 4.5: *Task Priorities*.
    - Understand generic and architecture-optimized schedulers (Sections 4.5.1–4.5.2).
  - Study Section 4.6: *Time Measurement and the Tick Interrupt*.
    - Learn about the tick interrupt and time management.
  - Explore Section 4.7: *Expanding the Not Running State*.
    - Understand Blocked, Suspended, and Ready states (Sections 4.7.1–4.7.3).
    - Study the *vTaskDelayUntil()* API function (Section 4.7.5).
  - Review Section 4.8: *The Idle Task and the Idle Task Hook*.
    - Learn about idle task hook functions and their limitations (Sections 4.8.1–4.8.2).
  - Study Section 4.9: *Changing the Priority of a Task*.
    - Explore *vTaskPrioritySet()* and *uxTaskPriorityGet()* APIs (Sections 4.9.1–4.9.2).
  - Review Section 4.10: *Deleting a Task*.
    - Learn about the *vTaskDelete()* API function (Section 4.10.1).
  - Study Section 4.11: *Thread Local Storage and Reentrancy*.
    - Understand thread local storage implementations (Sections 4.11.1–4.11.3).
  - Explore Section 4.12: *Scheduling Algorithms*.
    - Learn about prioritized preemptive scheduling (with/without time slicing) and cooperative scheduling (Sections 4.12.3–4.12.5).
  - **Practical**:
    - Implement Example 4.1: Create tasks using *xTaskCreate()*.
    - Experiment with task priorities (Example 4.3).
    - Use *vTaskDelay()* and *vTaskDelayUntil()* to manage task delays (Examples 4.4–4.5).
    - Implement an idle task hook (Example 4.7).
    - Change task priorities dynamically (Example 4.8) and delete tasks (Example 4.9).

---

## Step 5: Understand Queue Management
- **Objective**: Learn to use queues for inter-task communication.
- **Tasks**:
  - Read Section 5.1: *Introduction* and Section 5.2: *Characteristics of a Queue*.
    - Understand queue data storage, multi-task access, and blocking behaviors (Sections 5.2.1–5.2.5).
    - Learn about static and dynamic queue creation (Section 5.2.6).
  - Study Section 5.3: *Using a Queue*.
    - Explore APIs like *xQueueCreate()*, *xQueueSendToBack()*, *xQueueSendToFront()*, *xQueueReceive()*, and *uxQueueMessagesWaiting()* (Sections 5.3.1–5.3.4).
  - Review Section 5.4: *Receiving Data From Multiple Sources*.
    - Understand how to handle multiple data sources.
  - Study Section 5.5: *Working with Large or Variable Sized Data*.
    - Learn about queuing pointers and sending different data types (Sections 5.5.1–5.5.2).
  - Explore Section 5.6: *Receiving From Multiple Queues*.
    - Study queue sets and APIs like *xQueueCreateSet()*, *xQueueAddToSet()*, and *xQueueSelectFromSet()* (Sections 5.6.1–5.6.4).
  - Review Section 5.7: *Using a Queue to Create a Mailbox*.
    - Learn about *xQueueOverwrite()* and *xQueuePeek()* APIs (Sections 5.7.1–5.7.2).
  - **Practical**:
    - Implement Example 5.1: Block on queue receive.
    - Experiment with sending structures on a queue (Example 5.2).
    - Use a queue set to handle multiple queues (Example 5.3).

---

## Step 6: Master Software Timer Management
- **Objective**: Learn to create and manage software timers.
- **Tasks**:
  - Read Section 6.1: *Chapter Introduction and Scope*.
    - Understand the scope of software timers.
  - Study Section 6.2: *Software Timer Callback Functions*.
    - Learn about timer callback functions.
  - Explore Section 6.3: *Attributes and States of a Software Timer*.
    - Understand timer periods, one-shot, and auto-reload timers (Sections 6.3.1–6.3.2).
    - Learn about timer states (Section 6.3.3).
  - Review Section 6.4: *The Context of a Software Timer*.
    - Understand the RTOS daemon task and timer command queue (Sections 6.4.1–6.4.2).
  - Study Section 6.5: *Creating and Starting a Software Timer*.
    - Explore *xTimerCreate()* and *xTimerStart()* APIs (Sections 6.5.1–6.5.2).
  - Review Section 6.6: *The Timer ID*.
    - Learn about *vTimerSetTimerID()* and *pvTimerGetTimerID()* APIs (Sections 6.6.1–6.6.2).
  - Study Section 6.7: *Changing the Period of a Timer*.
    - Understand *xTimerChangePeriod()* API (Section 6.7.1).
  - Explore Section 6.8: *Resetting a Software Timer*.
    - Learn about *xTimerReset()* API (Section 6.8.1).
  - **Practical**:
    - Implement Example 6.1: Create one-shot and auto-reload timers.
    - Use timer IDs in callbacks (Example 6.2).
    - Reset a software timer (Example 6.3).

---

## Step 7: Learn Interrupt Management
- **Objective**: Understand how to handle interrupts in FreeRTOS.
- **Tasks**:
  - Read Section 7.1: *Introduction*.
    - Understand events and the scope of interrupt management.
  - Study Section 7.2: *Using the FreeRTOS API from an ISR*.
    - Learn about interrupt-safe APIs and the *xHigherPriorityTaskWoken* parameter (Sections 7.2.1–7.2.4).
    - Understand *portYIELD_FROM_ISR()* and *portEND_SWITCHING_ISR()* macros (Section 7.2.5).
  - Explore Section 7.3: *Deferred Interrupt Processing*.
    - Learn about deferring interrupt processing to tasks.
  - Review Section 7.4: *Binary Semaphores Used for Synchronization*.
    - Study *xSemaphoreCreateBinary()*, *xSemaphoreTake()*, and *xSemaphoreGiveFromISR()* APIs (Sections 7.4.1–7.4.3).
  - Study Section 7.5: *Counting Semaphores*.
    - Learn about *xSemaphoreCreateCounting()* API (Section 7.5.1).
  - Explore Section 7.6: *Deferring Work to the RTOS Daemon Task*.
    - Understand *xTimerPendFunctionCallFromISR()* API (Section 7.6.1).
  - Review Section 7.7: *Using Queues within an Interrupt Service Routine*.
    - Study *xQueueSendToFrontFromISR()* and *xQueueSendToBackFromISR()* APIs (Section 7.7.1).
  - Study Section 7.8: *Interrupt Nesting*.
    - Understand interrupt nesting and considerations for ARM Cortex-M (Section 7.8.1).
  - **Practical**:
    - Implement Example 7.1: Synchronize a task with an interrupt using a binary semaphore.
    - Use a counting semaphore for interrupt synchronization (Example 7.2).
    - Defer interrupt processing to the daemon task (Example 7.3).
    - Send and receive on a queue from an ISR (Example 7.4).

---

## Step 8: Understand Resource Management
- **Objective**: Learn to manage shared resources in FreeRTOS.
- **Tasks**:
  - Read Section 8.1: *Chapter Introduction and Scope*.
    - Understand mutual exclusion concepts.
  - Study Section 8.2: *Critical Sections and Suspending the Scheduler*.
    - Learn about basic critical sections and scheduler suspension (Sections 8.2.1–8.2.2).
    - Explore *vTaskSuspendAll()* and *xTaskResumeAll()* APIs (Sections 8.2.3–8.2.4).
  - Review Section 8.3: *Mutexes (and Binary Semaphores)*.
    - Study *xSemaphoreCreateMutex()*, priority inversion, priority inheritance, and recursive mutexes (Sections 8.3.1–8.3.5).
  - Explore Section 8.4: *Gatekeeper Tasks*.
    - Learn about gatekeeper tasks and their implementation (Section 8.4.1).
  - **Practical**:
    - Implement Example 8.1: Use a semaphore for mutual exclusion.
    - Rewrite a print task using a gatekeeper task (Example 8.2).

---

## Step 9: Master Event Groups
- **Objective**: Learn to use event groups for task synchronization.
- **Tasks**:
  - Read Section 9.1: *Chapter Introduction and Scope*.
    - Understand the scope of event groups.
  - Study Section 9.2: *Characteristics of an Event Group*.
    - Learn about event flags, bits, and multi-task access (Sections 9.2.1–9.2.3).
    - Study practical examples (Section 9.2.4).
  - Explore Section 9.3: *Event Management Using Event Groups*.
    - Study APIs like *xEventGroupCreate()*, *xEventGroupSetBits()*, *xEventGroupSetBitsFromISR()*, *xEventGroupWaitBits()*, and *xEventGroupGetStaticBuffer()* (Sections 9.3.1–9.3.5).
  - Review Section 9.4: *Task Synchronization Using an Event Group*.
    - Learn about *xEventGroupSync()* API (Section 9.4.1).
  - **Practical**:
    - Implement Example 9.1: Experiment with event groups.
    - Synchronize tasks using an event group (Example 9.2).

---

## Step 10: Learn Task Notifications
- **Objective**: Understand task notifications for direct task communication.
- **Tasks**:
  - Read Section 10.1: *Introduction*.
    - Understand direct-to-task communication vs. intermediary objects (Sections 10.1.1–10.1.2).
  - Study Section 10.2: *Task Notifications; Benefits and Limitations*.
    - Learn about performance and RAM benefits, and limitations (Sections 10.2.1–10.2.3).
  - Explore Section 10.3: *Using Task Notifications*.
    - Study APIs like *xTaskNotifyGive()*, *vTaskNotifyGiveFromISR()*, *ulTaskNotifyTake()*, *xTaskNotify()*, and *xTaskNotifyWait()* (Sections 10.3.2–10.3.6).
    - Learn about task notifications in peripheral drivers (Sections 10.3.7–10.3.8).
  - **Practical**:
    - Implement Example 10.1: Use task notifications instead of a semaphore (Method 1).
    - Implement Example 10.2: Use task notifications instead of a semaphore (Method 2).

---

## Step 11: Explore Low Power Support
- **Objective**: Learn to implement low-power features in FreeRTOS.
- **Tasks**:
  - Read Section 11.1: *Power Saving Introduction*.
    - Understand power-saving concepts.
  - Study Section 11.2: *FreeRTOS Sleep Modes*.
    - Learn about sleep modes in FreeRTOS.
  - Explore Section 11.3: *Functions and Enabling Built-in Tickless Idle Functionality*.
    - Study *portSUPPRESS_TICKS_AND_SLEEP()*, *vPortSuppressTicksAndSleep()*, *eTaskConfirmSleepModeStatus()*, and related macros (Sections 11.3.1–11.3.5).
  - Review Section 11.4: *Implementing portSUPPRESS_TICKS_AND_SLEEP() Macro*.
    - Understand how to implement tickless idle functionality.
  - Study Section 11.5: *Idle Task Hook Function*.
    - Learn about using idle task hooks for power management.
  - **Practical**:
    - Implement tickless idle mode in a project and measure power consumption.

---

## Step 12: Master Developer Support Tools
- **Objective**: Learn to use debugging and profiling tools in FreeRTOS.
- **Tasks**:
  - Read Section 12.1: *Introduction*.
    - Understand developer support features.
  - Study Section 12.2: *configASSERT()*.
    - Learn how to define and use *configASSERT()* for debugging (Section 12.2.1).
  - Explore Section 12.3: *Tracealyzer for FreeRTOS*.
    - Understand how Tracealyzer provides visualization and analysis.
  - Review Section 12.4: *Debug Related Hook (Callback) Functions*.
    - Study malloc failed hook (Section 12.4.1) and stack overflow hook (Section 12.4.2).
  - Study Section 12.5: *Viewing Run-time and Task State Information*.
    - Learn about task run-time statistics and the run-time statistics clock (Sections 12.5.1–12.5.2).
    - Explore configuration for collecting statistics (Section 12.5.3).
    - Study *uxTaskGetSystemState()*, *vTaskListTasks()*, and *vTaskGetRunTimeStatistics()* APIs (Sections 12.5.4–12.5.6).
  - Review Section 12.6: *Trace Hook Macros*.
    - Learn about available trace hook macros and how to define them (Sections 12.6.1–12.6.2).
    - Understand FreeRTOS-aware debugger plug-ins (Section 12.6.3).
  - **Practical**:
    - Implement run-time statistics collection (Section 12.5.7).
    - Define trace hook macros to log task switches and queue operations.
    - Use *configASSERT()* to trap errors in a project.

---

## Step 13: Troubleshoot FreeRTOS Applications
- **Objective**: Learn to identify and resolve common FreeRTOS issues.
- **Tasks**:
  - Read Section 13.1: *Chapter Introduction and Scope*.
    - Understand common issues faced by FreeRTOS users.
  - Study Section 13.2: *Interrupt Priorities*.
    - Learn about correct interrupt priority assignment and *configMAX_SYSCALL_INTERRUPT_PRIORITY*.
  - Explore Section 13.3: *Stack Overflow*.
    - Study *uxTaskGetStackHighWaterMark()* and stack checking methods (Sections 13.3.1–13.3.4).
  - Review Section 13.4: *Use of printf() and sprintf()*.
    - Understand issues with *printf()* and the role of *printf-stdarg.c* (Section 13.4.1).
  - Study Section 13.5: *Other Common Sources of Error*.
    - Learn about common errors like heap exhaustion, incorrect API usage in ISRs, and scheduler issues (Sections 13.5.1–13.5.7).
  - Review Section 13.6: *Additional Debugging Steps*.
    - Understand additional debugging techniques (e.g., checking API return values).
  - **Practical**:
    - Enable stack overflow checking and monitor stack usage in a project.
    - Debug an interrupt priority issue using *configASSERT()*.
    - Replace *printf()* with *printf-stdarg.c* in a demo project.

---

## Step 14: Apply FreeRTOS in Real-World Projects
- **Objective**: Build and optimize real-world FreeRTOS applications.
- **Tasks**:
  - Combine knowledge from previous steps to create a project with multiple tasks, queues, semaphores, and timers.
  - Implement interrupt-driven peripherals (e.g., UART or ADC) using task notifications or semaphores.
  - Optimize power consumption using tickless idle mode.
  - Use Tracealyzer to profile and debug the application.
  - Test the application for stack overflows and memory leaks.
  - **Practical**:
    - Develop a project that integrates a sensor (e.g., temperature sensor) with periodic data processing and UART output.
    - Create a multi-task application with synchronized tasks using event groups.
    - Deploy the project on a microcontroller and validate real-time performance.

---

This roadmap provides a structured path to mastering FreeRTOS, from foundational concepts to advanced debugging and real-world applications. Each step builds on the previous one, ensuring a comprehensive understanding of the FreeRTOS kernel and its practical use in embedded systems.