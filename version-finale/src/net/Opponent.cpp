#include "Opponent.hpp"
#include <iostream>
#include <jsoncpp/json/json.h>

Opponent::Opponent(MQTTClientWrapper& client, const std::string& topic)
    : client(client), topic(topic) {}

Move Opponent::suggest_move(const Game& g, double time_limit) {
    // std::cout << "[Opponent] Waiting for move from network on topic: " << topic << std::endl;
    
    // Blocking call
    std::string payload = client.wait_for_message(topic);
    
    if (payload.empty()) {
        std::cerr << "[Opponent] Received empty payload or error." << std::endl;
        return {0, MoveType::R}; // Fallback/Error
    }
    
    // std::cout << "[Opponent] Received: " << payload << std::endl;
    
    // Parse JSON
    Json::Value root;
    Json::Reader reader;
    bool parsingSuccessful = reader.parse(payload, root);
    
    if (!parsingSuccessful) {
        std::cerr << "[Opponent] Failed to parse JSON." << std::endl;
        return {0, MoveType::R};
    }
    
    int src = root.get("src", 0).asInt();
    std::string typeStr = root.get("type", "R").asString();
    
    MoveType type = MoveType::R;
    if (typeStr == "R") type = MoveType::R;
    else if (typeStr == "B") type = MoveType::B;
    else if (typeStr == "TR") type = MoveType::TR;
    else if (typeStr == "TB") type = MoveType::TB;
    
    return {(uint8_t)src, type};
}
