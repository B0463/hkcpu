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
    CPU core; // define core as a CPU object
    
    loadRom(argc, argv); // load the ROM from arguments
    core.setRam(ROM); // load ROM to core RAM
    
    bool hlt;
    int clockCount;
    while(!hlt) { // cpu cicle
        if(clockCount == 10) {
            // flip the clock state every 10 cpu cicles
            clockCount = 0;
            core.flipClock();
        }
        if(core.propagate() == 2) hlt = 1; // propag CPU, if propagate return 2 (hlt) set hlt to 1 to stop the while.
        clockCount++;
    }
    cout << "hlt";
    core.getRam(ROM); // load the core RAM to ROM
    saveRom(argc, argv); // save the result ROM file
    return 0;
}
