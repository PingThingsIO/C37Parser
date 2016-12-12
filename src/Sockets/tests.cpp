#include "ptsockets.h"

int main(){
	PTSockets::TCPClient ctest;
	std::string host="127.0.0.1";
	unsigned char reply[3000];
	unsigned char* cvalue;

	cvalue = new unsigned char[200];
	int k;

	ctest.setPort(5000);
	if(ctest.setConnection(host) > 0 ){
		ctest.sendData("test");
		k = ctest.receiveData(reply,sizeof(reply));
	}
	std::cout << reply << std::endl;
	std::cout << "count of receive data is " << k << std::endl;
	ctest.closeConnection();

	if(ctest.setConnection(host) > 0 ){
		ctest.sendData("test");
		k = ctest.receiveData(cvalue,200);
	}
	
	std::cout << cvalue << std::endl;
	std::cout << "count of receive data is " << k << std::endl;

	delete[] cvalue;

	ctest.closeConnection();
	return 0;

}