#pragma once
#include <lcm/lcm-cpp.hpp>
#include <iostream>
#include <utility>
#include <tomlcpp.hpp>
#include "skylight_time.h"

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

        /*
    if(msg->timestamp == 0)
    {
        msg->timestamp = skylight::Now();
    }
         */
    {
        auto [mapping_found, mapped_channel] = m_pRouting->getString(channel);

        if (mapping_found) {
            std::cout << "found mapping from " << channel << " to " << mapped_channel << std::endl;
            return lcm::LCM::publish(mapped_channel, msg);
        }
    }

    {
        auto channelArray = m_pRouting->getArray(channel);
        if (channelArray) {
            for (int i = 0; ; i++) {
                auto [mapping_found, mapped_channel] = channelArray->getString(i);
                if (!mapping_found) break;

                lcm::LCM::publish(mapped_channel, msg);
            }
        }
    }

    return lcm::LCM::publish(channel, msg);
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