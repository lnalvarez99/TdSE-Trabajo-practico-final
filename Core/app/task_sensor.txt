### **sensor.c**
- **Purpose**:  
  - Implements **sensor task management** for button and DIP switch state monitoring in the Bare Metal Event-Triggered System (ETS).  
  - Handles **sensor state transitions** and **event processing** based on GPIO input signals.  
  - Interacts with the **task system** to trigger events and manage task execution.

- **Features**:  
  - **Monitors button and DIP switch states** to detect changes (pressed or not pressed).  
  - **Handles state transitions** for each sensor input, such as up, down, falling, and increasing states.  
  - **Triggers system events** (e.g., signal up/down) based on sensor state changes.  
  - Integrates **interrupt handling** for time-based updates using cycle counters.

- **Main Functions**:  
  - task_sensor_init() - **Initializes sensor task**, configures GPIO settings, and logs task details.  
  - task_sensor_update() - **Handles sensor state transitions**, updates task counters, and triggers system events based on sensor input. 