#pragma once

void loadRom(int argc, char *argv[]) {
    if(argc <= 1) {
        cout << "add input rom, exemple:\n./hkcpu-emulator ./memory.rom\n";
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
}

void saveRom(int argc, char *argv[]) {
    if(argc <= 2) {
        cout << "add output rom, exemple:\n./hkcpu-emulator ./input.rom ./output.rom\n";
        exit(1);
    }
    string romfile = argv[2];
    ofstream arq(romfile, ios::binary);
    if(!arq) {
        cout << "can't open or create: " << romfile << "\n";
        exit(1);
    }
    arq.write(reinterpret_cast<char*>(ROM), sizeof(ROM));
    if (!arq) {
        cout << "Error writing to: " << romfile << "\n";
        exit(1);
    }
    arq.close();
    cout << "ROM saved.\n";
}