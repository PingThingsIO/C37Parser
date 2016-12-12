#include "ptsockets.h"

namespace PTSockets {
	int Server::getSock(){
		return this->sock;
	}

	int Server::getPort(){
		return this->port;
	}

	std::string Server::getAddress(){
		return this->address;
	}

	void Server::setSock(int newSock){
		this->sock = newSock;
	}

	void Server::setPort(int newPort){
		this->port = newPort;
	}

	void Server::setAddress(std::string newAddress){
		this->address = newAddress;
	}

	TCPServer::TCPServer(){
		setSock(-1);
		setPort(0);
		setAddress("");
	}
	TCPServer::TCPServer(int targetPort, std::string newAddress){
		setSock(-1);
		setPort(targetPort);
		setAddress(newAddress);
	}
	TCPServer::~TCPServer(){}
	

	UDPClient::UDPClient(){}
	UDPClient::~UDPClient(){}

	int Client::getSock(){
		return this->sock;
	}
	int Client::getPort(){
		return this->port;
	}
	std::string Client::getAddress(){
		return this->address;
	}

	void Client::setSock(int newSock){
		this->sock = newSock;
	}
	void Client::setPort(int newPort){
		this->port = newPort;
	}
	void Client::setAddress(std::string newAddress){
		this->address = newAddress;
	}

	TCPClient::TCPClient(){
		setSock(-1);
		setPort(0);
		setAddress("");
	}

	TCPClient::TCPClient(int targetPort, std::string serverAddress){
		setSock(-1);
		setPort(targetPort);
		setAddress(serverAddress);
	}

	TCPClient::~TCPClient(){
		close(getSock());
		setSock(-1);
	}

	int TCPClient::setConnection(std::string serverAddress){
		/*
		* Set up TCP connection to target server at specified port
		* return socket if it is connected; -1 if it fails
		*/
		int on = 1;
		int sockfd;

		setAddress(serverAddress); // Update server address

		if(getSock() == -1){
			/* Create socket */
			sockfd = socket(AF_INET,SOCK_STREAM, 0);
			if (sockfd == -1){	
				std::cout << "Can't create socket." << std::endl;	
				return -1;
			}
		}

		/* Set protocol at socket level and specifies that the rules used in validating
		*  addresses supplied to bind() should allow reuse of local addresses, if this is 
		*  supported by the protocol. This option takes an int value. This is a Boolean option.
		*/
		int status = setsockopt(sockfd, SOL_SOCKET,SO_REUSEADDR, (const char *) &on, sizeof(on));
		if(sockfd == -1){
			std::cout << "Error opening socket " << std::endl;
			return -1;
		}

		// update sock
		setSock(sockfd);
		/* Initialize socket structure */
		serv_addr.sin_family = AF_INET;
		// Assuming serverAddress is plain ip address
    	serv_addr.sin_addr.s_addr = inet_addr(serverAddress.c_str()); 
    	serv_addr.sin_port = htons(getPort());

	    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
	         perror("Connect Failed !\n");
	         return -1;
	    }

	    return sockfd;
	}

	int TCPClient::sendData(std::string contents){
		/*
		* Send a string message on a socket
		* The sendData() call may be used only when the socket is 
		* in a connected state (so that the intended recipient is known).
		* If the socket is not connected, return -1. Otherwise 
		* return the renturn value from send.
		* For send call, on success, it returns the number of characters sent;
		* on error, -1 is returned, and errno is set appropriately. 
		*/

		if( getSock() < 0 ){
			return -1; // socket is not connected
		}
		int status = send(getSock(),contents.c_str(),strlen(contents.c_str()),0);
		if( status < 0){
			std::cout << "Client send data failed. " << std::endl;
		}
		return status;
	}

	int TCPClient::sendData(unsigned char* buffer_tx, int size){
		/*
		* Send a c string message on a socket
		* The sendData() call may be used only when the socket is 
		* in a connected state (so that the intended recipient is known).
		* If the socket is not connected, return -1. Otherwise 
		* return the renturn value from send.
		* For send call, on success, it returns the number of characters sent;
		* on error, -1 is returned, and errno is set appropriately. 
		*/
		if( getSock() < 0){
			return -1; // socket is not connected
		}
		int status = send(getSock(),buffer_tx,size,0);
		if( status < 0){
			std::cout << "Client send data failed. " << std::endl;
		}
		return status;	
	}

	/*
	* receiveData: receives data from server and return the count of bytes received
	*/
	int TCPClient::receiveData(unsigned char* buffer_rx, int size = 65535){
		/* In C37.118 framesize is defined by 2-byte unsigned short integer
		*  The max size of a single frame is 65535 bytes.
		*  Super PDC aggregate data from many PMUs, the framesize might be much larger.
		*/
		int count = 0;
		
		if( getSock() < 0){
			return -1; // socket is not created
		}

		count = read(getSock(),buffer_rx,size);
		if( count < 0){
			std::cout << "Client failed to receive data from " << getAddress() << std::endl;
		}

		return count;
	}

	int TCPClient::closeConnection(){
		int status = close(getSock());
		if( status == -1){
			std::cout << "TCP client socket failed to close." << std::endl;
		}
		setSock(-1);
		return status;

	}

}