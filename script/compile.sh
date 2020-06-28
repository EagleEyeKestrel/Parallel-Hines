echo "compiling serial program..."
g++ -std=c++11 -fopenmp -o ../src/serial ../src/serial.cpp
echo "compiling parallel program..."
g++ -std=c++11 -fopenmp -o ../src/parallel ../src/parallel.cpp
