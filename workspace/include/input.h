#ifndef INPUT_H
#define INPUT_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#define INPUT_STRING_BUFFER_SIZE 256

/* --- 文字入力 --- */
char getChar(const char* s);

/* --- 整数入力 --- */
int8_t getByte(const char* s);

int16_t getShort(const char* s);

int getInt(const char* s);

long long getLong(const char* s);

/* --- 16進数・2進数入力 --- */
int8_t getHex(const char* s);

int8_t getBin(const char* s);

/* --- 実数入力 --- */
float getFloat(const char* s);

double getDouble(const char* s);

/* --- Boolean入力 --- */
bool getBoolean(const char* s);

/* --- 文字列入力 (バッファ指定版) ---
 * 呼び出し元で用意したバッファ(buffer)に格納します。
 * size にはバッファのバイト数を指定してください (sizeof(buffer))。
 * 戻り値:
 *   成功時: buffer へのポインタ
 *   特殊入力時: NULL (例: getStringE で "\\0" 入力時)
 */
char* getString(char* buffer, size_t size, const char* s);

char* getStringE(char* buffer, size_t size, const char* s);

/* --- 配列入力 ---
 * 1行入力をスペース区切りで分割して、配列へ先頭から格納します。
 * 戻り値は「実際に配列へセットした要素数」です。
 *
 * 共通仕様:
 * - data が NULL、または maxCount <= 0 のときは 0 を返します。
 * - 入力取得に失敗したときは 0 を返します。
 * - 入力数が maxCount を超える場合、maxCount 個まで格納します。
 *
 * 変換仕様:
 * - getIntArray / getDoubleArray は、変換できない要素を 0 / 0.0 として格納します。
 * - getStringArray は、各要素を INPUT_STRING_BUFFER_SIZE - 1 文字まで格納し、
 *   末尾に '\0' を付与します。
 */
int getIntArray(int* data, int maxCount, const char* s);

int getDoubleArray(double* data, int maxCount, const char* s);

int getStringArray(char data[][INPUT_STRING_BUFFER_SIZE], int maxCount, const char* s);

/* --- 座標入力 ---
 * コンマ区切りのCSVでdouble値を2つ入力します。
 * 入力文字列の先頭・末尾の非数字文字を除去後、コンマで分割し、
 * 両端の空白を取り除いてdoubleに変換します。
 * 変換できない場合は0.0を使用します。
 * 値がちょうど2つになるまで再入力を繰り返します。
 * s が NULL のとき、デフォルトプロンプト "[x, y] > " を使用します。
 */
void getCoordinate(double* x, double* y, const char* s);

#endif /* INPUT_H */