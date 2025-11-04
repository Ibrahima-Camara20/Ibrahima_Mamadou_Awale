// awale.h — header for awale game engine
#ifndef AWALE_H
#define AWALE_H

#include <stdio.h>

#define NHOLES 16

typedef enum { P1=0, P2=1 } Player;
typedef enum { BEHAV_RED=0, BEHAV_BLUE=1 } Behav;
typedef enum { MOVE_R, MOVE_B, MOVE_TR, MOVE_TB } MoveType;
typedef struct { int r,b,t; } Hole;

typedef struct {
    Hole h[NHOLES];
    int cap[2];
    Player to_play;
    int last_dst;
} Game;

typedef enum { CONTINUE,G1WIN,G2WIN,DRAW } State;
typedef struct{int src;MoveType mt;} ParsedMove;

/* Exported functions (originally in awale.c). Keep signatures in sync. */
int wrap(int i);
int hole_num(int idx);
int is_owned(Player p,int idx);
int sum_hole(const Hole*h);
int sum_board(const Game*g);
void init_game(Game*g);
void print_board(const Game*g);
int next_drop_idx(Player p,int src,Behav beh,int cur);
int sow_color(Game*g,Player p,int src,Behav beh,char color,int count,int start);
void apply_capture(Game*g,Player p,int last);
int has_move(const Game *g, Player p);
State check_end(const Game*g);
int parse_move(const char*s,ParsedMove*out);
int play_move(Game*g,const ParsedMove*mv);

#endif /* AWALE_H */
