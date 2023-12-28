#include "../Message/Message.h"

#ifndef ROBBER_H
#define ROBBER_H

using namespace std;

class Robber
{
public:
    Robber(int processes);

    int getLamportClock();
    void setLamportClock(int value);
    int incrementLamportClock();

    // Queue
    void insertMessageToQue(Message message);
    void removeMessageFromQue(int sender);
    Message getFirstMessageFromQue();

    // Vector
    void setLastClock(int sender, int clock);
    bool isMyClockBiggest(int clock);
    int countResponses();
    void printVector();

private:
    int lamportClock;
    priority_queue<Message> messagesQue;
    vector<int> otherClocks;
};

#endif