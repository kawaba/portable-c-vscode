# ============================================================
# Portable C言語開発環境 セットアップスクリプト
# MinGW-w64 + VS Code + 外部ライブラリ
# ============================================================

$ErrorActionPreference = "Stop"
$ProgressPreference = 'SilentlyContinue'  # 高速化

Write-Host "============================================================" -ForegroundColor Cyan
Write-Host " Portable C言語開発環境 セットアップ開始" -ForegroundColor Cyan
Write-Host " MinGW-w64 + VS Code + ライブラリ" -ForegroundColor Cyan
Write-Host "============================================================" -ForegroundColor Cyan
Write-Host ""

# ============================================================
# バージョン設定
# ============================================================
$MINGW_VERSION = "14.2.0"
$MINGW_RELEASE = "12.0.0-ucrt-r3"
$RAYLIB_VERSION = "5.5"

# ダウンロードURL
$urls = @{
    # winlibs.com からのMinGW-w64（ZIP形式）
    mingw = "https://github.com/brechtsanders/winlibs_mingw/releases/download/14.2.0posix-19.1.1-12.0.0-ucrt-r2/winlibs-x86_64-posix-seh-gcc-14.2.0-llvm-19.1.1-mingw-w64ucrt-12.0.0-r2.zip"
    
    # VS Code Portable
    vscode = "https://code.visualstudio.com/sha/download?build=stable&os=win32-x64-archive"
    
    # ライブラリ
    raylib = "https://github.com/raysan5/raylib/releases/download/$RAYLIB_VERSION/raylib-${RAYLIB_VERSION}_win64_mingw-w64.zip"
    stb_image = "https://raw.githubusercontent.com/nothings/stb/master/stb_image.h"
    stb_image_write = "https://raw.githubusercontent.com/nothings/stb/master/stb_image_write.h"
    miniz_h = "https://raw.githubusercontent.com/richgel999/miniz/master/miniz.h"
    miniz_c = "https://raw.githubusercontent.com/richgel999/miniz/master/miniz.c"
    aes_h = "https://raw.githubusercontent.com/kokke/tiny-AES-c/master/aes.h"
    aes_c = "https://raw.githubusercontent.com/kokke/tiny-AES-c/master/aes.c"
    sqlite = "https://sqlite.org/2024/sqlite-amalgamation-3470200.zip"
}

# 作業ディレクトリ
$baseDir = $PSScriptRoot
$tempDir = Join-Path $env:TEMP "portable-c-setup"

# ============================================================
# ヘルパー関数
# ============================================================
function Write-Step {
    param([string]$Step, [string]$Message)
    Write-Host ""
    Write-Host "[$Step] $Message" -ForegroundColor Green
}

function Write-SubStep {
    param([string]$Message)
    Write-Host "  $Message" -ForegroundColor Gray
}

function Write-Success {
    param([string]$Message)
    Write-Host "  ○ $Message" -ForegroundColor Green
}

function Write-Warning {
    param([string]$Message)
    Write-Host "  ! $Message" -ForegroundColor Yellow
}

function Download-File {
    param(
        [string]$Url,
        [string]$OutFile,
        [string]$Description
    )
    
    Write-SubStep "ダウンロード中: $Description"
    try {
        Invoke-WebRequest -Uri $Url -OutFile $OutFile -UseBasicParsing
        Write-Success "ダウンロード完了: $Description"
        return $true
    }
    catch {
        Write-Warning "ダウンロード失敗: $Description"
        Write-Host "    URL: $Url" -ForegroundColor Red
        return $false
    }
}

# 一時フォルダ作成
New-Item -ItemType Directory -Path $tempDir -Force | Out-Null

# ============================================================
# 1. MinGW-w64 のダウンロード・展開
# ============================================================
Write-Step "1/6" "MinGW-w64 をダウンロード中..."

$mingwDir = Join-Path $baseDir "mingw64"
$mingwZip = Join-Path $tempDir "mingw.zip"

if (Test-Path $mingwDir) {
    Write-Warning "MinGW-w64は既に存在します。スキップします。"
} else {
    Write-SubStep "（ファイルサイズが大きいため時間がかかります）"
    
    if (Download-File -Url $urls.mingw -OutFile $mingwZip -Description "MinGW-w64 GCC $MINGW_VERSION") {
        Write-SubStep "展開中..."
        Expand-Archive -Path $mingwZip -DestinationPath $baseDir -Force
        
        # winlibsの場合、mingw64フォルダが直接展開される
        if (-not (Test-Path $mingwDir)) {
            # フォルダ名が異なる場合の対応
            $extractedDir = Get-ChildItem $baseDir -Directory | Where-Object { $_.Name -like "mingw*" } | Select-Object -First 1
            if ($extractedDir) {
                Rename-Item $extractedDir.FullName $mingwDir
            }
        }
        
        Remove-Item $mingwZip -Force -ErrorAction SilentlyContinue
        Write-Success "MinGW-w64 の展開完了"
    } else {
        Write-Host ""
        Write-Host "代替方法: 手動でMinGW-w64をダウンロードしてください" -ForegroundColor Yellow
        Write-Host "  1. https://winlibs.com/ にアクセス" -ForegroundColor Yellow
        Write-Host "  2. UCRT版のZIPをダウンロード" -ForegroundColor Yellow
        Write-Host "  3. このフォルダに mingw64 として展開" -ForegroundColor Yellow
    }
}

# GCCのバージョン確認
$gccPath = Join-Path $mingwDir "bin\gcc.exe"
if (Test-Path $gccPath) {
    $gccVersion = & $gccPath --version | Select-Object -First 1
    Write-Success "GCC: $gccVersion"
}

# ============================================================
# 2. VS Code Portable のダウンロード・展開
# ============================================================
Write-Step "2/6" "VS Code Portable をダウンロード中..."

$vscodeDir = Join-Path $baseDir "vscode"
$vscodeZip = Join-Path $tempDir "vscode.zip"

if (Test-Path $vscodeDir) {
    Write-Warning "VS Codeは既に存在します。スキップします。"
} else {
    Write-SubStep "（ファイルサイズが大きいため時間がかかります）"
    
    if (Download-File -Url $urls.vscode -OutFile $vscodeZip -Description "VS Code Portable") {
        Write-SubStep "展開中..."
        Expand-Archive -Path $vscodeZip -DestinationPath $vscodeDir -Force
        Remove-Item $vscodeZip -Force -ErrorAction SilentlyContinue
        
        # ポータブルモード有効化（dataフォルダ作成）
        Write-SubStep "ポータブルモードを有効化中..."
        New-Item -Path "$vscodeDir\data" -ItemType Directory -Force | Out-Null
        
        Write-Success "VS Code の展開完了"
    }
}

# ============================================================
# 3. VS Code 拡張機能のインストール
# ============================================================
Write-Step "3/6" "VS Code 拡張機能をインストール中..."

$extFile = Join-Path $baseDir "config\extensions.txt"
$codePath = Join-Path $vscodeDir "bin\code.cmd"

if ((Test-Path $extFile) -and (Test-Path $codePath)) {
    $extensions = Get-Content $extFile | Where-Object { $_.Trim() -ne "" -and -not $_.StartsWith("#") }
    
    $count = 1
    $total = $extensions.Count
    
    foreach ($ext in $extensions) {
        Write-SubStep "[$count/$total] インストール中: $ext"
        & $codePath --install-extension $ext --force 2>&1 | Out-Null
        $count++
    }
    
    Write-Success "拡張機能のインストール完了 ($total 個)"
} else {
    Write-Warning "extensions.txt が見つからないか、VS Codeが未インストールです"
}

# ============================================================
# 4. VS Code 日本語化設定
# ============================================================
Write-Step "4/6" "VS Code の日本語化設定中..."

$localeDir = Join-Path $vscodeDir "data\user-data\User"
$localeFile = Join-Path $localeDir "locale.json"

if (-not (Test-Path $localeDir)) {
    New-Item -Path $localeDir -ItemType Directory -Force | Out-Null
}

@'
{
    "locale": "ja"
}
'@ | Out-File -FilePath $localeFile -Encoding utf8 -Force

Write-Success "日本語化設定 (locale.json) 完了"

# ============================================================
# 5. C言語ライブラリのダウンロード
# ============================================================
Write-Step "5/6" "C言語ライブラリをダウンロード中..."

$includeDir = Join-Path $baseDir "workspace\include"
$libDir = Join-Path $baseDir "workspace\lib"

# ディレクトリ確認
if (-not (Test-Path $includeDir)) { New-Item -Path $includeDir -ItemType Directory -Force | Out-Null }
if (-not (Test-Path $libDir)) { New-Item -Path $libDir -ItemType Directory -Force | Out-Null }

# --- raylib ---
Write-SubStep "raylib をダウンロード中..."
$raylibZip = Join-Path $tempDir "raylib.zip"
$raylibExtract = Join-Path $tempDir "raylib"

if (Download-File -Url $urls.raylib -OutFile $raylibZip -Description "raylib $RAYLIB_VERSION") {
    Expand-Archive -Path $raylibZip -DestinationPath $raylibExtract -Force
    
    $raylibFolder = Get-ChildItem $raylibExtract -Directory | Select-Object -First 1
    if ($raylibFolder) {
        Copy-Item "$($raylibFolder.FullName)\include\*.h" $includeDir -Force
        Copy-Item "$($raylibFolder.FullName)\lib\*.a" $libDir -Force
        Write-Success "raylib 配置完了"
    }
}

# --- stb_image ---
Write-SubStep "stb_image をダウンロード中..."
Download-File -Url $urls.stb_image -OutFile "$includeDir\stb_image.h" -Description "stb_image.h" | Out-Null
Download-File -Url $urls.stb_image_write -OutFile "$includeDir\stb_image_write.h" -Description "stb_image_write.h" | Out-Null

# --- miniz ---
Write-SubStep "miniz をダウンロード中..."
Download-File -Url $urls.miniz_h -OutFile "$includeDir\miniz.h" -Description "miniz.h" | Out-Null
Download-File -Url $urls.miniz_c -OutFile "$includeDir\miniz.c" -Description "miniz.c" | Out-Null

# --- tiny-AES-c ---
Write-SubStep "tiny-AES-c をダウンロード中..."
Download-File -Url $urls.aes_h -OutFile "$includeDir\aes.h" -Description "aes.h" | Out-Null
Download-File -Url $urls.aes_c -OutFile "$includeDir\aes.c" -Description "aes.c" | Out-Null

# --- SQLite ---
Write-SubStep "SQLite をダウンロード中..."
$sqliteZip = Join-Path $tempDir "sqlite.zip"
$sqliteExtract = Join-Path $tempDir "sqlite"

try {
    if (Download-File -Url $urls.sqlite -OutFile $sqliteZip -Description "SQLite") {
        Expand-Archive -Path $sqliteZip -DestinationPath $sqliteExtract -Force
        $sqliteFolder = Get-ChildItem $sqliteExtract -Directory | Select-Object -First 1
        if ($sqliteFolder) {
            Copy-Item "$($sqliteFolder.FullName)\sqlite3.h" $includeDir -Force
            Copy-Item "$($sqliteFolder.FullName)\sqlite3.c" $includeDir -Force
            Write-Success "SQLite 配置完了"
        }
    }
} catch {
    Write-Warning "SQLite のダウンロードに失敗しました"
    Write-Host "    手動でダウンロード: https://sqlite.org/download.html" -ForegroundColor Yellow
}

Write-Success "ライブラリのダウンロード完了"

# ============================================================
# 6. クリーンアップ
# ============================================================
Write-Step "6/6" "クリーンアップ中..."

# 一時フォルダ削除
Remove-Item $tempDir -Recurse -Force -ErrorAction SilentlyContinue
Write-Success "一時ファイルを削除しました"

# セットアップファイルの削除（オプション）
$deleteSetupFiles = $true  # セットアップ後に削除する場合は $true

if ($deleteSetupFiles) {
    $itemsToDelete = @(
        (Join-Path $baseDir "config"),
        (Join-Path $baseDir "setup.bat"),
        $PSCommandPath  # setup.ps1 自身
    )
    
    foreach ($item in $itemsToDelete) {
        if (Test-Path $item) {
            try {
                if (Test-Path $item -PathType Container) {
                    Remove-Item $item -Recurse -Force
                } else {
                    Remove-Item $item -Force
                }
                Write-SubStep "削除: $item"
            } catch {
                Write-Warning "$item の削除に失敗しました"
            }
        }
    }
}

# ============================================================
# 完了メッセージ
# ============================================================
Write-Host ""
Write-Host "============================================================" -ForegroundColor Cyan
Write-Host " セットアップが完了しました！" -ForegroundColor Cyan
Write-Host "============================================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "次の手順で起動してください:" -ForegroundColor White
Write-Host "  1. launch-vscode.bat をダブルクリック" -ForegroundColor Yellow
Write-Host "  2. VS Codeが起動します" -ForegroundColor Yellow
Write-Host ""
Write-Host "インストールされた環境:" -ForegroundColor White
Write-Host "  ・MinGW-w64 (GCC, GDB, Make)" -ForegroundColor Gray
Write-Host "  ・VS Code (Portable)" -ForegroundColor Gray
Write-Host "  ・C言語ライブラリ:" -ForegroundColor Gray
Write-Host "      - raylib (図形描画・ゲーム)" -ForegroundColor Gray
Write-Host "      - stb_image (PNG画像処理)" -ForegroundColor Gray
Write-Host "      - miniz (圧縮・展開)" -ForegroundColor Gray
Write-Host "      - SQLite (データベース)" -ForegroundColor Gray
Write-Host "      - tiny-AES-c (暗号処理)" -ForegroundColor Gray
Write-Host ""
Write-Host "GitHub Copilot を使用する場合:" -ForegroundColor White
Write-Host "  初回起動時にGitHubアカウントでサインインしてください" -ForegroundColor Gray
Write-Host ""

Write-Host "Press any key to exit..."
$null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")
