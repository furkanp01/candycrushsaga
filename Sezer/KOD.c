#include <stdio.h>
#include "raylib.h"
#include <time.h>

#define screenWidth 1366
#define screenHeight 768
#define gameWidth 900
#define gameHeight 1280


//Game states
typedef enum {
	MENU,
	LEVELS,
	SETTINGS,
	GAME
} gameScreen;

//Can switch between states owing to this function
typedef void (*screenFunction)(void);

typedef struct {
	Texture2D background, menu, levels;
	Music music;
	bool soundOn;
	float scale;
	int drawWidth, drawHeight, offsetX, offsetY;
	screenFunction currentScreen;


}gameRes;

gameRes resources;

//Initialize resources
void initRes(void) {
	resources.background= LoadTexture("C:/Images/background.png");
	resources.menu = LoadTexture("C:/Images/menu.jpg");
	resources.levels = LoadTexture("C:/Images/levels.png");
	resources.music = LoadMusicStream("C:/Images/thememusic.mp3");
	PlayMusicStream(resources.music);
	resources.soundOn = true;
}

void unlodRes(void) {
	UnloadTexture(resources.background);
	UnloadTexture(resources.menu);
	UnloadTexture(resources.levels);
	UnloadMusicStream(resources.music);
}

void drawmenuScreen(void) {



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
	InitWindow(screenWidth, screenHeight, "Candy Crush Saga");

	//Minimum screen size
	SetWindowMinSize(800, 600);

	//Set FPS
	SetTargetFPS(60);
	
	//Initialize sound
	InitAudioDevice();
	initRes();
	


	//Main loop 
	while (!WindowShouldClose()) {

		int currentScreenWidth = GetScreenWidth();
		int currentScreenHeight = GetScreenHeight();

		float scale = (float)currentScreenHeight / (float)gameHeight;
		int drawWidth = (int)(gameWidth * scale);
		int drawHeight = currentScreenHeight;
		int offsetX = (currentScreenWidth - drawWidth) / 2;
		int offsetY = 0;
		
		UpdateMusicStream(resources.music);

		if (!IsMusicStreamPlaying(resources.music) && resources.soundOn) {
			StopMusicStream(resources.music);
			PlayMusicStream(resources.music);
		}
        
		
		BeginDrawing();
		ClearBackground(RAYWHITE);


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
		
		

        EndDrawing();

    }

	CloseWindow();
    return 0;

	}
