#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#include "c37118.h"
#include "c37118configuration.h"
#include "c37118pmustation.h"
#include "c37118data.h"
#include "c37118header.h"
#include "c37118command.h"
#include <unistd.h>
#include <iostream>

#include <string>
#include <cstdlib>
#include <cstdio>
#include <csignal>
#include <cstring>

#ifdef _MSC_VER
#include "../win32/wingetopt.h"
#elif _AIX
#include <unistd.h>
#else
#include <getopt.h>
#endif

#include <librdkafka/rdkafkacpp.h>
// #include "rdkafkacpp.h"

#define A_SYNC_AA 0xAA     
#define A_SYNC_DATA 0x01
#define A_SYNC_HDR 0x11
#define A_SYNC_CFG1 0x21
#define A_SYNC_CFG2 0x31
#define A_SYNC_CMD 0x41

#define SIZE_BUFFER 65535
#define TCP_PORT 5000

static bool run = true;

class ExampleDeliveryReportCb : public RdKafka::DeliveryReportCb {
 public:
  void dr_cb (RdKafka::Message &message) {
    std::cout << "Message delivery for (" << message.len() << " bytes): " <<
        message.errstr() << std::endl;
    if (message.key())
      std::cout << "Key: " << *(message.key()) << ";" << std::endl;
  }
};

/**
* Send request to PMUs and Process Socket Packet C37.118-2011
*/
void doprocessing (int sock, CONFIG_1_Frame *myconf1, CONFIG_Frame *myconf2, DATA_Frame *my_data,HEADER_Frame *my_header);
void select_cmd_action(int sock, CMD_Frame *cmd,CONFIG_1_Frame *myconf1, CONFIG_Frame *myconf2, DATA_Frame *my_data, HEADER_Frame *my_header);

int main( int argc, char *argv[] ){
    int sockfd, newsockfd, portno, clilen;
    int pid;
    unsigned char *buffer_tx, buffer_rx[SIZE_BUFFER];
    struct sockaddr_in serv_addr, cli_addr;
    int  n;
    unsigned short size ;
    
    //Kafka
    std::string brokers = "localhost";
    std::string errstr;
    std::string topic_str;
    std::string mode;
    std::string debug;
    int32_t partition = RdKafka::Topic::PARTITION_UA;
    int64_t start_offset = RdKafka::Topic::OFFSET_BEGINNING;
    bool do_conf_dump = false;
    int opt;
    MyHashPartitionerCb hash_partitioner;
    int use_ccb = 0;

    /*
    * Create configuration objects
    */
    RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    RdKafka::Conf *tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);

    RdKafka::Producer *producer = RdKafka::Producer::create(conf, errstr);
    if (!producer) {
      std::cerr << "Failed to create producer: " << errstr << std::endl;
      exit(1);
    }

    std::cout << "% Created producer " << producer->name() << std::endl;



    //PMU	
    CMD_Frame *my_cmd = new CMD_Frame();
    CONFIG_Frame *my_config2 = new CONFIG_Frame();
    CONFIG_1_Frame *my_config1 = new CONFIG_1_Frame();
    HEADER_Frame *my_header = new HEADER_Frame("");

    /* First call to socket() function */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    int on =1;
    int status = setsockopt(sockfd, SOL_SOCKET,SO_REUSEADDR, (const char *) &on, sizeof(on));
    if (sockfd < 0) 
    {
        perror("ERROR opening socket");
        exit(1);
    }
    /* Initialize socket structure */
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = TCP_PORT;
    serv_addr.sin_family = AF_INET;
    /* local server */
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(portno);
 
    /* Now bind the host address using bind() call.*/
    if (connect(sockfd, (struct sockaddr *) &serv_addr,
                          sizeof(serv_addr)) < 0)
    {
         perror("Connect Failed !\n");
         exit(1);
    }
    
    //Request Config 2 frame
    my_cmd->CMD_set(0x05);
    my_cmd->SOC_set(1480444846);
    my_cmd->IDCODE_set(2);
    
    size = my_cmd->pack(&buffer_tx);
    n = write(sockfd,buffer_tx,size); 
    if (n < 0) {
    	    perror("ERROR writing to socket");
            exit(1);
    }
   
    //REceive Config 2 
    size = read(sockfd, buffer_rx, SIZE_BUFFER);
    my_config2->unpack(buffer_rx);
    
   // Create data with config frame received 
   DATA_Frame *my_data = new DATA_Frame(my_config2);
    
    // Request Header Frame
    
    my_cmd->CMD_set(0x03);
    size = my_cmd->pack(&buffer_tx);
    n = write(sockfd,buffer_tx,size); 
    if (n < 0) {
    	    perror("ERROR writing to socket");
            exit(1);
    }
    
    //REceive Header 2 
    size = read(sockfd, buffer_rx, SIZE_BUFFER);
    my_header->unpack(buffer_rx);
    cout << "INFO: " << my_header->DATA_get() << endl;
    
    
    PMU_Station *pmu_aux;
    pmu_aux = my_config2->PMUSTATION_GETbyIDCODE(2);
    cout << "PMU: " << pmu_aux->STN_get() << endl;
    
    // Enable DATA ON 
    my_cmd->CMD_set(0x02);
    size = my_cmd->pack(&buffer_tx);
    n = write(sockfd,buffer_tx,size); 
    if (n < 0) {
    	    perror("ERROR writing to socket");
            exit(1);
    }
    
    int k, i = 0;
    //REceive Data
    for(i = 0 ; i < 10 ; i++){
    	size = read(sockfd, buffer_rx, SIZE_BUFFER);
        if (size > 0) {
        my_data->unpack(buffer_xr);

        cout << "MY PMU---------------------------------------- "<< endl;
        for (k =0 ; k < pmu_aux->PHNMR_get(); k++){
        	    cout <<pmu_aux->PH_NAME_get(k) << ":" << abs(pmu_aux->PHASOR_VALUE_get(k)) <<"|_ " << arg(pmu_aux->PHASOR_VALUE_get(k))*180/M_PI << endl;
        }
        for (k =0 ; k < pmu_aux->ANNMR_get(); k++){
        	    cout <<pmu_aux->AN_NAME_get(k) << ":" << pmu_aux->ANALOG_VALUE_get(k) << endl;
        }

           // cout << "Analog Value: " << my_data->associate_current_config->pmu_station_list[0]->ANALOG_VALUE_get(0) <<endl << endl;
            //cout << "Analog Value: " << my_data->associate_current_config->pmu_station_list[0]->ANALOG_VALUE_get(1) <<endl;
          }
    }
    
    // Enable DATA OFF 
    my_cmd->CMD_set(0x01);
    size = my_cmd->pack(&buffer_tx);
    n = write(sockfd,buffer_tx,size); 
    if (n < 0) {
    	    perror("ERROR writing to socket");
            exit(1);
    }
   close(sockfd);
   return EXIT_SUCCESS;
} 
  
