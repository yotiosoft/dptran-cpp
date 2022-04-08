#include <stdio.h>
#include <unistd.h>

using namespace std;

int main() {
    cout << "Waiting..";

    sleep(3);

    cout << "\r";
    cout << "Hello World!";
}
