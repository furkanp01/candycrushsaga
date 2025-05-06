#include <raylib.h>

int main() {
	
	InitWindow(1280, 720, "Candy Crush Saga");
	Texture2D background = LoadTexture("resources/wp.png");
	Texture2D character = LoadTexture("resources/candyCharacter.png");
	Texture2D logo = LoadTexture("resources/candyLogo.png");
	Texture2D candy[6];
	candy[0] = LoadTexture("candy0.png");
	candy[1] = LoadTexture("candy1.png");
	candy[2] = LoadTexture("candy2.png");
	candy[3] = LoadTexture("candy3.png");
	candy[4] = LoadTexture("candy4.png");
	candy[5] = LoadTexture("candy5.png");


	SetTargetFPS(60);
	//Game Loop
	while (WindowShouldClose() == false) {
		//1. Event handling
		
		//2. Updating Positions
        
        
		//3. Drawing
		BeginDrawing();
		ClearBackground(WHITE);
		DrawTexture(background, 0, 0, WHITE);
		DrawTexture(character, 640, 520, WHITE);
		DrawTexture(logo, 640, 0, WHITE);


		

		EndDrawing();
	}

	CloseWindow();
	return 0;
}
