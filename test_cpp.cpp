#include <iostream>
#include <memory>
#include <array>
using namespace std;
int main(int argc, char const *argv[]) {
    int a = 1;
    int b = std::move(a);
    cout << a << endl;
    cout << b << endl;
    array<int, 10> c= {1,2,3,4,5,6,7,8,9,10};
    int* cp = new int[10];
    std::copy(c.begin(), c.end(), cp);
    auto d = unique_ptr<int[]>(cp);
    cout << d[1] << endl;
    return 0;
}
