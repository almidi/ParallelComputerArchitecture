#!/bin/sh
cmake --build ./Proj2Ref/cmake-build-debug --target Proj2Ref -- -j 4
cmake --build ./Proj2SSE/cmake-build-debug --target Proj2SSE -- -j 4
cmake --build ./Proj2MPI/cmake-build-debug --target Proj2MPI -- -j 4


chmod 755 ./Proj2Ref/cmake-build-debug/Proj2Ref
chmod 755 ./Proj2SSE/cmake-build-debug/Proj2SSE
chmod 755 ./Proj2MPI/cmake-build-debug/Proj2MPI

echo -e "\n\n"
echo -e "---------------------- Serial Execution -------------------------"
echo -e "- N: 100"
./Proj2Ref/cmake-build-debug/Proj2Ref 100
echo -e ""
echo -e "- N: 1000"
./Proj2Ref/cmake-build-debug/Proj2Ref 1000
echo -e ""
echo -e "- N: 10000"
./Proj2Ref/cmake-build-debug/Proj2Ref 10000
echo -e ""
echo -e "- N: 100000"
./Proj2Ref/cmake-build-debug/Proj2Ref 100000


echo -e "\n\n"
echo -e "---------------------- SSE Execution ----------------------------"
echo -e "- N: 100"
./Proj2SSE/cmake-build-debug/Proj2SSE 100
echo -e ""
echo -e "- N: 1000"
./Proj2SSE/cmake-build-debug/Proj2SSE 1000
echo -e ""
echo -e "- N: 10000"
./Proj2SSE/cmake-build-debug/Proj2SSE 10000
echo -e ""
echo -e "- N: 100000"
./Proj2SSE/cmake-build-debug/Proj2SSE 100000


echo -e "\n\n"
echo -e "---------------------- SSE & MPI Execution ----------------------"
echo -e "- N: 100 | P: 2"
mpiexec -np 2 ./Proj2MPI/cmake-build-debug/Proj2MPI 100
echo -e ""
echo -e "- N: 100 | P: 4"
mpiexec -np 4 ./Proj2MPI/cmake-build-debug/Proj2MPI 100
echo -e ""
echo -e "- N: 100 | P: 8"
mpiexec -np 8 ./Proj2MPI/cmake-build-debug/Proj2MPI 100

echo -e ""
echo -e "- N: 1000 | P: 2"
mpiexec -np 2 ./Proj2MPI/cmake-build-debug/Proj2MPI 1000
echo -e ""
echo -e "- N: 1000 | P: 4"
mpiexec -np 4 ./Proj2MPI/cmake-build-debug/Proj2MPI 1000
echo -e ""
echo -e "- N: 1000 | P: 8"
mpiexec -np 8 ./Proj2MPI/cmake-build-debug/Proj2MPI 1000

echo -e ""
echo -e "- N: 10000 | P: 2"
mpiexec -np 2 ./Proj2MPI/cmake-build-debug/Proj2MPI 10000
echo -e ""
echo -e "- N: 10000 | P: 4"
mpiexec -np 4 ./Proj2MPI/cmake-build-debug/Proj2MPI 10000
echo -e ""
echo -e "- N: 10000 | P: 8"
mpiexec -np 8 ./Proj2MPI/cmake-build-debug/Proj2MPI 10000

echo -e ""
echo -e "- N: 100000 | P: 2"
mpiexec -np 2 ./Proj2MPI/cmake-build-debug/Proj2MPI 100000
echo -e ""
echo -e "- N: 100000 | P: 4"
mpiexec -np 4 ./Proj2MPI/cmake-build-debug/Proj2MPI 100000
echo -e ""
echo -e "- N: 100000 | P: 8"
mpiexec -np 8 ./Proj2MPI/cmake-build-debug/Proj2MPI 100000