/*
Author:     Bea Belle Therese B. Caños
            Lysander S. Uy
Professor:  Engr. Blasminda C. Mayol
Course:     CpE 3202 | Group 3
Activity:   Laboratory Exercise #3 | The Buses
*/

#include <stdio.h>
#include <conio.h>
#include <stdlib.h>

// Arithmetic Control Signals
#define ADD 0x1E
#define SUB 0x1D
#define MUL 0x1B

// Logical Control Signals
#define AND 0x1A
#define OR 0x19
#define NOT 0x18
#define XOR 0x17
#define SHL 0x16
#define SHR 0x15

// Data Movement Signals
#define WM 0x01
#define RM 0x02
#define RIO 0x04
#define WIO 0x05
#define WB 0x06
#define WIB 0x07
#define WACC 0x09
#define RACC 0x0B
#define SWAP 0x0E

// Program Control Signals
#define BR 0x03
#define BRE 0x14
#define BRNE 0x13
#define BRGT 0x12
#define BRLT 0x11
#define EOP 0x1F

// Memory Configurations
#define MEM_SIZE 2048
#define IO_BUF_SIZE 32

// Memory Arrays
unsigned char dataMemory[MEM_SIZE]; // Array representing main memory
unsigned char ioBuffer[IO_BUF_SIZE]; // Array representing I/O buffer

// Global Bus Declarations
unsigned int ADDR = 0x00; // Address bus
unsigned char BUS = 0x00; // Data bus

// Global Control Signals
unsigned char CONTROL = 0x00;
unsigned char IOM = 0x00; // I/O Memory control signal
unsigned char RW = 0x00; // Read/Write control signal
unsigned char OE = 0x00; // Output Enable signal

// Function Prototypes
int CU(void);
void initMemory(void);
void MainMemory(void);
void IOMemory(void);

int main()
{
    initMemory(); // Populate memory

    if (CU() == 1)
        printf("\nProgram run successfully!\n\n");
    else
        printf("\nError encountered, program terminated!\n\n");

    return 0;
}

// Control Unit function that simulates instruction execution
int CU(void)
{
    unsigned int    PC = 0x000, 
                    IR = 0x000, 
                    MAR = 0x000, 
                    IOAR = 0x000, 
                    operand = 0x000;
    unsigned char   MBR = 0, 
                    IOBR = 0, 
                    inst_code;
    int             Fetch, 
                    Memory,  
                    IO;

    printf("Initializing Main Memory...\n\n");
    printf("*********************************\n");
    while (1)
    {
        printf("PC\t\t\t: 0x%03X\n", PC);
        printf("Fetching instruction...\n");

        // External control signals                       
        CONTROL = inst_code; 
        IOM = 1; 
        RW = 0; 
        OE = 1; 

        // Local control signals
        Fetch = 0x01;
        Memory = 0x00;
        IO = 0x00;

        // Fetching upper byte
        ADDR = PC;
        IOM = 1;
        RW = 0;
        OE = 1;
        MainMemory();
        IR = (unsigned int)BUS << 8; 
        PC++;

        // Fetching lower byte
        ADDR = PC;
        MainMemory();
        IR |= BUS;
        PC++;

        printf("IR\t\t\t: 0x%04X\n", IR);

        // Decoding Instruction
        inst_code = (IR >> 11) & 0x1F;
        operand = IR & 0x07FF;

        printf("Instruction Code\t: 0x%02X\n", inst_code);
        printf("Operand\t\t\t: 0x%03X\n", operand);

        // Instruction Processing
        if (inst_code == WM) // WM - Write data in MBR to memory at address pointed to by MAR
        {
            Memory = 0x01;

            CONTROL = WM; 
            RW = 1;      
            IOM = 1;      
            OE = 1;

            //=================================
            printf("Instruction\t\t: WM");
            printf("\nWriting data to memory...\n");

            MAR = operand; 

            ADDR = MAR;
            BUS = MBR;

            MainMemory();
            //=================================
        }
        else if (inst_code == RM) // RM - Read data from memory with the specified address
        {
            Memory = 0x01;

            CONTROL = RM; 
            RW = 0;      
            IOM = 1;      
            OE = 1;

            //=================================
            printf("Instruction\t\t: RM");
            printf("\nReading data from memory...\n");

            MAR = operand;      
            ADDR = MAR;
            MainMemory();

            MBR = BUS;
            //=================================
        }
        else if (inst_code == BR) // BR - Branch to specified address
        {
            //=================================
            printf("Instruction\t\t: BR");
            printf("\nBranching to 0x%03X on next cycle...\n", operand);

            PC = operand;
            //=================================
        }
        else if (inst_code == RIO) // RIO - Read data from IO memory with the specified address
        {
            IO = 0x01;

            CONTROL = RIO; 
            RW = 0;       
            IOM = 0;      
            OE = 1;

            //=================================
            printf("Instruction\t\t: RIO");
            printf("\nReading data from IO buffer...\n");

            IOAR = operand; 

            ADDR = IOAR;
            IOMemory();

            IOBR = BUS;
            //=================================
        }
        else if (inst_code == WIO) // WIO - Write data in IOBR to memory at address pointed to by IOAR
        {
            IO = 0x01;

            CONTROL = WIO; 
            RW = 1;        
            IOM = 0;       
            OE = 1;

            //=================================
            printf("Instruction\t\t: WIO");
            printf("\nWriting to IO buffer...\n");

            IOAR = operand; 

            ADDR = IOAR;
            BUS = IOBR;

            IOMemory();
            //=================================
        }
        else if (inst_code == WB) // WB - Write literal value to MBR
        {
            CONTROL = WB;  
            RW = 1;       
            IOM = 1;       
            OE = 1;

            //=================================
            printf("Instruction\t\t: WB");
            printf("\nLoading data to MBR...\n");

            ADDR = PC -1;
            MainMemory();

            MBR = BUS;
            //=================================
        }
        else if (inst_code == WIB) // WIB - Write literal value to IOBR
        {
            IO = 0x01;

            CONTROL = WIB;  
            RW = 1;   
            IOM = 0;       
            OE = 1;

            //=================================
            printf("Instruction\t\t: WIB");
            printf("\nLoading data to IOBR...\n");

            ADDR = PC -1;
            IOBR = BUS;

            IOMemory();
            //=================================
        }
        else if (inst_code == EOP) // EOP - End of Program
        {
            //=================================
            printf("Instruction\t\t: EOP\n");
            printf("End of program.\n");
            printf("*********************************\n");

            return 1;
            //=================================
        }
        else
        {   
            //=================================
            printf("Unknown Instruction\n");

            return 0;
            //=================================
        }

        printf("CONTROL\t\t\t: 0x%02X\n", CONTROL);
        printf("PC\t\t\t: 0x%03X\n", PC);
        printf("ADDR\t\t\t: 0x%03X\n", ADDR);
        printf("MAR\t\t\t: 0x%03X\n", MAR);
        printf("MBR\t\t\t: 0x%02X\n", MBR);
        printf("BUS\t\t\t: 0x%02X\n", BUS);
        printf("IOAR\t\t\t: 0x%03X\n", IOAR);
        printf("IOBR\t\t\t: 0x%02X\n", IOBR);
        printf("*********************************\n");
    }
    return 1;
}

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
    if (IOM == 1)
    {
        if (RW == 0 && OE == 1)
            BUS = dataMemory[ADDR]; // memory read
        else if (RW == 1 && OE == 1)
            dataMemory[ADDR] = BUS; // memory write
    }
}

// Function to handle I/O memory read/write operations
void IOMemory(void)
{
    if (IOM == 0)
    {
        if (RW == 0 && OE == 1)
            BUS = ioBuffer[ADDR]; // I/O read
        else if (RW == 1 && OE == 1)
            ioBuffer[ADDR] = BUS; // I/O write
    }
}
