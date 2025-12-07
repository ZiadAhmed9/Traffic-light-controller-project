#ifndef CAR_QUEUE_H
#define CAR_QUEUE_H

#define CAR_QUEUE_CAPACITY 10

static const char *DIRECTION_NAMES[] = {
    "ES", // DIR_EAST_S
    "NL", // DIR_NORTH_L
    "NR", // DIR_NORTH_R
    "WS", // DIR_WEST_S
    "WL"  // DIR_WEST_L
};

typedef enum
{
    DIR_EAST_S,
    DIR_NORTH_L,
    DIR_NORTH_R,
    DIR_WEST_S,
    DIR_WEST_L,
    DIR_COUNT
} direction_E;

typedef struct
{
    direction_E direction;
} car_S;

typedef struct
{
    car_S cars[CAR_QUEUE_CAPACITY];
    int front;
    int rear;
    int size;
    int directionCount[DIR_COUNT];
} carQueue_S;

void initQueue(carQueue_S *q);
bool isQueueFull(carQueue_S *q);
bool isQueueEmpty(carQueue_S *q);
bool enqueue(carQueue_S *q, car_S car);
bool dequeue(carQueue_S *q, car_S *carOut);
bool isQueueDirFull(carQueue_S *q, direction_E dir);
void printQueue(const carQueue_S *q);

#endif