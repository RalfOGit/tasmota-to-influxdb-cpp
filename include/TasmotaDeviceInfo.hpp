#ifndef __TASMOTADEVICEINFO_HPP__
#define __TASMOTADEVICEINFO_HPP__

#include <stdint.h>
#include <vector>
#include <TasmotaAPI.hpp>

using namespace libtasmota;


class TasmotaDeviceInfo {
protected:
    std::string url;
    std::string module_type;
    std::string host_name;
    TasmotaAPI  api;
    uint64_t    poll_interval;
    uint64_t    poll_time;
    std::vector<std::string> commands;

public:
    TasmotaDeviceInfo(const std::string& _url, uint64_t _poll_interval) :
        url(_url),
        poll_interval(_poll_interval * 1000),
        poll_time(0),
        api(_url) {
        module_type = api.getValue("Module");
        host_name   = api.getValueFromPath("StatusNET:Hostname");
    }

    const TasmotaAPI&  getApi         (void) const { return api; }
    const std::string& getUrl         (void) const { return url; }
    const std::string& getModuleType  (void) const { return module_type; }
    const std::string& getHostName    (void) const { return host_name; }
    const uint64_t     getPollInterval(void) const { return poll_interval; }
    const uint64_t     getPollTime    (void) const { return poll_time; }
    const std::vector<std::string>& getCommands(void) const { return commands; }

    bool               addToPollTime       (const uint64_t diff) { return (poll_time += diff) >= poll_interval; }
    uint64_t           subtractFromPollTime(const uint64_t diff) { bool result = (poll_time >= diff); poll_time -= diff; return result; }
    void               addGetValueCommand  (const std::string& name) { commands.push_back(name); }
};

#endif
