/**
 * 暗号処理サンプル
 * tiny-AES-c を使用してAES暗号化・復号を行います
 * 
 * ビルド: gcc -o crypto_app.exe main.c ../../include/aes.c -I../../include
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define CBC 1
#define CTR 0
#define ECB 0
#include "aes.h"

// バイト配列を16進数文字列で表示
void print_hex(const char* label, const uint8_t* data, size_t len) {
    printf("%s: ", label);
    for (size_t i = 0; i < len; i++) {
        printf("%02X ", data[i]);
    }
    printf("\n");
}

// パディング追加（PKCS7）
size_t add_padding(uint8_t* data, size_t len, size_t block_size) {
    size_t padding = block_size - (len % block_size);
    for (size_t i = 0; i < padding; i++) {
        data[len + i] = (uint8_t)padding;
    }
    return len + padding;
}

// パディング除去（PKCS7）
size_t remove_padding(uint8_t* data, size_t len) {
    if (len == 0) return 0;
    uint8_t padding = data[len - 1];
    if (padding > 16 || padding > len) return len;
    
    // パディングの検証
    for (size_t i = 0; i < padding; i++) {
        if (data[len - 1 - i] != padding) return len;
    }
    return len - padding;
}

int main(void) {
    printf("=== 暗号処理サンプル (AES-128-CBC) ===\n\n");

    // ============================================
    // 鍵と初期化ベクトル（IV）の設定
    // ============================================
    
    // AES-128 は 16バイト（128ビット）の鍵を使用
    uint8_t key[16] = {
        0x2B, 0x7E, 0x15, 0x16,
        0x28, 0xAE, 0xD2, 0xA6,
        0xAB, 0xF7, 0x15, 0x88,
        0x09, 0xCF, 0x4F, 0x3C
    };
    
    // CBC モードでは初期化ベクトル（IV）が必要
    uint8_t iv[16] = {
        0x00, 0x01, 0x02, 0x03,
        0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B,
        0x0C, 0x0D, 0x0E, 0x0F
    };
    
    // 復号用にIVをコピー（暗号化で変更されるため）
    uint8_t iv_dec[16];
    memcpy(iv_dec, iv, 16);

    printf("[鍵とIVの設定]\n");
    print_hex("鍵 (128bit)", key, 16);
    print_hex("IV         ", iv, 16);
    printf("\n");

    // ============================================
    // 暗号化するテキスト
    // ============================================
    
    const char* plaintext = "Hello, AES encryption! This is a secret message.";
    size_t plaintext_len = strlen(plaintext);
    
    printf("[平文]\n");
    printf("テキスト: \"%s\"\n", plaintext);
    printf("長さ:     %zu バイト\n\n", plaintext_len);

    // ============================================
    // 暗号化処理
    // ============================================
    
    printf("[暗号化処理]\n");
    
    // バッファを確保（パディング分を含む）
    size_t buffer_size = ((plaintext_len / 16) + 1) * 16;
    uint8_t* encrypted = (uint8_t*)malloc(buffer_size);
    
    if (!encrypted) {
        printf("エラー: メモリ確保失敗\n");
        return 1;
    }
    
    // データをコピーしてパディング追加
    memcpy(encrypted, plaintext, plaintext_len);
    size_t encrypted_len = add_padding(encrypted, plaintext_len, 16);
    
    printf("パディング後: %zu バイト\n", encrypted_len);
    
    // AES暗号化
    struct AES_ctx ctx;
    AES_init_ctx_iv(&ctx, key, iv);
    AES_CBC_encrypt_buffer(&ctx, encrypted, encrypted_len);
    
    print_hex("暗号文（先頭32バイト）", encrypted, 32);
    printf("\n");

    // ============================================
    // 復号処理
    // ============================================
    
    printf("[復号処理]\n");
    
    // 復号用バッファ
    uint8_t* decrypted = (uint8_t*)malloc(encrypted_len + 1);
    
    if (!decrypted) {
        printf("エラー: メモリ確保失敗\n");
        free(encrypted);
        return 1;
    }
    
    memcpy(decrypted, encrypted, encrypted_len);
    
    // AES復号（新しいコンテキストで、元のIVを使用）
    AES_init_ctx_iv(&ctx, key, iv_dec);
    AES_CBC_decrypt_buffer(&ctx, decrypted, encrypted_len);
    
    // パディング除去
    size_t decrypted_len = remove_padding(decrypted, encrypted_len);
    decrypted[decrypted_len] = '\0';  // NULL終端
    
    printf("復号後: \"%s\"\n", decrypted);
    printf("長さ:   %zu バイト\n\n", decrypted_len);

    // ============================================
    // 検証
    // ============================================
    
    printf("[検証]\n");
    if (strcmp(plaintext, (char*)decrypted) == 0) {
        printf("✓ 成功: 元のテキストと完全に一致しました！\n");
    } else {
        printf("✗ 失敗: テキストが一致しません\n");
    }

    // クリーンアップ
    free(encrypted);
    free(decrypted);

    // ============================================
    // AES の仕組み説明
    // ============================================
    
    printf("\n=== AES暗号の概要 ===\n");
    printf("・AES (Advanced Encryption Standard) は対称鍵暗号\n");
    printf("・鍵長: 128/192/256 ビット（今回は128ビット）\n");
    printf("・ブロックサイズ: 128 ビット（16バイト）\n");
    printf("・CBC モード: 前のブロックの暗号文を次のブロックとXOR\n");
    printf("・IV (初期化ベクトル): 最初のブロックのXOR用に使用\n");
    printf("\n注意: 実際の運用では鍵とIVを安全に管理する必要があります\n");

    printf("\n=== 完了 ===\n");
    return 0;
}
