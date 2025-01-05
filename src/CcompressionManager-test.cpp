#include "CcompressionManager-test.h"
#include "CcompressionManager.h"
using namespace std;

bool CcompressionTest::testCompression(int compressionMethod, dataPackage package){
	CcompressionManager *compressionManager = new CcompressionManager();
	CdataPackager *dataPackager = new CdataPackager();
		string fileNameCompressed = "testCompressed.pck";
	
	cout << "Begining Compression Testing..." << endl;
	{
		dataPackage compressedPackage = compressionManager->compress(compressionMethod, package);
		if (compressedPackage.dataPointer == nullptr || compressedPackage.size == -1){
			cout << "Error Compressing Package" << endl;
		}
		else{
			cout << "Successfully Compressed Package" << endl;
		}
		//write compressed package to disk:
		cout << "Writing Compressed Package to disk: " << fileNameCompressed << endl;
		if (!dataPackager->packageWriteToDisk(compressedPackage, fileNameCompressed)){
			cout << "Error Writing Compressed Package" << endl;
			return false;
		}
	}//scope to reduce memory overhead
	
	
	//read compressed package from disk:
	cout << "Reading package from disk" << endl;
	dataPackage recoveredCompressed = dataPackager->readPackageFromDisk(fileNameCompressed);
	if (recoveredCompressed.dataPointer == nullptr || recoveredCompressed.size <= 0){
		cout << "Could Not Read Compressed File From Disk" << endl;
		return false;
	}
	cout << "Read Compressed Package From Disk" << endl;
	//decompress
	dataPackage decompressedPackage = compressionManager->decompress(compressionMethod, recoveredCompressed);
	if (decompressedPackage.dataPointer == nullptr || decompressedPackage.size == -1){
		cout << "Error Decompressing Package" << endl;
	}
	else{
		cout << "Successfully Decompressed Package" << endl;
	}
	
	//run checksum to compare:
	if (decompressedPackage.size != package.size){
		cout << "Decompressed Package size difference" << endl <<
		"\tOriginal Size:\t\t" << package.size << endl <<
		"\tDecompressed Size:\t" << decompressedPackage.size << endl;
		return false;
	}
	unsigned int originalDataChecksum = 
				compressionManager->createChecksumForCompressionTests(package.dataPointer, package.size);
	unsigned int decompressedDataChecksum = 
				compressionManager->createChecksumForCompressionTests(decompressedPackage.dataPointer, decompressedPackage.size);
	cout << "Checksum comparison: " << endl << 
		"\tOriginal Checksum:\t" << originalDataChecksum << endl <<
		"\tDecompressed Checksum:\t" << decompressedDataChecksum << endl;
	if (originalDataChecksum != decompressedDataChecksum){
		cout << "\033[31mDecompression Failed\033[0m" << endl;
	}
	else{
		cout << endl << "\033[32mCompression & Decompression Tests Passed\033[0m" << endl << endl;
		return true;
	}
	
	
	return false;
}