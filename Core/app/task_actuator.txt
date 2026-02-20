### **task_actuator.c** / **task_actuator.h** / **task_actuator_attribute.h**
- **Purpose**:  
  - Implements **non-blocking task updates** for actuators.  
  - Tasks are executed based on **time-triggered updates** (periodic or event-driven).  
  - Models actuator behavior and state updates with precise timing control.  

- **Features**:  
  - Handles initialization and configuration of actuator modules.  
  - Updates actuator states periodically using SysTick or Timer-based interrupts.  
  - Supports interfacing with physical or simulated actuators.  
  - Provides attributes and parameters to define actuator behavior, thresholds, and limits.

- **Main Functions**:  
  - `task_actuator_init()` - Initializes the actuator module.  
  - `task_actuator_update()` - Periodically updates actuator states.  