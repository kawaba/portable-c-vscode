/**
 * PNG画像サイズ取得サンプル
 * stb_image を使用して画像のサイズを取得します
 * 
 * ビルド: gcc -o png_app.exe main.c -I../../include -lm
 */

// stb_image の実装を有効化（1つの .c ファイルでのみ定義）
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// テスト用の画像を生成する関数
void create_test_image(const char* filename, int width, int height) {
    // RGBA形式のピクセルデータを作成
    unsigned char* pixels = (unsigned char*)malloc(width * height * 4);
    
    if (!pixels) {
        printf("エラー: メモリ確保失敗\n");
        return;
    }

    // グラデーション画像を生成
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = (y * width + x) * 4;
            
            // 赤: 左から右へグラデーション
            pixels[idx + 0] = (unsigned char)(x * 255 / width);
            // 緑: 上から下へグラデーション
            pixels[idx + 1] = (unsigned char)(y * 255 / height);
            // 青: 対角線グラデーション
            pixels[idx + 2] = (unsigned char)((x + y) * 255 / (width + height));
            // アルファ: 不透明
            pixels[idx + 3] = 255;
        }
    }

    // PNG形式で保存
    if (stbi_write_png(filename, width, height, 4, pixels, width * 4)) {
        printf("テスト画像を生成しました: %s (%dx%d)\n", filename, width, height);
    } else {
        printf("エラー: 画像の保存に失敗しました\n");
    }

    free(pixels);
}

// 画像情報を表示する関数
void show_image_info(const char* filename) {
    int width, height, channels;
    
    printf("\n--- %s ---\n", filename);

    // 画像のメタデータを取得（ピクセルデータは読み込まない）
    if (stbi_info(filename, &width, &height, &channels)) {
        printf("幅:         %d px\n", width);
        printf("高さ:       %d px\n", height);
        printf("チャンネル: %d\n", channels);
        
        // チャンネル数の説明
        printf("形式:       ");
        switch (channels) {
            case 1: printf("グレースケール\n"); break;
            case 2: printf("グレースケール + アルファ\n"); break;
            case 3: printf("RGB\n"); break;
            case 4: printf("RGBA\n"); break;
            default: printf("不明\n"); break;
        }
        
        // メモリ使用量の計算
        long pixels = (long)width * height;
        long memory = pixels * channels;
        printf("総ピクセル: %ld\n", pixels);
        printf("メモリ:     %.2f KB (%.2f MB)\n", 
               memory / 1024.0, memory / 1024.0 / 1024.0);
        
    } else {
        printf("エラー: %s を読み込めませんでした\n", filename);
        printf("理由: %s\n", stbi_failure_reason());
    }
}

int main(int argc, char* argv[]) {
    printf("=== PNG画像処理サンプル (stb_image) ===\n\n");

    // コマンドライン引数がある場合はそのファイルを処理
    if (argc >= 2) {
        for (int i = 1; i < argc; i++) {
            show_image_info(argv[i]);
        }
        return 0;
    }

    // テスト画像を生成
    printf("テスト画像を生成中...\n");
    create_test_image("test_small.png", 100, 100);
    create_test_image("test_medium.png", 640, 480);
    create_test_image("test_large.png", 1920, 1080);

    // 生成した画像の情報を表示
    printf("\n=== 画像情報 ===");
    show_image_info("test_small.png");
    show_image_info("test_medium.png");
    show_image_info("test_large.png");

    printf("\n=== 完了 ===\n");
    printf("使い方: png_app.exe <画像ファイル> [画像ファイル2] ...\n");
    
    return 0;
}
