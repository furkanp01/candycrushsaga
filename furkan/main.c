#include <stdio.h>
#include "raylib.h"
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "resources.h"

gameResources resources;
infoLevels levelsInfo;
gameState currentState;
int highScores[maxScores];
int scoreCount;
bool isUnlocked[maxLevels];
int currentScore;
int currentLevel;
infoLevels levels[maxLevels];
bool pendingSwapCheck;
bool revertSwap;


void initRes(void);
void loadlevelData(void);
void loadHighScores(void);
void initgameBoard(void);
void updateGridOffset(void);
void drawmenuScreen(void);
void drawlevelScreen(void);
void drawgameScreen(void);
void drawWin(void);
void drawgameOver(void);
void drawHighScoresScreen(void);
void selectCandy(void);
bool isValidSwap(int row1, int col1, int row2, int col2);
void swapCandies(int row1, int col1, int row2, int col2);
void updateSwapAnimation(float delta);
Vector2 vector2Lerp(Vector2 a, Vector2 b, float t);
void updateExplodeAnimation(float delta);
void removeMatches(void);
bool handleFiveMatch(void);
bool handleWrappedMatch(void);
bool handleFourMatch(void);
bool checkMatches(void);
void triggerSpecialCandy(int row, int col, int colorBombTargetColor);
bool dropOneCandy(void);
bool spawnCandies(void);
void updateCandyFallAnimation(float fallSpeed);
void drawCandies(void);
void drawgridLines(void);
Vector2 getCellFromMouse(Vector2 mousePos);
void addHighScore(int score);
void saveHighScores(void);
void completeLevel(int level);
void savelevelData(void);
void unloadRes(void);
bool checkFourMatchAndMakeStriped();
bool checkFiveMatchAndMakeColorBomb();
bool checkWrappedMatchAndMakePackage();
bool checkFourStripedCandyMatchAndTrigger();
void triggerFourStripedHorizontal(int row, int col);
void triggerFourStripedVertical(int row, int col);
void triggerFiveColorBomb(int row, int col, int targetBaseType);
void triggerWrappedPackage(int row, int col);
bool checkWrappedCandyAndTrigger();


int main() {

	//Resizable screen
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);

	//Initialize Window
	InitWindow(screenWidth, screenHeight, "\0");

	//Minimum window size
	SetWindowMinSize(800, 600);

	//Set FPS
	SetTargetFPS(60);

	//Initialize sound
	InitAudioDevice();

	//Initialize resources
	initRes();

	srand(time(NULL));
	initgameBoard();
	loadlevelData();
	loadHighScores();
	currentState = MENU;

	bool running = true;

	//Main loop 
	while (running && !WindowShouldClose()) {

		UpdateMusicStream(resources.music);

		if (!IsMusicStreamPlaying(resources.music) && resources.soundOn) {
			StopMusicStream(resources.music);
			PlayMusicStream(resources.music);
		}


		BeginDrawing();
		ClearBackground(RAYWHITE);

		switch (currentState) {

		case MENU:
			drawmenuScreen();
			break;
		case LEVELS:
			drawlevelScreen();
			break;
		case GAME:
			drawgameScreen();
			break;
		case WIN:
			drawWin();
			break;
		case GAMEOVER:
			drawgameOver();
			break;
		case HIGHSCORES:
			drawHighScoresScreen();
			break;
		case QUIT:
			saveHighScores();
			running = false;
			break;
		}


		EndDrawing();
	}

	CloseWindow();
	return 0;
}
