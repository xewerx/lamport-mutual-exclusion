#include "../Message/Message.h"

using namespace std;

class Robber
{
public:
    Robber(int processes);

    // state - na jego podstawie switch mozna zminimalizowac do 3 casow

    bool isInterestedInS;
    bool isInterestedInN;

    int getLamportClock();
    void setLamportClock(int value);
    int incrementLamportClock();

    // Queues
    void insertMessageToQueS(Message message);
    void removeMessageFromQueS(int sender);
    Message getFirstMessageFromQueS();

    void insertMessageToQueN(Message message);
    void removeMessageFromQueN(int sender);
    Message getFirstMessageFromQueN();

    // Vector
    void setLastClock(int sender, int clock);
    bool isMyClockBiggest(int clock);
    int countResponses();
    void printVector();

private:
    int lamportClock;

    priority_queue<Message> messagesQueS;
    priority_queue<Message> messagesQueN;

    vector<int> otherClocks;
};
