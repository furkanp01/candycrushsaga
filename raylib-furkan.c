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
#define boardoffsetX 100
#define boardoffsetY 150
#define candyTypes 5
#define framecount 8



//Game states
typedef enum {
	MENU,
	LEVELS,
	GAME,
	GAMEOVER,
	WIN,
	LEVELUP,
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
	int targetScore;
	int maxMoves;
	float timeLimit;
	int requiredspecialCandies;
}levelState;

//Candy structure
typedef struct {
	int baseType;
	Vector2 position;
	Vector2 targerPosition;
	Vector2 startPosition;
	float animTime;
	float scale;
	float alpha;
	animationState animState;
}candyState;


//Game board structure
typedef struct {

	candyState gameBoard[gridSize][gridSize];
	levelState levels[5];
	gameState state;

	int score;
	int moves;
	int currentLevel;
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

	float animationTimer;
	float gameTime;

	Texture2D candyTextures[candyTypes];
	Texture2D specialTextures[4];
	Texture2D backgroundWp, levelWp, backIcon, settingsIcon;
	Texture2D red;
	Texture2D menu[framecount];
	Sound swapSound;
	Sound matchSound;
	Sound specialSound;

	Music music;

	Font myFont;

}gameBoard;

gameBoard resources;

//Initialize resources
void initRes() {

	resources.candyTextures[0] = LoadTexture("resources/candy0.png");
	resources.candyTextures[1] = LoadTexture("resources/candy1.png");
	resources.candyTextures[2] = LoadTexture("resources/candy2.png");
	resources.candyTextures[3] = LoadTexture("resources/candy3.png");
	resources.candyTextures[4] = LoadTexture("resources/candy4.png");
	resources.candyTextures[5] = LoadTexture("resources/candy5.png");
	resources.backgroundWp = LoadTexture("resources/background.png");
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


	int buttonRadius = 40;
	int buttonSpacing = 30;
	int totalHeight = 5 * buttonRadius * 2 + 4 * buttonSpacing;
	int startY = (currentScreenHeight - totalHeight) / 2 + buttonRadius;
	int centerX = currentScreenWidth / 2;

	for (int i = 0; i < 5; i++) {
		int levelNum = 5 - i;
		int cy = startY + i * (buttonRadius * 2 + buttonSpacing);

		DrawCircle(centerX, cy, buttonRadius, LIGHTGRAY);
		DrawCircleLines(centerX, cy, buttonRadius, DARKGRAY);

		char label[2];
		snprintf(label, sizeof(label), "%d", levelNum);

		int fontSize = 32;
		int textWidth = MeasureText(label, fontSize);
		DrawText(label, centerX - textWidth / 2, cy - fontSize / 2, fontSize, BLACK);
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
		Rectangle quitButton = { panel.x + 50, panel.y + 130, 200, 40 };
		DrawRectangleRounded(quitButton, 0.3f, 10, RED);
		DrawTextEx(resources.myFont, "Quit Game", (Vector2) { quitButton.x + 10, quitButton.y + 8 }, fontSize, spacing, BLACK);

		Vector2 mouse = GetMousePosition();
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			if (CheckCollisionPointRec(mouse, soundButton)) {
				resources.soundOn = !resources.soundOn;
				SetMasterVolume(resources.soundOn ? 1.0f : 0.0f);
			}
			if (CheckCollisionPointRec(mouse, quitButton)) {
				CloseWindow();  // Oyunu kapat
			}
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

void settings(void) {






}



void drawgameScreen() {





}


//Unload resources
void unloadRes(void) {
	UnloadTexture(resources.backgroundWp);
	UnloadTexture(resources.menu[framecount]);
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
