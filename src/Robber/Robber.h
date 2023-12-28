#include "../Message/Message.h"

#ifndef ROBBER_H
#define ROBBER_H

using namespace std;

class Robber
{
public:
    Robber();

    int getLamportClock();
    void setLamportClock(int value);
    int incrementLamportClock();

    // Queue
    void insertMessageToQue(Message message);
    void removeMessageFromQue(int sender);
    Message getFirstMessageFromQue();

    // Vector
    void setLastClock(int index, int clock);
    int getLowestClockIndex();
    int getClock(int index);
    bool isMyClockBiggest(int clock);
    int countResponses();
    void printVector();

private:
    int lamportClock;
    priority_queue<Message> messagesQue;
    // TODO: dynamic size
    std::vector<int> other_clocks = {-1, -1, -1, -1};
};

#endif