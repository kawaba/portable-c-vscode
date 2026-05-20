/**
 * HTTP通信サンプル
 * WinHTTP を使用してWebページを取得します
 * 
 * ビルド: gcc -o http_app.exe main.c -lwinhttp
 */

#include <stdio.h>
#include <windows.h>
#include <winhttp.h>

#pragma comment(lib, "winhttp.lib")

int main(void) {
    HINTERNET hSession = NULL;
    HINTERNET hConnect = NULL;
    HINTERNET hRequest = NULL;
    BOOL bResults = FALSE;
    DWORD dwSize = 0;
    DWORD dwDownloaded = 0;
    char* pszOutBuffer = NULL;

    printf("=== HTTP通信サンプル ===\n\n");

    // WinHTTPセッションを開く
    hSession = WinHttpOpen(
        L"C Workshop HTTP Client/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS,
        0
    );

    if (!hSession) {
        printf("エラー: WinHttpOpen failed (%lu)\n", GetLastError());
        return 1;
    }

    // サーバーに接続
    printf("接続先: httpbin.org\n");
    hConnect = WinHttpConnect(
        hSession,
        L"httpbin.org",
        INTERNET_DEFAULT_HTTPS_PORT,
        0
    );

    if (!hConnect) {
        printf("エラー: WinHttpConnect failed (%lu)\n", GetLastError());
        WinHttpCloseHandle(hSession);
        return 1;
    }

    // リクエストを作成
    printf("リクエスト: GET /get\n\n");
    hRequest = WinHttpOpenRequest(
        hConnect,
        L"GET",
        L"/get",
        NULL,
        WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        WINHTTP_FLAG_SECURE
    );

    if (!hRequest) {
        printf("エラー: WinHttpOpenRequest failed (%lu)\n", GetLastError());
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return 1;
    }

    // リクエストを送信
    bResults = WinHttpSendRequest(
        hRequest,
        WINHTTP_NO_ADDITIONAL_HEADERS,
        0,
        WINHTTP_NO_REQUEST_DATA,
        0,
        0,
        0
    );

    if (!bResults) {
        printf("エラー: WinHttpSendRequest failed (%lu)\n", GetLastError());
        goto cleanup;
    }

    // レスポンスを受信
    bResults = WinHttpReceiveResponse(hRequest, NULL);

    if (!bResults) {
        printf("エラー: WinHttpReceiveResponse failed (%lu)\n", GetLastError());
        goto cleanup;
    }

    // レスポンスボディを読み取る
    printf("--- レスポンス ---\n");
    
    do {
        dwSize = 0;
        if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) {
            printf("エラー: WinHttpQueryDataAvailable failed (%lu)\n", GetLastError());
            break;
        }

        if (dwSize == 0) break;

        pszOutBuffer = (char*)malloc(dwSize + 1);
        if (!pszOutBuffer) {
            printf("エラー: メモリ確保失敗\n");
            break;
        }

        ZeroMemory(pszOutBuffer, dwSize + 1);

        if (!WinHttpReadData(hRequest, pszOutBuffer, dwSize, &dwDownloaded)) {
            printf("エラー: WinHttpReadData failed (%lu)\n", GetLastError());
        } else {
            printf("%s", pszOutBuffer);
        }

        free(pszOutBuffer);
        pszOutBuffer = NULL;

    } while (dwSize > 0);

    printf("\n\n--- 完了 ---\n");

cleanup:
    if (hRequest) WinHttpCloseHandle(hRequest);
    if (hConnect) WinHttpCloseHandle(hConnect);
    if (hSession) WinHttpCloseHandle(hSession);

    return 0;
}
