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

Util *util = nullptr;

mutex mut;
condition_variable condVariableS;
condition_variable condVariableN;

void criticalSection(int rank, string criticalSectionName)
{
	util->infoLog("<SEKCJA KRYTYCZNA " + criticalSectionName + ">", rank);
	sleep(rand() % 3);
	util->infoLog("<KONIEC SEKCJA KRYTYCZNA " + criticalSectionName + ">", rank);
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
	int sender = status.MPI_SOURCE;

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
			util->debugLog("received REQ_S from [" + to_string(sender) + "]", rank, receivedClock);

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
			util->debugLog("received RELEASE_S from [" + to_string(sender) + "]", rank, receivedClock);

			robber->removeMessageFromQueS(status.MPI_SOURCE);

			// Aktualizacja wektora zegarow innych procesow
			robber->setLastClock(status.MPI_SOURCE, receivedClock);

			mut.unlock();
			break;
		}
		case ACK_S:
		{
			mut.lock();
			util->debugLog("received ACK_S from [" + to_string(sender) + "]", rank, receivedClock);

			// Aktualizacja wektora zegarow innych procesow
			robber->setLastClock(status.MPI_SOURCE, receivedClock);

			mut.unlock();
			break;
		}
		case REQ_N:
		{
			mut.lock(); // zawsze po wysłaniu sie odblokuje
			util->debugLog("received REQ_N from [" + to_string(sender) + "]", rank, receivedClock);

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
			util->debugLog("received RELEASE_N from [" + to_string(sender) + "]", rank, receivedClock);

			robber->removeMessageFromQueN(status.MPI_SOURCE);

			// Aktualizacja wektora zegarow innych procesow
			robber->setLastClock(status.MPI_SOURCE, receivedClock);

			mut.unlock();
			break;
		}
		case ACK_N:
		{
			mut.lock();
			util->debugLog("received ACK_N from [" + to_string(sender) + "]", rank, receivedClock);

			// Aktualizacja wektora zegarow innych procesow
			robber->setLastClock(status.MPI_SOURCE, receivedClock);

			mut.unlock();
			break;
		}
		}

		// sprawdz warunki i wejdz do sekcji S
		if (robber->isInterestedInS && !robber->isQueSEmpty())
		{
			int amountS = robber->amountS;

			int currentClock = robber->getLamportClock();
			int responsesAmount = robber->countResponses();

			bool cond1 = robber->isMessageInTopInQueS(rank, amountS);
			bool cond2 = robber->isMyClockInNBiggest(currentClock, amountS);

			util->debugLog("< " + to_string(currentClock) + " " + to_string(responsesAmount) + " " + to_string(cond1) + " " + to_string(cond2) + " >", rank, currentClock);

			// wlasne zadanie na szczycie kolejki Top N    i    mamy ack ze starszym clockiem od przynajmniej wszystkich - N
			if (cond1 && (responsesAmount > size - amountS) && cond2)
			{
				lock_guard<mutex> lock(mut);
				condVariableS.notify_one();
			}
		}

		// sprawdz warunki i wejdz do sekcji N
		if (robber->isInterestedInN && !robber->isQueNEmpty()) // nie czytaj kolejki jesli pusta byku tu we sprawdz czy pelna
		{
			int amountN = robber->amountN;

			int currentClock = robber->getLamportClock();
			int responsesAmount = robber->countResponses();

			bool cond1 = robber->isMessageInTopInQueN(rank, amountN);
			bool cond2 = robber->isMyClockInNBiggest(currentClock, amountN);

			util->debugLog("< " + to_string(currentClock) + " " + to_string(responsesAmount) + " " + to_string(cond1) + " " + to_string(cond2) + " >", rank, currentClock);

			// wlasne zadanie na szczycie kolejki Top N    i    mamy ack ze starszym clockiem od przynajmniej wszystkich - N
			if (cond1 && (responsesAmount > size - amountN) && cond2)
			{
				lock_guard<mutex> lock(mut);
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
	unique_lock<mutex> lock(mut);
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
	unique_lock<mutex> lock(mut);
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
	if (argc < 3)
	{
		cerr << "Provide 2 parameters: S, N" << endl;
		return 1;
	}

	bool IS_DEBUG_MODE_ENABLED = false;
	if (argc == 4)
	{
		IS_DEBUG_MODE_ENABLED = string(argv[3]) == "--debug";
	}

	util = new Util(IS_DEBUG_MODE_ENABLED);

	const int S = atoi(argv[1]);
	const int N = atoi(argv[2]);

	int rank;
	int size; // K
	int provided;

	MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	Robber robber(size, S, N);

	thread receiverThread(handleMessage, &robber, rank, size);

	// START MAIN FLOW

	// SEKCJA KRYTYCZNA 1 - pobranie S
	requestForS(&robber, size);
	waitForS();
	criticalSection(rank, "S"); // krazenie po miescie
	bool isPersonWithGoodMoodFound = util->getRandomBoolean();
	thread releaseSThread(releaseS, &robber, size, rank); // S zwracane po jakims czasie

	if (isPersonWithGoodMoodFound)
	{
		// SEKCJA KRYTYCZNA 2 - pobranie N
		requestForN(&robber, size);
		waitForN();
		criticalSection(rank, "N"); // laboratorium
		releaseN(&robber, size, rank);
	}

	// END MAIN FLOW

	releaseSThread.join();
	receiverThread.join();
	MPI_Finalize();
}