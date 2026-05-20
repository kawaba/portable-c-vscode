/**
 * 図形描画サンプル
 * raylib を使用して基本的な図形を描画します
 * 
 * ビルド: gcc -o graphics_app.exe main.c -I../../include -L../../lib -lraylib -lopengl32 -lgdi32 -lwinmm
 * 
 * 操作:
 *   左クリック: 円を移動
 *   マウスホイール: 円のサイズ変更
 *   スペース: 色をランダム変更
 *   ESC: 終了
 */

#include "raylib.h"
#include <stdlib.h>
#include <time.h>

// ランダムな色を生成
Color RandomColor(void) {
    return (Color){
        GetRandomValue(50, 255),
        GetRandomValue(50, 255),
        GetRandomValue(50, 255),
        255
    };
}

int main(void) {
    // 乱数初期化
    srand(time(NULL));
    SetRandomSeed(time(NULL));

    // ウィンドウの初期化
    const int screenWidth = 800;
    const int screenHeight = 600;
    
    InitWindow(screenWidth, screenHeight, "図形描画サンプル - raylib");
    SetTargetFPS(60);

    // 図形のパラメータ
    Vector2 circlePos = { screenWidth / 2.0f, screenHeight / 2.0f };
    float circleRadius = 60.0f;
    Color circleColor = RED;
    
    Color rectColor = BLUE;
    Color triangleColor = GREEN;

    // メインループ
    while (!WindowShouldClose()) {
        // === 更新処理 ===
        
        // マウス位置に円を移動
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            circlePos = GetMousePosition();
        }

        // マウスホイールで円のサイズ変更
        circleRadius += GetMouseWheelMove() * 10.0f;
        if (circleRadius < 20.0f) circleRadius = 20.0f;
        if (circleRadius > 200.0f) circleRadius = 200.0f;

        // スペースキーで色を変更
        if (IsKeyPressed(KEY_SPACE)) {
            circleColor = RandomColor();
            rectColor = RandomColor();
            triangleColor = RandomColor();
        }

        // === 描画処理 ===
        BeginDrawing();
        
        ClearBackground(RAYWHITE);

        // 背景のグリッド
        for (int i = 0; i < screenWidth; i += 50) {
            DrawLine(i, 0, i, screenHeight, LIGHTGRAY);
        }
        for (int i = 0; i < screenHeight; i += 50) {
            DrawLine(0, i, screenWidth, i, LIGHTGRAY);
        }

        // 四角形（塗りつぶし + 枠線）
        DrawRectangle(50, 50, 180, 120, rectColor);
        DrawRectangleLines(50, 50, 180, 120, Fade(BLACK, 0.5f));
        DrawText("Rectangle", 80, 100, 20, WHITE);

        // 角丸四角形
        DrawRectangleRounded(
            (Rectangle){ 50, 200, 180, 100 },
            0.3f,  // 角の丸み
            8,     // セグメント数
            Fade(rectColor, 0.7f)
        );
        DrawText("Rounded", 90, 240, 20, WHITE);

        // 円（マウスで移動可能）
        DrawCircleV(circlePos, circleRadius, Fade(circleColor, 0.7f));
        DrawCircleLines((int)circlePos.x, (int)circlePos.y, circleRadius, Fade(BLACK, 0.5f));
        DrawText("Circle", (int)circlePos.x - 25, (int)circlePos.y - 8, 16, WHITE);

        // 三角形
        Vector2 v1 = { 650, 80 };
        Vector2 v2 = { 570, 200 };
        Vector2 v3 = { 730, 200 };
        DrawTriangle(v1, v2, v3, triangleColor);
        DrawTriangleLines(v1, v2, v3, Fade(BLACK, 0.5f));
        DrawText("Triangle", 610, 130, 18, WHITE);

        // 線（いろいろな種類）
        DrawLineEx((Vector2){ 280, 80 }, (Vector2){ 480, 80 }, 3.0f, PURPLE);
        DrawText("Line", 360, 60, 16, PURPLE);

        DrawLineBezier(
            (Vector2){ 280, 120 },
            (Vector2){ 480, 180 },
            3.0f,
            ORANGE
        );
        DrawText("Bezier", 360, 130, 16, ORANGE);

        // 多角形（六角形）
        DrawPoly(
            (Vector2){ 650, 380 },  // 中心
            6,                       // 頂点数
            70,                      // 半径
            30,                      // 回転角度
            Fade(PURPLE, 0.7f)
        );
        DrawText("Polygon", 615, 370, 18, WHITE);

        // グラデーション四角形
        DrawRectangleGradientH(50, 350, 180, 100, BLUE, SKYBLUE);
        DrawText("Gradient", 90, 390, 18, WHITE);

        // リング
        DrawRing(
            (Vector2){ 400, 450 },  // 中心
            50,                      // 内側半径
            80,                      // 外側半径
            0,                       // 開始角度
            270,                     // 終了角度
            36,                      // セグメント数
            Fade(MAROON, 0.7f)
        );
        DrawText("Ring", 375, 440, 18, WHITE);

        // 操作説明
        DrawRectangle(0, screenHeight - 70, screenWidth, 70, Fade(BLACK, 0.8f));
        DrawText("操作方法:", 20, screenHeight - 60, 18, WHITE);
        DrawText("左クリック: 円を移動 | ホイール: 円のサイズ | スペース: 色変更 | ESC: 終了", 
                 20, screenHeight - 35, 16, LIGHTGRAY);

        // FPS表示
        DrawFPS(screenWidth - 100, 10);

        EndDrawing();
    }

    // リソースの解放
    CloseWindow();

    return 0;
}
