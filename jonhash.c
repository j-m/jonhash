#include <stdlib.h>

#define COST 10000
#define CHARACTERS "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
#define BLOCK_SIZE 32
#define H0 "jonathanmarsh123jonathanmarsh123" //The initial hash used for the first XOR (BLOCK_SIZE characters)

/*Comapres 2 strings of binary and XORs them*/
char* XOR(char* string1, char* string2) {
	char* XORed;
	size_t index = 0;
	for (; string1[index] != '\0'; index++)
		XORed[index] = (string1[index] != string2[index]) ? '1' : '0';
	XORed[index] = '\0';
	return XORed;
}

/*Left shifts string once*/
void shift(char* input) {
	char temp = input[0];
	size_t index = 1;
	for (; input[index] != '\0'; index++)
		input[index - 1] = input[index];
	input[index] = temp;
}

/*Converts decimal to 6 bit binary*/
char* getBinary(size_t decimal) {
	char binary[CHAR_BIT] = "000000";
	while (decimal > 0) {
		binary = ((decimal % 2==1)?'1':'0') + binary;
		decimal /= 2;
	}
	binary = std::string(BITS - binary.size(), '0').append(binary);
	return binary;
}

/*Converts base64 string to binary*/
std::string convertToBinary(std::string raw64) {
	std::string value = "";
	for (char character: raw64)
		for (size_t decimal = 0; decimal < pow(2.0, BITS); decimal++)
			if (character == CHARACTERS[decimal])
				value += getBinary(decimal);
	return value;
}

/*Converts binary to decimal*/
size_t getDecimal(size_t binary) {
	size_t decimal = 0, base = 1;
	while (binary > 0){
		decimal += binary % 10 * base;
		base *= 2;
		binary /= 10;
	}
	return decimal;
}

/*Converts string of 6 bit binaries to a base64 string*/
std::string convertFromBinary(std::string binary) {
	std::string value = "";
	for (size_t index = 0; index < binary.size() / BITS; index++)
		value+= CHARACTERS[getDecimal(std::stoul(binary.substr(index * BITS, BITS)))];
	return value;
}

/*Hashes the current block
	Uses previous hash to make it one way
	Encrypts by shifting 
	XORs the encrypted binary with the previous binary
*/
std::string hash(std::string currentBlock, std::string previousHash) {
	std::string currentBinary = convertToBinary(currentBlock),
				previousBinary = convertToBinary(previousHash);

	size_t shiftBy = 1;
	for (char character : previousBinary)
		if (character == '1')
			shiftBy++;
	for (size_t i = 0; i < 1; i++) 
		shift(currentBinary);

	return convertFromBinary(XOR(currentBinary, previousBinary));
}

/*Appends a character then appends a different character to make the length a multiple of BLOCK_SIZE*/
void padAlign(std::string &input) {
	input += '1';
	size_t padBy = 2 * BLOCK_SIZE - input.size() % BLOCK_SIZE;
	for (size_t i = 0; i < padBy; i++) 
		input += '0';
}

/*Alternates between the password and salt (weaves)*/
void weave(std::string &password, std::string salt) {
	std::string woven = "";
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
std::string createSalt() {
	std::string salt = "";
	for (size_t i = 0; i < BLOCK_SIZE; i++)
		salt.push_back(CHARACTERS[randomCharacter(random)]);
	return salt;
}

/*Creates the final hash by compressing the output of each stage*/
std::string hashPassword(std::string password, std::string salt = createSalt()) {
	weave(password, salt);
	padAlign(password);
	std::string hashed = H0;
	for (size_t iterate = 0; iterate < COST; iterate++)
		for (size_t i = 0; i < password.size() / BLOCK_SIZE; i++) {
			std::string result = hash(password.substr(i * BLOCK_SIZE, BLOCK_SIZE), hashed);
			hashed = result;
		}
	return salt + hashed;
}

/*Gets input from the user*/
std::string getPassword() {
	std::cout << "Password: ";
	std::string password;
	std::getline(std::cin, password);
	return password;
}

int main(int argc, char* argv[]){
	if(argc == 1)  hashPassword(getPassword());

	std::string savedHash = hashPassword(getPassword());
	std::string hashed = hashPassword(getPassword(), savedHash.substr(0, BLOCK_SIZE));
	if (hashed == savedHash) return 0;
	else return 1;
}