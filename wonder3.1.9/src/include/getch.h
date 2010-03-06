/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *                                                                                   *
 *  WONDER - Wave field synthesis Of New Dimensions of Electronic music in Realtime  *
 *  http://swonder.sourceforge.net                                                   *
 *                                                                                   *
 *                                                                                   *
 *  Technische Universität Berlin, Germany                                           *
 *  Audio Communication Group                                                        *
 *  www.ak.tu-berlin.de                                                              *
 *  Copyright 2006-2008                                                              *
 *                                                                                   *
 *                                                                                   *
 *  This program is free software; you can redistribute it and/or modify             *
 *  it under the terms of the GNU General Public License as published by             *
 *  the Free Software Foundation; either version 2 of the License, or                *
 *  (at your option) any later version.                                              *
 *                                                                                   *
 *  This program is distributed in the hope that it will be useful,                  *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                    *
 *  GNU General Public License for more details.                                     *
 *                                                                                   *
 *  You should have received a copy of the GNU General Public License                *
 *  along with this program; if not, write to the Free Software                      *
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.       *
 *                                                                                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>

int keypressed()
{
   fd_set input_fdset;
   int old_flags, old_flags2;
   struct termios term_attr;
   int c = 0;
   
   if (tcgetattr(STDIN_FILENO, &term_attr) != 0)
   {
      perror("getch:tcgetattr() failed!");
   }
   old_flags = term_attr.c_lflag;
   term_attr.c_lflag &= ~(ICANON | ECHOE);
   if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term_attr) != 0)
   {
      perror("getch:tcsetattr() failed!");
   }

   if (tcgetattr(STDOUT_FILENO, &term_attr) != 0)
   {
      perror("getch:tcgetattr() failed!");
   }
   old_flags2 = term_attr.c_lflag;
   term_attr.c_lflag = 0;
   if (tcsetattr(STDOUT_FILENO, TCSAFLUSH, &term_attr) != 0)
   {
      perror("getch:tcsetattr() failed!");
   }
   
   FD_ZERO(&input_fdset);
   FD_SET(STDIN_FILENO, &input_fdset);
   if (select(STDIN_FILENO + 1, &input_fdset, NULL, NULL, NULL) == -1)
   {
      perror("getch:select() failed!");
   }
   if (FD_ISSET(STDIN_FILENO, &input_fdset))
   {
      c = 1;
   }
   
   term_attr.c_lflag = old_flags;
   if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term_attr) != 0)
   {
      perror("getch:tcsetattr() failed!");
   }

   term_attr.c_lflag = old_flags2;
   if (tcsetattr(STDOUT_FILENO, TCSAFLUSH, &term_attr) != 0)
   {
      perror("getch:tcsetattr() failed!");
   }

   return(c);
}

char getch()  
{
   fd_set input_fdset;
   int old_flags, old_flags2;
   struct termios term_attr;
   char c = 0;
   
   if (tcgetattr(STDIN_FILENO, &term_attr) != 0)
   {
      perror("getch:tcgetattr() failed!");
   }
   old_flags = term_attr.c_lflag;
   term_attr.c_lflag &= ~(ICANON | ECHOE);
   if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term_attr) != 0)
   {
      perror("getch:tcsetattr() failed!");
   }
   
   if (tcgetattr(STDOUT_FILENO, &term_attr) != 0)
   {
      perror("getch:tcgetattr() failed!");
   }
   old_flags2 = term_attr.c_lflag;
   term_attr.c_lflag = 0 ;
   if (tcsetattr(STDOUT_FILENO, TCSAFLUSH, &term_attr) != 0)
   {
      perror("getch:tcsetattr() failed!");
   }
      
   
   while (1)
   {
      FD_ZERO(&input_fdset);
      FD_SET(STDIN_FILENO, &input_fdset);
      if (select(STDIN_FILENO + 1, &input_fdset, NULL, NULL, NULL) == -1)
      {
         perror("getch:select() failed!");
      }
      if (FD_ISSET(STDIN_FILENO, &input_fdset))
      {
         if (read(STDIN_FILENO, &c, 1) == -1)
         {
            perror("getch:read() failed!");
         }
         else
         {
            break;
         }
      }
   }
   term_attr.c_lflag = old_flags;
   if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term_attr) != 0)
   {
      perror("getch:tcsetattr() failed!");
   }
   term_attr.c_lflag = old_flags2;
   if (tcsetattr(STDOUT_FILENO, TCSAFLUSH, &term_attr) != 0)
   {
      perror("getch:tcsetattr() failed!");
   }

   return(c);
}

char getche()
{
   char c;
   c = getch();
   write(STDOUT_FILENO, &c, 1);
   return(c);
}
