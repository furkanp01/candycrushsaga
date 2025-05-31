#include "menu.h"
#include "main.h"
#include "resources.h"
#include <math.h>

gameState currentState = MENU;
float masterVolume = 1.0f;

void drawmenuScreen() {


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
		currentFrame = (currentFrame + 1) % frameCount;
	}


	DrawTexture(resources.backgroundWp, 0, 0, WHITE);

	if (!resources.showmenuSettings) {
		DrawTexture(resources.backgroundWp, 0, 0, WHITE);
	}

	DrawTexturePro(
		resources.menuWp[currentFrame],
		(Rectangle) {
		0, 0, (float)resources.menuWp[currentFrame].width, (float)resources.menuWp[currentFrame].height
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

	float highScoresY = settingsY + 100;
	Rectangle highScoresRect = { centerX, highScoresY, buttonWidth, buttonHeight };
	Vector2 highScoresTextSize = MeasureTextEx(resources.myFont, "High Scores", fontSize, spacing);
	Vector2 highScoresTextPos = {
		centerX + (buttonWidth - highScoresTextSize.x) / 2.0f,
		highScoresY + (buttonHeight - highScoresTextSize.y) / 2.0f
	};

	Rectangle highScoreRect = { centerX, settingsY + 100, buttonWidth, buttonHeight };

	DrawRectangleRounded(playRect, 0.3f, 10, ORANGE);

	DrawTextEx(resources.myFont, "Play", playTextPos, fontSize, spacing, BLACK);

	DrawRectangleRounded(settingsRec, 0.3f, 10, PINK);

	DrawTextEx(resources.myFont, "Settings", settingsTextPos, fontSize, spacing, BLACK);

	DrawRectangleRounded(highScoresRect, 0.3f, 10, BLUE);

	DrawTextEx(resources.myFont, "High Scores", highScoresTextPos, fontSize, spacing, BLACK);


	if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
		Vector2 mouseposS = GetMousePosition();
		if (CheckCollisionPointRec(mouseposS, playRect) && !resources.showmenuSettings) {
			PlaySound(resources.clickButton);
			currentState = LEVELS;
		}
		if (CheckCollisionPointRec(mouseposS, settingsRec) && !resources.showmenuSettings) {
			PlaySound(resources.clickButton);
			resources.showmenuSettings = true;
		}
		if (CheckCollisionPointRec(mouseposS, highScoreRect) && !resources.showmenuSettings) {
			PlaySound(resources.clickButton);
			currentState = HIGHSCORES;
		}
	}



	if (resources.showmenuSettings) {

		DrawRectangle(0, 0, currentScreenWidth, currentScreenHeight, Fade(BLACK, 0.8f));

		Rectangle panel = { centerX - 150, settingsY - 150, buttonWidth + 300, 200 };
		DrawRectangleRounded(panel, 10, 10, ORANGE);


		const char* musicToggleText = resources.soundOn ? "Music: ON" : "Music: Off";
		Vector2 musicSize = MeasureTextEx(resources.myFont, musicToggleText, fontSize, spacing);
		Vector2 musicPos = {
			panel.x + (panel.width - musicSize.x) / 2,
			panel.y + 30
		};
		DrawTextEx(resources.myFont, musicToggleText, musicPos, fontSize, spacing, BLACK);

		Rectangle musicToggle = { panel.x + 60, panel.y + 80, panel.width - 120, 50 };
		DrawRectangleRounded(musicToggle, 0.3f, 10, PINK);
		DrawTextEx(resources.myFont, "Music On/Off", (Vector2) { musicToggle.x + 10, musicToggle.y + 10 },
			fontSize, spacing, BLACK);


		float barWidth = 120;
		float barHeight = 20;
		float barX = musicToggle.x + (musicToggle.width - barWidth) / 2;
		float barY = musicToggle.y + musicToggle.height + 20;

		Rectangle decreaseRect = { barX - 60, barY - 10, 40, 40 };
		Rectangle increaseRect = { barX + barWidth + 20, barY - 10, 40, 40 };

		DrawRectangleRounded(decreaseRect, 0.3f, 10, RED);
		DrawTextEx(resources.myFont, "-", (Vector2) { decreaseRect.x + 12, decreaseRect.y + 5 }, fontSize, spacing, BLACK);

		DrawRectangleRounded(increaseRect, 0.3f, 10, RED);
		DrawTextEx(resources.myFont, "+", (Vector2) { increaseRect.x + 10, increaseRect.y + 5 }, fontSize, spacing, BLACK);

		DrawRectangle(barX, barY, barWidth, barHeight, WHITE);
		DrawRectangle(barX, barY, (int)(barWidth * masterVolume), barHeight, DARKGRAY);

		char volumeText[16];
		int percent = (int)roundf(masterVolume * 100.0f);
		sprintf(volumeText, "%d%%", (int)(masterVolume * 100));
		DrawTextEx(resources.myFont, volumeText, (Vector2) { barX + barWidth + 70, barY - 2 }, fontSize - 7, spacing, BLACK);

		Vector2 closeButtonCenter = { panel.x + panel.width - 30, panel.y - 30 };
		float closeButtonRadius = 25;
		DrawCircleV(closeButtonCenter, closeButtonRadius, DARKGRAY);
		DrawTextEx(resources.myFont, "X", (Vector2) { closeButtonCenter.x - 8, closeButtonCenter.y - 12 }, 30, spacing, WHITE);

		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			Vector2 mouse = GetMousePosition();


			if (CheckCollisionPointCircle(mouse, closeButtonCenter, closeButtonRadius)) {
				PlaySound(resources.clickButton);
				resources.showmenuSettings = false;
			}

			if (CheckCollisionPointRec(mouse, musicToggle)) {
				resources.soundOn = !resources.soundOn;
				PlaySound(resources.clickButton);
				SetMusicVolume(resources.music, resources.soundOn ? masterVolume : 0.0f);
			}

			if (CheckCollisionPointRec(mouse, decreaseRect)) {
				PlaySound(resources.clickButton);
				masterVolume -= 0.1f;
				if (masterVolume < 0.0f) masterVolume = 0.0f;

				masterVolume = roundf(masterVolume * 10.0f) / 10.0f;
				if (resources.soundOn) SetMusicVolume(resources.music, masterVolume);
			}

			if (CheckCollisionPointRec(mouse, increaseRect)) {
				PlaySound(resources.clickButton);
				masterVolume += 0.1f;
				if (masterVolume > 1.0f) masterVolume = 1.0f;
				masterVolume = roundf(masterVolume * 10.0f) / 10.0f;
				if (resources.soundOn) SetMusicVolume(resources.music, masterVolume);
			}

		}
	}
}
