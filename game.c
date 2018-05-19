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

#define VARS_HERE
#include "zis.h"

/* game defines */

int main(int argc, char *argv[]) 
{
   int   ans           = 0;     /* don't touch */
   char *usage         = "usage: zis [-d] [--help] [--version]\n";
   char *help_text_arg = 
      "zis - Zionist In Space\n"
      "A turn based strategy game\n"
      "options:\n\n"
      "-d           debugging mode gives acsess to certain debugging features.\n"
      "--help       shows this text.\n"
      "--version    shows the current version of the game.\n"
      "--frob       a frob....\n"
      "--sf         undocumented (yet)\n";
   char *version_text  = 
      "ZIS - Zionist In Space 0.99.5\n\n" /* @RELEASE@ */
      "written by Eli Cherniacsky with the help of Beni Cherniavsky\n"
      "and Barak Katzir\n\n"
      "A turn based strategy game base on GNUlactic Konquest\n";
   int   i;

   for(i = 1; i < argc; i ++)
   {
      if(strcmp(argv[i], "-d") == 0)
      {
         debug = 1;
      }
      else if(strcmp(argv[i], "--help") == 0)
      {
         printf("%s", usage);
         printf("%s", help_text_arg);
         exit(0);
      }
      else if(strcmp(argv[i], "--version") == 0)
      {
         printf("%s", version_text);
         exit(0);
      }
      else if(strcmp(argv[i], "--frob") == 0)
      {
         char *frob = 
            "Three to the kings of Elf who rule under the sky,\n"
            "Seven to the Dwarf lords in their halls of stone,\n"
            "Nine to mortal man doomed to die.\n"
            "One Frob to the Dark frob in his dark throne,\n"
            "In the land of Redmond where the hackers die.\n"
            "One Frob to rule them all one Frob to find them.\n"
            "One Frob to bring them all and in the darkness FROBNICATE them,\n"
            "In the land of Redmond where the hackers Die\n\n"
            "The author of the game, based on the lord of the "
            "frobs ehh rings by J.R.R Tolkien.\n\n"
            "One by one they fell to the power of the Frob...\n\n"
            "never mind me I'm just plain FROBNICATED...\n\n";
         printf("%s", frob);
         exit(0);
      }
      else if(strcmp(argv[i], "--gcp") == 0)
      {
         ans = 1;
      }
      else if(strcmp(argv[i], "--sf") == 0)
      {
         printf("Segmentation fault\n");
         exit(0);
      }
      else
      {
         printf("illegal argument:\n"
                "%s", usage);
         exit(0);
      }
   }

   getcwd(orig_cwd, 256);            /* the original cwd */


   srand(time(NULL));

#ifdef __DJGPP__
   system("set50");             /* before initializing curses */
#endif

   initscr();                   /* start curses mode */
   cbreak();                    /* line buffering disabled */
   echo();                      /* echo input while we getch */

   if(has_colors() == TRUE)
   {
      start_color();            /* Start color */
      init_pair(1, COLOR_CYAN, COLOR_BLACK);
      init_pair(2, COLOR_GREEN, COLOR_BLACK);
      init_pair(3, COLOR_YELLOW, COLOR_BLACK);
      init_pair(4, COLOR_RED, COLOR_BLACK);
      init_pair(5, COLOR_BLUE, COLOR_BLACK);
      init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
      init_pair(7, COLOR_WHITE, COLOR_BLACK);
      colors = 7;
   }
   init_players(PLAYERS - 1);
   
   xsize = COLS / 3 - 1;
   ysize = LINES - 3 - 8;       /* 8 lines for inp_win */
   if(xsize > ysize * 1.5)      /* balanced sizes by default */
   {
      xsize = ysize * 1.5;
   }

   init_windows();
   init_options();
   credits();
   if(ans)
   {
      gcp();
   }
   while(1)
   {
      mainmenu();
   }

  /* save(), endwin() are is called from quit() */
   return 0;                    /* make gcc happy */
}

void back_to_orig_cwd()
{
   if(chdir(orig_cwd) != 0)
   {
      message("man the original cwd is gone ... \n");
   }
}


void chdir_to_save()
{
   char savedir[512];
   char *s;


   s = getenv("HOME");
   if(s != NULL)
   {
      sprintf(savedir, "%s/.zis-saves", s);
   }
   else
   {
      sprintf(savedir, "save");
   }

   if(!(chdir(savedir) == 0 ||
        (mkdir(savedir, 0777) == 0 &&
         chdir(savedir) == 0)))
   {
      char cwd[256] = "";

      getcwd(cwd, 256);
      pw("Could not find/create saves directory `%s'.\n"
         "The save/load file names will be relative\n"
         "to the current directory `%s'.\n"
         "Press any key.",
         savedir, cwd);
      gw();
   }
}



void setup()              
{
   int i, max_players;

   bigbang();

   popup(10, 60);

   while(1)
   {
      cw();
      if(readyn("do you want to change the size of the map [y/n]: "))
      {
         while(1)
         {
            pw("vertical size of the map (current - %d): ", ysize);
            sw("%d", &ysize);
            pw("horizontal size of the map (current - %d): ", xsize);
            sw("%d", &xsize);
            if(readyn("are you sure [y/n]: "))
            {
               break;
            }
         }
         del_windows();
         init_windows();
         map();
      }
      max_players = ysize * xsize;

      pw("number of *c7{players}: ");
      sw("%d", &players);
      if(players < PLAYERS && players < max_players)
      {
         break;
      }
      if(players > PLAYERS && PLAYERS > max_players)
      {
         pw("no more then %d players !!\n", PLAYERS);
      }
      else if(players > max_players && max_players < PLAYERS)
      {
         pw("there is no place for more then %d !!\n", max_players);
      }
   }

   init_players(players);
   for(i = 1; i <= players; i++)
   {
      names[i][0] = '\0';    /* clear the strings */
   }

   map();

   for(i = 1; i <= players; i ++)
   {
      computer[i] = readyn("is the player a *c7{computer}[y/n]: ");

      if(debug == 1)
      {
         pw("debugging mode...\n");
         if(computer[i])
         {
            pw("type of ai: ");
            sw("%d", &aitype[i]);
         }
      }
      pw("name of player *c7{%d}: ", i);
   name_scanning:
     /* !!! hard coded MAX_NAME !!! */
      if(sw("%11[^\n]", names[i]) != 1)
      {
         sprintf(names[i], "chicken %d", i);
      }
      else if(strcmp(names[i], "C.S.") == 0)
      {
         sprintf(names[i], "SHOW-OFF");
      }
      else if(strcmp(names[i], "genkin") == 0 || strcmp(names[i], "jeri") == 0)
      {
         pw("%s", rainbow("GO AWAY GENKIN, ANTI-JEWS LIKE SHOULD NOT"
                          "PLAY ZIS."));
         gw();
         pw("\n"
            "another name for player %d:", i);
         goto name_scanning;
      }
      map();
   }

   while(1)
   {
      pw("number of *c7{planets}: ");
      sw("%d", &planets);
      if(planets > PLANETS)
      {
         
         pw("number of planets must be below *c7{%d}!  ", PLANETS);
      }
      else if(planets > ysize * xsize)
      {
         pw("there is only place for %d planets in the map!  ",
            ysize * xsize);
      }
      else if(planets < players - 1) /* the -1 is to counter the neutral */
      {
         pw("you must be joking (planets should be"
            " bigger or equal to players)!  ");
      }
      else
      {
         break;
      }
   }

   for(i = 1; i <= planets; i ++)
   {
      char sequence[] =
         "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
         "0123456789"
         "!@#$%^&*()"
         "abcdefghijklmnopqrstuvwxyz";

      pnames[i] = sequence[i - 1]; /* i = 1 --> 'A' */
   }

   popdown();

   randmap();                /* to generate one before asking */
   select_map();
}

void init_windows()
{
   int mapsize = ysize + 3;

   if(mapsize > LINES - 4)
   {
      mapsize = LINES - 4;
      message("Map too high to fit.  You'd better choose a smaller map.");
   }

   box_win = newwin(0, 0, 0, 0);
   box(box_win, 0, 0);
   mvwaddch(box_win, mapsize + 1, 0, ACS_LTEE);
   whline(box_win, ACS_HLINE, COLS - 2);
   mvwaddch(box_win, mapsize + 1, COLS - 1, ACS_RTEE);
   map_win = newwin(mapsize, COLS - 2, 1, 1);
   inp_win = newwin(LINES - 3 - mapsize, COLS - 2, mapsize + 2, 1);
   cur_win_stack[0] = inp_win;
   box_panel = new_panel(box_win);
   map_panel = new_panel(map_win);
   inp_panel = new_panel(inp_win);
   bottom_panel(inp_panel);     /* don't obscure active popups */
   bottom_panel(map_panel);
   bottom_panel(box_panel);

   scrollok(inp_win, TRUE);     /* scroll when getting to bottom */
   keypad(inp_win, TRUE);       /* to get F1, F2, left etc.. */
}

void del_windows()
{
   del_panel(box_panel);
   delwin(box_win);
   del_panel(inp_panel);
   delwin(inp_win);
   del_panel(map_panel);
   delwin(map_win);
}

void init_players(int num_players)
{
   int i;
   int symbol = 0, color = 0;

   players = num_players;

   pattr[0]   = A_NORMAL;
   psymbol[0] = ' ';
   use_psymbol = 0;

   for(i = 1; i <= players; i++)
   {
      char pchars[] = " *_)@=^%#;~!????????????????????????????????????????";

      color ++;
      if(color > colors)
      {
         color = 1;
         symbol ++;
         use_psymbol = 1;
      }

      pattr[i] = COLOR_PAIR(color) | A_BOLD;
      psymbol[i] = pchars[symbol];
   }
   werase(map_win);
}

int save() /* save a game */
{
   int i;
   FILE *f;
   char FileName[255];
   char *s;
      
   s = getenv("USER");
   sprintf(FileName, "/sbbs/doors/zis/%s.save", s);
   f = fopen(FileName, "w");
   if(f == NULL)
   {
      pw("error in opening file !!\n");
      return 0;
   }
  /* version indication */
   fpf(f, "version=%s\n", version);

   fpf(f, "current player=%d\n", player);
   fpf(f, "turn %d\n", counter);
   fpf(f, "mapsize x=%d, y=%d\n", xsize, ysize);

   for(i = 1; i <= planets; i++)
   {
      fpf(f,
          "planet pos=%d,%d name=%c owner=%d prod=%d ships=%d"
          " kill=%f defence=%d morale=%d aships=%d atarget=%d",
          x[i], y[i], pnames[i], owner[i], prod[i], ships[i],
          kratio[i], plnt[i].defence, plnt[i].morale, aships[i], atarget[i]);
   }

   for(i = 0; i < FLEETS; i ++)
   {
      if(fships[i] != 0)
      {
         fpf(f, "fleet fships=%d fowner=%d fsender=%d ftarget=%d fkill=%f fturn=%d\n",
             fships[i], fowner[i], fsender[i], ftarget[i], fkill[i], fturn[i]);
      }
   }

   for(i = 1; i <= players; i++)
   {
      fpf(f, "user battles=%d won=%d lost=%d computer=%d aitype=%d name=%s\n",
          battles[i], won[i], lost[i], computer[i], aitype[i], names[i]);
   }

   fclose(f);

   
   return 1;
}

void bigbang()                   /* wipes out all matter in the universe */
{
   int i, j;

   for(i = 0; i < YSIZE; i ++)
   {
      for(j = 0; j < XSIZE; j ++)
      {
         space[i][j] = 0;
      }
   } 
  /* note that we don't reset `planets' - that would interfere with randmap() */

   for(i = 0; i < FLEETS; i ++)
   {
      fships[i] = 0;
   }

   for(i = 0; i < PLANETS; i ++)
   {
      aships[i] = 0;
   }

   for(i = 1; i < PLAYERS; i++)
   {
      battles[i] = won[i] = lost[i] = 0;
   }

   player = 1;               /* 0 is nuetral */
   counter = 0;
}

void gcp()
{
   char string[128];
   
   popup_noanim(25, 80);
   pw("%s\n\n", my_title("ZIS: GCP version 1.00", ww(), TITLE));
   pw("Hello genkin, welcome to GCP...\n\n"
      "put here the sentence that you would like to "
      "process:\n\n"
      "sentence here: ");
   sw("%[^\n]", string);
   pw("\n\n"
      "procesing string ... complete, space for result\n\n");
   gw();
   pw("%s\n", rainbow(string));
   gw();
   pw("\n\n");
   pw("any key to exit");
   gw();
   pw("\n\n");
   pw("By the way GCP stands for Genkin-Color-Program\n");
   gw();
   popdown_noanim();
   endwin();
   exit(0);
}

int load() /* load previusly saved game */
{
   int   i;
   char fversion[64] = "(unspecified)";
   char FileName[255];
   char *s;
   
   s = getenv("USER");
   sprintf(FileName, "/sbbs/doors/zis/%s.save", s);

   FILE *f           = fopen(FileName, "r");
   if(f == NULL)
   {
      pw("error in opening file !!\n");
      return 0;
   }

  /*
   * This structure will only work if all lines begin with different
   * characters.  Otherwise one fscanf will eat part of a line that's not for
   * him and the right fscanf will not recognize it since the start is missing
   * and the parsing will stop there.  Also it demands correct order...
   */

  /* version loading */
   if(fsf(f, "version=%s\n", fversion) != 1 ||
      strcmp(fversion, version) != 0)
   {
      char wrong[200];
      sprintf(wrong, "incompatible version *c4{%s}\n", fversion);
      message(wrong);
      fclose(f);
      return;
   }
   
   bigbang();

  /* singleton lines */
   fsf(f, "current player=%d\n", &player);
   fsf(f, "turn %d\n", &counter);
   fsf(f, "mapsize x=%d, y=%d\n", &xsize, &ysize);
   del_windows();
   init_windows();

  /* planet things */
   for(i = 1; i < PLANETS; i++)
   {
      int ok;

      ok = fsf(f,
               "planet pos=%d,%d name=%c owner=%d prod=%d ships=%d"
               " kill=%lf defence=%d morale=%d aships=%d atarget=%d\n",
               &x[i], &y[i], &pnames[i], &owner[i], &prod[i], &ships[i],
               &kratio[i], &plnt[i].defence, &plnt[i].morale, &aships[i], &atarget[i]);
      plnt[i].old_morale = plnt[i].morale; /* display unknown change as 0 */
      if(ok != 11)
      {
         break;
      }
      space[y[i]][x[i]] = i;
   }
   planets = i - 1;

  /* fleet things */
   for(i = 0; i < FLEETS; i ++)
   {
      int ok;

      ok = fsf(f,
               "fleet fships=%d fowner=%d fsender=%d ftarget=%d"
               " fkill=%lf fturn=%d\n",
               &fships[i], &fowner[i], &fsender[i], &ftarget[i],
               &fkill[i], &fturn[i]);
      if(ok != 6)
      {
         break;
      }
   }

   for(i = 1; i < PLAYERS; i ++)
   {
      int ok;

     /* !!! hard coded MAX_NAME !!! */
      ok = fsf(f, "user battles=%d won=%d lost=%d computer=%d aitype=%d name=%11[^\n]\n",
               &battles[i], &won[i], &lost[i], &computer[i], &aitype[i], names[i]);
      if(ok != 6)
      {
         break;
      }
   }
   players = i - 1;
   init_players(players);

   fclose(f);
   
   return 1;
}

void play() /* game main loop */
{
   int answer = ' ';
   int run_ai = 1;

   isplay = 1; /* all questions in dialogs */ 

   pw("Type `h' for help.\n");

   while(game)
   {
      cancel = 0;
      
      map();
      pw("player ");
      showname(player);
      pw(": ");

      if(run_ai)
      {
         run_ai = 0;
         if(computer[player] == 1)
         {
            if(debug)
            {
               pw("press any key to run AI...");
               gw();
               pw("\n");
               if(aitype[player] == 1)
               {
                  ai2(player);
               }
               else
               {
                  ai(player);
               }
               continue;
            }
            else
            {
               ai(player);
               endturn();
            }
         }
      }

      answer = gw();
      pw("\n");
      
      switch(answer)
      {
      case 'F':
         {
            if(debug)
            {
               display_fleet_table();
            }
            break;
         }
      case 'Q': case 'q':
         {
            quit();
            break;
         }
      case 'e':
         {
            endturn();
            run_ai = 1;
            break;
         }
      case 'p':
         {
            showplanet(readplanet("Which planet: ", 0));
            break;
         }
      case 'd':
         {
            double dist = distance(readplanet("One planet: ", 0),
                                   readplanet("Another planet: ", 0));

            pw("%.4f (%d moves)\n", dist, (int)dist);
            break;
         }
      case 's':
         {
            if(check(player))
            {
               int from = readplanet("Source planet: ", 1);
               int to = readplanet("Destination planet: ", 0);
               int count = readships(ships[from]);

               if(count != 0)
               {
                  send(from, to, count, 0);
               }
            }
            else
            {
               pw("you do not control any planets.\n");
            }
            break;
         }
      case 't':
         {
            if(check(player))
            {
               transfer();
            }
            else
            {
               pw("you do not control any planets.\n");
            }
            
            break;
         }
      case 'b':
         {
            if(check(player))
            {
               int planet = readplanet("where would you like to build a base: "
                                       , 1);
               build_defence(planet);
            }
            else
            {
               pw("you do not control any planets.\n");
            }
            break;
         }
      case 'h': case '?': case KEY_F(1): case KEY_HELP:
         {
            help();
            break;
         }
      case 'H':
         {
            help_key();
            break;
         }
      case 'f':
         {
            showstat();
            break;
         }
      case 'm':
         {
            map_mode ++;
            if(map_mode > MAP_MAX)
               map_mode = 0;
            break;
         }
      case 'C':
         {
            credits();
            break;
         }
      case 'a':
         {
            if(!(check(player)))
            {
               pw("you do not control any planets.\n");
            }
            else
            {
            
               int from = readplanet("Source planet: ", 1);
               int to   = readplanet("Destination planet: ", 0);
               int count;

               if(cancel == 1)
               {
                  break;
               }
               
               pw("(type `0' to stop the autosend from this planet)\n");
               count = readships(ships[from]);

               aships[from]  = count;
               atarget[from] = to;
            }
            break;
         }
      case KEY_ESC: case KEY_F(10):
         {
            isplay = 0;
            return;
         }

        /* The rest is deprecated by the menu.  Will be removed at some point. */

      case 'S':
         {
            if(readyn("are you sure you want to save the game [y/n]: "))
            {
               if(save())
               {
                  pw("saved sucessfully\n");
               }
            }
            break;
         }
      case 'L':
         {
            if(readyn("are you sure you want to load [y/n]: "))
            {
               load();
            }
            break;
         }
      case 'R':
         {
            setup();
            break;
         }
      case 'G':
         {
            pw("Warning!  This will destroy the current game !!\n");
            select_map();
            break;
         }
      default:
         {
            pw("no such command `%s' -> try `h' for help.\n", keyname(answer));
            break;
         }
      }
   }
   isplay = 0;
}


void endturn()
{
   int oldplayer = player;

   if(check(player))
   {
      autosend(player);
   }
   else
   {
      pw("you do not control any planets )-:");
   }
   pw("Press any key.");
   gw();
   werase(inp_win);

   pw("player ");
   showname(player);
   pw(" ended his turn.\n");

   checkstat();

   do
   {
      player ++;

      if(player > players)      /* at most once during the loop */
      {
         player = 1;
         events();
      }
   }
   while(dead[player] == 1);

   if(player == oldplayer)
   {
      winner(player);
   }
}


void winner(int player)
{
   char *text_comp = "ALAS FOR HUMANS, COMPUTER ";
   char *text      = ", YOU ARE THE RULER OF THE GALAXY.\n";

   popup_len((computer[player] ? strlen(text_comp) : 0) +
             strlen(pname(player)) + 1 + strlen(text)); /* +1 for the psymbol */

   if(computer[player])
   {
      pw("%s", text_comp);
   }
   showname(player);
   pw("%s", text);
   NOTYET("winning movie");
   gw();
   popdown();
   if(!readyn("Type y to stay and look around (n resets the game) [y/n]: "))
   {
      game = 0;
      werase(map_win);
      werase(inp_win);
   }
}


void events()
{
   int i;

   counter ++;

   emsg = events_msg;

   spf("turn *c7{%d} finished !!", counter);
   emsg += sprintf(emsg, "%s", my_title(sfinish(), SCROLL_WIDTH, TITLE));
   emsg += sprintf(emsg, "\n");
  /* production of ships on planets */
   for(i = 1; i <= planets; i ++)
   {
      if(owner[i] == 0)
      {
        /* for neautral planet ships growth */
         ships[i] += prod[i] / 2;
      }
      else
      {
         cur_prod[i] = prod[i] * (100 + plnt[i].morale) / 200; /* the algoritm */
         
         ships[i] += cur_prod[i];                              /* this stupid thing
                                                            * is being done so
                                                            * that showplanet()
                                                            * will know the
                                                            *  prod[i] */
      }
   }

  /* inialize new_morale to morale, save old for showplanet() */
   for(i = 1; i <= planets; i ++)
   {
      plnt[i].new_morale = plnt[i].old_morale = plnt[i].morale;
   }
   emsg += sprintf(emsg, my_title("construction", SCROLL_WIDTH, SUB_TITLE));
   emsg += sprintf(emsg, "\n");
  /* defence construction on planets */
   for(i = 1; i <= planets; i ++)
   {
      Planet *p = &plnt[i];
      
      if(p -> ptime > 1)
      {
         p -> ptime --;
      }
      else if(p -> ptime == 1)
      {
         p -> ptime = 0;
         p -> defence ++;
         emsg += sprintf(emsg, "defence construction finished on planet %s \n", elname(i));
         if(p -> defence > 1)
         {
            emsg += sprintf(emsg, "there are now %d defence bases on that planet.\n", p -> defence);
         }
         else
         {
            emsg += sprintf(emsg, "there is now a single defecne base on the planet.\n");
         }
      }
   }

   emsg += sprintf(emsg, my_title("fleets and battles", SCROLL_WIDTH, SUB_TITLE));
   emsg += sprintf(emsg, "\n");
  /* arriving fleets */
   for(i = 0; i < FLEETS; i ++)
   {
      if(fships[i] != 0 && fturn[i] <= counter)
      {
         if(fowner[i] == owner[ftarget[i]])
         {
            if(fsender[i] != fowner[i])
            {
               emsg += sprintf(emsg, "gift fleet arrived from ");
               showname(fsender[i]);
               emsg += sprintf(emsg, " on planet %c :-)\n", pnames[ftarget[i]]);
            }
            else
            {
               emsg += sprintf(emsg, "fleet arrived on planet %c\n", pnames[ftarget[i]]);
            }
            ships[ftarget[i]] = ships[ftarget[i]] + fships[i];
         }
         else
         {
            battle(fships[i], fowner[i], fkill[i], ftarget[i]);
         }
         fships[i] = 0;      /* destroy the fleet */
      }
   }

  /* population growth */
  /*
   for(i = 1; i <= planets; i ++)
   {
      growth[i] = (population[i] + 249) / 250;
      population[i] += growth[i];
   }
  */
   
  /* morale changes are made here */
   for(i = 1; i <= planets; i ++)
   {
      plnt[i].morale = plnt[i].new_morale;
     /* exponential restoration, 0 -> 50 in 3 moves */
      plnt[i].morale += (104 - plnt[i].morale) / 5;
   }

   my_scroll(events_msg);
}


double distance(int planet1, int planet2) /* distance in turns between planets */
{
   double deltax = x[planet1] - x[planet2];
   double deltay = y[planet1] - y[planet2];

   return sqrt(deltax * deltax + deltay * deltay) / 2; /* sqrt == square root */
}


void send(int from, int to, int count, int peaceful)
{
   int i;

   IFCAN(/**/);

   for(i = 0; i < FLEETS; i ++)
   {
      if(fships[i] == 0)
      {
         fships[i]   = count;
         ships[from] = ships[from] - count;
         fkill[i]    = kratio[from];
         if(peaceful == 1)
         {
            fowner[i] = owner[to];
         }
         else
         {
            fowner[i] = player;
         }
         fsender[i] = player;
         ftarget[i] = to;
         fturn[i]   = (int)distance(from, to) + counter;
         if(debug == 1)
         {
            pw("player %s sending from %c to %c %d ships.\n", pname(player), pnames[from], pnames[to], count);
         }
         return;
      }
   }
   pw("Sending failed - FLEETS limit exceeded!\n");
}


void transfer()
{
   int from  = readplanet("Source planet: ", 1);
   int to    = readplanet("Destination planet: ", 0);
   int count = readships(ships[from]);

   if(check(owner[from]))
   {
      if(count != 0)
      {
         send(from, to, count, 1);
      }
   }
   else
   {
      pw("you do not control any planets )-;\n");
   }
}


void autosend(int user)
{
   int i;

   for(i = 0; i < planets; i ++)
   {
      if(aships[i] != 0 && owner[i] == user)
      {
         if(aships[i] > ships[i])
         {
            showname(owner[i]);
            pw(", you do not have enough ships to autosend from planet %c\n"
               "fewer ships will be sent\n", pnames[i]);
            send(i, atarget[i], ships[i], 0);
         }
         else
         {
            showname(owner[i]);
            pw(", auto sending from planet %c ...\n", pnames[i]);
            send(i, atarget[i], aships[i], 0);
         }
      }
   }
}


void battle(double sh, int ow, double kl, int trg)
{
   int    oldowner = owner[trg];
   double oldsh;                /* backup for morale computation */
   Planet *t = &plnt[trg];
   

   sh = sh * kl;

  /* needed for the showstat(), updating statistics */
   battles[ow] ++;
   battles[owner[trg]] ++;


  /* defence bases go first and they are independent from the other battle */

   if(t -> defence)             /* if any defeces are present at all */
   {
      double def_ships;         /* the defences converted to ships */
     /* every defence of the planet is equivalent to 20 ships with a kill
        ratio of 1 */

      def_ships = t -> defence * 20;
      sh -= def_ships;
      if(sh > 0)
      {
         t -> new_morale *= 1 - 0.05 * t -> defence;
         t -> defence = 0;
         emsg += sprintf(emsg, "all defence bases of planet %s destroyed\n", elname(trg));
      }
      else
      {
         int old_defence = t -> defence;
         
         t -> defence = ceil(-sh / 20);
         emsg += sprintf(emsg, "%d/%d defence bases of planet %s destroyed by %s, attack repelled\n",
                         old_defence - t -> defence, old_defence, elname(trg), pname(ow));
         t -> new_morale *= 1 - 0.03 * (old_defence - t -> defence);
         return;
      }
   }

   oldsh = sh;                  /* ships lost on defences already counted */
   
  /* FIXME: use doubles */
   ships[trg] = ships[trg] * kratio[trg];
   sh = sh - ships[trg];
   if(sh > 0)
   {
      won[ow] ++;               /* statistics */
      lost[oldowner] ++;      /* statistics */

     // population[trg] /= pow(, sh / kl); /* FIXME */

      t -> new_morale *= 0.25;
      
      emsg += sprintf(emsg, "planet %s has fallen to %s\n", elname(trg), pname(ow));

      if(check(ow) == 0)
      {
         emsg += sprintf(emsg, "the fallen empire of %s has risen from it's ashes\n", pname(ow));
      }

      owner[trg] = ow;
      ships[trg] = sh / kl;

      if(check(oldowner) == 0)
      {
         emsg += sprintf(emsg, "the once mighty empire of %s has fallen into ruins\n",
            pname(owner[trg]));
      }
      aships[trg] = 0;
   }
   else
   {
      lost[ow] ++;              /* statistics */
      won[oldowner] ++;         /* statistics */

      ships[trg] = -sh / kratio[trg];
     /* 7% lost per each lost 1/10 of the original fleet (for equal kill
        ratios, otherwise more complex). */
      t -> new_morale *= 1 - 0.7 * oldsh / (ships[trg] + oldsh);
      emsg += sprintf(emsg, "planet %s has held an attack from %s\n", elname(trg), pname(ow));
   }
}


void quit()
{
   if(readyn("EXIT [y/n]: "))
   {
      clear();
      refresh();
      endwin();            /* End curses mode */
      exit(0);
   }
   else
   {
      message("Excellent !!!");
   }
}


void checkstat()
{
   int i;

   for(i = 1; i <= players; i ++) /* clears the arrayes */
   {
      totplanets[i] = 0;
      totships[i] = 0;
   }

   for(i = 1; i <= planets; i ++) /* the total ships of players */
   {
      totplanets[owner[i]] ++;
      totships[owner[i]] += ships[i];
   }

   for(i = 0; i < FLEETS; i ++) /* fleet ships calculations */
   {
      if(fships[i] != 0)
      {
         totships[fowner[i]] += fships[i];
      }
   }
   for(i = 1; i <= players; i ++) /* checks who is dead */
   {
      if(totplanets[i] == 0 && totships[i] == 0)
      {
         dead[i] = 1;
      }
      else
      {
         dead[i] = 0;
      }
   }
   for(i = 1; i <= planets; i ++) /* cleans the average kill ratio */
   {
      average[i] = 0;
   }
   for(i = 1; i <= planets; i ++) /* average kill ratio for players */
   {
      if(owner[i] != 0)
      {
         average[owner[i]] += kratio[i];
      }
   }  
   for(i = 1; i <= players; i ++)
   {
      if(totplanets[i])
      {
         average[i] = average[i] / totplanets[i];
      }
      else
      {
         average[i] = 0;
      }
   }
   for(i = 1; i <= planets; i ++) /* average morale calc */
   {
      if(owner[i] != 0)
      {
         av_morale[owner[i]] += plnt[i].morale;
      }
   }
   
   for(i = 1; i <= players; i ++)
   {
      if(totplanets[i])
      {
         av_morale[i] /= totplanets[i];
      }
   }
}

void randmap()
{
   int i, j, k;

   bigbang();

   for(k = 1; k <= planets;) /* planets positioning */
   {
      i = rand() % ysize;
      j = rand() % xsize;

      if(space[i][j] == 0)
      {
         space[i][j]   = k;
         y[k]          = i;
         x[k]          = j;
         prod[k]       = rand() % 5 + rand() % 5 + rand() % 5;
         ships[k]      = rand() % 2 + rand() % 2 + rand() % 4;
         kratio[k]     = 0.1 +
            rand() * 0.3 / RAND_MAX +
            rand() * 0.3 / RAND_MAX + rand() * 0.3 / RAND_MAX;
         population[k] = 1e4 + rand() % 1000;
         
         if(k <= players)
         {
            owner[k]  = k;
            kratio[k] = 0.5;
            prod[k]   = 10;
            population[k] = 15000;
            growth[k] = 7500;
         }
         else
         {
            owner[k] = 0;
         }

         k++;
      }
   }
}

void help()
{
   char help_text[8192];
   sprintf(help_text, 
           "See `help.html' for more details...\n\n"
           "%s\n"
           "%s\n"
           "This game is a *c1{Turn-Based-Strategy} game. I'ts goal is to\n"
           "conquer the entire map.\n"
           "In the map you have *c1{planets}. they are the thing that you\n"
           "need to conquer. In order to do that you have *c1{ships}.\n"
           "you can send ships from planet to planet (it takes them time to\n"
           "reach) and then they will do battle with the ships that defend\n"
           "the planet. If they will win the planet is yours.\n"
           "The ships are produced automaticly on planets depending on the\n"
           "*c1{morale} of each planet. The morale dtermines\n"
           "the *c1{production} of ships on that planet. The morale is affected\n"
           "by attacks on the planet.\n"
           "each planet has a *c1{Kill ratio}. The kill ratio determines how\n"
           "well the ship will do battle.\n"
           "The kill ratio is generated from the beginning of the game and is\n"
           "constant\n\n"
           "%s"
           "*c1{h}: shows this help.\n\n"
           "*c1{H}: shows a long description for individual commands.\n\n"
           "*c1{F10}, *c1{ESC}: shows the menu,\n\n"
           "*c5{Cancelling commands}:\n"
           "  *c1{-} *c1{Ctrl-g} when asked for planet.\n"
           "  *c1{-} Enter `*c1{0}' when asked for number of ships.\n\n"
           "*c1{p}: gives you the properties of a given planet\n"
           "   (unless it's neutral).\n\n"
           "*c1{d}: computes the distance between any two given planets.\n\n"
           "*c1{s}: send a given amaunt of ships from one planet to another\n"
           "   (when they arrive, if the planet belongs to enemy\n"
           "   they will attack else they will bring reinforcements).\n\n"
           "*c1{t}: transfer ships to another player\n"
           "   +(like send but they become his ships).\n\n"
           "*c1{a}: autosend ships (will be done each turn until you change it).\n\n"
           "*c1{b}: builds a defence base on a planet. it takes some time to\n"
           "   build a base (*c4{depending on your population}). You can't build\n"
           "   more then 5 bases on a planet and each base equals to: *c5{20 ships}.\n\n"
           "*c1{e}: ends your turn (passes to next player,\n"
           "   after the last runs 'events' and passes to the first).\n\n"
           "*c1{f}: gives you the 'force' relations -> amount of ships, planets\n"
           "   to each player.\n\n"
           "%s"
           "This commands are also mostly accesible from the menu but they\n"
           "are good shortcuts...\n\n"
           "*c1{Q}: exits the game with a query.\n\n"
           "*c1{R}: restarts the game with a query\n\n"
           "*c1{G}: generates a new map\n\n"
           "*c1{S}: saves the game\n\n"
           "*c1{L}: loads a saved game\n\n"
           "*c1{C}: shows the cool winding flag and the credits\n\n"
           "%s"
           "*c1{colors}:     toggles colors on and of %s\n"
           "             and wont have any efect.\n\n"
           "*c1{aninations}: toggles window animations on/off\n\n"
           "*c1{popup_stat}: toggles whether the forces will be in popup or\n"
           "             in normal console.\n\n"
           "*c1{anim_speed}: toggles the speed of animating windows\n"
           "             [fast/slow].\n\n"
           "(This may not be up to date)\n\n"
           "*c1{-d}          debugging mode\n"
           "            this mode allows one to choose a different AI\n"
           "            for debugging purpuses or view the flag in\n"
           "            slow motion and several other things.\n\n"
           "%s"
           "*c1{--help}      shows a small builtin help on the cmd options\n\n"
           "*c1{--version}   shows the current version\n\n"
           "*c1{--frob}      shows a frob... (-:\n\n"
           "To return to the game press ESC...\n",
           my_title("Basic help for ZIS - "
                 "Zionist In Space version 0.99.5", SCROLL_WIDTH, SUB_TITLE), /* @RELEASE@ */
           my_title("Game Concepts", SCROLL_WIDTH, TITLE),
           my_title("Description of game commands", SCROLL_WIDTH, SUB_TITLE),
           my_title("Special Commands", SCROLL_WIDTH, SUB2_TITLE),
           my_title("Options", SCROLL_WIDTH, SUB_TITLE),
           str_notyet(),
           my_title("Command line options", SCROLL_WIDTH, SUB_TITLE));
   my_scroll(help_text);
}

void help_key()
{
   int key;
   
   popup(15, 45);
   pw("what key: ");
   key = gw();
   cw();
   pw("%s", my_title(keyname(key), ww(), TITLE));
   phelp(key);
   gw();
   popdown();
}

void phelp(int key)
{
   switch(key)
   {
   case 's':
      {
         pw("Send: send a given amount of ships from\n"
            "one of your planets to any other planet.\n"
            "If the planet that you are sending to is\n"
            "yours they will join the total amount\n"
            "of ships at that planet.\n"
            "Otherwise they will attack that planet.\n"
            "If they will win the planet will become\n"
            "yours.");
         break;
      }
   case 't':
      {
         pw("Transfer: transfers a given amount of ships\n"
            "from one planet to another.\n"
            "The ships will join the fleet of the\n"
            "destination REGARDLES of it's owner.");
         break;
      }
   case 'a':
      {
         pw("Auto-send: this command allows you to\n"
            "autommaticly send ships from one planet to\n"
            "another planet every turn.\n"
            "This can save you quite some time after you\n"
            "will get used to it.\n"
            "you can learn to whom is the planet\n"
            "'auto-sending' by typing 'p' and th name of\n"
            "the planet.\n"
            "A planet can 'auto-send' to only one planet\n"
            "at a time.");
         break;
      }
   case 'd':
      {
         pw("Distance: calculates distance between\n"
            "two planets.\n"
            "The oputput is the amount of turns it will\n"
            "take to a ship to cover that distance.");
         break;
      }
   case 'p':
      {
         pw("Properties: shows you the properties of a\n"
            "planet.\n"
            "The output is in the form of a table:\n"
            "  ships: the amount of ships on the planet.\n"
            "  kill ratio: how efficently will the ships\n" 
            "              sent from that planet battle.\n"
            "The rest is rather understandable.\n");
         break;
      }
   default:
      {
         pw("No game command with that name, try h, F1, ?\n"
            "for a short help on all commands.");
         break;
      }
   case 'f':
      {
         pw("Force relations: shows you the relation of\n"
            "forces in the game in a scrolling form.\n"
            "statistics.\n");
         break;
      }
   case 'e':
      {
         pw("Endturn: passes the turn to the next player.\n"
            "If the last player passed his turn it shows\n"
            "the events at the end of the turn and shows\n"
            "all the messages. Then it passes the turn to\n"
            "the first player.");
         break;
      }
   case 'h':
      {
         pw("Shows a short help on all game commands.");
         break;
      }
   case 'F':
      {
         pw("Debugging, Shows the fleet table.\n");
      }
   case 'H':
      {
         pw("Shows this help program.");
         break;
      }
   case KEY_F(10): case KEY_ESC:
      {
         pw("Shows the game menu.\n"
            "In the menu you move between the options\n"
            "by typing *c1{down/up} and selecting by the *c1{enter}\n"
            "key...");
         break;
      }
   case 'b':
      {
         pw("Builds a base on a planet.\n"
            "You can build bases on planets and they help\n"
            "the planet defend itself against attacks.\n"
            "the bases go into battle\n"
            "*c1{before} the ships and fight independently\n"
            "from them.\n"
            "each base is worth: *c1{20} ships\n"
            "with a kill eatio of *c1{1}.\n");
         break;
      }
   case 'Q': case 'R': case 'S': case 'G': case 'L': case 'C': case 'N':
      {
         pw("extendend commands *c1{availble} from the menu:\n\n"
            "*c1{L}: load game.\n"
            "*c1{S}: save game.\n"
            "*c1{R}: restart game.\n"
            "*c1{G}: generate new map.\n"
            "*c3{C}: credits, the nice show off with the flag.\n");
         break;
      }
   }
}

int check(int user)
{
   checkstat();
   if(totplanets[user] != 0)
      return 1;
   else
      return 0;
}


char* pname(int user)
{
   if(strcmp(names[user], "rainbowboy") == 0)
   {
      spf("%s", rainbow(names[user]));
   }
   else
   {
      spf("*c%d{%s}", user, names[user]);
   }
   if(psymbol[user] != ' ')
   {
      spf("%c", psymbol[user]);
   }

   return sfinish();
}


void showname(int user)         /* displays a user's name in color */
{
   pw("%s", pname(user));
   if(psymbol[user] != ' ')
   {
      pw("%c", psymbol[user]);
   }
}


void credits()
{
   popup(23, 78);

#define INDENT "                                            " /* 44 spaces */
   
   pw("%s\n\n"
      INDENT "Lead programmer:\n\n"
      INDENT "        *c3{Eli Cherniavsky}.\n\n"
      INDENT "Technical advisor:\n\n"
      INDENT "        *c3{Beni Cherniavsky}.\n\n"
      INDENT "Graphical designer:\n\n"
      INDENT "        *c3{Barak Katzir}.\n\n"
      INDENT "Campaign writer:\n\n"
      INDENT "        *c3{Tomer Razikovetski}.\n\n",
      my_title("ZIS - ZIONIST IN SPACE 0.99.5", /* @RELEASE@ */
               ww(), TITLE));

#undef INDENT
   
   {
      WINDOW *old_cur_win = cur_win;

      cur_win = derwin(cur_win, 20, 40, 3, 4);
      flag(15);
      cur_win = old_cur_win;
   }
   
   popdown();
}

void newgame()
{
   game = 1;

   setup();
   play();
}

void custom()
{
   NOTYET("custom game");
}

void init_options()
{
   option.colors     = 1;
   option.animations = 1;
   option.fast_anim  = 1;
}

void build_defence(int planet)
{
   Planet *p = &plnt[planet];
   
   if(p -> defence == 5)
   {
      message("you already have five bases at that planet, you can't build any more.");
      return;
   }
   else if(p -> ptime)
   {
      spf("A base is being built and will be ready in %d moves, until then: *c4{wait}",
          p -> ptime);
      message(sfinish());
   }
   
   else
   {
      p -> ptime = 5 / (p -> morale * 0.01);
      
      pw("it will take %d turns to build a base, building commencing...\n",
        (p -> ptime));
      
      return;
   }
}

/* el like in elberth, the star of the blessed
 * land in elvish (-: 
 * returnns the name of a planet in color */
char *elname(int planet)        
{
   spf("*c%d{%c}", owner[planet], pnames[planet]);
   if(psymbol[owner[planet]] != ' ')
   {
      spf("%c", psymbol[owner[planet]]);
   }
   return sfinish();
}

void mdl(float delay)           /* negative delay starts animation timer */
{
   static struct timeval target;
   
   struct timeval now, towait;

   if(delay < 0)
   {
      gettimeofday(&target, NULL);
      return;
   }
   
   gettimeofday(&now, NULL);
   {
     /* towait = target - now */
      long usec = target.tv_usec - now.tv_usec;
      
      towait.tv_sec = target.tv_sec - now.tv_sec - (usec < 0);
      towait.tv_usec = usec + 1000000 * (usec < 0);
   }
   select(0, NULL, NULL, NULL, &towait);
   {
     /* target = target + delay */
      long usec = target.tv_usec + delay * 1000000;
      
      target.tv_sec  += usec / 1000000;
      target.tv_usec  = usec % 1000000;
   }
}

void display_fleet_table()
{
   int   i;
   char  string[8192];
   char *s;

   s = string;

   s += sprintf(s, "%s", my_title("Fleet Table (debugging)", SCROLL_WIDTH, TITLE));
   s += sprintf(s, "\n");
   for(i = 0; i < FLEETS; i ++)
   {
      if(fships[i])
      {
         s += sprintf(s, "*c1{%d}: *c2{owner}: %11s, *c2{target}: %s, *c2{kill}: %.3f, *c2{ships}: %5d, *c2{time}: %2d.\n",
                      i, pname(fowner[i]), elname(ftarget[i]),  fkill[i], fships[i], fturn[i]);
      }
   }
   my_scroll(string);
}

