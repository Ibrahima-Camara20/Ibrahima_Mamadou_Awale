#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "awale.h"

int wrap(int i){ return (i+NHOLES)%NHOLES; }
int hole_num(int idx){ return idx+1; }

int is_owned(Player p,int idx){ 
    int n=hole_num(idx); 
    return (p==P1)?(n%2==1):(n%2==0);
}
int sum_hole(const Hole*h){ return h->r+h->b+h->t; }

int sum_board(const Game*g){ 
    int s=0;
    for(int i=0;i<NHOLES;++i)s+=sum_hole(&g->h[i]);
    return s;
}
void init_game(Game*g){ 
    for(int i=0;i<NHOLES;++i)g->h[i]=(Hole){2,2,2}; 
    g->cap[0]=g->cap[1]=0; g->to_play=P1; 
    g->last_dst=-1; 
}

void print_board(const Game*g){
    puts("");
    for(int i=0;i<NHOLES;++i){
        printf("%d", i);
        const Hole*h=&g->h[i];
        printf("%2d(",hole_num(i));
        if(sum_hole(h)==0)printf(" ) ");
        else{
            if(h->r)printf("%dR",h->r);
            if(h->b)printf("%dB",h->b);
            if(h->t)printf("%dT",h->t);
            printf(")");
        }
        if((i+1)%8==0)puts("");
    }
    printf("Captures -> P1:%d  P2:%d  | À jouer: %s\n",
           g->cap[P1],g->cap[P2],g->to_play==P1?"Joueur 1 (impairs)":"Joueur 2 (pairs)");
}

int next_drop_idx(Player p,int src,Behav beh,int cur){
    int idx=wrap(cur+1);
    if(idx==src) idx=wrap(idx+1);
    if(beh==BEHAV_RED) return idx;
    while(is_owned(p,idx)||idx==src){ idx=wrap(idx+1); if(idx==src)idx=wrap(idx+1); }
    return idx;
}

int sow_color(Game*g,Player p,int src,Behav beh,char color,int count,int start){
    int dst=start;
    for(int i=0;i<count;i++){
        dst=next_drop_idx(p,src,beh,dst);
        Hole*h=&g->h[dst];
        if(color=='R')h->r++; else if(color=='B')h->b++; else h->t++;
    }
    return dst;
}

void apply_capture(Game*g,Player p,int last){
    int idx=last, gained=0;
    while(idx>=0){
        Hole*h=&g->h[idx];
        int s=sum_hole(h);
        if(s==2||s==3){ gained+=s; *h=(Hole){0,0,0}; idx=wrap(idx-1); }
        else break;
    }
    g->cap[p]+=gained;
}

   
// Vérifie si le joueur p a au moins un coup possible
int has_move(const Game *g, Player p) {
    for (int i = 0; i < NHOLES; ++i) {
        if (!is_owned(p, i)) continue;
        const Hole *h = &g->h[i];
        if (h->r > 0 || h->b > 0 || h->t > 0) return 1;
    }
    return 0;
}

State check_end(const Game*g){
    if(g->cap[P1]>=49)return G1WIN;
    if(g->cap[P2]>=49)return G2WIN;
    if(sum_board(g)<10){
        if(g->cap[P1]>g->cap[P2])return G1WIN;
        if(g->cap[P2]>g->cap[P1])return G2WIN;
        return DRAW;
    }
    if (!has_move(g, g->to_play)) {
        return (g->to_play == P1) ? G2WIN : G1WIN;
    }
    return CONTINUE;
}

int parse_move(const char*s,ParsedMove*out){
    while(*s==' '||*s=='\t')s++;
    if(!isdigit((unsigned char)*s))return 0;
    int num=0;
    while(isdigit((unsigned char)*s)){ num=num*10+(*s-'0'); s++; }
    if(num<1||num>16)return 0;
    while(*s==' '||*s=='\t')s++;
    char a=toupper(*s),b=toupper(*(s+1));
    MoveType mt;
    if(a=='R')mt=MOVE_R;
    else if(a=='B')mt=MOVE_B;
    else if(a=='T'&&b=='R')mt=MOVE_TR;
    else if(a=='T'&&b=='B')mt=MOVE_TB;
    else return 0;
    out->src=num-1; out->mt=mt; return 1;
}

int play_move(Game*g,const ParsedMove*mv){
    Player p=g->to_play;
    if(!is_owned(p,mv->src))return 0;
    Hole*H=&g->h[mv->src];
    Behav beh=(mv->mt==MOVE_R||mv->mt==MOVE_TR)?BEHAV_RED:BEHAV_BLUE;
    int reds=0,blues=0,trans=0;
    if(mv->mt==MOVE_R){ if(H->r==0)return 0; reds=H->r; H->r=0; }
    else if(mv->mt==MOVE_B){ if(H->b==0)return 0; blues=H->b; H->b=0; }
    else if(mv->mt==MOVE_TR){ if(H->t+H->r==0)return 0; trans=H->t; reds=H->r; H->t=H->r=0; }
    else { if(H->t+H->b==0)return 0; trans=H->t; blues=H->b; H->t=H->b=0; }
    int cur=mv->src,last=-1;
    if(trans>0)last=sow_color(g,p,mv->src,beh,'T',trans,cur),cur=last;
    if(reds>0) last=sow_color(g,p,mv->src,BEHAV_RED,'R',reds,cur),cur=last;
    if(blues>0)last=sow_color(g,p,mv->src,BEHAV_BLUE,'B',blues,cur),cur=last;
    if(last>=0)apply_capture(g,p,last);
    g->to_play=(Player)(1-g->to_play);
    return 1;
}