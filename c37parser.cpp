#include <fstream>
#include <iomanip>
#include <bitset>
#include <iostream>

#include "c37118.h"
#include "c37118configuration.h"
#include "c37118pmustation.h"
#include "c37118data.h"
#include "c37118header.h"
#include "c37118command.h"

// #define SIZE_BUFFER 

void checkFrameType(unsigned char syncSecondByte){
	switch(syncSecondByte){
		case A_SYNC_DATA: std::cout << "This is a data frame." << endl; break;
		case A_SYNC_HDR: std::cout << "This is a header frame." << endl; break;
		case A_SYNC_CFG1: std::cout << "This is a configuration 1 frame." << endl; break;
		case A_SYNC_CFG2: std::cout << "This is a configuration 2 frame." << endl; break;
		case A_SYNC_CMD: std::cout << "This is a command frame." << endl; break;
		default: std::cout << "Undefined frame type." << endl; break;
	}
}
/*
* Read C37.118 data and parse them to proper formats
* Input file should have the formats as defined in C37.118: SYNC leading byte 0xaa
*/
int main(int argc, char* argv[]){
	fstream myfile;
	char buffer;
	char buffer2[2];
	
	// std::vector<unsigned char> buffer_rx(65535, 0);
	unsigned short int frameSize;
	int fileSize, msgCount = 0;
	int pos = 0;

	int pid,size;
	CMD_Frame *my_cmd = new CMD_Frame();
    CONFIG_Frame *my_config2 = new CONFIG_Frame();
    CONFIG_1_Frame *my_config1 = new CONFIG_1_Frame();
    HEADER_Frame *my_header = new HEADER_Frame("");
    PMU_Station *pmu_aux;

	std::vector<int> msgSize, msgStartPosition;	

	// buffer2 = new char[2];
	if(argc < 2){
		std::cout << "Please input file name" << endl;
	}

	myfile.open(argv[1], ios::in | ios::binary);

	if (!myfile){
		std::cout << "Can't open file!" << endl;
		return 1;
	}
	
	myfile.seekg(0, fstream::end);
	fileSize = myfile.tellg();
	std::cout << "Length of file is " << fileSize << endl;

	myfile.seekg(0);

	while(pos < fileSize){
		myfile.read(&buffer,1);	 //Read SYNC
		// cout << setfill('0') << setw(2) << hex << (unsigned int)(unsigned char)buffer << endl; 
		//Check SYNC leading byte
		if ((unsigned char)buffer != A_SYNC_AA){
			std::cout << "File is not in C37.118.2 format." << endl;
			// delete[] buffer2;
			myfile.close();
			return 1;
		}

		myfile.read(&buffer,1); //Read frame type
		checkFrameType((unsigned char)buffer); //Print frame type
		myfile.read(buffer2,2);
		frameSize = buffer2[0] << 8 | (buffer2[1] & 0xff); //Extract frame size
		cout << "frameSize is " << dec << frameSize << endl;
		// my_cmd->CMD_set(0x05);
		// my_cmd->SOC_set(1392733934);
		// my_cmd->IDCODE_set(7);    
		// size = my_cmd->pack(&buffer_tx);
		pos = myfile.tellg();
		cout << "pos is " << pos << endl;
		myfile.seekg(pos-4);
		
		// unsigned char *buffer_rx1 = new unsigned char[14]();
		unsigned char *buffer_rx = new unsigned char[frameSize]();

		cout << " Current pos is " << myfile.tellg() << endl;
		cout << "frameSize is 3: " << dec << frameSize << endl;
		myfile.read((char*)buffer_rx,frameSize);

		// for (int i = 0; i < frameSize ; i++)
		// 	cout << hex << buffer_rx[i] << " ";
		DATA_Frame *my_data = new DATA_Frame(my_config2);
		// for (int i=0;i<15;i++)
		// 	cout << hex << (unsigned int)(buffer_rx1[i]) << endl;
 	// 	cout << "TEST!" << endl;
 		// std::memcpy(dest, source, sizeof dest);

		my_data->unpack(buffer_rx);
		// cout << my_config2->SYNC_get() << endl;
		// cout << dec << my_config2->FRAMESIZE_get() << endl;
		// cout << my_config2->IDCODE_get() << endl;
		// cout << dec << my_config2->SOC_get() << endl;
		// cout << dec << my_config2->FRACSEC_get() << endl;

		cout << "Config2" << endl;
		pmu_aux = my_config2->PMUSTATION_GETbyIDCODE(1);
		std::cout << "PMU: " << pmu_aux->STN_get() << endl;;

		myfile.read((char*)buffer_rx2,frameSize-14);
		
		my_data->unpack(buffer_rx2);


		printf("Test data ");
		for (int k = 0;k < pmu_aux->PHNMR_get();k++){
			cout <<pmu_aux->PH_NAME_get(k) << ":" << abs(pmu_aux->PHASOR_VALUE_get(k)) <<"|_ " << arg(pmu_aux->PHASOR_VALUE_get(k))*180/M_PI << endl;
		}
		for (int k = 0;k < pmu_aux->ANNMR_get(); k++){
    	   	cout <<pmu_aux->AN_NAME_get(k) << ":" << pmu_aux->ANALOG_VALUE_get(k) << endl;
	    }


		pos += frameSize; //next frame position

		myfile.seekg(pos); 
		delete[] buffer_rx1;
		delete[] buffer_rx2;
	}
	// delete[] buffer2;
	// for (int i = 1; i < fileSize; i++){
		
	// 	// myfile.read(&buffer, sizeof(buffer));
	// 	myfile.read(&buffer, 1);
	// 	// cout << setfill('0') << setw(4) << hex << (unsigned int)(unsigned char)buffer;
	// 	std::cout  << setfill('0') << setw(2) << hex << (unsigned int)(unsigned char)buffer;
	// }
	myfile.close();
	
	// std::cout << endl;
	
	return 0;
}