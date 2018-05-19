/* Copyright 2002 Eli Cherniavsky <locutos@sf.net> */

/* Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE. */

#include <curses.h>             /* for WINDOW, etc. variables. */
#include <panel.h>
#include <menu.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <ctype.h>
#include <string.h>

#include <stdarg.h>
#include <unistd.h>             /* chdir(), unlink() */
#include <dirent.h>             /* readdir() et al */
#include <sys/stat.h>           /* mkdir(), stat() */
#include <sys/time.h>           /* select(), gettimeofday() */

/* functions */

/* macro */
/* command defines */

#define KEY_ESC 27
#define CTRL_G  7

#define sw(format, args...) (update_panels(), doupdate(), \
                             wscanw(cur_win, format , ## args))
#define gw()     (update_panels(), doupdate(), wgetch(cur_win))
#define mw(y, x) wmove(cur_win, y, x)
#define cw()     werase(cur_win)
#define yxw(y, x) getyx(cur_win, y, x)
#define ww()     (getmaxyx(cur_win, dummy1, dummy2), dummy2)
#define sa(attr) wattrset(cur_win, attr)
#define se()     wstandend(cur_win)
/* add-string-formatted */
#define adsf(var, format, args...) var += sprintf(var, format , ## args)
#define fpf fprintf
#define fsf fscanf              /* cool shortcut ahh...*/

/* macro functions */

#define CANCEL(ret)  do { cancel = 1; return ret; } while(0)
#define IFCAN(ret)   if(cancel == 1) return ret;
#define NOTYET(what) message(what " *c4{not yet implemented.}")

/* system */

void setup();
void chdir_to_save();           /* changing the directory to the save dir */
void back_to_orig_cwd();
int  save();
void bigbang();
int  load();
void quit();
void randmap();
void newgame();
void custom();

void  salloc(int len);          /* make sure there is place to append len chars */
void  sappend(char *string);
int   spf(char *format, ...);   /* sappend with formatting */
char *sfinish();

void mdl(float dl);             /* my delay - delaying the game a bit... */
void display_fleet_table();

/* playing */

void   play();                  /* command interpretor */
void   endturn();
void   events();                /* battles, ships, production ... */
void   help();
void   help_key();
void   phelp(int key);
int    check(int user);         /* checks if the player controls planets */
char  *pname(int user);         /* returns the name of the player in color */
char  *elname(int planet);      /* returns the name of the planet in color */
void   showname(int user);
void   showstat();              /* shows some statistics about the game */
void   checkstat();  /* checks if the player is dead -> no planets or ships */
void   winner(int player);
void   build_defence(int planet);
void   gcp();

/* ships and planets */

double distance(int planet1, int planet2); /* distance between two planets */
void   send(int from, int to, int count, int peaceful);
void   battle(double sh, int ow, double kl, int trg); /* the formula of a battle */
void   showplanet(int whatplanet);
void   transfer();              /* transfers ships to players other then yourself */
void   autosend(int user);

/* UI system */

void del_windows();
void init_windows();
void popup_noanim(int y, int x);
void popdown_noanim();
void popanim(int y, int x, int direction);
void popup(int y, int x);
void popdown();
int  popup_ok();                /* decides if it's a good idea to use a popup */

/* UI help functions (for other UI functions */

void  popup_len(int len);
void  message(char* text);
void  cmessage(int color, char *text); /* color message */
char* my_title(char *string, int width, int mode);
void  my_scroll(char *string);
char* string_scroll(char *string, int start, int length);
void  init_players(int num_players);
void  color_addstr(char *string);
int   color_strlen(char *string);
char* str_notyet();
void  pw(char *format, ...);
 
/* High level UI */

void  map();                    /* a graphical example of the map */
int   readyn(char *prompt);     /* reading yes/no returns true/false */
int   readships(int max);
char *readfile(char *prompt, int mustexist);
int   readplanet(char *prompt, int onlyyou);
void  select_map();
void  credits();
char *rainbow(char *string);
void  mainmenu();               /* start-menu, in game menu */
void  options();
int   check_option(int what_option);
void  display_options();
void  invert_option(int what);
void  init_options();

/* AI functions */

void  ai(int player);
void  ai2(int player);
void  distcalc(int mode);
int   power();
int   avkill();
int   supplyline(int planet, int range); /* finds the nearset friendly planet
                                         * a given range of a planetwithin */
int   check_neutral();          /* checks if there are any neutral players */
void  weak_planet();
float formula(int planet);      /* the formula that caculates the strength  */
int   check_ships(int planet);
int   check_chance(int planet, int planet2); /* checks the chance of a succesful
                                             * inavsion*/

/* graphical functions */

int flag(int repeat);

/* game defines very important */

#define DEF_POP 5000

/* size defines */

#define YSIZE    128
#define XSIZE    128
#define PLANETS  72             /* imposed by planet names (see setup()) */
#define FLEETS   4096
#define PLAYERS  32
#define MAX_NAME 12             /* imposed by different tables, etc.
                                   comments with MAX_NAME mark the places. */
/* Variables */

#ifdef VARS_HERE
#define E
#define IS(x) = x
#else
#define E extern
#define IS(x)
#endif

/* UI defines */

#define SCROLL_WIDTH  70
#define SCROLL_HEIGHT 22
#define SCROLL_PAGE   15
#define TITLE         0
#define SUB_TITLE     1
#define SUB2_TITLE    2


/* for macros */

int dummy1;
int dummy2;

/* arguments */

int debug IS(0);

/* game var. */

E char version[] IS({
   "0.99.5"                     /* @RELEASE@ */
});

E char orig_cwd[256];           /* the original cwd */

E int ysize, xsize;

E int space[YSIZE][XSIZE];      /* the final frontier */
E int game IS(0);               /* 1 -> active game, 0 -> no active game */
E int isplay IS(0);               /* checks if you are in function play */

E int cancel IS(0);
/* stops things from being done if 1 */
/* My very first struct, options for the
 * options function */

/* debug mode */

E int aitype[PLAYERS];

/* Planets go from 1 to planets inclusive!  Planet 0 means a blank space */

typedef struct 
{
   int defence;                 /* the amount of defences (1-5) */
   int ptime;                   /* the time that it takes to build a base */
   int morale;                  /* the morale of the planet (0..100) */
   int old_morale;              /* morale in previos turn (== morale if unknown) */
   int new_morale;              /* new morale that will be applied in the end
                                   of the turn */
} Planet;

E Planet plnt[PLANETS];

E int    planets;
E int    x[PLANETS], y[PLANETS]; /* coordinates of a given planet */
E int    owner[PLANETS];        /* the owner of the planet */
E int    prod[PLANETS];         /* production (ships/turn) on each planet */
E int    cur_prod[PLANETS];     /* the production of each turn */
E int    ships[PLANETS];        /* the amount of ships on current planet */
E double kratio[PLANETS];       /* kill ratio of ships going from each planet */
E char   pnames[PLANETS];       /* the names of the planets in a single character */
E int    population[PLANETS];   /* the population of each planet */
E int    growth[PLANETS];       /* the growth factor of the population every turn */
E int    defence[PLANETS];      /* the defence buidings on each planet */

E int    fowner[FLEETS];
E int    fsender[FLEETS];
E int    fships[FLEETS];        /* 0 means this fleet is unused */
E double fkill[FLEETS];
E int    ftarget[FLEETS];
E int    fturn[FLEETS];

/* auto send */
E int atarget[PLANETS];         /* target to send to */
E int aships[PLANETS];          /* ships to send */

/* Others */
E int counter;          /* counts the turns since the beginning of the game */

#define MAP_NORMAL 0
#define MAP_SHIPS  1
#define MAP_PROD   2
#define MAP_MORALE 3
#define MAP_MAX    3

E int map_mode IS(0);

/* UI */

/* options struct */

struct
{
   int colors;                  /* 1 - color enabled (FIXME: currently ignored) */
   int animations;              /* 1 - animations enabled */
   int fast_anim;               /* 0 - slow, 1 - fast */
   int slow_term;               /* 1 - conservative visual effects ;-) */
} option;

/* Players go from 0 to players inclusive, where planet 0 is neutral. */

E int players;                  /* tot. amount of players */
E int player;                   /* the current player */
E char names[PLAYERS][MAX_NAME] IS( /* the names of the players */
{
   "neutral"                    /* rest zeroed */
}
);

E int computer[PLAYERS] IS(
{
   0
}
);

/* statistics of players */
E int    totplanets[PLAYERS];   /* amount of planets a player controls */
E int    totships[PLAYERS];        /* amount of ships a player has,
                                   both on planets and in flight */
E double average[PLAYERS];      /* the average kill ratio for players */
E double av_morale[PLAYERS];    /* the average morale on all his planets */
E int battles[PLAYERS];         /* the amount of battles that a player fought */
E int won[PLAYERS];        /* the total amount of battles won by the player */
E int lost[PLAYERS];      /* the total amount of battles lost by the player */
E int dead[PLAYERS];            /* a flag that shows if they lost or not */
E double weak[PLANETS];

/* Attribute stuff */
E int colors IS(1);

E char psymbol[PLAYERS];
E int  pattr[PLAYERS];
E int  use_psymbol;               /* 1 if you need the symbols; if 0 all are spaces. */

/* System stuff */
#define WINSTACK 10

E int stacktop IS(0);

E WINDOW *box_win;
E WINDOW *map_win;
E WINDOW *inp_win;
E WINDOW *cur_win_stack[WINSTACK];
E WINDOW *border_win_stack[WINSTACK];
E PANEL  *box_panel;
E PANEL  *map_panel;
E PANEL  *inp_panel;
E PANEL  *pop_panel_stack[WINSTACK];

#define cur_win    cur_win_stack[stacktop]
#define border_win border_win_stack[stacktop]
#define pop_panel  pop_panel_stack[stacktop]

#define STRINGS_SIZE 8192

E char strings[STRINGS_SIZE];
E int shead IS(0), stail IS(0);

E char events_msg[STRINGS_SIZE]; /* events() and battle() report goes here */
E char *emsg;                   /* current place to append to in events_msg */

/* ai variables */

E int closeplanet[PLANETS];
E int strong[PLAYERS];


#undef E
