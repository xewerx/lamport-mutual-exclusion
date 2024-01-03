#include <queue>
#include <vector>
#include <iostream>
#include <algorithm>

#include "./Robber.h"

Robber::Robber(int processes)
{
    isInterestedInS = false;
    isInterestedInN = false;
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

void Robber::insertMessageToQueS(Message message)
{
    messagesQueS.push(message);
}

void Robber::removeMessageFromQueS(int sender)
{
    priority_queue<Message> tempQueue;

    // Przeszukiwanie oryginalnej kolejki
    while (!messagesQueS.empty())
    {
        Message mess = messagesQueS.top();
        if (mess.sender != sender)
        {
            tempQueue.push(mess);
        }
        messagesQueS.pop();
    }

    // Przywracanie elementów do oryginalnej kolejki
    messagesQueS = move(tempQueue);
}

Message Robber::getFirstMessageFromQueS()
{
    return messagesQueS.top();
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

void Robber::insertMessageToQueN(Message message)
{
    messagesQueN.push(message);
}

void Robber::removeMessageFromQueN(int sender)
{
    priority_queue<Message> tempQueue;

    // Przeszukiwanie oryginalnej kolejki
    while (!messagesQueN.empty())
    {
        Message mess = messagesQueN.top();
        if (mess.sender != sender)
        {
            tempQueue.push(mess);
        }
        messagesQueN.pop();
    }

    // Przywracanie elementów do oryginalnej kolejki
    messagesQueN = move(tempQueue);
}

Message Robber::getFirstMessageFromQueN()
{
    if (!messagesQueN.empty())
        return messagesQueN.top();
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