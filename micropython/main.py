from machine import Pin
from utime import sleep
from state_machine import *

# Hardware initialization
# Sensors
EastSensorS = Pin(10, Pin.IN, Pin.PULL_UP)
NorthSensorL = Pin(11, Pin.IN, Pin.PULL_UP)
NorthSensorR = Pin(12, Pin.IN, Pin.PULL_UP)
WestSensorS = Pin(27, Pin.IN, Pin.PULL_UP)
WestSensorL = Pin(26, Pin.IN, Pin.PULL_UP)
# Clock
Clock = Pin(15, Pin.IN, Pin.PULL_UP)
prevClock = Clock.value() # Initialize the previous clock value

'''
Transition methods defined here
'''
# Starter example transition functions
# Note: Define more functions here
def S0_transition(inputs):
  '''
  Initial state - sensor-based transitions
  Priority 1: NL detected → S4
  Priority 2: NR or WL detected → S1
  Otherwise: stay in S0
  '''
  if inputs.NorthL:
    return State.S4
  elif inputs.NorthR or inputs.WestL:
    return State.S1
  else:
    return None

def S1_transition(inputs):
  '''
  Always transition to S2
  '''
  return State.S2

def S2_transition(inputs):
  '''
  Always transition to S3
  '''
  return State.S3

def S3_transition(inputs):
  '''
  Always transition to S0
  '''
  return State.S0

def S4_transition(inputs):
  '''
  Always transition to S5
  '''
  return State.S5

def S5_transition(inputs):
  '''
  Sensor-based transitions
  Priority 1: WL detected → S7
  Priority 2: WS or ES detected → S6
  Otherwise: stay in S5
  '''
  if inputs.WestL:
    return State.S7
  elif inputs.WestS or inputs.EastS:
    return State.S6
  else:
    return None

def S6_transition(inputs):
  '''
  Always transition to S0
  '''
  return State.S0

def S7_transition(inputs):
  '''
  Always transition to S2
  '''
  return State.S2

'''
Fill in the transition table here
'''
# Note: Define more entries here as needed
transitionTable = { State.S0: S0_transition,
                    State.S1: S1_transition,
                    State.S2: S2_transition,
                    State.S3: S3_transition,
                    State.S4: S4_transition,
                    State.S5: S5_transition,
                    State.S6: S6_transition,
                    State.S7: S7_transition }

'''
Tuple output format:      ( ER,EY,EG,NR,NY,NG,NA,WR,WY,WG,WA )
All red example           (  1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0 )
'''
# Note: Add more output entries here as needed
#                         ( ER,EY,EG,NR,NY,NG,NA,WR,WY,WG,WA )
outputTable = { State.S0: (  0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0 ),  # East: Green, North: Red, West: Green
                State.S1: (  0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0 ),  # East: Yellow, North: Red, West: Green
                State.S2: (  1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1 ),  # East: Red, North: Red+Arrow, West: Green+Arrow
                State.S3: (  1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0 ),  # East: Red, North: Red, West: Green
                State.S4: (  0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0 ),  # East: Yellow, North: Red, West: Yellow
                State.S5: (  1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0 ),  # East: Red, North: Green+Arrow, West: Red
                State.S6: (  1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0 ),  # East: Red, North: Yellow, West: Red
                State.S7: (  1, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0 ) } # East: Red, North: Yellow+Arrow, West: Red

'''
Main application code starts here
'''
trafficFSM = StateMachine( initialState=State.S0, 
                          transitionTable=transitionTable, 
                          outputTable=outputTable)

print("Traffic Light Controller, Press Clock button to process state")

'''
Main infinite loop logic
'''
while True:
  '''
  Read all inputs at top of each iteration
  Note: For the slide switches, they are normally-closed (read as 1 when "OFF"). We
  flip this on reading so it matches up with the state diagram logic
  '''
  newClock = not Clock.value()
  newInputs = StateInputs(eastStraight=not EastSensorS.value(), 
                          northLeft=not NorthSensorL.value(),
                          northRight=not NorthSensorR.value(),
                          westStraight=not WestSensorS.value(),
                          westLeft=not WestSensorL.value() )

  '''
  Falling clock edge detection
  '''
  fallingEdgeDetected = prevClock is False and newClock != prevClock
  prevClock = newClock
  
  '''
  Main processing will only tick for each clock rising edge
  '''
  if fallingEdgeDetected:
    
    # Optional: Print out the new inputs read at this clock edge
    print(newInputs)

    # Process the next state based on these new inputs
    nextState = trafficFSM.nextState(newInputs)

    # If nextState was none, we stay in current state
    if nextState is not None:
      trafficFSM.state = nextState

    # Print the new state for this iteration, and update the 
    # outputs
    print(trafficFSM)
    trafficFSM.updateOutputs()
  
  sleep(0.1)