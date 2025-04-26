#pragma once

void loadRom(int argc, char *argv[]) {
    if(argc <= 1) { // detect if the command hasn't some argument
        cout << "add input rom, exemple:\n./hkcpu-emulator ./memory.rom\n";
        exit(1);
    }
    cout << "hkcpu emulator - 8bits CPU\n";
    string romfile = argv[1]; // define romfile as fist argument
    ifstream arq(romfile, ios::binary | ios::ate); // open the file correponding to the first argument
    if(!arq) { // if has some error opening file
        cout << "can't open: " << romfile << "\n";
        exit(1);
    }
    streamsize romSize = arq.tellg(); // sets the rom size
    if(romSize != sizeof(ROM)) { // error if rom size is diferrent of the program ROM
        cout << "Error: ROM file size is " << romSize << "B. Expected size is 256B.\n";
        exit(1);
    }
    arq.seekg(0, ios::beg);
    arq.read(reinterpret_cast<char*>(ROM), sizeof(ROM)); // read the file of rom
    if(arq.gcount() != sizeof(ROM)) { // if the bytes count did read if diferrent of ROM size, then has some error
        cout << "Error reading: " << romfile << "\n";
        exit(1);
    }
    arq.close(); // close the file
    cout << "ROM loaded.\n";
}

void saveRom(int argc, char *argv[]) {
    if(argc <= 2) { // exit if don't has arguments
        cout << "add output rom, exemple:\n./hkcpu-emulator ./input.rom ./output.rom\n";
        exit(1);
    }
    string romfile = argv[2]; // get the output file name
    ofstream arq(romfile, ios::binary); // open the output file
    if(!arq) { // if has some error opening the file
        cout << "can't open or create: " << romfile << "\n";
        exit(1);
    }
    arq.write(reinterpret_cast<char*>(ROM), sizeof(ROM)); // write ROM on file
    if (!arq) { // see for errors while writing
        cout << "Error writing to: " << romfile << "\n";
        exit(1);
    }
    arq.close(); // close and return
    cout << "ROM saved.\n";
}