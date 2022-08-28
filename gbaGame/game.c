#include "game.h"

#include <stdio.h>
#include <stdlib.h>

#include "gba.h"

#include "images/sunset.h"
#include "images/angry.h"
#include "images/sad.h"
#include "images/happy.h"

#include "time.h"


static volatile Player player;

typedef enum {
  START,
  PLAY,
  WIN,
  LOSE,
  WAITSTART,
} GBAState;

void playerMove(u32 buttonPressed) {
  if (KEY_DOWN(BUTTON_RIGHT, buttonPressed)) {
    if (player.col > 210) {
      player.col = player.col - 2;
    }
    else {
      player.col++;
      player.col++;
    }
  }
  if (KEY_DOWN(BUTTON_LEFT, buttonPressed)) {
    if (player.col < 3) {
      player.col = player.col + 2;
    }
    else {
      player.col--;
      player.col--;
      
    }
  }
  if (KEY_DOWN(BUTTON_UP, buttonPressed)) {
    if (player.row < 3) {
      player.row = player.row + 6;
    }
    else {
      player.row--;
      player.row--;
      
    }
  }
  if (KEY_DOWN(BUTTON_DOWN, buttonPressed)) {
    if (player.row > 130) {
      player.row = player.row - 6;
    }
    else {
      player.row++;
      player.row++;
     
    }
  }
}

int checkifGoal(int row) {
  if (player.col >= 0 && player.col <= 10) {
    if (player.row >= row && player.row <= (row+20)) {
      return 1;
    }
  }
  return 0;
}

int main(void) {
  REG_DISPCNT = MODE3 | BG2_ENABLE;

  u32 previousButtons = BUTTONS;
  u32 currentButtons = BUTTONS;

  GBAState state = START;

  int goal = 30;
  int initially_played = 0;
  int ended = 0;
  
  while (1) {
    currentButtons = BUTTONS;

    switch (state) {
      case START:
        waitForVBlank();
        drawFullScreenImageDMA(sunset);
        drawCenteredString(50,110,20,20, "WELCOME TO DA GAME", CYAN);
        drawCenteredString(70,110,20,20, "PUSH START TO PLAY!", CYAN);
        state = WAITSTART;
        player.col = 200;
        player.row = 0;
        player.moves = 0;
        int goalIndex = 0;
        goal = 30;
        break;

      case WAITSTART:
        if (KEY_DOWN(BUTTON_SELECT, currentButtons)) {
          state = START;
          break;
        }
        if (KEY_DOWN(BUTTON_START, currentButtons) && !KEY_DOWN(BUTTON_START, previousButtons)) {
          state = PLAY;
        }
        break;

      case PLAY:
        if (KEY_DOWN(BUTTON_SELECT, currentButtons)) {
          state = START;
          initially_played = 0;
          break;
        }
        waitForVBlank();

        if (initially_played == 0) {
          fillScreenDMA(BLACK);
          drawRectDMA(goal,0, 10, 10, GREEN);
          initially_played = 1;
        }
        drawRectDMA(player.row, player.col, ANGRY_WIDTH, ANGRY_HEIGHT, BLACK);
        playerMove(currentButtons);
        drawImageDMA(player.row,player.col,ANGRY_WIDTH,ANGRY_HEIGHT, angry);
        char goalsLeft[20];
        sprintf(goalsLeft, "Goals Left: %d", (5-goalIndex));
        drawString(10, 10, goalsLeft, BLUE);

        if (checkifGoal(goal) == 1) {
          goalIndex++;
          initially_played = 0;
          goal += 20;
          player.row = 0;
          player.col = 160;
          if (goalIndex > 4) {
            state = WIN;
          }
        }
        //clock_t begin = clock();
        //clock_t end = clock();
        //time = time + (end-begin)/ CLOCKS_PER_SEC;
        //if (time > 15) {
        //  state = LOSE;
        //}
        break;
      case WIN:
        if (KEY_DOWN(BUTTON_SELECT, currentButtons)) {
          state = START;
          initially_played = 0;
          ended = 0;
          break;
        }
        if (ended == 0) {
          waitForVBlank();
          drawFullScreenImageDMA(happy);
          drawCenteredString(70,110,20,20, "PRESS SELECT TO DO IT AGAIN :)", CYAN);
        }
        ended = 1;
        break;
      case LOSE:
        if (KEY_DOWN(BUTTON_SELECT, currentButtons)) {
          state = START;
          initially_played = 0;
          ended = 0;
          break;
        }
        if (ended == 0) {
          waitForVBlank();
          drawFullScreenImageDMA(sad);
          drawCenteredString(50,110,20,20, "OOF! PRESS SELECT TO TRY AGAIN :(", CYAN);
        }
        ended = 1;
        break;
    }

    previousButtons = currentButtons;
  }

  return 0;
}
