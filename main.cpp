/*
+----------------------------+
| Array of Structures (AoS)  |
|           vs               |
| Structures of Arrays (SoA) |
+----------------------------+

-compares AoS vs SoA with acessing, modifying variables and searching for
 single values within the structures
-with different sizes of elements (limits array)
-the test is then repeated multiple times (timesRepeatTest)
-the total set of tests are then repeated using conquer and dived over 4 unsynced threads
*/

//code is compiled in 32-bit or 64-bit
#if __WORDSIZE == 64
char *size = "64-bits";
#else
char *size = "32-bits";
#endif

#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <windows.h>
#include <iomanip>
#include <intrin.h>
#include "helper.h"
#include <map>

using std::string;
using std::vector;
using std::cout;
using std::endl;

double pcFreq;

int main(){
	//Array of Stuctures (AoS)
	vector<Person*> persons; 

	//Stucture of Arrays (SoA)
	People *people = new People;

	cout << "This is a test for comparing:" << endl <<
		"Array of Structures (AoS) against Structures of arrays (SoA)" << endl <<
		"all times in micro seconds (1/1000 ms)" << endl <<
		"Code compiled in: " << size << endl <<
		"Data structure actual size: " << Helper::sizeOfPersons() << " bytes" << endl << endl;
	
	Helper::readFile("file.txt", &persons, people);
	

	Result results[numTests][limitCount];
	Helper::doTests(persons, people, results);

	Helper::printResultHeading();
	Helper::displayResults("Acessing test", results[0], results[2]); //access results
	Helper::displayResults("Modifying test", results[1], results[3]); //modify results

	Helper::deletePeopleandPersons(persons, people);

	cout << endl << endl;
	system("pause");
	return 0;
}

void Helper::doTests(std::vector<Person*> persons, People *people, Result results[numTests][limitCount]){
	int totalAgeSoA, totalAgeAoS;

	LARGE_INTEGER frequency; // ticks per second

	//calculate the actual pc frequency
	QueryPerformanceFrequency(&frequency);
	pcFreq = (double)(frequency.QuadPart) / 1000000;
	std::cout << std::setprecision(2) << std::fixed;

	//repeat test multiple times to get averages for more accuracy on results
	for (unsigned int q = 0; q < timesRepeatTest; q++){
		for (unsigned int r = 0; r < limitCount; r++){
			if (r == 0)
				cout << "Test No." << (q + 1) << endl << "---------------------------" << endl;

			LARGE_INTEGER t1, t2; // ticks
			double AoS_access_time, SoA_access_time, AoS_mod_time, SoA_mod_time;

			// get ticks per second
			QueryPerformanceFrequency(&frequency);

			totalAgeSoA = 0;
			totalAgeAoS = 0;

			//AoS acess test
			QueryPerformanceCounter(&t1); //start timer
			for (unsigned int i = 0; i < limits[r]; i++)
				totalAgeAoS += persons.at(i)->age;
			QueryPerformanceCounter(&t2); //stop timer
			AoS_access_time = (t2.QuadPart - t1.QuadPart) *1000000.0 / frequency.QuadPart;

			//SoA acess test
			QueryPerformanceCounter(&t1); //start timer
			for (unsigned int i = 0; i < limits[r]; i++)
				totalAgeSoA += people->ages.at(i);
			QueryPerformanceCounter(&t2); //stop timer
			SoA_access_time = (t2.QuadPart - t1.QuadPart) *1000000.0 / frequency.QuadPart;

			//AoS modify test
			QueryPerformanceCounter(&t1); //start timer
			for (unsigned int i = 0; i < limits[r]; i++)
				persons.at(i)->age += 3;
			QueryPerformanceCounter(&t2); //stop timer
			AoS_mod_time = (t2.QuadPart - t1.QuadPart) *1000000.0 / frequency.QuadPart;

			//SoA modify test
			QueryPerformanceCounter(&t1); //start timer
			for (unsigned int i = 0; i < limits[r]; i++)
				people->ages.at(i) += 3;
			QueryPerformanceCounter(&t2); //stop timer
			SoA_mod_time = (t2.QuadPart - t1.QuadPart) *1000000.0 / frequency.QuadPart;

			//display test result
			cout << "Number of elements: " << limits[r] << endl << "Access" << endl;
			cout << "AoS_time: " << AoS_access_time << " micro seconds" << endl;
			cout << "SoA_time: " << SoA_access_time << " micro seconds" << endl;
			cout << endl << "Modify" << endl;
			cout << "AoS_time: " << AoS_mod_time << " micro seconds" << endl;
			cout << "SoA_time: " << SoA_mod_time << " micro seconds" << endl;
			cout << std::endl;

			double times[] = { AoS_access_time, AoS_mod_time, SoA_access_time, SoA_mod_time };

			//store results
			if (q == 0){
				for (unsigned int i = 0; i < numTests; i++)
					results[i][r] = *Helper::newResult(times[i]);
			}
			else{
				for (unsigned int i = 0; i < numTests; i++)
					Helper::appendResult(&results[i][r], times[i]);
			}
		}
	}
}

//source code for this function from: http://www.emoticode.net/c-plus-plus/get-the-cpu-name-stirng-with-__cpuid-instruction.html
string Helper::getCpuBrandString(){
	//display cpu brand
	// Get extended ids.
	int CPUInfo[4] = { -1 };
	__cpuid(CPUInfo, 0x80000000);
	unsigned int nExIds = CPUInfo[0];
	// Get the information associated with each extended ID.
	char CPUBrandString[0x40] = { 0 };
	for (unsigned int i = 0x80000000; i <= nExIds; ++i){
		__cpuid(CPUInfo, i);
		// Interpret CPU brand string and cache information.
		if (i == 0x80000002)
		{
			memcpy(CPUBrandString,
				CPUInfo,
				sizeof(CPUInfo));
		}
		else if (i == 0x80000003){
			memcpy(CPUBrandString + 16,
				CPUInfo,
				sizeof(CPUInfo));
		}
		else if (i == 0x80000004){
			memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
		}
	}
	string s = CPUBrandString;
	return s;
}

//create new insert for the SoA people
void Helper::newPeople(People *people, int age, char name[NAME_SIZE], bool isMale){
	people->ages.push_back(age);
	people->names.push_back(newName(name));
	people->male.push_back(isMale);
}

//converts character array to Name struct
Name* Helper::newName(char name[NAME_SIZE]){
	Name *s = new Name;
	for (unsigned int i = 0; i < NAME_SIZE; i++)
		s->s[i] = name[i];
	return s;
}

//creates new person struct
Person* Helper::newPerson(int age, char name[NAME_SIZE], bool isMale){
	Person *per = new Person;
	per->age = age;
	per->name = newName(name);
	per->male = isMale;
	return per;
}

//display results for test
void Helper::displayResults(string testHeading, Result AoS_res[limitCount], Result SoA_res[limitCount]){
	cout << endl << "---------------------------" << endl;
	cout << " " << testHeading << endl;
	cout << "---------------------------" << endl;
	cout << "Averages times over " << timesRepeatTest << " tests" << endl;
	cout << "elements: AoS : SoA" << endl;

	string limitStr;
	std::ostringstream out;
	string aosAvg;
	int limitCount = (sizeof(limits) / sizeof(*limits));
	for (int e = 0; e < limitCount; e++){
		limitStr = Helper::getLimitStr(e);
		out.str("");
		out << std::fixed << std::setprecision(2) << AoS_res[e].total / timesRepeatTest;
		aosAvg = out.str();
		aosAvg.resize(10, ' ');
		cout << limitStr << aosAvg << " : " << SoA_res[e].total / timesRepeatTest << endl;
	}

	cout << endl << "minimum and maximum times " << timesRepeatTest << " tests" << endl;
	cout << "elements:     AoS_min - AoS_max | SoA_min-SoA_max" << endl;
	string aosStr;
	for (int e = 0; e < limitCount; e++){
		limitStr = Helper::getLimitStr(e);
		out.str("");
		out << std::fixed << std::setprecision(2) << AoS_res[e].minTime << " - " << AoS_res[e].maxTime;
		aosStr = out.str();
		aosStr.resize(19, ' ');
		cout	<< limitStr << aosStr << "   |   " 
				<< SoA_res[e].minTime << " - " << SoA_res[e].maxTime << endl;
	}

	//calculate percentage difference and display
	cout << endl << " % difference by using SoA over AoS";
	cout << endl << "--------------------------------------" << endl;
	cout << "elements:time difference\n";

	for (int i = 0; i < limitCount; i++){
		double diff = AoS_res[i].total - SoA_res[i].total;
		double percent = diff / AoS_res[i].total;
		limitStr = Helper::getLimitStr(i);
		cout << limitStr << percent * 100 << "%" << endl;
	}

	cout << endl;
}

Result* Helper::newResult(double value){
	Result *r = new Result;
	r->maxTime = value;
	r->minTime = value;
	r->total = value;
	return r;
}

void Helper::appendResult(Result* res, double val){
	//total
	res->total += val;
	//min
	if (res->minTime > val)
		res->minTime = val;
	//max
	if (res->maxTime < val)
		res->maxTime = val;
}

//read file into persons and people
void Helper::readFile(string filename, vector<Person*>* persons, People *people){
	const int repeatTimes = 9;
	cout << "Reading file, please wait..." << endl;
	std::ifstream infile;
	infile.open(filename);
	string s, item;
	char name[NAME_SIZE];
	vector<string> items;
	int age;
	bool isMale;
	while (getline(infile, s)){
		std::istringstream ss(s);
		while (getline(ss, item, ',')){
			items.push_back(item);
		}
		if (items.size() >= 3){
			age = stoi(items[0]);
			strncpy_s(name, items[1].c_str(), NAME_SIZE);
			isMale = (items[2] == "true");

			//repeat data multiple times to have over 1,000,000 data structures
			for (unsigned int i = 0; i < repeatTimes; i++){
				Helper::newPeople(people, age, name, isMale);
				persons->push_back(Helper::newPerson(age, name, isMale));
			}
		}
	}
	infile.close();

	cout << "Total elements loaded from file:" << persons->size() << std::endl << endl;
}

void Helper::printResultHeading(){
	cout << endl << "----------------" << endl;
	cout << " RESULTS" << endl;
	cout << "----------------" << endl;
	cout << "Cpu brand: " << Helper::getCpuBrandString() << endl;
	cout << "Actual PC frequency: " << pcFreq << "Ghz" << endl;
	cout << "Code compiled in: " << size << endl;
	cout << "Data structure actual size: " << Helper::sizeOfPersons() << " bytes" << endl << endl;
}

//delete people persons and their structs
void Helper::deletePeopleandPersons(std::vector<Person*> persons, People *people){
	for (unsigned int i = 0; i < persons.size(); i++){
		delete persons.at(i)->name;
		delete(persons.at(i));
		delete people->names.at(i);
	}
	persons.clear();
	delete people;
}

//format string for limit value
string Helper::getLimitStr(unsigned int index){
	string s;
	if (index > limitCount) s = "";
	else s = std::to_string(limits[index]);
	s.resize(10, ' ');
	return s;
}

//sizeof(Person) not accurate enough
int Helper::sizeOfPersons(){
	int a = 0;
	a += sizeof(Name*);
	a += sizeof(double);
	a += sizeof(bool);

	/*
	//33 byte test
	a += sizeof(double) * 2; 
	a += sizeof(long);
	*/

	
	//64 byte test
	a += sizeof(bool);
	a += sizeof(short);
	a += sizeof(double) * 6;
	
	return a;
}