// 
// 
// 

#include "Arduino.h"
#include "TypeDefStruct.h"
#include "Task.h"

#define QueueTaskSize 8

unsigned int _queueStart;
unsigned int _queueEnd;
unsigned int _itemsInQueue;
taskItem _schedule[QueueTaskSize];
static instanceTaskFunction instanceFunc;
static endTaskFunction endFunc;
unsigned long currentMillis = 0;

int _queueGetTop(taskItem &item)
{
	int rv = 0;
    //Remove the top item, stuff it into item
    if (_queueEnd != _queueStart) {
            taskItem tempItem = _schedule[_queueStart];
            //This Algorithm also from Wikipedia.
            _queueStart = (_queueStart + 1) % QueueTaskSize;
            item = tempItem;
            _itemsInQueue--;
    } else {
    //if the buffer is empty, return an error code
        rv = -1;
    }
    return rv;  
}
int _addToQueue(taskItem item)
{
	//This is just a circular buffer, and this algorithm is stolen from wikipedia
	int rv = 0;
    if ((_queueEnd + 1) % QueueTaskSize != _queueStart) {
        _schedule[_queueEnd] = item;
        _queueEnd = (_queueEnd + 1) % QueueTaskSize;
        _itemsInQueue++;
    } else {
        //if buffer is full, error
        rv = -1;
    }
    return rv;
}
void addTask(taskFunction func, const char * id, uint32_t initialRun, uint32_t recur)
{
	if (strlen(id) > 7)
		return;
	
    taskItem newItem;
    newItem.fPtr = func;
    memset(newItem.itemName, 0, 8);
    memcpy(newItem.itemName, id, strlen(id));
    newItem.recur = recur;
    newItem.next = initialRun;
	_addToQueue(newItem);
}
void setInstanceTask(void (*userFunc)(void)) {
	instanceFunc = userFunc;
}void setEndTask(void (*userFunc)(void)) {
	endFunc = userFunc;
}
void loop() {
	currentMillis = millis();
	taskItem target;
	if (instanceFunc)
		(instanceFunc)();

    for (int i = 0; i < _itemsInQueue; ++i)
    {
        if(_queueGetTop(target)==0)
        {
			currentMillis = millis();
            if(target.next <= currentMillis)
            {
				(target.fPtr)(currentMillis);
                if(target.recur != 0)
                {
                    target.next = currentMillis + target.recur;
                    _addToQueue(target);
                }
            } else {
                _addToQueue(target);
            }
        } 
		else {
            break;
        }
    }
	currentMillis = millis();
	if (endFunc)
		(endFunc)();

}