#ifndef MQTT_CLIENT_HPP
#define MQTT_CLIENT_HPP

#include <string>
#include <vector>
#include <functional>
#include <MQTTClient.h>

class MQTTClientWrapper {
public:
    MQTTClientWrapper(const std::string& address, const std::string& client_id);
    ~MQTTClientWrapper();

    bool connect();
    void disconnect();
    bool subscribe(const std::string& topic);
    bool publish(const std::string& topic, const std::string& message);
    
    // Simple blocking receive for specific topic (simplified for turn-based)
    std::string wait_for_message(const std::string& topic);

private:
    std::string address;
    std::string client_id;
    MQTTClient client;
    
    // Callback static helpers could go here if we needed async
};

#endif
