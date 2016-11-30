
compile:
	g++ -lpthread -lrt c37118.cpp c37118command.cpp c37118configuration.cpp c37118data.cpp c37118header.cpp \
	c37118pmustation.cpp C37Paser2kafka.cpp kafkaproducer.cpp \
	-o C37Paser2kafka -lrdkafka++



