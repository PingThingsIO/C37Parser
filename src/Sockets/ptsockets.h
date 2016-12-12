#include <iostream>
#include <stdio.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

/*
* PTSockets defines sockets to send and receive data
*/
namespace PTSockets {
	class Server{
	private:
		int sock;
		int port; 
		std::string address;

	public:
		struct sockaddr_in serv_addr;
		/*
		* Get methods
		*/
		int getSock();
		int getPort();
		std::string getAddress();

		/* 
		* Set methods
		*/
		void setSock(int newSock);
		void setPort(int newPort);
		void setAddress(std::string newAddress);
	};

	/*
	* TCP/IP Server 
	*/
	class TCPServer: public Server{
	public:
		// Constructors and desctructor
		TCPServer();
		TCPServer(int targetPort, std::string newAddress);
		~TCPServer();
		
		int startServer();
		int sendData();
		int receiveData();

	};

	/*
	*	UDP Server
	*/
	class UDPServer: public Server{
	public:
		UDPServer();
		~UDPServer();
	};


	class Client{
	private:
		int sock;
		int port;
		std::string address;
	
	public:
		struct sockaddr_in serv_addr;

		int getSock();
		int getPort();
		std::string getAddress();

		void setSock(int newSock);
		void setPort(int newPort);
		void setAddress(std::string newAddress); 
	};

	/*
	* TCPClient implements basic methods of sending data to 
	* and receiving data from server
	*/
	class TCPClient: public Client{
	public:
		TCPClient();
		TCPClient(int targetPort, std::string serverAddress);
		~TCPClient();

		int setConnection(std::string serverAddress);
		int sendData(std::string contents);
		int sendData(unsigned char* buffer_tx, int size);
		int receiveData(unsigned char* buffer_rx, int size);
		int closeConnection();
	};

	class UDPClient: public Client{
	public:
		UDPClient();
		~UDPClient();
	}; 

}