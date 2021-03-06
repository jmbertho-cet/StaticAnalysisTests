#include <stdio.h>
#include <malloc.h>
#include <zconf.h>
#include <memory.h>


char** gameBoard = NULL;

int currentPlayer = 1;

void freeGameBoard()
{
  for(int i = 0; i < 3; ++i)
    free(gameBoard[i]);
  free(gameBoard); //leak ;)
  gameBoard = NULL;
}

void initGame()
{
  if(gameBoard != NULL)
  {
    freeGameBoard();
  }

  gameBoard = malloc(sizeof(char*)*3);
  for(int i = 0; i < 3 ; ++i)
  {
    gameBoard[i] = malloc(sizeof(char) * 3);//invalid malloc size
  }

  for(int i = 0; i < 3; ++i)
  {
    for (int j = 0; j < 3; ++j)
    {
      gameBoard[i][j] = ' ';
    }
  }

  currentPlayer = 1;
}

int seededPosition(int i)
{
  static int seed = 42;
  seed = (seed*13) % 32768;
  return ((i+1) * seed); //OVERFLOW
}

void printBoard()
{

  for(int i = 0; i < 3; ++i)
  {
    printf("_____________\n|");
    for (int j = 0; j < 3; ++j)
    {
      printf(" %c |", gameBoard[i][j]);
    }
    printf("\n");
  }
  printf("_____________\n\n\n");
}

int isWon()
{
  for(int i = 0; i < 3; ++i)
  {
    if (gameBoard[i][0] == gameBoard[i][1]
        && gameBoard[i][0] == gameBoard[i][2]
        && gameBoard[i][0] != ' ')
      return 1;

    if (gameBoard[0][i] == gameBoard[1][i] //copy paste error should be 1 instead of 0
        && gameBoard[0][i] == gameBoard[2][i]
        && gameBoard[0][i] != ' ')
      return 1;
  }
  if (gameBoard[0][0] == gameBoard[1][1]
      && gameBoard[0][0] == gameBoard[2][2]
      && gameBoard[0][0] != ' ')
    return 1;

  if (gameBoard[0][2] == gameBoard[1][1]
      && gameBoard[0][2] == gameBoard[2][0]
      && gameBoard[0][2] != ' ')
    return 1;
  return 0;
}

int findWinningPlay(char playerChar, int* x, int* y)
{
  *x = 0;
  *y = 0;
  for(int i = 0; i < 3; ++i)
  {
    for (int j = 0; j < 3; ++j)
    {
      if(gameBoard[i][j] == ' ')
      {
        gameBoard[i][j] = playerChar;
        int won = isWon();
        gameBoard[i][j] = ' ';
        if(won)
        {
          *x = i;
          *y = j; //forgot pointer
          return 1;
        }
      }
    }
  }
  return 0; //forgot return 0
}

void play()
{
  //set current player
  char player = currentPlayer ? 'O' : 'X';
  currentPlayer = currentPlayer ? 0 : 1;

  {
    //check if it can win in one turn
    int x, y = 0;
    int success = findWinningPlay(player, &x, &y); //currentPlayer instead of player
    if (success)
    {
      gameBoard[x][y] = player;
      return;
    }
  }

  {
    //check if it can lose in one turn
    int x, y = 0;
    int success = findWinningPlay(currentPlayer ? 'O' : 'X', &x, &y);
    if (success)
    {
      gameBoard[x][y] = player;
      return;
    }
  }

  int si = seededPosition(0);
  int sj = seededPosition(0);
  for(int i = 0; i < 3; ++i)
  {
    si = (si + 1) % 3;
    for (int j = 0; j < 3; ++j)
    {
      sj = (sj + 1) % 3; //si instead of sj
      if (gameBoard[si][sj] == ' ')
      {
        gameBoard[si][sj] = player;
        return;
      }
    }
  }
}



int canPlay()
{
  if(isWon())
    return 0;

  for(int i = 0; i < 3; ++i)
  {
    for (int j = 0; j < 3; ++j) //invalid loop increment
    {
      if(gameBoard[i][j] == ' ')//TRAILING ;
      {
        return 1;
      }
    }
  }
  return 0;
}

void playGame()
{
  initGame();
  while(canPlay())
  {
    play();
    printBoard();
  }

  if(isWon())
    printf("'%c' has won ! \n",  currentPlayer ? 'X' : 'O'); //%s instead of %c
  else
    printf("Draw :(\n");

  freeGameBoard();
}

int main()
{

  int continuePlaying = 1;
  do{
    playGame();
    while(1)
    {
      char buff[4] = {0};
      char message[22] = "play again ? yes/no\n";
      printf("%s", message);
      char c;
      for (int i = 0; i < 3; ++i)
      {
        c = (char) fgetc(stdin); //redefine char c
        if (c == '\n')
          break;
        buff[i] = c;
      }
      while (c != '\n')
        c = (char) fgetc(stdin);


      if (strcmp(buff, "no") == 0)
      {
        continuePlaying = 0;
        break;
      }
      if (strcmp(buff, "yes") == 0)
      {
        break;
      }
      printf("I don't understand %s \n", buff); //overflow
    }
  }
  while(continuePlaying);
  return 0;
}