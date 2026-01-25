#include "rasos.h"

int main(int argc, char** argv) {
    print("Hello, what's up my man\n");
    while(1) {
        if (getkey() != 0) {
            print("key was pressed\n");
        }
    }
    return 0;
}
