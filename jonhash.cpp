#include <iostream>
#include <string>
#include <random>

#define BITS 6
#define CHARACTERS "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz ." //Must be 2^BITS characters (64)
#define BLOCK_SIZE 32
#define H0 "jonathanmarsh123jonathanmarsh123" //The initial hash used for the first XOR (BLOCK_SIZE characters)
std::default_random_engine random; //rand() is now deprecated, use this instead
std::uniform_int_distribution<int> randomCharacter(0, (int)pow(2.0, BITS) - 1); //new random function
/*Comapres 2 strings of binary and XORs them*/
std::string XOR(std::string string1, std::string string2) {
	std::string XORed = "";
	for (size_t index = 0; index < string1.size(); index++)
		XORed += (string1[index] != string2[index]) ? '1' : '0';
	return XORed;
}

/*Left shifts string once*/
void shift(std::string &input) {
	char temp = input[0];
	for (size_t index = 1; index <= input.size(); index++)
		input[index - 1] = input[index];
	input.back() = temp;
}

/*Converts decimal to 6 bit binary*/
std::string getBinary(size_t decimal) {
	std::string binary;
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
	for (size_t i = 0; i < shiftBy; i++) 
		shift(currentBinary);

	return convertFromBinary(XOR(currentBinary, previousBinary));
}

/*Appends a character then appends a different character to make the length a multiple of BLOCK_SIZE*/
void padAlign(std::string &input) {
	input += CHARACTERS[0];
	size_t padBy = 2 * BLOCK_SIZE - input.size() % BLOCK_SIZE;
	for (size_t i = 0; i < padBy; i++) 
		input += CHARACTERS[1];
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
	while (std::getline(std::cin, password)) {
		bool valid = true;
		for (char character : password) {
			bool found = false;
			for (char allowed : CHARACTERS)
				if (character == allowed) { //can only allow characters in the character list or conversions fail
					found = true;
					break;
				}
			if (found == false) {
				valid = false;
				std::cout << "Passwords may only contain the characters " << CHARACTERS << std::endl << "Password: ";
				break;
			}
		}
		if (valid) break;
	}
	return password;
}

int main() {
	while (1) {
		std::cout << "==Register==" << std::endl;
		std::string savedHash = hashPassword(getPassword());
		std::cout << "Password hashed to: " << savedHash << std::endl; //shows resulting hash

		std::cout << "==Login==" << std::endl;
		std::string hashed = hashPassword(getPassword(), savedHash.substr(0, BLOCK_SIZE));
		std::cout << "Password hashed to: " << hashed << std::endl; //shows attempted hash
		if (hashed == savedHash) std::cout << "Correct!" << std::endl;
		else std::cout << "Incorrect!" << std::endl;

		system("pause");
		system("cls");
	}
    return 0;
}