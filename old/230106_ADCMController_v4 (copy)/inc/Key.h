// key thread
#include <term.h>
#include <termios.h>  
#include <unistd.h>

int getch(void)  
{  
  int ch;  
  struct termios buf;  
  struct termios save;  
  
   tcgetattr(0, &save);  
   buf = save;  
   buf.c_lflag &= ~(ICANON|ECHO);  
   buf.c_cc[VMIN] = 1;  
   buf.c_cc[VTIME] = 0;  
   tcsetattr(0, TCSAFLUSH, &buf);  
   ch = getchar();  
   tcsetattr(0, TCSAFLUSH, &save);  
   return ch;  
}

// void key_thread()
// {
//     char key;
//     for (; !(key == '\n');)
//     {
//         key = getch();
//         if(key == '-') {}
//     }
// }
