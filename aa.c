/* copyright Barak Katzir Eli Cherniavsly 2002 */
/* ascii-art file contains all the ascii-art in c-strings and several functions */

/*
  Permission is hereby granted, free of charge, to any person obtaining a copy
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
  SOFTWARE.
*/

#include "zis.h"

/* functions */


/* the Israeli flag (by Barak Katzir */
char *Iflag[] = 
{
"*c3{ _ }*c7{                ____}\n"
"*c3{| |}*c7{_            __/    \\__}\n"
"*c3{| |}*c7{ \\__      __/}   *c5{____}   *c7{\\__}\n"
"*c3{| |}*c5{_}  *c7{ \\____/}  *c5{ __/____\\__}   *c7{\\____}\n"
"*c3{| |}*c5{_\\__      __/__/    \\__\\__}     *c7{|\n}"
"*c3{| |}*c5{ \\__\\____/__/}   *c1{\\___}   *c5{\\__\\____}*c7{|}\n"
"*c3{| |}*c5{    \\____/   *c1{__/ \\_/}*c5{      \\____}*c7{|}\n"
"*c3{| |}*c1{           \\//    \\           } *c7{|}\n"
"*c3{| |}*c1{            \\/  _/__\\          }*c7{|}\n"
"*c3{| |}*c1{            /\\_//}*c5{___           }*c7{|}\n"
"*c3{| |}*c5{_}          *c1{//}*c5{_}*c1{\\}*c5{/____\\__        }*c7{|}\n"
"*c3{| |}*c5{_\\__      __/__/    \\__\\__     }*c7{|}\n"
"*c3{| |}*c5{ \\__\\____/__/   }*c7{____}   *c5{\\__\\____}*c7{|}\n"
"*c3{| |}*c7{_}   *c5{\\____/}   *c7{__/    \\__}   *c5{\\____}*c7{|}\n"
"*c3{| |}*c7{ \\__      __/          \\__     |}\n"
"*c3{|_|}*c7{    \\____/                \\____|}\n"
,
"*c3{ _ }*c7{__                  ____}\n"
"*c3{| |}*c7{  \\__            __/    \\__}\n"
"*c3{| |}*c5{__}   *c7{\\__}     *c7{ __/}   *c5{____}   *c7{\\__}\n"
"*c3{| |}*c5{__\\__}   *c7{\\____/}   *c5{__/____\\__}   *c7{|}\n"
"*c3{| |}*c5{  \\__\\__      __/__/    \\__\\__}*c7{|}\n"
"*c3{| |}*c5{     \\__\\____/__/          \\__}*c7{|}\n"
"*c3{| |}*c5{        \\____/}*c1{/\\ __           }*c7{|}\n"
"*c3{| |}*c1{           __/__/\\ /          }*c7{|}\n"
"*c3{| |}*c1{          \\ |     \\           }*c7{|}\n"
"*c3{| |}*c5{__}         *c1{\\/    /_\\}*c5{____      }*c7{|}\n"
"*c3{| |}*c5{__\\__}      *c1{/ \\__/}*c5{__/____\\__   }*c7{|}\n"
"*c3{| |}*c5{  \\__\\__}  *c1{/__/\\/}*c5{/__/    \\__\\__}*c7{|}\n"
"*c3{| |}*c7{__}   *c5{\\__\\____/__/   }*c7{____}   *c5{\\__}*c7{|}\n"
"*c3{| |}*c7{  \\__   }*c5{\\____/}  *c7{ __/    \\__   }*c7{|}\n"
"*c3{| |}*c7{     \\__      __/          \\__}*c7{|}\n"
"*c3{|_|}*c7{        \\____/}\n"
,
"*c3{ _ }*c7{  ____                  ____}\n"
"*c3{| |}*c7{_/    \\__            __/    \\__}\n"
"*c3{| |}*c5{  ____}   *c7{\\__      __/   }*c5{____   }*c7{\\}\n"
"*c3{| |}*c5{_/____\\__}   *c7{\\____/}   *c5{__/____\\__ }*c7{|}\n"
"*c3{| |}*c5{_/    \\__\\__      __/__/    \\__\\}*c7{|}\n"
"*c3{| |}*c5{         \\__\\____/__/          \\}*c7{|}\n"
"*c3{| |}*c1{          __}*c5{\\}*c1{_/\\_}*c5{/}*c1{__}            *c7{|}\n"
"*c3{| |}*c1{         \\  /____\\  /           }*c7{|}\n"
"*c3{| |}*c1{          \\/      \\/            }*c7{|}\n"
"*c3{| |}*c5{  ____}    *c1{/|      |\\}*c5{    ____    }*c7{|}\n"
"*c3{| |}*c5{_/____\\__}*c1{/_\\______/_\\}*c5{__/____\\__ }*c7{|}\n"
"*c3{| |}*c5{_/    \\__\\__}*c1{\\ __ /}*c5{__/__/    \\__\\}*c7{|}\n"
"*c3{| |}*c7{  ____}   *c5{\\__\\_}*c1{\\/}*c5{_/__/}   *c7{____}   *c5{\\}*c7{|}\n"
"*c3{| |}*c7{_/    \\__}   *c5{\\____/}   *c7{__/    \\__ }*c7{|}\n"
"*c3{| |}*c7{         \\__      __/          \\}*c7{|}\n"
"*c3{|_|}*c7{            \\____/}\n"
,
"*c3{ _ }*c7{      ____                  ____}\n"
"*c3{| |}*c7{   __/    \\__            __/    }*c7{|}\n"
"*c3{| |}*c7{__/}   *c5{____}   *c7{\\__}      *c7{__/}   *c5{____}*c7{|}\n"
"*c3{| |}*c5{   __/____\\__}   *c7{\\____/}   *c5{__/____}*c7{|}\n"
"*c3{| |}*c5{__/__/    \\__\\__      __/__/    }*c7{|}\n"
"*c3{| |}*c5{__/        }*c1{__}*c5{\\__\\____/__/       }*c7{|}\n"
"*c3{| |}*c1{          \\  \\/\\}*c5{\\____/}          *c7{|}\n"
"*c3{| |}*c1{           \\_/  \\__             }*c7{|}\n"
"*c3{| |}*c1{           /     \\ /            }*c7{|}\n"
"*c3{| |}*c5{      ____}*c1{/_\\    _/}         *c5{____}*c7{|}\n"
"*c3{| |}*c5{   __/____\\__}*c1{\\__/ \\}     *c5{ __/____}*c7{|}\n"
"*c3{| |}*c5{__/__/    \\__\\}*c1{\\/\\__\\}  *c5{__/__/    }*c7{|}\n"
"*c3{| |}*c5{__/}   *c7{____}   *c5{\\__\\____/__/}   *c7{____}*c7{|}\n"
"*c3{| |}*c7{   __/    \\__   }*c5{\\____/}  *c7{ __/}\n"
"*c3{| |}*c7{__/          \\__      __/}\n"
"*c3{|_|}*c7{                \\____/}\n"
,
"*c3{ _ }*c7{          ____}\n"
"*c3{| |}*c7{       __/    \\__            __/}*c7{|}\n"
"*c3{| |}*c7{    __/}   *c5{____}  *c7{ \\__      __/}   *c7{|}\n"
"*c3{| |}*c7{___/}   *c5{__/____\\__}   *c7{\\____/}   *c5{__/}*c7{|}\n"
"*c3{| |}*c5{    __/__/    \\__\\__      __/__/}*c7{|}\n"
"*c3{| |}*c5{___/__/    }*c1{___/\\} *c5{\\__\\____/__/}   *c7{|}\n"
"*c3{| |}*c5{___/}      *c1{\\ _/\\_\\}   *c5{\\____/}      *c7{|}\n"
"*c3{| |}*c1{           /     \\_}             *c7{|}\n"
"*c3{| |}*c1{          /_\\_   |_/}            *c7{|}\n"
"*c3{| |}*c5{          ___}*c1{\\\\__/\\}             *c7{|}\n"
"*c3{| |}*c5{       __/____}*c1{\\/}*c5{_}*c1{\\_\\}*c5{         __/}*c7{|}\n"
"*c3{| |}*c5{    __/__/    \\__\\__      __/__/}*c7{|}\n"
"*c3{| |}*c5{___/__/}  *c7{ ____}   *c5{\\__\\____/__/}   *c7{|}\n"
"*c3{| |}*c5{___/}   *c7{__/}    *c7{\\__   }*c5{\\____/}   *c7{__/}\n"
"*c3{| |}*c7{    __/          \\__      __/}\n"
"*c3{|_|}*c7{___/                \\____/}\n"
,
"*c3{ _ }*c7{              ____}\n"
"*c3{| |}*c7{           __/    \\__}\n"
"*c3{| |}*c7{__      __/}   *c5{____}   *c7{\\__      __}\n"
"*c3{| |}*c7{  \\____/}   *c5{__/____\\__}   *c7{\\____/}  *c7{|}\n"
"*c3{| |}*c5{__      __/__/*c1{_/\\_}*c5{\\__\\__      __}*c7{|}\n"
"*c3{| |}*c5{__\\____/__/} *c1{_//  \\\\_} *c5{\\__\\____/__}*c7{|}\n"
"*c3{| |}*c5{  \\____/}   *c1{\\ /    \\ /}   *c5{\\____/}  *c7{|}\n"
"*c3{| |}*c1{            \\|    |/            }*c7{|}\n"
"*c3{| |}*c1{            / \\__/ \\            }*c7{|}\n"
"*c3{| |}*c1{           /_/}*c5{_}*c1{\\/}*c5{_}*c1{\\_\\           }*c7{|}\n"
"*c3{| |}*c5{           __/____\\__           }*c7{|}\n"
"*c3{| |}*c5{__      __/__/    \\__\\__      __}*c7{|}\n"
"*c3{| |}*c5{__\\____/__/}  *c7{ ____}   *c5{\\__\\____/__}*c7{|}\n"
"*c3{| |}*c5{  \\____/}   *c7{__/    \\__}   *c5{\\____/}  *c7{|}\n"
"*c3{| |}*c7{__      __/          \\__      __}*c7{|}\n"
"*c3{|_|}*c7{  \\____/                \\____/}\n"
};


/* a function that shows you a winding Israeli flag */

int flag(int repeat)
{
   int i, j;
   
   pw("Press any key to continue.");
   
   nodelay(cur_win, TRUE);

   mdl(-1);
   for(j = 0; j < repeat; j ++)
   {
      for(i = 0; i < 6; i ++)
      {
         cw();
         pw("%s", Iflag[i]);
         wrefresh(cur_win); /* PDCurses' getch doesn't refresh with nodelay */
         if(gw() != ERR)
         {
            repeat = 0;
            break;
         }
         if(debug)
         {
            mdl(0.8);
         }
         else
         {
            mdl(0.2);
         }         
      }
   }

   nodelay(cur_win, FALSE);
}

   
