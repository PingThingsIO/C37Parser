#include "kafkaproducer.h"

void sendToKafka(std::string mytopic, std::string mymsg){
    //Kafka
    std::string brokers = "localhost";
    std::string errstr;
    std::string topic_str = mytopic;
    std::string debug;
    bool run = true;
  
    int32_t partition = RdKafka::Topic::PARTITION_UA;
    int64_t start_offset = RdKafka::Topic::OFFSET_BEGINNING;
    // bool do_conf_dump = false;
    int opt;
    // MyHashPartitionerCb hash_partitioner;
    int use_ccb = 0;

    /*
    * Create configuration objects
    */
    RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    RdKafka::Conf *tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);

    /*
    * Set configuration properties
    */
    conf->set("metadata.broker.list", brokers, errstr);

    if (!debug.empty()) {
    if (conf->set("debug", debug, errstr) != RdKafka::Conf::CONF_OK) {
      std::cerr << errstr << std::endl;
      exit(1);
    }
    }

    RdKafka::Producer *producer = RdKafka::Producer::create(conf, errstr);
    if (!producer) {
      std::cerr << "Failed to create producer: " << errstr << std::endl;
      exit(1);
    }

    std::cout << "% Created producer " << producer->name() << std::endl;

    /*
     * Create topic handle.
     */
    RdKafka::Topic *topic = RdKafka::Topic::create(producer, topic_str,
                           tconf, errstr);
    if (!topic) {
      std::cerr << "Failed to create topic: " << errstr << std::endl;
      exit(1);
    }

    /*
     * Read messages from stdin and produce to broker.
     */
    
    for (; run; ) 
    {
      // std::string line = "This is an internal test";
      // std::cerr << line << std::endl;
      // std::string line;
      std::string line = mymsg;
   
      /*
      *Produce message
      */
    RdKafka::ErrorCode resp = producer->produce(topic, partition,
              RdKafka::Producer::RK_MSG_COPY /* Copy payload */,
              const_cast<char *>(line.c_str()), line.size(), NULL, NULL);
    if (resp != RdKafka::ERR_NO_ERROR)
      std::cerr << "% Produce failed: " << RdKafka::err2str(resp) << std::endl;
    else
      std::cerr << "% Produced message (" << line.size() << " bytes)" << std::endl;
    producer->poll(0);
    run = true;
    }

    //Delete kafka instance
    delete topic;
    delete producer;
    delete tconf;
    delete conf;
};