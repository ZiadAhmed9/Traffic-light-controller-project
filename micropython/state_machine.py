from machine import Pin

# Pin definitions
# East bound direction
EastR = Pin(3, Pin.OUT)
EastY = Pin(4, Pin.OUT)
EastG = Pin(5, Pin.OUT)
# North bound direction 
# Traffic lights
NorthR = Pin(6, Pin.OUT)
NorthY = Pin(7, Pin.OUT)
NorthG = Pin(8, Pin.OUT)
NorthA = Pin(9, Pin.OUT)
# West bound direction
# Traffic lights
WestR = Pin(18, Pin.OUT)
WestY = Pin(17, Pin.OUT)
WestG = Pin(16, Pin.OUT)
WestA = Pin(21, Pin.OUT)

'''
State Machine framework class definition
'''
class StateMachine:
  def __init__(self, initialState, transitionTable, outputTable):
    self.state = initialState
    self.transitionTable = transitionTable
    self.outputTable = outputTable
  
  def __str__(self):
    return f"Current State: {self.state}"
  
  def nextState(self, inputs):
    if self.state in self.transitionTable:
      if self.transitionTable[self.state] is not None:
        # Call the transition function with current inputs
        return self.transitionTable[self.state](inputs)
      else:
        print("No supported transition function")
    else:
      print("Invalid current state")
  
  def updateOutputs(self):
    ''' 
    This method updates the LEDs according to the output table
    tuple provided, order is positional
    '''
    if self.state in self.outputTable:
      # Unpack the tuple. Must match tuple table order!
      (ER, EY, EG, NR, NY, NG, NA, WR, WY, WG, WA) = self.outputTable[self.state]

      # Now update all lights accordingly
      EastR.on() if ER else EastR.off()
      EastY.on() if EY else EastY.off()
      EastG.on() if EG else EastG.off()

      NorthR.on() if NR else NorthR.off()
      NorthY.on() if NY else NorthY.off()
      NorthG.on() if NG else NorthG.off()
      NorthA.on() if NA else NorthA.off()

      WestR.on() if WR else WestR.off()
      WestY.on() if WY else WestY.off()
      WestG.on() if WG else WestG.off()
      WestA.on() if WA else WestA.off()

    else:
      print("Invalid output state")


'''
Input data storage class
'''
class StateInputs:
  def __init__(self, eastStraight, northLeft, northRight, westStraight, westLeft):
    self.EastS  = eastStraight
    self.NorthL = northLeft
    self.NorthR = northRight
    self.WestS  = westStraight
    self.WestL  = westLeft

  def __str__(self):
    return f"ES: {self.EastS} NL: {self.NorthL} NR: {self.NorthR} WS: {self.WestS} WL: {self.WestL}"


'''
State class definition
'''
class State:
  def __init__(self, name): self.name = name
  def __str__(self): return self.name
  def transition(self): pass

# Note: Define more states here as needed
State.S0 = "S0"
State.S1 = "S1"
State.S2 = "S2"
State.S3 = "S3"
State.S4 = "S4"
State.S5 = "S5"
State.S6 = "S6"
State.S7 = "S7"