#include <iostream>
#include <queue>
#include <vector>

#include "./Robber/Robber.h"

void testLamportClock()
{
    Robber robber(1);

    int lamportClock = robber.getLamportClock();

    if (lamportClock == 0)
        cout
            << "#1 testLamportClock: passed" << endl;
    else
        cout << "#1 testLamportClock: failed" << endl;

    robber.incrementLamportClock();
    lamportClock = robber.getLamportClock();

    if (lamportClock == 1)
        cout
            << "#2 testLamportClock: passed" << endl;
    else
        cout << "#2 testLamportClock: failed" << endl;

    robber.setLamportClock(999);
    lamportClock = robber.getLamportClock();

    if (lamportClock == 999)
        cout
            << "#3 testLamportClock: passed" << endl;
    else
        cout << "#3 testLamportClock: failed" << endl;
}

void testQueue()
{
    Robber robber(1);

    robber.insertMessageToQueS(Message(1, 3));
    robber.insertMessageToQueS(Message(2, 5));
    robber.insertMessageToQueS(Message(3, 3));
    robber.insertMessageToQueS(Message(4, 21));

    Message firstMessageInQue = robber.getFirstMessageFromQueS();

    if (firstMessageInQue.sender == 1)
        cout << "#1 testQueue: passed" << endl;
    else
        cout << "#1 testQueue: failed" << endl;

    robber.removeMessageFromQueS(1);
    firstMessageInQue = robber.getFirstMessageFromQueS();

    if (firstMessageInQue.sender == 3)
        cout << "#2 testQueue: passed" << endl;
    else
        cout << "#2 testQueue: failed" << endl;

    robber.removeMessageFromQueS(3);
    firstMessageInQue = robber.getFirstMessageFromQueS();

    if (firstMessageInQue.sender == 2)
        cout << "#1 testQueue: passed" << endl;
    else
        cout << "#1 testQueue: failed" << endl;
}

void testVector()
{
    Robber robber(4);

    robber.setLastClock(0, 5);
    robber.setLastClock(1, 3);
    robber.setLastClock(2, 2);

    int responses = robber.countResponses();

    if (responses == 3)
        cout << "#1 testVector: passed" << endl;
    else
        cout << "#1 testVector: failed" << endl;

    int isMyClockBiggest = robber.isMyClockBiggest(2);

    if (isMyClockBiggest == 0)
        cout << "#2 testVector: passed" << endl;
    else
        cout << "#2 testVector: failed" << endl;
}

int main(int argc, char **argv)
{
    testQueue();
    testVector();
    testLamportClock();
}