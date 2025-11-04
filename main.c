#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "awale.h"

int main(){
    Game g; init_game(&g);
    puts("=== Nouveau jeu AwaleX ===");
    print_board(&g);

    char line[64];
    while(1){
        printf("\nEntrez un coup (ex: 3R, 14B, 4TR, 8TB) ou Q pour quitter: ");
        if(!fgets(line,sizeof(line),stdin))break;
        if(toupper(line[0])=='Q')break;
        ParsedMove mv;
        if(!parse_move(line,&mv)){ puts("Coup invalide."); continue; }
        if(!play_move(&g,&mv)){ puts("Coup illégal (mauvais trou ou vide)."); continue; }

        print_board(&g);
        State s=check_end(&g);
        if(s!=CONTINUE){
            if(s==G1WIN)puts("Joueur 1 gagne !");
            else if(s==G2WIN)puts("Joueur 2 gagne !");
            else puts("Match nul !");
            break;
        }
    }
    puts("Fin de partie.");
    return 0;
}
