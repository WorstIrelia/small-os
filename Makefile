all:
	cd ./boot;make
	cd ./kernel;make
clean:
	cd ./boot;make clean
	cd ./kernel;make clean
