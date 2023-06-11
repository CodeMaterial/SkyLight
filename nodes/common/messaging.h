#pragma once
#include <lcm/lcm-cpp.hpp>
#include <iostream>
#include <utility>
#include "tomlcpp.hpp"

namespace skylight
{

class Messaging
: public lcm::LCM
{
public:
    explicit Messaging(std::string lcm_url = "") : lcm::LCM(std::move(lcm_url))
    {
        LoadMapping();
    };

template<class MessageType>
inline int publish(const std::string &channel, const MessageType *msg) {
    auto [mapping_found, mapped_channel] = m_pRouting->getString(channel);
    if (mapping_found) {
        std::cout << "found mapping from " << channel << " to " << mapped_channel << std::endl;
        return lcm::LCM::publish(mapped_channel, msg);
    }
    else
    {
        return lcm::LCM::publish(mapped_channel, msg);
    }
}
private:
    void LoadMapping()
    {
        std::cout << "loading mapping" << std::endl;
        // load mapping
        toml::Result res = toml::parseFile("/home/pi/SkyLight/config/lcm_config.toml");
        if (!res.table) {
            std::cout << "cannot parse file!: " << res.errmsg << std::endl;
        }
        m_pRouting = res.table->getTable("routing");
    }
private:

    std::shared_ptr<toml::Table> m_pRouting;
};
}