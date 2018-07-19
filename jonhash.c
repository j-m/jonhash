#ifndef CHAR_BIT
#define CHAR_BIT 8 //The number of bits in a char type variable
#endif
#define COST 10 //The number of loops, multiplied by 1000
#define BASE64 "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/" //Characters used to represent numbers in base 64
#define BLOCK_SIZE 32 //The size of the salt and hash; result is always double this 
#define H0 "jonathanmarsh123jonathanmarsh123" //The initial hash used for the first XOR (BLOCK_SIZE characters)

#include <stdio.h> //TODO: remove

/*Compares 2 strings of binary and XORs them.
	output:	char* binary[BLOCK_SIZE * CHAR_BIT];
	input1:	char* binary[BLOCK_SIZE * CHAR_BIT];
	input2:	char* binary[BLOCK_SIZE * CHAR_BIT];
*/
void XOR(char* output, char* input1, char* input2) {
	for (unsigned int index = 0; index < BLOCK_SIZE * CHAR_BIT; index++)
		output[index] = (input1[index] != input2[index]) ? '1' : '0';
}

/*Left-shifts binary string `shift` times
	output:	char* binary[BLOCK_SIZE * CHAR_BIT];
	input:	char* binary[BLOCK_SIZE * CHAR_BIT];
	shift:	unsigned int;
*/
void shift(char* output, char* input, unsigned int shift) {
	for (unsigned int index = 0; index < BLOCK_SIZE * CHAR_BIT; index++)
		if(index < shift)
			output[BLOCK_SIZE * CHAR_BIT - shift + index] = input[index];
		else
			output[index - shift] = input[index];
}

/*Converts a base64 string to concatenated binary string
	output:	char* binary[BLOCK_SIZE * CHAR_BIT];
	input: 	char* binary[BLOCK_SIZE];
*/
void convertBase64ToBinary(char* output, char* input) {
	for (unsigned int index = 0; index < BLOCK_SIZE; index++){
		unsigned int quotient = (unsigned int)input[index],
			base = CHAR_BIT;
		while(quotient != 0 || base > 0){
			output[index * CHAR_BIT + base] = (quotient != quotient / 2 * 2) ? '1' : '0';
			quotient /= 2;
			base--;
		}
	}
}

/*Converts concatenated binary string to a base64 string
	output:	char* binary[BLOCK_SIZE];
	input: 	char* binary[BLOCK_SIZE * CHAR_BIT];
*/
void convertBinaryToBase64(char* output, char* input) {
	unsigned int base = 2, total = 0, increment = 0;
	for (unsigned int index = BLOCK_SIZE * CHAR_BIT - 1; index >= 0; index--){
		total += input[index] * base;
		base *= 2;
		increment++;
		if(increment == CHAR_BIT){
			base = 2;
			output[index / CHAR_BIT] = total; 
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
char* hash(char* currentBlock, char* previousHash) {
	char* currentBinary = convertToBinary(currentBlock),
		  previousBinary = convertToBinary(previousHash);

	char shifted[BLOCK_SIZE * CHAR_BIT];
	size_t shiftBy = 1;
	for (char character : previousBinary)
		if (character == '1')
			shiftBy++;
	for (size_t i = 0; i < 1; i++) 
		shift(shifted, currentBinary, shiftBy);

	char XORed[BLOCK_SIZE * CHAR_BIT];
	XOR(XORed, currentBinary, previousBinary)
	return convertFromBinary(XORed);
}

/*Appends a 1 then appends 0s to make it a BLOCK_SIZE multiple*/
void padAlign(char* input) {
	input += '1';
	size_t padBy = 2 * BLOCK_SIZE - input.size() % BLOCK_SIZE;
	for (size_t i = 0; i < padBy; i++) 
		input += '0';
}

/*Alternates between the password and salt (weaves)*/
void weave(char* password, char* salt) {
	char* woven;
	for (size_t i = 0; i < password.size(); i++) {
		woven += password[i];
		if (i < salt.size()) 
			woven += salt[i]; //Add next salt character if there's any left
	}
	if (password.size() < salt.size()) //Will be left over salt, append it all
		woven += salt.substr(password.size());
	password = woven;
}

/*Creates a random salt using the new random function*/
char* createSalt() {
	std::string salt = "";
	for (size_t i = 0; i < BLOCK_SIZE; i++)
		salt.push_back(BASE64[randomCharacter(random)]);
	return salt;
}

/*Creates the final hash by compressing the output of each stage*/
char* hashPassword(char* password, char* salt = createSalt()) {
	weave(password, salt);
	padAlign(password);
	char* hashed = H0;
	for (size_t iterate = 0; iterate < COST * 1000; iterate++)
		for (size_t i = 0; i < password.size() / BLOCK_SIZE; i++) {
			char* result = hash(password.substr(i * BLOCK_SIZE, BLOCK_SIZE), hashed);
			hashed = result;
		}
	return salt + hashed;
}


int main(int argc, char* argv[]){
	printf("%d",argc);
	if(argc == 1)  hashPassword(getPassword());

	char* savedHash = hashPassword(getPassword());
	char* hashed = hashPassword(getPassword(), savedHash.substr(0, BLOCK_SIZE));
	if (hashed == savedHash) return 0;
	else return 1;
}