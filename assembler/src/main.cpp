#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <set>


using namespace std;

u_int8_t ROM[256];

int main(int argc, char *argv[]) {
    if(argc <= 2) {
        cout << "add a file do exec, exemple:\n./assembler ./code.asm ./memory.rom\n";
        exit(1);
    }
    string srcfile = argv[1];
    string binfile = argv[2];
    ifstream arq(srcfile);
    if(!arq) {
        cout << "can't open: " << srcfile << "\n";
        exit(1);
    }
    unordered_map<string, u_int8_t> noAddrOpcodeMap = {
        {"NOP", 0x00},
        {"ADD", 0x01},
        {"SUB", 0x02},
        {"MUL", 0x03},
        {"DIV", 0x04},
        {"MAB", 0x0B},
        {"MAC", 0x0C},
        {"MBA", 0x0D},
        {"MBC", 0x0E},
        {"MCA", 0x0F},
        {"MCB", 0x10},
        {"CMP", 0x12},
        {"RST", 0xFE},
        {"HLT", 0xFF}
    };
    unordered_map<string, u_int8_t> addrOpcodeMap = {
        {"LDA", 0x05},
        {"LDB", 0x06},
        {"LDC", 0x07},
        {"SVA", 0x08},
        {"SVB", 0x09},
        {"SVC", 0x0A},
        {"JMP", 0x11},
        {"JEQ", 0x13},
        {"JNE", 0x14},
        {"JBT", 0x15},
        {"JLT", 0x16},
        {"OUT", 0x17},
    };
    string line;
    string lastLine;
    bool text = 1;
    int textsize = 0;
    for(int i = 0; getline(arq, line); ++i, lastLine = line) {
        if(i > 255) {
            cout << "your code passes 256 Bytes\n";
            exit(1);
        }
        transform(line.begin(), line.end(), line.begin(),
                [](unsigned char c) { return toupper(c); });
        if(text) {
            string command;
            try {
                command = line.substr(0, 3);
            } catch (const exception& e) {
                cout << "invalid command: \"" << line << "\" in line " << i+1 << "\n";
            }
            auto naoc = noAddrOpcodeMap.find(command);
            if(naoc != noAddrOpcodeMap.end()) {
                u_int8_t opcode = naoc->second;
                ROM[i] = opcode;
                continue;
            }
            auto aoc = addrOpcodeMap.find(command);
            if(aoc != addrOpcodeMap.end()) {
                u_int8_t opcode = aoc->second;
                ROM[i] = opcode;
                try {
                    string addr = line.substr(4, 2);
                    ROM[i+1] = stoi(addr, nullptr, 16);
                    i++;
                } catch (const exception& e) {
                    cout << "TEXT: invalid address: \"" << line << "\" in line " << i+1 << "\n";
                    exit(1);
                }
                continue;
            }
            if(line == ".DATA") {
                text = 0;
                if(lastLine.substr(0, 3) != "HLT") {
                    cout << "HLT not found, auto add HLT\n";
                    ROM[i] = 0xFF;
                }
                textsize = i;
                continue;
            }
            else {
                cout << "TEXT: instruction not found: \"" << line << "\" in line " << i+1 << "\n";
                exit(1);
            }
        }

        try {
            u_int8_t addr = stoi(line.substr(0, 2), nullptr, 16);
            u_int8_t value = stoi(line.substr(3, 2), nullptr, 16);
            ROM[addr] = value;
        } catch (const exception& e) {
            cout << "DATA: invalid value or address: \"" << line << "\" in line " << i+1 << "\n";
            exit(1);
        }
    }
    arq.close();
    cout << "writing " << textsize << "B of TEXT code.\n";
    ofstream outFile(binfile, ios::out | ios::binary);
    if (!outFile) {
        cout << "can't open: " << binfile << " for writing\n";
        exit(1);
    }
    outFile.write(reinterpret_cast<char*>(ROM), sizeof(ROM));
    outFile.close();
    cout << "Well done.\n";
    return 0;
}