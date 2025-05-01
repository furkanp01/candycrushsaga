#include "raylib.h"
#include <stdlib.h>
#include <time.h>

#define SATIR_SAYISI 8
#define SUTUN_SAYISI 8
#define HEDEF_SEKER_BOYUTU 75
#define SEKERSAYISI 6

int main() {
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