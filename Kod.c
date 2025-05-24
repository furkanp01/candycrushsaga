#include <stdio.h>
#include "raylib.h"
#include <time.h>
#include <stdlib.h>
#include <string.h>

#define screenWidth 1366
#define screenHeight 768
#define gameWidth 900
#define gameHeight 1280
#define gridSize 10
#define candyTypes 6
#define maxLevels 5
#define cellSize 60
#define swapSpeed 8.0f
#define explodeDuration 0.3f
#define specialTypes 4
#define maxScores 100

int highScores[maxScores];
int scoreCount = 0;

//Game states
typedef enum {
	MENU,
	LEVELS,
	GAME,
	GAMEOVER,
	WIN,
	HIGHSCORES,
	QUIT
}gameState;

//Animation types
typedef struct{

	Vector2 startPos;
	Vector2 endPos;
	float t;
	bool active;

}animSwap;

typedef struct {
	bool active;
	float timer;
}animExplode;


//Candy structure
typedef struct {
	int baseType;
	int specialType;
	Vector2 position;
	Vector2 targetPosition;
}candyState;

typedef struct {

	int levelId;
	int targetScore;
	int moves;

}infoLevels;

infoLevels levelsInfo;


//Game board structure
typedef struct {

	candyState gameBoard[gridSize][gridSize];
	
	int score;
	int levelId;
	int targetScore;
	int moves;

	bool soundOn;
	bool showmenuSettings;
	bool showlevelSettings;
	bool showgameSettings;
	
	Texture2D normalTextures[candyTypes];
	Texture2D stripedHTextures[candyTypes];
	Texture2D stripedVTextures[candyTypes];
	Texture2D wrappedTextures[candyTypes];
	Texture2D colorBomb;
	Texture2D backgroundWp, menuWp, levelWp,backIcon,settingsIcon,gameboardWp;

	Sound swapSound;
	Sound matchSound;
	Sound clickButton;
	Sound winSound;
	Sound gameoverSound;
	
	Music music;

	Font myFont;

}gameResources;

gameResources resources;
infoLevels levelsInfo;
gameState currentState;
int highScores[maxScores];
int scoreCount;
bool isUnlocked[maxLevels];
int currentScore;
int currentLevel;
infoLevels levels[maxLevels];
Vector2 gridOffset;
bool hasSelected;
Vector2 selectedCell;
animSwap swapAnim;
animExplode explodeAnims[gridSize][gridSize];
bool isSwapping;
Vector2 swap1, swap2;
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
void ToggleSound(void);
void unloadRes(void);



void initRes() {

	for (int i = 0; i < candyTypes; i++) {
		char path[128];
		sprintf(path, "resources/candy%d.png", i);
		resources.normalTextures[i] = LoadTexture(path);
		SetTextureFilter(resources.normalTextures[i], TEXTURE_FILTER_BILINEAR);
	}
	
	for (int i = 0;i < candyTypes;i++) {
		char path1[128];
		sprintf(path1, "resources/candystripedV%d.png", i);
		resources.stripedVTextures[i] = LoadTexture(path1);
		SetTextureFilter(resources.stripedVTextures[i], TEXTURE_FILTER_BILINEAR);
	}
	//Striped Horizontal
	for (int i = 0;i < candyTypes;i++) {
		char path2[128];
		sprintf(path2, "resources/candystripedH%d.png", i);
		resources.stripedHTextures[i] = LoadTexture(path2);
		SetTextureFilter(resources.stripedHTextures[i], TEXTURE_FILTER_BILINEAR);
	}
	//Wrapped
	for (int i = 0;i < candyTypes;i++) {
		char path3[128];
		sprintf(path3, "resources/candyWrapped%d.png", i);
		resources.wrappedTextures[i] = LoadTexture(path3);
		SetTextureFilter(resources.wrappedTextures[i], TEXTURE_FILTER_BILINEAR);
	}

	resources.colorBomb = LoadTexture("resources/colorBomb.png");
	SetTextureFilter(resources.colorBomb, TEXTURE_FILTER_BILINEAR);

	resources.gameboardWp = LoadTexture("resources/gameBoardWp.png");
	resources.backgroundWp = LoadTexture("resources/background.png");
	resources.menuWp = LoadTexture("resources/menu.jpg");
	resources.levelWp = LoadTexture("resources/levels.png");
	resources.backIcon = LoadTexture("resources/backIcon.png");
	resources.settingsIcon = LoadTexture("resources/settingsIcon.png");
	resources.music = LoadMusicStream("resources/thememusic.mp3");
	resources.myFont = LoadFont("resources/font.ttf");
	resources.swapSound = LoadSound("resources/swap.wav");
	resources.matchSound = LoadSound("resources/match.wav");
	resources.clickButton = LoadSound("resources/buttonClick.wav");
	resources.winSound = LoadSound("resources/win.wav");
	resources.gameoverSound = LoadSound("resources/gameOver.wav");
	resources.soundOn = false;
	
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
			
			currentState = LEVELS;
		}
		if (CheckCollisionPointRec(mouseposS, settingsRec) && !resources.showmenuSettings) {
			resources.showmenuSettings = true;
		}
		if (CheckCollisionPointRec(mouseposS, highScoreRect) && !resources.showmenuSettings) {
			
			currentState = HIGHSCORES;
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

int currentScore = 0;


void loadlevelData() {
	FILE* xPtr = fopen("levels.txt", "r");
	if (xPtr == NULL) {
		printf("File couldn't be opened.");
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

infoLevels levels[maxLevels] = {
	{1,2000,18},
	{2,3000,20},
	{3,4800,46},
	{4,2700,10},
	{5,3600,12}
};

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

		// Kilitli seviye ise üzerine kilit işareti çiz
		if (!isUnlocked[i]) {
			DrawText("X", centerX - 12, cy - 18, 36, DARKGRAY);
		}

		// Sadece açık olan seviyeler tıklanabilir
		if (isUnlocked[i] && !resources.showlevelSettings) {
			if (CheckCollisionPointCircle(GetMousePosition(), (Vector2) { centerX, cy }, buttonRadius)) {
				if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
					currentLevel = i;
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
		removeMatches();
	}

	void drawCandies() {
		for (int i = 0; i < gridSize; i++) {
			for (int j = 0; j < gridSize; j++) {
				int type = resources.gameBoard[i][j].baseType;
				int special = resources.gameBoard[i][j].specialType;

				if (type >= 0) {
					Texture2D tex;

					if (special == 1)      tex = resources.stripedHTextures[type];
					else if (special == 2) tex = resources.stripedVTextures[type];
					else if (special == 3) tex = resources.wrappedTextures[type];
					else if (special == 4) tex = resources.colorBomb;
					else                   tex = resources.normalTextures[type];

					Vector2 pos = { gridOffset.x + j * cellSize, gridOffset.y + i * cellSize };
					DrawTexture(tex, pos.x, pos.y, WHITE);
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

	animSwap swapAnim = { 0 };
	animExplode explodeAnims[gridSize][gridSize] = { 0 };
	bool isSwapping = false;
	Vector2 swap1, swap2;

	void swapCandies(int row1, int col1, int row2, int col2) {

		PlaySound(resources.swapSound);
		swap1 = (Vector2){ col1, row1 };
		swap2 = (Vector2){ col2, row2 };

		candyState temp = resources.gameBoard[row1][col1];
		resources.gameBoard[row1][col1] = resources.gameBoard[row2][col2];
		resources.gameBoard[row2][col2] = temp;

		swapAnim.startPos = resources.gameBoard[row2][col2].position;
		swapAnim.endPos = (Vector2){ col2 * cellSize, row2 * cellSize };
		swapAnim.t = 0.0f;
		swapAnim.active = true;
		isSwapping = true;
	}


	Vector2 vector2Lerp(Vector2 a, Vector2 b, float t) {
		Vector2 result;
		result.x = a.x + (b.x - a.x) * t;
		result.y = a.y + (b.y - a.y) * t;
		return result;
	}

	bool pendingSwapCheck = false;
	bool revertSwap = false; 


	void updateSwapAnimation(float delta) {
		if (swapAnim.active) {
			swapAnim.t += swapSpeed * delta;
			if (swapAnim.t >= 1.0f) {
				swapAnim.t = 1.0f;
				swapAnim.active = false;
				isSwapping = false;

				resources.gameBoard[(int)swap1.y][(int)swap1.x].position = (Vector2){ swap1.x * cellSize, swap1.y * cellSize };
				resources.gameBoard[(int)swap2.y][(int)swap2.x].position = (Vector2){ swap2.x * cellSize, swap2.y * cellSize };

				if (pendingSwapCheck) {
					pendingSwapCheck = false;
					int row1 = (int)swap1.y, col1 = (int)swap1.x;
					int row2 = (int)swap2.y, col2 = (int)swap2.x;

					if (revertSwap) {
						swapCandies(row1, col1, row2, col2); // Swap'ı geri al
						revertSwap = false;
					}
					else {
						removeMatches(); // Patlat ve boşlukları doldur
					}
				}
			}
			else {
				resources.gameBoard[(int)swap1.y][(int)swap1.x].position = vector2Lerp(swapAnim.startPos, swapAnim.endPos, swapAnim.t);
				resources.gameBoard[(int)swap2.y][(int)swap2.x].position = vector2Lerp(swapAnim.endPos, swapAnim.startPos, swapAnim.t);
			}
		}
	}

	bool isValidSwap(int row1, int col1, int row2, int col2) {
		
		if (resources.gameBoard[row1][col1].baseType == resources.gameBoard[row2][col2].baseType)
			return false;

	
		int dx = abs(col1 - col2);
		int dy = abs(row1 - row2);
		if (!((dx == 1 && dy == 0) || (dx == 0 && dy == 1))) return false;

		
		candyState temp = resources.gameBoard[row1][col1];
		resources.gameBoard[row1][col1] = resources.gameBoard[row2][col2];
		resources.gameBoard[row2][col2] = temp;

		bool hasMatch = handleFiveMatch() || handleWrappedMatch() || handleFourMatch() || checkMatches();


		temp = resources.gameBoard[row1][col1];
		resources.gameBoard[row1][col1] = resources.gameBoard[row2][col2];
		resources.gameBoard[row2][col2] = temp;

		return hasMatch;
	}
	

	void updateExplodeAnimation(float delta) {
		for (int i = 0; i < gridSize; i++) {
			for (int j = 0; j < gridSize; j++) {
				if (explodeAnims[i][j].active) {
					explodeAnims[i][j].timer += delta;
					if (explodeAnims[i][j].timer >= explodeDuration) {
						explodeAnims[i][j].active = false;
					}
				}
			}
		}
	}

	bool handleFourMatch() {
		bool found = false;
		// YATAY 4'lü
		for (int i = 0; i < gridSize; i++) {
			for (int j = 0; j < gridSize - 3; j++) {
				int type = resources.gameBoard[i][j].baseType;
				if (type >= 0 &&
					type == resources.gameBoard[i][j + 1].baseType &&
					type == resources.gameBoard[i][j + 2].baseType &&
					type == resources.gameBoard[i][j + 3].baseType) {

					int specialIndex = j + 1; // Ortadakine çizgili vereceğiz

					// Önce eşleşen kutulardaki özel şekerleri patlat!
					for (int k = 0; k < 4; k++) {
						int col = j + k;
						int special = resources.gameBoard[i][col].specialType;
						if (special > 0) {
							triggerSpecialCandy(i, col, -1);
						}
					}

					// Tüm eşleşen kutuları temizle (oluşum yeri hariç)
					for (int k = 0; k < 4; k++) {
						int col = j + k;
						if (col != specialIndex) {
							resources.gameBoard[i][col].baseType = -1;
							resources.gameBoard[i][col].specialType = 0;
							explodeAnims[i][col].active = true;
							explodeAnims[i][col].timer = 0.0f;
							currentScore += 30;
						}
					}
					// Çizgili şekeri oluştur
					resources.gameBoard[i][specialIndex].baseType = type;
					resources.gameBoard[i][specialIndex].specialType = 1; // Yatay çizgili

					found = true;
				}
			}
		}
		// DİKEY 4'lü
		for (int j = 0; j < gridSize; j++) {
			for (int i = 0; i < gridSize - 3; i++) {
				int type = resources.gameBoard[i][j].baseType;
				if (type >= 0 &&
					type == resources.gameBoard[i + 1][j].baseType &&
					type == resources.gameBoard[i + 2][j].baseType &&
					type == resources.gameBoard[i + 3][j].baseType) {

					int specialIndex = i + 1; // Ortadakine çizgili vereceğiz

					// Önce eşleşen kutulardaki özel şekerleri patlat!
					for (int k = 0; k < 4; k++) {
						int row = i + k;
						int special = resources.gameBoard[row][j].specialType;
						if (special > 0) {
							triggerSpecialCandy(row, j, -1);
						}
					}

					// Tüm eşleşen kutuları temizle (oluşum yeri hariç)
					for (int k = 0; k < 4; k++) {
						int row = i + k;
						if (row != specialIndex) {
							resources.gameBoard[row][j].baseType = -1;
							resources.gameBoard[row][j].specialType = 0;
							explodeAnims[row][j].active = true;
							explodeAnims[row][j].timer = 0.0f;
							currentScore += 30;
						}
					}
					// Çizgili şekeri oluştur
					resources.gameBoard[specialIndex][j].baseType = type;
					resources.gameBoard[specialIndex][j].specialType = 2; // Dikey çizgili

					found = true;
				}
			}
		}
		return found;
	}
	bool handleWrappedMatch() {
		bool found = false;
		// T ve L şekli için tüm olasılıkları kontrol et!
		for (int i = 0; i < gridSize; i++) {
			for (int j = 0; j < gridSize; j++) {
				int t = resources.gameBoard[i][j].baseType;
				// Merkez hücre normal olmalı!
				if (t < 0 || resources.gameBoard[i][j].specialType != 0) continue;

				// Sağ-alt L
				if (i >= 2 && j + 2 < gridSize &&
					resources.gameBoard[i - 1][j].baseType == t && resources.gameBoard[i - 2][j].baseType == t &&
					resources.gameBoard[i][j + 1].baseType == t && resources.gameBoard[i][j + 2].baseType == t &&
					resources.gameBoard[i - 1][j].specialType == 0 && resources.gameBoard[i - 2][j].specialType == 0 &&
					resources.gameBoard[i][j + 1].specialType == 0 && resources.gameBoard[i][j + 2].specialType == 0) {
					resources.gameBoard[i][j].specialType = 3;
					resources.gameBoard[i][j].baseType = t; // <-- EKLENDİ!
					resources.gameBoard[i - 1][j].baseType = -1; resources.gameBoard[i - 1][j].specialType = 0;
					resources.gameBoard[i - 2][j].baseType = -1; resources.gameBoard[i - 2][j].specialType = 0;
					resources.gameBoard[i][j + 1].baseType = -1; resources.gameBoard[i][j + 1].specialType = 0;
					resources.gameBoard[i][j + 2].baseType = -1; resources.gameBoard[i][j + 2].specialType = 0;
					found = true;
					continue;
				}
				// Sol-alt L
				if (i >= 2 && j >= 2 &&
					resources.gameBoard[i - 1][j].baseType == t && resources.gameBoard[i - 2][j].baseType == t &&
					resources.gameBoard[i][j - 1].baseType == t && resources.gameBoard[i][j - 2].baseType == t &&
					resources.gameBoard[i - 1][j].specialType == 0 && resources.gameBoard[i - 2][j].specialType == 0 &&
					resources.gameBoard[i][j - 1].specialType == 0 && resources.gameBoard[i][j - 2].specialType == 0) {
					resources.gameBoard[i][j].specialType = 3;
					resources.gameBoard[i][j].baseType = t; // <-- EKLENDİ!
					resources.gameBoard[i - 1][j].baseType = -1; resources.gameBoard[i - 1][j].specialType = 0;
					resources.gameBoard[i - 2][j].baseType = -1; resources.gameBoard[i - 2][j].specialType = 0;
					resources.gameBoard[i][j - 1].baseType = -1; resources.gameBoard[i][j - 1].specialType = 0;
					resources.gameBoard[i][j - 2].baseType = -1; resources.gameBoard[i][j - 2].specialType = 0;
					found = true;
					continue;
				}
				// Sağ-üst L
				if (i + 2 < gridSize && j + 2 < gridSize &&
					resources.gameBoard[i + 1][j].baseType == t && resources.gameBoard[i + 2][j].baseType == t &&
					resources.gameBoard[i][j + 1].baseType == t && resources.gameBoard[i][j + 2].baseType == t &&
					resources.gameBoard[i + 1][j].specialType == 0 && resources.gameBoard[i + 2][j].specialType == 0 &&
					resources.gameBoard[i][j + 1].specialType == 0 && resources.gameBoard[i][j + 2].specialType == 0) {
					resources.gameBoard[i][j].specialType = 3;
					resources.gameBoard[i][j].baseType = t; // <-- EKLENDİ!
					resources.gameBoard[i + 1][j].baseType = -1; resources.gameBoard[i + 1][j].specialType = 0;
					resources.gameBoard[i + 2][j].baseType = -1; resources.gameBoard[i + 2][j].specialType = 0;
					resources.gameBoard[i][j + 1].baseType = -1; resources.gameBoard[i][j + 1].specialType = 0;
					resources.gameBoard[i][j + 2].baseType = -1; resources.gameBoard[i][j + 2].specialType = 0;
					found = true;
					continue;
				}
				// Sol-üst L
				if (i + 2 < gridSize && j >= 2 &&
					resources.gameBoard[i + 1][j].baseType == t && resources.gameBoard[i + 2][j].baseType == t &&
					resources.gameBoard[i][j - 1].baseType == t && resources.gameBoard[i][j - 2].baseType == t &&
					resources.gameBoard[i + 1][j].specialType == 0 && resources.gameBoard[i + 2][j].specialType == 0 &&
					resources.gameBoard[i][j - 1].specialType == 0 && resources.gameBoard[i][j - 2].specialType == 0) {
					resources.gameBoard[i][j].specialType = 3;
					resources.gameBoard[i][j].baseType = t; // <-- EKLENDİ!
					resources.gameBoard[i + 1][j].baseType = -1; resources.gameBoard[i + 1][j].specialType = 0;
					resources.gameBoard[i + 2][j].baseType = -1; resources.gameBoard[i + 2][j].specialType = 0;
					resources.gameBoard[i][j - 1].baseType = -1; resources.gameBoard[i][j - 1].specialType = 0;
					resources.gameBoard[i][j - 2].baseType = -1; resources.gameBoard[i][j - 2].specialType = 0;
					found = true;
					continue;
				}
			}
		}
		return found;
	}

	bool handleFiveMatch() {
		bool found = false;

		// Yatay 5'li
		for (int i = 0; i < gridSize; i++) {
			for (int j = 0; j < gridSize - 4; j++) {
				int type = resources.gameBoard[i][j].baseType;
				if (type >= 0 &&
					type == resources.gameBoard[i][j + 1].baseType &&
					type == resources.gameBoard[i][j + 2].baseType &&
					type == resources.gameBoard[i][j + 3].baseType &&
					type == resources.gameBoard[i][j + 4].baseType) {

					for (int k = 0; k < 5; k++) {
						resources.gameBoard[i][j + k].baseType = -1;
						resources.gameBoard[i][j + k].specialType = 0;
						explodeAnims[i][j + k].active = true;
						explodeAnims[i][j + k].timer = 0.0f; // Hatalı: explodeAnims[i][k].timer yerine j+k kullandık!
						currentScore += 40; // Points for each candy in the match
					}

					int center = j + 2;
					resources.gameBoard[i][center].baseType = type;
					resources.gameBoard[i][center].specialType = 4; // Color Bomb
					found = true;
				}
			}
		}

		// Dikey 5'li
		for (int j = 0; j < gridSize; j++) {
			for (int i = 0; i < gridSize - 4; i++) {
				int type = resources.gameBoard[i][j].baseType;
				if (type >= 0 &&
					type == resources.gameBoard[i + 1][j].baseType &&
					type == resources.gameBoard[i + 2][j].baseType &&
					type == resources.gameBoard[i + 3][j].baseType &&
					type == resources.gameBoard[i + 4][j].baseType) {

					for (int k = 0; k < 5; k++) {
						resources.gameBoard[i + k][j].baseType = -1;
						resources.gameBoard[i + k][j].specialType = 0;
						explodeAnims[i + k][j].active = true;
						explodeAnims[i + k][j].timer = 0.0f;
						currentScore += 40; // Points for each candy in the match
					}

					int center = i + 2;
					resources.gameBoard[center][j].baseType = type;
					resources.gameBoard[center][j].specialType = 4; // Color Bomb
					found = true;
				}
			}
		}

		return found;
	}

	void triggerSpecialCandy(int row, int col, int colorBombTargetColor) {
		// Zaten patlatılmışsa tekrar patlatma!
		if (resources.gameBoard[row][col].baseType == -1)
			return;

		int special = resources.gameBoard[row][col].specialType;
		int type = resources.gameBoard[row][col].baseType;

		// Önce hemen patlatılan hücreyi temizle (zincirleme çağrılarda yeniden girilmesin)
		resources.gameBoard[row][col].baseType = -1;
		resources.gameBoard[row][col].specialType = 0;
		explodeAnims[row][col].active = true;
		explodeAnims[row][col].timer = 0.0f;

		if (special == 1) { // Striped yatay (Horizontal)
			for (int j = 0; j < gridSize; j++) {
				if (resources.gameBoard[row][j].baseType != -1) {
					int otherSpecial = resources.gameBoard[row][j].specialType;
					if (otherSpecial > 0) {
						triggerSpecialCandy(row, j, -1);
					}
					else {
						resources.gameBoard[row][j].baseType = -1;
						resources.gameBoard[row][j].specialType = 0;
						explodeAnims[row][j].active = true;
						explodeAnims[row][j].timer = 0.0f;
					}
					currentScore += 10;
				}
			}
		}
		else if (special == 2) { // Striped dikey (Vertical)
			for (int i = 0; i < gridSize; i++) {
				if (resources.gameBoard[i][col].baseType != -1) {
					int otherSpecial = resources.gameBoard[i][col].specialType;
					if (otherSpecial > 0) {
						triggerSpecialCandy(i, col, -1);
					}
					else {
						resources.gameBoard[i][col].baseType = -1;
						resources.gameBoard[i][col].specialType = 0;
						explodeAnims[i][col].active = true;
						explodeAnims[i][col].timer = 0.0f;
					}
					currentScore += 10;
				}
			}
		}
		else if (special == 3) { // Wrapped (3x3)
			for (int di = -1; di <= 1; di++) {
				for (int dj = -1; dj <= 1; dj++) {
					int r = row + di;
					int c = col + dj;
					if (r >= 0 && r < gridSize && c >= 0 && c < gridSize && resources.gameBoard[r][c].baseType != -1) {
						int otherSpecial = resources.gameBoard[r][c].specialType;
						if (otherSpecial > 0) {
							triggerSpecialCandy(r, c, -1);
						}
						else {
							resources.gameBoard[r][c].baseType = -1;
							resources.gameBoard[r][c].specialType = 0;
							explodeAnims[r][c].active = true;
							explodeAnims[r][c].timer = 0.0f;
						}
						currentScore += 15;
					}
				}
			}
		}
		else if (special == 4) { // Color Bomb
			int colorType = colorBombTargetColor;
			for (int i = 0; i < gridSize; i++) {
				for (int j = 0; j < gridSize; j++) {
					if (resources.gameBoard[i][j].baseType == colorType &&
						resources.gameBoard[i][j].specialType != 4 &&
						resources.gameBoard[i][j].baseType != -1) {
						int otherSpecial = resources.gameBoard[i][j].specialType;
						if (otherSpecial > 0) {
							triggerSpecialCandy(i, j, -1);
						}
						else {
							resources.gameBoard[i][j].baseType = -1;
							resources.gameBoard[i][j].specialType = 0;
							explodeAnims[i][j].active = true;
							explodeAnims[i][j].timer = 0.0f;
						}
						currentScore += 20;
					}
				}
			}
		}
		
		resources.gameBoard[row][col].baseType = -1;
		resources.gameBoard[row][col].specialType = 0;
	}

	bool checkMatches() {
		bool matchFound = false;

		// YATAY
		for (int i = 0; i < gridSize; i++) {
			for (int j = 0; j < gridSize - 2; j++) {
				int t0 = resources.gameBoard[i][j].baseType;
				int t1 = resources.gameBoard[i][j + 1].baseType;
				int t2 = resources.gameBoard[i][j + 2].baseType;
				if (t0 >= 0 && t1 >= 0 && t2 >= 0 && (t0 == t1 && t1 == t2)) {
					for (int k = 0; k < 3; k++) {
						int col = j + k;
						int special = resources.gameBoard[i][col].specialType;
						if (special > 0) {
							triggerSpecialCandy(i, col, -1);
						}
						else {
							explodeAnims[i][col].active = true;
							explodeAnims[i][col].timer = 0.0f;
							resources.gameBoard[i][col].baseType = -1;
							resources.gameBoard[i][col].specialType = 0;
							currentScore += 20;
						}
					}
					matchFound = true;
				}
			}
		}

		// DİKEY
		for (int j = 0; j < gridSize; j++) {
			for (int i = 0; i < gridSize - 2; i++) {
				int t0 = resources.gameBoard[i][j].baseType;
				int t1 = resources.gameBoard[i + 1][j].baseType;
				int t2 = resources.gameBoard[i + 2][j].baseType;
				if (t0 >= 0 && t1 >= 0 && t2 >= 0 && (t0 == t1 && t1 == t2)) {
					for (int k = 0; k < 3; k++) {
						int row = i + k;
						int special = resources.gameBoard[row][j].specialType;
						if (special > 0) {
							triggerSpecialCandy(row, j, -1);
						}
						else {
							explodeAnims[row][j].active = true;
							explodeAnims[row][j].timer = 0.0f;
							resources.gameBoard[row][j].baseType = -1;
							resources.gameBoard[row][j].specialType = 0;
							currentScore += 20;
						}
					}
					matchFound = true;
				}
			}
		}
		return matchFound;
	}

	void selectCandy() {
		if (isSwapping) return;

		Vector2 mousePos = GetMousePosition();
		Vector2 cell = getCellFromMouse(mousePos);

		if (cell.x < 0 || cell.x >= gridSize || cell.y < 0 || cell.y >= gridSize) return;

		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			if (!hasSelected) {
				selectedCell = cell;
				hasSelected = true;
			}
			else {
				int dx = abs((int)cell.x - (int)selectedCell.x);
				int dy = abs((int)cell.y - (int)selectedCell.y);
				if ((dx == 1 && dy == 0) || (dx == 0 && dy == 1)) {
					int row1 = (int)selectedCell.y;
					int col1 = (int)selectedCell.x;
					int row2 = (int)cell.y;
					int col2 = (int)cell.x;

					int s1 = resources.gameBoard[row1][col1].specialType;
					int s2 = resources.gameBoard[row2][col2].specialType;

					// Sadece color bomb + normal şeker için özel patlatma
					if ((s1 == 4 && s2 == 0) || (s2 == 4 && s1 == 0)) {
						int bombRow, bombCol, targetColor;
						if (s1 == 4) {
							bombRow = row1; bombCol = col1;
							targetColor = resources.gameBoard[row2][col2].baseType;
						}
						else {
							bombRow = row2; bombCol = col2;
							targetColor = resources.gameBoard[row1][col1].baseType;
						}
						triggerSpecialCandy(bombRow, bombCol, targetColor);
						PlaySound(resources.matchSound);
						hasSelected = false;
						resources.moves--;
						return; // Artık normal swap işlemi yapılmasın!
					}

					resources.moves--;

					if (isValidSwap(row1, col1, row2, col2)) {
						swapCandies(row1, col1, row2, col2);
						pendingSwapCheck = true;
						revertSwap = false;
					}
					else {
						swapCandies(row1, col1, row2, col2);
						pendingSwapCheck = true;
						revertSwap = true;
					}
					hasSelected = false;
				}
				else {
					selectedCell = cell;
				}
			}
		}
	}

	bool dropOneCandy() {
		bool moved = false;
		for (int col = 0; col < gridSize; col++) {
			for (int row = gridSize - 1; row > 0; row--) {
				if (resources.gameBoard[row][col].baseType == -1 && resources.gameBoard[row - 1][col].baseType != -1) {
					resources.gameBoard[row][col] = resources.gameBoard[row - 1][col];
					resources.gameBoard[row][col].targetPosition = (Vector2){ col * cellSize, row * cellSize };
					resources.gameBoard[row - 1][col].baseType = -1;
					resources.gameBoard[row - 1][col].specialType = 0;
					resources.gameBoard[row - 1][col].position = (Vector2){ col * cellSize, (row - 1) * cellSize };
					resources.gameBoard[row - 1][col].targetPosition = resources.gameBoard[row - 1][col].position;
					moved = true;
				}
			}
		}
		return moved;
	}

	
	bool spawnCandies() {
		bool spawned = false;
		for (int col = 0; col < gridSize; col++) {
			if (resources.gameBoard[0][col].baseType == -1) {
				int newType = rand() % candyTypes;
				resources.gameBoard[0][col].baseType = newType;
				resources.gameBoard[0][col].specialType = 0;
				resources.gameBoard[0][col].position = (Vector2){ col * cellSize, -cellSize };
				resources.gameBoard[0][col].targetPosition = (Vector2){ col * cellSize, 0 };
				spawned = true;
			}
		}
		return spawned;
	}

	void updateCandyFallAnimation(float fallSpeed) {
		for (int i = 0; i < gridSize; i++) {
			for (int j = 0; j < gridSize; j++) {
				Vector2* pos = &resources.gameBoard[i][j].position;
				Vector2* target = &resources.gameBoard[i][j].targetPosition;

				float dx = target->x - pos->x;
				float dy = target->y - pos->y;

				if (fabsf(dx) < 1.0f && fabsf(dy) < 1.0f) {
					pos->x = target->x;
					pos->y = target->y;
				}
				else {
					
					pos->x += dx * fallSpeed;
					pos->y += dy * fallSpeed;
				}
			}
		}
	}

	void removeMatches() {
		bool found;
		do {
			found = false;

			if (handleFiveMatch()) found = true;
			if (handleWrappedMatch()) found = true;
			if (handleFourMatch()) found = true;
			if (checkMatches()) found = true;
			if (found) PlaySound(resources.matchSound);
			
		} while (found);
	}

	void addHighScore(int score) {
		int i = scoreCount;
		if (scoreCount < maxScores) scoreCount++;
		while (i > 0 && highScores[i - 1] < score) {
			highScores[i] = highScores[i - 1];
			i--;
		}
		highScores[i] = score;
	}

	void saveHighScores() {
		FILE* xPtr = fopen("highscores.txt", "w");
		if (!xPtr) return;
		fprintf(xPtr, "%d\n", scoreCount);
		for (int i = 0; i < scoreCount; i++)
			fprintf(xPtr, "%d\n", highScores[i]);
		fclose(xPtr);
	}

	void loadHighScores() {
		FILE* xPtr = fopen("highscores.txt", "r");
		if (!xPtr) {
			scoreCount = 0;
			return;
		}
		fscanf(xPtr, "%d", &scoreCount);
		for (int i = 0; i < scoreCount && i < maxScores; i++)
			fscanf(xPtr, "%d", &highScores[i]);
		fclose(xPtr);
	}


	void drawHighScoresScreen() {
		int w = GetScreenWidth();
		int h = GetScreenHeight();
		ClearBackground(RAYWHITE);

		DrawText("High-Scores", w / 2 - 180, 80, 42, DARKGRAY);

		// En yüksek skorun indexini bul
		int maxIdx = -1;
		int maxScore = -1;
		for (int i = 0; i < scoreCount; i++) {
			if (highScores[i] > maxScore) {
				maxScore = highScores[i];
				maxIdx = i;
			}
		}

		float fontSize = 32;
		float spacing = 2;

		for (int i = 0; i < scoreCount; i++) {
			int idx = scoreCount - 1 - i;
			Color c = (idx == maxIdx) ? GOLD : BLUE;
			int x = w / 2 - 50;
			int y = 140 + i * 36;
			DrawText(TextFormat("%d. %d", i + 1, highScores[idx]), x, y, (int)fontSize, c);

			// Eğer bu en yüksek skor ise yanına king yaz (senin fontunla)
			if (idx == maxIdx) {
				// Skor yazısının genişliğini ölç
				char scoreText[32];
				snprintf(scoreText, sizeof(scoreText), "%d. %d", i + 1, highScores[idx]);
				Vector2 textSize = MeasureTextEx(resources.myFont, scoreText, fontSize, spacing);
				
				DrawTextEx(
					resources.myFont,
					" (King of The Jungle)",
					(Vector2) {
					x + textSize.x + 8, y
				},
					fontSize * 0.9f, // biraz daha küçük
					spacing,
					GOLD
				);
			}
		}

		Rectangle backRect = { w / 2 - 60, h - 100, 120, 50 };
		DrawRectangleRounded(backRect, 0.3f, 10, LIGHTGRAY);
		DrawText("Back", w / 2 - 30, h - 90, 32, DARKGRAY);
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			Vector2 mouse = GetMousePosition();
			if (CheckCollisionPointRec(mouse, backRect)) {
				currentState = MENU;
			}
		}
	}
	void drawgameScreen() {
		float delta = GetFrameTime();
		updateSwapAnimation(delta);
		updateExplodeAnimation(delta);

		static bool candiesDropping = false;
		bool anyAnimations = false;
		if (swapAnim.active || isSwapping) anyAnimations = true;
		for (int i = 0; i < gridSize && !anyAnimations; i++) {
			for (int j = 0; j < gridSize; j++) {
				if (explodeAnims[i][j].active) {
					anyAnimations = true;
					break;
				}
			}
		}

		// Oyun akışı...
		if (!anyAnimations) {
			if (dropOneCandy()) {
				candiesDropping = true;
			}
			else if (spawnCandies()) {
				candiesDropping = true;
			}
			else if (candiesDropping) {
				candiesDropping = false;
				if (handleFiveMatch() || handleWrappedMatch() || handleFourMatch() || checkMatches()) {
					candiesDropping = true;
				}
			}
		}

		// Çizimler
		DrawTexture(resources.backgroundWp, 0, 0, WHITE);

		int currentScreenWidth = GetScreenWidth();
		int currentScreenHeight = GetScreenHeight();
		float scale = (float)currentScreenHeight / (float)gameHeight;
		int drawWidth = (int)(gameWidth * scale) + 100;
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
		},
			0.0f, WHITE
		);

		DrawText(TextFormat("Score: %d", currentScore), 30, 30, 30, WHITE);
		DrawText(TextFormat("Target Score: %d", resources.targetScore), 30, 70, 30, WHITE);
		DrawText(TextFormat("Moves: %d", resources.moves), 30, 110, 30, WHITE);

		drawgridLines();
		drawCandies();

		if (!resources.showgameSettings && !isSwapping && !swapAnim.active && !anyAnimations && !candiesDropping) {
			selectCandy();
		}

		if (hasSelected) {
			Rectangle selectedRect = {
				gridOffset.x + selectedCell.x * cellSize,
				gridOffset.y + selectedCell.y * cellSize,
				cellSize, cellSize
			};
			DrawRectangleLinesEx(selectedRect, 3, RED);
		}

		if (resources.moves <= 0) {
			if ((scoreCount == 0) || (currentScore > highScores[scoreCount - 1]) || (scoreCount < maxScores)) {
				addHighScore(currentScore);
				saveHighScores();
			}
			if (currentScore >= resources.targetScore) {
				currentState = WIN;
				completeLevel(currentLevel);
			}
			else {
				currentState = GAMEOVER;
			}
		}

		Vector2 backCenter = { 60, currentScreenHeight - 60 };
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
			0.0f, WHITE
		);

		Vector2 mouse = GetMousePosition();
		int settingsSize = 50;
		int settingsX = currentScreenWidth - settingsSize - 30;
		int settingsY = currentScreenHeight - settingsSize - 28;
		Rectangle settingsRect = { settingsX, settingsY, settingsSize, settingsSize };

		DrawTextureEx(resources.settingsIcon, (Vector2) { settingsX, settingsY }, 0.0f,
			(float)settingsSize / resources.settingsIcon.width, WHITE);

		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			if (CheckCollisionPointRec(mouse, backRect)) {
				currentState = LEVELS;
			}
			else if (CheckCollisionPointRec(mouse, settingsRect)) {
				resources.showgameSettings = !resources.showgameSettings; // ARTIK showgameSettings aç/kapa
			}
		}

		// Oyun Ayarları Paneli
		if (resources.showgameSettings) {
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
					resources.soundOn = !resources.soundOn;
					SetMasterVolume(resources.soundOn ? 1.0f : 0.0f);
				}
				if (CheckCollisionPointRec(mouse, backBtn)) {
					resources.showgameSettings = false;
				}
			}
		}
	}

	void drawWin() {
		ClearBackground(DARKGREEN);
		int currentscreenWidth = GetScreenWidth();
		int currentscreenHeight = GetScreenHeight();

		DrawText("You Win!", currentscreenWidth / 2 - 100, currentscreenHeight / 2 - 50, 50, YELLOW);
		DrawText("Press SPACE to return to level screen.", (currentscreenWidth / 2) - 250, currentscreenHeight / 2 + 10, 30, WHITE);
		if (IsKeyPressed(KEY_SPACE)) {
			currentState = LEVELS;
		}
	}

	void drawgameOver() {
		ClearBackground(RED);
		int currentscreenWidth = GetScreenWidth();
		int currentscreenHeight = GetScreenHeight();
		DrawText("Game Over!", currentscreenWidth / 2 - 150, currentscreenHeight / 2 - 50, 50, WHITE);
		DrawText("Press SPACE to return to level screen.", currentscreenWidth / 2 - 320, currentscreenHeight / 2 + 10, 30, WHITE);
		if (IsKeyPressed(KEY_SPACE)) {
			currentState = LEVELS;
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
	
