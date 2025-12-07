#include <stdbool.h>
#include <stdio.h>
#include "car_queue.h"

void initQueue(carQueue_S *q)
{
    if (q == NULL)
        return;
    
    q->size = 0;
    q->front = 0;
    q->rear = -1;
    for (int i = 0; i < DIR_COUNT; i++)
    {
        q->directionCount[i] = 0;
    }
}

bool isQueueFull(carQueue_S *q)
{
    if (q == NULL)
        return true;
    
    return (q->size >= CAR_QUEUE_CAPACITY);
}

bool isQueueEmpty(carQueue_S *q)
{
    if (q == NULL)
        return true;
    
    return (q->size == 0);
}

bool enqueue(carQueue_S *q, car_S car)
{
    if (q == NULL || car.direction >= DIR_COUNT)
        return false;
    
    if (isQueueFull(q))
        return false;
    
    q->rear = (q->rear + 1) % CAR_QUEUE_CAPACITY;
    q->cars[q->rear] = car;
    q->size++;
    q->directionCount[car.direction]++;
    return true;
}

bool dequeue(carQueue_S *q, car_S *carOut)
{
    if (q == NULL || carOut == NULL)
        return false;
    
    if (isQueueEmpty(q))
        return false;
    
    *carOut = q->cars[q->front];
    
    if (carOut->direction < DIR_COUNT)
    {
        q->directionCount[carOut->direction]--;
    }
    
    q->front = (q->front + 1) % CAR_QUEUE_CAPACITY;
    q->size--;
    
    return true;
}

bool isQueueDirFull(carQueue_S *q, direction_E dir)
{
    if (q == NULL || dir >= DIR_COUNT)
        return true;
    return (q->directionCount[dir] >= 2);
}

void printQueue(const carQueue_S *q)
{
    printf("Queue: ");

    if (q->size == 0)
    {
        printf("0\n");
        return;
    }

    int count = 0;
    int index = q->front;
    while (count < q->size)
    {
        direction_E dir = q->cars[index].direction;
        printf("%s ", DIRECTION_NAMES[dir]);

        index = (index + 1) % CAR_QUEUE_CAPACITY;
        count++;
    }
    printf("\n");
}