#include <stdio.h>
#include "raylib.h"
#include <time.h>


#define screenWidth 1366
#define screenHeight 768
#define gameWidth 900
#define gameHeight 1280
#define gridSize 8
#define tableSize 64


//Game states
typedef enum {
	MENU,
	LEVELS,
	SETTINGS,
	GAME,
}gameScreen;

typedef enum {
	candyRed,
	candyBlue,
	candyGreen,
	candyYellow,
	candyPurple,
	candyOrange,
	candyspecialFour,
	candyspecialFive,
	candyTypes

}candyType;

//Can switch between states owing to this function
typedef void (*screenFunction)(void);



typedef struct {

	Texture2D background, menu, levels;
	Music music;
	bool soundOn;
	float scale;
	int drawWidth, drawHeight, offsetX, offsetY;
	screenFunction currentScreen;
	gameScreen screen;
	bool isMoving;
	bool isMatched;
	
}gameRes;

gameRes resources;

//Initialize resources
void initRes(void) {

	resources.background = LoadTexture("C:/Images/background.png");
	resources.menu = LoadTexture("C:/Images/menu.jpg");
	resources.levels = LoadTexture("C:/Images/levels.png");
	resources.music = LoadMusicStream("C:/Images/thememusic.mp3");
	resources.soundOn = false;
	if (resources.soundOn) {
		PlayMusicStream(resources.music);
	}
}

//Unload resources
void unloadRes(void) {
	UnloadTexture(resources.background);
	UnloadTexture(resources.menu);
	UnloadTexture(resources.levels);
	UnloadMusicStream(resources.music);
	
}



void drawmenuScreen(void) {


	int currentScreenWidth = GetScreenWidth();
	int currentScreenHeight = GetScreenHeight();

	float scale = (float)currentScreenHeight / (float)gameHeight;
	int drawWidth = (int)(gameWidth * scale);
	int drawHeight = currentScreenHeight;
	int offsetX = (currentScreenWidth - drawWidth) / 2;
	int offsetY = 0;

	DrawTexture(resources.background, 0, 0, WHITE);
	
	DrawTexturePro(
		resources.menu,
		(Rectangle) {
		0, 0, (float)resources.menu.width, (float)resources.menu.height
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
	float posY = (screenHeight - buttonHeight) /2;

	
	Font myFont = LoadFont("C:/Images/font.ttf");
	float fontSize = 20;
	float spacing = 2;
	float centerX = (screenWidth - buttonWidth) / 2.0f;

    
    float playY = currentScreenHeight / 2.0f - 80;
    Rectangle playRec = { centerX, playY, buttonWidth, buttonHeight };
    Vector2 playTextSize = MeasureTextEx(myFont, "Play", fontSize, spacing);
    Vector2 playTextPos = {
        centerX + (buttonWidth - playTextSize.x) / 2.0f,
        playY + (buttonHeight - playTextSize.y) / 2.0f
    };

   
    float settingsY = playY + 80;
    Rectangle settingsRec = { centerX, settingsY, buttonWidth, buttonHeight };
    Vector2 settingsTextSize = MeasureTextEx(myFont, "Settings", fontSize, spacing);
    Vector2 settingsTextPos = {
        centerX + (buttonWidth - settingsTextSize.x) / 2.0f,
        settingsY + (buttonHeight - settingsTextSize.y) / 2.0f
    };

    DrawRectangleRounded(playRec, 0.3f, 10, ORANGE);
    DrawTextEx(myFont, "Play", playTextPos, fontSize, spacing, BLACK);

    DrawRectangleRounded(settingsRec, 0.3f, 10, RED);
    DrawTextEx(myFont, "Settings", settingsTextPos, fontSize, spacing, BLACK);

    

}

void drawlevelScreen(void) {

	


}

void settings(void) {





}

void drawgameScreen() {





}


int main() {

	//Resizable screen
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);

	//Initialize Window
	InitWindow(screenWidth, screenHeight, "\0");

	//Minimum screen size
	SetWindowMinSize(800, 600);

	//Set FPS
	SetTargetFPS(60);
	
	//Initialize sound
	InitAudioDevice();
	initRes();
	
	


	//Main loop 
	while (!WindowShouldClose()) {

		
		
		UpdateMusicStream(resources.music);

		if (!IsMusicStreamPlaying(resources.music) && resources.soundOn) {
			StopMusicStream(resources.music);
			PlayMusicStream(resources.music);
		}
        
		
		BeginDrawing();
		ClearBackground(RAYWHITE);

		drawmenuScreen();
		

		

		
		
		
		
		

        EndDrawing();

    }

	CloseWindow();
    return 0;

	}
