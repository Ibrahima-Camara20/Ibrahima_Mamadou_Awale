#ifndef EVALUATOR_HPP
#define EVALUATOR_HPP

#include "Rules.hpp"
#include <algorithm>
#include <cmath>

class Evaluator {
private:
    EvalWeights phase1, phase2, phase3;

public:
    Evaluator() {
        // Initialisation des poids (Calibrés)
        phase1 = ::phase1;
        phase2 = ::phase2;
        phase3 = ::phase3;
    }

    // Helper: Détermine si un index appartient au Joueur 1 (indices pairs)
    inline bool is_p1_hole(int index) const {
        return (index % 2 == 0);
    }

    // Sélection des poids selon la phase
    const EvalWeights& get_weights(int total_seeds) const {
        if (total_seeds > THRESHOLD_OPENING) return phase1;
        if (total_seeds > THRESHOLD_ENDGAME) return phase2;
        return phase3;
    }

    // Fonction d'évaluation principale
    // Retourne le score du point de vue du Joueur 1 (Positif = Bon pour P1)
    double evaluate(const GameState& state) {
        // 1. Détection Conditions Terminales (Victoire/Défaite Immédiate)
        if (state.score_p1 >= WINNING_SCORE) return 1000000.0;
        if (state.score_p2 >= WINNING_SCORE) return -1000000.0;
        
        // Si moins de 10 graines, le jeu s'arrête. Celui qui a le plus a gagné.
        if (state.total_seeds < 10) {
            if (state.score_p1 > state.score_p2) return 1000000.0;
            if (state.score_p2 > state.score_p1) return -1000000.0;
            return 0.0;  // Draw
        }

        const EvalWeights& w = get_weights(state.total_seeds);
        double eval = 0.0;

        // 2. Score Direct
        eval += w.w_score * (state.score_p1 - state.score_p2);

        // Variables d'accumulation
        int p1_mat = 0, p2_mat = 0;
        int p1_vuln = 0, p2_vuln = 0;
        int p1_kroo = 0, p2_kroo = 0;
        int p1_mob = 0, p2_mob = 0;
        int p1_blue = 0, p2_blue = 0;
        int p1_trans = 0, p2_trans = 0;

        // Analyse des Chaînes
        int p1_chain_accum = 0;
        int p2_chain_accum = 0;
        double p1_chain_score = 0;
        double p2_chain_score = 0;

        for (int i = 0; i < NHOLES; ++i) {
            const EvalHole& h = state.holes[i];
            int t = h.total();
            bool critical = h.is_critical();

            if (is_p1_hole(i)) {
                // --- Statistiques Joueur 1 ---
                p1_mat += t;
                p1_blue += h.blue;
                p1_trans += h.transparent;
                if (t > 0) p1_mob++;
                if (t > 12) p1_kroo++;

                if (critical) {
                    p1_vuln++;
                    p1_chain_accum++;
                } else {
                    if (p1_chain_accum > 1) p1_chain_score += (p1_chain_accum * p1_chain_accum);
                    p1_chain_accum = 0;
                }

            } else {
                // --- Statistiques Joueur 2 ---
                p2_mat += t;
                p2_blue += h.blue;
                p2_trans += h.transparent;
                if (t > 0) p2_mob++;
                if (t > 12) p2_kroo++;

                if (critical) {
                    p2_vuln++;
                    p2_chain_accum++;
                } else {
                    if (p2_chain_accum > 1) p2_chain_score += (p2_chain_accum * p2_chain_accum);
                    p2_chain_accum = 0;
                }
            }
        }
        
        // Finalize chains
        if (p1_chain_accum > 1) p1_chain_score += (p1_chain_accum * p1_chain_accum);
        if (p2_chain_accum > 1) p2_chain_score += (p2_chain_accum * p2_chain_accum);

        // 3. Application des Poids
        
        // Matériel sur plateau
        eval += w.w_board_mat * (p1_mat - p2_mat);
        
        // Vulnérabilité et Attaque
        eval += w.w_vuln * p1_vuln; 
        eval += w.w_attack * p2_vuln; 

        // Chaînes (Pénalité non-linéaire)
        eval += w.w_chain * p1_chain_score;
        eval -= w.w_chain * p2_chain_score;

        // Kroo et Mobilité
        eval += w.w_kroo * (p1_kroo - p2_kroo);
        eval += w.w_mob * (p1_mob - p2_mob);

        // Avantage Chromatique (Normalisé)
        if (p1_mat > 0) eval += w.w_blue * ((double)p1_blue / p1_mat * 10.0);
        if (p2_mat > 0) eval -= w.w_blue * ((double)p2_blue / p2_mat * 10.0);

        // Flexibilité Transparente
        eval += w.w_trans * (p1_trans - p2_trans);

        return eval;
    }
};

#endif
