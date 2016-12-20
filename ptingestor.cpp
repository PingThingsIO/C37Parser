#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
// #include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
// #include <arpa/inet.h>
#include "c37118.h"
#include "c37118configuration.h"
#include "c37118pmustation.h"
#include "c37118data.h"
#include "c37118header.h"
#include "c37118command.h"
#include <unistd.h>
#include <iostream>
#include <ctime>
#include <iomanip>

#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "kafkaproducer.h"
#include "frametojson.h"
#include "ptsockets.h"

#include "easylogging++.h"
#define ELPP_STL_LOGGING

INITIALIZE_EASYLOGGINGPP

// Under C37.118-2011 protocol, max frame size is 65535
#define SIZE_BUFFER 65535  
#define TCP_PORT 5000 // Default TCP port

/**
* Send request to PMUs and parse config, header, data frames(SEE C37.118-2011 for details)
* Use rapidjson to package data and write to kafka
*/
int main(int argc, char *argv[] ){

    int opt;
    int port = TCP_PORT; //default TCP socket port
    std::string serverIP = "127.0.0.1"; // default server is localhost
    std::time_t t;

    // std::cout << argc << std::endl;

    // usage parameters
    while ( (opt = getopt(argc, argv, ":i:p:")) != -1) {
        switch (opt) {
        case 'i':
            serverIP = optarg;
            break;
        case 'p':
            port = atoi(optarg);
            break;
        default: /* '?' */
            std::cout << "Usage: " << argv[0] << " [-i|defalut=127.0.0.1] serverip [-p|default=5000] port" << std::endl;
            return 1;
        }
    } 

    /*
    * Setup easylogging configuration
    */ 
    START_EASYLOGGINGPP(argc, argv);
    el::Configurations conf("./Logconf.conf");// Load configuration from file
    el::Loggers::reconfigureLogger("default", conf);// Reconfigure single logger
    // Actually reconfigure all loggers instead
    el::Loggers::reconfigureAllLoggers(conf);
    // Now all the loggers will use configuration from file

    int n;
    unsigned char *buffer_tx, buffer_rx[SIZE_BUFFER];
    unsigned short size;
    PTSockets::TCPClient dataServer;
    
    std::string pmutopic="PMU_DATA";
    rapidjson::StringBuffer dataToJson;
    rapidjson::Writer<rapidjson::StringBuffer> writer(dataToJson);
    
    // Define all frames	
    CMD_Frame *my_cmd = new CMD_Frame();
    CONFIG_Frame *my_config2 = new CONFIG_Frame();
    CONFIG_1_Frame *my_config1 = new CONFIG_1_Frame();
    HEADER_Frame *my_header = new HEADER_Frame("");

    //Set socket port
    dataServer.setPort(port);

    //Request config 2 frame at SOC
    my_cmd->CMD_set(0x05);   

    LOG(INFO) <<"is requesting configuration frame from server " 
    << serverIP << " at " << port << " at SOC " << t;
    do{
        //Get SOC 
        t = std::time(0);
        //Request config 2 frame at SOC
        my_cmd->SOC_set(t);
        
        size = my_cmd->pack(&buffer_tx);

        if(dataServer.setConnection(serverIP) > 0 ){
            LOG(INFO) << "send request to "<< serverIP << " at " 
            << port << " at SOC " << t;

            if(dataServer.sendData(buffer_tx,size) >0){
                LOG(INFO) << "Command sent to server " << serverIP << " at " << port;
                break;
            }else{
                dataServer.closeConnection();}
        }else{
            usleep(1000000);
            LOG(INFO) << "resend request for configuration frame from server " <<
            serverIP << " at " << port << " in 1s";
        }
    }while(true);
    
    //Receive Config
    size = dataServer.receiveData(buffer_rx,SIZE_BUFFER);
    
    // Check if the received data conforms to IEEE C37.118 protocol
    // If it does not conform, return 1
    std::cout << ntohs(*(unsigned short*)(buffer_rx)) << std::endl;

    if (size > 0){
        if (ntohs(*(unsigned short*)(buffer_rx)) == 0xAA31 |ntohs(*(unsigned short*)(buffer_rx)) == 0xAA21){
            LOG(INFO) << "receive configuration frame from " << serverIP << " at "
            << port;     
            my_config2->unpack(buffer_rx);
        }else{
            LOG(ERROR) << "receive data that does not conform to IEEE C37.118 protocol";
            std::cout << "Receive data that does not conform to IEEE C37.118 protocol!" << std::endl;
            dataServer.closeConnection();  
            return 1;
        }
    }
    else{
        LOG(WARNING) << "Do not receive configuration frame from server";
        LOG(WARNING) << "Exit program";
        dataServer.closeConnection();
        return 0;
    }

    std::cout << frameToJson(my_config2).GetString() << std::endl;
    // dataServer.closeConnection();

    // Create data with config frame received 
    DATA_Frame *my_data = new DATA_Frame(my_config2);
    
    // Request and receive Header Frame
    t = std::time(0);
    my_cmd->SOC_set(t);
    my_cmd->CMD_set(0x03);
    size = my_cmd->pack(&buffer_tx);
    dataServer.sendData(buffer_tx,size); 
    size = dataServer.receiveData(buffer_rx,SIZE_BUFFER);
    my_header->unpack(buffer_rx);
    
    std::cout << "Received Header fils is: " << std::endl;
    std::cout << frameToJson(my_header).GetString() << std::endl;
    dataServer.closeConnection();

    // Request and receive config 1    
    // my_cmd->CMD_set(0x04);
    // size = my_cmd->pack(&buffer_tx);
    // if(dataServer.setConnection(serverIP) > 0 ){
    //     dataServer.sendData(buffer_tx,size); 
    // } 
    // size = dataServer.receiveData(buffer_rx,SIZE_BUFFER);
    // my_config1->unpack(buffer_rx);
    // std::cout << std::endl;
    // std::cout << "Received config 1 fils is: " << std::endl;
    // std::cout << frameToJson(my_config1).GetString() << std::endl;
    // dataServer.closeConnection();

    // Turn on transmission of data frame
    t = std::time(0);
    my_cmd->SOC_set(t);    
    my_cmd->CMD_set(0x02);
    size = my_cmd->pack(&buffer_tx);
    if(dataServer.setConnection(serverIP) > 0 ){
        dataServer.sendData(buffer_tx,size); 
    } 
    // size = dataServer.receiveData(buffer_rx,SIZE_BUFFER);

    std::cout << "Transmission of data frame is ON" << std::endl;
    LOG(INFO) << serverIP << " transmission of data frame is ON!";
    // Receive transmission of continuous data frame
    while(true){
        size = dataServer.receiveData(buffer_rx,SIZE_BUFFER);
        my_data->unpack(buffer_rx);
        // std::cout << frameToJson2(my_data).GetString() << std::endl;
        sendToKafka(pmutopic,frameToJson(my_data).GetString());
    }
    dataServer.closeConnection();

    return 0;
   
    
    
    // sendToKafka(pmutopic,dataToJson.GetString());
    
    // return EXIT_SUCCESS;
} 


  
