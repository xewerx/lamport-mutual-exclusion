#include <queue>
#include <vector>
#include <iostream>
#include <algorithm>

#include "./Robber.h"

Robber::Robber(int processes)
{
    isInterestedInCriticalSection = false;
    lamportClock = 0;
    otherClocks.resize(processes, -1); // puste miejsce wypełnij -1
}

int Robber::getLamportClock()
{
    return lamportClock;
}

void Robber::setLamportClock(int value)
{
    lamportClock = value;
}

int Robber::incrementLamportClock()
{
    return ++lamportClock;
}

void Robber::insertMessageToQue(Message message)
{
    messagesQue.push(message);
}

void Robber::removeMessageFromQue(int sender)
{
    priority_queue<Message> tempQueue;

    // Przeszukiwanie oryginalnej kolejki
    while (!messagesQue.empty())
    {
        Message mess = messagesQue.top();
        if (mess.sender != sender)
        {
            tempQueue.push(mess);
        }
        messagesQue.pop();
    }

    // Przywracanie elementów do oryginalnej kolejki
    messagesQue = move(tempQueue);
}

Message Robber::getFirstMessageFromQue()
{
    return messagesQue.top();
}

void Robber::setLastClock(int sender, int clock)
{
    otherClocks.at(sender) = clock;
}

int Robber::countResponses()
{
    int responsesAmount = 0;
    for (int el : otherClocks)
    {
        if (el != -1)
            responsesAmount++;
    }

    return responsesAmount;
}

bool Robber::isMyClockBiggest(int clock)
{
    for (int el : otherClocks)
    {
        if (el == -1)
            continue;
        if (el > clock)
            return false;
    }

    return true;
}

void Robber::printVector()
{
    cout << "VECTOR: ";
    for (int element : otherClocks)
    {
        cout << element << " ";
    }
    cout << endl;
}