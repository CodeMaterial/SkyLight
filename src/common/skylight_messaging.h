#pragma once

#include <spdlog/spdlog.h>
#include <lcm/lcm-cpp.hpp>
#include <tomlcpp.hpp>
#include <string>
#include "skylight_config.h"

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

            spdlog::debug("Publishing to {}", channel);

            if(m_pRouting) {

                auto [mapping_found, mapped_channel] = m_pRouting->getString(channel);

                if (mapping_found) {
                    spdlog::debug("found 1:1 mapping from {} to {}", channel, mapped_channel);
                    lcm::LCM::publish(mapped_channel, msg);
                }

                auto channelArray = m_pRouting->getArray(channel);
                if (channelArray) {
                    for (int i = 0;; i++) {
                        auto [mapping_found, mapped_channel] = channelArray->getString(i);
                        if (!mapping_found) break;
                        spdlog::debug("found 1:many mapping from {} to {}", channel, mapped_channel);
                        lcm::LCM::publish(mapped_channel, msg);
                    }
                }
            }

            return lcm::LCM::publish(channel, msg);
        }

    private:
        void LoadMapping() {

            std::shared_ptr<toml::Table> pConfig = GetConfig("topic_config.toml", {"/usr/local/etc/"});

            if (!pConfig) {
                spdlog::error("Could not load lcm config, using none");
            }
            else
            {
                m_pRouting = pConfig->getTable("routing");
            }

        }

    private:

        std::shared_ptr<toml::Table> m_pRouting;
    };
}