@echo off
REM ============================================================
REM Portable VS Code + MinGW-w64 起動スクリプト
REM C言語開発環境用
REM ============================================================

REM スクリプトのあるディレクトリを基準にする
set BASE_DIR=%~dp0

REM ============================================================
REM MinGW-w64 環境変数の設定
REM ============================================================

REM MinGW-w64 のパスを設定
set MINGW_PATH=%BASE_DIR%mingw64\bin
set PATH=%MINGW_PATH%;%PATH%

REM コンパイラへの明示的なパス（VS Codeの設定で参照可能）
set CC=%MINGW_PATH%\gcc.exe
set CXX=%MINGW_PATH%\g++.exe
set GDB_PATH=%MINGW_PATH%\gdb.exe
set MAKE_PATH=%MINGW_PATH%\mingw32-make.exe

REM ============================================================
REM ライブラリパスの設定
REM ============================================================

REM ワークスペース内のincludeとlibへのパス
set C_INCLUDE_PATH=%BASE_DIR%workspace\include
set LIBRARY_PATH=%BASE_DIR%workspace\lib

REM ============================================================
REM 文字エンコーディング設定
REM ============================================================

set LANG=ja_JP.UTF-8
set LC_ALL=ja_JP.UTF-8

REM ============================================================
REM VS Code の起動
REM ============================================================

REM ワークスペースを指定して起動
REM --disable-workspace-trust: 信頼の確認ダイアログをスキップ
REM --locale=ja: 日本語モードで起動

start "" "%BASE_DIR%vscode\Code.exe" --locale=ja --disable-workspace-trust "%BASE_DIR%workspace"
