#include <stdio.h>
#include "raylib.h"
#include <time.h>
#include <stdlib.h>
#include <string.h>



#define screenWidth 1366
#define screenHeight 768
#define gameWidth 900
#define gameHeight 1280
#define gridSize 8
#define candyTypes 6
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
	bool hasSelected;
	bool soundOn;
	bool showmenuSettings;
	bool showlevelSettings;
	bool showgameSettings;
	bool confirmQuit;
	bool boardInitialized;

	float animationTimer;
	float gameTime;
	float cellWidth;
	float cellHeight;
	float cellSize;
	float boardoffsetX;
	float boardoffsetY;

	Texture2D candyTextures[candyTypes];
	Texture2D specialTextures[4];
	Texture2D backgroundWp, menuWp, levelWp,backIcon,settingsIcon,gameboardWp;
	Sound swapSound;
	Sound matchSound;
	Sound specialSound;

	Music music;

	Font myFont;

}gameResources;

gameResources resources;

void initRes();
void drawmenuScreen();
void loadlevelData();
void savelevelData();
void completeLevel(int level);
void drawlevelScreen();
void initgameBoard();
void drawgameScreen();
void ToggleSound();
void unloadRes();

void initRes() {

	resources.candyTextures[0] = LoadTexture("resources/candy0.png");
	resources.candyTextures[1] = LoadTexture("resources/candy1.png");
	resources.candyTextures[2] = LoadTexture("resources/candy2.png");
	resources.candyTextures[3] = LoadTexture("resources/candy3.png");
	resources.candyTextures[4] = LoadTexture("resources/candy4.png");
	resources.candyTextures[5] = LoadTexture("resources/candy5.png");
	resources.backgroundWp = LoadTexture("resources/background.png");
	resources.menuWp = LoadTexture("resources/menu.jpg");
	resources.levelWp = LoadTexture("resources/levels.png");
	resources.gameboardWp = LoadTexture("resources/gameBoard.png");
	resources.backIcon = LoadTexture("resources/backIcon.png");
	resources.settingsIcon = LoadTexture("resources/settingsIcon.png");
	resources.music = LoadMusicStream("resources/thememusic.mp3");
	resources.myFont = LoadFont("resources/font.ttf");
	resources.swapSound = LoadSound("resources/swapSound.mp3");
	resources.matchSound = LoadSound("resources/matchSound.mp3");
	resources.specialSound = LoadSound("resources/specialSound.mp3");
	resources.soundOn = false;
	resources.cellWidth = (float)gameWidth / gridSize;
	resources.cellHeight = (float)gameHeight / gridSize;
	resources.cellSize = fmin(resources.cellWidth, resources.cellHeight);

	int currentScreenWidth = GetScreenWidth();
	int currentScreenHeight = GetScreenHeight();
	resources.boardoffsetX = (currentScreenWidth - gridSize * resources.cellSize) / 2;
	resources.boardoffsetY = (currentScreenHeight - gridSize * resources.cellSize) / 2;
	PlayMusicStream(resources.music);
	SetMasterVolume(resources.soundOn ? 1.0f : 0.0f);

}

gameState currentState = MENU;


void drawmenuScreen() {

	int currentScreenWidth = GetScreenWidth();
	int currentScreenHeight = GetScreenHeight();

	float scale = (float)currentScreenHeight / (float)gameHeight;
	int drawWidth = (int)(gameWidth * scale);
	int drawHeight = currentScreenHeight;
	int offsetX = (currentScreenWidth - drawWidth) / 2;
	int offsetY = 0;


	DrawTexture(resources.backgroundWp, 0, 0, WHITE);

	if (!resources.showmenuSettings) {
		DrawTexture(resources.backgroundWp, 0, 0, WHITE);
	}

	DrawTexturePro(
		resources.menuWp,
		(Rectangle) {
		0, 0, (float)resources.menuWp.width, (float)resources.menuWp.height
	},
		(Rectangle) {
		offsetX, offsetY, drawWidth, drawHeight
	},
		(Vector2) {
		0, 0
	}, 0.0f, WHITE);


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


		const char* soundToggleText = resources.soundOn ? "Sound: ON" : "Sound: Off";
		Vector2 soundSize = MeasureTextEx(resources.myFont, soundToggleText, fontSize, spacing);
		Vector2 soundPos = {
			panel.x + (panel.width - soundSize.x) / 2,
			panel.y + 30
		};
		DrawTextEx(resources.myFont, soundToggleText, soundPos, fontSize, spacing, BLACK);

		Rectangle soundToggle = { panel.x + 60, panel.y + 80, panel.width - 120, 50 };
		DrawRectangleRounded(soundToggle, 0.3f, 10, PINK);
		DrawTextEx(resources.myFont, "Sound On/Off", (Vector2) { soundToggle.x + 10, soundToggle.y + 10 },
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
			     if (fscanf(xPtr, "%d", &val) == 1) {
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
	else{
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
	}, 0.0f, WHITE
	);


	int buttonRadius = 25;
	int buttonSpacing = 20;
	int totalHeight = 5 * buttonRadius * 2 + 4 * buttonSpacing;
	int startY = (currentScreenHeight - totalHeight) / 2 + buttonRadius;
	int centerX = (currentScreenWidth / 2) + 5;

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
	}, 0.0f, WHITE
	);

	Vector2 mouse = GetMousePosition();
	
	int settingsSize = 50;
	int settingsX = currentScreenWidth - settingsSize - 20; 
	int settingsY = 20;
	Rectangle settingsRect = { settingsX, settingsY, settingsSize, settingsSize };

	DrawTextureEx(resources.settingsIcon, (Vector2) { settingsX, settingsY }, 0.0f,
		(float)settingsSize / resources.settingsIcon.width, WHITE);



	if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
		if (CheckCollisionPointRec(mouse, backRect)) {
			currentState = MENU;
		}
		else if (CheckCollisionPointRec(mouse, settingsRect)) {
			resources.showlevelSettings = !resources.showlevelSettings;  
		}
	}

	if (resources.showlevelSettings) {
		
		int panelW = 400, panelH = 300;
		int panelX = (currentScreenWidth - panelW) / 2;
		int panelY = (GetScreenHeight() - panelH) / 2;
		Rectangle panelRect = { panelX, panelY, panelW, panelH };

		DrawRectangleRounded(panelRect,0.1,1,Fade(ORANGE,2.0f));
		

		Rectangle soundBtn = { panelX + 125, panelY + 100, 150, 50 };
		DrawRectangleRec(soundBtn, resources.soundOn ? GREEN : RED);
		DrawText(resources.soundOn ? "Sound:On" : "Sound:Off", soundBtn.x + 25, soundBtn.y + 15, 20, BLACK);

	
		Rectangle backBtn = { panelX + panelW - 110, panelY + panelH - 60, 100, 40 };
		DrawRectangleRec(backBtn, LIGHTGRAY);
		DrawRectangleLines(backBtn.x, backBtn.y, backBtn.width, backBtn.height, BLACK);
		DrawText("Back", backBtn.x + 30, backBtn.y + 10, 20, BLACK);

		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			Vector2 mouse = GetMousePosition();
			if (CheckCollisionPointRec(mouse, soundBtn)) {
				resources.soundOn = !resources.soundOn;
				SetMasterVolume(resources.soundOn ? 1.0f : 0.0f);
			}
		}
		
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			Vector2 mouse = GetMousePosition();
			if (CheckCollisionPointRec(mouse, backBtn)) {
				resources.showlevelSettings = false;
			 }
		  }
	   }
	}

	candyState gameBoard[gridSize][gridSize];

	
	bool boardInitialized = false;
	
	void initgameBoard(void) {
	
		srand(time(NULL));

		for (int i = 0;i < gridSize;i++) {
			for (int j = 0;j < gridSize;j++) {

				resources.gameBoard[i][j].position=(Vector2){
			    resources.boardoffsetX + i * resources.cellSize,
				resources.boardoffsetY + j * resources.cellSize
				};

				resources.gameBoard[i][j].targetPosition = resources.gameBoard[i][j].position;
				resources.gameBoard[i][j].baseType = rand() % candyTypes;
				resources.gameBoard[i][j].animState = animNone;
				resources.gameBoard[i][j].animTime = 2.0f;
				resources.gameBoard[i][j].scale = 3.0f;
				resources.gameBoard[i][j].alpha = 3.0f;
			}

		}
		resources.score = 0;
		resources.moves = 0;
		resources.specialcandiesCreated = 0;
		resources.selectedRow = -1;
		resources.selectedColumn = -1;
		resources.isMoving = false;
		resources.isAnimating = false;
		resources.isSwapping = false;
		resources.isgameOver = false;
		resources.hasSelected = false;
		resources.boardInitialized = true;
	}

	void drawgameScreen() {
		
		int currentScreenWidth = GetScreenWidth();
		int currentScreenHeight = GetScreenHeight();

		DrawTexture(resources.backgroundWp, 0, 0, WHITE);
		if (!boardInitialized) {
			initgameBoard();
			resources.boardInitialized = true;
		}

		float scale = (float)currentScreenHeight / (float)gameHeight;
		int drawWidth = (int)(gameWidth * scale);
		int drawHeight = currentScreenHeight;
		int offsetX = (currentScreenWidth - drawWidth) / 2;
		int offsetY = 0;

		DrawTexturePro(
			resources.gameboardWp,
			(Rectangle) {
			0, 0, (float)resources.gameboardWp.width, (float)resources.gameboardWp.height
		},
			(Rectangle) {
			offsetX, offsetY, drawWidth, drawHeight
		},
			(Vector2) {
			0, 0
		}, 0.0f, WHITE
		);

		for (int i = 0; i <= gridSize; i++) {
			DrawLine(
				resources.boardoffsetX + i * resources.cellSize,
				resources.boardoffsetY,
				resources.boardoffsetX + i * resources.cellSize,
				resources.boardoffsetY + gridSize * resources.cellSize,
				Fade(DARKGRAY, 0.3f)
			);

			DrawLine(
				resources.boardoffsetX,
				resources.boardoffsetY + i * resources.cellSize,
				resources.boardoffsetX + gridSize * resources.cellSize,
				resources.boardoffsetY + i * resources.cellSize,
				Fade(DARKGRAY, 0.3f)
			);
		}

		// Draw candies
		for (int i = 0; i < gridSize; i++) {
			for (int j = 0; j < gridSize; j++) {
				candyState* candy = &resources.gameBoard[i][j];

				// Draw candy only if it has a valid base type
				if (candy->baseType >= 0 && candy->baseType < candyTypes) {
					float drawSize = resources.cellSize * 0.9f * candy->scale; // Slightly smaller than cell size

					DrawTexturePro(
						resources.candyTextures[candy->baseType],
						(Rectangle) {
						0, 0,
							(float)resources.candyTextures[candy->baseType].width,
							(float)resources.candyTextures[candy->baseType].height
					},
						(Rectangle) {
						candy->position.x + resources.cellSize / 2 - drawSize / 2,
							candy->position.y + resources.cellSize / 2 - drawSize / 2,
							drawSize, drawSize
					},
						(Vector2) {
						0, 0
					},
						0.0f,
						Fade(WHITE, candy->alpha)
					);
				}
			}
		}

		// Draw score, moves, and level information
		char scoreText[50];
		sprintf(scoreText, "Score: %d", resources.score);
		DrawText(scoreText, 20, 20, 30, BLACK);

		char movesText[50];
		sprintf(movesText, "Moves: %d", resources.moves);
		DrawText(movesText, 20, 60, 30, BLACK);

		char levelText[50];
		sprintf(levelText, "Level: %d", currentLevel + 1);
		DrawText(levelText, 20, 100, 30, BLACK);

		// Draw back button
		Vector2 backCenter = { 60, GetScreenHeight() - 60 };
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

		
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			Vector2 mousePos = GetMousePosition();
			if (CheckCollisionPointRec(mousePos, backRect)) {
				currentState = LEVELS;
				resources.boardInitialized = false;
			}
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
	UnloadTexture(resources.menuWp);
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
		
		default:
			puts("Error,please do a proper transaction.");
			break;
		}
		

		EndDrawing();
	}

	CloseWindow();
	return 0;
}
