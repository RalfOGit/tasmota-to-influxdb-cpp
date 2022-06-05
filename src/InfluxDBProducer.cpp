#define _CRT_SECURE_NO_WARNINGS

#include <InfluxDBProducer.hpp>
#include <InfluxDBFactory.h>


//using namespace libtasmota;


InfluxDBProducer::InfluxDBProducer(void) :
  //influxDB(influxdb::InfluxDBFactory::Get("udp://localhost:8094/?db=test")),
  //influxDB(influxdb::InfluxDBFactory::Get("http://localhost:8086/?db=test")),
    influxDB(influxdb::InfluxDBFactory::Get("http://192.168.178.16:8086/?db=test")),
    influxPoint("tasmota") {
    influxDB->batchOf(100);
}

InfluxDBProducer::~InfluxDBProducer(void) {}


void InfluxDBProducer::flush(void) {
    influxDB.get()->flushBuffer();
}


void InfluxDBProducer::produce(const TasmotaDeviceInfo& device) {

    influxPoint.addTag("device", conformString(device.getModuleType()));
    influxPoint.addTag("serial", conformString(device.getHostName()));

    for (auto& command : device.getCommands()) {
        std::string value = device.getApi().getValueFromPath(command);
        double dvalue = 0.0;
        int n = sscanf(value.c_str(), "%lf", &dvalue);
        if (n == 1) {
            influxPoint = influxPoint.addField(conformString(command), dvalue);
        }
        fprintf(stderr, "%s  %s  %s => %s\n", device.getModuleType().c_str(), device.getHostName().c_str(), command.c_str(), value.c_str());
    }

    std::string name = influxPoint.getName();
    influxDB.get()->write(std::move(influxPoint));
    influxPoint = influxdb::Point(name);
}


std::string InfluxDBProducer::conformString(const std::string& str) {
    std::string result;
    for (auto c : str) {
        switch (c) {
        case ' ':  result.append(1, '-'); break;
        case ':':  result.append(1, '-'); break;
        default:   result.append(1, c); break;
        }
    }
    return result;
}
