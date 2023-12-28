#include <queue>
#include <vector>
#include <iostream>
#include <algorithm>

#include "./Robber.h"

Robber::Robber()
{
    lamportClock = 0;
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
    std::priority_queue<Message> tempQueue;

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
    messagesQue = std::move(tempQueue);
}

Message Robber::getFirstMessageFromQue()
{
    return messagesQue.top();
}

void Robber::setLastClock(int sender, int clock)
{
    other_clocks.at(sender) = clock;
}

int Robber::countResponses()
{
    int responsesAmount = 0;
    for (int el : other_clocks)
    {
        if (el != -1)
            responsesAmount++;
    }

    return responsesAmount;
}

int Robber::getLowestClockIndex()
{
    // Znajdź iterator do najmniejszego elementu
    auto minElementIterator = min_element(other_clocks.begin(), other_clocks.end());

    // Oblicz indeks na podstawie iteratora
    int indexOfMinElement = distance(other_clocks.begin(), minElementIterator);

    return indexOfMinElement;
}

int Robber::getClock(int index)
{
    return other_clocks[index];
}

bool Robber::isMyClockBiggest(int clock)
{
    for (int el : other_clocks)
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
    for (int element : other_clocks)
    {
        cout << element << " ";
    }
    cout << endl;
}