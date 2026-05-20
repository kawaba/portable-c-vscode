#ifndef KAKEIBO_H
#define KAKEIBO_H

#include <stddef.h>

#define MAX_TRANSACTIONS 2000
#define MAX_CATEGORIES 50
#define MAX_LINE_LENGTH 256
#define DATE_TEXT_LENGTH 11

// デフォルトの月予算金額
#define DEFAULT_MONTHLY_BUDGET 50000

#define INOUT_EXPENSE 0
#define INOUT_INCOME 1

// 年月日を表す日付データ
typedef struct {
    int year;
    int month;
    int day;
} Date;

// 入出金1件分の取引データ
typedef struct {
    int id;
    Date date;
    int inout_type;
    int amount;
    int category_id;
} Transaction;

// 日付ごとの集計結果データ
typedef struct {
    Date date;
    int income_total;
    int expense_total;
    int balance;
} DateTotal;

// 適用項目ごとの集計結果データ
typedef struct {
    int category_id;
    int income_total;
    int expense_total;
    int balance;
} CategoryTotal;

// 画面表示に必要な集計データ一式
typedef struct {
    CategoryTotal category_totals[MAX_CATEGORIES];
    size_t category_count;
    int month_spending;
    int remaining;
} ScreenData;

// 起動時にCSV読み込みと次ID計算を行う
int init_system(
    const char *csv_path,
    Transaction records[],
    size_t max_count,
    size_t *count,
    int *next_id
);

// CSVファイルから取引一覧を読み込む
int load_transactions_csv(
    const char *csv_path,
    Transaction records[],
    size_t max_count,
    size_t *count
);

// 取引一覧をCSVファイルへ保存する
int save_transactions_csv(
    const char *csv_path,
    const Transaction records[],
    size_t count
);

// CSV1行を取引構造体に変換する
int parse_transaction_line(const char *line, Transaction *out_tx);

// 取引構造体をCSV1行文字列に変換する
int to_transaction_line(
    const Transaction *transaction,
    char *buffer,
    size_t buffer_size
);

// 次に使用する連番IDを取得する
int get_next_transaction_id(const Transaction records[], size_t count);

// 取引を1件追加する
int add_transaction(
    Transaction records[],
    size_t max_count,
    size_t *count,
    int *next_id,
    int inout_type,
    int amount,
    int category_id,
    Date date
);

// eコマンド引数を解析して値を取り出す
int parse_e_command_args(
    const char *arg_csv,
    int *inout_type,
    int *amount,
    int *category_id
);

// 日付ごとに取引を集計する
int aggregate_by_date(
    const Transaction records[],
    size_t count,
    DateTotal totals[],
    size_t max_totals,
    size_t *total_count
);

// 適用項目ごとに取引を集計する
int aggregate_by_category(
    const Transaction records[],
    size_t count,
    CategoryTotal totals[],
    size_t max_totals,
    size_t *total_count
);

// 指定年月の予算を読み込む
int load_monthly_budget(const char *budget_path, int year, int month, int *budget);

// 指定年月の予算を保存する
int save_monthly_budget(const char *budget_path, int year, int month, int budget);

// 指定年月の出金合計を計算する
int calculate_month_spending(
    const Transaction records[],
    size_t count,
    int year,
    int month
);

// 当月予算と支出から予算残額を計算する
int calculate_budget_remaining(int monthly_budget, int month_spending);

// ダッシュボード画面を表示する
void render_dashboard(
    const CategoryTotal category_totals[],
    size_t category_count,
    int monthly_budget,
    int remaining
);

// コマンド入力を読み取りコマンドと引数に分ける
int read_command_line(
    char *command,
    size_t command_size,
    char *args,
    size_t args_size
);

// 入力コマンドを解釈して処理を実行する
int execute_command(
    char command,
    const char *args,
    Transaction records[],
    size_t max_count,
    size_t *count,
    int *next_id,
    const char *csv_path,
    int *need_redraw,
    int *should_exit
);

// 画面表示用の集計データを再計算する
int refresh_screen_data(
    const Transaction records[],
    size_t count,
    int year,
    int month,
    int budget,
    ScreenData *out_data
);

#endif
