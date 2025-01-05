#define testFolder "testFolder"
//#define testFolder "/mnt/c/archive/games/nes"

#include <iostream>
#include <ctime>
#include "CdataPackager.h"
#include "CcompressionManager-test.h"
#include "CencryptionManager-test.h"
using namespace std;

bool testFileEnumerator(){
	CdataPackager *dataPackager = new CdataPackager();
	//enumerateFiles(string rootFolder);
	vector<string> listOfFiles = dataPackager->enumerateFiles(testFolder);
	
	
	for (int i = 0; i < listOfFiles.size(); i++){
		cout << listOfFiles[i] << endl;
	}
	
	return true;
}

bool testFilePackager(){
	//test file enumerator
	cout << endl << "Starting File Enumerator Test" << endl;
	CdataPackager *dataPackager = new CdataPackager();
	vector<string> filters;	//null for now
	filters.push_back(".txt");
	//filters.push_back(".gif");
	filters.push_back(".jpg");
	filters.push_back(".sfc");
	filters.push_back("ico");
	vector<string> listOfFiles = dataPackager->enumerateFiles(testFolder);
	if (listOfFiles.empty())
		return false;
	else
		cout << "\033[32mFiles Enumerated Successfully\033[0m" << endl << endl;
	
	//package file:
	cout << endl << "Starting File Packaging Test" << endl;
	dataPackage package = dataPackager->packageFiles(listOfFiles, filters);
	cout << "Size of returned data package: " << package.size << endl;
	if (package.dataPointer == nullptr || package.size <= 0){
		return false;
	}
	//write package to disk
	string fileName = "test.pck";
	cout << "Writing Package to disk: " << fileName << endl;
	if (!dataPackager->packageWriteToDisk(package, fileName)){
		cout << "Error Writing Package" << endl;
		return false;
	}
		cout << "\033[32mFiles Packaged Successfully\033[0m" << endl << endl;
	
	//compression test:
	
	CcompressionTest *compressionTest = new CcompressionTest();
	if(compressionTest->testCompression(0, package)){
		cout << "\033[32mPackage Compressed Testing Passed\033[0m" << endl << endl;
	}
	else{
		cout << "\033[31mPackage Compression Testing Failed\033[0m" << endl;
		return false;
	}
	
	
	//encryption test:
	string encryptionKey = "helloPasswordTest256";
	CencryptionManagerTest *encryptionTest = new CencryptionManagerTest();
	if (encryptionTest->encryptionTest(0, encryptionKey, package)){
		cout << "\033[32mPackage Encryption Testing Passed\033[0m" << endl << endl;
	}
	else{
		cout << "\033[31mPackage Encryption Testing Failed\033[0m" << endl;
		return false;
	}
	
	//read package & restore from disk:
	cout << "Reading package from disk" << endl;
	dataPackage recovered = dataPackager->readPackageFromDisk(fileName);
	if (recovered.dataPointer == nullptr || recovered.size <= 0){
		cout << "Could Not Read File From Disk" << endl;
		return false;
	}
	//recover data:
	if (!dataPackager->unpackageFile("./testDestFolder", recovered)){
		return false;
	}
		cout << "\033[32mFiles Unpackaged Successfully\033[0m" << endl << endl;
	
	return true;
}

int main(){
	time_t now = time(0);
	tm* localTime = localtime(&now);
	int minute = localTime->tm_min;
	string minString = to_string(minute);
	if (minute < 10){
		minString = "0" + minString;
	}
	cout << "Running Tests " << (localTime->tm_year+1900) << "/" << (localTime->tm_mon + 1) << "/" 
		<< localTime->tm_mday << " " << localTime->tm_hour << ":" << minString << endl;
	
	

	
	if (testFilePackager()){
		cout << endl << "\033[32mAll Tests Passed\033[0m" << endl << endl;
	}
	else{
		cout << endl << "\033[31mTests Failed\033[0m" << endl << endl;
	}
	return 0;
}