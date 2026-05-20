#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "input.h"

#define MAX_BUFFER INPUT_STRING_BUFFER_SIZE

/* --- 内部ユーティリティ関数 --- */

/* 文字列の前後の空白を取り除く */
static char* trimWhitespace(char* text);

/* コンソールから文字列を取得し、バッファに格納する */
static char* getStr(const char* s, char* buffer, size_t size) {
    if (buffer == NULL || size == 0) {
        return NULL;
    }

    if (s != NULL) {
        printf("%s", s);
    }
    fflush(stdout);

    if (fgets(buffer, size, stdin) == NULL) {
        return NULL;
    }

    size_t newlinePos = strcspn(buffer, "\r\n");
    if (buffer[newlinePos] == '\r' || buffer[newlinePos] == '\n') {
        buffer[newlinePos] = '\0'; /* 改行を除去 */
    } else {
        int ch;
        while ((ch = getchar()) != '\n' && ch != '\r' && ch != EOF) {
            /* 長すぎる入力の残りを読み捨てる */
        }
        if (ch == '\r') {
            int next = getchar();
            if (next != '\n' && next != EOF) {
                ungetc(next, stdin);
            }
        }
    }

    return buffer;
}

/* 文字列の前後の空白を取り除く */
static char* trimWhitespace(char* text) {
    if (text == NULL) {
        return NULL;
    }

    while (*text != '\0' && isspace((unsigned char)*text)) {
        text++;
    }

    if (*text == '\0') {
        return text;
    }

    char* end = text + strlen(text) - 1;
    while (end > text && isspace((unsigned char)*end)) {
        end--;
    }
    end[1] = '\0';

    return text;
}

/* ラベルがNULLのとき既定ラベルを返す
　　また、ラベルがNULLでないときは、前後の空白を取り除いて末尾に'>'がなければ、" > "を追加して返す */
static const char* resolveLabel(const char* s, const char* defaultLabel) {
    static char labelBuffer[MAX_BUFFER];
    const char* start;
    const char* end;
    size_t length;

    if (s == NULL) {
        return (defaultLabel != NULL) ? defaultLabel : "";
    }

    start = s;
    while (*start != '\0' && isspace((unsigned char)*start)) {
        start++;
    }

    end = start + strlen(start);
    while (end > start && isspace((unsigned char)*(end - 1))) {
        end--;
    }

    length = (size_t)(end - start);
    if (length >= MAX_BUFFER) {
        length = MAX_BUFFER - 1;
    }

    memcpy(labelBuffer, start, length);
    labelBuffer[length] = '\0';

    if ((length == 0 || labelBuffer[length - 1] != '>') && length < MAX_BUFFER - 3) {
        labelBuffer[length] = ' ';
        labelBuffer[length + 1] = '>';
        labelBuffer[length + 2] = ' ';
        labelBuffer[length + 3] = '\0';
    }

    return labelBuffer;
}

/* --- 文字入力 --- */
char getChar(const char* s) {
    const char* label = resolveLabel(s, "char > ");
    char buffer[MAX_BUFFER];
    if (getStr(label, buffer, sizeof(buffer)) == NULL || strlen(buffer) == 0) return 0;
    return buffer[0];
}

/* --- 整数入力 --- */
int8_t getByte(const char* s) {
    const char* label = resolveLabel(s, "byte > ");
    char buffer[MAX_BUFFER];
    if (getStr(label, buffer, sizeof(buffer)) == NULL || strlen(buffer) == 0) return 0;
    char *endptr;
    long val = strtol(buffer, &endptr, 10);
    if (endptr == buffer) return 0;
    return (int8_t)val;
}

int16_t getShort(const char* s) {
    const char* label = resolveLabel(s, "short > ");
    char buffer[MAX_BUFFER];
    if (getStr(label, buffer, sizeof(buffer)) == NULL || strlen(buffer) == 0) return 0;
    char *endptr;
    long val = strtol(buffer, &endptr, 10);
    if (endptr == buffer) return 0;
    return (int16_t)val;
}

int getInt(const char* s) {
    const char* label = resolveLabel(s, "int > ");
    char buffer[MAX_BUFFER];
    if (getStr(label, buffer, sizeof(buffer)) == NULL || strlen(buffer) == 0) return 0;
    char *endptr;
    long val = strtol(buffer, &endptr, 10);
    if (endptr == buffer) return 0;
    return (int)val;
}

long long getLong(const char* s) {
    const char* label = resolveLabel(s, "long > ");
    char buffer[MAX_BUFFER];
    if (getStr(label, buffer, sizeof(buffer)) == NULL || strlen(buffer) == 0) return 0;
    char *endptr;
    long long val = strtoll(buffer, &endptr, 10); /* Javaのlongは64bitなのでstrtollを使用 */
    if (endptr == buffer) return 0;
    return val;
}

/* --- 16進数・2進数入力 --- */
int8_t getHex(const char* s) {
    const char* label = resolveLabel(s, "hex > ");
    char buffer[MAX_BUFFER];
    if (getStr(label, buffer, sizeof(buffer)) == NULL || strlen(buffer) == 0) return 0;
    if (strlen(buffer) > 2) buffer[2] = '\0'; /* 先頭2文字だけを使う */

    char *endptr;
    long val = strtol(buffer, &endptr, 16);
    if (endptr == buffer) return 0;
    return (int8_t)val;
}

int8_t getBin(const char* s) {
    const char* label = resolveLabel(s, "bin > ");
    char buffer[MAX_BUFFER];
    if (getStr(label, buffer, sizeof(buffer)) == NULL || strlen(buffer) == 0) return 0;
    if (strlen(buffer) > 8) buffer[8] = '\0'; /* 先頭8文字だけを使う */

    char *endptr;
    long val = strtol(buffer, &endptr, 2);
    if (endptr == buffer) return 0;
    return (int8_t)val;
}

/* --- 実数入力 --- */
float getFloat(const char* s) {
    const char* label = resolveLabel(s, "float > ");
    char buffer[MAX_BUFFER];
    if (getStr(label, buffer, sizeof(buffer)) == NULL || strlen(buffer) == 0) return 0.0f;
    char *endptr;
    float val = strtof(buffer, &endptr);
    if (endptr == buffer) return 0.0f;
    return val;
}

double getDouble(const char* s) {
    const char* label = resolveLabel(s, "double > ");
    char buffer[MAX_BUFFER];
    if (getStr(label, buffer, sizeof(buffer)) == NULL || strlen(buffer) == 0) return 0.0;
    char *endptr;
    double val = strtod(buffer, &endptr);
    if (endptr == buffer) return 0.0;
    return val;
}

/* --- Boolean入力 --- */
bool getBoolean(const char* s) {
    const char* label = resolveLabel(s, "boolean > ");
    char buffer[MAX_BUFFER];
    if (getStr(label, buffer, sizeof(buffer)) == NULL) return false;

    /* 小文字に変換して判定 */
    for (int i = 0; buffer[i]; i++) {
        buffer[i] = tolower((unsigned char)buffer[i]);
    }

    if (strcmp(buffer, "true") == 0) {
        return true;
    }
    return false;
}

/* --- 文字列入力 --- */
char* getString(char* buffer, size_t size, const char* s) {
    const char* label = resolveLabel(s, "string > ");
    /* バッファオーバーラン防止のため、必ずsizeを渡す */
    if (getStr(label, buffer, size) == NULL || strlen(buffer) == 0) {
        return NULL;
    }
    return buffer;
}

char* getStringE(char* buffer, size_t size, const char* s) {
    const char* label = resolveLabel(s, "string > ");
    if (getStr(label, buffer, size) == NULL) {
        buffer[0] = '\0';
        return buffer; /* 入力失敗時は空文字を返す */
    }
    if (strlen(buffer) == 0) {
        return buffer; /* 空文字 */
    } else if (strcmp(buffer, "\\0") == 0 || strcmp(buffer, "\\null") == 0) {
        return NULL; /* 特殊入力でNULLを返す */
    } else {
        return buffer;
    }
}

/* --- 配列入力 --- */
int getIntArray(int* data, int maxCount, const char* s) {
    if (data == NULL || maxCount <= 0) {
        return 0;
    }

    const char* label = resolveLabel(s, "入力例：3 12 20 18 9（スペース区切り）> ");

    char buffer[MAX_BUFFER];
    if (getStr(label, buffer, sizeof(buffer)) == NULL) {
        return 0;
    }

    /* 入力1行を空白で分割し、intへ変換して格納する */
    int index = 0;
    char* token = strtok(buffer, " ");
    while (token != NULL && index < maxCount) {
        char* trimmed = trimWhitespace(token);
        char* endptr = NULL;
        long value = strtol(trimmed, &endptr, 10);

        if (trimmed[0] == '\0' || endptr == trimmed || *endptr != '\0') {
            data[index] = 0;
        } else {
            data[index] = (int)value;
        }

        index++;
        token = strtok(NULL, " ");
    }

    return index;
}

int getDoubleArray(double* data, int maxCount, const char* s) {
    if (data == NULL || maxCount <= 0) {
        return 0;
    }

    const char* label = resolveLabel(s, "入力例：1.5  3.2  2.0 （スペース区切り）> ");

    char buffer[MAX_BUFFER];
    if (getStr(label, buffer, sizeof(buffer)) == NULL) {
        return 0;
    }

    /* 入力1行を空白で分割し、doubleへ変換して格納する */
    int index = 0;
    char* token = strtok(buffer, " ");
    while (token != NULL && index < maxCount) {
        char* trimmed = trimWhitespace(token);
        char* endptr = NULL;
        double value = strtod(trimmed, &endptr);

        if (trimmed[0] == '\0' || endptr == trimmed || *endptr != '\0') {
            data[index] = 0.0;
        } else {
            data[index] = value;
        }

        index++;
        token = strtok(NULL, " ");
    }

    return index;
}

int getStringArray(char data[][INPUT_STRING_BUFFER_SIZE], int maxCount, const char* s) {
    if (data == NULL || maxCount <= 0) {
        return 0;
    }

    const char* label = resolveLabel(s, "入力例：hello world（スペース区切り）> ");

    char buffer[MAX_BUFFER];
    if (getStr(label, buffer, sizeof(buffer)) == NULL) {
        return 0;
    }

    /* 入力1行を空白で分割し、文字列をそのまま格納する */
    int index = 0;
    char* token = strtok(buffer, " ");
    while (token != NULL && index < maxCount) {
        char* trimmed = trimWhitespace(token);
        strncpy(data[index], trimmed, INPUT_STRING_BUFFER_SIZE - 1);
        data[index][INPUT_STRING_BUFFER_SIZE - 1] = '\0';

        index++;
        token = strtok(NULL, " ");
    }

    return index;
}

/* --- 座標入力 --- */
void getCoordinate(double* x, double* y, const char* s) {
    const char* label = resolveLabel(s, "[x, y] > ");
    char buffer[MAX_BUFFER];

    while (1) {
        if (getStr(label, buffer, sizeof(buffer)) == NULL) {
            continue;
        }

        /* 先頭の非数字文字（数字・符号・小数点以外）を除去 */
        char* start = buffer;
        while (*start != '\0' && !isdigit((unsigned char)*start)
               && *start != '-' && *start != '+' && *start != '.') {
            start++;
        }

        /* 末尾の非数字文字を除去 */
        if (*start != '\0') {
            char* end = start + strlen(start) - 1;
            while (end > start && !isdigit((unsigned char)*end)) {
                end--;
            }
            end[1] = '\0';
        }

        /* コンマで分割し、値を最大3つまで数える */
        char temp[MAX_BUFFER];
        strncpy(temp, start, MAX_BUFFER - 1);
        temp[MAX_BUFFER - 1] = '\0';

        double values[2] = {0.0, 0.0};
        int count = 0;
        char* token = strtok(temp, ",");
        while (token != NULL) {
            char* trimmed = trimWhitespace(token);
            if (count < 2) {
                char* endptr = NULL;
                double val = strtod(trimmed, &endptr);
                values[count] = (trimmed[0] == '\0' || endptr == trimmed) ? 0.0 : val;
            }
            count++;
            if (count > 2) {
                break; /* 3つ以上は即座に判定打ち切り */
            }
            token = strtok(NULL, ",");
        }

        if (count == 2) {
            *x = values[0];
            *y = values[1];
            break;
        }
        /* 2つでなければ再入力 */
    }
}
