#include <stdio.h>
#include <mpi.h>
#include <unistd.h>

#include <iostream>
#include <thread>
#include <queue>
#include <vector>
#include <mutex>
#include <condition_variable>

#include "./Robber/Robber.h"
#include "./Util/Util.h"
#include "./constants.h"

using namespace std;

Util util;

mutex mut;
condition_variable condVariableS;
condition_variable condVariableN;

void criticalSection(int rank, string criticalSectionName)
{
	cout << "[" << rank << "]: <SEKCJA KRYTYCZNA " << criticalSectionName << ">" << endl;
	sleep(rand() % 3);
	cout << "[" << rank << "]: <KONIEC SEKCJA KRYTYCZNA " << criticalSectionName << ">" << endl;
}

void sendToAll(int size, int clock, TAG tag)
{
	for (int i = 0; i < size; ++i)
	{
		MPI_Send(&clock, 1, MPI_INT, i, tag, MPI_COMM_WORLD);
	}
}

void handleMessage(Robber *robber, int rank, int size)
{
	MPI_Status status;
	int receivedClock;

	while (true)
	{
		MPI_Recv(&receivedClock, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

		// Aktualizacja zegara Lamporta
		robber->setLamportClock(max(robber->getLamportClock(), receivedClock) + 1);

		switch (status.MPI_TAG)
		{
		case REQ_S:
		{
			mut.lock(); // zawsze po wysłaniu sie odblokuje
			// cout << "[" << rank << "]: received REQ " << receivedClock << " from [" << status.MPI_SOURCE << "]" << endl;

			robber->insertMessageToQueS(Message(status.MPI_SOURCE, receivedClock));

			int updatedClock = robber->getLamportClock();

			if (status.MPI_SOURCE != rank) // nie wysyla sam do siebie bo nikt nie odbierze - deadlock
			{
				MPI_Send(&updatedClock, 1, MPI_INT, status.MPI_SOURCE, ACK_S, MPI_COMM_WORLD);
			}

			mut.unlock();
			break;
		}
		case RELEASE_S:
		{
			mut.lock();
			// cout << "[" << rank << "]: received RELEASE " << receivedClock << " from [" << status.MPI_SOURCE << "]" << endl;

			robber->removeMessageFromQueS(status.MPI_SOURCE);

			// Aktualizacja wektora zegarow innych procesow
			robber->setLastClock(status.MPI_SOURCE, receivedClock);

			mut.unlock();
			break;
		}
		case ACK_S:
		{
			mut.lock();
			// cout << "[" << rank << "]: received ACK " << receivedClock << " from [" << status.MPI_SOURCE << "]" << endl;

			// Aktualizacja wektora zegarow innych procesow
			robber->setLastClock(status.MPI_SOURCE, receivedClock);

			mut.unlock();
			break;
		}
		case REQ_N:
		{
			mut.lock(); // zawsze po wysłaniu sie odblokuje
			// cout << "[" << rank << "]: received REQ " << receivedClock << " from [" << status.MPI_SOURCE << "]" << endl;

			robber->insertMessageToQueN(Message(status.MPI_SOURCE, receivedClock));

			int updatedClock = robber->getLamportClock();

			if (status.MPI_SOURCE != rank) // nie wysyla sam do siebie bo nikt nie odbierze - deadlock
			{
				MPI_Send(&updatedClock, 1, MPI_INT, status.MPI_SOURCE, ACK_N, MPI_COMM_WORLD);
			}

			mut.unlock();
			break;
		}
		case RELEASE_N:
		{
			mut.lock();
			// cout << "[" << rank << "]: received RELEASE " << receivedClock << " from [" << status.MPI_SOURCE << "]" << endl;

			robber->removeMessageFromQueN(status.MPI_SOURCE);

			// Aktualizacja wektora zegarow innych procesow
			robber->setLastClock(status.MPI_SOURCE, receivedClock);

			mut.unlock();
			break;
		}
		case ACK_N:
		{
			mut.lock();
			// cout << "[" << rank << "]: received ACK " << receivedClock << " from [" << status.MPI_SOURCE << "]" << endl;

			// Aktualizacja wektora zegarow innych procesow
			robber->setLastClock(status.MPI_SOURCE, receivedClock);

			mut.unlock();
			break;
		}
		}

		// sprawdz warunki i wejdz do sekcji
		if (robber->isInterestedInS)
		{
			Message firstMessageInQue = robber->getFirstMessageFromQueS();

			int currentClock = robber->getLamportClock();
			int responsesAmount = robber->countResponses();

			int isMyClockBiggest = robber->isMyClockBiggest(currentClock);

			// cout << "[" << rank << "]: RANK " << firstMessageInQue.sender << " " << responsesAmount << " " << currentClock << " " << isMyClockBiggest << endl;
			// robber->printVector();

			// wlasne zadanie na szczycie kolejki    i    mamy ack od wszystkich pozostalych ze starszym timestamp
			if (firstMessageInQue.sender == rank && (responsesAmount == size - 1) && isMyClockBiggest)
			{
				lock_guard<std::mutex> lock(mut);
				condVariableS.notify_one();
			}
		}

		// sprawdz warunki i wejdz do sekcji
		if (robber->isInterestedInN)
		{
			Message firstMessageInQue = robber->getFirstMessageFromQueN();

			int currentClock = robber->getLamportClock();
			int responsesAmount = robber->countResponses();

			int isMyClockBiggest = robber->isMyClockBiggest(currentClock);

			// cout << "[" << rank << "]: RANK " << firstMessageInQue.sender << " " << responsesAmount << " " << currentClock << " " << isMyClockBiggest << endl;
			// robber->printVector();

			// wlasne zadanie na szczycie kolejki    i    mamy ack od wszystkich pozostalych ze starszym timestamp
			if (firstMessageInQue.sender == rank && (responsesAmount == size - 1) && isMyClockBiggest)
			{
				lock_guard<std::mutex> lock(mut);
				condVariableN.notify_one();
			}
		}
	}
}

void requestForS(Robber *robber, int size)
{
	robber->isInterestedInS = true;
	int clock = robber->incrementLamportClock();
	sendToAll(size, clock, REQ_S);
}

void waitForS()
{
	std::unique_lock<std::mutex> lock(mut);
	condVariableS.wait(lock);
}

void releaseS(Robber *robber, int size, int rank)
{
	sleep(3);

	robber->isInterestedInS = false;
	robber->removeMessageFromQueS(rank);
	int clock = robber->getLamportClock();

	sendToAll(size, clock, RELEASE_S);
}

void requestForN(Robber *robber, int size)
{
	robber->isInterestedInN = true;
	int clock = robber->incrementLamportClock();
	sendToAll(size, clock, REQ_N);
}

void waitForN()
{
	std::unique_lock<std::mutex> lock(mut);
	condVariableN.wait(lock);
}

void releaseN(Robber *robber, int size, int rank)
{
	robber->isInterestedInN = false;
	robber->removeMessageFromQueN(rank);
	int clock = robber->getLamportClock();

	sendToAll(size, clock, RELEASE_N);
}

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		cout << "Provide 2 parameters: S, N" << endl;
		return 1;
	}

	const int S = atoi(argv[1]);
	const int N = atoi(argv[2]);

	int rank;
	int size; // K
	int provided;

	MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	Robber robber(size);

	thread receiverThread(handleMessage, &robber, rank, size);

	// START MAIN FLOW

	// SEKCJA KRYTYCZNA 1 - pobranie S
	requestForS(&robber, size);
	waitForS();
	criticalSection(rank, "S"); // krazenie po miescie
	bool isPersonWithGoodMoodFound = util.getRandomBoolean();
	thread releaseSThread(releaseS, &robber, size, rank); // S zwracane po jakims czasie

	if (isPersonWithGoodMoodFound)
	{
		requestForN(&robber, size);
		waitForN();
		criticalSection(rank, "N"); // laboratorium
		releaseN(&robber, size, rank);
	}

	// END MAIN FLOW

	receiverThread.join();
	releaseSThread.join();
	MPI_Finalize();
}