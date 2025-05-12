#include "state.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

/* Helper function definitions */
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_state_t* state, unsigned int snum);
static char next_square(game_state_t* state, unsigned int snum);
static void update_tail(game_state_t* state, unsigned int snum);
static void update_head(game_state_t* state, unsigned int snum);
/* Task 1 */
game_state_t* create_default_state() {
  // TODO: Implement this function.
  game_state_t* default_game =(game_state_t*) malloc(sizeof(game_state_t));
  default_game -> num_rows = 18;
  default_game -> board = (char**)malloc(sizeof(char*) * default_game->num_rows);
  //num of rows^
  default_game -> num_snakes = 1;
  default_game ->snakes = (snake_t*) malloc(default_game -> num_snakes * sizeof(snake_t));
  default_game ->snakes[0].live = true; //had a [0] before the .live
  //default_game ->board[] = (char*) malloc(sizeof(char) * 21);
  char* boardcpy[] ={"####################",
                     "#                  #",
                     "# d>D    *         #",
                     "#                  #",
                     "#                  #",
                     "#                  #",
                     "#                  #",
                     "#                  #",
                     "#                  #",
                     "#                  #",
                     "#                  #",
                     "#                  #",
                     "#                  #",
                     "#                  #",
                     "#                  #",
                     "#                  #",
                     "#                  #",
                     "####################"};   
   int i;
   for (i = 0; i < default_game->num_rows;i++) {
       default_game->board[i] = (char*) malloc(sizeof(char) * 21);//cols
       strcpy(default_game->board[i],boardcpy[i]);
       if (default_game->board[i] == NULL) {
           free(default_game->board);
       }
    }
   default_game->snakes[0].tail_row = 2;
   default_game->snakes[0].tail_col = 2;
   default_game->snakes[0].head_row = 2;
   default_game->snakes[0].head_col = 4;
   snake_t* snake = (default_game->snakes);
   //vim
  return default_game;
}

/* Task 2 */
void free_state(game_state_t* state) {
    free(state->snakes);
    
    int i;
    for (i = 0; i < state->num_rows; i++) {
        free(state->board[i]);
    }
    free(state->board);
    free(state);
}

/* Task 3 */
void print_board(game_state_t* state, FILE* fp) {

  for (int i = 0; i < state->num_rows; i++) {
      fprintf(fp,"%s\n", state->board[i]);
  }
}

/*
  Saves the current state into filename. Does not modify the state object.
  (already implemented for you).
*/
void save_board(game_state_t* state, char* filename) {
  FILE* f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Task 4.1 */

/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_state_t* state, unsigned int row, unsigned int col) {
  return state->board[row][col];
}

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch) {
  state->board[row][col] = ch;
}

/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c) {
    if (c == 'w' || c == 'a' || c == 's' || c == 'd') {
        return true;
    }
    return false;
}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c) {
    if (c == 'W' || c == 'A' || c == 'S' || c == 'D' || c == 'x') {
        return true;
    }
    return false;
}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
    
  if (is_head(c) || is_tail(c)) {
      return true;
  } else if (body_to_tail(c) != '?') {
      return true;
  }
  return false;
}

/*
  Converts a character in the snake's body ("^<v>")
  to the matching character representing the snake's
  tail ("wasd").
*/
static char body_to_tail(char c) {
  if (c == '^') {
      return 'w';
  }
  if (c == '<') {
      return 'a';
  }
  if (c == 'v') {
      return 's';
  }
  if (c == '>') {
      return 'd';
  }
  return '?';
}

/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<v>").
*/
static char head_to_body(char c) {
  if (c == 'W') {
      return '^';
  }
  if (c == 'A') {
      return '<';
  }
  if (c == 'S') {
      return 'v';
  }
  if (c == 'D') {
      return '>';
  }
  return '?';
}

/*
  Returns cur_row + 1 if c is 'v' or 's' or 'S'.
  Returns cur_row - 1 if c is '^' or 'w' or 'W'.
  Returns cur_row otherwise.
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  if (c == 'v' || c == 's' || c == 'S') {
      cur_row = cur_row + 1;
  }
  if (c == '^' || c =='w' || c == 'W') {
      cur_row = cur_row - 1;
  }
  return cur_row;
}

/*
  Returns cur_col + 1 if c is '>' or 'd' or 'D'.
  Returns cur_col - 1 if c is '<' or 'a' or 'A'.
  Returns cur_col otherwise.
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  // TODO: Implement this function.
  if (c == '>' || c == 'd' || c == 'D') {
      cur_col = cur_col + 1;
  }
  if (c == '<' || c == 'a' || c == 'A') {
      cur_col = cur_col - 1;
  }
  return cur_col;
}

/*
  Task 4.2

  Helper function for update_state. Return the character in the cell the snake is moving into.

  This function should not modify anything.
*/
static char next_square(game_state_t* state, unsigned int snum) {
    
    unsigned int row = state->snakes[snum].head_row;
    unsigned int col = state->snakes[snum].head_col;
    char c = state->board[row][col];
    char updated;
    

    if (c == 'W' || c == 'S') {
        unsigned int newrow = get_next_row(row, c);
        updated = get_board_at(state, newrow, col);
        return updated;

    }
    if (c == 'A' || c == 'D') {
        unsigned int newcol = get_next_col(col, c);
        updated = get_board_at(state, row, newcol);
        return updated;

    }
    return '?';
  //
}

/*
  Task 4.3

  Helper function for update_state. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the row and col of the head

  Note that this function ignores food, walls, and snake bodies when moving the head.
*/
static void update_head(game_state_t* state, unsigned int snum) {
    


    snake_t* snek = &state->snakes[snum];
    char c = get_board_at(state, snek->head_row, snek->head_col);
    state->board[snek->head_row][snek->head_col] = head_to_body(c);
    snek->head_col = get_next_col(snek->head_col, c);
    snek->head_row = get_next_row(snek->head_row, c);

    unsigned int newrow = snek->head_row;
    unsigned int newcol = snek->head_col;

    state->board[newrow][newcol] = c;
}

/*
  Task 4.4

  Helper function for update_state. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^<v>) into a tail character (wasd)

  ...in the snake struct: update the row and col of the tail
*/
static void update_tail(game_state_t* state, unsigned int snum) {
    unsigned int row = state->snakes[snum].tail_row;
    unsigned int col = state->snakes[snum].tail_col;

    char tail = state->board[row][col];


    if (tail == 'a' || tail == 'd') {
        state->board[row][col] = ' ';
        unsigned int nextcol = get_next_col(col, tail);
        tail = body_to_tail(state->board[row][nextcol]);
        state->board[row][nextcol] = tail;
        state->snakes[snum].tail_col = nextcol;
    }
    else if (tail == 'w' || tail == 's') {
        state->board[row][col] = ' ';
        unsigned int nextrow = get_next_row(row, tail);
        tail = body_to_tail(state->board[nextrow][col]);
        state->board[nextrow][col] = tail;
        state->snakes[snum].tail_row = nextrow;
    }

}

/* Task 4.5 */
void update_state(game_state_t* state, int (*add_food)(game_state_t* state)) {


    for (unsigned int i = 0; i < state->num_snakes; i++) {

        snake_t* snek = &state->snakes[i];
        
        char temp = next_square(state, i);
        if (snek->live == true) {

            if (temp == '#' || is_snake(temp)) {
                
                state->board[snek->head_row][snek->head_col] = 'x';
                snek->live = false;
            } else if (temp == '*') {
                update_head(state, i);
                add_food(state);

            } else {
                update_head(state, i);
                update_tail(state, i);
            }
        }
    }
}

/* Task 5 */
game_state_t* load_board(FILE* fp) {

    game_state_t* secondboard = (game_state_t*) malloc(sizeof(game_state_t));
    secondboard->num_rows = 1;
    secondboard->num_snakes = 0;
    secondboard->snakes = NULL;
    secondboard->board = (char**) malloc(sizeof(char*) * secondboard->num_rows);
    secondboard->board[secondboard->num_rows - 1] = NULL;
    int pos = 0;
    //char* storechar = malloc(sizeof(char));

    //null check
    if (fp == NULL) {
        return NULL;
    }
    char count = (char) fgetc(fp);

    //loop to iterate through file
    while (count != EOF) {
        
        //move these lines 
        //secondboard->board[second->num_rows] = (char*) malloc(sizeof(char) * secondboard->num_rows);
        //secondboard->board[secondboard->num_rows - 1][pos] = count;
        if (count != '\n') {
            //strlen(secondboard->board[secondboard->num_rows])
            //secondboard->board[secondboard->num_rows] = (char**) malloc(sizeof(strlen(secondboard->num_rows)));
            secondboard->board[secondboard->num_rows - 1] = realloc(secondboard->board[secondboard->num_rows - 1], pos + 1);
            secondboard->board[secondboard->num_rows - 1][pos] = count;
            //secondboard->board = realloc(secondboard->board, secondboard->num_rows * pos);
            pos += 1;
            count = (char) fgetc(fp);
        } else {
            //first line reallocate curr row
            //second line null terminate curr row
            //allocate space in board for new row
            //initialze new row using malloc

            secondboard->board[secondboard->num_rows - 1] = realloc(secondboard->board[secondboard->num_rows - 1], pos + 1);
            secondboard->board[secondboard->num_rows - 1][pos] = '\0';
            secondboard->num_rows += 1;
            secondboard->board = realloc(secondboard->board, secondboard->num_rows * sizeof(char*));
            secondboard->board[secondboard->num_rows - 1] = NULL;
            pos = 0;
            count = (char) fgetc(fp);
        }
    }
    secondboard->num_rows -= 1;
    return secondboard;

    //here i essentially tried two different implementations, this one i essentially try
    //and store the file in an array
    //char* storedwithchar;
    //int pos = 0;
    //int curr = 0;
    //from there i try and break apart the file into rows everytime i see the '\n operator'
    //from here i try to reallocate the memory everytime i loop through
    //i use the strtok method i found from stack overflow to be able to split the array
    //
    //
    //char count = getc(fp);
    //while (count != EOF) {

        //storechar[pos] = count;
        
        //if (count == '\n') {
            //secondboard->num_rows += 1;
        //}
        //storechar = realloc(storechar, sizeof(char) * pos);
        //pos++;
    //}
    //secondboard->board = (char*) malloc(sizeof(char*));
    //storechar = realloc(storechar, sizeof(char) * pos);

    //char * split = strtok(storechar, '\n');

    //while (split != NULL) {
        //secondboard->board[curr] = malloc(sizeof(split));
        //curr++;
        //storechar = strtok(NULL, '\n');
    //}
    //strcpy(secondboard->board[curr], split);


    //this method i essentially traverse through the file through two seperate loops and from
    //there add in the characters each iteration
    //inner loop is meant to traverse through the rows and outer the columns
}

/*
  Task 6.1

  Helper function for initialize_snakes.
  Given a snake struct with the tail row and col filled in,
  trace through the board to find the head row and col, and
  fill in the head row and col in the struct.
*/
static void find_head(game_state_t* state, unsigned int snum) {
    unsigned int tailrow = state->snakes[snum].tail_row;
    unsigned int tailcol = state->snakes[snum].tail_col;
    unsigned int currow = tailrow;
    unsigned int curcol = tailcol;

    char curr = get_board_at(state, currow, curcol);
    while (!is_head(curr)) {
        currow = get_next_row(currow, curr);
        curcol = get_next_col(curcol, curr);
        curr = get_board_at(state, currow, curcol);
    }
    state->snakes[snum].head_row = currow;
    state->snakes[snum].head_col = curcol;

}

/* Task 6.2 */
game_state_t* initialize_snakes(game_state_t* state) {
    unsigned int count = 0;
    state->snakes = malloc(sizeof(snake_t));
    //state->snakes = malloc(sizeof(struct snake_t));
    for (unsigned int i = 0; i < state->num_rows; i++) {

        long unsigned int scan = strlen(state->board[i]);
        for (unsigned int j = 0; j < scan ; j++) {

            char temp = get_board_at(state, i ,j);
            if (is_tail(temp)) {
                if (count >= 1) {
                    state->snakes = realloc(state->snakes, (count + 1) * sizeof(snake_t));
                }
                snake_t* secondsnake = &state->snakes[count];
                secondsnake->live = true;
                secondsnake = &state->snakes[count];
                secondsnake->tail_row = i;
                secondsnake->tail_col = j;
                find_head(state, count);

                count++;
            }
        }
    }
    state->num_snakes = count;
    return state;
}
