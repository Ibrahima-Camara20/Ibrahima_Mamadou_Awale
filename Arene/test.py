"""
debug.py
Programme pour debugger une partie en entrant des coups ligne par ligne
Format attendu: 1:A -> 15R ou 2:B -> 3TR
"""

import re
from game import AwaleGame

def parse_move_line(line: str):
    """
    Parse une ligne de coup au format: 1:A -> 15R ou 2:B -> 3TR
    Retourne: (move_num, player (0 ou 1), hole_idx (0-15), color_code)
    """
    line = line.strip()
    if not line:
        return None
    
    # Pattern: numéro:joueur -> trou + couleur
    # Exemples: 1:A -> 15R, 2:B -> 3TR, 3:A -> 7TB
    pattern = r'^(\d+):([AB])\s*->\s*(\d+)(T?)([RB])$'
    match = re.match(pattern, line, re.IGNORECASE)
    
    if not match:
        return None
    
    move_num = int(match.group(1))
    player_char = match.group(2).upper()
    hole_ui = int(match.group(3))  # 1-16 (UI)
    is_transparent = match.group(4).upper() == 'T'
    color_char = match.group(5).upper()
    
    # Joueur A = 0 (impairs), Joueur B = 1 (pairs)
    player = 0 if player_char == 'A' else 1
    
    # Convertir trou UI (1-16) vers index (0-15)
    hole_idx = hole_ui - 1
    
    # Construire le color_code pour play_move
    if is_transparent:
        color_code = 'T' + color_char  # TR ou TB
    else:
        color_code = color_char  # R ou B
    
    return (move_num, player, hole_idx, color_code, hole_ui)

def display_board(game: AwaleGame):
    """Affiche le plateau de manière lisible."""
    print("\n" + "=" * 60)
    print(f"Tour {game.moves_played + 1}/400 | Joueur actuel: {'A (impairs)' if game.current_player == 0 else 'B (pairs)'}")
    print(f"Scores - A: {game.scores[0]} | B: {game.scores[1]}")
    print("=" * 60)
    
    # Joueur A (indices pairs = trous impairs 1,3,5,7,9,11,13,15)
    print("\nJoueur A (trous impairs):")
    for i in range(0, 16, 2):
        r, b, t = game.board[i]
        print(f"  Trou {i+1:2}: {r}R {b}B {t}T")
    
    # Joueur B (indices impairs = trous pairs 2,4,6,8,10,12,14,16)
    print("\nJoueur B (trous pairs):")
    for i in range(1, 16, 2):
        r, b, t = game.board[i]
        print(f"  Trou {i+1:2}: {r}R {b}B {t}T")
    print()

def main():
    print("=" * 60)
    print("       DEBUGGER AWALE - Mode Replay Interactif")
    print("=" * 60)
    print("\nFormat des coups: <num>:<joueur> -> <trou><couleur>")
    print("  1:A -> 15R   (Joueur A joue trou 15, Rouge)")
    print("  2:B -> 2B    (Joueur B joue trou 2, Bleu)")
    print("  3:A -> 7TR   (Joueur A joue trou 7, Transparent+Rouge)")
    print("\nCommandes: 'q' quitter, 'reset' reinitialiser, 'valid' coups valides")
    print("=" * 60)
    
    game = AwaleGame()
    display_board(game)
    
    while True:
        try:
            line = input(">>> ").strip()
        except (EOFError, KeyboardInterrupt):
            print("\nAu revoir!")
            break
        
        if line.lower() in ['quit', 'q', 'exit']:
            break
        
        if line.lower() == 'reset':
            game = AwaleGame()
            print("\n*** JEU RÉINITIALISÉ ***")
            display_board(game)
            continue
        
        if line.lower() == 'valid':
            print("\nCoups valides:")
            for hole_idx, color_code in game.get_valid_moves():
                print(f"  Trou {hole_idx + 1} -> {color_code}")
            continue
        
        if line == '':
            display_board(game)
            continue
        
        # Parser le coup
        parsed = parse_move_line(line)
        if parsed is None:
            print(f"Erreur de parsing: '{line}'")
            continue
        
        move_num, player, hole_idx, color_code, hole_ui = parsed
        
        # Vérifier le joueur attendu
        if player != game.current_player:
            print(f"⚠️  Attendu: {'A' if game.current_player == 0 else 'B'}, "
                  f"reçu: {'A' if player == 0 else 'B'}")
        
        # Jouer le coup
        success, message = game.play_move(hole_idx, color_code)
        
        if success:
            print(f"\n✓ Coup {move_num}: Trou {hole_ui} {color_code} - {message}")
            display_board(game)
            
            # Vérifier fin de partie
            over, result = game.is_game_over()
            if over:
                print(f"\n*** PARTIE TERMINÉE: {result} ***")
        else:
            print(f"✗ Coup invalide: {message}")

if __name__ == "__main__":
    main()

debug.py5 Ko
