# HKcpu

The HKcpu is an 8-bit educational CPU designed for academic and learning purposes. Its goal is to help users understand how a real CPU works through interactive simulation and visualization. If you want to try it now, go to _[run it](#run-it)_ part.

## Architecture

It features:

- **3 General Purpose Registers (GPRs):** `RA`, `RB`, `RC`

  Used for arithmetic, logic, and general data manipulation.

- **6 Special Purpose Registers (SPRs):** `IR`, `AR`, `MA`, `PC`, `FG`, `RO`

  - `IR` – **Instruction Register:** Holds the current instruction being executed.
  - `AR` – **Argument Register:** Holds the current argument of the instruction.
  - `MA` – **Memory Address Register:** Interface for memory read/write addresses.
  - `PC` – **Program Counter:** Points to the next instruction to be executed.
  - `FG` – **Flags Register:** Stores comparisons flags (==, <, > and !=).
  - `RO` - **Register Output:** Holds the output value to be displayed on the screen or interface.

- **8-bit Memory Address Width (256 bytes addressable)**

- **22 MICs (Micro Instructions Code): follow MICs**

  - **Time Control:**

    - `RST` - **Reset:** Reset the CPU registers and states.
    - `HLT` - **Halt:** Stop the clock to end execution.

  - **Program Counter:**

    - `CE` - **Counter Enable:** Enable counter to increment on the next clock.
    - `CO` - **Counter Output:** Put the counter value on data bus.
    - `CL` - **Counter Load:** Loads the data bus value to counter value on the next clock.

  - **Registers:**

    - `RAI` - **RA Input:** Loads the data bus value to RA register.
    - `RBI` - **RB Input:** Loads the data bus value to RB register.
    - `RCI` - **RC Input:** Loads the data bus value to RC register.
    - `RAO` - **RA Output:** Puts the RA value on data bus.
    - `RBO` - **RB Output:** Puts the RB value on data bus.
    - `RCO` - **RC Output:** Puts the RC value on data bus.

  - **ALU:**

    - `AOP1-AOP3` - **ALU Operation (3bits)** Defines which ALU operation will be used.
    - `AO` - **ALU Output:** Puts the ALU result on data bus.

  - **Logic Control:**

    - `IRI` - **IR Input:** Loads the data bus value to IR register.
    - `ARO` - **AR Output:** Puts the ARO value on data bus.
    - `ARI` - **AR Input:** Loads the data bus value to AR register.

  - **Memory:**

    - `MAI` - **MA Input:** Loads the data bus value to MA interface register.
    - `MI` - **Memory Input:** Loads the data bus value to the specified address of memory.
    - `MO` - **Memory Output:** Puts the specified addres of memory value on data bus.
    - `OE` - **Output Enable:** Loads the data bus value to OE interface register.

- **1 Microcode PLA (Programmable Logic Array) used to store Micro Instructions (`microcode.txt` contains the Logisim PLA table).**.

- **1 ALU supporting 4 arithmetic and 4 comparison operations:** `ADD`, `SUB`, `MUL`, `DIV`, `==`, `!=`, `>`, `<`

- **25 Instruction Codes Supported: Follow Instructions**

  - `0x00` `NOP` - **No Operation:** Don't execute any operation on clock.
  - `0x01` `ADD` - **Addition ALU Operation:** RA + RB => RC
  - `0x02` `SUB` - **Subtraction ALU Operation:** RA - RB => RC
  - `0x03` `MUL` - **Multiplication ALU Operation** RA * RB => RC
  - `0x04` `DIV` - **Division ALU Operation:** RA / RB => RC
  - `0x05` `LDA ADDR` - **Load A:** Load the value of memory at the address specified by the argument, and put on RA.
  - `0x06` `LDB ADDR` - **Load B:** Load the value of memory at the address specified by the argument, and put on RB.
  - `0x07` `LDC ADDR` - **Load C:** Load the value of memory at the address specified by the argument, and put on RC.
  - `0x08` `SVA ADDR` - **Save A:** Save RA value on memory at the address specified by the argument.
  - `0x09` `SVB ADDR` - **Save B:** Save RB value on memory at the address specified by the argument.
  - `0x0A` `SVC ADDR` - **Save C:** Save RC value on memory at the address specified by the argument.
  - `0x0B` `MAB` - **Move A to B:** Copy the value of RA to RB.
  - `0x0C` `MAC` - **Move A to C:** Copy the value of RA to RC.
  - `0x0D` `MBA` - **Move B to A:** Copy the value of RB to RA.
  - `0x0E` `MBC` - **Move B to C:** Copy the value of RB to RC.
  - `0x0F` `MCA` - **Move C to A:** Copy the value of RC to RA.
  - `0x10` `MCB` - **Move C to B:** Copy the value of RC to RB.
  - `0x11` `JMP ADDR` - **Jump:** Jump PC to argument value.
  - `0x12` `CMP` - **Compare:** Compare RA and RB and set FG.
  - `0x13` `JEQ ADDR` - **Jump If Equal:** Using FG, jump to argument value if RA and RB were equal in the last CMP.
  - `0x14` `JNE ADDR` - **Jump If Not Equal:** Using FG, jump to argument value if RA and RB were not equal in the last CMP.
  - `0x15` `JBT ADDR` - **Jump If Bigger Than:** Using FG, jump to argument value if RA were bigger than RB in the last CMP.
  - `0x16` `JLT ADDR` - **Jump If Less Than:** Using FG, jump to argument value if RA were less than RB in the last CMP.
  - `0x17` `OUT ADDR` - **Output:** Puts the value of memory at the address specified by the argument to RO.
  - `0xFE` `RST` - **Reset:** Execute RST MC.
  - `0xFF` `HLT` - **Halt:** Execute HLT MC.

## Run It

You can run the CPU simulation using **Logisim** for visual, step-by-step execution, or use the **emulator** for faster results. If you want to write more complex programs, you can also use the **assembler**.

### Assembler and Emulator

To compile the assembler and emulator, run the following scripts:

- `assembler/build.sh`
- `emulator/build.sh`

Make sure the `bin` folders exist in both directories before running the build scripts.

Once compiled, you can run the Assembler and Emulator directly from their respective `bin` folders. To view usage instructions, simply execute them without any arguments.

If you want example programs to study or test without writing your own assembly, check the `example` directory.

### Logisim Simulation

To explore the internal architecture and step-by-step execution, open the file `hkcpu.circ` using [Logisim Evolution 1.9.0](https://github.com/logisim-evolution/logisim-evolution/releases/tag/v3.9.0).

To use the CPU in Logisim:

- Manually program the ROM with instructions.

- Reset and start the CPU clock.

- For more advanced programs, you can load precompiled HEX or BIN files into the ROM.
