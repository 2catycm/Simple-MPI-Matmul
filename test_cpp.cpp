#include <iostream>
#include <memory>
using namespace std;
int main(int argc, char const *argv[]) {
    int a = 1;
    int b = std::move(a);
    cout << a << endl;
    cout << b << endl;
    return 0;
}
