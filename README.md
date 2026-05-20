# Portable C言語開発環境

スクリプトで完全ポータブルな **VS Code + MinGW-w64 + C言語ライブラリ** 環境を自動生成します。

## 概要

C言語の応用講習のための開発環境です。外部ライブラリを使った以下のミニアプリを作成できます：

| プロジェクト | ライブラリ | 内容 |
|-------------|-----------|------|
| 01_http | WinHTTP | HTTP通信（Webページ取得） |
| 02_compress | miniz | 文字列の圧縮・展開 |
| 03_png | stb_image | PNG画像のサイズ計算 |
| 04_database | SQLite | 簡単なデータベース操作 |
| 05_graphics | raylib | 図形の描画 |
| 06_crypto | tiny-AES | 暗号処理 |
| 07_math | 標準ライブラリ | 方程式の解を求める |
| 08_game | raylib | シューティングゲーム |

## セットアップ

### 1. ダウンロード

このフォルダを**ローカルドライブ**の適切な場所に配置してください。

### 2. セットアップ実行

**setup.bat** をダブルクリックして実行します。

```
処理内容：
1. MinGW-w64 のダウンロード・展開
2. VS Code Portable のダウンロード・展開
3. VS Code 拡張機能のインストール
4. C言語ライブラリのダウンロード
```

**所要時間: 10〜20分**（ネットワーク速度による）

### 3. 完了後のフォルダ構成

```
portable-c-vscode/
├── mingw64/              # MinGW-w64 コンパイラ
├── vscode/               # VS Code Portable
├── workspace/            # ワークスペース
│   ├── .vscode/          # VS Code 設定
│   │   ├── tasks.json    # ビルドタスク
│   │   ├── launch.json   # デバッグ設定
│   │   └── ...
│   ├── include/          # ライブラリヘッダ
│   │   ├── raylib.h
│   │   ├── stb_image.h
│   │   ├── miniz.h
│   │   ├── sqlite3.h
│   │   └── aes.h
│   ├── lib/              # 静的ライブラリ
│   │   └── libraylib.a
│   └── projects/         # サンプルプロジェクト
│       ├── 01_http/
│       ├── 02_compress/
│       └── ...
├── launch-vscode.bat     # ★ VS Code 起動用
└── README.md
```

## 使い方

### VS Code の起動

必ず **launch-vscode.bat** をダブルクリックして起動してください。

このバッチファイルが以下の環境変数を設定します：
- MinGW-w64 へのパス（gcc, gdb, make）
- ライブラリへのパス（include, lib）

### ビルド方法

#### 方法1: VS Code のタスク（推奨）

1. プロジェクトフォルダ内の `main.c` を開く
2. `Ctrl+Shift+B` でビルドタスク一覧を表示
3. 対応するタスクを選択してビルド

#### 方法2: キーボードショートカット

- `F5` : デバッグ実行（ビルド→実行）
- `Ctrl+F5` : デバッグなしで実行

### デバッグ

1. ソースコードの行番号をクリックしてブレークポイントを設定
2. `F5` でデバッグ開始
3. `F10` : ステップオーバー
4. `F11` : ステップイン
5. `Shift+F5` : デバッグ終了

## GitHub Copilot の使用

### 初回設定

1. VS Code 起動後、左下の「サインイン」をクリック
2. GitHub アカウントでログイン
3. 認証完了後、Copilot が有効になります

### 使い方

- **コード補完**: コメントや関数名を入力すると候補が表示
- **チャット**: `Ctrl+Shift+I` で Copilot Chat を開く
- **インライン提案**: `Tab` で提案を受け入れ、`Esc` で却下

## 各ライブラリの使い方

### stb_image（画像処理）

```c
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int width, height, channels;
unsigned char* data = stbi_load("image.png", &width, &height, &channels, 0);
// ... 処理 ...
stbi_image_free(data);
```

### miniz（圧縮・展開）

```c
#define MINIZ_IMPLEMENTATION
#include "miniz.h"

mz_ulong compressed_len = compressBound(original_len);
compress(compressed, &compressed_len, original, original_len);
uncompress(decompressed, &decompressed_len, compressed, compressed_len);
```

### SQLite（データベース）

```c
#include "sqlite3.h"

sqlite3* db;
sqlite3_open("test.db", &db);
sqlite3_exec(db, "CREATE TABLE ...", NULL, NULL, NULL);
sqlite3_close(db);
```

### raylib（グラフィックス）

```c
#include "raylib.h"

InitWindow(800, 600, "Title");
while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText("Hello!", 10, 10, 20, BLACK);
    EndDrawing();
}
CloseWindow();
```

### tiny-AES（暗号）

```c
#define CBC 1
#include "aes.h"

struct AES_ctx ctx;
AES_init_ctx_iv(&ctx, key, iv);
AES_CBC_encrypt_buffer(&ctx, data, len);
```

## トラブルシューティング

### Q: gcc が見つからない

**原因**: launch-vscode.bat を使わずに VS Code を起動した  
**解決**: 必ず launch-vscode.bat から起動してください

### Q: raylib のウィンドウが表示されない

**原因**: ライブラリのリンク順序が間違っている  
**解決**: リンクオプションを以下の順序で指定
```
-lraylib -lopengl32 -lgdi32 -lwinmm
```

### Q: 日本語が文字化けする

**原因**: ソースファイルのエンコーディング  
**解決**: ファイルを UTF-8 (BOM なし) で保存

### Q: IntelliSense が機能しない

**原因**: C/C++ 拡張機能が正しく設定されていない  
**解決**: VS Code を再起動、または `Ctrl+Shift+P` → 「C/C++: IntelliSense の設定をリセット」

## ライブラリのライセンス

| ライブラリ | ライセンス | 商用利用 |
|-----------|-----------|---------|
| raylib | zlib | ○ 可能 |
| stb_image | MIT / Public Domain | ○ 可能 |
| miniz | MIT | ○ 可能 |
| SQLite | Public Domain | ○ 可能 |
| tiny-AES-c | Public Domain | ○ 可能 |

すべて教育目的・商用利用ともに問題ありません。

## 参考リンク

- [raylib 公式サイト](https://www.raylib.com/)
- [stb ライブラリ](https://github.com/nothings/stb)
- [SQLite 公式サイト](https://sqlite.org/)
- [MinGW-w64](https://www.mingw-w64.org/)
- [VS Code ドキュメント](https://code.visualstudio.com/docs)
