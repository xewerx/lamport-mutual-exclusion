#include <cstdlib>
#include <ctime>

#include "./Util.h"

bool Util::getRandomBoolean()
{
    srand(time(0));

    int randomValue = rand() % 2;

    return (randomValue == 1);
}
