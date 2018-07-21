#ifndef CHAR_BIT
#define CHAR_BIT 8 //The number of bits in a char type variable
#endif
#define COST 10 //The number of loops, multiplied by 1000
#define BASE64 "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/" //Characters used to represent numbers in base 64
#define BLOCK_SIZE 32 //The size of the salt and hash; result is always double this 
#define H0 "jonathanmarsh123jonathanmarsh123" //The initial hash used for the first XOR (BLOCK_SIZE characters)

#include <stdio.h> //TODO: remove

/*Compares 2 strings of binary and XORs them.
	output:	char XORed[BLOCK_SIZE * CHAR_BIT];
	input1:	char currentBinary[BLOCK_SIZE * CHAR_BIT];
	input2:	char shiftedBinary[BLOCK_SIZE * CHAR_BIT];
*/
void XOR(char* output, char* input1, char* input2) {
	for (unsigned int index = 0; index < BLOCK_SIZE * CHAR_BIT; index++)
		output[index] = (input1[index] != input2[index]) ? 1 : 0;
}

/*Left-shifts binary string `shift` times
	output:	char shifted[BLOCK_SIZE * CHAR_BIT];
	input:	char currentBinary[BLOCK_SIZE * CHAR_BIT];
	shift:	unsigned int shiftBy;
*/ 
void shift(char* output, char* input, unsigned int shift) {
	for (unsigned int index = 0; index < BLOCK_SIZE * CHAR_BIT; index++)
		if(index < shift)
			output[BLOCK_SIZE * CHAR_BIT - shift + index] = input[index];
		else
			output[index - shift] = input[index];
}

/*Converts a base64 string to concatenated binary string
	output:	char binary[BLOCK_SIZE * CHAR_BIT];
	input: 	char base64[BLOCK_SIZE];
*/
void convertBase64ToBinary(char* output, char* input) {
	unsigned int index = 0, quotient, base;
	for (; index < BLOCK_SIZE; index++){
		quotient = (unsigned int)input[index];
		base = CHAR_BIT;
		while(quotient != 0 || base > 0){
			output[index * CHAR_BIT + base] = (quotient != quotient / 2 * 2) ? 1 : 0;
			quotient /= 2;
			base--;
		}
	}
}

/*Converts concatenated binary string to a base64 string
	output:	char base64[BLOCK_SIZE];
	input: 	char binary[BLOCK_SIZE * CHAR_BIT];
*/
void convertBinaryToBase64(char* output, char* input) {
	unsigned int base = 2, 
				total = 0, 
				increment = 0,
				index = BLOCK_SIZE * CHAR_BIT - 1;
	for (; index >= 0; index--){
		total += input[index] * base;
		base *= 2;
		increment++;
		if(increment == CHAR_BIT){
			base = 2;
			output[index / CHAR_BIT] = BASE64[total]; 
			total = 0;
			increment = 0;
		}
	}
}

/*Hashes the current block
	-Uses previous hash to make it one way
	-Encrypts by shifting 
	-XORs the encrypted binary with the previous binary
*/
void hash(char* output, char* input) {
	char currentBinary[BLOCK_SIZE * CHAR_BIT] = {0},
		previousBinary[BLOCK_SIZE * CHAR_BIT] = {0},
		shifted[BLOCK_SIZE * CHAR_BIT] = {0},
		XORed[BLOCK_SIZE * CHAR_BIT] = {0};

	convertBase64ToBinary(currentBinary, input),
	convertBase64ToBinary(previousBinary, output);

	unsigned int shiftBy = 1, index = 0;
	for (; index < BLOCK_SIZE * CHAR_BIT; index++)
		if (previousBinary[index] == 1)
			shiftBy++;

	shift(shifted, currentBinary, shiftBy);

	XOR(XORed, currentBinary, previousBinary);
	convertBinaryToBase64(output, XORed);
}

unsigned int random(){
	return 1; /*TODO: CSPRNG*/
}

/*Creates a random salt using the new random function*/
void createSalt(char* output) {
	for (unsigned int index = 0; index < BLOCK_SIZE; index++)
		output[index] = BASE64[random()]; 
}

/*Creates the final hash by compressing the output of each stage*/
void hashPassword(char* output, char* input) {
	createSalt(output);
	char hashed[BLOCK_SIZE] = H0;
	unsigned int iteration = 0, index;
	for (; iteration < COST * 1000; iteration++){
		index = 0;
		while(input[index] != '\0'){
			char block[BLOCK_SIZE] = {0};
			
			hash(hashed, block);
		}
	}
	for(index = 0; index < BLOCK_SIZE; index++)
		output[BLOCK_SIZE + index] = hashed[index];
}

int testBaseConversions(){
    char input[BLOCK_SIZE] = "jonathanmarsh123jonathanmarsh123";
    char binary[BLOCK_SIZE * CHAR_BIT + 1];
    convertBase64ToBinary(binary, input);
	binary[BLOCK_SIZE * CHAR_BIT] = '\0';
    printf("Binary: %s\n", binary);
    
    char base64[BLOCK_SIZE + 1];
    convertBinaryToBase64(base64, binary);
	base64[BLOCK_SIZE] = '\0';
    printf("Base64: %s\n", base64);

	return base64 == input;
}


int main(int argc, char* argv[]){
	printf("Number of arguments: %d\n",argc);
	printf("Conversion test: %d\n",testBaseConversions());
	
	return 0;
}