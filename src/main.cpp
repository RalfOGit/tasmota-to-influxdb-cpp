// tasmota-to-influxdb-cpp.cpp: Definiert den Einstiegspunkt für die Anwendung.
//
#ifdef _WIN32
#include <Winsock2.h>
#include <Ws2tcpip.h>
#define poll(a, b, c)  WSAPoll((a), (b), (c))
#else
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#endif

//#include <LocalHost.hpp>
#include <Logger.hpp>
#include <TasmotaAPI.hpp>
#include <TasmotaDeviceInfo.hpp>
#include <InfluxDBProducer.hpp>

using namespace libtasmota;


static Logger logger("main");

class LogListener : public ILogListener {
public:
    virtual ~LogListener() {}

    virtual void log_msg(const std::string& msg, const LogLevel& level) {
        fprintf(stdout, "%s", msg.c_str());
    }

    virtual void log_msg_w(const std::wstring& msg, const LogLevel& level) {
        fprintf(stdout, "%ls", msg.c_str());
    }

    virtual void operator ()(const std::string&, const libtasmota::LogLevel&) {

    }
};


/**
 *  Platform neutral sleep method.
 */
static void sleepMs(uint32_t millis) {
#ifdef _WIN32
    Sleep(millis);
#else
    ::sleep(millis / 1000);
#endif
}


/**
 *  Platform neutral method to get a tick count provided in ms ticks; this is useful for timing purposes.
 */
static uint64_t getTickCountInMs(void) {  // return a tick counter with ms resolution
#if 1
    std::chrono::steady_clock::duration time = std::chrono::steady_clock::now().time_since_epoch();  // this is not relative to the unix epoch(!)
    std::chrono::milliseconds time_in_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time);
    return time_in_ms.count();
#else
    // fallback code, in case std::chrono cannot be used
#ifdef _WIN32
    return GetTickCount64();
#else
    struct timespec spec;
    if (clock_gettime(CLOCK_MONOTONIC, &spec) == -1) {
        perror("clock_gettime(CLOCK_MONOTONIC,) failure");
    }
    return spec.tv_sec * 1000 + spec.tv_nsec / 1000000;
#endif
#endif
}


int main(int argc, char** argv) {

    // configure logger and logging levels
    ILogListener* log_listener = new LogListener();
    LogLevel log_level = LogLevel::LOG_ERROR | LogLevel::LOG_WARNING;
    log_level = log_level | LogLevel::LOG_INFO_0;
    log_level = log_level | LogLevel::LOG_INFO_1;
    //log_level = log_level | LogLevel::LOG_INFO_2;
    //log_level = log_level | LogLevel::LOG_INFO_3;
    Logger::setLogOutput(*log_listener, log_level);

    // configure tasmota devices
    std::vector<TasmotaDeviceInfo> tasmota_devices;

    TasmotaDeviceInfo device1("http://tasmota-994e5a-3674/", 30);
    device1.addGetValueCommand("StatusSNS:ENERGY:Power");
    tasmota_devices.push_back(device1);

    // configure influxdb
    InfluxDBProducer influx;

    //
    // main loop
    //
    uint64_t time_previous = getTickCountInMs();

    while (true) {
        uint64_t time_now = getTickCountInMs();
        for (auto& device : tasmota_devices) {
            uint64_t time_delta = time_now - time_previous;
            if (device.addToPollTime(time_delta) == true) {
                influx.produce(device);
                device.subtractFromPollTime(device.getPollInterval());
            }
        }
        time_previous = time_now;
        sleepMs(1000);
    }

    return 0;
}

