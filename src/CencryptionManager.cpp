/**
 * Project 用例图
 */


#include "CencryptionManager.h"

/**
 * CencryptionManager implementation
 */
 
unsigned int CencryptionManager::createChecksumForEncryptionTests(char *data, int size){
	unsigned int sum = 0;
	cout << "checksum length: " << size << endl;
	while (size-- > 0){
		sum += (unsigned int)data[size];
	}
	return sum;
}

bool generateHashKey(string passkey, unsigned char* hashKey){
    // Create the hash
	EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
	if (mdctx == NULL) {
		cout << "Error creating digest context\n" << endl;
		return false;
	}

	if (EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL) != 1) {
		cout << "Error initializing digest\n" << endl;
		EVP_MD_CTX_free(mdctx);
		return false;
	}

	// Update digest with the password
	if (EVP_DigestUpdate(mdctx, passkey.c_str(), strlen(passkey.c_str())) != 1) {
		cout << "Error updating digest\n" << endl;
		EVP_MD_CTX_free(mdctx);
		return false;
	}

	// Finalize digest and get the hash
	unsigned int hashLen;
	if (EVP_DigestFinal_ex(mdctx, hashKey, &hashLen) != 1) {
		cout << "Error finalizing digest\n" << endl;
		EVP_MD_CTX_free(mdctx);
		return false;
	}
	
	if (hashLen != 32){
		cout << "Error in hash length" << endl;
		return false;
	}


	EVP_MD_CTX_free(mdctx);
	return true;
}

/**
 * @param int encryptionMethod
 * @param string encryptionKey
 * @param char* filePackage
 * @return char*
 */
dataPackage CencryptionManager::encrypt(int encryptionMethod, string encryptionKey, dataPackage filePackage) {
    dataPackage encrypted;
	encrypted.dataPointer = nullptr;
	encrypted.size = -1;
	
	//generate hashKey from encryptionKey
	unsigned char hashKey[32];
	if (!generateHashKey(encryptionKey, hashKey)){
        cout << "Failed to Generate Key" << endl;
		return encrypted;
	}
	
	//encrypt filePackage
    // Output buffer
	int ciphertextLength = (filePackage.size / 16 + 1) * 16;
    unsigned char* ciphertext = (unsigned char*)calloc(1, ciphertextLength);
	unsigned char* packageData = (unsigned char*)filePackage.dataPointer;
    // Initialize context
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx)
	{
		cout << "Failed to Initialize Context For Encryption" << endl;
		return encrypted;
	}

    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, hashKey, hashKey))
    {
		cout << "Failed to Run Initial Encryption" << endl;
		return encrypted;
	}

    if (1 != EVP_EncryptUpdate(ctx, ciphertext, &ciphertextLength, packageData, filePackage.size))   
    {
		cout << "Failed to Run Encryption Update" << endl;
		return encrypted;
	}

    int len;
    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + ciphertextLength, &len))
    {
		cout << "Failed to Run Final Encryption" << endl;
		return encrypted;
	}
    ciphertextLength += len;

    // Clean up
    EVP_CIPHER_CTX_free(ctx);
	
	cout << "Successfully Encrypted Package" << endl;
	
	encrypted.dataPointer = (char*)ciphertext;
	encrypted.size = ciphertextLength;
	
	return encrypted;
}

/**
 * @param int encryptionMethod
 * @param string encryptionKey
 * @param char* filePackage
 * @return char*
 */
dataPackage CencryptionManager::decrypt(int encryptionMethod, string encryptionKey, dataPackage filePackage) {
    dataPackage decrypted;
	decrypted.dataPointer = nullptr;
	decrypted.size = -1;
	
	//generate hashKey from encryptionKey
	unsigned char hashKey[32];
	if (!generateHashKey(encryptionKey, hashKey)){
        cout << "Failed to Generate Key" << endl;
		return decrypted;
	}
	
	// Encrypted data (ciphertext) and its length
	unsigned char* ciphertext = (unsigned char*)filePackage.dataPointer;
	unsigned int cipherLength = (unsigned int)filePackage.size;

    // 256-bit key and 128-bit IV (should be the same as used for encryption)
    unsigned char key[32] = { /* your key here */ };
    unsigned char iv[16] = { /* your IV here */ };

    // Buffer to hold the decrypted text
    int decryptedtextLen =  (filePackage.size / 16 + 1) * 16;
    unsigned char* decryptedtext = (unsigned char*)malloc(decryptedtextLen);

    // Initialize context
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx)
	{
		cout << "Error Initializing Context" << endl;
		return decrypted;
	}

    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, hashKey, hashKey))
    {
		cout << "Error Decrypting Init" << endl;
		return decrypted;
	}

    // Decrypt the ciphertext
    if (1 != EVP_DecryptUpdate(ctx, decryptedtext, &decryptedtextLen, ciphertext, cipherLength))
    {
		cout << "Error Decrypting Update" << endl;
		return decrypted;
	}

    int len;
    if (1 != EVP_DecryptFinal_ex(ctx, decryptedtext + decryptedtextLen, &len))
	{
		cout << "Error Decrypting Final" << endl;
		return decrypted;
	}
    
    decryptedtextLen += len;

    // Clean up
    EVP_CIPHER_CTX_free(ctx);
	
	
	decrypted.dataPointer = (char*)decryptedtext;
	decrypted.size = decryptedtextLen;
	cout << "Successfully Decrypted Package" << endl;
	
	return decrypted;
}