/**
 * データベース操作サンプル
 * SQLite を使用して簡単なデータベース操作を行います
 * 
 * ビルド: gcc -o db_app.exe main.c ../../include/sqlite3.c -I../../include -lpthread
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"

// エラーチェック用マクロ
#define CHECK_ERROR(rc, db, msg) \
    if (rc != SQLITE_OK) { \
        printf("エラー: %s - %s\n", msg, sqlite3_errmsg(db)); \
        sqlite3_close(db); \
        return 1; \
    }

// コールバック関数（SELECT結果の表示用）
int callback(void* data, int argc, char** argv, char** col_names) {
    printf("  ");
    for (int i = 0; i < argc; i++) {
        printf("%s: %s", col_names[i], argv[i] ? argv[i] : "NULL");
        if (i < argc - 1) printf(" | ");
    }
    printf("\n");
    return 0;
}

int main(void) {
    sqlite3* db;
    char* err_msg = NULL;
    int rc;

    printf("=== データベース操作サンプル (SQLite) ===\n\n");

    // ============================================
    // データベースを開く（なければ作成）
    // ============================================
    printf("[1] データベースを開く\n");
    
    rc = sqlite3_open("sample.db", &db);
    if (rc != SQLITE_OK) {
        printf("エラー: データベースを開けません - %s\n", sqlite3_errmsg(db));
        return 1;
    }
    printf("  ✓ sample.db を開きました\n\n");

    // ============================================
    // テーブル作成
    // ============================================
    printf("[2] テーブル作成\n");
    
    const char* sql_create = 
        "DROP TABLE IF EXISTS students;"
        "CREATE TABLE students ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  name TEXT NOT NULL,"
        "  age INTEGER,"
        "  grade TEXT"
        ");";
    
    rc = sqlite3_exec(db, sql_create, NULL, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        printf("エラー: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return 1;
    }
    printf("  ✓ students テーブルを作成しました\n\n");

    // ============================================
    // データ挿入
    // ============================================
    printf("[3] データ挿入\n");
    
    const char* sql_insert = 
        "INSERT INTO students (name, age, grade) VALUES ('田中太郎', 20, 'A');"
        "INSERT INTO students (name, age, grade) VALUES ('鈴木花子', 19, 'B');"
        "INSERT INTO students (name, age, grade) VALUES ('佐藤次郎', 21, 'A');"
        "INSERT INTO students (name, age, grade) VALUES ('山田三郎', 20, 'C');"
        "INSERT INTO students (name, age, grade) VALUES ('高橋美咲', 22, 'A');";
    
    rc = sqlite3_exec(db, sql_insert, NULL, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        printf("エラー: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return 1;
    }
    printf("  ✓ 5件のデータを挿入しました\n\n");

    // ============================================
    // データ取得（全件）
    // ============================================
    printf("[4] データ取得（全件）\n");
    
    const char* sql_select_all = "SELECT * FROM students;";
    
    rc = sqlite3_exec(db, sql_select_all, callback, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        printf("エラー: %s\n", err_msg);
        sqlite3_free(err_msg);
    }
    printf("\n");

    // ============================================
    // データ取得（条件付き）
    // ============================================
    printf("[5] データ取得（grade='A' の学生）\n");
    
    const char* sql_select_a = "SELECT name, age FROM students WHERE grade = 'A';";
    
    rc = sqlite3_exec(db, sql_select_a, callback, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        printf("エラー: %s\n", err_msg);
        sqlite3_free(err_msg);
    }
    printf("\n");

    // ============================================
    // データ更新
    // ============================================
    printf("[6] データ更新（山田三郎のgradeをBに変更）\n");
    
    const char* sql_update = "UPDATE students SET grade = 'B' WHERE name = '山田三郎';";
    
    rc = sqlite3_exec(db, sql_update, NULL, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        printf("エラー: %s\n", err_msg);
        sqlite3_free(err_msg);
    } else {
        printf("  ✓ 更新しました（変更行数: %d）\n\n", sqlite3_changes(db));
    }

    // ============================================
    // 集計クエリ
    // ============================================
    printf("[7] 集計クエリ（gradeごとの人数）\n");
    
    const char* sql_aggregate = 
        "SELECT grade, COUNT(*) as count FROM students GROUP BY grade ORDER BY grade;";
    
    rc = sqlite3_exec(db, sql_aggregate, callback, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        printf("エラー: %s\n", err_msg);
        sqlite3_free(err_msg);
    }
    printf("\n");

    // ============================================
    // プリペアドステートメント（パラメータ付きクエリ）
    // ============================================
    printf("[8] プリペアドステートメント\n");
    
    sqlite3_stmt* stmt;
    const char* sql_prepared = "SELECT * FROM students WHERE age >= ?;";
    
    rc = sqlite3_prepare_v2(db, sql_prepared, -1, &stmt, NULL);
    if (rc == SQLITE_OK) {
        // パラメータをバインド（age >= 20）
        sqlite3_bind_int(stmt, 1, 20);
        
        printf("  20歳以上の学生:\n");
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            const char* name = (const char*)sqlite3_column_text(stmt, 1);
            int age = sqlite3_column_int(stmt, 2);
            const char* grade = (const char*)sqlite3_column_text(stmt, 3);
            
            printf("    %d: %s (%d歳) - %s\n", id, name, age, grade);
        }
        sqlite3_finalize(stmt);
    }
    printf("\n");

    // ============================================
    // データ削除
    // ============================================
    printf("[9] データ削除（1件削除）\n");
    
    const char* sql_delete = "DELETE FROM students WHERE id = 2;";
    
    rc = sqlite3_exec(db, sql_delete, NULL, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        printf("エラー: %s\n", err_msg);
        sqlite3_free(err_msg);
    } else {
        printf("  ✓ 削除しました（削除行数: %d）\n\n", sqlite3_changes(db));
    }

    // 最終状態を表示
    printf("[10] 最終状態\n");
    rc = sqlite3_exec(db, sql_select_all, callback, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        printf("エラー: %s\n", err_msg);
        sqlite3_free(err_msg);
    }

    // データベースを閉じる
    sqlite3_close(db);
    
    printf("\n=== 完了 ===\n");
    printf("データベースファイル: sample.db\n");
    
    return 0;
}
