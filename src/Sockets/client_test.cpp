#include "ptsockets.h"

int main(){
	PTSockets::TCPClient ctest;
	std::string host="127.0.0.1";
	// std::string host="52.10.177.108";
	unsigned char reply[3000];
	unsigned char* cvalue;

	cvalue = new unsigned char[200];
	int k;

	ctest.setPort(5000);
	if(ctest.setConnection(host) > 0 ){
		ctest.sendData("test1");
		k = ctest.receiveData(reply,sizeof(reply));
	}else{
		delete[] cvalue;
		return 1;
	}
	std::cout << reply << std::endl;
	std::cout << "count of receive data is " << k << std::endl;
	ctest.closeConnection();

	if(ctest.setConnection(host) > 0 ){
		ctest.sendData("test2");
		k = ctest.receiveData(reply,200);
	}else{
		delete[] cvalue;
		return 1;
	}
	
	std::cout << cvalue << std::endl;
	std::cout << "count of receive data is " << k << std::endl;
	// delete[] cvalue;
	ctest.closeConnection();

	cvalue = (unsigned char*)("C-test");
	if(ctest.setConnection(host) > 0 ){
		ctest.sendData(cvalue,sizeof(cvalue));
		std::cout << cvalue << std::endl;
		k = ctest.receiveData(cvalue,200);
		if (k==-1){
			delete[] cvalue;
			return 1;
		}
		std::cout << "count of receive data is " << k << std::endl;
	}else{
		delete[] cvalue;
		return 1;
	}
	delete[] cvalue;
	ctest.closeConnection();

	return 0;

}