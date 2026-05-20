/**
 * utils.h - 共通ユーティリティ関数
 *
 * 配置場所: workspace/include/utils.h
 *
 * 使い方:
 *   #include "utils.h"
 *
 *   int main(void) {
 *       printf("Hello!\n");
 *       wait_enter();
 *       return 0;
 *   }
 */

#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>

/**
 * Enterキーが押されるまで待機
 * ※ printf のみ使用した場合（scanfなし）
 */
static inline void wait_enter(void) {
    printf("Press Enter to exit...");
    getchar();
}

/**
 * Enterキーが押されるまで待機
 * ※ scanf 使用後に呼ぶ場合はこちらを使用
 *   （入力バッファに残った改行を先にクリア）
 */
static inline void wait_enter_after_scanf(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);  // バッファクリア
    printf("Press Enter to exit...");
    getchar();
}

/**
 * 入力バッファをクリア
 * scanf の後に fgets を使う前などに呼ぶ
 */
static inline void flush_stdin(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/**
 * 画面をクリア（Windows用）
 */
static inline void clear_screen(void) {
    system("cls");
}

/**
 * CP932文字列をUTF-8文字列に変換して dst に格納する
 * dst_size には dst 配列のサイズ（sizeof(dst)）を渡す
 */
static inline void cp932_to_utf8(const char *src, char *dst, size_t dst_size) {
    wchar_t wide_buf[1024];
    MultiByteToWideChar(CP_ACP, 0, src, -1, wide_buf, 1024);
    WideCharToMultiByte(CP_UTF8, 0, wide_buf, -1, dst, (int)dst_size, NULL, NULL);
}

/**
 * UTF-8文字列をCP932文字列に変換して dst に格納する
 * dst_size には dst 配列のサイズ（sizeof(dst)）を渡す
 */
static inline void utf8_to_cp932(const char *src, char *dst, size_t dst_size) {
    wchar_t wide_buf[1024];
    MultiByteToWideChar(CP_UTF8, 0, src, -1, wide_buf, 1024);
    WideCharToMultiByte(CP_ACP, 0, wide_buf, -1, dst, (int)dst_size, NULL, NULL);
}

/**
 * 小数点以下digits桁で四捨五入
 */
static inline double round_to_digits(double value, int digits) {
    double scale = 1.0;

    if (digits < 0) {
        return value;
    }

    for (int index = 0; index < digits; index++) {
        scale *= 10.0;
    }

    return round(value * scale) / scale;
}

#endif /* UTILS_H */
