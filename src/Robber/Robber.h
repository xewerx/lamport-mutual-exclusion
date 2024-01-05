#include "../Message/Message.h"

using namespace std;

class Robber
{
public:
    Robber(int processes, int amountS, int amountN);

    bool isInterestedInS;
    bool isInterestedInN;

    int amountN;
    int amountS;

    int getLamportClock();
    void setLamportClock(int value);
    int incrementLamportClock();

    // Queues
    void insertMessageToQueS(Message message);
    void removeMessageFromQueS(int sender);
    bool isMessageInTopInQueS(int sender, int n);
    bool isQueSEmpty();

    void insertMessageToQueN(Message message);
    void removeMessageFromQueN(int sender);
    bool isMessageInTopInQueN(int sender, int n);
    bool isQueNEmpty();

    // Vector
    void setLastClock(int sender, int clock);
    bool isMyClockInNBiggest(int clock, int n);
    int countResponses();
    void printVector();

private:
    int lamportClock;

    priority_queue<Message> messagesQueS;
    priority_queue<Message> messagesQueN;

    vector<int> otherClocks;
};
