/*
Author:     Bea Belle Therese B. Caños
            Lysander S. Uy
Professor:  Engr. Blasminda C. Mayol
Course:     CpE 3202 | Group 3
Activity:   Laboratory Exercise #4 | The ALU v2.0
*/

#include <stdio.h>
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

// Global Flag
unsigned char FLAGS = 0x00;

// Function Prototypes
void initMemory();
void MainMemory();
void IOMemory();
int CU();
int ALU();

unsigned char twosComp(unsigned data);
unsigned char setFlags(unsigned int ACC);
void printBin(int data, unsigned char data_width);

unsigned char add(unsigned char operand1, unsigned char operand2);
unsigned char subtract(unsigned char operand1, unsigned char operand2);
unsigned int multiply(unsigned char operand1, unsigned char operand2);

unsigned char and (unsigned char operand1, unsigned char operand2);
unsigned char or (unsigned char operand1, unsigned char operand2);
unsigned char not(unsigned char operand);
unsigned char xor (unsigned char operand1, unsigned char operand2);
unsigned char shl(unsigned char operand);
unsigned char shr(unsigned char operand);

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
        else if (inst_code == WACC) // WACC - Write data on BUS to ACC
        {
            Memory = 0x01;

            CONTROL = WACC;
            IOM = 0;
            RW = 0;
            OE = 0;

            //=================================
            printf("Instruction\t\t: WACC");
            printf("\nWriting data on BUS to ACC...");

            BUS = MBR;

            ALU();
            //=================================
        }
        else if (inst_code == RACC) // RACC - Read ACC to BUS
        {
            Memory = 0x01;

            CONTROL = RACC;
            IOM = 0;
            RW = 0;
            OE = 0;

            //=================================
            printf("Instruction\t\t: RACC");
            printf("\nReading data on ACC to BUS...");

            ALU();

            MBR = BUS;
            //=================================
        }
        else if (inst_code == SWAP) // SWAP - Swap data of MBR and IOBR
        {
            IO = 0x01;

            CONTROL = SWAP;
            IOM = 0;
            RW = 0;
            OE = 0;

            //=================================
            printf("Instruction\t\t: SWAP");
            printf("\nSwapping data of MBR and IOBR...\n");

            unsigned char temp = MBR;
            MBR = IOBR;
            IOBR = temp;
            //=================================
        }

        else if (inst_code == BRLT) // BRLT - Compare ACC and BUS, if ACC < BUS, branch to address specified
        {
            CONTROL = BRLT;
            IOM = 0;
            RW = 0;
            OE = 0;

            //=================================
            printf("Instruction\t\t: BRLT");
            printf("\nComparing ACC and BUS...\n");

            ADDR = operand;
            BUS = MBR;

            ALU();

            if (FLAGS & 0x04) // If Sign Flag (SF) is 1, ACC < BUS
            { 
                printf("Branching to 0x%03X...\n", ADDR);
                PC = ADDR;
                OE = 1;
            }
            //=================================
        }
        else if (inst_code == BRGT) // BRGT - Compare ACC and BUS, if ACC > BUS, branch to address specified
        {
            CONTROL = BRGT;
            IOM = 0;
            RW = 0;
            OE = 0;

            //=================================
            printf("Instruction\t\t: BRGT");
            printf("\nComparing ACC and BUS...\n");

            ADDR = operand;
            BUS = MBR;

            ALU();

            if (!(FLAGS & 0x04)) // If Sign Flag (SF) is 0, ACC > BUS
            { 
                printf("Branching to 0x%03X...\n", ADDR);
                PC = ADDR;
                OE = 1;
            }
            //=================================
        }
        else if (inst_code == BRNE) // BRNE - Compare ACC and BUS, if not equal branch to address specified
        {
            CONTROL = BRNE;
            IOM = 0;
            RW = 0;
            OE = 0;

            //=================================
            printf("Instruction\t\t: BRNE");
            printf("\nComparing ACC and BUS...\n");

            ADDR = operand;
            BUS = MBR;

            ALU();

            if (!(FLAGS & 0x01)) // If Zero Flag (ZF) is 0, PC <- ADDR
            { 
                printf("Branching to 0x%03X...\n", ADDR);
                PC = ADDR;
                OE = 1;
            }
            //=================================
        }
        else if (inst_code == BRE) // BRE - Compare ACC and BUS, if equal branch to address specified
        {
            CONTROL = BRE;
            IOM = 0;
            RW = 0;
            OE = 0;

            //=================================
            printf("Instruction\t\t: BRE");
            printf("\nComparing ACC and BUS...\n");

            ADDR = operand;
            BUS = MBR;

            ALU();

            if (FLAGS & 0x01) // If Zero Flag (ZF) is set, PC <- ADDR
            { 
                printf("Branching to 0x%03X...\n", ADDR);
                PC = ADDR;
                OE = 1;
            }
            //=================================
        }
        else if (inst_code == SHR) // SHR - Shift the value of ACC 1 bit to the right
        {
            CONTROL = SHR;
            IOM = 0;
            RW = 0;
            OE = 0;

            //=================================
            printf("Instruction\t\t: SHR");

            BUS = MBR;

            ALU();
            //=================================
        }
        else if (inst_code == SHL) // SHL - Shift the value of ACC 1 bit to the left
        {
            CONTROL = SHL;
            IOM = 0;
            RW = 0;
            OE = 0;

            //=================================
            printf("Instruction\t\t: SHL");

            BUS = MBR;

            ALU();
            //=================================
        }
        else if (inst_code == XOR) // XOR - XOR the value of ACC and BUS
        {
            CONTROL = XOR;
            IOM = 0;
            RW = 0;
            OE = 0;

            //=================================
            printf("Instruction\t\t: XOR");

            BUS = MBR;

            ALU();
            //=================================
        }
        else if (inst_code == NOT) // NOT - Complement the value of ACC
        {
            CONTROL = NOT;
            IOM = 0;
            RW = 0;
            OE = 0;

            //=================================
            printf("Instruction\t\t: NOT");

            BUS = MBR;

            ALU();
            //=================================
        }
        else if (inst_code == OR) // OR - OR the value of ACC and BUS
        {
            CONTROL = OR;
            IOM = 0;
            RW = 0;
            OE = 0;

            //=================================
            printf("Instruction\t\t: OR");

            BUS = MBR;

            ALU();
            //=================================
        }
        else if (inst_code == AND) // AND - AND the value of ACC and BUS
        {
            CONTROL = AND;
            IOM = 0;
            RW = 0;
            OE = 0;

            //=================================
            printf("Instruction\t\t: AND");

            BUS = MBR;

            ALU();
            //=================================
        }
        else if (inst_code == MUL) // MUL - Multiply the value of ACC to BUS
        {
            CONTROL = MUL;
            IOM = 0;
            RW = 0;
            OE = 0;

            //=================================
            printf("Instruction\t\t: MUL");

            BUS = MBR; // Ensure BUS contains the second operand

            ALU();
            //=================================
        }
        else if (inst_code == SUB) // SUB - Subtract the data on the BUS from ACC
        {
            CONTROL = SUB;
            IOM = 0;
            RW = 0;
            OE = 0;

            //=================================
            printf("Instruction\t\t: SUB");

            BUS = MBR; // Ensure BUS contains the second operand

            ALU();
            //=================================
        }
        else if (inst_code == ADD) // ADD - Adds the data on the BUS to ACC
        {
            CONTROL = ADD;
            IOM = 0;
            RW = 0;
            OE = 0;

            //=================================
            printf("Instruction\t\t: ADD");

            BUS = MBR;

            ALU();
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

// Arithmetic Logic Unit function 
int ALU()
{
    static int ACC = 0x00; // Accumulator register

    switch (CONTROL)
    {
    case ADD:
        printf("\nFetching operands...");

        printf("\nOP1 = ");
        printBin(ACC, 8);
        printf("\nOP2 = ");
        printBin(BUS, 8);

        ACC = add(ACC, BUS);

        printf("\nPerforming Addition operation...");
        printf("\nACC = ");
        printBin(ACC, 16);
        printf("\n");
        break;

    case SUB:
        printf("\nFetching operands...");
        printf("\nOP1 = ");
        printBin(ACC, 8);
        printf("\nOP2 = ");
        printBin(BUS, 8);

        ACC = subtract(ACC, BUS);

        printf("\nPerforming Subtraction operation...");
        printf("\nACC = ");
        printBin(ACC, 16);
        printf("\n");

        break;

    case MUL:
        printf("\nFetching operands...");
        printf("\nOP1 = ");
        printBin(ACC, 8);
        printf("\nOP2 = ");
        printBin(BUS, 8);

        ACC = multiply(ACC, BUS);

        printf("\nPerforming Multiplication operation...");
        printf("\nACC = ");
        printBin(ACC, 16);
        printf("\n");

        break;

    case AND:
        printf("\nFetching operands...");
        printf("\nOP1 = ");
        printBin(ACC, 8);
        printf("\nOP2 = ");
        printBin(BUS, 8);

        ACC = and(ACC, BUS);

        printf("\nPerforming AND operation...");
        printf("\nACC = ");
        printBin(ACC, 8);
        printf("\n");

        break;

    case OR:
        printf("\nFetching operands...");
        printf("\nOP1 = ");
        printBin(ACC, 8);
        printf("\nOP2 = ");
        printBin(BUS, 8);

        ACC = or (ACC, BUS);
        
        printf("\nPerforming OR operation...");
        printf("\nACC = ");
        printBin(ACC, 8);
        printf("\n");

        break;

    case NOT:
        printf("\nFetching operand...");
        printf("\nACC = ");
        printBin(ACC, 8); 

        ACC = not(ACC); 
        
        printf("\nPerforming NOT operation...");
        printf("\nACC = ");
        printBin(ACC, 8);
        printf("\n");

        break;

    case XOR:
        printf("\nFetching operands...");
        printf("\nOP1 = ");
        printBin(ACC, 8);
        printf("\nOP2 = ");
        printBin(BUS, 8);

        ACC = xor(ACC, BUS);

        printf("\nPerforming XOR operation...");
        printf("\nACC = ");
        printBin(ACC, 8);
        printf("\n");

        break;

    case SHR:
        printf("\nFetching operand...");
        printf("\nACC = ");
        printBin(ACC, 8);

        printf("\nShifting ACC >> 1...");

        FLAGS = (FLAGS & ~0x02) | ((ACC & 0x01) << 1); // Update CF with LSB of ACC
        ACC = shr(ACC);                              

        printf("\nACC = ");
        printBin(ACC, 8);
        printf("\n");

        break;

    case SHL:
        printf("\nFetching operand...");
        printf("\nACC = ");
        printBin(ACC, 8);

        printf("\nShifting ACC << 1...");

        FLAGS = (FLAGS & ~0x02) | ((ACC & 0x80) >> 7); // Update CF with MSB of ACC
        ACC = shl(ACC);                                

        printf("\nACC = ");
        printBin(ACC, 8);
        printf("\n");

        break;

    case WACC:
        ACC = (unsigned char)BUS;

        printf("\nACC = ");
        printBin(ACC, 16);
        printf("\n");

        break;

    case RACC:
        BUS = ACC;

        printf("\nACC = ");
        printBin(ACC, 16);
        printf("\n");

        break;

    case BRLT:
        ACC = subtract(ACC, BUS) & 0xFF;
        printf("ACC = ");
        printBin(ACC, 16);
        printf("\n");

        break;

    case BRGT:
        ACC = subtract(ACC, BUS) & 0xFF;
        printf("ACC = ");
        printBin(ACC, 16);
        printf("\n");

        break;

    case BRNE:
        ACC = subtract(ACC, BUS) & 0xFF;
        printf("ACC = ");
        printBin(ACC, 16);
        printf("\n");

        break;
    case BRE:
        ACC = subtract(ACC, BUS) & 0xFF;
        printf("ACC = ");
        printBin(ACC, 16);
        printf("\n");

        break;

    default:
        printf("Invalid operation\n");
        exit(1);
    }

    if ((CONTROL == ADD) || (CONTROL == SUB) || (CONTROL == MUL) || (CONTROL == BRLT) || 
    (CONTROL == BRGT) || (CONTROL == BRNE) || (CONTROL == BRE)) 
    {
        setFlags(ACC);
    }
    printf("OF=%d, SF=%d, CF=%d, ZF=%d\n", (FLAGS >> 7) & 0x01, (FLAGS >> 2) & 0x01, (FLAGS >> 1) & 0x01, FLAGS & 0x01);
}

// Function to initialize memory with predefined values
void initMemory(void)
{
    dataMemory[0x000] = 0x30;
    dataMemory[0x001] = 0x15;
    dataMemory[0x002] = 0x0C;
    dataMemory[0x003] = 0x00;
    dataMemory[0x004] = 0x30;
    dataMemory[0x005] = 0x05;
    dataMemory[0x006] = 0x48;
    dataMemory[0x007] = 0x00;
    dataMemory[0x008] = 0x30;
    dataMemory[0x009] = 0x08;
    dataMemory[0x00A] = 0xF0;
    dataMemory[0x00B] = 0x00;
    dataMemory[0x00C] = 0x14;
    dataMemory[0x00D] = 0x00;
    dataMemory[0x00E] = 0xD8;
    dataMemory[0x00F] = 0x00;
    dataMemory[0x010] = 0x58;
    dataMemory[0x011] = 0x00;
    dataMemory[0x012] = 0x0C;
    dataMemory[0x013] = 0x01;
    dataMemory[0x014] = 0x38;
    dataMemory[0x015] = 0x0B;
    dataMemory[0x016] = 0x28;
    dataMemory[0x017] = 0x00;
    dataMemory[0x018] = 0x30;
    dataMemory[0x019] = 0x10;
    dataMemory[0x01A] = 0xE8;
    dataMemory[0x01B] = 0x00;
    dataMemory[0x01C] = 0x58;
    dataMemory[0x01D] = 0x00;
    dataMemory[0x01E] = 0x28;
    dataMemory[0x01F] = 0x01;
    dataMemory[0x020] = 0xB0;
    dataMemory[0x021] = 0x00;
    dataMemory[0x022] = 0xB0;
    dataMemory[0x023] = 0x00;
    dataMemory[0x024] = 0xA8;
    dataMemory[0x025] = 0x00;
    dataMemory[0x026] = 0x14;
    dataMemory[0x027] = 0x01;
    dataMemory[0x028] = 0xC8;
    dataMemory[0x029] = 0x00;
    dataMemory[0x02A] = 0xC0;
    dataMemory[0x02B] = 0x00;
    dataMemory[0x02C] = 0x20;
    dataMemory[0x02D] = 0x01;
    dataMemory[0x02E] = 0x70;
    dataMemory[0x02F] = 0x00;
    dataMemory[0x030] = 0xB8;
    dataMemory[0x031] = 0x00;
    dataMemory[0x032] = 0x30;
    dataMemory[0x033] = 0xFF;
    dataMemory[0x034] = 0xD0;
    dataMemory[0x035] = 0x00;
    dataMemory[0x036] = 0x14;
    dataMemory[0x037] = 0x01;
    dataMemory[0x038] = 0xA0;
    dataMemory[0x039] = 0x3C;
    dataMemory[0x03A] = 0x30;
    dataMemory[0x03B] = 0xF0;
    dataMemory[0x03C] = 0x90;
    dataMemory[0x03D] = 0x40;
    dataMemory[0x03E] = 0x88;
    dataMemory[0x03F] = 0x44;
    dataMemory[0x040] = 0x30;
    dataMemory[0x041] = 0x00;
    dataMemory[0x042] = 0x48;
    dataMemory[0x043] = 0x00;
    dataMemory[0x044] = 0x30;
    dataMemory[0x045] = 0x03;
    dataMemory[0x046] = 0x48;
    dataMemory[0x047] = 0x00;
    dataMemory[0x048] = 0x30;
    dataMemory[0x049] = 0x00;
    dataMemory[0x04A] = 0xA0;
    dataMemory[0x04B] = 0x52;
    dataMemory[0x04C] = 0x30;
    dataMemory[0x04D] = 0x01;
    dataMemory[0x04E] = 0xE8;
    dataMemory[0x04F] = 0x00;
    dataMemory[0x050] = 0x18;
    dataMemory[0x051] = 0x48;
    dataMemory[0x052] = 0xF8;
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

unsigned char twosComp(unsigned data)
{
    return (~data + 1);
}

unsigned char setFlags(unsigned int ACC) {
    FLAGS = 0; // Reset FLAGS before updating  

    // Zero Flag (ZF): Set if ACC is zero
    if (ACC == 0)
        FLAGS |= 0x01; // Set ZF (bit 0)
    
    // Carry Flag (CF): Set if ACC exceeds 8-bit limit (0xFF)
    if (ACC > 0xFF)
        FLAGS |= 0x02; // Set CF (bit 1)

    // Overflow Flag (OF): Set if the result exceeds 8-bit signed range (-128 to 127)
    if (ACC > 0x7F)
        FLAGS |= 0x80; // Set OF (bit 7)

    /* Not applicable for unsigned system
    // Sign Flag (SF) - Set if the most significant bit (bit 7) of ACC is 1 (negative)
    if (ACC & 0x80)  
        FLAGS |= 0x04; 
    */
}

void printBin(int data, unsigned char data_width)
{
    for (int i = data_width - 1; i >= 0; i--)
    {
        int bit = ((unsigned int)data >> i) & 1;
        printf("%d", bit);
    }
}

//========================================= Arithmetic functions =========================================

unsigned char add(unsigned char operand1, unsigned char operand2)
{
    return (operand1 + operand2);
}

unsigned char subtract(unsigned char operand1, unsigned char operand2)
{
    operand2 = twosComp(operand2);
    return (operand1 + operand2);
}

unsigned int multiply(unsigned char operand1, unsigned char operand2)
{
    unsigned short A = 0x00, Q = operand2, M = operand1;
    unsigned char Qsubn1 = 0x00, count = 8;

    while (count--)
    {
        if ((Q & 0x01) && !Qsubn1)
        {
            A = subtract(A, M);
        }
        else if (!(Q & 0x01) && Qsubn1)
        {
            A = add(A, M);
        }
        Qsubn1 = Q & 0x01;

        unsigned short combined = (A << 8) | Q;
        combined >>= 1; // Arithmetic shift right
        A = (combined >> 8);
        Q = (combined & 0xFF);
    }

    return ((A << 8) | Q);
}

//========================================= Logic functions =========================================

unsigned char and (unsigned char operand1, unsigned char operand2)
{
    return (operand1 & operand2);
}

unsigned char or (unsigned char operand1, unsigned char operand2)
{
    return (operand1 | operand2);
}

unsigned char not(unsigned char operand1)
{
    return ~(operand1);
}

unsigned char xor (unsigned char operand1, unsigned char operand2) {
    return (operand1 ^ operand2);
}

unsigned char shl(unsigned char operand)
{
    return (operand << 1);
}

unsigned char shr(unsigned char operand)
{
    return (operand >> 1);
}