#include <stdio.h>

// Global Variables
unsigned char C = 0, Z = 0, OF = 0, SF = 0, ACC = 0;

// Function Prototypes
int ALU(unsigned char operand1, unsigned char operand2, unsigned char control_signals);

unsigned char twosComp(unsigned data);
void setFlags(unsigned int ACC);
void printBin(int data, unsigned char data_width);
void display(unsigned char operand1, unsigned char operand2, unsigned char control_signals);

unsigned int add(unsigned char operand1, unsigned char operand2);
unsigned int subtract(unsigned char operand1, unsigned char operand2);
unsigned int multiply(unsigned char operand1, unsigned char operand2);

unsigned int and (unsigned char operand1, unsigned char operand2);
unsigned int or (unsigned char operand1, unsigned char operand2);
unsigned int not(unsigned char operand);
unsigned int xor (unsigned char operand1, unsigned char operand2);
unsigned int shl(unsigned char operand);
unsigned int shr(unsigned char operand);

void main(void) {
    int result = ALU(0x02,0x05,0x03);
    display(0x02, 0x05, 0x03);
    printf("ACC = ");
    printBin(result, 16);
    setFlags(result);
}

int ALU(unsigned char operand1, unsigned char operand2, unsigned char control_signals)
{
    // 0x01 - Addition
    // 0x02 - Subtraction
    // 0x03 - Multiplication
    // 0x04 - AND
    // 0x05 - OR
    // 0x06 - NOT
    // 0x07 - XOR
    // 0x08 - Shift Right (logical)
    // 0x09 - Shift Left (logical)

    unsigned int result = 0x0000; // Use unsigned int for potential 16-bit results
    
    switch (control_signals)
    {
    case 0x01: // Addition
        result = add(operand1, operand2);
        break;

    case 0x02: // Subtraction
        result = subtract(operand1, operand2);
        break;

    case 0x03: // Multiplication (Booth's Algorithm)
        result = multiply(operand1, operand2);
        break;

    case 0x04: // AND
        result = and(operand1, operand2);
        break;

    case 0x05: // OR
        result = or(operand1, operand2);
        break;

    case 0x06: // NOT
        result = not(operand1);
        break;

    case 0x07: // XOR
        result = xor(operand1, operand2);
        break;

    case 0x08: // Shift right logical
        result = shr(operand1);
        break;

    case 0x09: // Shift left logical
        result = shl(operand1);
        break;

    default:
        printf("Invalid operation\n");
        return -1;
    }

    return result;
}

unsigned char twosComp(unsigned data) {
    return (~data + 1);
}

void setFlags(unsigned int ACC) {
    Z = (ACC == 0x0000) ? 1 : 0;

    SF = (ACC & 0x0080) ? 1 : 0;

    OF = (ACC > 0x007F) ? 1 : 0;

    C = (ACC > 0x00FF) ? 1 : 0;
}

void printBin(int data, unsigned char data_width) {
    for (int i = data_width - 1; i >= 0; i--)
    {
        int bit = (data >> i) & 1;
        printf("%d", bit);
    }
}

void display(unsigned char operand1, unsigned char operand2, unsigned char control_signals) {
    printf("********************");
    printf("\nFetching operands...");
    printf("\nOP1 = ");
    printBin(operand1, 8);
    printf("\nOP2 = ");
    printBin(operand2, 8);
    printf("\n");

    switch (control_signals)
    {
    case 0x01:
        printf("Operation = ADD\n");
        printf("Adding OP1 and OP2...\n");
        break;

    case 0x02:
        printf("Operation = SUB\n");
        printf("2's Complement of OP2 \nAdding OP1 & OP2...\n");
        break;

    case 0x03:
        printf("Operation = MUL (Booth's)\n");
        printf("Multiplying OP1 and OP2...\n");
        break;

    case 0x04:
        printf("Operation = AND\n");
        break;

    case 0x05:
        printf("Operation = OR\n");
        break;

    case 0x06:
        printf("Operation = NOT\n");
        break;

    case 0x07:
        printf("Operation = XOR\n");
        break;

    case 0x08:
        printf("Operation = SHR\n");
        break;

    case 0x09: 
        printf("Operation = SHL\n");
        break;

    default:
        
    }
}

// Arithmetic functions

unsigned int add(unsigned char operand1, unsigned char operand2) {
    return (operand1 + operand2);
}

unsigned int subtract(unsigned char operand1, unsigned char operand2) {
    return operand1 - operand2;
}

unsigned int multiply(unsigned char operand1, unsigned char operand2)
{
    unsigned char A = 0x00, Q = operand2, M = operand1, Qsubn1 = 0x00;
    unsigned char count = 8;
    
    while (count)
    {
        if (((Q & 0x01) == 1) && (Qsubn1 == 0))
        {
            A = subtract(A, M);
        }
        else if (((Q & 0x01) == 0) && (Qsubn1 == 1))
        {
            A = add(A, M);
        }

        Qsubn1 = Q & 0x01;   // Store LSB of Q
        unsigned char newA = (A & 0x80) ? (A >> 1) | 0x80 : (A >> 1); // Sign-extend A
        unsigned char newQ = (Q >> 1) | ((A & 0x01) << 7); // Shift Q with A's LSB
        
        A = newA;
        Q = newQ;

        count--;
    }

    return ((A << 8 ) | Q);
}

// Logic functions

unsigned int and (unsigned char operand1, unsigned char operand2) {
    return (operand1 & operand2);
}

unsigned int or (unsigned char operand1, unsigned char operand2) {
    return (operand1 | operand2);
}

unsigned int not(unsigned char operand1) {
    return (~operand1) & 0xFF;
}

unsigned int xor (unsigned char operand1, unsigned char operand2) {
    return (operand1 ^ operand2);
}

unsigned int shl(unsigned char operand) {
    return (operand << 1) & 0xFF;
}

unsigned int shr(unsigned char operand) {
    return (operand >> 1);
}