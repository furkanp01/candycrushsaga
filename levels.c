#include "levels.h"
#include "menu.h"

infoLevels levelsInfo;
bool isUnlocked[maxLevels] = { true };
int currentScore = 0;
int currentLevel = 0;
infoLevels levels[maxLevels] = {
	{1, 1000, 20},
	{2, 1800, 23},
	{3, 2300, 25},
	{4, 3000, 30},
	{5, 5000, 35}
};

void loadlevelData() {
	FILE* xPtr = fopen("levels.txt", "r");
	if (!xPtr) {
		for (int i = 0; i < maxLevels; i++) {
			isUnlocked[i] = false;
		}
		isUnlocked[0] = true;
		return;
	}

	for (int i = 0; i < maxLevels; i++) {
		int val;
		if (fscanf(xPtr, "%d", &val) == 1) {
			isUnlocked[i] = val ? true : false;
		}
		else {
			isUnlocked[i] = false;
		}
	}

	fclose(xPtr);
	isUnlocked[0] = true;
}

void savelevelData() {
	FILE* xPtr = fopen("levels.txt", "w");
	if (xPtr == NULL) {
		printf("The file couldn't be opened.");
	}
	else {
		for (int i = 0; i < maxLevels; i++) {
			fprintf(xPtr, "%d\n", isUnlocked[i] ? 1 : 0);
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
	int totalHeight = maxLevels * buttonRadius * 2 + (maxLevels - 1) * buttonSpacing;
	int startY = (currentScreenHeight - totalHeight) / 2 + buttonRadius;
	int centerX = (currentScreenWidth / 2) + 5;

	for (int i = 0; i < maxLevels; i++) {
		int cy = startY + i * (buttonRadius * 2 + buttonSpacing);

		Color fillColor = isUnlocked[i] ? (Color) { 173, 216, 230, 255 } : GRAY;
		DrawCircle(centerX, cy, buttonRadius, fillColor);
		DrawCircleLines(centerX, cy, buttonRadius, BLACK);

		char label[3];
		snprintf(label, sizeof(label), "%d", i + 1);

		int fontSize = 30;
		int textWidth = MeasureText(label, fontSize);
		DrawText(label, centerX - textWidth / 2, cy - fontSize / 2, fontSize, BLACK);

		if (!isUnlocked[i]) {
			DrawText("X", centerX - 12, cy - 18, 36, DARKGRAY);
		}

		if (isUnlocked[i] && !resources.showlevelSettings) {
			if (CheckCollisionPointCircle(GetMousePosition(), (Vector2) { centerX, cy }, buttonRadius)) {
				if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
					PlaySound(resources.clickButton); 
					levelsInfo.levelId = levels[i].levelId;
					resources.targetScore = levels[i].targetScore;
					resources.moves = levels[i].moves;
					currentScore = 0;
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
			PlaySound(resources.clickButton); 
			currentState = MENU;
		}
		else if (CheckCollisionPointRec(mouse, settingsRect)) {
			PlaySound(resources.clickButton); 
			resources.showlevelSettings = !resources.showlevelSettings;
		}
	}

	if (resources.showlevelSettings) {
		int panelW = 400, panelH = 300;
		int panelX = (currentScreenWidth - panelW) / 2;
		int panelY = (GetScreenHeight() - panelH) / 2;
		Rectangle panelRect = { panelX, panelY, panelW, panelH };

		DrawRectangleRounded(panelRect, 0.1, 1, Fade(ORANGE, 2.0f));

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
				PlaySound(resources.clickButton); 
				resources.soundOn = !resources.soundOn;
				SetMusicVolume(resources.music, resources.soundOn ? 1.0f : 0.0f); 
			}
			else if (CheckCollisionPointRec(mouse, backBtn)) {
				PlaySound(resources.clickButton); 
				resources.showlevelSettings = false;
			}
		}
	}
}
