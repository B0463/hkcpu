#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <bitset>

using namespace std;

u_int8_t ROM[256];

#include "./cpu.h"
#include "./fileMgr.h"

int main(int argc, char *argv[]) {
    CPU core;
    
    loadRom(argc, argv);
    core.setRam(ROM);
    
    bool hlt;
    int clockCount;
    while(!hlt) {
        if(clockCount == 10) {
            clockCount = 0;
            core.flipClock();
        }
        if(core.propagate() == 2) hlt = 1;
        clockCount++;
    }
    cout << "hlt";
    core.getRam(ROM);
    saveRom(argc, argv);
    return 0;
}
