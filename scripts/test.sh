mpic++ src/test.cpp src/Robber/Robber.cpp -o bin/test -std=c++14
mpiexec -n 1 ./bin/test