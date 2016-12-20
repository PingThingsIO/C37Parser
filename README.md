# C37Parser

C++ codebase for communicating with PMU/PDC, parse received data frames with IEEE C37.118-2011 protocols, then write data in json format and pass them to kafka.

# Dependencies

1. librdkafka
2. rapidjson
3. easylogging

The IEEE C37.118-2011 synchophasor protocol parser is build on top of Open-C37.118 by marsolla. 

To Do:
1. How to accommodate when config changes
2. Benchmarking   
3. Add config-3 frame support if needed


