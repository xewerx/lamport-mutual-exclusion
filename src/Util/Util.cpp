#include <cstdlib>
#include <ctime>
#include <iostream>

#include "./Util.h"

Util::Util(bool isDebugModeEnabled) : isDebugModeEnabled(isDebugModeEnabled)
{
}

bool Util::getRandomBoolean()
{
    srand(time(0));

    int randomValue = rand() % 2;

    return (randomValue == 1);
}

void Util::debugLog(const string &message, int rank, int clock)
{
    if (isDebugModeEnabled)
    {
        cout << "[DEBUG][" << rank << "][t" << clock << "]: " << message << endl;
    }
}

void Util::infoLog(const string &message, int rank, int clock)
{
    if (clock == -1)
    {
        cout << "[INFO][" << rank << "]: " << message << endl;
    }
    else
    {
        cout << "[INFO][" << rank << "][t" << clock << "]: " << message << endl;
    }
}