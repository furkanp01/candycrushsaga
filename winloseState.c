#include "winloseState.h"
#include "menu.h"

bool winSoundPlayed = false;
bool gameoverSoundPlayed = false;

void drawWin(void) {
    ClearBackground(DARKGREEN);
    int currentscreenWidth = GetScreenWidth();
    int currentscreenHeight = GetScreenHeight();

    if (!winSoundPlayed) {
        PlaySound(resources.winSound);
        winSoundPlayed = true; 
    }

    DrawText("You Win!", currentscreenWidth / 2 - 100, currentscreenHeight / 2 - 50, 50, YELLOW);
    DrawText("Press SPACE to return to level screen.", (currentscreenWidth / 2) - 270, currentscreenHeight / 2 + 10, 30, WHITE);

    if (IsKeyPressed(KEY_SPACE)) {
        currentState = LEVELS;
        winSoundPlayed = false; 
    }
}

void drawgameOver(void) {
    ClearBackground(RED);
    int currentscreenWidth = GetScreenWidth();
    int currentscreenHeight = GetScreenHeight();

    if (!gameoverSoundPlayed) {
        PlaySound(resources.gameoverSound);
        gameoverSoundPlayed = true;
    }

    DrawText("Game Over!", currentscreenWidth / 2 - 150, currentscreenHeight / 2 - 50, 50, WHITE);
    DrawText("Press SPACE to return to level screen.", currentscreenWidth / 2 - 320, currentscreenHeight / 2 + 10, 30, WHITE);

    if (IsKeyPressed(KEY_SPACE)) {
        currentState = LEVELS;
        gameoverSoundPlayed = false;
    }
}
