#include <iostream>

using namespace std;
int main(int argc, char *argv[])
{
    cout << "You called program " << argv[0] << " with arguments " << endl;
    for (int i = 1; i < argc; i++) {
        cout << argv[i] << " ";
    }
    cout << endl;
}
