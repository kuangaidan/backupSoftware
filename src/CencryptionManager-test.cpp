#include "CencryptionManager-test.h"
#include "CdataPackager.h"


bool CencryptionManagerTest::encryptionTest(int encryptionMethod, string encryptionKey, dataPackage filePackage)
{
	CencryptionManager *encryptionManager = new CencryptionManager();
	cout << "Starting Encryption Test..." << endl;
	dataPackage encrypted = encryptionManager->encrypt(encryptionMethod, encryptionKey, filePackage);
	if (encrypted.dataPointer == nullptr || encrypted.size == -1){
		cout << "Encryption Failed" << endl;
		return false;
	}
	
	//write to disk:
	CdataPackager *dataPackager = new CdataPackager();
	string fileName = "testEncrypted.pck";
	cout << "Writing encrypted file to disk..." << endl;
	dataPackager->packageWriteToDisk(encrypted, fileName);
	
	//decrypt:
	dataPackage decrypted = encryptionManager->decrypt(encryptionMethod, encryptionKey, encrypted);
	if (encrypted.dataPointer == nullptr || encrypted.size == -1){
		cout << "Decryption Failed" << endl;
		return false;
	}
	
	//verify checksum:
	cout << "Checking checksum:" << endl;
	unsigned int originalChecksum = encryptionManager->createChecksumForEncryptionTests(filePackage.dataPointer, filePackage.size);
	unsigned int newChecksum = encryptionManager->createChecksumForEncryptionTests(filePackage.dataPointer, filePackage.size);
	cout << "\tOriginial File Checksum: " << originalChecksum << endl;
	cout << "\tDecrypted File Checksum: " << newChecksum << endl;
	if (newChecksum != newChecksum){
		cout << "Checksum failed" << endl;
		return false;
	}
	else{
		cout << "Checksum Passed" << endl;
	}
	return true;
}