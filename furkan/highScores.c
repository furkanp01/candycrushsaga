#include "raylib.h"
#include <stdio.h>
#include "main.h"
#include "highScores.h"

int highScores[maxScores];
int scoreCount = 0;

void addHighScore(int score) {

    if (score > highScores[maxScores - 1]) {

        int i = maxScores - 1;
        while (i > 0 && highScores[i - 1] < score) {
            highScores[i] = highScores[i - 1];
            i--;
        }
        highScores[i] = score;
    }
}

void loadHighScores() {
    FILE* xPtr = fopen("highScores.txt", "r");
    if (!xPtr) {

        for (int i = 0; i < maxScores; i++) {
            highScores[i] = 0;
        }
        scoreCount = maxScores;
        return;
    }
    for (int i = 0; i < maxScores; i++) {
        if (fscanf(xPtr, "%d", &highScores[i]) != 1) {
            highScores[i] = 0;
        }
    }
    fclose(xPtr);
    scoreCount = maxScores;
}

void saveHighScores() {
    FILE* xPtr = fopen("highScores.txt", "w");
    if (!xPtr) return;
    for (int i = 0; i < maxScores; i++) {
        fprintf(xPtr, "%d\n", highScores[i]);
    }
    fclose(xPtr);
}

void drawHighScoresScreen() {
    int w = GetScreenWidth();
    int h = GetScreenHeight();
    ClearBackground(RAYWHITE);

    DrawText("High-Scores", w / 2 - 180, 80, 42, DARKGRAY);

    float fontSize = 32;
    float spacing = 2;

    for (int i = 0; i < maxScores; i++) {
        int idx = i;
        Color c = (idx == 0) ? GOLD : BLUE;
        int x = w / 2 - 50;
        int y = 140 + i * 36;

        DrawText(TextFormat("%d. %d", i + 1, highScores[idx]), x, y, (int)fontSize, c);

        if (idx == 0) {
            // King of the Jungle yazısı
            char scoreText[32];
            snprintf(scoreText, sizeof(scoreText), "%d. %d", i + 1, highScores[idx]);
            Vector2 textSize = MeasureTextEx(resources.myFont, scoreText, fontSize, spacing);

            DrawTextEx(
                resources.myFont,
                " (King of The Jungle)",
                (Vector2) {
                x + textSize.x + 8, y
            },
                fontSize * 0.9f,
                spacing,
                GOLD
            );
        }
    }

    Rectangle backRect = { w / 2 - 60, h - 100, 120, 50 };
    DrawRectangleRounded(backRect, 0.3f, 10, LIGHTGRAY);
    DrawText("Back", w / 2 - 30, h - 90, 32, DARKGRAY);
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mouse = GetMousePosition();
        if (CheckCollisionPointRec(mouse, backRect)) {
            PlaySound(resources.clickButton);
            currentState = MENU;
        }
    }
}
