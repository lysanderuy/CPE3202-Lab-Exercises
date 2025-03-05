/*
Author:     Bea Belle Therese B. Caños
            Lysander S. Uy
Professor:  Engr. Blasminda C. Mayol
Course:     CpE 3202 | Group 3
Activity:   Laboratory Exercise #3 | The Buses
*/

#include <stdio.h>
#include <conio.h>

// Memory Configurations
#define MEM_SIZE 2048  // Define the size of the main memory
#define IO_BUF_SIZE 32 // Define the size of the I/O buffer

// Memory Arrays
unsigned char dataMemory[MEM_SIZE]; // Array representing main memory
unsigned char ioBuffer[IO_BUF_SIZE]; // Array representing I/O buffer

// Global Bus Declarations
unsigned short ADDR = 0;  // Address bus
unsigned char BUS = 0;     // Data bus
unsigned char CONTROL = 0; // Control signals

// Global Control Signals
unsigned char IOM = 0; // I/O Memory control signal
unsigned char RW = 0;  // Read/Write control signal
unsigned char OE = 0;  // Output Enable signal

// Function Prototypes
void initMemory(void);
void MainMemory(void);
void IOMemory(void);
int CU(void);

// Function to initialize memory with predefined values
void initMemory(void)
{
    dataMemory[0x000] = 0x30;
    dataMemory[0x001] = 0xFF;
    dataMemory[0x002] = 0x0C;
    dataMemory[0x003] = 0x00;
    dataMemory[0x004] = 0x14;
    dataMemory[0x005] = 0x00;
    dataMemory[0x006] = 0x19;
    dataMemory[0x007] = 0x2A;
    dataMemory[0x12A] = 0x38;
    dataMemory[0x12B] = 0x05;
    dataMemory[0x12C] = 0x28;
    dataMemory[0x12D] = 0x0A;
    dataMemory[0x12E] = 0xF8;
    dataMemory[0x12F] = 0x00;
}

// Function to handle main memory read/write operations
void MainMemory(void)
{
    if (IOM == 1 && OE == 1)
    {
        if (RW == 0)
        {
            BUS = dataMemory[ADDR]; // Read from memory
        }
        else if (RW == 1)
        {
            dataMemory[ADDR] = BUS; // Write to memory
        }
    }
}

// Function to handle I/O memory read/write operations
void IOMemory(void)
{
    if (IOM == 0 && OE == 1)
    {
        if (RW == 0)
        {
            BUS = ioBuffer[ADDR]; // Read from I/O buffer
        }
        else if (RW == 1)
        {
            ioBuffer[ADDR] = BUS; // Write to I/O buffer
        }
    }
}

// Control Unit function that simulates instruction execution
int CU(void)
{
    unsigned short PC = 0x000, IR = 0, MAR = 0, IOAR = 0, operand;
    unsigned char MBR = 0, IOBR = 0, inst_code;

    printf("Initializing Main Memory...\n\n");
    printf("****************************\n");
    while (1)
    {
        printf("PC              : 0x%03X\n", PC);
        printf("Fetching instruction...\n");

        // Fetching the instruction from memory
        CONTROL = 0x00;
        IOM = 1;  // Set to main memory
        RW = 0;   // Read mode
        OE = 1;   // Output enable

        ADDR = PC;
        MAR = PC; // Update MAR with current PC
        MainMemory();

        // Fetch first byte
        IR = BUS;
        IR = IR << 8;
        PC++;
        ADDR = PC;
        MAR = PC; // Update MAR with new PC
        MainMemory();

        // Fetch second byte
        IR |= BUS;
        PC++;

        printf("IR              : 0x%04X\n", IR);

        inst_code = IR >> 11;
        operand = IR & 0x07FF;

        printf("Instruction Code: 0x%02X\n", inst_code);
        printf("Operand         : 0x%03X\n", operand);

        // Instruction Processing
        if (inst_code == 0x01) // WM - Write Memory
        {
            printf("Instruction      : WM\n");
            printf("Writing data to memory...\n");

            CONTROL = 0x01; // Write memory signal
            RW = 1;         // Write mode
            IOM = 1;        // Main memory access
            ADDR = operand;
            MAR = operand;  // Update MAR
            BUS = operand;
            MainMemory();
        }
        else if (inst_code == 0x02) // RM - Read Memory
        {
            printf("Instruction     : RM\n");
            printf("Reading data from memory...\n");

            CONTROL = 0x02; // Read memory signal
            RW = 0;         // Read mode
            IOM = 1;        // Main memory access
            ADDR = operand;
            MAR = operand;  // Update MAR
            MainMemory();
            MBR = BUS;      // Store read value in MBR
        }
        else if (inst_code == 0x03) // BR - Branch
        {
            printf("Instruction     : BR\n");
            printf("Branching to 0x%03X on next cycle...\n", operand);
            PC = operand;
        }
        else if (inst_code == 0x04) // RIO - Read from IO Buffer
        {
            printf("Instruction     : RIO\n");
            printf("Reading data from IO buffer...\n");

            CONTROL = 0x04; // Read IO signal
            RW = 0;         // Read mode
            IOM = 0;        // I/O memory access
            ADDR = operand;
            IOAR = operand; // Update IOAR
            IOMemory();
            IOBR = BUS;     // Store read value in IOBR
        }
        else if (inst_code == 0x05) // WIO - Write to IO Buffer
        {
            printf("Instruction     : WIO\n");
            printf("Writing to IO buffer...\n");

            CONTROL = 0x05; // Write IO signal
            RW = 1;         // Write mode
            IOM = 0;        // I/O memory access
            ADDR = operand;
            IOAR = operand; // Update IOAR
            BUS = operand; 
            IOMemory();
        }
        else if (inst_code == 0x06) // WB - Write to MBR
        {
            printf("Instruction     : WB\n");
            printf("Loading data to MBR...\n");

            CONTROL = 0x06; // Write buffer signal
            MBR = operand;  // Store operand in MBR
        }
        else if (inst_code == 0x07) // WIB - Write to IOBR
        {
            printf("Instruction     : WIB\n");
            printf("Loading data to IOBR...\n");

            CONTROL = 0x07; // I/O buffer write signal
            IOBR = operand; // Store operand in IOBR
        }
        else if (inst_code == 0x1F) // EOP - End of Program
        {
            printf("Instruction     : EOP\n");
            printf("End of program.\n");
            return 1;
        }
        else
        {
            printf("Unknown Instruction\n");
            return 0;
        }

        printf("BUS             : 0x%02X\n", BUS);
        printf("ADDR            : 0x%03X\n", ADDR);
        printf("PC              : 0x%03X\n", PC);
        printf("MAR             : 0x%03X\n", MAR);
        printf("IOAR            : 0x%03X\n", IOAR);
        printf("IOBR            : 0x%02X\n", IOBR);
        printf("CONTROL         : 0x%02X\n", CONTROL);
        printf("**********************\n");
    }
    return 1;
}


int main(void)
{
    initMemory();
    if (CU() == 1)
        printf("\nProgram run successfully!\n");
    else
        printf("\nError encountered, program terminated!\n");
    return 0;
}
