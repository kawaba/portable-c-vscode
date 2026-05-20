/**
 * 圧縮・展開サンプル
 * miniz を使用して文字列を圧縮・展開します
 * 
 * ビルド: gcc -o compress_app.exe main.c -I../../include
 */

#define MINIZ_IMPLEMENTATION
#include "miniz.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// 圧縮率を計算する関数
float calc_ratio(size_t original, size_t compressed) {
    return (1.0f - (float)compressed / original) * 100.0f;
}

int main(void) {
    printf("=== 圧縮・展開サンプル (miniz) ===\n\n");

    // 圧縮するテキスト（繰り返しが多いと圧縮率が高い）
    const char* original_text = 
        "Hello, World! This is a compression test. "
        "The quick brown fox jumps over the lazy dog. "
        "The quick brown fox jumps over the lazy dog. "
        "The quick brown fox jumps over the lazy dog. "
        "Compression works best with repetitive data. "
        "Compression works best with repetitive data. "
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA "
        "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB "
        "1234567890123456789012345678901234567890 "
        "End of test data.";

    size_t original_len = strlen(original_text) + 1;  // NULL終端を含む
    
    printf("元のテキスト:\n%s\n\n", original_text);
    printf("元のサイズ: %zu bytes\n\n", original_len);

    // ============================================
    // 圧縮
    // ============================================
    printf("--- 圧縮処理 ---\n");

    // 圧縮後の最大サイズを取得
    mz_ulong compressed_len = compressBound(original_len);
    unsigned char* compressed_data = (unsigned char*)malloc(compressed_len);

    if (!compressed_data) {
        printf("エラー: メモリ確保失敗\n");
        return 1;
    }

    // 圧縮実行
    int status = compress(
        compressed_data,
        &compressed_len,
        (const unsigned char*)original_text,
        original_len
    );

    if (status != Z_OK) {
        printf("エラー: 圧縮失敗 (status=%d)\n", status);
        free(compressed_data);
        return 1;
    }

    printf("圧縮後サイズ: %lu bytes\n", compressed_len);
    printf("圧縮率: %.1f%%\n\n", calc_ratio(original_len, compressed_len));

    // 圧縮データの先頭数バイトを表示
    printf("圧縮データ（先頭16バイト）: ");
    for (int i = 0; i < 16 && i < (int)compressed_len; i++) {
        printf("%02X ", compressed_data[i]);
    }
    printf("...\n\n");

    // ============================================
    // 展開
    // ============================================
    printf("--- 展開処理 ---\n");

    // 展開用バッファを確保
    mz_ulong decompressed_len = original_len;
    unsigned char* decompressed_data = (unsigned char*)malloc(decompressed_len);

    if (!decompressed_data) {
        printf("エラー: メモリ確保失敗\n");
        free(compressed_data);
        return 1;
    }

    // 展開実行
    status = uncompress(
        decompressed_data,
        &decompressed_len,
        compressed_data,
        compressed_len
    );

    if (status != Z_OK) {
        printf("エラー: 展開失敗 (status=%d)\n", status);
        free(compressed_data);
        free(decompressed_data);
        return 1;
    }

    printf("展開後サイズ: %lu bytes\n\n", decompressed_len);

    // ============================================
    // 検証
    // ============================================
    printf("--- 検証 ---\n");

    if (strcmp(original_text, (char*)decompressed_data) == 0) {
        printf("✓ 成功: 元のテキストと完全に一致しました！\n");
    } else {
        printf("✗ 失敗: テキストが一致しません\n");
    }

    printf("\n展開後テキスト:\n%s\n", decompressed_data);

    // クリーンアップ
    free(compressed_data);
    free(decompressed_data);

    printf("\n=== 完了 ===\n");
    return 0;
}
