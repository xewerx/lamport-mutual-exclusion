#include <stdio.h>
#include <mpi.h>
#include <unistd.h>

#include <iostream>
#include <thread>
#include <queue>
#include <vector>
#include <mutex>

#include "./Robber/Robber.h"
#include "./constants.h"

using namespace std;

bool IS_INTERESTED_IN_CS = false;
mutex mut;

void criticalSection()
{
	sleep(3);
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
		case REQ:
		{
			mut.lock(); // zawsze po wysłaniu sie odblokuje
			cout << "[" << rank << "]: received REQ " << receivedClock << " from [" << status.MPI_SOURCE << "]" << endl;

			robber->insertMessageToQue(Message(status.MPI_SOURCE, receivedClock));

			int updatedClock = robber->getLamportClock();

			if (status.MPI_SOURCE != rank) // nie wysyla sam do siebie bo nikt nie odbierze - deadlock
			{
				MPI_Send(&updatedClock, 1, MPI_INT, status.MPI_SOURCE, ACK, MPI_COMM_WORLD);
			}

			mut.unlock();
			break;
		}
		case RELEASE:
		{
			mut.lock();
			cout << "[" << rank << "]: received RELEASE " << receivedClock << " from [" << status.MPI_SOURCE << "]" << endl;

			robber->removeMessageFromQue(status.MPI_SOURCE);

			// Aktualizacja wektora zegarow innych procesow
			robber->setLastClock(status.MPI_SOURCE, receivedClock);

			mut.unlock();
			break;
		}
		case ACK:
		{
			mut.lock();
			cout << "[" << rank << "]: received ACK " << receivedClock << " from [" << status.MPI_SOURCE << "]" << endl;

			// Aktualizacja wektora zegarow innych procesow
			robber->setLastClock(status.MPI_SOURCE, receivedClock);

			mut.unlock();
			break;
		}
		}

		// sprawdz warunki i wejdz do sekcji
		// TODO: refactor do uzywania conditional variable
		if (IS_INTERESTED_IN_CS)
		{
			Message firstMessageInQue = robber->getFirstMessageFromQue();

			int currentClock = robber->getLamportClock();
			int responsesAmount = robber->countResponses();

			int isMyClockBiggest = robber->isMyClockBiggest(currentClock);

			cout << "[" << rank << "]: RANK " << firstMessageInQue.sender << " " << responsesAmount << " " << currentClock << " " << isMyClockBiggest << endl;
			robber->printVector();

			// wlasne zadanie na szycie kolejki    i    mamy ack od wszystkich pozostalych ze starszym timestamp
			if (firstMessageInQue.sender == rank && (responsesAmount == size - 1) && isMyClockBiggest)
			{
				cout << "[" << rank << "]: <SEKCJA KRYTYCZNA>" << endl;
				criticalSection();
				IS_INTERESTED_IN_CS = false;
				robber->removeMessageFromQue(rank);
				int clock = robber->getLamportClock();
				for (int i = 0; i < size; ++i)
				{
					MPI_Send(&clock, 1, MPI_INT, i, RELEASE, MPI_COMM_WORLD);
				}
				cout << "[" << rank << "]: <KONIEC SEKCJA KRYTYCZNA>" << endl;
			}
		}
	}
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
	int size;
	int provided;

	MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	Robber robber(size);

	thread receiverThread(handleMessage, &robber, rank, size);

	// Wysyłanie REQ do kazdego
	if (rank == 0 || rank == 2)
	{
		IS_INTERESTED_IN_CS = true;
		int clock = robber.incrementLamportClock();

		for (int i = 0; i < size; ++i)
		{
			mut.lock();
			MPI_Send(&clock, 1, MPI_INT, i, REQ, MPI_COMM_WORLD);
			cout << "[" << rank << "]: sent " << clock << " to [" << i << "]" << endl;
			mut.unlock();
		}
	}

	receiverThread.join();
	MPI_Finalize();
}