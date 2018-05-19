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

#include "zis.h"


void map()                      /* the map of the universe */
{
   int i, j;
   int width = 2;               /* characters per planet (excluding psymbol) */
   WINDOW *old_cur_win = cur_win;

   cur_win = map_win;

   cw();
   mw(0, 0);

  /* legend */
   if(map_mode == MAP_SHIPS)
      pw("*c7{[Ships (press m to change)]}  ");
   else if(map_mode == MAP_PROD)
      pw("*c7{[Productions (press m to change)]}  ");
   else if(map_mode == MAP_MORALE)
      pw("*c7{[Morale (press m to change)]}  ");
   pw("Owners: ");

   for(i = 0; i <= players; i++)
   {
      if(i != 0)
         pw(", ");
      sa(pattr[i]);
      pw("%s", pname(i));
      se();
      if(use_psymbol != 0)
         pw("=`%c'", psymbol[i]);
   }

   mw(2, 0);       /* the map is more important than the legend */

  /* the map */
   for(i = 0; i < ysize; i++)
   {
      for(j = 0; j < xsize; j++)
      {
         int planet = space[i][j];

         if(planet == 0)
         {
            pw("%*s", width, ".");
            if(use_psymbol)
               pw(" ");
         }
         else
         {
            int ow = owner[planet];

            sa(pattr[ow]);
            if(map_mode == MAP_NORMAL)
            {
               pw("%*c", width, pnames[planet]);
            }
            else
            {
               if(owner[planet] == 0)
               {
                  pw("%*s", width, "?");
               }
               else if(map_mode == MAP_SHIPS)
               {
                  pw("%*d", width, ships[planet]);
               }
               else if(map_mode == MAP_PROD)
               {
                  pw("%*d", width, prod[planet]);
               }
               else if(map_mode == MAP_MORALE)
               {
                  pw("%*d", width, plnt[planet].morale);
               }
            }
            se();
            if(use_psymbol != 0)
            {
               pw("%c", psymbol[ow]);
            }
         }
      }
      pw("\n");
   }

   cur_win = old_cur_win;
}

void select_map()
{
   while(1)
   {
      int c;

      map();
      pw("press enter to generate another map, anything else to stop: ");
      c = gw();
      if(c != '\n' && c != KEY_ENTER)
      {
         break;
      }
      randmap();
   }
   pw("\n");
}

int readplanet(char *prompt, int onlyyou)
{
   char name;

   IFCAN(0);

   pw("%s", prompt);
   while(1)
   {
      int oldy, oldx;

      name = '\n';

      noecho();
      yxw(oldy, oldx);
      name = gw();
      echo();
     /* echo the newline (to scroll at bottom) */
      mw(oldy, oldx);
      pw("\n");
      if(name == CTRL_G)
      {
         CANCEL(0);
      }

      if(name == '\n')
      {
         pw("Type something!");
      }
      else
      {
         int num;

         for(num = 1; num <= planets; num ++)
         {
            if(pnames[num] == name)
            {
               break;
            }
         }

         if(num > planets)
         {
           /* second chance - shifted letter */
            for(num = 1; num <= planets; num ++)
            {
               if(pnames[num] == toupper(name))
               {
                  break;
               }
            }
         }

         if(num > planets)
         {
            pw("No such planet!");
         }
         else if(onlyyou == 1 && owner[num] != player)
         {
            pw("That planet isn't yours!");
         }
         else
         {
            return num;         /* It's good */
         }
      }

      pw("  %s", prompt);
   }
}

int readships(int max)
{
   IFCAN(0);
   
   pw("How many ships (0 to cancel): ");
   while(1)
   {
      int num = 0;
      int oldy, oldx;
      
      noecho();
      yxw(oldy, oldx);
      sw("%d", &num);
      echo();
     /* echo the newline (to scroll at bottom) */
      mw(oldy, oldx);
      pw("\n");
      
      if(num == 0)
      {
         CANCEL(0);
      }
      else if(num < 0)
      {
         pw("Must be positive!");
      }
      else if(num > max)
      {
         pw("Too many (max %d)!", max);
      }
      else
      {
         return num;
      }

      pw("  How many ships (0 to cancel): ");
   }
}

int qsort_string(const void *e1, const void *e2)
{
   return strcmp(*(char **)e1, *(char **)e2);
}

char *readfile(char *prompt, int mustexist)
{
   int start_pos = 0;  /* used to start at same place after deleting a file */

   chdir_to_save();
   
   while(1)                     /* repeat with changing directories... */
   {
      DIR *dir;
      char *names[1024];        /* I hate games limiting saves to ~100 ;-) */
      ITEM *items[1024];
      int count = 0, i;
      int y, x;
      MENU *menu;
      int c;
     /* force non-matching chars to be appended anyway (for mustexist==0 and
        free changing of directories) by handling the pattern ourselves and
        syncing menu's idea of it with ours... */
      char pattern[256] = "";
      static char name[256];    /* returned string */

      dir = opendir("./");
      if(dir != NULL)
      {
         struct dirent *entry;

         for(count = 0; count < 1024 && (entry = readdir(dir)) != NULL; count++)
         {
            names[count] = strdup(entry->d_name);
         }
         closedir(dir);
        /* I should also append ".." if it didn't appear but I'm lazy.
           The user can still type ".." manually on such systems... */

         qsort(names, count, sizeof(char *), qsort_string);
         for(i = 0; i < count; i++)
         {
            items[i] = new_item(names[i], "");
         }
      }
      items[count] = NULL;

      menu = new_menu(items);
      set_menu_mark(menu, "=> ");

      scale_menu(menu, &y, &x);
      x = 60;
      popup(y, x);

      set_menu_win(menu, border_win);
      set_menu_sub(menu, cur_win);
      if(start_pos >= count)
      {
         start_pos = count - 1;
      }
      set_current_item(menu, items[start_pos]);
      start_pos = 0;

      mvwaddstr(border_win, 0, 1, prompt);
      {
         char *s = "DELETE/F8/^D to delete";

         mvwaddstr(border_win, 0, x - strlen(s), s);
      }

      post_menu(menu);
      noecho();

      while(1)
      {
         wmove(border_win, y + 1, 1);
         whline(border_win, ACS_HLINE, x);
         mvwprintw(border_win, y + 1, 1, "Or type the name: %s", pattern);

         c = gw();
         if(c == '\n' || c == KEY_ENTER)
         {
            break;
         }

         switch(c)
         {
         case KEY_DOWN:
            {
               menu_driver(menu, REQ_DOWN_ITEM);
               pattern[0] = '\0';
               break;
            }
         case KEY_UP:
            {
               menu_driver(menu, REQ_UP_ITEM);
               pattern[0] = '\0';
               break;
            }
         case KEY_NPAGE:
            {
               menu_driver(menu, REQ_SCR_DPAGE);
               pattern[0] = '\0';
               break;
            }
         case KEY_PPAGE:
            {
               menu_driver(menu, REQ_SCR_UPAGE);
               pattern[0] = '\0';
               break;
            }
         case KEY_HOME:
            {
               menu_driver(menu, REQ_FIRST_ITEM);
               pattern[0] = '\0';
               break;
            }
         case KEY_END:
            {
               menu_driver(menu, REQ_LAST_ITEM);
               pattern[0] = '\0';
               break;
            }
         case KEY_DC: case KEY_F(8): case 4: /* 4 == Ctrl-D */
            {
               char q[1024];
               const char *name = item_name(current_item(menu));

               sprintf(q, "Are you sure you want to _delete_ `%s' [y/n]: ", name);
               if(readyn(q))
               {
                  unlink(name);
                  start_pos = item_index(current_item(menu));
                 /* reread dircetory.  ugly hack... */
                  strcpy(pattern, ".");
                  ungetch('\n');
               }
               break;
            }
         case '\b': case 127: case KEY_BACKSPACE: /* 127 == DEL */
            {
               int len = strlen(pattern);

               if(len > 0)
               {
                  pattern[len - 1] = '\0';
                  set_menu_pattern(menu, pattern);
               }
               break;
            }
         default:               /* handle ascii characters */
            {
               if(isprint(c))
               {
                  int len = strlen(pattern);

                  if(len < sizeof(pattern))
                  {
                     int i;

                     pattern[len] = c;
                     pattern[len + 1] = '\0';
                    /* set_menu_pattern() doesn't refresh the menu to the new
                       position.  So we do it in this bogus way. */
                     menu_driver(menu, REQ_CLEAR_PATTERN);
                     for(i = 0; i <= len; i++)
                     {
                        if(menu_driver(menu, pattern[i]) != E_OK)
                        {
                           break;
                        }
                     }
                  }
                  break;
               }
            }
         }
      }

      echo();
      unpost_menu(menu);
      popdown();

     /* allow unexistant files on saves */
     /* on load, if pattern doesn't exist, retry with current selection */
     /* (that's why this cumbersome loop is used...) */
      for(i = 0; i < 2; i++)
      {
         struct stat st;

         if(i == 0)
         {
            strcpy(name, pattern);
         }
         else
         {
            strcpy(name, item_name(current_item(menu)));
         }

         st.st_mode = 0; /* if stat fails (new file), don't think it's a dir */
         if(name[0] != '\0' && (stat(name, &st) == 0 || mustexist == 0))
         {
            for(i = 0; i < count; i++)
            {
               free(names[i]);
            }

            if(S_ISDIR(st.st_mode))
            {
               chdir(name);
               break;
            }
            else
            {
               return name;
            }
         }
      }
   }
   back_to_orig_cwd();

}

void mainmenu()
{
   ITEM *items[8];
   MENU *menu;

   if(game == 0)
   {
      items[0] = new_item("New game", "");
      items[1] = new_item("Custom game", "");
      items[2] = new_item("Load game", "");
      items[3] = new_item("View help", "");
      items[4] = new_item("Display intro", "");
      items[5] = new_item("Options", "");
      items[6] = new_item("Exit", "");
      items[7] = NULL;
   }
   else
   {
      items[0] = new_item("Resume game", "");
      items[1] = new_item("Save game", "");
      items[2] = new_item("Load game", "");
      items[3] = new_item("View help", "");
      items[4] = new_item("Generate new map", "");
      items[5] = new_item("Options", "");
      items[6] = new_item("Abort game", "");
      items[7] = NULL;
   }
   menu = new_menu(items);

#ifdef DUMB_TERM
   set_menu_mark(menu, "-> ");
#else
   set_menu_mark(menu, "");
#endif

#ifdef NCURSES_VERSION
   set_menu_spacing(menu, 0, 2, 0);
#endif

   {
      int y, x, len;
      char title[] = "ZIS 0.99.5 " /* @RELEASE@ */
#ifdef MSDOS
         "DOS"
#else
         "POSIX"
#endif
         ;
      
      scale_menu(menu, &y, &x);
      len = strlen(title);
      if(x < len)
      {
         x = len;
      }
      popup(y, x);

      mvwprintw(border_win, 0, 1, "%s", title);
   
   }
   set_menu_win(menu, border_win);
   set_menu_sub(menu, cur_win);

   post_menu(menu);
   noecho();

   while(1)
   {
      int c = gw();

      if(c == '\n' || c == KEY_ENTER)
      {
         break;
      }

      switch(c)
      {
      case KEY_DOWN:
         {
            menu_driver(menu, REQ_DOWN_ITEM);
            break;
         }
      case KEY_UP:
         {
            menu_driver(menu, REQ_UP_ITEM);
            break;
         }
      case KEY_NPAGE:
         {
            menu_driver(menu, REQ_SCR_DPAGE);
            break;
         }
      case KEY_PPAGE:
         {
            menu_driver(menu, REQ_SCR_UPAGE);
            break;
         }
      case KEY_HOME:
         {
            menu_driver(menu, REQ_FIRST_ITEM);
            break;
         }
      case KEY_END:
         {
            menu_driver(menu, REQ_LAST_ITEM);
            break;
         }
      default:                  /* handle ascii charcters */
         {
            menu_driver(menu, REQ_CLEAR_PATTERN);
            menu_driver(menu, c);
            break;
         }
      }
   }

   echo();
   unpost_menu(menu);
   popdown();

   switch(item_name(current_item(menu))[0])
   {
   case 'N':
      {
         newgame();
         break;
      }
   case 'C':
      {
         custom();
         break;
      }
   case 'R':
      {
         play();
         break;
      }
   case 'S':
      {
         char q[1024];
         const char* s = getenv("USER");

         sprintf(q, "Save into `%s' [y/n]: ", s);
         if(readyn(q))
         {
            save();
            play();
         }
         break;
      }
   case 'L':
      {
         char quest[1024];
         const char* s = getenv("USER");

         sprintf(quest, "Load from `%s' [y/n]: ", s);
         if(readyn(quest))
         {
            load();
            game = 1;
            play();
         }
         break;
      }
   case 'V':
      {
         help();
         break;
      }
   case 'D':
      {
         NOTYET("intro movie");
         break;
      }
   case 'G':
      {
         select_map();
         play();
         break;
      }
   case 'O':
      {
         options();
         break;
      }
   case 'A':
      {
         if(readyn("Are you sure you want to abort [y/n]: "))
         {
            bigbang();
            werase(inp_win);
            werase(map_win);
            game = 0;
            break;
         }
      }
   case 'E':
      {
         quit();
         break;
      }
   }
}

int readyn(char *prompt)
{
   char ans;
   int ok = popup_ok();

   if(ok)
   {
      popup_len(color_strlen(prompt));
   }
   pw("%s", prompt);
   ans = gw();
   pw("\n");

   if(ok)
   {
      popdown();
   }
   return ans == 'y' || ans == 'Y';
}


void popup_noanim(int y, int x)
{
   stacktop++;

   if(y > LINES - 2)
   {
      y = LINES - 2;
   }
   if(x > COLS - 2)
   {
      x = COLS - 2;
   }
   border_win = newwin(y + 2, x + 2, (LINES - y) / 2 - 1, (COLS - x) / 2 - 1);
   box(border_win, 0, 0);
   cur_win = derwin(border_win, y, x, 1, 1);
   scrollok(cur_win, TRUE);
   keypad(cur_win, TRUE);
   pop_panel = new_panel(border_win);
}


void popdown_noanim()
{
   del_panel(pop_panel);
   delwin(cur_win);
   delwin(border_win);

   stacktop--;

  /*
    I had problems with gnome-terminal here - it leaves remants of the popup.
    xterm & linux console are fine.  In any case I think it's not my fault.
    Anyway, I added clear() at end of popdown() to work around...
  */
}


/* Helper function.  direction is +1 (popup) or -1 (popdown). */
void popanim(int y, int x, int direction)
{
   int count, i, step;
   float time;

   if(!option.animations)
   {
      return;
   }

   step = 1; /*option.slow_term ? 2 : 1;*/
   time = option.slow_term ? 0.3 : 0.2;
   
  /* the shorter / longer dimension */
   if(option.slow_term ? y < x : y > x)
   {
      count = y / step;
   }
   else
   {
      count = x / step;
   }

   mdl(-1);

   for(i = 0; i < count; i++)   /* i always counts up */
   {
      int j;                    /* j counts depending on direction */

      if(direction > 0)
      {
         j = i;
      }
      else
      {
         j = count - 1 - i;     /* 0 < j < count-1 */
      }

      popup_noanim(y * j / count, x * j / count);
      update_panels();
      doupdate();
      popdown_noanim();

      mdl(time / count);
   }
}

void popup(int y, int x)                
{
   if(option.animations)
   {
      popanim(y, x, +1);
   }
   popup_noanim(y, x);
}

void popdown()
{
   int y, x;

   getmaxyx(cur_win, y, x);
   popdown_noanim();
   if(option.animations)
   {
      popanim(y, x, -1);
   }
   clear();                     /* work around gnome-terminal bug(?) */
}

int popup_ok()
{
  /* Current logic: use popups except when during play() on inp_win
   * (to not interrupt the streamlined interaction). */
   return !(game == 1 && cur_win == inp_win && isplay == 1);
}

void popup_len(int len)
{
   int alt = (len + (COLS - 2) - 1) / (COLS - 2); /* round towards up */

   if(len > COLS - 2)
   {
      len = COLS - 2;
   }
   popup(alt, len+1);           /* work around PDCurses problems */
}

void message(char* text)
{
   popup_len(color_strlen(text));
   pw("%s", text);
   gw();
   popdown();
}

void cmessage(int color, char *text) /* like message but paints the string */
{
   char *string;
   char *s = text;

   for(s = 0; s != '\0'; s ++)
   {
      if(strcmp(s, "\n") == 0)
      {
         message("cmessage:*c4{ \n in input string...}");
         return;
      }
   }
   sprintf(string, "*c%d{%s}", color, text);
   popup_len(color_strlen(string));
   pw("%s", string);
   gw();
   popdown();
}

void options()
{
   int ans;
   
   popup(15, 40);
   mvwprintw(border_win ,0, 17, "Options");
   while(1)
   {
      display_options();
      ans = gw();
      switch(ans)
      {
      case 'a':
         {
            option.colors = !option.colors;
            break;
         }
      case 'b':
         {
            option.slow_term = !option.slow_term;
            break;
         }
      case 'c':
         {
            option.animations = !option.animations;
            break;
         }
      case 'd':
         {
            option.fast_anim = !option.fast_anim;
            break;
         }
      case KEY_ESC: case ' ': case KEY_ENTER: case '\n':
         {
            popdown();   
            return;
            break;
         }
      }
   }
}


void display_options()
{
   cw();
   pw("%s", my_title("Misc", ww(), TITLE));
   pw("[a] colors            ---  ");
   check_option(option.colors);
   pw("[b] slow terminal     ---  ");
   check_option(option.slow_term);
   pw("\n%s", my_title("Windows", ww(), TITLE));
   pw("[c] animation         ---  ");
   check_option(option.animations);
   pw("[d] animation speed   ---  %s\n",
      option.fast_anim ? "[fast]" : "[slow]");
}

int check_option(int flag)
{
   if(flag)
   {
      pw("[X]\n");
   }
   else
   {
      pw("[-]\n");
   }
}

void my_scroll(char *string)
{
   
   int  i = 0, length = 0;
   int  start_line = 0;

   for(i = 0; string[i] != '\0'; i ++) /* definition of string size */
   {
      if(string[i] == '\n')
      {
         length ++;
      }
   }
   
   noecho();
   popup(22, SCROLL_WIDTH);
   do
   {
      cw();
      mw(0, 0);
      pw("%s", string_scroll(string, start_line, SCROLL_HEIGHT));
      switch(gw())
      {
      case KEY_DOWN:
         {
            if(start_line < length - SCROLL_HEIGHT)
            {
               start_line ++;
            }
            break;
         }
      case KEY_UP:
         {
            if(start_line > 0)
            {
               start_line --;
            }
            break;
         }
      case KEY_PPAGE:
         {
            if(start_line >= SCROLL_PAGE)
            {
               start_line -= SCROLL_PAGE;
            }
            else if(start_line <= SCROLL_PAGE && start_line > 0)
            {
               start_line = 0;
            }
            break;
         }
      case KEY_NPAGE:
         {
            if(start_line < length - SCROLL_HEIGHT - SCROLL_PAGE)
            {
               start_line += SCROLL_PAGE;
            }
            else if(start_line < length - SCROLL_HEIGHT && start_line >
                    length - SCROLL_PAGE - SCROLL_HEIGHT)
            {
               start_line = length - SCROLL_HEIGHT;
            }
            break;
         }
      case KEY_ESC: case ' ': case KEY_ENTER: case '\n': case 'e':
         {
            popdown();
            echo();
            return;
         }
      }
   }
   while(1);
}

char* string_scroll(char *string, int start, int length)
{
   int  i = 0, lines = 0;
   int  start_here, end_here;
   static char return_string[16768];

   for(i = 0; string[i] != '\0'; i ++)
   {
      if(string[i] == '\n')
      {
         lines ++;
      }
      if(lines == start)
      {
         break;
      }
   }
   if(start > 0)
   {
      start_here = i + 1;
   }
   else
   {
      start_here = 0;
   }
   lines = 0;
   
   for(i = start_here; string[i] != '\0'; i ++)
   {
      if(string[i] == '\n')
      {
         lines ++;
      }
      if(lines == length)
      {
         break;
      }
   }
   end_here = i;
   
   for(i = start_here; i < end_here; i ++)
   {
      return_string[i - start_here] = string[i];
   }
   return_string[i - start_here] = '\0';
  /*fpf(stderr, "start=%d length=%d start_here=%d end_here=%d\n%s\n",
    start, length, start_here, end_here, return_string);*/
   return return_string;
}

char *my_title(char *string, int width, int mode)
{
   int  len = strlen(string);
   int  spaces;
   char underline;
   char color;
   int i;

   switch(mode)
   {
   case TITLE:
      {
         spaces    = (width - len) / 2;
         underline = '*';
         color = '7';
         break;
      }
   case SUB_TITLE:
      {
         spaces    = (width - len) / 2;
         underline = '~';
         color = '7';
         break;
      }
   case SUB2_TITLE:
      {
         spaces    = 1;
         underline = '~';
         color = '1';
         break;
      }
   }

   for(i = 0; i < spaces; i++)
   {
      sappend(" ");
   }
   spf("*c%c{", color);
   sappend(string);
   sappend("}\n");
   for(i = 0; i < spaces - 1; i ++)
   {
      sappend(" ");
   }
   sappend("*c5{");
   for(i = 0; i < len + 2; i ++)
   {
      spf("%c", underline);
   }
   sappend("}\n");

   return sfinish();

}

void showplanet(int planet) /* gives you the proprties of a given planet */
{
   int ow = owner[planet];

   IFCAN(/**/);
   if(ow != 0)
   {
      waddch(cur_win, '\n');
      pw("   Owner: %16s.   Kill ratio: *c%d{%11.3lf}.   Defence bases: *c%d{%d}.\n"
         "   Morale: *c%d{%9d%%}.   Morale changed by: *c%d{%+3d%%}.\n"
         "   Ships: *c%d{%11d}.   Ship production:   *c%d{%3d/%3d}.\n",
         pname(owner[planet]), ow, kratio[planet], ow, plnt[planet].defence,
         ow, plnt[planet].morale, ow, plnt[planet].morale - plnt[planet].old_morale,
         ow, ships[planet], ow, cur_prod[planet], prod[planet]);
      if(owner[planet] == player)
      {
         if(atarget[planet] != 0 && aships[planet] != 0)
         {
            pw("   Autosending to: %s .   Amount: *c%d{%d}.\n",
               elname(planet), ow, aships[planet]);
         }
         else
         {
            pw("   *c%d{Not auto-sending}.\n", ow);
         }
      }
   }
#if 0                           /* an older version of the same thing */
   if(ow != 0)
   {
      if(ow == player)
      {
         pw("|   owner   |  ships | production | kill ratio | autosending to | amount |\n"
            "|-----------|--------|------------|------------|----------------|--------|\n"
            "|%s""|%8d"   "|%12d"      "|%12.3lf"    "|}",
            owner[planet], pname(player), ships[planet], prod[planet], kratio[planet]);

         if(atarget[planet] != 0)
         {
            pw("*c%d{%16c""|%8d|}\n", player, pnames[planet], aships[planet]);
         }
         else
         {
            pw("*c%d{     not autosending     |}\n", player);
         }
      }
      else
      {
         pw("|   owner   |  ships | production | kill ratio |\n"
            "|-----------|--------|------------|------------|\n"
            "|*%s""|%8d"   "|%12d"      "|%12.3lf"   "|}\n",
            owner[planet], pname(player), ships[planet], prod[planet], kratio[planet]);

      }
   }
#endif
   else
   {
      pw("there is no intelegnce data on that planet\n");
   }
}

void showstat()                 /* shows statistics about the game  */
{
   int  i;
   char string[8192];
   char *s = string;

   checkstat();                 /* be up-to-date */

   adsf(s, "%s\n", my_title("Game Statistics", SCROLL_WIDTH, TITLE));

   adsf(s, "\n*c7{%s:}\n\n", "Owned planets");
   for(i = 1; i <= players; i ++)
   {
      if(!dead[i])
      {
         adsf(s, "\t%s:\t*c%d{%5d}\n", pname(i), i, totplanets[i]);
      }
   }

   adsf(s, "\n*c7{%s:}\n\n", "Total number of ships");
   for(i = 1; i <= players; i ++)
   {
      if(!dead[i])
      {
         adsf(s, "\t%s:\t*c%d{%5d}\n", pname(i), i, totships[i]);
      }
   }

   adsf(s, "\n*c7{%s:}\n\n", "Average morale on planets");
   for(i = 1; i <= players; i ++)
   {
      if(!dead[i])
      {
         adsf(s, "\t%s:\t*c%d{%5f}\n", pname(i), i, av_morale[i]);
      }
   }

   adsf(s, "\n*c7{%s:}\n\n", "Average kill ratio on planets");
   for(i = 1; i <= players; i ++)
   {
      if(!dead[i])
      {
         adsf(s, "\t%s:\t*c%d{%5f}\n", pname(i), i, average[i]);
      }
   }

   adsf(s, "\n*c7{%s:}\n\n", "Battles fought by player");
   for(i = 1; i <= players; i ++)
   {
      if(!dead[i])
      {
         adsf(s, "\t%s:\t*c%d{%5d}\n", pname(i), i, battles[i]);
      }
   }

   adsf(s, "\n*c7{%s:}\n\n", "Battles that were won");
   for(i = 1; i <= players; i ++)
   {
      if(!dead[i])
      {
         adsf(s, "\t%s:\t*c%d{%5d}\n", pname(i), i, won[i]);
      }
   }

   adsf(s, "\n*c7{%s:}\n\n", "Battles that were lost");
   for(i = 1; i <= players; i ++)
   {
      if(!dead[i])
      {
         adsf(s, "\t%s:\t*c%d{%5d}\n", pname(i), i, lost[i]);
      }
   }
   
   my_scroll(string);


   
#if 0
   if(option.popup_stat)
   {
      popup(15, 78);
   }

   pw(   " ____________" "__________" "_____________________________________________________\n");
   pw(   "|   player   " "| planets " "| ships | battles faught | won | lost | average kill |\n");
   pw(   "|------------" "|---------" "|-------|----------------|-----|------|--------------|\n");
   for(i = 1; i <= players; i ++)
   {
     /* !!! hard coded MAX_NAME !!! */
      pw("| %s | *c%d{%5d} | *c%d{%5d} | *c%d{%3d} | *c%d{%3d} | *c%d{%3d} | *c%d{%.3f} |\n",
         pname(i), i, totplanets[i], i, totships[i], i, battles[i], i, won[i], i, lost[i],
         i, average[i]);

   }
   pw(   "|____________" "|_________" "|_______|________________|_____|______|______________|\n");
   
   if(option.popup_stat)
   {
      gw();
      popdown();
   }
#endif
}

void pw(char *format, ...)
{
   va_list args;

   char buf[16768];

   va_start(args, format);
   vsprintf(buf, format, args);
   color_addstr(buf);
   va_end(args);
}
void color_addstr(char *string)
{
   for(; string[0] != '\0'; string ++)
   {
      if(string[0] == '}')
      {
         se();
      }
      else if(string[0] == '*')
      {
         if((++string)[0] == 'c')
         {
            int color = 0;

            while((++string)[0] != '{')
            {
               color = 10 * color + (string[0] - '0');
            }
            sa(pattr[color]);
         }
         else
         {
            waddch(cur_win, '*');
            string--;
         }
      }
      else
      {
         waddch(cur_win, string[0]);
      }
   }
}

int color_strlen(char *string)
{
   int i = 0;
   
   for(; string[0] != '\0'; string ++)
   {
      if(string[0] == '}')
      {
      }
      else if(string[0] == '*')
      {
         if((++string)[0] == 'c')
         {
            while((++string)[0] != '{')
            {
            }
         }
         else
         {
            i ++;
            string--;
         }
      }
      else
      {
         i ++;
      }

   }
   return i;
}

char *str_notyet()
{
   return "*c4{not yet working ...}";
}

void salloc(int len)      /* make sure there is place to append len chars */
{
   if(stail + len >= STRINGS_SIZE - 1) /* consider '\0' */
   {
     /* not strcpy to be safe from missing '\0' and to handle overlap */
      memmove(strings, strings + shead, stail - shead + 1);
      stail = stail - shead;
      shead = 0;
   }
}

void sappend(char *string)
{
   int len = strlen(string);
   
   salloc(len);
   strcpy(strings + stail, string);
   stail += len;
}

int spf(char *format, ...)      /* sappend with formatting */
{
   va_list args;

   va_start(args, format);
   salloc(STRINGS_SIZE / 4);    /* arbitrary guess of needed space */
   stail += vsprintf(strings + stail, format, args);
   va_end(args);
}

char *sfinish()
{
   char *return_string = strings + shead;

   shead = ++stail;

   return return_string;
}

char *rainbow(char *string)
{
   char *s = string;
   int i;
   
   for(i = 1; *s != '\0'; s ++, i ++)
   {
      if(i > colors - 1)
      {
         i = 1;
      }
      spf("*c%d{%c}", i, *s);
   }
   return sfinish();
}




