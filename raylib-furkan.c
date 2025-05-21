#include <stdio.h>
#include "raylib.h"
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


#define screenWidth 1366
#define screenHeight 768
#define gameWidth 900
#define gameHeight 1280
#define gridSize 8
#define cellSize 100
#define candyTypes 6
#define framecount 8
#define maxLevels 5




//Game states
typedef enum {
	MENU,
	LEVELS,
	GAME,
	GAMEOVER,
	WIN,
	LEVELUP,
	QUIT
}gameState;

//Candy types
typedef enum {
	candyRed,
	candyBlue,
	candyGreen,
	candyYellow,
	candyPurple,
	candyOrange,
	candystripedH,
	candystripedV,
	candyWrapped,
	candycolorBomb
}candyType;

//Animation types
typedef enum {

	animNone,
	animSwap,
	animSwapBack,
	animFall,
	animRemove,
	animAppear

}animationState;

//Level structure
typedef struct {
	int levelId;
	int targetScore;
	int maxMoves;
	bool isUnlocked;
}levelState;

//Candy structure
typedef struct {
	int baseType;
	Vector2 position;
	Vector2 targetPosition;
	Vector2 startPosition;
	float animTime;
	float scale;
	float alpha;
	animationState animState;
	bool hasCandy;
}candyState;


//Game board structure
typedef struct {

	candyState gameBoard[gridSize][gridSize];
	gameState state;

	int score;
	int moves;
	int specialcandiesCreated;
	int selectedRow;
	int selectedColumn;

	bool isMoving;
	bool isAnimating;
	bool isSwapping;
	bool isgameOver;
	bool soundOn;
	bool showmenuSettings;
	bool showlevelSettings;
	bool showgameSettings;
	bool confirmQuit;
	bool boardInitialized;
	bool checkMatches;

	float animationTimer;
	float gameTime;
	float cellWidth;
	float cellHeight;
	float boardoffsetX;
	float boardoffsetY;

	Texture2D candyTextures[candyTypes];
	Texture2D specialTextures[4];
	Texture2D backgroundWp, levelWp, backIcon, settingsIcon, gameboardWp;
	Texture2D menu[framecount];
	Sound swapSound;
	Sound matchSound;
	Sound specialSound;

	Music music;

	Font myFont;

}gameResources;

gameResources resources;

void initgameBoard();
void drawmenuScreen();
void loadlevelData();
void savelevelData();
void completeLevel(int level);
void drawlevelScreen();
void drawgameScreen();
void ToggleSound();
void unloadRes();

//Initialize resources
void initRes() {

	resources.candyTextures[0] = LoadTexture("resources/candy0.png");
	resources.candyTextures[1] = LoadTexture("resources/candy1.png");
	resources.candyTextures[2] = LoadTexture("resources/candy2.png");
	resources.candyTextures[3] = LoadTexture("resources/candy3.png");
	resources.candyTextures[4] = LoadTexture("resources/candy4.png");
	resources.candyTextures[5] = LoadTexture("resources/candy5.png");
	resources.backgroundWp = LoadTexture("resources/background.png");
	resources.gameboardWp = LoadTexture("resources/gameBoardWp.png");
	resources.menu[0] = LoadTexture("resources/menu.png");
	resources.menu[1] = LoadTexture("resources/menu1.png");
	resources.menu[2] = LoadTexture("resources/menu2.png");
	resources.menu[3] = LoadTexture("resources/menu3.png");
	resources.menu[4] = LoadTexture("resources/menu4.png");
	resources.menu[5] = LoadTexture("resources/menu5.png");
	resources.menu[6] = LoadTexture("resources/menu6.png");
	resources.menu[7] = LoadTexture("resources/menu7.png");
	resources.levelWp = LoadTexture("resources/levels.png");
	resources.backIcon = LoadTexture("resources/backIcon.png");
	resources.settingsIcon = LoadTexture("resources/settingsIcon.png");
	resources.music = LoadMusicStream("resources/thememusic.mp3");
	resources.myFont = LoadFont("resources/font.ttf");
	resources.swapSound = LoadSound("resources/swapSound.mp3");
	resources.matchSound = LoadSound("resources/matchSound.mp3");
	resources.specialSound = LoadSound("resources/specialSound.mp3");
	resources.soundOn = true;




	PlayMusicStream(resources.music);
	SetMasterVolume(resources.soundOn ? 1.0f : 0.0f);

}

gameState currentState = MENU;

//Menu 
void drawmenuScreen(void) {

	static int currentFrame = 0;
	static float frameTimer = 0.0f;
	float frameDelay = 0.085f;

	int currentScreenWidth = GetScreenWidth();
	int currentScreenHeight = GetScreenHeight();

	float scale = (float)currentScreenHeight / (float)gameHeight;
	int drawWidth = (int)(gameWidth * scale);
	int drawHeight = currentScreenHeight;
	int offsetX = (currentScreenWidth - drawWidth) / 2;
	int offsetY = 0;

	frameTimer += GetFrameTime();
	if (frameTimer >= frameDelay) {
		frameTimer = 0.0f;
		currentFrame = (currentFrame + 1) % framecount;
	}


	DrawTexture(resources.backgroundWp, 0, 0, WHITE);

	if (!resources.showmenuSettings) {
		DrawTexture(resources.backgroundWp, 0, 0, WHITE);
	}

	DrawTexturePro(
		resources.menu[currentFrame],
		(Rectangle) {
		0, 0, (float)resources.menu[currentFrame].width, (float)resources.menu[currentFrame].height
	},
		(Rectangle) {
		offsetX, offsetY, drawWidth, drawHeight
	},
		(Vector2) {
		0, 0
	},
		0.0f,
		WHITE
	);


	
	float buttonWidth = 200;
	float buttonHeight = 60;
	float posX = (screenWidth - buttonWidth) / 2;
	float posY = (screenHeight - buttonHeight) / 2;

	float fontSize = 25;
	float spacing = 2;
	float centerX = (currentScreenWidth - buttonWidth) / 2.0f;


	float playY = currentScreenHeight / 2.0f;
	Rectangle playRect = { centerX, playY, buttonWidth, buttonHeight };
	Vector2 playTextSize = MeasureTextEx(resources.myFont, "Play", fontSize, spacing);
	Vector2 playTextPos = {
		centerX + (buttonWidth - playTextSize.x) / 2.0f,
		playY + (buttonHeight - playTextSize.y) / 2.0f
	};


	float settingsY = playY + 100;
	Rectangle settingsRec = { centerX, settingsY, buttonWidth, buttonHeight };
	Vector2 settingsTextSize = MeasureTextEx(resources.myFont, "Settings", fontSize, spacing);
	Vector2 settingsTextPos = {
		centerX + (buttonWidth - settingsTextSize.x) / 2.0f,
		settingsY + (buttonHeight - settingsTextSize.y) / 2.0f
	};


	DrawRectangleRounded(playRect, 0.3f, 10, ORANGE);

	DrawTextEx(resources.myFont, "Play", playTextPos, fontSize, spacing, BLACK);

	DrawRectangleRounded(settingsRec, 0.3f, 10, PINK);

	DrawTextEx(resources.myFont, "Settings", settingsTextPos, fontSize, spacing, BLACK);

	if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
		Vector2 mouseposS = GetMousePosition();
		if (CheckCollisionPointRec(mouseposS, playRect) && !resources.showmenuSettings) {
			currentState = LEVELS;
		}
		if (CheckCollisionPointRec(mouseposS, settingsRec)) {
			resources.showmenuSettings = true;
		}
	}


	if (resources.showmenuSettings) {

		DrawRectangle(0, 0, currentScreenWidth, currentScreenHeight, Fade(BLACK, 0.8f));


		Rectangle panel = { centerX - 50, settingsY - 150, buttonWidth + 100, 200 };
		DrawRectangleRounded(panel, 10, 10, ORANGE);


		const char* soundToggleText = resources.soundOn ? "Sound: ON" : "Sound: OFF";
		Vector2 soundSize = MeasureTextEx(resources.myFont, soundToggleText, fontSize, spacing);
		Vector2 soundPos = {
			panel.x + (panel.width - soundSize.x) / 2,
			panel.y + 30
		};
		DrawTextEx(resources.myFont, soundToggleText, soundPos, fontSize, spacing, BLACK);

		Rectangle soundToggle = { panel.x + 60, panel.y + 80, panel.width - 120, 50 };
		DrawRectangleRounded(soundToggle, 0.3f, 10, PINK);
		DrawTextEx(resources.myFont, "Toggle Sound", (Vector2) { soundToggle.x + 10, soundToggle.y + 10 },
			fontSize, spacing, BLACK);


		Vector2 closeButtonCenter = { panel.x + panel.width - 30, panel.y - 30 };
		float closeButtonRadius = 25;
		DrawCircleV(closeButtonCenter, closeButtonRadius, DARKGRAY);
		DrawTextEx(resources.myFont, "X", (Vector2) { closeButtonCenter.x - 8, closeButtonCenter.y - 12 }, 30, spacing, WHITE);


		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			Vector2 mouse = GetMousePosition();
			if (CheckCollisionPointCircle(mouse, closeButtonCenter, closeButtonRadius)) {
				resources.showmenuSettings = false;
			}
		}


		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			Vector2 mouse = GetMousePosition();
			if (CheckCollisionPointRec(mouse, soundToggle)) {
				resources.soundOn = !resources.soundOn;
				SetMasterVolume(resources.soundOn ? 1.0f : 0.0f);
			}
		}
	}
}

bool isUnlocked[maxLevels] = { true,false,false,false,false };

void loadlevelData() {
	FILE* xPtr = fopen("levels.txt", "r");
	if (xPtr == NULL) {
		printf("File couldn't be opened.");
		currentState = QUIT;
	}
	else
	{
		for (int i = 0;i < maxLevels;i++) {
			int val;
			if (fscanf_s(xPtr, "%d", &val) == 1) {
				isUnlocked[i] = val;
			}
		}
		fclose(xPtr);
	}

}

void savelevelData() {
	FILE* xPtr = fopen("levels.txt", "w");
	if (xPtr == NULL) {
		printf("The file couldn't be opened.");
		currentState = QUIT;
	}
	else {
		for (int i = 0;i < maxLevels;i++) {
			fprintf(xPtr, "%d\n", isUnlocked[i]);
		}
		fclose(xPtr);
	}
}

void completeLevel(int level) {
	if (level + 1 < maxLevels) {
		isUnlocked[level + 1] = true;
	}
	savelevelData();
}

int currentLevel = 0;



void drawlevelScreen(void) {




	int currentScreenWidth = GetScreenWidth();
	int currentScreenHeight = GetScreenHeight();
	float scale = (float)currentScreenHeight / (float)gameHeight;
	int drawWidth = (int)(gameWidth * scale);
	int drawHeight = currentScreenHeight;
	int offsetX = (currentScreenWidth - drawWidth) / 2;
	int offsetY = 0;


	DrawTexture(resources.backgroundWp, 0, 0, WHITE);


	DrawTexturePro(
		resources.levelWp,
		(Rectangle) {
		0, 0, (float)resources.levelWp.width, (float)resources.levelWp.height
	},
		(Rectangle) {
		offsetX, offsetY, drawWidth, drawHeight
	},
		(Vector2) {
		0, 0
	},
		0.0f,
		WHITE
	);


	int buttonRadius = 25;
	int buttonSpacing = 20;
	int totalHeight = 5 * buttonRadius * 2 + 4 * buttonSpacing;
	int startY = (currentScreenHeight - totalHeight) / 2 + buttonRadius;
	int centerX = currentScreenWidth / 2;

	for (int i = 0; i < maxLevels; i++) {
		int cy = startY + i * (buttonRadius * 2 + buttonSpacing);

		Color fillColor = isUnlocked[i] ? (Color) { 173, 216, 230, 255 } : GRAY;
		DrawCircle(centerX, cy, buttonRadius, fillColor);
		DrawCircleLines(centerX, cy, buttonRadius, BLACK);

		char label[2];
		snprintf(label, sizeof(label), "%d", i + 1);

		int fontSize = 30;
		int textWidth = MeasureText(label, fontSize);
		DrawText(label, centerX - textWidth / 2, cy - fontSize / 2, fontSize, BLACK);


		if (isUnlocked[i]) {
			if (CheckCollisionPointCircle(GetMousePosition(), (Vector2) { centerX, cy }, buttonRadius)) {
				if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
					currentLevel = i;
					currentState = GAME;
					initgameBoard();
				}
			}
		}
	}


	Vector2 backCenter = { 60, 60 };
	Rectangle backRect = { backCenter.x - 30, backCenter.y - 30, 60, 60 };

	DrawTexturePro(
		resources.backIcon,
		(Rectangle) {
		0, 0, (float)resources.backIcon.width, (float)resources.backIcon.height
	},
		(Rectangle) {
		backRect.x, backRect.y, backRect.width, backRect.height
	},
		(Vector2) {
		0, 0
	},
		0.0f,
		WHITE
	);


	Vector2 settingsCenter = { currentScreenWidth - 60, 60 };
	Rectangle settingsRect = { settingsCenter.x - 30, settingsCenter.y - 30, 60, 60 };

	DrawTexturePro(
		resources.settingsIcon,
		(Rectangle) {
		0, 0, (float)resources.settingsIcon.width, (float)resources.settingsIcon.height
	},
		(Rectangle) {
		settingsRect.x, settingsRect.y, settingsRect.width, settingsRect.height
	},
		(Vector2) {
		0, 0
	},
		0.0f,
		WHITE
	);


	Vector2 mouse = GetMousePosition();
	if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
		if (CheckCollisionPointRec(mouse, backRect)) {
			currentState = MENU;
		}
		if (CheckCollisionPointRec(mouse, settingsRect)) {
			resources.showlevelSettings = true;
		}
	}
	if (resources.showlevelSettings) {
		// Ayar paneli arka planı
		DrawRectangle(0, 0, currentScreenWidth, currentScreenHeight, Fade(BLACK, 0.8f));

		Rectangle panel = { currentScreenWidth / 2 - 150, currentScreenHeight / 2 - 100, 300, 200 };
		DrawRectangleRounded(panel, 0.3f, 10, ORANGE);

		Font myFont = LoadFont("resources/font.ttf");
		float fontSize = 25;
		float spacing = 2;

		// Ses yazısı
		const char* soundText = resources.soundOn ? "Sound: ON" : "Sound: OFF";
		Vector2 soundSize = MeasureTextEx(myFont, soundText, fontSize, spacing);
		Vector2 soundPos = { panel.x + (panel.width - soundSize.x) / 2, panel.y + 30 };
		DrawTextEx(myFont, soundText, soundPos, fontSize, spacing, BLACK);

		// Ses butonu
		Rectangle soundButton = { panel.x + 50, panel.y + 70, 200, 40 };
		DrawRectangleRounded(soundButton, 0.3f, 10, PINK);
		DrawTextEx(myFont, "Sound On/Off", (Vector2) { soundButton.x + 10, soundButton.y + 8 }, fontSize, spacing, BLACK);

		// Çıkış butonu
		Rectangle backBtn = { panel.x + panel.width - 110, panel.y + panel.height - 60, 100, 40 };
		DrawRectangleRec(backBtn, LIGHTGRAY);
		DrawRectangleLines(backBtn.x, backBtn.y, backBtn.width, backBtn.height, BLACK);
		DrawText("Back", backBtn.x + 30, backBtn.y + 10, 20, BLACK);

		Vector2 mouse = GetMousePosition();
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			if (CheckCollisionPointRec(mouse, soundButton)) {
				resources.soundOn = !resources.soundOn;
				SetMasterVolume(resources.soundOn ? 1.0f : 0.0f);
			}
			if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
				Vector2 mouse = GetMousePosition();
				if (CheckCollisionPointRec(mouse, backBtn)) {
					resources.showlevelSettings = false;
				}
			}
		}
	}
}

int gameBoard[gridSize][gridSize];

Vector2 gridOffset;
bool hasSelected = false;
Vector2 selectedCell = { -1,-1 };

void updateGridOffset() {
	int currentscreenWidth = GetScreenWidth();
	int currentscreenHeight = GetScreenHeight();
	gridOffset.x = (currentscreenWidth - (gridSize * cellSize)) / 2;
	gridOffset.y = (currentscreenHeight - (gridSize * cellSize)) / 2;
}

void initgameBoard() {
	for (int i = 0; i < gridSize; i++) {
		for (int j = 0; j < gridSize; j++) {
			int validTypes[candyTypes];
			int validCount = 0;

			for (int type = 0; type < candyTypes; type++) {
				bool valid = true;

				if (j >= 2 &&
					resources.gameBoard[i][j - 1].baseType == type &&
					resources.gameBoard[i][j - 2].baseType == type) {
					valid = false;
				}

				if (i >= 2 &&
					resources.gameBoard[i - 1][j].baseType == type &&
					resources.gameBoard[i - 2][j].baseType == type) {
					valid = false;
				}

				if (valid) {
					validTypes[validCount++] = type;
				}
			}

			int selected = validTypes[rand() % validCount];
			resources.gameBoard[i][j].baseType = selected;
			resources.gameBoard[i][j].position.x = j * cellSize;
			resources.gameBoard[i][j].position.y = i * cellSize;
		}
	}
	updateGridOffset();
}

void drawCandies() {
	for (int i = 0; i < gridSize; i++) {
		for (int j = 0; j < gridSize; j++) {
			int type = resources.gameBoard[i][j].baseType;
			if (type >= 0 && type < candyTypes) {
				Texture2D tex = resources.candyTextures[type];
				float offsetX = (cellSize - tex.width) / 2.0f;
				float offsetY = (cellSize - tex.height) / 2.0f;

				DrawTexture(
					tex,
					(int)(resources.gameBoard[i][j].position.x + gridOffset.x + offsetX),
					(int)(resources.gameBoard[i][j].position.y + gridOffset.y + offsetY),
					WHITE
				);
			}
		}
	}
}

void drawgridLines() {
	Color boxColor = Fade(GRAY, 0.3f);
	for (int i = 0; i < gridSize; i++) {
		for (int j = 0; j < gridSize; j++) {
			Rectangle cellRect = {
				gridOffset.x + j * cellSize,
				gridOffset.y + i * cellSize,
				cellSize,
				cellSize
			};
			DrawRectangleRoundedLines(cellRect, 0.2f, 6, boxColor);
		}
	}
	updateGridOffset();
}

Vector2 getCellFromMouse(Vector2 mousePos) {
	return (Vector2) {
		(int)((mousePos.x - gridOffset.x) / cellSize),
			(int)((mousePos.y - gridOffset.y) / cellSize)
	};
}

void swapCandies(int row1, int col1, int row2, int col2) {
	candyState temp = resources.gameBoard[row1][col1];
	resources.gameBoard[row1][col1] = resources.gameBoard[row2][col2];
	resources.gameBoard[row2][col2] = temp;

	resources.gameBoard[row1][col1].position = (Vector2){ col1 * cellSize, row1 * cellSize };
	resources.gameBoard[row2][col2].position = (Vector2){ col2 * cellSize, row2 * cellSize };
}


void selectCandy() {
	if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
		Vector2 mousePos = GetMousePosition();
		Vector2 clickedCell = getCellFromMouse(mousePos);

		if (clickedCell.x >= 0 && clickedCell.x < gridSize &&
			clickedCell.y >= 0 && clickedCell.y < gridSize) {

			if (!hasSelected) {
				selectedCell = clickedCell;
				hasSelected = true;
			}
			else {
				int dx = (int)(clickedCell.x - selectedCell.x);
				int dy = (int)(clickedCell.y - selectedCell.y);

				if ((abs(dx) == 1 && dy == 0) || (abs(dy) == 1 && dx == 0)) {
					swapCandies((int)selectedCell.y, (int)selectedCell.x, (int)clickedCell.y, (int)clickedCell.x);
				}

				hasSelected = false;
			}
		}
	}
}

bool checkMatches() {
	bool matchFound = false;

	for (int i = 0; i < gridSize; i++) {
		for (int j = 0; j < gridSize - 2; j++) {
			int type = resources.gameBoard[i][j].baseType;
			if (type >= 0 &&
				type == resources.gameBoard[i][j + 1].baseType &&
				type == resources.gameBoard[i][j + 2].baseType) {

				resources.gameBoard[i][j].baseType = -1;
				resources.gameBoard[i][j + 1].baseType = -1;
				resources.gameBoard[i][j + 2].baseType = -1;
				matchFound = true;
			}
		}
	}

	for (int j = 0; j < gridSize; j++) {
		for (int i = 0; i < gridSize - 2; i++) {
			int type = resources.gameBoard[i][j].baseType;
			if (type >= 0 &&
				type == resources.gameBoard[i + 1][j].baseType &&
				type == resources.gameBoard[i + 2][j].baseType) {

				resources.gameBoard[i][j].baseType = -1;
				resources.gameBoard[i + 1][j].baseType = -1;
				resources.gameBoard[i + 2][j].baseType = -1;
				matchFound = true;
			}
		}
	}

	return matchFound;
}

void dropCandies() {
	for (int j = 0; j < gridSize; j++) {
		for (int i = gridSize - 1; i >= 0; i--) {
			if (resources.gameBoard[i][j].baseType == -1) {
				for (int k = i - 1; k >= 0; k--) {
					if (resources.gameBoard[k][j].baseType != -1) {
						resources.gameBoard[i][j].baseType = resources.gameBoard[k][j].baseType;
						resources.gameBoard[k][j].baseType = -1;
						break;
					}
				}
			}
		}
	}
}

void refillBoard() {
	for (int i = 0; i < gridSize; i++) {
		for (int j = 0; j < gridSize; j++) {
			if (resources.gameBoard[i][j].baseType == -1) {
				resources.gameBoard[i][j].baseType = rand() % candyTypes;
			}
		}
	}
}

void removeMatches() {
	bool chainReaction = true;
	while (chainReaction) {
		chainReaction = false;
		if (checkMatches()) {
			dropCandies();
			refillBoard();
			chainReaction = true;
			resources.score += 100;
		}
	}
}

void drawgameScreen() {
	DrawTexture(resources.backgroundWp, 0, 0, WHITE);

	int currentScreenWidth = GetScreenWidth();
	int currentScreenHeight = GetScreenHeight();
	float scale = (float)currentScreenHeight / (float)gameHeight;
	int drawWidth = (int)(gameWidth * scale);
	int drawHeight = currentScreenHeight;
	int offsetX = (currentScreenWidth - drawWidth) / 2;

	DrawTexturePro(
		resources.gameboardWp,
		(Rectangle) {
		0, 0, (float)resources.gameboardWp.width, (float)resources.gameboardWp.height
	},
		(Rectangle) {
		offsetX, 0, drawWidth, drawHeight
	},
		(Vector2) {
		0, 0
	}, 0.0f, WHITE
	);

	drawgridLines();
	drawCandies();
	selectCandy();

	if (hasSelected) {
		Rectangle selectedRect = {
			gridOffset.x + selectedCell.x * cellSize,
			gridOffset.y + selectedCell.y * cellSize,
			cellSize,
			cellSize
		};
		DrawRectangleLinesEx(selectedRect, 3, RED);
	}

	static bool shouldCheckMatches = false;
	if (!shouldCheckMatches && !hasSelected && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
		shouldCheckMatches = true;
	}

	if (shouldCheckMatches) {
		removeMatches();
		resources.moves--;
		shouldCheckMatches = false;
	}
}




void ToggleSound() {
	resources.soundOn = !resources.soundOn;

	if (resources.soundOn) {
		SetMasterVolume(1.0f);
	}
	else {
		SetMasterVolume(0.0f);
	}
}




//Unload resources
void unloadRes(void) {
	UnloadTexture(resources.backgroundWp);
	for (int i = 0; i < framecount; i++) {
		UnloadTexture(resources.menu[i]);
	}
	UnloadTexture(resources.levelWp);
	UnloadMusicStream(resources.music);
	UnloadFont(resources.myFont);
}






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
	initRes();

	currentState = MENU;

	//Main loop 
	while (!WindowShouldClose()) {

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
		}

		EndDrawing();

	}

	CloseWindow();
	return 0;

}
