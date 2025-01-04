/**
 * Project 用例图
 */


#include "CcompressionManager.h"
const int sizeOfChecksum = 4;	//four bytes
 unsigned int CcompressionManager::createChecksumForCompressionTests(char *data, int size){
	unsigned int sum = 0;
	cout << "checksum length: " << size << endl;
	while (size-- > 0){
		sum += (unsigned int)data[size];
	}
	return sum;
}

// Define a structure to hold the LZ77 encoded triplet
const int maxMatchSize = 256;
struct LZ77Triplet {
    char offset;
    char length;
    char nextChar;
};

// LZ77 compression function
char* LZ77Compress(const char* data, size_t dataSize, int& compressedLength) {
    vector<LZ77Triplet> dictionary;
    size_t pos = 0;
	size_t maxBufferSize = 255;

	cout << endl;
    while (pos < dataSize) {
        size_t matchLength = 0;
        size_t matchOffset = 0;

        // Adjust the actual window size based on current position
        size_t windowStart = (pos > maxBufferSize) ? pos - maxBufferSize : 0;
        size_t windowSize = pos - windowStart;

        // Search for the longest match in the buffer
        for (size_t bufferStart = windowStart; bufferStart < pos; ++bufferStart) {
            size_t length = 0;
            while (bufferStart + length < dataSize && pos + length < dataSize &&
                   data[bufferStart + length] == data[pos + length] && length < maxBufferSize) {
                ++length;
            }
            if (length > matchLength) {
                matchLength = length;
                matchOffset = pos - bufferStart;
            }
        }

        // Create the LZ77 triplet
        LZ77Triplet triplet;
        triplet.offset = static_cast<char>(matchOffset);
        triplet.length = static_cast<char>(matchLength);
        triplet.nextChar = (pos + matchLength < dataSize) ? data[pos + matchLength] : 0;

        dictionary.push_back(triplet);
        // Move the position in the data
        pos += matchLength + 1;
		cout << "\x1b[1F\x1b[2KCompressing:\t\t" << pos << endl;
    }
	
	compressedLength = dictionary.size() * (sizeof(struct LZ77Triplet));
    char* compressedArray = new char[compressedLength];

    char* ptr = compressedArray;
    for (const auto& entry : dictionary) {
        memcpy(ptr, &entry.offset, sizeof(entry.offset));
        ptr += sizeof(char);
        memcpy(ptr, &entry.length, sizeof(entry.length));
        ptr += sizeof(char);
        memcpy(ptr, &entry.nextChar, sizeof(char));
        ptr += sizeof(char);
    }

    return compressedArray;

}


char* LZ77Decompress(vector<LZ77Triplet> dictionary, int& originalSize) {
    vector<char>* decompressedData = new vector<char>;
	cout << endl;
    for (int k = 0; k < dictionary.size(); k++) {
		cout << "\x1b[1F\x1b[2KDecompressing:\t" << k;
		unsigned int offset = ((unsigned int)(dictionary[k].offset)) % 256;
		unsigned int length = ((unsigned int)(dictionary[k].length)) % 256;
        unsigned int startPos = decompressedData->size() - offset;
		cout << "\t" << decompressedData->size() << "\t"; 
			//cout << offset << "\t" << startPos << "\t";
        for (unsigned int i = 0; i < length; i++) {
			if (startPos + i >= decompressedData->size() || startPos + i < startPos){
				cout << "got em\t" << decompressedData->size() << "\t" << (startPos + i)<< endl;
			}
            decompressedData->push_back(decompressedData->at(startPos + i));
        }
		if (k + 1 != dictionary.size())
			decompressedData->push_back(dictionary[k].nextChar);
		//cout << "\tSize: " << decompressedData->size();
		cout << endl;
    }
    originalSize = decompressedData->size();
    char* result = (char*)malloc(originalSize);
    for (int i = 0; i < originalSize; i++){
		result[i] = decompressedData->at(i);
	}

    return result;
}

/**
 * @param int compressionMethod
 * @param char* filePackage
 * @return char*
 */
dataPackage CcompressionManager::compress(int compressionMethod, dataPackage filePackage) {
	//lz
	int compressedLength;
	
	cout << "Initial Length: \t" << filePackage.size << endl;
    char* compressedData = LZ77Compress(filePackage.dataPointer, filePackage.size, compressedLength);
	cout << "Compressed length: \t" << compressedLength << endl;
	dataPackage compressedPackage;
	compressedPackage.dataPointer = compressedData;
	compressedPackage.size = compressedLength;
	return compressedPackage;
}

/**
 * @param int compressionMethod
 * @param char*filePacke
 * @return char*
 */
dataPackage CcompressionManager::decompress(int compressionMethod, dataPackage filePackage) {
	dataPackage decompressedPackage;
	cout << "Decompressing..." << endl;
	//create dictionary of triplets:
	unsigned int position = 0;
	vector<LZ77Triplet> dictionary;
	while (position < filePackage.size){
		LZ77Triplet triplet;
		triplet.offset 	 = filePackage.dataPointer[position++];
		triplet.length 	 = filePackage.dataPointer[position++];
		triplet.nextChar = filePackage.dataPointer[position++];
		dictionary.push_back(triplet);
	}
	cout << "Number of Triplets: " << dictionary.size() << endl;
	
	int originalSize;
	char* decompressedData = LZ77Decompress(dictionary, originalSize);
	cout << "original size: " << originalSize << endl;
	
	decompressedPackage.dataPointer = decompressedData;
	decompressedPackage.size = originalSize;
	
    return decompressedPackage;
}