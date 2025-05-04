#include <stdio.h>
#include "raylib.h"
#include <time.h>

void StartGame() {
	
}
//Title properties
#define screenTitle "Candy Crush Saga"
#define fontSize 100
#define titleColor RED

//Properties for background board
#define screenWidth 1920
#define screenHeight 1080

//Properties for game screen
#define gameWidth 680
#define gameHeight 1080

int main() {

	//Initialize Window
	InitWindow(screenWidth, screenHeight, screenTitle);

	

	//Set FPS
	SetTargetFPS(60);

	//Title x and y positions
	int posX = 500;
	int posY = 540;

	Vector2 mousePos = GetMousePosition();

	Texture2D background = LoadTexture("C:/Images/backgroundWallpaper.png");
	Texture2D character = LoadTexture("C:/Images/candyCharacter.png");
	Texture2D logo = LoadTexture("C:/Images/candyLogo.png");
	
	Rectangle rec = { 850,600,250,70 };

	

	const char *play = "Play";
	//Time function for randomness
	srand(time(NULL));
	
	//Main loop 
	while (!WindowShouldClose()) {

		
		BeginDrawing();
		ClearBackground(WHITE);
		

		DrawTexture(background, 0, 0, WHITE);
		DrawTexture(character, 800, 700, WHITE);
		DrawTexture(logo, 800, 0, WHITE);
		DrawRectangleRounded(rec, 30, 10, PINK);
		DrawText(play,920, 610, 50, BLACK);
		
		int mouseOver = CheckCollisionPointRec(mousePos, rec);

		if (mouseOver && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			StartGame();
		}
		
		
			
		
        EndDrawing();
    }

    CloseWindow();

    return 0;


	}
