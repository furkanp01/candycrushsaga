#ifndef mainH
#define mainH

#include "raylib.h"
#include <stdio.h>
#include <stdbool.h>

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
#define maxScores 5
#define frameCount 8

typedef enum {
    MENU,
    LEVELS,
    GAME,
    GAMEOVER,
    WIN,
    HIGHSCORES,
    QUIT
} gameState;

// Animasyon yapýlarý
typedef struct {
    Vector2 startPos;
    Vector2 endPos;
    float t;
    bool active;
} animSwap;

typedef struct {
    bool active;
    float timer;
} animExplode;

// Þeker (candy) yapýsý
typedef struct {
    int baseType;
    int specialType;
    Vector2 position;
    Vector2 targetPosition;
} candyState;

// Seviye bilgi yapýsý
typedef struct {
    int levelId;
    int targetScore;
    int moves;
} infoLevels;

// Oyun kaynaklarý yapýsý
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
    bool justCausedWrappedExplosion[gridSize][gridSize];

    Texture2D normalTextures[candyTypes];
    Texture2D stripedHTextures[candyTypes];
    Texture2D stripedVTextures[candyTypes];
    Texture2D wrappedTextures[candyTypes];
    Texture2D colorBomb;
    Texture2D backgroundWp, menuWp[frameCount], levelWp, backIcon, settingsIcon, gameboardWp,character1,character2;

    Sound swapSound;
    Sound matchSound;
    Sound clickButton;
    Sound winSound;
    Sound gameoverSound;

    Music music;

    Font myFont;

} gameResources;

extern gameResources resources;
extern infoLevels levelsInfo;
extern gameState currentState;
extern int highScores[maxScores];
extern int scoreCount;
extern bool isUnlocked[maxLevels];
extern int currentScore;
extern int currentLevel;
extern infoLevels levels[maxLevels];
extern bool pendingSwapCheck;
extern bool revertSwap;
extern bool justCausedWrappedExplosion[gridSize][gridSize];

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
void unloadRes(void);
bool checkFourMatchAndMakeStriped();
bool checkFiveMatchAndMakeColorBomb();
bool checkWrappedMatchAndMakePackage();
bool checkFourStripedCandyMatchAndTrigger();
void triggerFourStripedHorizontal(int row, int col);
void triggerFourStripedVertical(int row, int col);
void triggerFiveColorBomb(int row, int col, int targetBaseType);
void triggerWrappedPackage(int row, int col);
bool checkWrappedCandyAndTrigger();

#endif
