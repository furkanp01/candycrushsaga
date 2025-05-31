#include "main.h"
#include "resources.h"


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
	
	for (int i = 0;i < candyTypes;i++) {
		char path2[128];
		sprintf(path2, "resources/candystripedH%d.png", i);
		resources.stripedHTextures[i] = LoadTexture(path2);
		SetTextureFilter(resources.stripedHTextures[i], TEXTURE_FILTER_BILINEAR);
	}
	
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
	resources.menuWp[0] = LoadTexture("resources/menu.png");
	resources.menuWp[1] = LoadTexture("resources/menu1.png");
	resources.menuWp[2] = LoadTexture("resources/menu2.png");
	resources.menuWp[3] = LoadTexture("resources/menu3.png");
	resources.menuWp[4] = LoadTexture("resources/menu4.png");
	resources.menuWp[5] = LoadTexture("resources/menu5.png");
	resources.menuWp[6] = LoadTexture("resources/menu6.png");
	resources.menuWp[7] = LoadTexture("resources/menu7.png");
	resources.levelWp = LoadTexture("resources/levels.png");
	resources.character1 = LoadTexture("resources/character1.png");
	resources.character2 = LoadTexture("resources/character2.png");
	resources.backIcon = LoadTexture("resources/backIcon.png");
	resources.settingsIcon = LoadTexture("resources/settingsIcon.png");
	resources.music = LoadMusicStream("resources/thememusic.mp3");
	resources.myFont = LoadFont("resources/font.ttf");
	resources.swapSound = LoadSound("resources/swap.wav");
	resources.matchSound = LoadSound("resources/match.wav");
	resources.clickButton = LoadSound("resources/buttonClick.wav");
	resources.winSound = LoadSound("resources/win.wav");
	resources.gameoverSound = LoadSound("resources/gameOver.wav");
	resources.soundOn = true;

	PlayMusicStream(resources.music);
	SetMasterVolume(resources.soundOn ? 1.0f : 0.0f);

}

void unloadRes() {

	for (int i = 0; i < candyTypes; i++) {
		UnloadTexture(resources.normalTextures[i]);
		UnloadTexture(resources.stripedVTextures[i]);
		UnloadTexture(resources.stripedHTextures[i]);
		UnloadTexture(resources.wrappedTextures[i]);
	}

	UnloadTexture(resources.colorBomb);
	UnloadTexture(resources.gameboardWp);
	UnloadTexture(resources.backgroundWp);
	UnloadTexture(resources.menuWp[0]);
	UnloadTexture(resources.menuWp[1]);
	UnloadTexture(resources.menuWp[2]);
	UnloadTexture(resources.menuWp[3]);
	UnloadTexture(resources.menuWp[4]);
	UnloadTexture(resources.menuWp[5]);
	UnloadTexture(resources.menuWp[6]);
	UnloadTexture(resources.menuWp[7]);
	UnloadTexture(resources.levelWp);
	UnloadTexture(resources.backIcon);
	UnloadTexture(resources.settingsIcon);
	
	UnloadFont(resources.myFont);

	UnloadSound(resources.swapSound);
	UnloadSound(resources.matchSound);
	UnloadSound(resources.clickButton);
	UnloadSound(resources.winSound);
	UnloadSound(resources.gameoverSound);

	UnloadMusicStream(resources.music);
}
