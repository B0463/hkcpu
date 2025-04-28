#pragma once

class CPU {
    private:
        // define registers and control states of CPU
        // see the README for descriptions of variavle names.
        u_int8_t RAM[256];
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
        u_int8_t bus;
        bool clock;
        bool rst;

        u_int32_t lastControlBus;
        u_int8_t lastFlg;
        u_int8_t lastRa;
        u_int8_t lastRb;
        u_int8_t lastBus;
        bool lastClock;
        bool lastRst;

        void alu() {
            // get operations by controlbus segment
            u_int8_t opr = (controlBus & 0b11100000000) >> 8;
            bool ao = (controlBus & 0b10000000) >> 7;
            if(clock) {
                flg = ((ra>rb)<<2) + ((ra==rb)<<1) + (ra<rb); // on clock enable, set the flg register
            }
            if(ao) { // if output enable, put the result on bus
                if(opr == 0x00) bus = ra+rb;
                if(opr == 0x01) bus = ra-rb;
                if(opr == 0x02) bus = ra*rb;
                if(opr == 0x03) bus = (rb != 0) ? (ra / rb) : 0;
            }
            if(rst) flg = 0; // on reset enable, reset the flg register
        }

        void programCounter() {
            // get operations by controlbus segment
            bool ce = (controlBus & 0b10000000000000000000) >> 19;
            bool co = (controlBus & 0b1000000000000000000) >> 18;
            bool ci = (controlBus & 0b100000000000000000) >> 17;
            if(co) bus = pc; // put the pc value on bus if output enable
            if(ci && clock) pc = bus; // load bus if input enable
            if(ce && clock) pc = (pc + 1) % 256; // increment clock value if counter enable
            if(rst) pc = 0; // on reset enable, reset the pc register
        }

        void regs() {
            // get operations by controlbus segment
            u_int8_t ri = (controlBus & 0b11100000000000000) >> 14;
            u_int8_t ro = (controlBus & 0b11100000000000) >> 11;
            if((ri & 0b00000100) && clock) ra = bus; // load bus to ra register if rai and clock enable
            if((ri & 0b00000010) && clock) rb = bus; // load bus to rb register if rbi and clock enable
            if((ri & 0b00000001) && clock) rc = bus; // load bus to rc register if rci and clock enable
            if(ro & 0b00000100) bus = ra; // put the ra value on bus if rao enable
            if(ro & 0b00000010) bus = rb; // put the rb value on bus if rbo enable
            if(ro & 0b00000001) bus = rc; // put the rc value on bus if rco enable
            if(rst) { ra=0; rb=0; rc=0; } // on reset enable, reset GPRs
        }

        void mem() {
            // get operations by controlbus segment
            bool mai = (controlBus & 0b1000) >> 3;
            bool mi = (controlBus & 0b100) >> 2;
            bool mo = (controlBus & 0b10) >> 1;
            if(mo) bus = RAM[addr];
            if(mi && clock) RAM[addr] = bus;
            if(mai && clock) addr = bus;
            if(rst) addr=0;
        }

        void rout() {
            // get operations by controlbus segment
            bool oe = (controlBus & 0b1);
            if(oe && clock) {
                ro = bus;
            }
            if(rst) ro = 0;
        }
        void lu() {
            // get operations by controlbus segment
            bool ari = (controlBus & 0b100000) >> 5;
            bool aro = (controlBus & 0b10000) >> 4;
            bool iri = (controlBus & 0b1000000) >> 6;
            if(!clock && lastClock) lc = (lc + 1) % 8; // increment lc register on clock falling
            if(aro) bus = ar; // load bus value to ar register if aro enable
            if(ari && clock) ar = bus; // ...
            if(iri && clock) ir = bus; // ...

            u_int16_t pla_in = (ir << 6) + (flg << 3) + lc; //create a segment of PLA in

            switch(pla_in & 0b11111111000111) {
                //                PLA (No conditional instructions)                  //
                //    pla_in condition                  controls segments            //
                //        I     F                     RH   RRRRRR A  IAAM            //
                //        N     L  L                  SLCCCABCABC O ARRRAMMO         //
                //        S     G  C                  TTEOLIIIOOO P OIIOIIOE         //
                //     ********   ***                 **   ***   *** ***   *         //
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
                    //                             PLA (Conditional instructions)                                   //
                    //                 pla_in condition                                       controls segments     //
                    //                     I     F                                          RH   RRRRRR A  IAAM     //
                    //                     N     L  L                                       SLCCCABCABC O ARRRAMMO  //
                    //                     S     G  C                                       TTEOLIIIOOO P OIIOIIOE  //
                    //                  ********   ***                                      **   ***   *** ***   *  //
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

    public:
        CPU() { // class constructor, inicialize registers and states
            RAM[256] = 0;
            controlBus = 0;
            addr = 0;
            flg = 0;
            pc = 0;
            lc = 0;
            ra = 0;
            rb = 0;
            rc = 0;
            ar = 0;
            ir = 0;
            ro = 0;
            bus = 0;
            clock = 0;
            rst = 0;
        }
        void flipClock() {
            clock = !clock;
        }
        void flipRst() {
            rst = !rst;
        }
        void setRam(u_int8_t ROM[]) { // load some ROM to internal RAM
            for(int i=0;i<256;i++) {
                RAM[i] = ROM[i];
            }
        }
        void getRam(u_int8_t ROM[]) { // load internal RAM to some ROM
            for(int i=0;i<256;i++) {
                ROM[i] = RAM[i];
            }
        }
        int propagate() { // do the cpu loop
            u_int8_t diff = // see the diferences of actual control bus and last control bus
                ((controlBus != lastControlBus) << 6)+
                ((bus != lastBus) << 5)+
                ((flg != lastFlg) << 4)+
                ((ra != lastRa) << 3)+
                ((rb != lastRb) << 2)+
                ((clock != lastClock) << 1)+
                (rst != lastRst);

            // update last states
            lastControlBus = controlBus;
            lastBus = bus;
            lastFlg = flg;
            lastRa = ra;
            lastRb = rb;
            lastClock = clock;
            lastRst = rst;

            // see if the diference modify some cpu module
            if(diff & 0b1010011) lu();
            if(diff & 0b1001111) alu();
            if(diff & 0b1100011) {
                programCounter();
                regs();
                mem();
                rout();
            }

            bool hlt = (controlBus & 0b100000000000000000000) >> 20; // get hlt operation segment
            if(hlt) return 2; // return 2 if hlt enable
            return !diff; // return 1 if hasn't diference, and 0 if has diference
        }
};