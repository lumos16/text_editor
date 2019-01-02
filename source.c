#include<ncurses.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<malloc.h>
#include<menu.h>

struct node
{
  int count;
  char *array;
  struct node *up, *down;
};

MEVENT event;
typedef struct node *HEADER;
HEADER first, temp, last;
char copied[1000];
int count_copied;

WINDOW *win, *w1, *w2, *w3, *add, *curses;
int posx, posy, LMARGIN, RMARGIN, BEGIN, END, WIDTH, HEIGHT;
int saved, pageno=0, line = 1, flag = 0;

char filename[20], ch;

void gotomouse (int, int);
void filehead ();		
void openfile ();		
void savefile ();		
void putfile ();			
void newfile ();				
void copyline ();		
void cutline ();			
void pasteline ();		
void filemenu ();		
void editmenu ();				
void mrefresh ();		
void wrow (int, char *);
void addname ();
void print_loc ();		
void up ();
void down ();			
void left ();
void right ();
void help ();
void newline ();

void charinsert (chtype);
void insertline ();		
void deleteline ();
void deleteword ();
void deletechar ();
void dcur ();
void backspace ();
char displaymesg ();
void helpmenu ();
void about ();
void home();
void end();
void pageup();
void pagedown();

HEADER getnode ();

int main ()
{
  chtype ch;
  int k;
  initscr ();		
  start_color ();		
  mousemask (ALL_MOUSE_EVENTS, NULL);
  init_pair (1, COLOR_BLACK, COLOR_CYAN);
  init_pair (2, COLOR_WHITE, COLOR_BLACK);
  win = newwin (LINES - 4, COLS - 2, 2, 1);
  wbkgd (win, COLOR_PAIR (1));

  cbreak ();
  noecho ();

  keypad (win, TRUE);	
  scrollok(win, TRUE);
  filehead ();
  wrefresh (w1);
  wrefresh (w2);
  wrefresh (win);
  wrefresh (w3);
  posx = posy = 0;		
  HEIGHT = LINES - 4;
  saved = 1;
  filename[0] = '\0';
  last = NULL;
  first = getnode ();
  temp = first;
 
  wmove (win, posx, posy);
  wrefresh (win);
  flushinp ();

  addname ();
  wrefresh (w3);
  wrefresh (win);
  print_loc ();

  while (1)
    {
      keypad (win, TRUE);
      ch = wgetch (win);
      switch (ch)
	{
	case KEY_UP:
	  up ();
	  break;
	case KEY_DOWN:
	  down ();
	  break;
	case KEY_RIGHT:
	  right ();
	  break;
	case KEY_LEFT:
	  left ();
	  break;
	case KEY_PPAGE:
	  pageup();
	  break;
	case KEY_NPAGE:
	  pagedown();
	  break;
	case KEY_MOUSE:
	  if (getmouse (&event) == OK)
	    {
	      if (event.bstate && BUTTON1_CLICKED)
		{
		  gotomouse (event.x + 1, event.y + 1);
		}
	      else if (event.bstate && BUTTON3_CLICKED)
		{
		  helpmenu ();
		  mrefresh ();
		  return;
		}
	    }
	  break;
	  
	case 10:
	  newline ();
	  break;
	case 263:
	  backspace ();
	  break;
	case 330:
	  dcur ();
	  break;
	case 12:
	  deleteline ();
	  break;
	case 23:
	  deleteword ();
	  break;
	case 9:
	  copyline();
	  break;
	case 11:
	  cutline();
	  break;
    case 7:
      pasteline();
      break;
    case KEY_HOME:
      home();
      break;
    case KEY_END:
      end();
      break;
	case 6:
	  filemenu ();
	  mrefresh ();
	  break;
	case 5:
	  editmenu ();
	  mrefresh ();
	  break;
	case 8:
	  helpmenu ();
	  mrefresh ();
	  break;
	default:
	  charinsert (ch);
	  break;
	}
      wmove (win, posy, posx);
      touchwin (win);
      wrefresh (win);
    }
}

void gotomouse (int x, int y)
{

  if (y >= 0 && y <= 2)
    {

      if (x >= 0 && x <= 15)
	{
	  filemenu ();
	  mrefresh ();
	  return;
	}
      if (x >= 15 && x <= 30)
	{
	  editmenu ();
	  mrefresh ();
	  return;
	}
      if (x >= 30 && x <= 45)
	{
	  helpmenu ();
	  mrefresh ();
	  return;
	}
    }

}

HEADER getnode()
{
  HEADER tmp;
  tmp = (HEADER) (malloc (sizeof (struct node)));
  tmp->array = (char *) malloc (sizeof (char) * WIDTH);
  tmp->up = tmp->down = NULL;
  tmp->count = 0;
  tmp->array[0] = '\0';
  return tmp;
}

void addname ()
{
  add = newwin (1, 12, 1, 31);
  wbkgd (add, COLOR_PAIR (2));
  if (filename[0] == '\0')
    mvwprintw (add, 0, 2, "%s", "UNTITLED");
  else
    mvwprintw (add, 0, 2,"%s", filename);
  wmove (win, 0, 0);
  wrefresh (add);
  return;
}

void filehead ()
{
  w1 = newwin (1, COLS, 0, 0);
  wattrset (w1, A_BOLD);
  wbkgd (w1, COLOR_PAIR (2));
  waddstr (w1, "FILE           EDIT           HELP           ");
  w2 = newwin (LINES - 2, COLS, 1, 0);
  wbkgd (w2, COLOR_PAIR (2));
  wrefresh (w2);
}

void copyline ()
{
  int i;
  count_copied = 0;
  for (i = 0; i < temp->count; i++)
    {
      copied[i] = temp->array[i];
      count_copied++;
    }
  mrefresh ();
}

void pasteline ()
{
  int i;
  insertline ();
  for (i = 0; i < count_copied; i++)
    {
      temp->array[i] = copied[i];
      temp->count++;
    }
  temp->array[i] = '\0';
  wrow (posy, temp->array);
  mrefresh ();
}

void insertline ()
{
  HEADER prev, next;
  saved = 0;
  prev = temp->up;
  next = temp;
  temp = getnode ();
  temp->up = prev;
  temp->down = next;
  next->up = temp;
  if (prev)
    prev->down = temp;
  winsertln (win);
  posx = 0;
}

void cutline ()
{
  copyline ();
  deleteline ();
  mrefresh ();
}

void print_loc ()
{
  w3 = newwin (1, COLS, LINES - 1, 0);
  wattrset (w3, A_BOLD);
  wbkgd (w3, COLOR_PAIR (1));
  wrefresh (w3);
  mvwprintw (w3, 0, 60, "%s", "PAGE NO.:");
  pageno = (int) (line / 21) + 1;
  mvwprintw (w3, 0, 71, "%d", pageno);
  mvwprintw (w3, 0, 10, "%s", "ROWS:");
  mvwprintw (w3, 0, 16, "%3d", line);
  mvwprintw (w3, 0, 22, "COLS:");
  mvwprintw (w3, 0, 28, "%3d", posx + 1);
  wrefresh (w3);
  wmove (win, posx, posy);
  return;
}

void mrefresh ()
{
  touchwin (w2);
  wrefresh (w2);
  touchwin (win);
  wrefresh (win);
  touchwin (add);
  wrefresh (add);
  touchwin (w3);
  wrefresh (w3);
  touchwin (w1);
  wrefresh (w1);
}

void memclose ()
{
  HEADER tmp;
  while (first)
    {
      tmp = first;
      free (tmp->array);
      free (tmp);
      first = first->down;
    }
  delwin (win);
}

void filemenu ()
{
  WINDOW *p;
  int i = 1, A = 0;
  chtype ch;
  char c;
  p = newwin (7, 12, 1, 0);
  wbkgd (p, COLOR_PAIR (2));
  wattrset (p, A_BOLD);
  keypad (p, TRUE);
  mvwaddstr (p, 1, 1, "NEW");
  mvwaddstr (p, 2, 1, "OPEN");
  mvwaddstr (p, 3, 1, "SAVE");
  mvwaddstr (p, 4, 1, "SAVE AS");
  mvwaddstr (p, 5, 1, "EXIT");
  wmove (p, 1, 1);
  wrefresh (p);
  do
    {
      ch = wgetch (p);
      if (ch == KEY_MOUSE)
	{
	  if (getmouse (&event) == OK)
	    {
	      if (event.bstate && BUTTON2_PRESSED)
		A = 1;
	      if (event.bstate && BUTTON2_RELEASED)
		A = 2;
	      if (event.bstate && BUTTON1_CLICKED)
		A = 3;

	    }

	  switch (event.y - 1)
	    {
	    case 1:
	      i = 1;
	      break;
	    case 2:
	      i = 2;
	      break;
	    case 3:
	      i = 3;
	      break;
	    case 4:
	      i = 4;
	      break;
	    case 5:
	      i = 5;
	      break;
	    }
	}
      if (ch == KEY_UP)
	{
	  if (i == 1)
	    i = 6;
	  else
	    i--;
	}
      if (ch == KEY_DOWN)
	{
	  if (i == 6)
	    i = 1;
	  else
	    i++;
	}
      wmove (p, i, 1);
      wrefresh (p);
    }
  while (ch != 27 && ch != 10 && A != 3);
  delwin (p);
  touchwin (w2);
  wrefresh (w2);
  mrefresh ();
  if (ch == 27)
    return;
  wrefresh (w2);

  switch (i)
    {
    case 1:
      newfile ();
      break;
    case 2:
      openfile ();
      break;
    case 3:
      savefile ();
      break;
    case 4:
      filename[0] = '\0';
      savefile ();
      break;
    case 5:
      if (!saved)
	c = displaymesg ();
      else
	c = 'n';
      if (c == 'y')
	savefile ();
      memclose ();
      delwin (win);
      delwin (w2);
      delwin (w1);
      system ("clear");
      endwin ();
      exit (0);

    default:
      break;
    }
}

void openfile ()
{
  WINDOW *op;
  FILE *fp;
  char c;
  if (!saved)
    {
      c = displaymesg ();
      mrefresh ();
      if (c == 'y')
	{
	  touchwin (win);
	  wrefresh (win);
	  savefile ();
	}
    }
  op = newwin (3, 45, 18, 1);
  wbkgd (op, A_STANDOUT);
  wmove (op, 1, 4);
  waddstr (op, "OPEN FILE:");
  wrefresh (op);
  echo ();
  wgetstr (op, filename);
  noecho ();

  delwin (op);

  posx = 0;
  posy = 0;
  wmove (win, posy, posx);
  wclrtobot (win);
  touchwin (win);
  wrefresh (win);
  memclose ();
  delwin (win);
  win = newwin (LINES - 4, COLS - 2, 2, 1);

  LMARGIN = RMARGIN = 0;
  HEIGHT = LINES - 4;
  WIDTH = COLS - 2;

  if (filename[0] == '\0')
    {
      delwin (op);
      wrefresh (op);
      return;
    }
  fp = fopen (filename, "r");
  if (!fp)
    {
      fp = fopen (filename, "w");
      fputc ('\n', fp);
    }
  fclose (fp);
  line = 1;
  putfile ();
  print_loc ();
}

char displaymesg ()
{
  WINDOW *diswin;
  char ch;
  diswin = newwin (3, 45, 18, 30);
  wbkgd (diswin, A_STANDOUT);
  echo ();
  mvwaddstr (diswin, 1, 4, "DO YOU WANT TO SAVE THE FILE(y/n)? :");
  wrefresh (diswin);
  ch = wgetch (diswin);
  noecho ();
  delwin (diswin);
  return (ch);
}

void save()
{
  FILE *fp;
  HEADER next = first;
  int i;
  fp = fopen (filename, "w");
  if (fp)
    {
      while (next)
	{
	  fputs (next->array, fp);
	  fputc ('\n', fp);
	  next = next->down;
	}
    }
  fclose (fp);
  addname ();
}

void savefile()
{
  WINDOW *w;
  if (!strlen (filename))
    {
      w = newwin (3, 45, 18, 30);
      wbkgd (w, A_STANDOUT);
      wmove (w, 1, 4);
      waddstr (w, "SAVE FILE AS:");
      wrefresh (w);
      echo ();
      wgetstr (w, filename);
      if (filename[0] == '\0')
	{
	  delwin (w);
	  wrefresh (w);
	  return;
	}
      if (filename[0] == '\n')
	{
	  delwin (w);
	  wrefresh (w);
	  return;
	}
      wrefresh (w);
      save ();
      noecho ();
      addname ();
      delwin (w);
      mrefresh ();
    }
  else
    save ();
  saved = 1;
}

void putfile ()
{
  FILE *fp;
  HEADER ret;
  int i;
  char c;
  fp = fopen (filename, "r");
  saved = posx = posy = 0;
  first = getnode ();
  temp = first;
  ret = first;
  i = 0;
  while ((c = fgetc (fp)) != EOF)
    {
      if (c == '\n' || i == (WIDTH - 1))
	{
	  ret->down = getnode ();
	  (ret->down)->up = ret;
	  ret = ret->down;
	  i = 0;
	}
      else
	{
	  ret->array[i + 1] = ret->array[i];
	  ret->array[i] = c;
	  ret->count++;
	  i++;
	}
    }
  (ret->up)->down = NULL;
  free (ret->array);
  free (ret);
  ret = first;
  for (i = 0; i < HEIGHT && ret; i++)
    {
      wrow (i, ret->array);
      ret = ret->down;
    }
  addname ();
}

void newfile ()
{
  int i;
  char c;
  if (!saved)
    {
      c = displaymesg ();
      mrefresh ();
      if (c == 'y')
	{
	  touchwin (win);
	  wrefresh (win);
	  wrefresh (win);
	  savefile ();
	}
    }
  filename[0] = '\0';
  saved = 0;
  posx = posy = 0;
  line = 1;
  wmove (win, posy, posx);
  wclrtobot (win);
  touchwin (win);
  wrefresh (win);
  memclose ();
  LMARGIN = RMARGIN = BEGIN = END = 0;
  WIDTH = COLS - 2;
  HEIGHT = LINES - 4;
  first = getnode ();
  temp = first;
  delwin (win);
  win = newwin (HEIGHT, WIDTH, 2, 1);
  wbkgd (win, COLOR_PAIR (1));
  addname ();
  print_loc ();
}

void wrow (int y, char *arr)
{
  int i, j;
  for (i = 0; arr[i] != '\0'; i++)
    {
      wmove (win, y, i);
      waddch (win, arr[i]);
    }
}

void charinsert (chtype c)
{
  int i;
  if (temp->count == WIDTH - 1)
    beep ();
  else if (isprint (c))
    {
      saved = 0;
      for (i = temp->count; i >= posx; i--)
	temp->array[i + 1] = temp->array[i];
      temp->array[posx] = c;
      temp->count++;
      winsch (win, c);
      posx++;
    }
  else
    saved = 1;
  print_loc ();
}

void newline ()
{
  HEADER prev, next;
  int i, j;
  line++;
  saved = 0;
  prev = temp;
  next = temp->down;
  temp = getnode ();
  for (i = posx, j = 0; i <= prev->count; i++, j++)
    temp->array[j] = prev->array[i];
  prev->array[posx] = '\0';
  temp->count = prev->count - posx;
  prev->count = posx;
  temp->up = prev;
  temp->down = next;
  prev->down = temp;

  if (next)
    next->up = temp;
  wclrtoeol (win);
  posy++;
  if (posy >= HEIGHT)
    {
      wmove (win, 0, 0);
      wdeleteln (win);
      posy = HEIGHT - 1;
    }
  else
    {
      wmove (win, posy, posx);
      winsertln (win);
    }
  wrow (posy, temp->array);
  posx = 0;
  print_loc ();
}

void up ()				
{
  if (!temp->up)
     return;
  else
    {
      line--;
      temp = temp->up;
      posy--;
      posx = (posx > (temp->count)) ? temp->count : posx;
      if (posy < 0)
	{
	  wmove (win, 0, 0);
	  //winsertln (win);
	  wrow (0, temp->array);
	  posy = 0;
	}
    }
  print_loc ();
}

void down ()
{
  chtype ch;
  if (!temp->down)
    return;
  else
    {
      line++;
      temp = temp->down;
      posy++;
      posx = (posx > (temp->count)) ? temp->count : posx;
      if (posy >= HEIGHT)
	{
	  wmove (win, 0, 0);
	  wdeleteln (win);
	  wrow (HEIGHT - 1, temp->array);
	  posy = HEIGHT - 1;
	}
    }
  print_loc ();
}

void right ()
{
  if (posx == temp->count)
    beep ();
  else
    posx++;
  print_loc ();
}

void left ()
{
  if (posx == 0)
    beep ();
  else
    posx--;
  print_loc ();
}

void backspace ()
{
  if (posx == 0)
    beep ();
  else
    {
      saved = 0;
      posx--;
      wmove (win, posy, posx);
      dcur ();
    }
  print_loc ();
}

void dcur ()
{
  int i;
  if (posx == temp->count)
    beep ();
  else
    {
      saved = 0;
      for (i = posx; i < temp->count; i++)
	temp->array[i] = temp->array[i + 1];
      temp->count--;
      wdelch (win);
    }
}

void pageup()
{
    int x,y;
    getyx(win,y,x);
    posx=0;
    y=y-(y%21)-1;
    if(y==-1)
        y=0;
    posy=y;
    wmove(win,posx,posy);
    wrefresh(win);
    print_loc();
    return;
}

void pagedown()
{
    int x,y;
    getyx(win,y,x);
    posx=0;
    y=y+(21-y)-1;
    if((y+1)%21==0)
        y=20+(20*(int)(y/21));
    posy=y;
    wmove(win,posx,posy);
    wrefresh(win);
    print_loc();
    return;
}

void home()
{
    posx=0;
    wmove(win,posx,posy);
    wrefresh(win);
    print_loc();
    return;
}

void end()
{
    posx=temp->count-1;
    wmove(win,posx,posy);
    wrefresh(win);
    print_loc();
    return;
}

void help ()
{
  chtype w;
  WINDOW *he;
  chtype c;
  he = newwin (21, 50, 3, 20);
  wbkgd (he, COLOR_PAIR (2));
  wattrset (he, A_BOLD);
  wrefresh (he);
  mvwaddstr (he, 1, 15, "HELP MENU");
  mvwaddstr (he, 3, 2, "FILE MENU                     CTRL-F");
  mvwaddstr (he, 4, 2, "EDIT MENU	                CTRL-E");
  mvwaddstr (he, 7, 2, "HELP                          CTRL-H");
  mvwaddstr (he, 8, 2, "CURSOR UP                     UP ARROW");
  mvwaddstr (he, 9, 2, "CURSOR DOWN	                DOWN ARROW");
  mvwaddstr (he, 10, 2,"CURSOR RIGHT	                RIGHT ARROW");
  mvwaddstr (he, 11, 2,"CURSOR LEFT	                LEFT ARROW");
  mvwaddstr (he, 12, 2,"DELETE WORD             	CTRL-W");
  mvwaddstr (he, 13, 2,"DELETE LINE             	CTRL-L");
  mvwaddstr (he, 14, 2,"DELETE CHARACTER        	DELETE KEY");
  mvwaddstr (he, 15, 2,"COPY LINE      	        CTRL-I");
  mvwaddstr (he, 16, 2,"CUT LINE       	        CTRL-K");
  mvwaddstr (he, 17, 2,"PASTE LINE                    CTRL-G");
  mvwaddstr (he, 18, 2,"BEGINNING OF LINE             HOME KEY");
  mvwaddstr (he, 19, 2,"END OF LINE                   END KEY");
 
  wmove (he, 1, 1);
  wrefresh (he);
  keypad (he, TRUE);
  do
    {
      w = wgetch (he);
      wmove (he, 1, 1);
      wrefresh (he);
    }
  while (w != 27 && w != 10 && w != KEY_MOUSE);
  delwin (he);
  mrefresh ();
  if (ch == 27)
    return;
}

void editmenu ()
{
  WINDOW *edit;
  int i = 1, A = 0;
  chtype k;
  edit = newwin (9, 21, 1, 15);
  wbkgd (edit, COLOR_PAIR (2));
  wattrset (edit, A_BOLD);
  mvwaddstr (edit, 1, 1, "DELETE A LINE (^L)");
  mvwaddstr (edit, 2, 1, "DELETE WORD (^W)");
  mvwaddstr (edit, 3, 1, "DELETE CHAR (del)");
  mvwaddstr (edit, 4, 1, "INSERT CHAR");
  mvwaddstr (edit, 5, 1, "COPY (^I)");
  mvwaddstr (edit, 6, 1, "CUT (^K)");
  mvwaddstr (edit, 7, 1, "PASTE (^G)");
  wmove (edit, 1, 1);
  wrefresh (edit);
  keypad (edit, TRUE);

  do
    {
      k = wgetch (edit);
      switch (k)
	{
	case KEY_MOUSE:
	  if (getmouse (&event) == OK)
	    {
	      if (event.bstate && BUTTON2_PRESSED)
		A = 1;
	      if (event.bstate && BUTTON2_RELEASED)
		A = 2;
	      if (event.bstate && BUTTON1_CLICKED)
		A = 3;


	      switch (event.y)
		{
		case 1:
		  i = 1;
		  break;
		case 2:
		  i = 2;
		  break;
		case 3:
		  i = 3;
		  break;
		case 4:
		  i = 4;
		  break;
		case 5:
		  i = 5;
		  break;
		case 6:
		  i = 6;
		  break;
		case 7:
		  i = 7;
		  break;
		
		}

	    }


	case KEY_UP:
	  if (i != 1)
	    i--;
	  else
	    i = 7;
	  break;

	case KEY_DOWN:
	  if (i != 7)
	    i++;
	  else
	    i = 1;
	  break;
	default:
	  break;
	}
      wmove (edit, i, 1);
      wrefresh (edit);
    }
  while (k != 27 && k != 10 && A != 3);
  delwin (edit);
  mrefresh ();
  if (k == 27)
    return;
  switch (i)
    {
    case 1:
      deleteline ();
      break;
    case 2:
      deleteword ();
      break;
    case 3:
      dcur ();
      break;
    case 4:
      charinsert (ch);
      break;
    case 5:
      copyline ();
      break;
    case 6:
      cutline ();
      break;
    case 7:
      pasteline ();
      break;
    }
}

void dline ()
{
  int c;
  HEADER prev;
  HEADER tex = temp;
  prev = temp->up;
  for (c = posy; c < HEIGHT && tex; c++)
    {
      prev = tex;
      tex = tex->down;
    }
  wdeleteln (win);
  if (c == HEIGHT)
    wrow (HEIGHT - 1, prev->array);
  posx = 0;
}

void deleteline ()
{
  HEADER prev, next, tex;
  int c, i;
  saved = 0;
  if (!temp->up && !temp->down)
    {
      temp->count = 0;
      temp->array[0] = '\0';
      posx = 0;
      wmove (win, posy, posx);
      wclrtoeol (win);
      return;
    }
  if (!temp->down)
    {
      line--;
      print_loc ();
      prev = temp->up;
      free (temp->array);
      free (temp);
      temp = prev;
      temp->down = NULL;
      posx = 0;
      wmove (win, posy, posx);
      wclrtoeol (win);
      if (posy)
	posy--;
      else
	wrow (0, temp->array);
      return;
    }


  if (!(temp->up))
    {
      next = temp->down;
      free (temp->array);
      free (temp);
      temp = next;
      first = temp;
      temp->up = NULL;
      dline ();
      return;
    }
  prev = temp->up;
  next = temp->down;
  prev->down = next;
  next->up = prev;
  free (temp->array);
  free (temp);
  temp = next;
  dline ();
}

void deletechar ()
{
  int i;
  saved = 0;
  if (temp->count == 0)
    beep ();
  for (i = posx; i <= temp->count; i++)
    temp->array[i] = temp->array[i - 1];
  temp->count--;
  wdelch (win);
  print_loc ();
}

void deleteword ()
{
  int cnt, r1, j;
  cnt = posx;
  if (temp->array[posx] == '\0')
    {
      beep ();
      return;
    }
  if (temp->array[cnt] == ' ')
    {
      beep ();
      return;
    }
  else
    {
      if (temp->array[cnt - 1] != ' ' && temp->array[cnt - 1] != '\0')
	beep ();
      else
	{
	  while (temp->array[cnt] != ' ' && temp->array[cnt] != '\0')
	    {
	      cnt++;
	      temp->count--;
	      wdelch (win);
	    }
	}
    }
  for (j = cnt, r1 = posx; j < COLS - 2 - RMARGIN - LMARGIN; j++, r1++)
    temp->array[r1] = temp->array[j];
  wrefresh (win);
  saved = 0;
  return;
}

void helpmenu ()
{
  WINDOW *op1;
  chtype c;
  int i = 1, A = 0;
  op1 = newwin (4, 12, 1, 30);
  wattrset (op1, A_BOLD);
  wbkgd (op1, COLOR_PAIR (2));
  wattrset (op1, A_BOLD);
  mvwaddstr (op1, 1, 1, "SHORTCUTS");
  mvwaddstr (op1, 2, 1, "ABOUT");
  wmove (op1, 1, 1);
  wrefresh (op1);
  keypad (op1, TRUE);
  do
    {
      c = wgetch (op1);
      switch (c)
	{
	case KEY_MOUSE:
	  if (getmouse (&event) == OK)
	    {
	      if (event.bstate && BUTTON2_PRESSED)
		A = 1;
	      if (event.bstate && BUTTON2_RELEASED)
		A = 2;
	      if (event.bstate && BUTTON1_CLICKED)
		A = 3;

	      switch (event.y - 1)
		{
		case 1:
		  i = 1;
		  break;
		case 2:
		  i = 2;
		  break;

		}

	    }
	  break;
	case KEY_UP:
	  if (i == 2)
	    i--;
	  else
	    i = 1;
	  break;

	case KEY_DOWN:
	  if (i = 1)
	    i++;
	  else
	    i = 1;
	  break;
	default:
	  break;
	}
      wmove (op1, 1, 1);
      wrefresh (op1);
    }
  while (c != 27 && c != 10 && A != 3);
  delwin (op1);
  touchwin (w2);
  wrefresh (w2);
  mrefresh ();
  if (c == 27)
    return;
  wrefresh (w2);
  switch (i)
    {
    case 1:
      help ();
      break;
    case 2:
      about ();
      break;
    default:
      break;
    }
}

void about ()
{
  WINDOW *p;
  chtype ch;
  p = newwin (9, 28, 7, 26);
  wattrset (p, A_BOLD);
  wbkgd (p, COLOR_PAIR (2));
  wrefresh (p);
  wattrset (p, A_BOLD);
  mvwaddstr (p, 1, 2, "SS PROJECT-TEXT EDITOR");
  mvwaddstr (p, 2, 2, "ANISHA J PRASAD");
  mvwaddstr (p, 3, 2, "REG No:1RV12IS005");
  mvwaddstr (p, 4, 2, "ANUSHA M");
  mvwaddstr (p, 5, 2, "REG No:1RV12IS007");
  mvwaddstr (p, 6, 2, "5th Sem ISE dept");
  mvwaddstr (p, 7, 2, "RVCE");
  wmove (p, 1, 1);
  wrefresh (p);
  keypad (p, TRUE);
  do
    {
      ch = wgetch (p);
      wmove (p, 1, 1);
      wrefresh (p);
    }
  while (ch != 27 && ch != 10 && ch != KEY_MOUSE);
  delwin (p);
  mrefresh ();
  if (ch == 27)
    return;
}