//Program to encrypt and decrypt a wide variety of classical ciphers
//By Hussein Ahmad

#include<iostream>
#include <fstream>
#include<sstream>
#include<string>
#include<vector>
#include <algorithm>

using namespace std;

constexpr int num_of_letters = 26; //number of uppercase letters.
constexpr int rep_of_A= 65; //The integer representation of the smallest capital letter, 'A' in ASCII
string cleaned = "cleaned.txt"; //where cleaned text is stored. See clean_text for how text is cleaned.
vector<double> eng_letter_mfreq{ 8.167, 1.492 ,2.782,4.253,12.702,2.228,2.015,6.094,6.966,0.153,0.772,4.025,2.406,6.749,7.507,
								1.929,0.095,5.987,6.327,9.056,2.758,0.978,2.360,0.150,1.974,0.074 }; //26 positions for the 26 english letters
																									//hardcoded numbers are percentages for mono frequency for associated letter

enum class ciphers {//sorted alphabetically
	affine = 1, caesar, hill, vigenere //note if any new ciphers are added, function int_to_cipher must be modified to include that cipher as well
};



struct cipher_action {
	cipher_action(ciphers c, int a)
		:cipher{ c }, action{ a }{

	}

	ciphers cipher{ciphers::affine};
	int action; //Encryption or decryption
};



//Error handling. Note the following was borrowed from Strouroup's Programming: Principles and Practice using C++
struct Exit : runtime_error {
	Exit() : runtime_error("Exit") {}
};

// error() simply disguises throws:
inline void error(const string& s)
{
	throw runtime_error(s);
}

inline void error(const string& s, const string& s2)
{
	error(s + s2);
}

inline void error(const string& s, int i)
{
	ostringstream os;
	os << s << ": " << i;
	error(os.str());
}
//End of Error Handling

//Numerical
vector<double> get_freq(const string& inname) {//returns the frequency of english letters found in the plain text file given.
	//Note this function assumes text given has already been cleaned
	istringstream ist{ inname };
	if (!ist) error("Cannot open file to get freq of letters!");
	
	vector<double> count(26);
	for (char ch; ist.get(ch);) {
		++count[(int)ch - 65]; //each position of the vector is associated with the letter in natural alphabetical order. Check what letter you have converting to int and subtracting 65 to be in range 0-25
	}
	
	return count;
}

inline int mod(int a, int b) { int x = a % b; return x >= 0 ? x : x + 26; } //% in C++ rounds towards 0 and does not yield mathematical remainder for negatives. ie. -7%3 =-1 rather than 2



ciphers int_to_cipher(int x) {//simple check just in case int given is not associated with a cipher
	if (x< 1 || x>(int) ciphers::vigenere) error("Given int not associated with listed ciphers. \n");
	return ciphers(x);
}

void clean_text(const string& inname, const string& outname) {//convert all alphabetical characters to uppercase ignoring everything else including punctuation, output to chosen file name. Ie. A B! c?##D. becomes ABCD 

	ifstream ist{ inname };
	if (!ist) error("Error! Cannot open plain text file for reading!");

	ofstream ost{ outname, ios_base::trunc }; //file where cleaned text will be stored. Note anything in there will be deleted before writing to it

	for (char ch; ist.get(ch);) {

		if (isalpha(ch)) {
			ch = toupper(ch);
			ost << ch;
		}
		else {
			//if its not alphabetical do nothing and just eat the character.
		}
	}
}



//Caesar
void Caesar_enc(const string& inname, const string& outname, int& key) {
	//Assumes uncleaned text
	key %=26; // Do a quick conversion for the key in case its larger than 25 since a shift by, say, 26 characters is really no shift at all
	
	clean_text(inname, cleaned);

	ifstream ist{ cleaned };
	if (!ist) error("Error! Cannot open converted plain text file for reading!");
	ofstream ost{ outname, ios_base::trunc };

	for (char ch; ist.get(ch);) {
		ch = mod((ch-rep_of_A + key),num_of_letters) + rep_of_A;//convert to letter to within 0-25, shift by chosen amout then conversion back to the ASCII range of 65-90
		ost << ch;
	}

}

void Caesar_dec(const string& inname, const string& outname, int& key) {//decryption assuming key is given
	//Assumes uncleaned text
	key %= 26;
	clean_text(inname, cleaned);

	ifstream ist{ cleaned };
	if (!ist) error("Error! Cannot open converted plain text file for reading!");
	ofstream ost{ outname, ios_base::trunc };

	for (char ch; ist.get(ch);) {
		ch = mod((ch-rep_of_A - key),num_of_letters) + rep_of_A;
		ost << ch;
	}
}

void Caesar_dec(const string& inname, const string& outname) {//decryption assuming no key is given (ie attack)
	//Assumes uncleaned text
	//Note that the frequency of the letters (mono) are currently hardcoded and drawn from Wikipedia. See vector eng_letter_mfreq top.
	clean_text(inname, cleaned);
	ifstream ist{ cleaned };
	if (!ist) error("Error! Cannot open converted plain text file for reading!");
	
	vector<vector<double>> possible_keys(26);//will hold the generated freq of letters based upon each possible key from A-Z
	vector<double> differences(26);//will store the differences between the freq of each possible key vector and the vector of the actual freq

	for (int key = 0; key <= 25; ++key) {
		ist.clear();//reset reading position to begining
		ist.seekg(0, ios_base::beg);

		string s; //place new text after decryption from possible key here
		for (char ch; ist.get(ch);) {
			ch = mod((ch - rep_of_A - key), num_of_letters) + rep_of_A;
			s += ch;
		}

		possible_keys[key] = get_freq(s);
	}


	for (int i = 0; i < 26; ++i) {//calculate the distance between the vectors, ie. ||u-v|| = sqrt ( (u_1-v_2)^2 + (u_2-v_2)^2 +...+ (u_26-v_26)^2 ) where the euclidean norm is the one on R^26
		double sum = 0;
		for (int j = 0; j < 26; ++j) {//find (u_1-v_2)^2 + (u_2-v_2)^2 +...+ (u_26-v_26)^2
			sum += pow((eng_letter_mfreq[j] - possible_keys[i][j]), 2); 
		}
		differences[i] = sqrt(sum);
	}



	int closest = distance(differences.begin(), min_element(differences.begin(), differences.end())); //find the smallest number corresponding to the closest to the freq of english and return its position
	cout << "Best guess is: " << (char)(closest + 65) << "\n";
	Caesar_dec(inname, outname, closest); //key found, call Caesar decryption as usual
}
//End of Caesar






//User input functions
bool get_yesno() {//prompts user for a yes or no
	char ans;
	while (true) {//keep asking for input until correct format is entered
		cin.get(ans);
		if (isalpha(ans))
		{
			switch (tolower(ans))
			{
			case 'y':
				cin.ignore(numeric_limits<streamsize>::max(), '\n'); //clear buffer
				return true;
				break;
			case 'n':
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				return false;
				break;
			default:
				cout << "Sorry incorrect alphabetic character entered, please try again. \n";
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				break;
			}
		}

		else {
			cout << "Sorry non-alphabetical character entered, please try again. \n";
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
		}
	}
}

int get_num() {//takes number given by user. Used for key

	int num = -1; //set to -1 for while loop check below in case cin>> num fails
	cin >> num;

	while (!cin || num < 0) {//cin must be good() and num has a value that is reasonable ie. >0
							//keep asking until both are satisfied
		cin >> num;
		if (cin.bad()) error("Input to num is bad(). \n");
		else if (cin.fail()) {
			cout << "Improper input entered, try again. \n";
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n'); //clear buffer
			num = -1; //reset to failure case
		}
		else if (num <= 0) cout << "Invalid number entered, must be greater than 0. Try again. \n";
	}
	cin.ignore();
	return num;
}

inline string get_filename() {
	string inname;
	getline(cin, inname);
	return inname;
}
//End of user input functions



int main() {
	try {
		ciphers cipher;
		int action;
		constexpr int encrypt = 1;
		constexpr int decrypt = 2;

		cout << "Hello, which cipher would you like to use? Affine (1), Caesar (2), Hill (3), or Vigenere (4)? \n"; // number representation in case of repitition on first letter of future added cipher, say vernam for ex.
		cipher = int_to_cipher(get_num());

		cout << "Encryption (1) or Decryption (2)? \n";
		while (true) {//keep asking until proper input is entered.
			action = get_num();
			if (action == encrypt || action == decrypt) break;
			else cout << "Sorry not one of the two choices entered. Must be '1' for encryption and '2' for decryption. \n";
		}
		
		cipher_action request{cipher, action };

		string inname, outname; //to be used below
		int key;
		switch (request.cipher) 
		{
		case ciphers::affine:
			//PLACEHOLDER//
			//first check if the action is encrypt or decrypt then call the associated affine function
			break;
		case ciphers::caesar:
			if (request.action == encrypt) {
				cout << "plaintext file name? \n";
				inname = get_filename();
				cout << "Key? Please enter the number representing the letter where A=0 and Z=25. \n";
				key = get_num();
				cout << "Name of file to place encrypted text? \n";
				outname = get_filename();

				Caesar_enc(inname, outname, key);
			}

			else {//must've been decrypt, forced encrypt or decrypt. See above
				cout << "cipher text file name? \n";
				inname = get_filename();
				cout << "Name of file to place decrypted text? \n";
				outname = get_filename();
				cout << "Do you know the key? (y) or (n) \n";
				if (get_yesno()){
					cout << "Key? Please enter the number representing the letter where A=0 and Z=25. \n";
					key = get_num();
					Caesar_dec(inname, outname, key);
				}
				else {
					Caesar_dec(inname, outname);
				}
			}
			break;
		case ciphers::hill:
			//PLACEHOLDER//
			//first check if the action is encrypt or decrypt then call the associated affine function
			break;
		case ciphers::vigenere:
			//PLACEHOLDER//
			//first check if the action is encrypt or decrypt then call the associated affine function
			break;
		}
		cout << "Complete! \n";
		
	}

	catch (runtime_error&e) {
		cerr << "Run time error: " << e.what() << "\n";
		return 1;
	}

	catch (...) {
		cerr << "Error unknown exception. \n";
		return 2;
	}


}

