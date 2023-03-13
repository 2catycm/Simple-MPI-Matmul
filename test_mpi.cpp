/**
 * @file test_mpi.cpp
 * @brief 测试MPI语言特性
 * 快速运行：
 * mpic++ test_mpi.cpp -O3 && mpirun -np 2 ./a.out
*/
#include <mpi.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <numeric>
using namespace std;
int mpi_rank, mpi_size;
void leader();
void follower();
int main(int argc, char** argv) {
    //MPI程序初始化
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
    //主从模式
    if (mpi_rank == 0) {
        leader();
    } else {
        follower();
    }
    cout<<"Process "<<mpi_rank<<" finished."<<endl;
    MPI_Finalize();
    return 0;
}
// test1 
// void leader() {
//     int data[] = {1,2,3};
//     MPI_Send(data, 3, MPI_INT, 1, 0, MPI_COMM_WORLD);
// }

// void follower() {
//     cout<<"follower waiting."<<endl;
//     std::this_thread::sleep_for(std::chrono::seconds(2));
//     int data[] = {0,0,0};
//     MPI_Recv(data, 3, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//     cout<<"follower received: "<<data[0]<<data[1]<<data[2]<<endl;
// }

//test2 
void leader() {
    constexpr size_t num_elements = 100'000;
    vector<int> data(num_elements); 
    iota(data.begin(), data.end(), 0);

    MPI_Barrier(MPI_COMM_WORLD);
    auto time_start = MPI_Wtime();
    MPI_Bcast(&data[0], num_elements, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    auto time_end = MPI_Wtime();
    if (mpi_rank == 0)
    {
        cout<<"finished in "<<time_end - time_start<<" seconds."<<endl;
    }
}
void follower(){
    leader();
}