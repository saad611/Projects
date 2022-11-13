//Inspired by channel: https://www.youtube.com/user/NVitanovic

#include <iostream>
#include <conio.h>
using namespace std;

bool gameover;
const int w=20, h= 20;
int x,y, fruitX,fruity,score;
int tailX[100], tailY[100];
int nTail;

enum edirection 
{
  STOP=0,LEFT,RIGHT,UP,DOWN
};

edirection dir;

void Setup()
{
  gameover = false;
  dir = STOP;
  x = w/2;
  y = h/2;
  fruitX = rand()%w;
  fruity = rand()%h;
  score =0;
}

void Draw()
{
  cout << " " << endl;
  for(int i =0; i<w+2;i++)
  {
    cout << "#";
  }
  cout << endl;

  for(int i =0; i<h;i++)
  {
    for(int j=0; j<w;j++)
    {
        if(j==0)
        {
          cout << "#";
        }

        if(i==y && j==x)
        {
          cout << "O";
        }
          else if(i==fruity && j==fruitX)
          {
            cout << "F";
          }
      else 
        {
            bool print = false;
            for(int k=0; k < nTail;k++)
            {
                
                if(tailX[k]==j && tailY[k]==i)
                {
                  cout << "o";
                  print = true;
                }

            }

          if(!print)
              {
                 cout << " ";
              }
            
         
        }
      if(j==w-1)
      {
        cout << "#";
      }
    }
    cout << endl;
  }
    

  for(int i=0;i<w+2;i++)
  {
    cout<< "#";
  }
  cout << endl;
  cout << "Score:"<< score << endl;
}

void Input()
{
  if(_kbhit())
  {
    switch(_getch())
    {
      case 'a':
        dir = LEFT;
      break;

      case 'd':
        dir = RIGHT;
      break;

      case 'w':
        dir = UP;
      break;

      case 's':
        dir = DOWN;
      break;

      case 'x':
        gameover = true;
      break;

      default:
      break;
      
    }
  }
}


void Logic()
{
  int prevX= tailX[0];
  int prevY = tailY[0];
  int prev2X, prev2Y;
  tailX[0]=x;
  tailY[0]=y;

  for(int i=1; i<nTail;i++)
  {
    prev2X = tailX[i];
    prev2Y = tailY[i];
    tailX[i]=prevX;
    tailY[i]=prevY;
    prevX = prev2X;
    prevY= prev2Y;
  }


  
  switch(dir)
  {
    case LEFT:
      x--;
    break;

    case RIGHT:
      x++;
    break;

    case UP:
      y--;
    break;

    case DOWN:
      y++;  
    break;

    default:
    break;
    
  }

  if(x>w || x<0 || y >h || y<0)
  {
    
    gameover=true;
  }

  for(int i=0; i<nTail;i++)
  {
    if(tailX[i]==x && tailY[i]==y)
    {
      gameover = true;
    }
  }


  if(x== fruitX && y==fruity)
  {
   
    score+=10;
    fruitX = rand()%w;
    fruity = rand()%h;
     nTail++;
  }
}

int main() {
  //std::cout << "Hello World!\n";
  Setup();
  while(!gameover)
  {
    Draw();
    Input();
    Logic();
    
  }
  
  
  }