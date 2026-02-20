# Trabajo Final Taller de Sistemas Embebidos

## Description:
Bare Metal - Event-Triggered Systems (ETS)  
App - Retarget `printf` to Console  
Project for STM32 Project (STM32CubeIDE Version: 1.7.0)  

### System Configuration:
- **SystemCoreClock**: 64 MHz (15.625 nS per clock cycle)  
- **SysTick Rate Hertz**: 1000 ticks per second (1 mS per tick)  

---

## File Overview:
### **app.c** / **app.h**
- Implements endless loops that execute tasks with fixed computing time.  
- Sequential execution is only interrupted by an event-driven interrupt.  

### **task_system.c** / **task_system.h** / **task_system_attribute.h**
- **Purpose**: Non-blocking code for system modeling.  

### **task_system_interface.c** / **task_system_interface.h**
- **Purpose**: Non-blocking code for interfacing the system.

### **task_actuator.c** / **task_actuator.h** / **task_actuator_attribute.h**
- **Purpose**: Actuator modeling with non-blocking and time-based updates.  

### **task_actuator_interface.c** / **task_actuator_interface.h**
- **Purpose**: Non-blocking interface for actuator control.  

### **task_sensor.c** / **task_sensor.h** / **task_sensor_attribute.h**
- **Purpose**: Sensor modeling with non-blocking and time-based updates.  

### **logger.c**
- **Purpose**: Utilities for retargeting `printf` to the console output.  

### **dwt.h**
- **Purpose**: Provides utilities for measuring clock cycles and execution time of the code.  

### **display.c** / **display.h**
- **Purpose**: Handles display output management.
- Provides functions for initializing and updating display values.
- Supports formatted output for system states, sensor values, or actuator feedback.

### **task_temperature.c** / **task_temperature.h**
- **Purpose**: Models temperature-related tasks.
- Includes non-blocking time updates to periodically read or simulate temperature values.

### **logger.c**
- **Purpose**: Provides logging utilities for debugging and real-time monitoring.
- Retargets the standard output (e.g., `printf`) to a serial console.

---

## Build and Execution Instructions:

1. **Required Tools**:
   - STM32CubeIDE Version 1.7.0 or higher.
   - STM32CubeMX for code generation (optional but recommended).
   - STM32F103RB microcontroller (or compatible STM32 board).

2. **Project Setup**:
   - Open the project in STM32CubeIDE.
   - Ensure the project is configured for the STM32F103RB microcontroller.
   - Verify the SysTick timer is set to generate 1ms interrupts.

3. **Compilation**:
   - Clean and build the project by selecting **Project > Build Project**.
   - Ensure no errors are present in the console output.

4. **Flashing the Microcontroller**:
   - Connect the STM32 board via an ST-Link debugger.
   - Select **Run > Debug** or **Run > Run As > STM32 Cortex-M C/C++ Application** to flash the binary.

---

## References:
- **STM32 Step-by-Step Guide**:  
  [Getting Started with STM32](https://wiki.st.com/stm32mcu/wiki/STM32StepByStep:Getting_started_with_STM32_:_STM32_step_by_step)  

- **STM32CubeIDE Documentation**:  
  [STM32CubeIDE User Manual](https://www.st.com/en/development-tools/stm32cubeide.html)  

---

## Build procedures:
Visit the Getting started with STM32: STM32 step-by-step at 
"https://wiki.st.com/stm32mcu/wiki/STM32StepByStep:Getting_started_with_STM32_:_STM32_step_by_step"
to get started building STM32 Projects.
