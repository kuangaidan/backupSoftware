programName = backupSoftware
testProgramName = "test-program"
ABIName = BACKUP.so
testFolderDestName = "testDestFolder"
buildpath = build/exe.linux-x86_64-3.10/
G = \033[0;32m

build:
	sudo apt install python3
	sudo apt install python3-tk
	pip install cx_Freeze
	g++ -shared -o $(ABIName) -fPIC src/commonStructures.cpp src/CdataPackager.cpp src/CcompressionManager.cpp \
		src/CencryptionManager.cpp src/CmainInterface.cpp -lcrypto -lssl
	python3 src/GUI/compile.py build
	mv $(buildpath)/start $(buildpath)$(programName)
	#mv $(ABIName) build/exe.linux-x86_64-3.10/$(ABIName)
	#cp src/GUI/icon.png build/exe.linux-x86_64-3.10/icon.png
	ln -s $(buildpath)$(programName) $(programName)
	@echo "${G}Build Sucessful"
	
test:
	-rm -r $(testFolderDestName)
	mkdir $(testFolderDestName)
	g++ -o $(testProgramName) src/commonStructures.cpp src/CdataPackager.cpp src/CcompressionManager.cpp \
		src/CencryptionManager.cpp \
		src/CdataPackager-test.cpp src/CcompressionManager-test.cpp src/CencryptionManager-test.cpp \
		-lcrypto -lssl
	./$(testProgramName)
	rm $(testProgramName)
	
run:
	./$(programName)
	
clean:
	-rm -r $(testFolderDestName)
	-rm -r build
	-rm $(ABIName)
	-rm $(programName)
	-rm *.pck
	@echo "${G}Clean Sucessful"
	

