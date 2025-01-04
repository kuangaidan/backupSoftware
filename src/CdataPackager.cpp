/**
 * Project 用例图
 */


#include "CdataPackager.h"
using namespace std;

/**
 * CdataPackager implementation
 */
//constants
#define fileHeaderStringTag "PCKGEHEADER"
const int HEADERBLOCKSIZE = 512;
const int BLOCKSIZE = 512;
const int sizeOfChecksum = 4;	//4 bytes per checksum

struct fileHeader {
	//total size is 512
    char name[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char chksum[8];
    char typeflag;
    char linkname[100];
    char magic[6];
    char version[2];
    char uname[32];
    char gname[32];
    char devmajor[8];
    char devminor[8];
    char prefix[155];
    char padding[12];
};

unsigned int createChecksum(char *data, int size){
	unsigned int sum = 0;
	cout << "checksum length: " << size << endl;
	while (size-- > 0){
		sum += (unsigned int)data[size];
	}
	sum &= 0x000FFFFF;	//MAX VALUE FOR 11 CHAR OCTAL
	return sum;
}

void getFileHeaderData(fileHeader &header, const string &filename, const struct stat &fileStat, int fileSize, unsigned int checkSum) {
    memset(&header, 0, sizeof(header));
    strncpy(header.name, filename.c_str(), 100);
    sprintf(header.mode, "%07o", fileStat.st_mode & 0777);
    sprintf(header.uid, "%07o", fileStat.st_uid);
    sprintf(header.gid, "%07o", fileStat.st_gid);
    sprintf(header.size, "%011o", fileSize);
    sprintf(header.mtime, "%011o", static_cast<unsigned>(fileStat.st_mtime));
    sprintf(header.chksum, "%07o", checkSum);
    header.typeflag = S_ISDIR(fileStat.st_mode) ? '5' : '0';
    strncpy(header.magic, "ustar", 6);
    strncpy(header.version, "00", 2);
    strncpy(header.padding, fileHeaderStringTag, 12);	//setting header tag
}

bool setFileFromHeaderData(string fileName, const fileHeader &header, struct stat &fileStat) {
    fileStat.st_mode = static_cast<mode_t>(std::strtol(header.mode, nullptr, 8));
    fileStat.st_uid = static_cast<uid_t>(std::strtol(header.uid, nullptr, 8));
    fileStat.st_gid = static_cast<gid_t>(std::strtol(header.gid, nullptr, 8));
    fileStat.st_size = static_cast<off_t>(std::strtol(header.size, nullptr, 8));
    fileStat.st_mtime = static_cast<time_t>(std::strtol(header.mtime, nullptr, 8));
	
	// Set file mode
    if (chmod(fileName.c_str(), fileStat.st_mode) != 0) {
        std::cerr << "Error: Could not set file mode for " << fileName << std::endl;
        return false;
    }

    // Set file ownership
    if (chown(fileName.c_str(), fileStat.st_uid, fileStat.st_gid) != 0) {
        std::cerr << "Error: Could not set file ownership for " << fileName << std::endl;
        return false;
    }

    // Set file modification time
    struct utimbuf timebuf;
    timebuf.actime = time(nullptr); // Access time
    timebuf.modtime = fileStat.st_mtime; // Modification time
    if (utime(fileName.c_str(), &timebuf) != 0) {
        std::cerr << "Error: Could not set file modification time for " << fileName << std::endl;
		return false;
    }
	return true;
}


bool passFilters(vector<string> fileFilters, string fileName, struct stat fileStat){
	cout << "Filter size: " << fileFilters.size() << endl;
	if (fileFilters.size() == 0){ return true; } //no filters
	if (S_ISDIR(fileStat.st_mode)) { return true; }	//directories pass
	//check regular files against extension filters
	for (int i = 0; i < fileFilters.size(); i++){
		if (strcmp(fileFilters[i].c_str(), 
			fileName.substr(fileName.length() - fileFilters[i].length(), fileFilters[i].length()).c_str()) == 0){
				return true;
		}
	}
	return false;
}

char* readEntireFileIntoBuffer(const string& filePath, struct stat fileStat) {
    // Open the file in binary mode and move the file pointer to the end
    ifstream file(filePath, ios::binary | ios::ate);
    if (!file.is_open()) {
        cerr << "Error: Could not open file " << filePath << endl;
        return nullptr;
    }

    // Get the file size
    size_t fileSize = fileStat.st_size;
    file.seekg(0, ios::beg);

    // Allocate a buffer to hold the file content
    char* buffer = new char[fileSize + 1];

    // Read the file content into the buffer
    if (!file.read(buffer, fileSize)) {
        cerr << "Error: Could not read file " << filePath << endl;
        delete[] buffer;
        return nullptr;
    }

    // Null-terminate the buffer
    buffer[fileSize] = '\0';

    file.close();
    return buffer;
}

char* listDirectoryReverse(const string& dirPath, struct stat fileStat) {
	DIR* dir = opendir(dirPath.c_str());
    if (!dir) {
        cerr << "Error: Could not open directory " << dirPath << endl;
        return nullptr;
    }

    struct dirent* entry;
    string contents;
    
    while ((entry = readdir(dir)) != nullptr) {
        contents += entry->d_name;
        contents += "\n";
    }

    closedir(dir);	
	
    char* buffer = new char[contents.size() + 1];
    strcpy(buffer, contents.c_str());
    return buffer;
}

/**
 * @param vector<string> fileList
 * @param vector<string> fileFilters
 * @return chr*
 
 * fileFilters is list of file extensions that must be included, if empty, all are included
 */
dataPackage CdataPackager::packageFiles(vector<string> fileList, vector<string> fileFilters) {
	dataPackage package;
	if (fileList.size() == 0){	//no files
		package.dataPointer = nullptr;
		package.size = -1;
		return package;
	}
	vector<fileHeader> headerBuffer;
	vector<char*> contentBuffer;
	vector<int> contentIndex;
	vector<int> contentSize;
	int totalSize = 0;
	//set root string:
	string fileRoot = fileList[0];
	int indexOfSlash = fileList[0].rfind("/");
	if (indexOfSlash != string::npos){
		fileRoot = fileList[0].substr(indexOfSlash);
	}
	cout << "Root file to save from: " << fileRoot << endl;
	//iterate through list of files
	for (int i = 0; i < fileList.size(); i++){		
		struct stat st;
		stat(fileList[i].c_str(), &st);
		//if file passes filters, add header to end of header buffer, add content to end of content buffer
		if (passFilters(fileFilters, fileList[i].c_str(), st)){
			cout << "File passed filters:\t" << fileList[i].c_str() << endl;
		}
		else{
			cout << "File failed filters:\t" << fileList[i].c_str() << endl;
			continue;
		}
		//print file name:
		cout << "file name: " << fileList[i].c_str() << endl;
		string pathNameMinusRoot = fileList[i].substr(indexOfSlash+1); 
		cout << "local file name: " << pathNameMinusRoot << endl;
		//get content of file:
		char* fileContentBuffer;
		int fileSize;
		if (!S_ISDIR(st.st_mode)){	//byte content if not a directory
			fileContentBuffer = readEntireFileIntoBuffer(fileList[i].c_str(), st);
			fileSize = st.st_size;
		}
		//else list contents of directory in recursive order
		else{						//list of files if a directory
			fileContentBuffer = listDirectoryReverse(fileList[i].c_str(), st);
			fileSize = strlen(fileContentBuffer);
		}
		//create header and add to header buffer
		fileHeader newHeader;
		getFileHeaderData(newHeader, pathNameMinusRoot.c_str(), st, fileSize, createChecksum(fileContentBuffer, fileSize));
		cout << "\t\t\t checksum: " << newHeader.chksum << endl;
		headerBuffer.push_back(newHeader);
		//report data:
		cout << "Size of " << fileList[i].c_str() << ": \t" << fileSize << endl;
		int lastSize = totalSize;
		contentIndex.push_back(lastSize);	//end of last length
		totalSize += fileSize;
		totalSize = (BLOCKSIZE - (totalSize % BLOCKSIZE)) + totalSize;	//offset to BLOCKSIZE (512)
		contentSize.push_back(totalSize - lastSize);
		contentBuffer.push_back(fileContentBuffer);
		cout << "running total content: " << totalSize << endl;
	}
	cout << "Total Number of Header Files: " << headerBuffer.size() << endl;
	cout << "Total Number of Content Files: " << totalSize << endl;
	cout << "Total SIZE of Package File: " << (headerBuffer.size() * BLOCKSIZE) + totalSize << endl;
	
	
	char* contentFile = (char*)calloc(1, (headerBuffer.size() * BLOCKSIZE) + totalSize + sizeOfChecksum);	//header Size + content size
	//merge data into one file;
	for (int i = 0; i < headerBuffer.size(); i++){
		//copy headers to content file
		memcpy(contentFile + (i * BLOCKSIZE), &headerBuffer[i], BLOCKSIZE);
	}
	
	int headerLength = headerBuffer.size() * BLOCKSIZE;
	for (int i = 0; i < contentBuffer.size(); i++){
		//copy file contents to content file
		memcpy(contentFile + headerLength + contentIndex[i], 
			contentBuffer[i], contentSize[i]);
	}
	cout << "total file length: " << (headerLength + totalSize) << " Bytes" << endl;
	package.dataPointer = contentFile;
	package.size = (headerLength + totalSize);
	//adding final checksum:
	unsigned int packageChecksum = createChecksum(package.dataPointer, package.size);
	memcpy(package.dataPointer + package.size, &packageChecksum, sizeOfChecksum);
	package.size += sizeOfChecksum;
	cout << "Package Checksum: " << packageChecksum << endl;
	
	return package;
}

bool restoreFile(string destRoot, fileHeader* header, char* content, int fileSize){
	mode_t mode = static_cast<mode_t>(strtol(header->mode, nullptr, 8));
	string filePath = destRoot + "/" + header->name;

    if (header->typeflag == '5') {
        // Create directory
        if (mkdir(filePath.c_str(), mode) != 0) {
            cerr << "Error: Could not create directory " << filePath << endl;
            return false;
        }
    } else {
        // Create file
        int fd = open(filePath.c_str(), O_CREAT | O_WRONLY | O_TRUNC, mode);
        if (fd == -1) {
            cerr << "Error: Could not create file " << filePath << endl;
            return false;
        }
		//restore data from content
		ssize_t bytesWritten = write(fd, content, fileSize); 
		if (bytesWritten == -1) { 
			std::cerr << "Error: Could not write to file " << filePath << std::endl; 
			return false;
		}

		
		//restore metadata:
		struct stat st;
		stat(filePath.c_str(), &st);
		if (!setFileFromHeaderData(filePath, *header, st)){
			return false;
		}
		//close file
        close(fd);
    }
	return true;
}

/**
 * @param string destFolder
 * @param char* package
 * @return bool
 */
bool CdataPackager::unpackageFile(string destFolder, dataPackage package) {
	//test is destination folder exists
	struct stat info; 
	if (stat(destFolder.c_str(), &info) != 0 || !(info.st_mode & S_IFDIR)){
		cout << "Destination folder " << destFolder << " does not exist" << endl
			<< "\033[31mRestoration Failed\033[0m" << endl;
		return false;
	}
	
	//check file checksum in last four bytes:
	unsigned int packageChecksum = createChecksum(package.dataPointer, package.size - sizeOfChecksum);
		//grab old checksum
		unsigned int newChecksum = 
		(static_cast<unsigned int>(static_cast<uint8_t>(package.dataPointer[package.size - sizeOfChecksum + 3])) << 24) |
        (static_cast<unsigned int>(static_cast<uint8_t>(package.dataPointer[package.size - sizeOfChecksum + 2])) << 16) |
        (static_cast<unsigned int>(static_cast<uint8_t>(package.dataPointer[package.size - sizeOfChecksum + 1])) << 8)  |
         static_cast<unsigned int>(static_cast<uint8_t>(package.dataPointer[package.size - sizeOfChecksum]));
	cout << packageChecksum << " ?== " << newChecksum << endl;
	if (packageChecksum != newChecksum){
		cout << "\033[31mPackage Checksum Failed\033[0m" << endl;
		return false;
	}
	else{
		cout << "Package Checksum Passed" << endl; 
	}


	//first read each header file (512 bytes)
	vector <fileHeader*> headers;
	//headers are recognized by fileHeaderStringTag in last 12 bytes
	int headerIndex = 0;
	bool readAllHeaders = false;
	int numberOfFilesToRestore = 0;
	while (!readAllHeaders){
		char* headerTag = (char*)malloc(12);
		memcpy(headerTag, package.dataPointer + headerIndex + BLOCKSIZE-12, 12);
		if (strcmp(headerTag, fileHeaderStringTag) == 0){
			numberOfFilesToRestore++;
			fileHeader* restoredHeader = (fileHeader*)malloc(BLOCKSIZE);
			memcpy(restoredHeader, package.dataPointer + headerIndex, BLOCKSIZE);
			headers.push_back(restoredHeader);
			headerIndex += BLOCKSIZE;
		}
		else{
			readAllHeaders = true;
		}
	}
	cout << "Total number of files found from package: " << numberOfFilesToRestore << endl;
	//read each file's contents (ensuring alignment with block size)
	cout << "header total size " << headerIndex << endl;
	int contentIndex = 0;
	int fileIndex = 0;
	int fileRestoredCount = 0;
	while (fileIndex < numberOfFilesToRestore){
		//cout << headers[fileIndex]->size << endl;
		int fileSize = stoi(headers[fileIndex]->size, 0, 8);
		//cout << fileSize << endl;
		char* fileContent = (char*)malloc(fileSize);
		memcpy(fileContent, package.dataPointer + headerIndex + contentIndex, fileSize);
		//checksum
		unsigned int newChecksum = createChecksum(fileContent, fileSize);
		cout << "New checksum: " << newChecksum << endl;
		unsigned int oldChecksum = stoi(headers[fileIndex]->chksum, 0, 8);
		cout << "Old checksum: " << oldChecksum << endl;
		if (oldChecksum != newChecksum){
			cout << "\033[31mFile Checksum Not Equal\033[0m" << endl;
			return false;
		}
		//restore file/folder
		if (!restoreFile(destFolder, headers[fileIndex], fileContent, fileSize)){
			cout << "\033[31mFAILED TO RESTORE FILE\033[0m" << endl;
		}
		else{
			fileRestoredCount++;
		}
		
		//increase index for next file:
		contentIndex += fileSize + (BLOCKSIZE - (fileSize % BLOCKSIZE));	//get proper block size offset
		fileIndex++;
	}
	//restore file structure to destinataion
	if (fileRestoredCount == numberOfFilesToRestore)
		return true;
	return false;
}

/**
 * @param string rootFolder
 * @return vector<string> listOfFiles
 */
vector<string> CdataPackager::enumerateFiles(string rootFolder) {
	vector<string> listOfFiles;
	//verify the root folder exists:	
	struct stat st;
    if (stat(rootFolder.c_str(), &st) == 0) {
        cout << rootFolder << " - Directory exists" << endl;
    } else {
        cerr << rootFolder << " - Directory does not exist" << endl;
		return listOfFiles;	//is empty so will fail
    }
	
	//grab list of file paths
	listOfFiles.push_back(rootFolder);
	DIR *dir;
	for (int i = 0; i < listOfFiles.size(); i++){
		if(stat(listOfFiles[i].c_str(), &st) != 0){
			//error files doesn't exists
			cerr << listOfFiles[i] << " does not exist" << endl;
			listOfFiles.clear();	//will cause failure
			return listOfFiles;
		}
		if (S_ISDIR(st.st_mode)){
			dir = opendir(listOfFiles[i].c_str());
		}
		else{
			continue;	//skip if file
		}
		if (dir) {
			struct dirent *entry;
			while ((entry = readdir(dir)) != nullptr) {
				if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
					continue; // skip current and parent directories
				}	
				string fullPath = listOfFiles[i] + string("/") + entry->d_name;
				listOfFiles.push_back(fullPath);
				cout << fullPath << endl;
			}
			closedir(dir);
		} else {
			cerr << "Error opening directory: " << listOfFiles[i] << endl;
		}
	}
	
	return listOfFiles;
}


//write package to disk:
bool CdataPackager::packageWriteToDisk(dataPackage package, string dest){
	//write, create, replace, only owner can write
	int fd = open(dest.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fd == -1) {
        cerr << "Error opening file: " << strerror(errno) << endl;
        return false;
    }

    // Write the data to the file.
    ssize_t bytesWritten = write(fd, package.dataPointer, package.size);

    if (bytesWritten == -1) {
        cerr << "Error writing to file: " << strerror(errno) << endl;
        close(fd); // Close the file descriptor even on error
        return false;
    }

    // Close the file.
    if (close(fd) == -1) {
        cerr << "Error closing file: " << strerror(errno) << endl;
        return false;
    }

    return true; 
}

//read package from disk:
dataPackage CdataPackager::readPackageFromDisk(string src){
	dataPackage package;
	int fd = open(src.c_str(), O_RDONLY);
    if (fd == -1) {
        cerr << "Error opening file: " << strerror(errno) << endl;
		package.dataPointer = nullptr;
		package.size = -1;
		return package;
    }

    struct stat fileStats;
    if (fstat(fd, &fileStats) == -1) {
        cerr << "Error getting file size: " << strerror(errno) << endl;
        close(fd);
		package.dataPointer = nullptr;
		package.size = -1;
		return package;
    }

    off_t fileSize = fileStats.st_size;

    char* buffer = new char[fileSize];
    if (buffer == nullptr) {
        cerr << "Memory allocation failed." << endl;
        close(fd);
		package.dataPointer = nullptr;
		package.size = -1;
		return package;
    }

    ssize_t bytesRead = read(fd, buffer, fileSize);
    if (bytesRead == -1) {
        cerr << "Error reading file: " << strerror(errno) << endl;
        close(fd);
		delete[] buffer;
		package.dataPointer = nullptr;
		package.size = -1;
		return package;
    }

    buffer[bytesRead] = '\0'; // Null-terminate the string

    if (close(fd) == -1) {
        cerr << "Error closing file: " << strerror(errno) << endl;
        delete[] buffer;
		package.dataPointer = nullptr;
		package.size = -1;
		return package;
    }

   
	package.dataPointer = buffer;
	package.size = fileSize;
	return package;
}