# Traffic Light Controller - Finite State Machine Project

An embedded systems project implementing an intelligent traffic light controller using Finite State Machines (FSM). The project demonstrates FSM design, queue-based data structures, and translating high-level Python code to efficient C/C++ for embedded systems.

## Getting Started

This project contains two implementations of the same traffic light controller:
1. **MicroPython** - Sensor-based FSM with button-triggered state transitions
2. **C/C++** - Queue-based FSM with automatic tick processing

Both versions run on the Raspberry Pi Pico and can be simulated in Wokwi.

### Dependencies

**For Simulation:**
```
- Wokwi Simulator (https://wokwi.com)
- Web browser
```

**For Hardware Deployment:**
```
- Raspberry Pi Pico
- MicroPython firmware (for Python version)
- Pico SDK (for C version)
- LEDs (11 total: 3 for East, 4 for North, 4 for West)
- Push buttons (1-5 depending on version)
- Breadboard and jumper wires
```

### Installation

**Step 1: Clone or Download the Project**
```bash
git clone <repository-url> (to get the starter code)
Then created a new repo and uploaded my changes there
```

**Step 2: Choose Your Implementation**

For **MicroPython Version**:
```bash
1) analyze the requirements
2) prepare the environments on both wokwi and local
3) Understand the statemachine
4) Implement the code according to the requirements
5) Test the code  
```

For **C/C++ Version**:
```bash
- Same as Micropython
```

**Step 3: Run in Wokwi Simulator**
- Navigate to https://wokwi.com
- Click "Start from Scratch" or open existing project
- Upload the `diagram.json` file from your chosen implementation
- Click the green "Play" button to start simulation

## Testing

### MicroPython Version Tests

**Test 1: Empty Queue / No Sensors Active**
- Expected: System stays in State S0 (East Green, West Green, North Red)
- How to test: Run simulation without pressing any sensor buttons
```
Result: FSM remains in S0, no state transitions occur
```

**Test 2: North Left Turn Request**
- Expected: Transition from S0 → S4 → S5
- How to test: Activate North Left sensor (GPIO 11), press Clock button
```
Clock Press 1: S0 → S4 (Yellow transition)
Clock Press 2: S4 → S5 (North Green + Arrow)
```

**Test 3: Priority-Based Transitions**
- Expected: North Left has priority over North Right
- How to test: Activate both NL and NR sensors
```
Result: System chooses S4 path (North Left) over S1 path
```

### C/C++ Version Tests

**Test 1: Add Cars to Queue**
- Expected: Button press adds car with random direction (ES, NL, NR, WS, WL)
- How to test: Press New Car button (GPIO 15)
```
Console Output: "New car added: NL (Total: 1 cars)"
```

**Test 2: Automatic Tick Processing**
- Expected: System processes one state per second automatically
- How to test: Add cars and observe console output
```
Tick 1: "State: S0, Queue: NL"
Tick 2: "State: S4, Queue: NL"
Tick 3: "State: S5, Queue: NL"
Tick 4: "Car passed: NL"
```

**Test 3: Queue Full Handling**
- Expected: System rejects cars when queue is full (10 cars) or direction limit reached (2 per direction)
- How to test: Press button rapidly to add 11+ cars
```
Console Output: "Cannot add car: Queue is full"
```

**Test 4: Car Removal When Light is Green**
- Expected: Front car passes only when light permits its direction
- How to test: Add ES car, observe it passes in State S0
```
State S0: East Green → "Car passed: ES"
```

## Project Instructions

This project fulfills the following learning objectives:

### Part 1: Python FSM Implementation
- Define a Finite State Machine with 8 states (S0-S7)  
- Implement states as class attributes (`State.S0`, `State.S1`, etc.)  
- Create transition functions based on sensor inputs  
- Build a main loop with clock-based state processing  
- Update LED outputs based on current state  
- Demonstrate working traffic light simulation in Wokwi  

### Part 2: Translate to C/C++
- Convert Python FSM to C using enums (`STATE_S0`, `STATE_S1`, etc.)  
- Replace button-based processing with automatic tick counters  
- Maintain functional equivalence with Python version  
- Preserve all state transitions and output configurations  

### Part 3: Queue Data Structure
- Implement circular queue for car management  
- Store car direction (ES, NL, NR, WS, WL) for each entry  
- Add cars via button press with random direction assignment  
- Process one car per tick based on FSM state  
- Remove car from queue when light permits passage  
- Make FSM transitions dependent on queue contents  
- Handle empty queue (no unnecessary transitions)  

### File Descriptions

**`starter/micropython/main.py`**
- Main application loop
- Transition function definitions (S0_transition through S7_transition)
- Transition table and output table
- Clock edge detection and input reading
- Hardware pin definitions

**`starter/micropython/state_machine.py`**
- StateMachine class framework
- State class definition
- StateInputs data class
- LED update logic

**`starter/c/main.c`**
- Complete C implementation
- State machine with enum-based states
- Queue integration with FSM logic
- Tick-based processing (1 second intervals)
- Car passing logic based on traffic light state
- Button debouncing for new car input

**`starter/c/car_queue.h`**
- Queue data structure definitions
- Direction enum (DIR_EAST_S, DIR_NORTH_L, etc.)
- Function declarations for queue operations

**`starter/c/car_queue.c`**
- Queue implementation (circular buffer)
- `enqueue()`: Add car with direction tracking
- `dequeue()`: Remove front car and update counts
- `isQueueDirFull()`: Check per-direction limits (max 2)
- `printQueue()`: Console output for debugging

## Built With

* [MicroPython](https://micropython.org/) - Python implementation for microcontrollers
* [Raspberry Pi Pico SDK](https://github.com/raspberrypi/pico-sdk) - C/C++ development framework
* [Wokwi](https://wokwi.com) - Online embedded systems simulator
* C Programming language built with GCC compiler 

## Key Design Decisions

**Why 8 States?**
- S0, S3: Main green states for different directions
- S1, S4, S6: Yellow transition states (safety delays)
- S2, S5, S7: Protected left turn states (conflict resolution)

**Why Circular Queue?**
- Fixed memory allocation (embedded systems requirement)
- O(1) enqueue/dequeue operations
- Direction counting for smart FSM decisions

**Why 2-Tick Green Wait? (C Version Bonus)**
- Allows multiple cars to pass per green cycle
- Improves traffic throughput
- More realistic traffic light behavior

## Architecture Highlights

**Clean Separation of Concerns:**
- FSM logic separated from hardware I/O
- Queue management isolated in separate module
- Transition logic decoupled from output control

**Defensive Programming:**
- Null pointer checks in all queue operations
- Boundary validation (queue full, direction limits)
- Invalid state handling

**Memory Safety:**
- Static allocation (no malloc/free)
- Bounded arrays with capacity checking
- Circular buffer prevents overflow

## License

[License](LICENSE.txt)

---

**Project developed for Udacity's Embedded Systems Nanodegree - Term 1**  
**Topics:** Finite State Machines, Data Structures, Python to C Translation, Embedded Systems Design
