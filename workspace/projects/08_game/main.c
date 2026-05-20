/**
 * シューティングゲーム サンプル
 * raylib を使用した簡単なシューティングゲーム
 * 
 * ビルド: gcc -o game.exe main.c -I../../include -L../../lib -lraylib -lopengl32 -lgdi32 -lwinmm
 * 
 * 操作:
 *   矢印キー / WASD: 移動
 *   スペース: 弾を発射
 *   P: ポーズ
 *   ESC: 終了
 */

#include "raylib.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>

// === 定数定義 ===
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600
#define MAX_BULLETS   100
#define MAX_ENEMIES   30
#define MAX_PARTICLES 200
#define MAX_STARS     100

#define PLAYER_SPEED  6.0f
#define BULLET_SPEED  10.0f
#define ENEMY_SPEED_MIN 1.5f
#define ENEMY_SPEED_MAX 4.0f

// === 構造体定義 ===
typedef struct {
    Vector2 position;
    Vector2 size;
    int lives;
    int score;
    float shootCooldown;
    bool invincible;
    float invincibleTimer;
} Player;

typedef struct {
    Vector2 position;
    float speed;
    bool active;
} Bullet;

typedef struct {
    Vector2 position;
    Vector2 size;
    float speed;
    int type;  // 0: 通常, 1: 高速, 2: 大型
    bool active;
} Enemy;

typedef struct {
    Vector2 position;
    Vector2 velocity;
    Color color;
    float life;
    bool active;
} Particle;

typedef struct {
    Vector2 position;
    float speed;
    float brightness;
} Star;

// === グローバル変数 ===
Player player;
Bullet bullets[MAX_BULLETS];
Enemy enemies[MAX_ENEMIES];
Particle particles[MAX_PARTICLES];
Star stars[MAX_STARS];

float enemySpawnTimer = 0.0f;
float spawnInterval = 1.5f;
bool isPaused = false;
bool isGameOver = false;
int highScore = 0;

// === 関数プロトタイプ ===
void InitGame(void);
void UpdateGame(void);
void DrawGame(void);
void SpawnEnemy(void);
void FireBullet(void);
void SpawnParticles(Vector2 pos, Color color, int count);
void UpdateParticles(void);
void DrawParticles(void);
void InitStars(void);
void UpdateStars(void);
void DrawStars(void);
bool CheckCollision(Vector2 pos1, Vector2 size1, Vector2 pos2, Vector2 size2);

// === メイン関数 ===
int main(void) {
    srand(time(NULL));
    SetRandomSeed(time(NULL));
    
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "シューティングゲーム - C言語講習");
    SetTargetFPS(60);

    InitGame();
    InitStars();

    while (!WindowShouldClose()) {
        // ポーズ切り替え
        if (IsKeyPressed(KEY_P) && !isGameOver) {
            isPaused = !isPaused;
        }

        if (!isPaused) {
            UpdateGame();
        }
        
        DrawGame();
    }

    CloseWindow();
    return 0;
}

// === ゲーム初期化 ===
void InitGame(void) {
    player.position = (Vector2){ SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT - 80.0f };
    player.size = (Vector2){ 50.0f, 40.0f };
    player.lives = 3;
    player.score = 0;
    player.shootCooldown = 0.0f;
    player.invincible = false;
    player.invincibleTimer = 0.0f;

    for (int i = 0; i < MAX_BULLETS; i++) bullets[i].active = false;
    for (int i = 0; i < MAX_ENEMIES; i++) enemies[i].active = false;
    for (int i = 0; i < MAX_PARTICLES; i++) particles[i].active = false;

    enemySpawnTimer = 0.0f;
    spawnInterval = 1.5f;
    isGameOver = false;
}

// === 星の初期化 ===
void InitStars(void) {
    for (int i = 0; i < MAX_STARS; i++) {
        stars[i].position = (Vector2){ 
            GetRandomValue(0, SCREEN_WIDTH), 
            GetRandomValue(0, SCREEN_HEIGHT) 
        };
        stars[i].speed = GetRandomValue(1, 3) * 0.5f;
        stars[i].brightness = GetRandomValue(50, 255) / 255.0f;
    }
}

// === 星の更新 ===
void UpdateStars(void) {
    for (int i = 0; i < MAX_STARS; i++) {
        stars[i].position.y += stars[i].speed;
        if (stars[i].position.y > SCREEN_HEIGHT) {
            stars[i].position.y = 0;
            stars[i].position.x = GetRandomValue(0, SCREEN_WIDTH);
        }
    }
}

// === 星の描画 ===
void DrawStars(void) {
    for (int i = 0; i < MAX_STARS; i++) {
        unsigned char b = (unsigned char)(stars[i].brightness * 255);
        DrawPixel((int)stars[i].position.x, (int)stars[i].position.y, 
                  (Color){b, b, b, 255});
    }
}

// === ゲーム更新 ===
void UpdateGame(void) {
    float dt = GetFrameTime();

    // ゲームオーバーチェック
    if (player.lives <= 0) {
        isGameOver = true;
        if (player.score > highScore) highScore = player.score;
        
        if (IsKeyPressed(KEY_ENTER)) {
            InitGame();
        }
        return;
    }

    // 星の更新
    UpdateStars();

    // 無敵時間の更新
    if (player.invincible) {
        player.invincibleTimer -= dt;
        if (player.invincibleTimer <= 0) {
            player.invincible = false;
        }
    }

    // プレイヤー移動
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) player.position.x -= PLAYER_SPEED;
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) player.position.x += PLAYER_SPEED;
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) player.position.y -= PLAYER_SPEED;
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) player.position.y += PLAYER_SPEED;

    // 画面制限
    if (player.position.x < 0) player.position.x = 0;
    if (player.position.x > SCREEN_WIDTH - player.size.x) 
        player.position.x = SCREEN_WIDTH - player.size.x;
    if (player.position.y < 0) player.position.y = 0;
    if (player.position.y > SCREEN_HEIGHT - player.size.y) 
        player.position.y = SCREEN_HEIGHT - player.size.y;

    // 射撃クールダウン
    player.shootCooldown -= dt;
    if (player.shootCooldown < 0) player.shootCooldown = 0;

    // 弾の発射
    if (IsKeyDown(KEY_SPACE) && player.shootCooldown <= 0) {
        FireBullet();
        player.shootCooldown = 0.15f;
    }

    // 弾の更新
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            bullets[i].position.y -= bullets[i].speed;
            if (bullets[i].position.y < -10) bullets[i].active = false;
        }
    }

    // 敵の出現
    enemySpawnTimer += dt;
    if (enemySpawnTimer > spawnInterval) {
        SpawnEnemy();
        enemySpawnTimer = 0.0f;
        // 難易度上昇
        if (spawnInterval > 0.5f) spawnInterval -= 0.01f;
    }

    // 敵の更新と当たり判定
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) continue;

        enemies[i].position.y += enemies[i].speed;

        // 画面外
        if (enemies[i].position.y > SCREEN_HEIGHT) {
            enemies[i].active = false;
            continue;
        }

        // プレイヤーとの衝突
        if (!player.invincible && 
            CheckCollision(player.position, player.size, 
                          enemies[i].position, enemies[i].size)) {
            enemies[i].active = false;
            player.lives--;
            player.invincible = true;
            player.invincibleTimer = 2.0f;
            SpawnParticles(player.position, RED, 20);
        }

        // 弾との衝突
        for (int j = 0; j < MAX_BULLETS; j++) {
            if (!bullets[j].active) continue;
            
            Vector2 bulletSize = { 6.0f, 12.0f };
            if (CheckCollision(bullets[j].position, bulletSize,
                              enemies[i].position, enemies[i].size)) {
                enemies[i].active = false;
                bullets[j].active = false;
                
                // スコア（敵タイプで変動）
                int points = (enemies[i].type == 2) ? 300 : 
                            (enemies[i].type == 1) ? 150 : 100;
                player.score += points;
                
                SpawnParticles(enemies[i].position, ORANGE, 15);
                break;
            }
        }
    }

    // パーティクル更新
    UpdateParticles();
}

// === 描画 ===
void DrawGame(void) {
    BeginDrawing();
    ClearBackground((Color){10, 10, 30, 255});

    // 星
    DrawStars();

    // ゲームオーバー画面
    if (isGameOver) {
        DrawText("GAME OVER", SCREEN_WIDTH/2 - 120, SCREEN_HEIGHT/2 - 50, 50, RED);
        DrawText(TextFormat("SCORE: %d", player.score), 
                SCREEN_WIDTH/2 - 80, SCREEN_HEIGHT/2 + 20, 30, WHITE);
        DrawText(TextFormat("HIGH SCORE: %d", highScore), 
                SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 + 60, 24, GOLD);
        DrawText("Press ENTER to restart", 
                SCREEN_WIDTH/2 - 120, SCREEN_HEIGHT/2 + 110, 20, GRAY);
        EndDrawing();
        return;
    }

    // パーティクル
    DrawParticles();

    // プレイヤー（三角形の宇宙船）
    if (!player.invincible || (int)(GetTime() * 10) % 2 == 0) {
        Vector2 p1 = { player.position.x + player.size.x/2, player.position.y };
        Vector2 p2 = { player.position.x, player.position.y + player.size.y };
        Vector2 p3 = { player.position.x + player.size.x, player.position.y + player.size.y };
        
        DrawTriangle(p1, p2, p3, SKYBLUE);
        DrawTriangleLines(p1, p2, p3, WHITE);
        
        // エンジン炎
        DrawTriangle(
            (Vector2){ player.position.x + player.size.x/2, player.position.y + player.size.y },
            (Vector2){ player.position.x + player.size.x/2 - 8, player.position.y + player.size.y + 15 },
            (Vector2){ player.position.x + player.size.x/2 + 8, player.position.y + player.size.y + 15 },
            Fade(ORANGE, 0.7f + 0.3f * sinf(GetTime() * 20))
        );
    }

    // 弾
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            DrawRectangle((int)bullets[i].position.x, (int)bullets[i].position.y, 
                         6, 12, YELLOW);
            DrawRectangle((int)bullets[i].position.x + 1, (int)bullets[i].position.y + 1, 
                         4, 10, WHITE);
        }
    }

    // 敵
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) continue;
        
        Color enemyColor;
        switch (enemies[i].type) {
            case 1: enemyColor = ORANGE; break;
            case 2: enemyColor = PURPLE; break;
            default: enemyColor = RED; break;
        }
        
        DrawRectangle((int)enemies[i].position.x, (int)enemies[i].position.y,
                     (int)enemies[i].size.x, (int)enemies[i].size.y, enemyColor);
        DrawRectangleLines((int)enemies[i].position.x, (int)enemies[i].position.y,
                          (int)enemies[i].size.x, (int)enemies[i].size.y, 
                          Fade(WHITE, 0.5f));
    }

    // UI
    DrawText(TextFormat("SCORE: %d", player.score), 20, 20, 24, WHITE);
    DrawText(TextFormat("HIGH: %d", highScore), 20, 50, 18, GOLD);
    
    // ライフ表示
    for (int i = 0; i < player.lives; i++) {
        DrawTriangle(
            (Vector2){ SCREEN_WIDTH - 40 - i * 30, 25 },
            (Vector2){ SCREEN_WIDTH - 50 - i * 30, 40 },
            (Vector2){ SCREEN_WIDTH - 30 - i * 30, 40 },
            SKYBLUE
        );
    }

    // 操作説明
    DrawText("WASD/Arrow: Move | SPACE: Fire | P: Pause | ESC: Quit", 
             10, SCREEN_HEIGHT - 25, 14, GRAY);

    // ポーズ表示
    if (isPaused) {
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.5f));
        DrawText("PAUSED", SCREEN_WIDTH/2 - 80, SCREEN_HEIGHT/2 - 20, 50, WHITE);
        DrawText("Press P to continue", SCREEN_WIDTH/2 - 90, SCREEN_HEIGHT/2 + 40, 20, GRAY);
    }

    DrawFPS(SCREEN_WIDTH - 100, 10);
    EndDrawing();
}

// === 敵の出現 ===
void SpawnEnemy(void) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) continue;
        
        enemies[i].type = GetRandomValue(0, 10) < 7 ? 0 : 
                         (GetRandomValue(0, 10) < 7 ? 1 : 2);
        
        switch (enemies[i].type) {
            case 1:  // 高速
                enemies[i].size = (Vector2){ 25.0f, 25.0f };
                enemies[i].speed = ENEMY_SPEED_MAX;
                break;
            case 2:  // 大型
                enemies[i].size = (Vector2){ 50.0f, 50.0f };
                enemies[i].speed = ENEMY_SPEED_MIN;
                break;
            default:  // 通常
                enemies[i].size = (Vector2){ 35.0f, 35.0f };
                enemies[i].speed = ENEMY_SPEED_MIN + 
                    GetRandomValue(0, 100) / 100.0f * (ENEMY_SPEED_MAX - ENEMY_SPEED_MIN);
        }
        
        enemies[i].position.x = GetRandomValue(0, SCREEN_WIDTH - (int)enemies[i].size.x);
        enemies[i].position.y = -enemies[i].size.y;
        enemies[i].active = true;
        break;
    }
}

// === 弾の発射 ===
void FireBullet(void) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) continue;
        
        bullets[i].position.x = player.position.x + player.size.x/2 - 3;
        bullets[i].position.y = player.position.y - 5;
        bullets[i].speed = BULLET_SPEED;
        bullets[i].active = true;
        break;
    }
}

// === パーティクル生成 ===
void SpawnParticles(Vector2 pos, Color color, int count) {
    for (int i = 0; i < MAX_PARTICLES && count > 0; i++) {
        if (particles[i].active) continue;
        
        float angle = GetRandomValue(0, 360) * DEG2RAD;
        float speed = GetRandomValue(50, 200) / 100.0f;
        
        particles[i].position = pos;
        particles[i].velocity = (Vector2){ cosf(angle) * speed, sinf(angle) * speed };
        particles[i].color = color;
        particles[i].life = 1.0f;
        particles[i].active = true;
        count--;
    }
}

// === パーティクル更新 ===
void UpdateParticles(void) {
    float dt = GetFrameTime();
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (!particles[i].active) continue;
        
        particles[i].position.x += particles[i].velocity.x;
        particles[i].position.y += particles[i].velocity.y;
        particles[i].life -= dt * 2;
        
        if (particles[i].life <= 0) particles[i].active = false;
    }
}

// === パーティクル描画 ===
void DrawParticles(void) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (!particles[i].active) continue;
        
        Color c = particles[i].color;
        c.a = (unsigned char)(particles[i].life * 255);
        DrawCircleV(particles[i].position, 3, c);
    }
}

// === 矩形の当たり判定 ===
bool CheckCollision(Vector2 pos1, Vector2 size1, Vector2 pos2, Vector2 size2) {
    return (pos1.x < pos2.x + size2.x &&
            pos1.x + size1.x > pos2.x &&
            pos1.y < pos2.y + size2.y &&
            pos1.y + size1.y > pos2.y);
}
