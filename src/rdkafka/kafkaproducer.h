#include <string>
#include <cstdlib>
#include <cstdio>
#include <csignal>
#include <cstring>
#include <iostream>

#ifdef _MSC_VER
#include "../win32/wingetopt.h"
#elif _AIX
#include <unistd.h>
#else
#include <getopt.h>
#endif

#include <librdkafka/rdkafkacpp.h>

void sendToKafka(std::string mytopic, std::string mymsg);