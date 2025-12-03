#include <iostream>
#include <unistd.h>
#include <termios.h>
#include <cstdlib> // For atexit
using namespace std;
struct termios original_termios;

void restoreTerminal() {
    tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);
}

void hideCursor() {
    cout << "\033[?25l";
}

void showCursor() {
    cout << "\033[?25h";
}

char getch_linux() {
    static bool first_call = true;

    if (first_call) {
        tcgetattr(STDIN_FILENO, &original_termios);
        
        atexit(restoreTerminal);
        atexit(showCursor);

        struct termios new_termios = original_termios;
        new_termios.c_lflag &= ~(ICANON | ECHO); // Raw mode
        new_termios.c_cc[VMIN] = 0;  // Return immediately if no input
        new_termios.c_cc[VTIME] = 0; // No wait time
        
        tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
        
        first_call = false;
    }

    char c = 0;
    read(STDIN_FILENO, &c, 1);
    
    return c;
}

void renderScreen(char screen[][49], int height, int width,int ammo,int score,int level) {
   
   cout << "\033[H"; // Move cursor to home position
   cout<<"====================LEVEL : "<<level<<" ================="<<endl<<endl;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if((y==1||y==2)&&(x>5&&x<width-5))
              {char ch_brick=screen[y][x];
              if(ch_brick=='$')
                 cout<<"\033[94m"<<ch_brick<<"\033[0m";
              else if(ch_brick=='!')
                 cout<<"\033[96m"<<ch_brick<<"\033[0m";
              else if(ch_brick=='*')
                 cout<<"\033[95m"<<ch_brick<<"\033[0m";
              else if(ch_brick=='#')
                 cout<<"\033[91m"<<ch_brick<<"\033[0m";
              else if(ch_brick=='=')
                 cout<<"\033[93m"<<ch_brick<<"\033[0m";
              else if(ch_brick=='-')
                 cout<<"\033[92m"<<ch_brick<<"\033[0m";
              else
                cout<<ch_brick;
              }
            else
            cout << screen[y][x];
            
        }
        if (y < height - 1) {
            cout << "\n";
        }
    }
   cout<<"\nammo="<<ammo<<"\t\tscore:"<<score;
}

void brick_modify(char bricks[][36],int b_row,int b_col,int &score)
{ char ch;
  int check=0;
       // bricks[0][0]=screen[1][6];      so as their is offset
   b_col=b_col-6;   //ta k offset/difference between bulletX on(screen) && w.r.t bricks khatam hojai   
   b_row=b_row-1;       //same waja jo b_col   k liye ki
   
    for(int x = 0; x <= 4; x++) {
    int col = b_col + x;         //4 bricks k liye kaam krna hai to variable banaya extra
    if(col < 0 || col >= 36) 
       continue;                //ta k out of bound kaam kharab na ho;
    ch = bricks[b_row][col];
     
     if(ch == '$')
       {ch= '!';
       check++;
      }   
    else if(ch == '!')
       {ch= '*';
       check++;
      }
    else if(ch == '*')
       {ch= '#';
       check++;
      }
    else if(ch == '#')
       {ch= '=';
       check++;
      }
    else if(ch == '=') 
      {ch = '-';
      check++;
      }
    else if(ch == '-') 
      {ch = ' ';
      check++;
      }
    else      //else mein space hai to kuch nhi ho ga;
      ch=' ';
      
     bricks[b_row][col]=ch;
     
    
}

     if(check!=0)
      score+=10;
     
 
}
void obstacle(char screen[][49],int height,int width,int level)  //for making levels
{
  switch(level)
  {
  case 10: 
      screen[height-6][width/2+5]='M';
       screen[height-6][width/2]='L';
  case 9:
       screen[height-9][width/2+9]='_';
       screen[height-9][width/2-9]='|';
  case 8:
      screen[height-17][width-20]='%';
       screen[height-17][20]='%';
  case 7:
      screen[height-17][width-10]='%';
       screen[height-17][10]='%';
  case 6:
      screen[height-9][width-2]='%';
       screen[height-9][2]='%';
  case 5:
       screen[height-5][width/2-10]='%';
       screen[height-5][width/2+10]='%';
  case 4:
       screen[height-12][width-5]='%';
       screen[height-12][5]='%';
  case 3:
       screen[height-7][width-20]='%';
       screen[height-7][5]='%';
  case 2:
       screen[height-13][width-6]='_';
       screen[height-13][4]='|';
  case 1:
       screen[height-10][width-14]='|';
       screen[height-10][14]='|';
       break;
    }
}

char brick_block_evaluator(int level)    //make type of bricke more difficult to break on basis of levels
{
char block;
if(level>7&&level<=10)
  block='$';
else if(level>5&&level<=7)
      block='!';
 else if(level>3&&level<=5)
      block='*';
 else
   block='#';
   
  return block;
}

void runGame(char screen[][49], int &cannonX, bool &gameRunning, int height, int width, int cannonY,int &ammo,int &score,int &level,bool &result)
{   
    if(level>1)
    ammo=ammo+level*15;
    
    
    gameRunning = true;
    cannonX = width / 2;
     static bool up=false,down=false,left=false,right=false;
     char canonboders='|';
    static int bulletY=cannonY,bulletX=cannonX;
    char cannon = 'o'; // the cannon
    screen[cannonY][cannonX-1] = canonboders;
    screen[cannonY][cannonX+1] = canonboders;
    screen[bulletY][bulletX] = 'o';
    const int brick_row = 2, brick_col = 36;
    
    char brick_block= brick_block_evaluator(level);   //makes brick type on basis of condition (level)
    
    char bricks[brick_row][brick_col]; // the bricks
    for (int i = 0; i < brick_row; i++)
        for (int j = 0; j < brick_col; j++)
            bricks[i][j] = brick_block;

    while (gameRunning)
    {
       
    
        char c = getch_linux();

        if (c == 'a' || c == 'A')
        {
            if (cannonX - 2 > 0)
                cannonX--;
            if(!up&&!down&&!right&&!left)
               bulletX--;
               bulletX=(bulletX<2)?2:bulletX;
        }
        else if (c == 'd' || c == 'D')
        {
            if (cannonX + 2 < width - 1)
                cannonX++;
            if(!up&&!down&&!right&&!left)
               bulletX++;     
               bulletX=(bulletX>width-3)?width-3:bulletX;
        }
        else if (c == 'q' || c == 27)
        {
            gameRunning = false;
        }
        else if (c == 'k' || c == 'K')
        {
            canonboders= '/';
            canonboders= '/';
        }
        else if (c == 'J' || c == 'j')
        {
            canonboders= '\\';
            canonboders= '\\';
        }
        else if (c == 'w' || c == 'W')
        {
             canonboders= '|';
            canonboders= '|';
        }
        else if ((c == ' ')&&(!up&&!down&&!right&&!left))
        {  
            if (canonboders == '|')
            {
               up=true;
            }
            else if (canonboders == '/')
            {
               right=true; up=true;
            }
            else if (canonboders == '\\')
            {
               left=true; up=true;
            }
        }
  

        if(up)
        { 
         screen[bulletY][bulletX]=' ';
         bulletY--;
         screen[bulletY][bulletX]='o';
        }
       if(left)
       { 
         screen[bulletY][bulletX]=' ';
         bulletX--;
         screen[bulletY][bulletX]='o';
        }
       if(right)
       { 
         screen[bulletY][bulletX]=' ';
         bulletX++;
         screen[bulletY][bulletX]='o';
        }
       if(down)
       { 
         screen[bulletY][bulletX]=' ';
         bulletY++;
         screen[bulletY][bulletX]='o';
        }
        
        //checking if any side collision
        if(screen[bulletY-1][bulletX]!=' '&&up)   //moving up
        {
         down=true;
         up=false;
        
         if(bulletX>5&&bulletX<width-6)     
           if(bulletY-1>0&&bulletY-1<=brick_row)  
             brick_modify(bricks,bulletY-1,bulletX,score);      
        }
        if(screen[bulletY+1][bulletX]!=' '&& down)  //moving down
        {
         
        if(bulletY+1==height-1)
        { down=false;
         up=false;
         left=false;
         right=false;
         bulletX=cannonX;
         bulletY=cannonY;
         ammo--;
         continue;
         }
         
         down=false;
         up=true; 
        
        }
        if(screen[bulletY][bulletX-1]!=' '&&left&&(up||down))   //moving left
        {
         left=false;
         right=true;
        
         if(bulletX-1>5&&bulletX-1<width-6)     
           if(bulletY>0&&bulletY<=brick_row)  
             brick_modify(bricks,bulletY,bulletX-1,score);      
        }
        else if(screen[bulletY-1][bulletX-1]!=' '&&left&&up)   //moving left
        {
         left=false;
         right=true;
        
         if(bulletX-1>5&&bulletX-1<width-6)     
           if(bulletY-1>0&&bulletY-1<=brick_row)  
             brick_modify(bricks,bulletY-1,bulletX-1,score);      
        }
        else if(screen[bulletY+1][bulletX-1]!=' '&&left&&down)   //moving left
        {
         left=false;
         right=true;
        
         if(bulletX-1>5&&bulletX-1<width-6)     
           if(bulletY+1>0&&bulletY+1<=brick_row)  
             brick_modify(bricks,bulletY+1,bulletX-1,score);      
        }
        if(screen[bulletY][bulletX+1]!=' '&&right&&(up||down))   //moving right
        {
         left=true;
         right=false;
        
         if(bulletX+1>5&&bulletX+1<width-6)     
           if(bulletY>0&&bulletY<=brick_row)  
             brick_modify(bricks,bulletY,bulletX+1,score);      
        }
        else if(screen[bulletY-1][bulletX+1]!=' '&&right&&up)   //moving right
        {
         left=true;
         right=false;
        
         if(bulletX+1>5&&bulletX+1<width-6)     
           if(bulletY-1>0&&bulletY-1<=brick_row)  
             brick_modify(bricks,bulletY-1,bulletX+1,score);      
        }
        else if(screen[bulletY+1][bulletX+1]!=' '&&right&&down)   //moving right
        {
         left=true;
         right=false;
        
         if(bulletX-1>5&&bulletX-1<width-6)     
           if(bulletY+1>0&&bulletY+1<=brick_row)  
             brick_modify(bricks,bulletY+1,bulletX+1,score);      
        }     
        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                screen[y][x] = ' ';
            }
            screen[y][width] = '\0';
        }

        for (int y = 0; y < height; ++y)
        {
            if (y == 0 || y == height - 1)
            {
                for (int x = 0; x < width; ++x)
                {
                    screen[y][x] = '#';
                }
            }
            else
            {
                screen[y][0] = '#';
                screen[y][width - 1] = '#';
            }
        }
int check=0;
        for (int i = 0; i < brick_row; i++)
            for (int j = 0; j < brick_col; j++)
                {if(bricks[i][j]!=' ')
                   check++;
                 }
              

        // displaying the bricks
        for (int i = 0; i < brick_row; i++)
            for (int j = 0; j < brick_col; j++)
                screen[i + 1][j + 6] = bricks[i][j];
        obstacle(screen,height,width,level);
        screen[cannonY][cannonX - 1] = canonboders;
        screen[bulletY][bulletX] = cannon;
        screen[cannonY][cannonX + 1] = canonboders;

        renderScreen(screen, height, width,ammo,score,level);
      if(check==0||ammo==0)
	{ if(check<ammo||check==ammo)
   	result=true;
  	else
   	result=false;
   
   
  	break;
	}
        usleep(16000);
    }
}
int main() {
    system("clear");  //also using clear hear to make clear output
    const int SCREEN_HEIGHT = 25;
    const int SCREEN_WIDTH = 48; 
    const int CANNON_Y = 23;
    int ammo=30;
    int score=0;
    int level=2;
    bool result;
    cout<<"Enter Level   (1----10)=";
    while(level<0||level>10)
    {
    
    cout<<"Level not made\nRe Enter in provided levels=";
    cin>>level;
    }
    cin>>level;
    int cannonX=SCREEN_WIDTH/2;
    bool gameRunning;

    char screen[SCREEN_HEIGHT][SCREEN_WIDTH + 1];

    system("clear");
    hideCursor();
    
    runGame(screen, cannonX, gameRunning, 
            SCREEN_HEIGHT, SCREEN_WIDTH, CANNON_Y,ammo,score,level,result);
  
    system("clear");
    for (int i = 0; i < 10; ++i) 
    cout << "\n";
    cout << "                      Game Closed.\n";
    
    for (int i = 0; i < 10; ++i) 
    cout << "\n";
    system("clear");   //using also hear for clean output;
    for (int i = 0; i < 10; ++i) 
    cout << "\n";
    if(result)
    cout<<"You won"<<endl;
    else
    cout<<"You lost"<<endl;
    for (int i = 0; i < 10; ++i) 
    cout << "\n";
    return 0;
}


