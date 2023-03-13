#include <iostream>
#include <memory>
#include "matrix.hpp"
using namespace std;
int main(int argc, char const *argv[]) {
    auto a = ones<double>(3, 5);
    cout<<a<<endl;
    auto b = std::move(a);
    cout<<a<<endl;
    cout<<b<<endl;
    a = b;
    cout<<a<<endl;
    cout<<b<<endl;
    return 0;
}
