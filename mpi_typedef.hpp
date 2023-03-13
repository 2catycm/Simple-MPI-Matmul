#include <mpi.h>
//使用模版特化的方法来定义MPI_Datatype
template <typename T>
constexpr MPI_Datatype mpi_type(){
    return MPI_DOUBLE;
}
template<>
constexpr MPI_Datatype mpi_type<int>(){
    return MPI_INT;
}
template<>
constexpr MPI_Datatype mpi_type<float>(){
    return MPI_FLOAT;
}
template<>
constexpr MPI_Datatype mpi_type<double>(){
    return MPI_DOUBLE;
}
template<>
constexpr MPI_Datatype mpi_type<long double>(){
    return MPI_LONG_DOUBLE;
}
template<>
constexpr MPI_Datatype mpi_type<long long>(){
    return MPI_LONG_LONG;
}
template<>
constexpr MPI_Datatype mpi_type<unsigned long long>(){
    return MPI_UNSIGNED_LONG_LONG;
}
template<>
constexpr MPI_Datatype mpi_type<unsigned long>(){
    return MPI_UNSIGNED_LONG;
}
template<>  
constexpr MPI_Datatype mpi_type<unsigned int>(){
    return MPI_UNSIGNED;
}
template<>
constexpr MPI_Datatype mpi_type<unsigned short>(){
    return MPI_UNSIGNED_SHORT;
}
template<>
constexpr MPI_Datatype mpi_type<unsigned char>(){
    return MPI_UNSIGNED_CHAR;
}
template<>
constexpr MPI_Datatype mpi_type<short>(){
    return MPI_SHORT;
}
template<>
constexpr MPI_Datatype mpi_type<char>(){
    return MPI_CHAR;
}
template<>
constexpr MPI_Datatype mpi_type<bool>(){
    return MPI_CXX_BOOL;
}