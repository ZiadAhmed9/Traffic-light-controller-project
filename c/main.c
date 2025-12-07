#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include "pico/stdlib.h"
#include "car_queue.h"

//-------------------------------------------------------------------
// Defines and macros
//-------------------------------------------------------------------
// Outputs
#define EAST_R_PIN (3U)
#define EAST_Y_PIN (4U)
#define EAST_G_PIN (5U)
#define NORTH_R_PIN ( 6U)
#define NORTH_Y_PIN ( 7U)
#define NORTH_G_PIN ( 8U)
#define NORTH_A_PIN ( 9U)
#define WEST_R_PIN  (18U)
#define WEST_Y_PIN  (17U)
#define WEST_G_PIN  (16U)
#define WEST_A_PIN  (21U)
#define NEW_CAR_PIN (15U)

//-------------------------------------------------------------------
// Enumerations and structs
//-------------------------------------------------------------------
typedef enum
{
  // Outputs
  GPIO_OUT_EAST_R,
  GPIO_OUT_EAST_Y,
  GPIO_OUT_EAST_G,
  GPIO_OUT_NORTH_R,
  GPIO_OUT_NORTH_Y,
  GPIO_OUT_NORTH_G,
  GPIO_OUT_NORTH_A,
  GPIO_OUT_WEST_R,
  GPIO_OUT_WEST_Y,
  GPIO_OUT_WEST_G,
  GPIO_OUT_WEST_A,
  GPIO_IN_NEW_CAR,
  GPIO_COUNT
} gpioName_E;
typedef struct {
  uint pin;
  uint direction;
} gpioInit_S;

typedef enum
{
  STATE_S0,
  STATE_S1,
  STATE_S2,
  STATE_S3,
  STATE_S4,
  STATE_S5,
  STATE_S6,
  STATE_S7,
  STATE_COUNT
} stateName_E;
typedef struct {
  bool ER; // East Red
  bool EY; // East Yellow
  bool EG; // East Green
  bool NR; // North Red
  bool NY; // North Yellow
  bool NG; // North Green
  bool NA; // North Arrow
  bool WR; // West Red
  bool WY; // West Yellow
  bool WG; // West Green
  bool WA; // West Arrow
} stateOutputs_S;

//-------------------------------------------------------------------
// Global tables and data types
//-------------------------------------------------------------------

// OutputTable
// Each row contains a specific state for { ER,EY,EG,NR,NY,NG,NA,WR,WY,WG,WA }
// For example:
// All reds -> { true, false, false, true, false, false, false, true, false, false, false }
static const stateOutputs_S gOutputTable[STATE_COUNT] = {
    //     ER,    EY,    EG,    NR,    NY,    NG,    NA,    WR,    WY,    WG,    WA
    {false, false,  true,  true, false, false, false, false, false,  true, false}, // S0: E:Green, N:Red, W:Green
    {false,  true, false,  true, false, false, false, false, false,  true, false}, // S1: E:Yellow, N:Red, W:Green
    { true, false, false,  true, false, false,  true, false, false,  true,  true}, // S2: E:Red, N:Red+Arrow, W:Green+Arrow
    { true, false, false,  true, false, false, false, false, false,  true, false}, // S3: E:Red, N:Red, W:Green
    {false,  true, false,  true, false, false, false, false,  true, false, false}, // S4: E:Yellow, N:Red, W:Yellow
    { true, false, false, false, false,  true,  true,  true, false, false, false}, // S5: E:Red, N:Green+Arrow, W:Red
    { true, false, false, false,  true, false, false,  true, false, false, false}, // S6: E:Red, N:Yellow, W:Red
    { true, false, false, false,  true, false,  true,  true, false, false, false}, // S7: E:Red, N:Yellow+Arrow, W:Red
};

static const gpioInit_S gpioList[GPIO_COUNT] = {
    {.pin = EAST_R_PIN, .direction = GPIO_OUT},
    {.pin = EAST_Y_PIN, .direction = GPIO_OUT},
    {.pin = EAST_G_PIN, .direction = GPIO_OUT},
    {.pin = NORTH_R_PIN, .direction = GPIO_OUT},
    {.pin = NORTH_Y_PIN, .direction = GPIO_OUT},
    {.pin = NORTH_G_PIN, .direction = GPIO_OUT},
    {.pin = NORTH_A_PIN, .direction = GPIO_OUT},
    {.pin = WEST_R_PIN, .direction = GPIO_OUT},
    {.pin = WEST_Y_PIN, .direction = GPIO_OUT},
    {.pin = WEST_G_PIN, .direction = GPIO_OUT},
    {.pin = WEST_A_PIN, .direction = GPIO_OUT},
    {.pin = NEW_CAR_PIN, .direction = GPIO_IN},
};

static stateName_E gState;
static uint32_t tick_count = 0;
static uint32_t traffic_tick = 0;
static uint8_t green_wait_counter = 0; // Bonus: 2-tick wait for green states

//-------------------------------------------------------------------
// Private function definitions
//-------------------------------------------------------------------

// This function initializes all used GPIOs in this project
static void gpioSetup(void)
{
  for (size_t i = 0U; i < GPIO_COUNT; i++)
  {
    gpio_init(gpioList[i].pin);
    if (GPIO_OUT == gpioList[i].direction)
    {
      gpio_set_dir(gpioList[i].pin, GPIO_OUT);
    }
    else
    {
      gpio_set_dir(gpioList[i].pin, GPIO_IN);
      gpio_pull_up(gpioList[i].pin);
    }
  }
}

//-------------------------------------------------------------------
// State machine framework functions
//-------------------------------------------------------------------
static void updateOutputs(const stateName_E currentState)
{
  if (currentState < STATE_COUNT)
  {
    stateOutputs_S *const pOutput = &gOutputTable[currentState];
    gpio_put(gpioList[GPIO_OUT_EAST_R].pin, pOutput->ER);
    gpio_put(gpioList[GPIO_OUT_EAST_Y].pin, pOutput->EY);
    gpio_put(gpioList[GPIO_OUT_EAST_G].pin, pOutput->EG);
    gpio_put(gpioList[GPIO_OUT_NORTH_R].pin, pOutput->NR);
    gpio_put(gpioList[GPIO_OUT_NORTH_Y].pin, pOutput->NY);
    gpio_put(gpioList[GPIO_OUT_NORTH_G].pin, pOutput->NG);
    gpio_put(gpioList[GPIO_OUT_NORTH_A].pin, pOutput->NA);
    gpio_put(gpioList[GPIO_OUT_WEST_R].pin, pOutput->WR);
    gpio_put(gpioList[GPIO_OUT_WEST_Y].pin, pOutput->WY);
    gpio_put(gpioList[GPIO_OUT_WEST_G].pin, pOutput->WG);
    gpio_put(gpioList[GPIO_OUT_WEST_A].pin, pOutput->WA);
  }
}

static void checkNewCarPressed(carQueue_S *carQueue)
{
  static bool prevButtonState = true;
  
  bool currentButtonState = gpio_get(gpioList[GPIO_IN_NEW_CAR].pin);
  bool buttonPressed = (prevButtonState == true && currentButtonState == false);
  
  prevButtonState = currentButtonState;

  if (buttonPressed)
  {
    direction_E randomDirection = (direction_E)(rand() % DIR_COUNT);
    
    if (isQueueDirFull(carQueue, randomDirection))
    {
      printf("Cannot add car: Direction %s is full (max 2 cars)\n", 
             DIRECTION_NAMES[randomDirection]);
      return;
    }
    
    if (isQueueFull(carQueue))
    {
      printf("Cannot add car: Queue is full\n");
      return;
    }
    
    car_S newCar;
    newCar.direction = randomDirection;
    
    if (enqueue(carQueue, newCar))
    {
      printf("New car added: %s (Total: %d cars)\n", 
             DIRECTION_NAMES[randomDirection], carQueue->size);
    }
    else
    {
      printf("Failed to add car to queue\n");
    }
  }
}

static stateName_E getNextStateFromQueue(stateName_E currentState, const carQueue_S *carQueue)
{
  if (carQueue == NULL)
    return STATE_COUNT;

  switch (currentState)
  {
  case STATE_S0:
    // Check queue for transition: NL → S4, NR/WL → S1
    if (!isQueueEmpty(carQueue))
    {
      if (carQueue->directionCount[DIR_NORTH_L] > 0)
      {
        return STATE_S4;
      }
      else if (carQueue->directionCount[DIR_NORTH_R] > 0 || 
               carQueue->directionCount[DIR_WEST_L] > 0)
      {
        return STATE_S1;
      }
    }
    return STATE_COUNT;
    break;

  case STATE_S1:
    return STATE_S2;
    break;

  case STATE_S2:
    return STATE_S3;
    break;

  case STATE_S3:
    return STATE_S0;
    break;

  case STATE_S4:
    return STATE_S5;
    break;

  case STATE_S5:
    // Check queue for transition: WL → S7, WS/ES → S6
    if (!isQueueEmpty(carQueue))
    {
      if (carQueue->directionCount[DIR_WEST_L] > 0)
      {
        return STATE_S7;
      }
      else if (carQueue->directionCount[DIR_WEST_S] > 0 || 
               carQueue->directionCount[DIR_EAST_S] > 0)
      {
        return STATE_S6;
      }
    }
    return STATE_COUNT;
    break;

  case STATE_S6:
    return STATE_S0;
    break;

  case STATE_S7:
    return STATE_S2;
    break;

  default:
    return STATE_COUNT;
  }
}

static bool tryPassCarFromQueue(stateName_E state, carQueue_S *queue)
{
  if (queue == NULL || isQueueEmpty(queue))
    return false;

  bool carCanPass = false;
  
  car_S frontCar = queue->cars[queue->front];
  direction_E carDirection = frontCar.direction;

  // Check if light is green for this car's direction
  switch (state)
  {
  case STATE_S0:
    if (carDirection == DIR_EAST_S || carDirection == DIR_WEST_S)
    {
      carCanPass = true;
    }
    break;

  case STATE_S2:
    if (carDirection == DIR_NORTH_L || carDirection == DIR_WEST_L || carDirection == DIR_WEST_S)
    {
      carCanPass = true;
    }
    break;

  case STATE_S3:
    if (carDirection == DIR_WEST_S)
    {
      carCanPass = true;
    }
    break;

  case STATE_S5:
    if (carDirection == DIR_NORTH_L || carDirection == DIR_NORTH_R)
    {
      carCanPass = true;
    }
    break;

  default:
    carCanPass = false;
    break;
  }

  if (carCanPass)
  {
    car_S passedCar;
    if (dequeue(queue, &passedCar))
    {
      printf("Car passed: %s\n", DIRECTION_NAMES[passedCar.direction]);
      return true;
    }
  }

  return false;
}

//-------------------------------------------------------------------
// Main application code starts here
//-------------------------------------------------------------------
int main()
{

  printf("Traffic Light Controller\n");

  gState = STATE_S0;

  stdio_init_all();
  gpioSetup();
  srand(time(NULL));

  carQueue_S carQueue;
  initQueue(&carQueue);
  while (true)
  {
    tick_count += 1;

    checkNewCarPressed(&carQueue);

    // Process every second (10 x 100ms ticks)
    if (tick_count - traffic_tick == 10)
    {
      traffic_tick = tick_count;

      updateOutputs(gState);

      printf("State: S%i, ", gState);
      printQueue(&carQueue);

      tryPassCarFromQueue(gState, &carQueue);

      stateName_E nextState = getNextStateFromQueue(gState, &carQueue);

      // 2-tick wait for green states so more cars can pass
      bool isGreenState = (gState == STATE_S0 || gState == STATE_S5);
      
      if (isGreenState && nextState != STATE_COUNT)
      {
        green_wait_counter++;
        
        if (green_wait_counter >= 2)
        {
          gState = nextState;
          green_wait_counter = 0;
        }
        else
        {
          printf("(Green wait: %d/2)\n", green_wait_counter);
        }
      }
      else
      {
        green_wait_counter = 0;
        
        if (nextState != STATE_COUNT)
        {
          gState = nextState;
        }
      }
    }

    sleep_ms(100);
  }
}
