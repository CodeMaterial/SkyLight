#pragma once

#include <spdlog/spdlog.h>
#include <lcm/lcm-cpp.hpp>
#include <tomlcpp.hpp>
#include <string>

namespace skylight {

    class Messaging
            : public lcm::LCM {
    public:
        explicit Messaging(std::string lcm_url = "udpm://239.255.76.67:7667?ttl=1") :
        lcm::LCM(std::move(lcm_url)) {
            LoadMapping();
        };

        template<class MessageType>
        inline int publish(const std::string &channel, const MessageType *msg) {

            spdlog::info("Publishing to {}", channel);


            auto [mapping_found, mapped_channel] = m_pRouting->getString(channel);

            if (mapping_found) {
                spdlog::info("found 1:1 mapping from {} to {}", channel, mapped_channel);
                lcm::LCM::publish(mapped_channel, msg);
            }


            auto channelArray = m_pRouting->getArray(channel);
            if (channelArray) {
                for (int i = 0;; i++) {
                    auto [mapping_found, mapped_channel] = channelArray->getString(i);
                    if (!mapping_found) break;
                    spdlog::info("found 1:many mapping from {} to {}", channel, mapped_channel);
                    lcm::LCM::publish(mapped_channel, msg);
                }
            }


            return lcm::LCM::publish(channel, msg);
        }

    private:
        void LoadMapping() {
            // this path needs to be not so static, yikes
            std::string configPath = "/home/pi/SkyLight/src/config/lcm_config.toml";
            toml::Result res = toml::parseFile(configPath);
            if (!res.table) {
                spdlog::error("Could not load {}", configPath);
            }
            m_pRouting = res.table->getTable("routing");
        }

    private:

        std::shared_ptr<toml::Table> m_pRouting;
    };
}