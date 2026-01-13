#include "MQTTClient.hpp"
#include <iostream>
#include <cstring>
#include <chrono>
#include <thread>

MQTTClientWrapper::MQTTClientWrapper(const std::string& address, const std::string& client_id)
    : address(address), client_id(client_id) {
    MQTTClient_create(&client, address.c_str(), client_id.c_str(),
                      MQTTCLIENT_PERSISTENCE_NONE, NULL);
}

MQTTClientWrapper::~MQTTClientWrapper() {
    disconnect();
    MQTTClient_destroy(&client);
}

bool MQTTClientWrapper::connect() {
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    int rc;
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        std::cerr << "Failed to connect, return code " << rc << std::endl;
        return false;
    }
    return true;
}

void MQTTClientWrapper::disconnect() {
    if (MQTTClient_isConnected(client)) {
        MQTTClient_disconnect(client, 10000);
    }
}

bool MQTTClientWrapper::subscribe(const std::string& topic) {
    int rc;
    if ((rc = MQTTClient_subscribe(client, topic.c_str(), 0)) != MQTTCLIENT_SUCCESS) {
        std::cerr << "Failed to subscribe, return code " << rc << std::endl;
        return false;
    }
    return true;
}

bool MQTTClientWrapper::publish(const std::string& topic, const std::string& message) {
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    pubmsg.payload = (void*)message.c_str();
    pubmsg.payloadlen = (int)message.length();
    pubmsg.qos = 1;
    pubmsg.retained = 0;
    
    MQTTClient_deliveryToken token;
    int rc;
    if ((rc = MQTTClient_publishMessage(client, topic.c_str(), &pubmsg, &token)) != MQTTCLIENT_SUCCESS) {
        std::cerr << "Failed to publish, return code " << rc << std::endl;
        return false;
    }
    
    rc = MQTTClient_waitForCompletion(client, token, 1000);
    return rc == MQTTCLIENT_SUCCESS;
}

std::string MQTTClientWrapper::wait_for_message(const std::string& topic) {
    char* topicName = NULL;
    int topicLen;
    MQTTClient_message* message = NULL;
    
    // std::cout << "[MQTT] Waiting for message on " << topic << "..." << std::endl;
    
    while (true) {
        // Blocking receive
        int rc = MQTTClient_receive(client, &topicName, &topicLen, &message, 1000);
        
        if (rc != MQTTCLIENT_SUCCESS && rc != 0) {
             std::cerr << "Receive failed or disconnected. rc=" << rc << std::endl;
             break;
        }

        if (message) {
            // Check if it's the right topic (should be if we only sub to one, but good verify)
            // Actually MQTTClient_receive returns messages for any subscribed topic.
            // For this simple implementation, we assume we want this message.
            
            std::string payload((char*)message->payload, message->payloadlen);
            
            MQTTClient_freeMessage(&message);
            MQTTClient_free(topicName);
            
            return payload;
        }
        
        // If timed out (rc == 0 and message == NULL), loop again
        // std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    return "";
}
