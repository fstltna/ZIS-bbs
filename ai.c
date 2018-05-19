/* Copyright 2002 Eli Cherniavsky <locutos@sf.net> */

/* Permission is hereby granted, free of charge, to any person obtaining a
   copy of this software and associated documentation files (the "Software"),
   to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
   DEALINGS IN THE SOFTWARE. */

#include "zis.h"
#include <stdlib.h>

/* defines */

#define FRIEND 65536
#define ENEMY  131072
#define NEUTRAL 262144

/* Nothing here. */

void ai(int player)
{
   int i;

   if(check_neutral())
   {
      distcalc(NEUTRAL);
      for(i = 1; i <= planets; i ++)
      {
         if(owner[i] == player && closeplanet[i] != 0)
         {
            send(i, closeplanet[i], ships[i], 0);
         }
      }
   }
   else
   {
      distcalc(ENEMY);
      for(i = 1; i <= planets; i ++)
      {
         if(owner[i] == player && closeplanet[i] != 0)
         {
            send(i, closeplanet[i], ships[i], 0);
         }
      }
   }
}

void ai2(int player)
{
   int i, j, attacked[72], count = 0, choice = rand() % 10;

   for(i = 0; i < 72; i ++)
   {
      attacked[i] = 0;
   }

   if(check_neutral())          /* expansion algoritm */
   {
      distcalc(NEUTRAL);
      
      for(i = 1; i <= planets; i ++)
      {
         if(owner[i] == player)
         {
            if(closeplanet[i] != 0)
            {
               if(attacked[count] == 0)
               {
                  send(i, closeplanet[i], check_ships(i), 0);
                  attacked[count] = closeplanet[i];
                  count ++;
               }
               else
               {
                  int min_dist = YSIZE + XSIZE;
                  
                  for(j = 0; j < count; j ++)
                  {
                     int dist     = distance(i, attacked[j]);

                     if(dist < min_dist)
                     {
                        min_dist = dist;
                     }
                  }

                  if(min_dist < (closeplanet[i] * 1.5))
                  {
                     send(i, attacked[j], check_ships(i), 0);
                  }
                  else
                  {
                     send(i, closeplanet[i], check_ships(i), 0);
                  }
               }
            }
            else
            {
               distcalc(FRIEND);
               if(closeplanet[i] != 0)
               {
                  send(i, closeplanet[i], check_ships(i), 0);
               }
               distcalc(ENEMY);
            }
         }
      }
   }
      
   else if(choice > 2)          /* attacking algoritm */
   {
      distcalc(ENEMY);
      for(i = 1; i <= planets; i ++)
      {
         if(owner[i] == player)
         {
            if(attacked[count] == 0)
            {
               attacked[count] = closeplanet[i];
               if(check_chance(i, attacked[count]))
               {
                  send(i, attacked[count], check_ships(i), 0);
               }
            }
            else
            {
               if((distance(i, attacked[count]) / 1.5) < distance(i, closeplanet[i]))
               {
                  send(i, attacked[count], check_ships(i), 0);
               }
               else
               {
                  send(i, closeplanet[i], check_ships(i), 0);
               }
            }
         }
      }
   }
   else if(choice <= 2)
   {
      distcalc(FRIEND);
      for(i = 1; i <= planets; i ++)
      {
         if(supplyline(i, 4))
         {
            send(i, supplyline(i, 4), check_ships(i), 0);
         }
         else if(debug == 1)
         {
            pw("debugging mode...\n"
               "planet %c is keeping the ships\n", pnames[i]);
         }
      }
   }
}

void distcalc(int mode)
{
   int dist, min_dist, i, j;

   for(i = 1; i <= planets; i++)
   {
      min_dist = XSIZE + YSIZE; /* bigger than any distance */
      
      closeplanet[i] = 0;
      for(j = 1; j <= planets; j++)
      {
         if((mode == FRIEND  && owner[j] == player && j != i) ||
            (mode == ENEMY   && owner[j] != player && owner[j] != 0) ||
            (mode == NEUTRAL && owner[j] == 0))
         {
            dist = distance(i, j);
            if(dist < min_dist)
            {
               min_dist = dist;
               closeplanet[i] = j;
            }
         }
      }
   }
}

int power()
{
   int cur, i;

   checkstat();
   for(i = 1; i <= players; i ++)
   {
      cur = totships[i] * totplanets[i] * average[i] - lost[i] + won[i];
      strong[i] = cur;
   }
}

int avkill() /*a function that updates the averegae kill ratio for players */
{
   int i;

   for(i = 1; i <= planets; i ++)
   {
      if(owner[i] != 0)
      {
         average[owner[i]] += kratio[i];
      }
   }  
   for(i = 1; i <= players; i ++)
   {
      average[i] = average[i] / totplanets[i];
   }
}

int supplyline(int planet, int range)
{
   int i;

   for(i = 1; i <= planets; i ++)
   {
      if(player == owner[planet] && distance(planet, i) < range)
         return i;
   }
   return 0;
}

int check_neutral()
{
   int i;

   for(i = 1; i <= planets; i ++)
   {
      if(owner[i] == 0)
      {
         return 1;
      }
   }
   return 0;
}

void weakplanet(int mode)
{
   int i, cur, weak = 1000;

   for(i = 0; i <= planets; i ++)
   {
      if((mode == FRIEND && owner[i] == player ) ||
         (mode == ENEMY && owner[i] != player && owner[i] != 0))
      {
         cur = formula(i);
         if(cur < weak)
         {
            weak = cur;
         }
      }
   }
}

float formula(int planet)                 /* for weakplanet() */
{
   float power;

   power = ships[planet] * kratio[planet];

   return power;
}

int check_ships(int planet)
{
   return (int)ships[planet] * 0.8;
}

int check_chance(int planet, int planet2)
{
   if((formula(planet) * 2) < formula(planet2))
      return 0;
   else
      return 1;
}


      

   

