#include <iostream>
#include <string>

#include "game/Game.hpp"
#include "io/Parser.hpp"

int main() {
    Game g;
    std::cout << "=== Nouveau jeu AwaleX (C++ Modulaire) ===\n";
    g.print_board();

    std::string line;

    while (true) {

        std::cout << "\nEntrez un coup (ex: 3R, 14B, 4TR, 8TB) ou Q pour quitter: ";
        std::getline(std::cin, line);

        if (line.empty()) continue;

        // Quitter
        if (line.size() >= 1 && toupper(line[0]) == 'Q')
            break;

        Move mv;
        if (!Parser::parse(line, mv)) {
            std::cout << "Coup invalide.\n";
            continue;
        }

        if (!g.play_move(mv)) {
            std::cout << "Coup illégal (mauvais trou ou vide).\n";
            continue;
        }

        g.print_board();

        State s = g.check_end();
        if (s != State::CONTINUE) {
            if (s == State::G1WIN) std::cout << ">>> Joueur 1 gagne !\n";
            else if (s == State::G2WIN) std::cout << ">>> Joueur 2 gagne !\n";
            else std::cout << ">>> Match nul !\n";
            break;
        }
    }

    std::cout << "Fin de partie.\n";
    return 0;
}
