#include "kafkaproducer.h"

#include <sstream>

int main(){
	std::string name="mytest0";
	std::string value="this value";
	std::ostringstream oss;

	std::cout << "topic name: " << name << std::endl;
	for (int i=0;i<1;i++){
		// std::cout << value << i << std::endl;
		oss << value << " " << i << std::endl;
	}
	// std::cout << oss.str();
	sendToKafka(name,oss.str());
	return 0;
}