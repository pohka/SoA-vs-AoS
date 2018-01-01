#ifndef HELPER_H
#define HELPER_H

static const int NAME_SIZE = 11;
const int limits[] = { 100, 1000, 10000, 100000 , 1000000 };
const int limitCount = sizeof(limits) / sizeof(limits[0]);
const int timesRepeatTest = 5;
const int numTests = 4;

/* 
using static memory to store names as
You cannot store arrays in vectors (or in any other standard library container).
The things that standard library containers store must be copyable and assignable,
and arrays are neither of these.
*/
struct Name {
	char s[NAME_SIZE];
};

struct Result {
	double total, minTime, maxTime;
};

/*
name is only 4 bytes as its a pointer not the actual struct
*/
struct Person{ 
	//13 byte test
	double age; //8 bytes
	Name* name; //4 bytes
	bool male;  //1 byte
	
	/*
	//33 byte test
	double a1,a2; //16 bytes
	long a3; //4 bytes
	*/
	
	
	//64 byte test
	bool alive; //1 byte
	short bb; //2 bytes
	double b1, b2, b3, b4, b5, b6; //48 bytes
	
};


struct People{
	std::vector<long> ages;
	std::vector<Name*> names;
	std::vector<bool> male;

	/*
	//33 byte test
	std::vector<double> a1, a2;
	std::vector<long> a3;
	*/

	//64 byte test
	std::vector<bool> alives;
	std::vector<short> bbs;
	std::vector<double> b1, b2, b3, b4, b5, b6;
};




class Helper{
public:
	static void readFile(std::string filename, std::vector<Person*>* persons, People *people);

	static void doTests(std::vector<Person*> persons, People *people, Result results[numTests][limitCount]);
	static std::string getCpuBrandString();
	static Person* newPerson(int age, char name[NAME_SIZE], bool isMale);
	static void newPeople(People* people, int age, char name[NAME_SIZE], bool isMale);
	static void printResultHeading();
	static void displayResults(std::string testHeading, Result AoS_res[limitCount], Result SoA_res[limitCount]);
	static Result* newResult(double value);
	static void appendResult(Result* res, double val);
	static void deletePeopleandPersons(std::vector<Person*> persons, People *people);
	static std::string getLimitStr(unsigned int index);
	static int sizeOfPersons();

private:
	static Name* newName(char[NAME_SIZE]);

};
#endif
