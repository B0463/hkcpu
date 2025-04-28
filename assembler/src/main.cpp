#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <set>


using namespace std;

u_int8_t ROM[256];

int main(int argc, char *argv[]) {
    if(argc <= 2) { // see if the user pass the file
        cout << "add a file do exec, exemple:\n./assembler ./code.asm ./memory.rom\n";
        exit(1);
    }
    // get the file name
    string srcfile = argv[1];
    string binfile = argv[2];
    ifstream arq(srcfile); // open the src file
    if(!arq) { // error opening the file
        cout << "can't open: " << srcfile << "\n";
        exit(1);
    }
    // create a map to associate the opcodes without addres argument with the instruction
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
    // and the opcodes with addres argument with the instruction
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

    for(int i = 0; getline(arq, line); ++i, lastLine = line) { // read the src code line by line
        if(i > 255) { // chech the code size
            cout << "your code passes 256 Bytes\n";
            exit(1);
        }
        transform(line.begin(), line.end(), line.begin(),
                [](unsigned char c) { return toupper(c); }); 
        
        if(text) { // if the line is in the text section
            string command;
            try {
                command = line.substr(0, 3); // get the command on 3 first chars
            } catch (const exception& e) {
                cout << "invalid command: \"" << line << "\" in line " << i+1 << "\n";
            }
            // check if the command is a noAddrOpcode
            auto naoc = noAddrOpcodeMap.find(command);
            if(naoc != noAddrOpcodeMap.end()) {
                u_int8_t opcode = naoc->second; // get the opcode of the command
                ROM[i] = opcode;// get the opcode of the command
                continue; // go to the next line
            }
            // check if the command is a addrOpcode
            auto aoc = addrOpcodeMap.find(command);
            if(aoc != addrOpcodeMap.end()) {
                u_int8_t opcode = aoc->second; // get the opcode of the command
                ROM[i] = opcode;// get the opcode of the command
                try {
                    string addr = line.substr(4, 2); // try to get the address argument
                    ROM[i+1] = stoi(addr, nullptr, 16); // convert the address to int and write it in the next byte of ROM
                    i++; // increment the line counter to skip the next line
                } catch (const exception& e) {
                    cout << "TEXT: invalid address: \"" << line << "\" in line " << i+1 << "\n";
                    exit(1);
                }
                continue; // go to the next line
            }
            if(line == ".DATA") { // check if the line is the start of the data section
                text = 0; // set the text flag to 0
                if(lastLine.substr(0, 3) != "HLT") { // check if the last line is not HLT
                    cout << "HLT not found, auto add HLT\n";
                    ROM[i] = 0xFF; // add HLT to the end of the text section
                }
                cout << "DATA: start of data section\n";
                textsize = i; // save the size of the text section
                continue; // go to the next line
            }
            else { // if the command is not in the opcode map
                cout << "TEXT: instruction not found: \"" << line << "\" in line " << i+1 << "\n";
                exit(1);
            }
        }
        // if the line is in the data section
        try {
            u_int8_t addr = stoi(line.substr(0, 2), nullptr, 16); // get the address of the data
            u_int8_t value = stoi(line.substr(3, 2), nullptr, 16); // get the value of the data
            ROM[addr] = value; // set the data in the ROM
        } catch (const exception& e) {
            cout << "DATA: invalid value or address: \"" << line << "\" in line " << i+1 << "\n";
            exit(1);
        }
    }
    arq.close(); // close the src file
    cout << "writing " << textsize << "B of TEXT code.\n"; // show the size of the text section
    ofstream outFile(binfile, ios::out | ios::binary); // open the bin file
    if (!outFile) { // error opening the file
        cout << "can't open: " << binfile << " for writing\n";
        exit(1);
    }
    outFile.write(reinterpret_cast<char*>(ROM), sizeof(ROM)); // write the ROM to the bin file
    outFile.close(); // close the bin file
    cout << "Well done.\n";
    return 0;
}