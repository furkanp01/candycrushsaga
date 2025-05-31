#ifndef levelsH
#define levelsH

#include "main.h"

extern infoLevels levelsInfo;
extern bool isUnlocked[maxLevels];
extern int currentScore;
extern int currentLevel;
extern infoLevels levels[maxLevels];

void loadlevelData(void);
void savelevelData(void);
void completeLevel(int level);
void drawlevelScreen(void);

#endif 