/**
 * @file mpi_matmul_simple.cpp
 * @brief 使用主从模式、行分配实现矩阵乘法 
 * 快速运行：
 * mpic++ mpi_matmul_simple.cpp -O3 && mpirun -np 1 ./a.out
*/
#include "mpi_typedef.hpp"
#include "matrix.hpp"
#include <mpi.h>
#include <iostream>
using namespace std;
// 大小定义
constexpr size_t MAT_SIZE = 2048;
// constexpr size_t MAT_SIZE = 1024;
// constexpr size_t MAT_SIZE = 512;
// constexpr size_t MAT_SIZE = 500;
// constexpr size_t MAT_SIZE = 3;
// 函数提前声明
void leader();
template<typename Tp>
Matrix<Tp> TMPI_MatMul(Matrix<Tp> a, Matrix<Tp> b);
template <typename Tp=double>
void follower();
// MPI rank定义
constexpr int leader_rank = 0;
int mpi_rank, mpi_size;
// main
int main(int argc, char** argv) {
    //MPI程序初始化
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
    //主从模式
    if (mpi_rank == leader_rank){
        leader();
    }else{
        follower();
    }
    cout<<"Process "<<mpi_rank<<" finished."<<endl;
    MPI_Finalize();
    return 0;
}
void leader() {
    //主进程
    cout<<"I am the leader"<<endl;
    //生成两个矩阵
    auto a = Matrix(MAT_SIZE, MAT_SIZE);
    auto b = Matrix(MAT_SIZE, MAT_SIZE);
    //初始化矩阵
    for (size_t i = 0; i < MAT_SIZE; ++i) {
        for (size_t j = 0; j < MAT_SIZE; ++j) {
            a(i, j) = i + j;
            b(i, j) = i * j;
        }
    }
    if (MAT_SIZE <= 10) {
        cout<<"A="<<a<<endl;
        cout<<"B="<<b<<endl;
    }
    //计算矩阵乘法
    double start = MPI_Wtime();
    // auto result = a.brute_force_matmul(b);
    // auto result = a.memory_continue_matmul(b);
    auto result = TMPI_MatMul(a, b);

    double finish = MPI_Wtime();
    printf("Done in %f seconds.\n", finish - start);
    auto bf_result = a.brute_force_matmul(b);
    if (MAT_SIZE <= 10) {
        cout<<"res_actually="<<result<<endl;
        cout<<"res_expected="<<bf_result<<endl;
    }
    cout<<"Error: "<<bf_result.root_mean_square_error(result)<<endl;
}

template<typename Tp>
Matrix<Tp> TMPI_MatMul(Matrix<Tp> A, Matrix<Tp> B){
    auto global_result = zeros<Tp>(A.mRowCount, B.mColCount);
    auto result = zeros<Tp>(A.mRowCount, B.mColCount);

    MPI_Bcast(B.mData.get(), B.mColCount*B.mRowCount, mpi_type<Tp>(), leader_rank, MPI_COMM_WORLD);
    MPI_Bcast(A.mData.get(), A.mColCount*A.mRowCount, mpi_type<Tp>(), leader_rank, MPI_COMM_WORLD);


    // 一种行分配方式。
    for (size_t i = mpi_rank; i < A.mRowCount; i+=mpi_size) {
        for (size_t k = 0; k < A.mColCount; ++k) {
            const auto &temp = A(i, k);
            for (size_t j = 0; j < B.mColCount; ++j) {
                result(i, j) += temp * B(k, j);
            }
        }
    }
    // cout<<mpi_rank<<" reports "<<result<<endl;
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Reduce(result.mData.get(), global_result.mData.get(), result.mRowCount*result.mColCount, mpi_type<Tp>(), MPI_SUM, leader_rank, MPI_COMM_WORLD);
    return global_result;
}
template <typename Tp>
void follower() {
    //从进程
    cout<<"I am a follower"<<endl;
    auto a = Matrix(MAT_SIZE, MAT_SIZE);
    auto b = Matrix(MAT_SIZE, MAT_SIZE);
    TMPI_MatMul(a,b); // 主从进程都是这里
}