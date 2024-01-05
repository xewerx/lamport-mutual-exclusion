using namespace std;

class Util
{
public:
    Util(bool isDebugModeEnabled);

    bool isDebugModeEnabled;

    bool getRandomBoolean();

    void debugLog(const string &message, int rank, int clock = -1);
    void infoLog(const string &message, int rank, int clock = -1);
};
