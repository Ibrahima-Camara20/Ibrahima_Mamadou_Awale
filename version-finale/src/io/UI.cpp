#include "UI.hpp"
#include <iostream>
#include <limits>
#include "../game/BotFactory.hpp"
#include "../net/Opponent.hpp"

int UI::get_player_choice(int player_num) {
    int choice;
    while (true) {
        std::cout << "Type de Joueur " << player_num << " :\n";
        std::cout << "  1. Humain\n";
        std::cout << "  2. GreedyBot\n";
        std::cout << "  3. AlphaBetaBot\n";
        if (player_num == 1) 
            std::cout << "  4. Jouer en Ligne (MQTT)\n";
            
        std::cout << "Votre choix : ";
        if (std::cin >> choice) {
            if (choice >= 1 && choice <= 3) return choice;
            if (player_num == 1 && choice == 4) return choice;
        }
        std::cout << "Choix invalide. Veuillez reessayer.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

GameConfig UI::configure_game() {
    GameConfig config;
    std::cout << "=== Nouveau jeu AwaleX ===\n";

    int p1_choice = get_player_choice(1);
    
    // Check if Online Mode
    config.online_mode = (p1_choice == 4);
    
    if (!config.online_mode) {
        int p2_choice = get_player_choice(2);
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
        config.bot1 = BotFactory::create(p1_choice);
        config.bot2 = BotFactory::create(p2_choice);
    } else {
        // Online Configuration
        std::string address = "tcp://test.mosquitto.org:1883";
        std::string game_id;
        
        std::cout << "ID de la partie (ex: 1234) : ";
        std::cin >> game_id;
        
        std::cout << "Etes-vous Joueur 1 (1) ou Joueur 2 (2) ? : ";
        std::cin >> config.my_role;
        
        std::string client_id = "AwalePlayer_" + game_id + "_" + std::to_string(config.my_role);
        
        config.mqtt_client = std::make_unique<MQTTClientWrapper>(address, client_id);
        if (!config.mqtt_client->connect()) {
            std::cerr << "Impossible de se connecter au broker MQTT.\n";
            exit(1);
        }
        
        std::cout << "\n[INFO] Connecte au Broker: " << address << "\n";
        
        if (config.my_role == 1) {
            config.topic_publish = "awale/" + game_id + "/p1";
            config.topic_subscribe = "awale/" + game_id + "/p2";
            
            std::cout << "[INFO] Publication sur: " << config.topic_publish << "\n";
            std::cout << "[INFO] Abonnement a:  " << config.topic_subscribe << "\n\n";

            std::cout << "Configuration de VOTRE joueur (P1 Local) :\n";
            int local_choice = 0;
            do {
                 std::cout << "Quel type de joueur etes-vous ? (1-3 only): ";
                 std::cin >> local_choice;
            } while (local_choice < 1 || local_choice > 3);
            
            config.bot1 = BotFactory::create(local_choice);
            config.bot2 = std::make_unique<Opponent>(*config.mqtt_client, config.topic_subscribe);
            config.mqtt_client->subscribe(config.topic_subscribe);
            
        } else {
            config.topic_publish = "awale/" + game_id + "/p2";
            config.topic_subscribe = "awale/" + game_id + "/p1";

            std::cout << "[INFO] Publication sur: " << config.topic_publish << "\n";
            std::cout << "[INFO] Abonnement a:  " << config.topic_subscribe << "\n\n";
            
            std::cout << "Configuration de VOTRE joueur (P2 Local) :\n";
            int local_choice = 0;
            do {
                 std::cout << "Quel type de joueur etes-vous ? (1-3 only): ";
                 std::cin >> local_choice;
            } while (local_choice < 1 || local_choice > 3);

            config.bot1 = std::make_unique<Opponent>(*config.mqtt_client, config.topic_subscribe);
            config.bot2 = BotFactory::create(local_choice);
            config.mqtt_client->subscribe(config.topic_subscribe);
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    
    std::cout << "\n--- Configuration ---\n";
    if (config.online_mode) std::cout << "[EN LIGNE] ";
    std::cout << "Joueur 1 : " << (config.bot1 ? config.bot1->name() : "Humain") << "\n";
    std::cout << "Joueur 2 : " << (config.bot2 ? config.bot2->name() : "Humain") << "\n";
    std::cout << "---------------------\n\n";
    
    return config;
}
