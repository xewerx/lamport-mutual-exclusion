#include <queue>
#include <vector>
#include <iostream>
#include <algorithm>

#include "./Robber.h"

Robber::Robber(int processes, int S, int N)
{
    isInterestedInS = false;
    isInterestedInN = false;
    amountS = S;
    amountN = N;
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

bool Robber::isMessageInTopInQueS(int sender, int n)
{
    priority_queue<Message> tempQueue = messagesQueS;

    // Przeszukiwanie oryginalnej kolejki
    for (int i = 0; i < n; i++)
    {
        Message mess = tempQueue.top();
        if (mess.sender == sender)
        {
            return true;
        }
        tempQueue.pop();
    }

    return false;
}

bool Robber::isQueSEmpty()
{
    return messagesQueS.empty();
}

bool Robber::isMessageInTopInQueN(int sender, int n)
{
    priority_queue<Message> tempQueue = messagesQueN;

    // Przeszukiwanie oryginalnej kolejki
    for (int i = 0; i < n; i++)
    {
        Message mess = tempQueue.top();
        if (mess.sender == sender)
        {
            return true;
        }
        tempQueue.pop();
    }

    return false;
}

bool Robber::isQueNEmpty()
{
    return messagesQueN.empty();
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

bool Robber::isMyClockInNBiggest(int clock, int n) // sprawdza czy clock posrod na najwiekszych clockow
{
    vector<int> otherClocksCopy = otherClocks;
    std::sort(otherClocksCopy.rbegin(), otherClocksCopy.rend());

    for (int i = 0; i < n; i++)
    {
        if (clock >= otherClocksCopy[i])
        {
            return true;
        }
    }

    return false;
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

void Robber::printQue()
{
    priority_queue<Message> tempQueue = messagesQueS;

    while (!tempQueue.empty())
    {
        cout << tempQueue.top().sender << " ";
        tempQueue.pop();
    }
    cout << endl;
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