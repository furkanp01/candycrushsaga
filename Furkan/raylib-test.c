
#define SATIR_SAYISI 8
#define SUTUN_SAYISI 8
#define HEDEF_SEKER_BOYUTU 75
#define SEKERSAYISI 6


#include <stdio.h>
#include "raylib.h"
#include <time.h>

void StartGame() {
    const int ekranGenisligi = SUTUN_SAYISI * HEDEF_SEKER_BOYUTU;
    const int ekranYuksekligi = SATIR_SAYISI * HEDEF_SEKER_BOYUTU;

    InitWindow(ekranGenisligi, ekranYuksekligi, "Candy Crush Tahtası");

    SetTargetFPS(60);

    // Şeker görsellerini yükle
    Texture2D sekerler[SEKERSAYISI];
    sekerler[0] = LoadTexture("resources/candy0.png");
    sekerler[1] = LoadTexture("resources/candy1.png");
    sekerler[2] = LoadTexture("resources/candy2.png");
    sekerler[3] = LoadTexture("resources/candy3.png");
    sekerler[4] = LoadTexture("resources/candy4.png");
    sekerler[5] = LoadTexture("resources/candy5.png");

    // Oyun tahtası (her hücredeki şekerin indeksini tutar)
    int tahta[SATIR_SAYISI][SUTUN_SAYISI];

    // Rastgele sayı üreteciyi başlat
    srand(time(NULL));

    // Oyun tahtasını rastgele şekerlerle doldur
    for (int i = 0; i < SATIR_SAYISI; i++) {
        for (int j = 0; j < SUTUN_SAYISI; j++) {
            tahta[i][j] = rand() % SEKERSAYISI;
        }
    }

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Oyun tahtasını ve şekerleri çiz
        for (int i = 0; i < SATIR_SAYISI; i++) {
            for (int j = 0; j < SUTUN_SAYISI; j++) {
                int sekerIndex = tahta[i][j];
                Rectangle hedefRect = { (float)j * HEDEF_SEKER_BOYUTU, (float)i * HEDEF_SEKER_BOYUTU, HEDEF_SEKER_BOYUTU, HEDEF_SEKER_BOYUTU };
                DrawTexturePro(
                    sekerler[sekerIndex], // Çizilecek texture
                    (Rectangle) {
                    0.0f, 0.0f, (float)sekerler[sekerIndex].width, (float)sekerler[sekerIndex].height
                }, // Kaynak dikdörtgen (tamamı)
                    hedefRect,             // Hedef dikdörtgen (konum ve boyut)
                    (Vector2) {
                    0.0f, 0.0f
                }, // Döndürme merkezi (sol üst köşe)
                    0.0f,                  // Döndürme açısı
                    WHITE                  // Renklendirme (beyaz = orijinal renkler)
                );
                DrawRectangleLinesEx(hedefRect, 2.0f, LIGHTGRAY); // Hücre kenarlıklarını tekrar çizelim
            }
        }

        EndDrawing();
    }

    // Yüklenen görselleri temizle
    for (int i = 0; i < SEKERSAYISI; i++) {
        UnloadTexture(sekerler[i]);
    }

    CloseWindow();

    return 0;
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

    Texture2D background = LoadTexture("resources/backgroundWallpaper.png");
    Texture2D character = LoadTexture("resources/candyCharacter.png");
    Texture2D logo = LoadTexture("resources/candyLogo.png");

    Rectangle rec = { 850,600,250,70 };



    const char* play = "Play";
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
        DrawText(play, 920, 610, 50, BLACK);

        int mouseOver = CheckCollisionPointRec(mousePos, rec);

        if (mouseOver && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            StartGame();
        }




        EndDrawing();
    }

    CloseWindow();

    return 0;


}
