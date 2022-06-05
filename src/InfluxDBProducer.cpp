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

    influxPoint.addTag("device", device.getModuleType());
    influxPoint.addTag("serial", device.getHostName());

    for (auto& command : device.getCommands()) {
        std::string value = device.getApi().getValueFromPath(command);
        influxPoint = influxPoint.addField(command, value);
        fprintf(stderr, "%s  %s  %s => %s\n", device.getModuleType().c_str(), device.getHostName().c_str(), command.c_str(), value.c_str());
    }

    std::string name = influxPoint.getName();
    influxDB.get()->write(std::move(influxPoint));
    influxPoint = influxdb::Point(name);
}