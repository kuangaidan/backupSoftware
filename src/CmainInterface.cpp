#include "CmainInterface.h"
#include "CdataPackager.h"
#include "CcompressionManager.h"
#include "CencryptionManager.h"

//interface error codes:

const int noError = 0;
const int enumerationError = 1;
const int filterError = 2;
const int packagingError = 3;
const int compressionError = 4;
const int encryptionError = 5;
const int writeError = 6;
using namespace std;


extern "C" {
    int add(int a, int b) {
        return a + b;
    }
	
	int backup(const char* srcLocation, const char* destlocation, const char* filters, bool compress, bool encrypt, const char* key){
		CdataPackager *dataPackager = new CdataPackager();
		CcompressionManager *compressionManager = new CcompressionManager();
		CencryptionManager *encryptionManager = new CencryptionManager();
		CmainInterface *mainInterface = new CmainInterface();
		//get initial package:
		vector<string> listOfFiles = dataPackager->enumerateFiles(srcLocation);
		if (listOfFiles.empty())
			return enumerationError;
		vector<string> split = mainInterface->splitFilters(filters);
		for (int i = 0; i < split.size(); i++){
			cout << "Filter: " << split[i] << endl;
		}
		dataPackage package = dataPackager->packageFiles(listOfFiles, split);
		if (package.dataPointer == nullptr || package.size == -1)
			return packagingError;
		//compression:
		if (compress)
			package = compressionManager->compress(0, package);
		if (package.dataPointer == nullptr || package.size == -1)
			return compressionError;
		//encrypt
		if (encrypt)
			package = encryptionManager->encrypt(0, key, package);
		if (package.dataPointer == nullptr || package.size == -1)
			return encryptionError;
		//write package to disk:
		if (!dataPackager->packageWriteToDisk(package, destlocation))
			return writeError;
		
		return noError;
	}
	
	int restore(const char* srcFile, const char* destLocation, bool compressed, bool encrypted, const char* key){
		CdataPackager *dataPackager = new CdataPackager();
		CcompressionManager *compressionManager = new CcompressionManager();
		CencryptionManager *encryptionManager = new CencryptionManager();
		
		//read package from disk:
		dataPackage package = dataPackager->readPackageFromDisk(srcFile);
		if (package.dataPointer == nullptr || package.size == -1)
			return writeError;
		
		//decrypt
		if (encrypted)
			package = encryptionManager->decrypt(0, key, package);
		if (package.dataPointer == nullptr || package.size == -1)
			return encryptionError;
		//compression:
		if (compressed)
			package = compressionManager->decompress(0, package);
		if (package.dataPointer == nullptr || package.size == -1)
			return compressionError;
		//unpack:
		if (!dataPackager->unpackageFile(destLocation, package))
			return packagingError;
		
		return noError;
	}
	
}


void CmainInterface::interactGUI(){
	
}



string trim(const string &s) {
    auto start = s.begin();
    while (start != s.end() && isspace(*start)) {
        start++;
    }

    auto end = s.end();
    do {
        end--;
    } while (distance(start, end) > 0 && isspace(*end));

    return string(start, end + 1);
}

// Function to split the string by commas and return a vector of trimmed strings
vector<string> CmainInterface::splitFilters(const char* filters) {
    vector<string> result;
    stringstream ss(filters);
    string item;

    while (getline(ss, item, ',')) {
        result.push_back(trim(item));
    }

    return result;
}