/**
 * @file mpi_matmul_simple.cpp
 * @brief 使用对等模式、行分配实现矩阵乘法 
 * 快速运行：
 * mpic++ mpi_matmul_simple.cpp -O3 && mpirun -np 1 ./a.out
*/
#include "mpi_typedef.hpp"
#include "matrix.hpp"
#include <mpi.h>
#include <iostream>
#include <array>
using namespace std;
// 大小定义
// constexpr size_t MAT_SIZE = 2048;
// constexpr size_t MAT_SIZE = 1024;
// constexpr size_t MAT_SIZE = 512;
// constexpr size_t MAT_SIZE = 500;
// constexpr size_t MAT_SIZE = 3;
// 允许外部编译时定义。
#ifndef MAT_SIZE
// #define MAT_SIZE 500
#define MAT_SIZE 1000
#endif // !MAT_SIZE

constexpr size_t MAT_SIZE_SQUARED = MAT_SIZE*MAT_SIZE;
// 编译时生成矩阵和答案
constexpr array<array<double, MAT_SIZE*MAT_SIZE>, 3> get_ABC() {
// array<array<double, MAT_SIZE*MAT_SIZE>, 3> get_ABC() {
    auto result = array<array<double, MAT_SIZE*MAT_SIZE>, 3>{};
    for (size_t i = 0; i < MAT_SIZE; ++i) {
        for (size_t j = 0; j < MAT_SIZE; ++j) {
            result[0][i*MAT_SIZE+j] = i + j;
            result[1][i*MAT_SIZE+j] = i * j;
        }
    }
    // memory contined matmul to compute C
    for (size_t i = 0; i < MAT_SIZE; ++i) {
        for (size_t k = 0; k < MAT_SIZE; ++k) {
            const auto &temp = result[0][i*MAT_SIZE+k];
            for (size_t j = 0; j < MAT_SIZE; ++j) {
                result[2][i*MAT_SIZE+j] += temp * result[1][k*MAT_SIZE+j];
            }
        }
    }
    return result;
}
auto abc = get_ABC();
array<Matrix<double>, 3> get_ABC_mat(){
    auto result = array<Matrix<double>, 3>();
    for (size_t i = 0; i < 3; i++)
    {
        double* data = new double[MAT_SIZE_SQUARED];
        copy(abc[i].begin(), abc[i].end(), data);
        result[i].mData = std::unique_ptr<double[]>(data);
        result[i].mRowCount = MAT_SIZE;
        result[i].mColCount = MAT_SIZE;
    }
    return result;
}
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
    auto ABC = get_ABC_mat();
    cout<<"Get Matrix from compile time information."<<endl;
    const auto& a = ABC[0];
    const auto& b = ABC[1];
    const auto& bf_result = ABC[2];
    if (MAT_SIZE <= 10) {
        cout<<"A="<<a<<endl;
        cout<<"B="<<b<<endl;
    }
    cout<<"Start computing..."<<endl;
    //计算矩阵乘法
    double start = MPI_Wtime();
    // auto result = a.brute_force_matmul(b);
    // auto result = a.memory_continue_matmul(b);
    auto result = TMPI_MatMul(a, b);

    double finish = MPI_Wtime();
    double time = finish - start;
    printf("Done in %f seconds.\n", time);
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