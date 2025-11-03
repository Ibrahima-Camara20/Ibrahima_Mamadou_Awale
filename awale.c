// awaleX.c — Version jouable dans le terminal
#include <stdio.h>
#include <string.h>
#include <ctype.h>

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

static int wrap(int i){ return (i+NHOLES)%NHOLES; }
static int hole_num(int idx){ return idx+1; }
static int is_owned(Player p,int idx){ int n=hole_num(idx); return (p==P1)?(n%2==1):(n%2==0); }
static int sum_hole(const Hole*h){ return h->r+h->b+h->t; }
static int sum_board(const Game*g){ int s=0;for(int i=0;i<NHOLES;++i)s+=sum_hole(&g->h[i]);return s;}
static void init_game(Game*g){ for(int i=0;i<NHOLES;++i)g->h[i]=(Hole){2,2,2}; g->cap[0]=g->cap[1]=0; g->to_play=P1; g->last_dst=-1; }

static void print_board(const Game*g){
    puts("");
    for(int i=0;i<NHOLES;++i){
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

static int next_drop_idx(Player p,int src,Behav beh,int cur){
    int idx=wrap(cur+1);
    if(idx==src) idx=wrap(idx+1);
    if(beh==BEHAV_RED) return idx;
    while(is_owned(p,idx)||idx==src){ idx=wrap(idx+1); if(idx==src)idx=wrap(idx+1); }
    return idx;
}

static int sow_color(Game*g,Player p,int src,Behav beh,char color,int count,int start){
    int dst=start;
    for(int i=0;i<count;i++){
        dst=next_drop_idx(p,src,beh,dst);
        Hole*h=&g->h[dst];
        if(color=='R')h->r++; else if(color=='B')h->b++; else h->t++;
    }
    return dst;
}

static void apply_capture(Game*g,Player p,int last){
    int idx=last, gained=0;
    while(idx>=0){
        Hole*h=&g->h[idx];
        int s=sum_hole(h);
        if(s==2||s==3){ gained+=s; *h=(Hole){0,0,0}; idx=wrap(idx-1); }
        else break;
    }
    g->cap[p]+=gained;
}

typedef enum { CONTINUE,G1WIN,G2WIN,DRAW } State;
static State check_end(const Game*g){
    if(g->cap[P1]>=49)return G1WIN;
    if(g->cap[P2]>=49)return G2WIN;
    if(sum_board(g)<10){
        if(g->cap[P1]>g->cap[P2])return G1WIN;
        if(g->cap[P2]>g->cap[P1])return G2WIN;
        return DRAW;
    }
    return CONTINUE;
}

typedef struct{int src;MoveType mt;}ParsedMove;
static int parse_move(const char*s,ParsedMove*out){
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

static int play_move(Game*g,const ParsedMove*mv){
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
