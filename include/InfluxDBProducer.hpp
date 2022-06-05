#ifndef __INFLUXDBPRODUCER_HPP__
#define __INFLUXDBPRODUCER_HPP__

#include <memory>               // for std::unique_ptr
#include <InfluxDB.h>
#include <TasmotaDeviceInfo.hpp>


class InfluxDBProducer {

protected:
    std::unique_ptr<influxdb::InfluxDB> influxDB;
    influxdb::Point                     influxPoint;

public:

    InfluxDBProducer(void);
    ~InfluxDBProducer(void);

    void flush(void);
    void produce(const TasmotaDeviceInfo &device);
};

#endif