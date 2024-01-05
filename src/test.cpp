#include <iostream>
#include <queue>
#include <vector>

#include "./Robber/Robber.h"

void testLamportClock()
{
    Robber robber(1, 1, 1);

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
    Robber robber(1, 1, 1);

    robber.insertMessageToQueS(Message(1, 10));
    robber.insertMessageToQueS(Message(2, 15));
    robber.insertMessageToQueS(Message(3, 20));
    robber.insertMessageToQueS(Message(4, 25));

    bool result = robber.isMessageInTopInQueS(2, 2);

    if (result)
        cout << "#1 testQueue: passed" << endl;
    else
        cout << "#1 testQueue: failed" << endl;

    result = robber.isMessageInTopInQueS(3, 2);

    if (!result)
        cout << "#2 testQueue: passed" << endl;
    else
        cout << "#2 testQueue: failed" << endl;

    robber.removeMessageFromQueS(1);
    robber.removeMessageFromQueS(2);
    robber.removeMessageFromQueS(3);
    robber.removeMessageFromQueS(4);

    result = robber.isQueSEmpty();

    if (result)
        cout << "#3 testQueue: passed" << endl;
    else
        cout << "#3 testQueue: failed" << endl;
}

void testVector()
{
    Robber robber(4, 1, 1);

    robber.setLastClock(0, 10);
    robber.setLastClock(1, 20);
    robber.setLastClock(2, 30);

    int responses = robber.countResponses();

    if (responses == 3)
        cout << "#1 testVector: passed" << endl;
    else
        cout << "#1 testVector: failed" << endl;

    bool isMyClockBiggest = robber.isMyClockInNBiggest(35, 2);

    if (isMyClockBiggest)
        cout << "#2 testVector: passed" << endl;
    else
        cout << "#2 testVector: failed" << endl;

    isMyClockBiggest = robber.isMyClockInNBiggest(15, 2);

    if (!isMyClockBiggest)
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