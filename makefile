CC=mpicc -std=c11
CXX=mpic++ -std=c++17
all: leader.exe peer.exe
peer: mpi_matmul_peer_pattern.exe
	mpirun -np 4 out/mpi_matmul_peer_pattern.exe

mpi_matmul_peer_pattern.exe: mpi_matmul_peer_pattern.cpp out
	$(CXX) -o out/mpi_matmul_peer_pattern.exe mpi_matmul_peer_pattern.cpp -O3
	

leader.exe: mpi_matmul_simple.cpp out
	$(CXX) -o out/mpi_matmul_simple.exe mpi_matmul_simple.cpp -O3

out:
	mkdir out

test_cpp: test_cpp.exe
	out/test_cpp.exe
test_cpp.exe: test_cpp.cpp
	$(CXX) -o out/test_cpp.exe test_cpp.cpp -O3