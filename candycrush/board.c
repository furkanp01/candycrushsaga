#include "raylib.h"
#include "main.h"
#include "board.h"
#include "resources.h"
#include <math.h>

// Global değişkenler
Vector2 gridOffset;
bool hasSelected = false;
Vector2 selectedCell = { -1,-1 };
animSwap swapAnim = { 0 };
animExplode explodeAnims[gridSize][gridSize] = { 0 };
bool isSwapping = false;
Vector2 swap1, swap2;
bool pendingSwapCheck = false;
bool revertSwap = false;

// Puan hesaplama fonksiyonu
void addScore(int count) {
    currentScore += count * 20; // Her şeker için 20 puan
}

// Grid pozisyonunu güncelleme
void updateGridOffset() {
    int currentscreenWidth = GetScreenWidth();
    int currentscreenHeight = GetScreenHeight();
    gridOffset.x = (currentscreenWidth - (gridSize * cellSize)) / 2;
    gridOffset.y = (currentscreenHeight - (gridSize * cellSize)) / 2;
}

// Oyun tahtasını başlatma
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

// Şekerleri çizme
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

// Grid çizgilerini çizme
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

// Mouse pozisyonundan hücre koordinatlarını alma
Vector2 getCellFromMouse(Vector2 mousePos) {
    return (Vector2) {
        (int)((mousePos.x - gridOffset.x) / cellSize),
            (int)((mousePos.y - gridOffset.y) / cellSize)
    };
}

// Şekerleri değiştirme
void swapCandies(int row1, int col1, int row2, int col2, bool playSound) {
    if (playSound) PlaySound(resources.swapSound);
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


bool hasThreeMatchOnBoard() {
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            int t = resources.gameBoard[i][j].baseType;
            if (t < 0) continue;
            // Yatay
            if (j + 2 < gridSize &&
                resources.gameBoard[i][j + 1].baseType == t &&
                resources.gameBoard[i][j + 2].baseType == t)
                return true;
            // Dikey
            if (i + 2 < gridSize &&
                resources.gameBoard[i + 1][j].baseType == t &&
                resources.gameBoard[i + 2][j].baseType == t)
                return true;
        }
    }
    return false;
}


bool isValidSwap(int row1, int col1, int row2, int col2) {
    int s1 = resources.gameBoard[row1][col1].specialType;
    int s2 = resources.gameBoard[row2][col2].specialType;
    int t1 = resources.gameBoard[row1][col1].baseType;
    int t2 = resources.gameBoard[row2][col2].baseType;

    // İki özel şeker kombinasyonları
    if (s1 > 0 && s2 > 0) {
        // İki çizgili şeker (1 ve 2)
        if ((s1 == 1 || s1 == 2) && (s2 == 1 || s2 == 2)) {
            triggerFourStripedHorizontal(row1, col1);
            triggerFourStripedVertical(row1, col1);
            triggerFourStripedHorizontal(row2, col2);
            triggerFourStripedVertical(row2, col2);
            addScore(gridSize * 4); // Ekstra puan
            return true;
        }
        // İki paket şeker
        else if (s1 == 3 && s2 == 3) {
            // 5x5 alanı patlat
            for (int i = row1 - 2; i <= row1 + 2; i++) {
                for (int j = col1 - 2; j <= col1 + 2; j++) {
                    if (i >= 0 && i < gridSize && j >= 0 && j < gridSize) {
                        resources.gameBoard[i][j].baseType = -1;
                        resources.gameBoard[i][j].specialType = 0;
                        explodeAnims[i][j].active = true;
                        explodeAnims[i][j].timer = 0.0f;
                    }
                }
            }
            addScore(25 * 20); // 5x5 alan için ekstra puan
            return true;
        }
        // İki renk bombası
        else if (s1 == 4 && s2 == 4) {
            // Tüm tahtayı patlat
            for (int i = 0; i < gridSize; i++) {
                for (int j = 0; j < gridSize; j++) {
                    if (resources.gameBoard[i][j].baseType != -1) {
                        resources.gameBoard[i][j].baseType = -1;
                        resources.gameBoard[i][j].specialType = 0;
                        explodeAnims[i][j].active = true;
                        explodeAnims[i][j].timer = 0.0f;
                    }
                }
            }
            addScore(gridSize * gridSize * 20); // Tüm tahta için ekstra puan
            return true;
        }
    }

    if (s1 == 0 && s2 == 0 && t1 == t2)
        return false;

    // Sadece komşu olanlar swap yapılabilir
    int dx = abs(col1 - col2);
    int dy = abs(row1 - row2);
    if (!((dx == 1 && dy == 0) || (dx == 0 && dy == 1))) return false;

    // Swap'ı uygula
    candyState temp = resources.gameBoard[row1][col1];
    resources.gameBoard[row1][col1] = resources.gameBoard[row2][col2];
    resources.gameBoard[row2][col2] = temp;

    bool hasMatch = hasThreeMatchOnBoard();

    // Swap'ı geri al
    temp = resources.gameBoard[row1][col1];
    resources.gameBoard[row1][col1] = resources.gameBoard[row2][col2];
    resources.gameBoard[row2][col2] = temp;

    return hasMatch;
}

// Özel şeker tetikleme yardımcı fonksiyonu
void triggerSpecialCandy(int row, int col, int specialType, int baseType) {
    switch (specialType) {
    case 1: // Yatay çizgili
        triggerFourStripedHorizontal(row, col);
        break;
    case 2: // Dikey çizgili
        triggerFourStripedVertical(row, col);
        break;
    case 3: // Paket
        triggerWrappedPackage(row, col);
        break;
    case 4: // Renk bombası
        triggerFiveColorBomb(row, col, baseType);
        break;
    }
}

// Patlama animasyonunu güncelleme
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

// Özel şeker eşleşmelerini kontrol etme
bool checkSpecialCandyMatches() {
    bool found = false;
   
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize - 2; j++) {
            int t = resources.gameBoard[i][j].baseType;
            if (t < 0) continue;

            int s0 = resources.gameBoard[i][j].specialType;
            int s1 = resources.gameBoard[i][j + 1].specialType;
            int s2 = resources.gameBoard[i][j + 2].specialType;

            if (t == resources.gameBoard[i][j + 1].baseType &&
                t == resources.gameBoard[i][j + 2].baseType) {

                
                if (s0 == 1) { triggerFourStripedHorizontal(i, j); found = true; }
                if (s1 == 1) { triggerFourStripedHorizontal(i, j + 1); found = true; }
                if (s2 == 1) { triggerFourStripedHorizontal(i, j + 2); found = true; }
                if (s0 == 2) { triggerFourStripedVertical(i, j); found = true; }
                if (s1 == 2) { triggerFourStripedVertical(i, j + 1); found = true; }
                if (s2 == 2) { triggerFourStripedVertical(i, j + 2); found = true; }

               
                if (s0 == 3) { triggerWrappedPackage(i, j); found = true; }
                if (s1 == 3) { triggerWrappedPackage(i, j + 1); found = true; }
                if (s2 == 3) { triggerWrappedPackage(i, j + 2); found = true; }

              
                if (s0 == 0) {
                    resources.gameBoard[i][j].baseType = -1;
                    resources.gameBoard[i][j].specialType = 0;
                    addScore(1);
                }
                if (s1 == 0) {
                    resources.gameBoard[i][j + 1].baseType = -1;
                    resources.gameBoard[i][j + 1].specialType = 0;
                    addScore(1);
                }
                if (s2 == 0) {
                    resources.gameBoard[i][j + 2].baseType = -1;
                    resources.gameBoard[i][j + 2].specialType = 0;
                    addScore(1);
                }
            }
        }
    }

    // DİKEY
    for (int j = 0; j < gridSize; j++) {
        for (int i = 0; i < gridSize - 2; i++) {
            int t = resources.gameBoard[i][j].baseType;
            if (t < 0) continue;

            int s0 = resources.gameBoard[i][j].specialType;
            int s1 = resources.gameBoard[i + 1][j].specialType;
            int s2 = resources.gameBoard[i + 2][j].specialType;

            if (t == resources.gameBoard[i + 1][j].baseType &&
                t == resources.gameBoard[i + 2][j].baseType) {

                // Önce çizgili şekerleri kontrol et
                if (s0 == 1) { triggerFourStripedHorizontal(i, j); found = true; }
                if (s1 == 1) { triggerFourStripedHorizontal(i + 1, j); found = true; }
                if (s2 == 1) { triggerFourStripedHorizontal(i + 2, j); found = true; }
                if (s0 == 2) { triggerFourStripedVertical(i, j); found = true; }
                if (s1 == 2) { triggerFourStripedVertical(i + 1, j); found = true; }
                if (s2 == 2) { triggerFourStripedVertical(i + 2, j); found = true; }

                // Sonra paket şekerleri kontrol et
                if (s0 == 3) { triggerWrappedPackage(i, j); found = true; }
                if (s1 == 3) { triggerWrappedPackage(i + 1, j); found = true; }
                if (s2 == 3) { triggerWrappedPackage(i + 2, j); found = true; }

                // Normal şekerleri sil
                if (s0 == 0) {
                    resources.gameBoard[i][j].baseType = -1;
                    resources.gameBoard[i][j].specialType = 0;
                    addScore(1);
                }
                if (s1 == 0) {
                    resources.gameBoard[i + 1][j].baseType = -1;
                    resources.gameBoard[i + 1][j].specialType = 0;
                    addScore(1);
                }
                if (s2 == 0) {
                    resources.gameBoard[i + 2][j].baseType = -1;
                    resources.gameBoard[i + 2][j].specialType = 0;
                    addScore(1);
                }
            }
        }
    }
    return found;
}

// Eşleşmeleri kaldırma
void removeMatches() {
    bool found;
    do {
        found = false;
        if (checkFiveMatchAndMakeColorBomb()) found = true;
        if (checkWrappedMatchAndMakePackage()) found = true;
        if (checkFourMatchAndMakeStriped()) found = true;
        if (checkSpecialCandyMatches()) found = true;
        if (found) PlaySound(resources.matchSound);
    } while (found);
}

// Yatay çizgili şeker tetikleme
void triggerFourStripedHorizontal(int row, int col) {
    int count = 0;
    for (int j = 0; j < gridSize; j++) {
        if (resources.gameBoard[row][j].baseType != -1) {
            count++;
            resources.gameBoard[row][j].baseType = -1;
            resources.gameBoard[row][j].specialType = 0;
            explodeAnims[row][j].active = true;
            explodeAnims[row][j].timer = 0.0f;
        }
    }
    addScore(count);
    removeMatches(); // Zincirleme reaksiyon için
}

// Dikey çizgili şeker tetikleme
void triggerFourStripedVertical(int row, int col) {
    int count = 0;
    for (int i = 0; i < gridSize; i++) {
        if (resources.gameBoard[i][col].baseType != -1) {
            count++;
            resources.gameBoard[i][col].baseType = -1;
            resources.gameBoard[i][col].specialType = 0;
            explodeAnims[i][col].active = true;
            explodeAnims[i][col].timer = 0.0f;
        }
    }
    addScore(count);
    removeMatches(); // Zincirleme reaksiyon için
}

void triggerWrappedPackage(int row, int col) {
    int count = 0;
    // 3x3 alanı önce özel şekerleri tespit et
    int specials[3][3] = { 0 }; // Özel şeker tiplerini tutar
    int bases[3][3] = { 0 };    // BaseType'ları tutar
    for (int di = -1; di <= 1; di++) {
        for (int dj = -1; dj <= 1; dj++) {
            int i = row + di, j = col + dj;
            if (i >= 0 && i < gridSize && j >= 0 && j < gridSize) {
                specials[di + 1][dj + 1] = resources.gameBoard[i][j].specialType;
                bases[di + 1][dj + 1] = resources.gameBoard[i][j].baseType;
            }
            else {
                specials[di + 1][dj + 1] = 0;
                bases[di + 1][dj + 1] = -1;
            }
        }
    }
    // Önce özel şekerleri tetikle
    for (int di = -1; di <= 1; di++) {
        for (int dj = -1; dj <= 1; dj++) {
            int i = row + di, j = col + dj;
            int special = specials[di + 1][dj + 1];
            int base = bases[di + 1][dj + 1];
            if (i >= 0 && i < gridSize && j >= 0 && j < gridSize && special > 0) {
                // Kendi kendini tekrar patlatmasını önlemek için önce specialType'ı sıfırla
                resources.gameBoard[i][j].specialType = 0;
                switch (special) {
                case 1: triggerFourStripedHorizontal(i, j); break;
                case 2: triggerFourStripedVertical(i, j); break;
                case 3: triggerWrappedPackage(i, j); break;
                case 4: triggerFiveColorBomb(i, j, base); break;
                }
            }
        }
    }
    // Sonra 3x3 alanı temizle
    for (int di = -1; di <= 1; di++) {
        for (int dj = -1; dj <= 1; dj++) {
            int i = row + di, j = col + dj;
            if (i >= 0 && i < gridSize && j >= 0 && j < gridSize) {
                if (resources.gameBoard[i][j].baseType != -1) {
                    count++;
                    resources.gameBoard[i][j].baseType = -1;
                    resources.gameBoard[i][j].specialType = 0;
                    explodeAnims[i][j].active = true;
                    explodeAnims[i][j].timer = 0.0f;
                }
            }
        }
    }
    addScore(count);
    removeMatches();
}

// Renk bombası tetikleme
void triggerFiveColorBomb(int row, int col, int targetBaseType) {
    int count = 0;
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            if (resources.gameBoard[i][j].baseType == targetBaseType) {
                count++;
                resources.gameBoard[i][j].baseType = -1;
                resources.gameBoard[i][j].specialType = 0;
                explodeAnims[i][j].active = true;
                explodeAnims[i][j].timer = 0.0f;
            }
        }
    }
    if (resources.gameBoard[row][col].baseType != -1) {
        count++;
        resources.gameBoard[row][col].baseType = -1;
        resources.gameBoard[row][col].specialType = 0;
        explodeAnims[row][col].active = true;
        explodeAnims[row][col].timer = 0.0f;
    }
    addScore(count);
    removeMatches(); // Zincirleme reaksiyon için
}

// Swap animasyonunu güncelleme
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
                    swapCandies(row1, col1, row2, col2, true);
                    revertSwap = false;
                }
                else {
                    PlaySound(resources.matchSound);
                    removeMatches();
                }
            }
        }
        else {
            resources.gameBoard[(int)swap1.y][(int)swap1.x].position = vector2Lerp(swapAnim.startPos, swapAnim.endPos, swapAnim.t);
            resources.gameBoard[(int)swap2.y][(int)swap2.x].position = vector2Lerp(swapAnim.endPos, swapAnim.startPos, swapAnim.t);
        }
    }
}

// Şeker seçme
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
                int t1 = resources.gameBoard[row1][col1].baseType;
                int t2 = resources.gameBoard[row2][col2].baseType;

                // Renk bombası swapı
                if ((s1 == 4 && s2 == 0) || (s2 == 4 && s1 == 0)) {
                    int bombRow, bombCol, targetColor;
                    if (s1 == 4) {
                        bombRow = row1; bombCol = col1;
                        targetColor = t2;
                    }
                    else {
                        bombRow = row2; bombCol = col2;
                        targetColor = t1;
                    }
                    triggerFiveColorBomb(bombRow, bombCol, targetColor);
                    PlaySound(resources.matchSound);
                    hasSelected = false;
                    resources.moves--;
                    return;
                }

                resources.moves--;

                // Swap geçerli mi?
                if (isValidSwap(row1, col1, row2, col2)) {
                    swapCandies(row1, col1, row2, col2, true);
                    pendingSwapCheck = true;
                    revertSwap = false;
                }
                else {
                    swapCandies(row1, col1, row2, col2, true);
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

// Şekerleri düşürme
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

// Yeni şekerler oluşturma
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



// Oyun ekranını çizme
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

    // YENİ ZİNCİR: drop/spawn sonrası zincirleme removeMatches
    if (!anyAnimations) {
        if (dropOneCandy()) {
            candiesDropping = true;
        }
        else if (spawnCandies()) {
            candiesDropping = true;
        }
        else if (candiesDropping) {
            candiesDropping = false;
            removeMatches();
        }
    }

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
            PlaySound(resources.clickButton);
            currentState = LEVELS;
        }
        else if (CheckCollisionPointRec(mouse, settingsRect)) {
            PlaySound(resources.clickButton);
            resources.showgameSettings = !resources.showgameSettings;
        }
    }

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
                PlaySound(resources.clickButton);
                resources.soundOn = !resources.soundOn;
                SetMusicVolume(resources.music, resources.soundOn ? 1.0f : 0.0f);
            }
            if (CheckCollisionPointRec(mouse, backBtn)) {
                PlaySound(resources.clickButton);
                resources.showgameSettings = false;
            }
        }
    }
}

bool checkAndTriggerSpecialCandiesInMatch(int startRow, int startCol, int endRow, int endCol) {
    bool triggered = false;
    for (int i = startRow; i <= endRow; i++) {
        for (int j = startCol; j <= endCol; j++) {
            int special = resources.gameBoard[i][j].specialType;
            if (special > 0) {
                switch (special) {
                case 1: // Yatay çizgili
                    triggerFourStripedHorizontal(i, j);
                    triggered = true;
                    break;
                case 2: // Dikey çizgili
                    triggerFourStripedVertical(i, j);
                    triggered = true;
                    break;
                case 3: // Paket
                    triggerWrappedPackage(i, j);
                    triggered = true;
                    break;
                case 4: // Renk bombası
                    triggerFiveColorBomb(i, j, resources.gameBoard[i][j].baseType);
                    triggered = true;
                    break;
                }
            }
        }
    }
    return triggered;
}

bool checkFiveMatchAndMakeColorBomb() {
    bool found = false;
    // YATAY
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize - 4; j++) {
            int t = resources.gameBoard[i][j].baseType;
            if (t >= 0 &&
                t == resources.gameBoard[i][j + 1].baseType &&
                t == resources.gameBoard[i][j + 2].baseType &&
                t == resources.gameBoard[i][j + 3].baseType &&
                t == resources.gameBoard[i][j + 4].baseType) {

                // Önce eşleşen şekerler içinde özel şeker var mı kontrol et
                bool hasSpecial = false;
                int specialRow = -1, specialCol = -1;
                for (int k = 0; k < 5; k++) {
                    if (resources.gameBoard[i][j + k].specialType > 0) {
                        hasSpecial = true;
                        specialRow = i;
                        specialCol = j + k;
                        break;
                    }
                }

                // Eğer özel şeker varsa önce onu patlat
                if (hasSpecial) {
                    switch (resources.gameBoard[specialRow][specialCol].specialType) {
                    case 1: // Yatay çizgili
                        triggerFourStripedHorizontal(specialRow, specialCol);
                        break;
                    case 2: // Dikey çizgili
                        triggerFourStripedVertical(specialRow, specialCol);
                        break;
                    case 3: // Paket
                        triggerWrappedPackage(specialRow, specialCol);
                        break;
                    case 4: // Renk bombası
                        triggerFiveColorBomb(specialRow, specialCol, t);
                        break;
                    }
                }

                int center = j + 2;
                for (int k = 0; k < 5; k++) {
                    resources.gameBoard[i][j + k].baseType = -1;
                    resources.gameBoard[i][j + k].specialType = 0;
                    explodeAnims[i][j + k].active = true;
                    explodeAnims[i][j + k].timer = 0.0f;
                }
                resources.gameBoard[i][center].baseType = t;
                resources.gameBoard[i][center].specialType = 4; // renk bombası
                found = true;
            }
        }
    }
    // DİKEY
    for (int j = 0; j < gridSize; j++) {
        for (int i = 0; i < gridSize - 4; i++) {
            int t = resources.gameBoard[i][j].baseType;
            if (t >= 0 &&
                t == resources.gameBoard[i + 1][j].baseType &&
                t == resources.gameBoard[i + 2][j].baseType &&
                t == resources.gameBoard[i + 3][j].baseType &&
                t == resources.gameBoard[i + 4][j].baseType) {

                // Önce eşleşen şekerler içinde özel şeker var mı kontrol et
                bool hasSpecial = false;
                int specialRow = -1, specialCol = -1;
                for (int k = 0; k < 5; k++) {
                    if (resources.gameBoard[i + k][j].specialType > 0) {
                        hasSpecial = true;
                        specialRow = i + k;
                        specialCol = j;
                        break;
                    }
                }

                // Eğer özel şeker varsa önce onu patlat
                if (hasSpecial) {
                    switch (resources.gameBoard[specialRow][specialCol].specialType) {
                    case 1: // Yatay çizgili
                        triggerFourStripedHorizontal(specialRow, specialCol);
                        break;
                    case 2: // Dikey çizgili
                        triggerFourStripedVertical(specialRow, specialCol);
                        break;
                    case 3: // Paket
                        triggerWrappedPackage(specialRow, specialCol);
                        break;
                    case 4: // Renk bombası
                        triggerFiveColorBomb(specialRow, specialCol, t);
                        break;
                    }
                }

                int center = i + 2;
                for (int k = 0; k < 5; k++) {
                    resources.gameBoard[i + k][j].baseType = -1;
                    resources.gameBoard[i + k][j].specialType = 0;
                    explodeAnims[i + k][j].active = true;
                    explodeAnims[i + k][j].timer = 0.0f;
                }
                resources.gameBoard[center][j].baseType = t;
                resources.gameBoard[center][j].specialType = 4; // renk bombası
                found = true;
            }
        }
    }
    return found;
}

bool checkWrappedMatchAndMakePackage() {
    bool found = false;
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            int t = resources.gameBoard[i][j].baseType;
            if (t < 0) continue;

            // T şekli kontrolü
            if (i >= 1 && i < gridSize - 1 && j >= 1 && j < gridSize - 1 &&
                t == resources.gameBoard[i][j - 1].baseType &&
                t == resources.gameBoard[i][j + 1].baseType &&
                t == resources.gameBoard[i - 1][j].baseType &&
                t == resources.gameBoard[i + 1][j].baseType) {

                // Önce eşleşen şekerler içinde özel şeker var mı kontrol et
                bool hasSpecial = false;
                for (int k = -1; k <= 1; k++) {
                    for (int l = -1; l <= 1; l++) {
                        if (k == 0 && l == 0) continue;
                        if (resources.gameBoard[i + k][j + l].specialType > 0) {
                            hasSpecial = true;
                            triggerSpecialCandy(i + k, j + l, resources.gameBoard[i + k][j + l].specialType, t);
                        }
                    }
                }

                // Eğer özel şeker yoksa veya patlatıldıysa paket şeker oluştur
                resources.gameBoard[i][j - 1].baseType = -1;
                resources.gameBoard[i][j + 1].baseType = -1;
                resources.gameBoard[i - 1][j].baseType = -1;
                resources.gameBoard[i + 1][j].baseType = -1;
                resources.gameBoard[i][j - 1].specialType = 0;
                resources.gameBoard[i][j + 1].specialType = 0;
                resources.gameBoard[i - 1][j].specialType = 0;
                resources.gameBoard[i + 1][j].specialType = 0;
                resources.gameBoard[i][j].baseType = t;
                resources.gameBoard[i][j].specialType = 3; // Paket şeker
                found = true;
            }

            // L şekli kontrolü (sol üst köşe)
            else if (i < gridSize - 2 && j < gridSize - 2 &&
                t == resources.gameBoard[i + 1][j].baseType &&
                t == resources.gameBoard[i + 2][j].baseType &&
                t == resources.gameBoard[i][j + 1].baseType &&
                t == resources.gameBoard[i][j + 2].baseType) {

                // Önce eşleşen şekerler içinde özel şeker var mı kontrol et
                bool hasSpecial = false;
                if (resources.gameBoard[i + 1][j].specialType > 0) {
                    hasSpecial = true;
                    triggerSpecialCandy(i + 1, j, resources.gameBoard[i + 1][j].specialType, t);
                }
                if (resources.gameBoard[i + 2][j].specialType > 0) {
                    hasSpecial = true;
                    triggerSpecialCandy(i + 2, j, resources.gameBoard[i + 2][j].specialType, t);
                }
                if (resources.gameBoard[i][j + 1].specialType > 0) {
                    hasSpecial = true;
                    triggerSpecialCandy(i, j + 1, resources.gameBoard[i][j + 1].specialType, t);
                }
                if (resources.gameBoard[i][j + 2].specialType > 0) {
                    hasSpecial = true;
                    triggerSpecialCandy(i, j + 2, resources.gameBoard[i][j + 2].specialType, t);
                }

                // Eğer özel şeker yoksa veya patlatıldıysa paket şeker oluştur
                resources.gameBoard[i + 1][j].baseType = -1;
                resources.gameBoard[i + 2][j].baseType = -1;
                resources.gameBoard[i][j + 1].baseType = -1;
                resources.gameBoard[i][j + 2].baseType = -1;
                resources.gameBoard[i + 1][j].specialType = 0;
                resources.gameBoard[i + 2][j].specialType = 0;
                resources.gameBoard[i][j + 1].specialType = 0;
                resources.gameBoard[i][j + 2].specialType = 0;
                resources.gameBoard[i][j].baseType = t;
                resources.gameBoard[i][j].specialType = 3; // Paket şeker
                found = true;
            }

            // Diğer L şekli varyasyonları için de aynı mantık uygulanacak
            // L şekli (sağ üst köşe)
            else if (i < gridSize - 2 && j >= 2 &&
                t == resources.gameBoard[i + 1][j].baseType &&
                t == resources.gameBoard[i + 2][j].baseType &&
                t == resources.gameBoard[i][j - 1].baseType &&
                t == resources.gameBoard[i][j - 2].baseType) {

                resources.gameBoard[i + 1][j].baseType = -1;
                resources.gameBoard[i + 2][j].baseType = -1;
                resources.gameBoard[i][j - 1].baseType = -1;
                resources.gameBoard[i][j - 2].baseType = -1;
                resources.gameBoard[i + 1][j].specialType = 0;
                resources.gameBoard[i + 2][j].specialType = 0;
                resources.gameBoard[i][j - 1].specialType = 0;
                resources.gameBoard[i][j - 2].specialType = 0;
                resources.gameBoard[i][j].baseType = t;
                resources.gameBoard[i][j].specialType = 3; // Paket şeker
                found = true;
            }
            // L şekli (sol alt köşe)
            else if (i >= 2 && j < gridSize - 2 &&
                t == resources.gameBoard[i - 1][j].baseType &&
                t == resources.gameBoard[i - 2][j].baseType &&
                t == resources.gameBoard[i][j + 1].baseType &&
                t == resources.gameBoard[i][j + 2].baseType) {

                resources.gameBoard[i - 1][j].baseType = -1;
                resources.gameBoard[i - 2][j].baseType = -1;
                resources.gameBoard[i][j + 1].baseType = -1;
                resources.gameBoard[i][j + 2].baseType = -1;
                resources.gameBoard[i - 1][j].specialType = 0;
                resources.gameBoard[i - 2][j].specialType = 0;
                resources.gameBoard[i][j + 1].specialType = 0;
                resources.gameBoard[i][j + 2].specialType = 0;
                resources.gameBoard[i][j].baseType = t;
                resources.gameBoard[i][j].specialType = 3; // Paket şeker
                found = true;
            }
            // L şekli (sağ alt köşe)
            else if (i >= 2 && j >= 2 &&
                t == resources.gameBoard[i - 1][j].baseType &&
                t == resources.gameBoard[i - 2][j].baseType &&
                t == resources.gameBoard[i][j - 1].baseType &&
                t == resources.gameBoard[i][j - 2].baseType) {

                resources.gameBoard[i - 1][j].baseType = -1;
                resources.gameBoard[i - 2][j].baseType = -1;
                resources.gameBoard[i][j - 1].baseType = -1;
                resources.gameBoard[i][j - 2].baseType = -1;
                resources.gameBoard[i - 1][j].specialType = 0;
                resources.gameBoard[i - 2][j].specialType = 0;
                resources.gameBoard[i][j - 1].specialType = 0;
                resources.gameBoard[i][j - 2].specialType = 0;
                resources.gameBoard[i][j].baseType = t;
                resources.gameBoard[i][j].specialType = 3; // Paket şeker
                found = true;
            }
        }
    }
    return found;
}

bool checkFourMatchAndMakeStriped() {
    bool found = false;
    // YATAY
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize - 3; j++) {
            int t = resources.gameBoard[i][j].baseType;
            if (t >= 0 &&
                t == resources.gameBoard[i][j + 1].baseType &&
                t == resources.gameBoard[i][j + 2].baseType &&
                t == resources.gameBoard[i][j + 3].baseType) {

                // Önce eşleşen şekerler içinde özel şeker var mı kontrol et
                bool hasSpecial = false;
                int specialRow = -1, specialCol = -1;
                for (int k = 0; k < 4; k++) {
                    if (resources.gameBoard[i][j + k].specialType > 0) {
                        hasSpecial = true;
                        specialRow = i;
                        specialCol = j + k;
                        break;
                    }
                }

                // Eğer özel şeker varsa önce onu patlat
                if (hasSpecial) {
                    switch (resources.gameBoard[specialRow][specialCol].specialType) {
                    case 1: // Yatay çizgili
                        triggerFourStripedHorizontal(specialRow, specialCol);
                        break;
                    case 2: // Dikey çizgili
                        triggerFourStripedVertical(specialRow, specialCol);
                        break;
                    case 3: // Paket
                        triggerWrappedPackage(specialRow, specialCol);
                        break;
                    case 4: // Renk bombası
                        triggerFiveColorBomb(specialRow, specialCol, t);
                        break;
                    }
                }

                // Oynanan hamlenin yapıldığı yeri bul
                int specialIndex = -1;
                if (j == (int)swap1.x || j == (int)swap2.x) {
                    specialIndex = j;
                }
                else if (j + 1 == (int)swap1.x || j + 1 == (int)swap2.x) {
                    specialIndex = j + 1;
                }
                else if (j + 2 == (int)swap1.x || j + 2 == (int)swap2.x) {
                    specialIndex = j + 2;
                }
                else if (j + 3 == (int)swap1.x || j + 3 == (int)swap2.x) {
                    specialIndex = j + 3;
                }

                // Eğer oynanan hamle bulunamazsa ortadaki şekeri seç
                if (specialIndex == -1) {
                    specialIndex = j + 1;
                }

                for (int k = 0; k < 4; k++) {
                    int col = j + k;
                    if (col == specialIndex) {
                        resources.gameBoard[i][col].baseType = t;
                        resources.gameBoard[i][col].specialType = 1; // Yatay çizgili
                    }
                    else {
                        resources.gameBoard[i][col].baseType = -1;
                        resources.gameBoard[i][col].specialType = 0;
                        explodeAnims[i][col].active = true;
                        explodeAnims[i][col].timer = 0.0f;
                    }
                }
                found = true;
            }
        }
    }

    // DİKEY
    for (int j = 0; j < gridSize; j++) {
        for (int i = 0; i < gridSize - 3; i++) {
            int t = resources.gameBoard[i][j].baseType;
            if (t >= 0 &&
                t == resources.gameBoard[i + 1][j].baseType &&
                t == resources.gameBoard[i + 2][j].baseType &&
                t == resources.gameBoard[i + 3][j].baseType) {

                // Önce eşleşen şekerler içinde özel şeker var mı kontrol et
                bool hasSpecial = false;
                int specialRow = -1, specialCol = -1;
                for (int k = 0; k < 4; k++) {
                    if (resources.gameBoard[i + k][j].specialType > 0) {
                        hasSpecial = true;
                        specialRow = i + k;
                        specialCol = j;
                        break;
                    }
                }

                // Eğer özel şeker varsa önce onu patlat
                if (hasSpecial) {
                    switch (resources.gameBoard[specialRow][specialCol].specialType) {
                    case 1: // Yatay çizgili
                        triggerFourStripedHorizontal(specialRow, specialCol);
                        break;
                    case 2: // Dikey çizgili
                        triggerFourStripedVertical(specialRow, specialCol);
                        break;
                    case 3: // Paket
                        triggerWrappedPackage(specialRow, specialCol);
                        break;
                    case 4: // Renk bombası
                        triggerFiveColorBomb(specialRow, specialCol, t);
                        break;
                    }
                }

                // Oynanan hamlenin yapıldığı yeri bul
                int specialIndex = -1;
                if (i == (int)swap1.y || i == (int)swap2.y) {
                    specialIndex = i;
                }
                else if (i + 1 == (int)swap1.y || i + 1 == (int)swap2.y) {
                    specialIndex = i + 1;
                }
                else if (i + 2 == (int)swap1.y || i + 2 == (int)swap2.y) {
                    specialIndex = i + 2;
                }
                else if (i + 3 == (int)swap1.y || i + 3 == (int)swap2.y) {
                    specialIndex = i + 3;
                }

                // Eğer oynanan hamle bulunamazsa ortadaki şekeri seç
                if (specialIndex == -1) {
                    specialIndex = i + 1;
                }

                for (int k = 0; k < 4; k++) {
                    int row = i + k;
                    if (row == specialIndex) {
                        resources.gameBoard[row][j].baseType = t;
                        resources.gameBoard[row][j].specialType = 2; // Dikey çizgili
                    }
                    else {
                        resources.gameBoard[row][j].baseType = -1;
                        resources.gameBoard[row][j].specialType = 0;
                        explodeAnims[row][j].active = true;
                        explodeAnims[row][j].timer = 0.0f;
                    }
                }
                found = true;
            }
        }
    }
    return found;
}
