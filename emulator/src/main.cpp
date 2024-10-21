#include <iostream>
#include <fstream>
#include <string>

using namespace std;

u_int8_t ROM[256];
u_int32_t controlBus;
u_int8_t addr;
u_int8_t flg;
u_int8_t pc;
u_int8_t lc;
u_int8_t ra;
u_int8_t rb;
u_int8_t rc;
u_int8_t ar;
u_int8_t ir;
u_int8_t ro;

void alu(u_int8_t *buso, bool clock, bool rst);
void programCounter(u_int8_t *buso, u_int8_t busi, bool lastClock, bool clock, bool rst);
void regs(u_int8_t *buso, u_int8_t busi, bool clock, bool rst);
void mem(u_int8_t *buso, u_int8_t busi, bool clock, bool rst);
void rout(u_int8_t busi, bool clock, bool rst);
void lu(u_int8_t *buso, u_int8_t busi, bool lastClock, bool clock, bool rst);

int main(int argc, char *argv[]) {
    if(argc <= 1) {
        cout << "add a rom, exemple:\n./hkcpu-emulator ./memory.rom\n";
        exit(1);
    }
    cout << "hkcpu emulator - 8bits CPU\n";
    string romfile = argv[1];
    ifstream arq(romfile, ios::binary | ios::ate);
    if(!arq) {
        cout << "can't open: " << romfile << "\n";
        exit(1);
    }
    streamsize romSize = arq.tellg();
    if(romSize != sizeof(ROM)) {
        cout << "Error: ROM file size is " << romSize << "B. Expected size is 256B.\n";
        exit(1);
    }
    arq.seekg(0, ios::beg);
    arq.read(reinterpret_cast<char*>(ROM), sizeof(ROM));
    if(arq.gcount() != sizeof(ROM)) {
        cout << "Error reading: " << romfile << "\n";
        exit(1);
    }
    arq.close();
    cout << "ROM loaded.\n";

    u_int8_t bus;
    u_int8_t clockCount;
    u_int8_t lastRo = ro;
    bool clock;
    bool lastClock;
    bool rst;
    bool hlt;
    while(!hlt) {
        if(rst) { clock=0;clockCount=0; }
        regs(&bus, bus, clock, rst);
        alu(&bus, clock, rst);
        programCounter(&bus, bus, lastClock, clock, rst);
        mem(&bus, bus, clock, rst);
        rout(bus, clock, rst);
        lu(&bus, bus, lastClock, clock, rst);
        lastClock = clock;
        if(clockCount == 3) {
            clockCount = 0;
            clock = !clock;
        } else {
            clockCount = clockCount+1;
        }
        if(ro != lastRo) {
            cout << "out: " << static_cast<int>(ro) << "\n";
            lastRo = ro;
        }
        rst = (controlBus & 0b1000000000000000000000) >> 21;
        hlt = (controlBus & 0b100000000000000000000) >> 20;
    }
    cout << "halt!\n";
    return 0;
}

void alu(u_int8_t *buso, bool clock, bool rst) {
    u_int8_t opr = (controlBus & 0b11100000000) >> 8;
    bool ao = (controlBus & 0b10000000) >> 7;
    if(clock) {
        flg = ((ra>rb)<<2) + ((ra==rb)<<1) + (ra<rb);
    }
    if(ao) {
        if(opr == 0x00) *buso = ra+rb;
        if(opr == 0x01) *buso = ra-rb;
        if(opr == 0x02) *buso = ra*rb;
        if(opr == 0x03) *buso = (rb != 0) ? (ra / rb) : 0;
    }
    if(rst) flg = 0;
}

void programCounter(u_int8_t *buso, u_int8_t busi, bool lastClock, bool clock, bool rst) {
    bool ce = (controlBus & 0b10000000000000000000) >> 19;
    bool co = (controlBus & 0b1000000000000000000) >> 18;
    bool ci = (controlBus & 0b100000000000000000) >> 17;
    if(co) *buso = pc;
    if(ci && clock) pc = busi;
    if(ce && clock && !lastClock) pc = (pc + 1) % 256;
    if(rst) pc = 0;
}

void regs(u_int8_t *buso, u_int8_t busi, bool clock, bool rst) {
    u_int8_t ri = (controlBus & 0b11100000000000000) >> 14;
    u_int8_t ro = (controlBus & 0b11100000000000) >> 11;
    if((ri & 0b00000100) && clock) ra = busi;
    if((ri & 0b00000010) && clock) rb = busi;
    if((ri & 0b00000001) && clock) rc = busi;
    if(ro & 0b00000100) *buso = ra;
    if(ro & 0b00000010) *buso = rb;
    if(ro & 0b00000001) *buso = rc;
    if(rst) { ra=0; rb=0; rc=0; }
}

void mem(u_int8_t *buso, u_int8_t busi, bool clock, bool rst) {
    bool mai = (controlBus & 0b1000) >> 3;
    bool mi = (controlBus & 0b100) >> 2;
    bool mo = (controlBus & 0b10) >> 1;
    if(mo) *buso = ROM[addr];
    if(mi && clock) ROM[addr] = busi;
    if(mai && clock) addr = busi;
    if(rst) addr=0;
}

void rout(u_int8_t busi, bool clock, bool rst) {
    bool oe = (controlBus & 0b1);
    if(oe && clock) ro = busi;
    if(rst) ro = 0;
}

void lu(u_int8_t *buso, u_int8_t busi, bool lastClock, bool clock, bool rst) {
    bool ari = (controlBus & 0b100000) >> 5;
    bool aro = (controlBus & 0b10000) >> 4;
    bool iri = (controlBus & 0b1000000) >> 6;
    if(!clock && lastClock) lc = (lc + 1) % 8;
    if(aro) *buso = ar;
    if(ari && clock) ar = busi;
    if(iri && clock) ir = busi;

    u_int16_t pla_in = (ir << 6) + (flg << 3) + lc;
    //                               PLA
    //            condition                          control
    //            I     F                     RH   RRRRRR A  IAAM   
    //            N     L  L                  SLCCCABCABC O ARRRAMMO
    //            S     G  C                  TTEOLIIIOOO P OIIOIIOE
    //         ********   ***                 **   ***   *** ***   *
    switch(pla_in & 0b11111111000111) {
        case 0b00000000000010: controlBus = 0b0000000000000000000000; break; // NOP
        case 0b00000001000010: controlBus = 0b0000000100000010000000; break; // ADD
        case 0b00000010000010: controlBus = 0b0000000100000110000000; break; // SUB
        case 0b00000011000010: controlBus = 0b0000000100001010000000; break; // MUL
        case 0b00000100000010: controlBus = 0b0000000100001110000000; break; // DIV
        case 0b00000101000010: controlBus = 0b0001000000000000001000; break; // LDA
        case 0b00000101000011: controlBus = 0b0010000000000000100010; break; //
        case 0b00000101000100: controlBus = 0b0000000000000000011000; break; //
        case 0b00000101000101: controlBus = 0b0000010000000000000010; break; //
        case 0b00000110000010: controlBus = 0b0001000000000000001000; break; // LDB
        case 0b00000110000011: controlBus = 0b0010000000000000100010; break; //
        case 0b00000110000100: controlBus = 0b0000000000000000011000; break; //
        case 0b00000110000101: controlBus = 0b0000001000000000000010; break; //
        case 0b00000111000010: controlBus = 0b0001000000000000001000; break; // LDC
        case 0b00000111000011: controlBus = 0b0010000000000000100010; break; //
        case 0b00000111000100: controlBus = 0b0000000000000000011000; break; //
        case 0b00000111000101: controlBus = 0b0000000100000000000010; break; //
        case 0b00001000000010: controlBus = 0b0001000000000000001000; break; // SVA
        case 0b00001000000011: controlBus = 0b0010000000000000100010; break; //
        case 0b00001000000100: controlBus = 0b0000000000000000011000; break; //
        case 0b00001000000101: controlBus = 0b0000000010000000000100; break; //
        case 0b00001001000010: controlBus = 0b0001000000000000001000; break; // SVB
        case 0b00001001000011: controlBus = 0b0010000000000000100010; break; //
        case 0b00001001000100: controlBus = 0b0000000000000000011000; break; //
        case 0b00001001000101: controlBus = 0b0000000001000000000100; break; //
        case 0b00001010000010: controlBus = 0b0001000000000000001000; break; // SVC
        case 0b00001010000011: controlBus = 0b0010000000000000100010; break; //
        case 0b00001010000100: controlBus = 0b0000000000000000011000; break; //
        case 0b00001010000101: controlBus = 0b0000000000100000000100; break; //
        case 0b00001011000010: controlBus = 0b0000001010000000000000; break; // MAB
        case 0b00001100000010: controlBus = 0b0000000110000000000000; break; // MAC
        case 0b00001101000010: controlBus = 0b0000010001000000000000; break; // MBA
        case 0b00001110000010: controlBus = 0b0000000101000000000000; break; // MBC
        case 0b00001111000010: controlBus = 0b0000010000100000000000; break; // MCA
        case 0b00010000000010: controlBus = 0b0000001000100000000000; break; // MCB
        case 0b00010001000010: controlBus = 0b0001000000000000001000; break; // JMP
        case 0b00010001000011: controlBus = 0b0000100000000000000010; break; //
        case 0b00010010000010: controlBus = 0b0000000000010000000000; break; // CMP
        case 0b00010111000010: controlBus = 0b0001000000000000001000; break; // OUT
        case 0b00010111000011: controlBus = 0b0010000000000000100010; break; //
        case 0b00010111000100: controlBus = 0b0000000000000000011000; break; //
        case 0b00010111000101: controlBus = 0b0000000000000000000011; break; //
        case 0b11111110000010: controlBus = 0b1000000000000000000000; break; // RST
        case 0b11111111000010: controlBus = 0b0100000000000000000000; break; // HLT
    default:
        if     ((pla_in & 0b00000000000111) == 0b00000000000000) controlBus = 0b0001000000000000001000; // Base
        else if((pla_in & 0b00000000000111) == 0b00000000000001) controlBus = 0b0010000000000001000010; //
        else if((pla_in & 0b11111111111111) == 0b00010011010010) controlBus = 0b0001000000000000001000; // JEQ
        else if((pla_in & 0b11111111111111) == 0b00010011010011) controlBus = 0b0000100000000000000010; //
        else if((pla_in & 0b11111111010111) == 0b00010100000010) controlBus = 0b0001000000000000001000; // JNE
        else if((pla_in & 0b11111111010111) == 0b00010100000011) controlBus = 0b0000100000000000000010; //
        else if((pla_in & 0b11111111111111) == 0b00010101100010) controlBus = 0b0001000000000000001000; // JBT
        else if((pla_in & 0b11111111111111) == 0b00010101100011) controlBus = 0b0000100000000000000010; //
        else if((pla_in & 0b11111111111111) == 0b00010110001010) controlBus = 0b0001000000000000001000; // JLT
        else if((pla_in & 0b11111111111111) == 0b00010110001011) controlBus = 0b0000100000000000000010; //
        break;
    }

    if(rst) { lc=0; ar=0; ir=0; }
}