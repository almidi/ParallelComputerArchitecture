#!/bin/sh
gcc ./Proj1/main.c -o ./Proj1/Proj1
gcc -fopenmp ./Proj1OMP/main.c -o ./Proj1OMP/Proj1OMP
gcc -lpthread ./Proj1PT/main.c -o ./Proj1PT/Proj1PT

chmod 755 ./Proj1/Proj1
chmod 755 ./Proj1OMP/Proj1OMP
chmod 755 ./Proj1PT/Proj1PT

echo -e "---------------------- Arguments -------------------------"
echo -e "------------- m: 100 | n: 100 | l: 10 | t: 8 -------------"
echo -e "----------------------------------------------------------\n"
./Proj1/Proj1 100 100 10
./Proj1OMP/Proj1OMP 100 100 10 8
./Proj1PT/Proj1PT 100 100 10 8


echo -e "\n\n"
echo -e "---------------------- Arguments -------------------------"
echo -e "----------- m: 1000 | n: 1000 | l: 100 | t: 8 ------------"
echo -e "----------------------------------------------------------\n"
./Proj1/Proj1 1000 1000 100
./Proj1OMP/Proj1OMP 1000 1000 100 8
./Proj1PT/Proj1PT 1000 1000 100 8


echo -e "\n\n"
echo -e "---------------------- Arguments -------------------------"
echo -e "----------- m: 1000 | n: 1000 | l: 100 | t: 8 ------------"
echo -e "----------------------------------------------------------\n"
./Proj1/Proj1 1000 1000 1000
./Proj1OMP/Proj1OMP 1000 1000 1000 8
./Proj1PT/Proj1PT 1000 1000 1000 8


echo -e "\n\n"
echo -e "---------------------- Arguments -------------------------"
echo -e "----------- m: 1000 | n: 1000 | l: 100 | t: 8 ------------"
echo -e "----------------------------------------------------------\n"
./Proj1/Proj1 10000 10000 1000
./Proj1OMP/Proj1OMP 10000 10000 1000 8
./Proj1PT/Proj1PT 10000 10000 1000 8