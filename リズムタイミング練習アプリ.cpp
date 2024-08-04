#include <windows.h>
#include <gdiplus.h>
#include <thread>
#include <commctrl.h>
#include <sstream>
#include "forThisAPP.h"

#pragma comment (lib,"Gdiplus.lib")
using namespace Gdiplus;

#include <SFML/Audio.hpp> //音声再生用
//#include <SFML/Graphics.hpp> //画像表示用

#define ID_TimingBar 1
#define ID_TempoBar 2
#define IDM_PLAY 3
#define MAX_LOADSTRING 100
// グローバル変数:
HINSTANCE hInst;                                // 現在のインターフェイス
WCHAR szTitle[MAX_LOADSTRING];                  // タイトル バーのテキスト
WCHAR szWindowClass[MAX_LOADSTRING];            // メイン ウィンドウ クラス名
// このコード モジュールに含まれる関数の宣言を転送します:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int noteLength = 666; //音符の長さ
int tempo = 120;
int cycleTime = 1000 * 60 / tempo;


//操作用関数・変数・ハンドル
void Play();
bool play = false; // 再生・停止用の判定
int soundMode = 0; // ドラムとクリックのモードか判定用
int beat = 4;

HWND PlaytButton;
HWND hTempoBar;
HWND hNoteLengthBar;                            // ノートの長さを変更するトラックバーのハンドル

HWND hButton_off;
HWND hButton_1;
HWND hButton_2;
HWND hButton_3;
HWND hButton_4;
HWND hButton_5;
HWND hButton_6;
HWND hButton_7;
HWND hButton_8;
HWND hButton_Snare_Kick;
HWND hButton_Click;

HFONT hFont;
HFONT hFontS;


//表示位置決め用
int onpu_pojition[2] = { 120, 225 }; //音符の位置
int TB[4] = { 200, 100, 300, 30 }; // Timing Barの位置　(左,上,長さ,幅)
int TpB[4] = { 740, 400, 200, 30 }; // Tempo Barの位置　(左,上,長さ,幅)
int Beat[5] = { 735, 120,775,122,140 };//Beatの位置
int ST[5] = { 740, 250,755,252,155 };// SoundTypeの位置
int sita_po = 84; //下矢印の位置
int migi_po = 25; //右矢印の位置



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////ウィンメイン関数
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    //    AttachConsole(); //コンソール出力用
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MY, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY));
    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

        }
    }

    return (int)msg.wParam;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////ウィンドウクラスの登録
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    //wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_MY);
    wcex.lpszMenuName = NULL;  // メニューを設定しない
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////インスタンスハンドルの登録
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // グローバル変数にインスタンス ハンドルを格納する

    HWND hWnd = CreateWindowEx(0, szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1040, 600, nullptr, NULL, hInst, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////プロシージャ
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        //使用するフォントの設定
        hFont = CreateFont(30, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Calibri");
        hFontS = CreateFont(25, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Calibri"); 
 

        //トラックバー　テンポ
        hTempoBar = CreateWindowEx(0, TRACKBAR_CLASS, NULL,
            WS_CHILD | WS_VISIBLE | TBS_HORZ,
            TpB[0], TpB[1], TpB[2], TpB[3], hWnd, (HMENU)ID_TempoBar, hInst, NULL);
        SendMessage(hTempoBar, TBM_SETRANGE, TRUE, MAKELPARAM(-100, 100));   // 操作可能な範囲を-100 ～ 100に設定
        SendMessage(hTempoBar, TBM_SETPOS, TRUE, 0);                       // 初期値を0に設定

        //トラックバー　タイミング
        hNoteLengthBar = CreateWindowEx(0, TRACKBAR_CLASS, NULL,
            WS_CHILD | WS_VISIBLE | TBS_HORZ,
            TB[0], TB[1], TB[2], TB[3], hWnd, (HMENU)ID_TimingBar, hInst, NULL);
        SendMessage(hNoteLengthBar, TBM_SETRANGE, TRUE, MAKELPARAM(0, 1000));   // 操作可能な範囲を-50 ～ 50に設定
        SendMessage(hNoteLengthBar, TBM_SETPOS, TRUE, 666);                       // 初期値を0に設定

        
        //Playボタン
        PlaytButton = CreateWindowEx(0, L"BUTTON", L"Play", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            250, 400, 200, 60, hWnd, (HMENU)IDM_PLAY, hInst, NULL);
        SendMessage(PlaytButton, WM_SETFONT, (WPARAM)hFont, TRUE);
        
        //Off ～ 8ボタン
        int aaa = 50;
        int bbb = 20;
        hButton_off = CreateWindowEx(0, L"BUTTON", L"", BS_OWNERDRAW | WS_TABSTOP | WS_VISIBLE | WS_CHILD, Beat[0] - 10     , Beat[1] + aaa, 30, 25, hWnd, (HMENU)21, hInst, NULL);
        hButton_off = CreateWindowEx(0, L"BUTTON", L"", BS_OWNERDRAW | WS_TABSTOP | WS_VISIBLE | WS_CHILD, Beat[0] + bbb    , Beat[1] + aaa, 30, 25, hWnd, (HMENU)22, hInst, NULL);
        hButton_off = CreateWindowEx(0, L"BUTTON", L"", BS_OWNERDRAW | WS_TABSTOP | WS_VISIBLE | WS_CHILD, Beat[0] + bbb * 2, Beat[1] + aaa, 30, 25, hWnd, (HMENU)23, hInst, NULL);
        hButton_off = CreateWindowEx(0, L"BUTTON", L"", BS_OWNERDRAW | WS_TABSTOP | WS_VISIBLE | WS_CHILD, Beat[0] + bbb * 3, Beat[1] + aaa, 30, 25, hWnd, (HMENU)24, hInst, NULL);
        hButton_off = CreateWindowEx(0, L"BUTTON", L"", BS_OWNERDRAW | WS_TABSTOP | WS_VISIBLE | WS_CHILD, Beat[0] + bbb * 4, Beat[1] + aaa, 30, 25, hWnd, (HMENU)25, hInst, NULL);
        hButton_off = CreateWindowEx(0, L"BUTTON", L"", BS_OWNERDRAW | WS_TABSTOP | WS_VISIBLE | WS_CHILD, Beat[0] + bbb * 5, Beat[1] + aaa, 30, 25, hWnd, (HMENU)26, hInst, NULL);
        hButton_off = CreateWindowEx(0, L"BUTTON", L"", BS_OWNERDRAW | WS_TABSTOP | WS_VISIBLE | WS_CHILD, Beat[0] + bbb * 6, Beat[1] + aaa, 30, 25, hWnd, (HMENU)27, hInst, NULL);
        hButton_off = CreateWindowEx(0, L"BUTTON", L"", BS_OWNERDRAW | WS_TABSTOP | WS_VISIBLE | WS_CHILD, Beat[0] + bbb * 7, Beat[1] + aaa, 30, 25, hWnd, (HMENU)28, hInst, NULL);
        hButton_off = CreateWindowEx(0, L"BUTTON", L"", BS_OWNERDRAW | WS_TABSTOP | WS_VISIBLE | WS_CHILD, Beat[0] + bbb * 8, Beat[1] + aaa, 30, 25, hWnd, (HMENU)29, hInst, NULL);
        
        // SoundType ボタン
        hButton_off = CreateWindowEx(0, L"BUTTON", L"", BS_OWNERDRAW | WS_TABSTOP | WS_VISIBLE | WS_CHILD, ST[0] + 30, ST[1] + 25, ST[2] - ST[0] + ST[4] - 30, 25, hWnd, (HMENU)30, hInst, NULL);
        hButton_off = CreateWindowEx(0, L"BUTTON", L"", BS_OWNERDRAW | WS_TABSTOP | WS_VISIBLE | WS_CHILD, ST[0] + 30, ST[1] + 55, ST[2] - ST[0] + ST[4] - 30, 25, hWnd, (HMENU)31, hInst, NULL);
        


        break;
    }
    case WM_DRAWITEM: // 自作ボタンの描画内容の編集
    {

            LPDRAWITEMSTRUCT pDrawItem = (LPDRAWITEMSTRUCT)lParam;
        if (21 <= pDrawItem->CtlID && pDrawItem->CtlID <= 31)  // ボタンのID
        { 
            HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
            FillRect(pDrawItem->hDC, &pDrawItem->rcItem, hBrush);
            DeleteObject(hBrush);
            SetBkMode(pDrawItem->hDC, TRANSPARENT); SetTextColor(pDrawItem->hDC, RGB(0, 0, 0)); SelectObject(pDrawItem->hDC, hFontS);
            SetBkMode(pDrawItem->hDC, TRANSPARENT); // 背景を透過
            SetTextColor(pDrawItem->hDC, RGB(0, 0, 0)); // テキストの色
            SelectObject(pDrawItem->hDC, hFontS);
        }
        if (pDrawItem->CtlID == 21) { DrawText(pDrawItem->hDC, L"Off", -1, &pDrawItem->rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE); }
        if (pDrawItem->CtlID == 22) { DrawText(pDrawItem->hDC, L"1", -1, &pDrawItem->rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE); }
        if (pDrawItem->CtlID == 23) { DrawText(pDrawItem->hDC, L"2", -1, &pDrawItem->rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE); }
        if (pDrawItem->CtlID == 24) { DrawText(pDrawItem->hDC, L"3", -1, &pDrawItem->rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE); }
        if (pDrawItem->CtlID == 25) { DrawText(pDrawItem->hDC, L"4", -1, &pDrawItem->rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE); }
        if (pDrawItem->CtlID == 26) { DrawText(pDrawItem->hDC, L"5", -1, &pDrawItem->rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE); }
        if (pDrawItem->CtlID == 27) { DrawText(pDrawItem->hDC, L"6", -1, &pDrawItem->rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE); }
        if (pDrawItem->CtlID == 28) { DrawText(pDrawItem->hDC, L"7", -1, &pDrawItem->rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE); }
        if (pDrawItem->CtlID == 29) { DrawText(pDrawItem->hDC, L"8", -1, &pDrawItem->rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE); }
        if (pDrawItem->CtlID == 30) { DrawText(pDrawItem->hDC, L"Snaer + Kick", -1, &pDrawItem->rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE); }
        if (pDrawItem->CtlID == 31) { DrawText(pDrawItem->hDC, L"Click", -1, &pDrawItem->rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE); }
        
        break;
    }

    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        int index_count;
        int index_level;

        switch (wmId)
        {
            // Playボタン操作時
        case IDM_PLAY:
            if (play == false) {
                play = true;
                std::wstring buttonText = L"Playing";
                SetWindowText(PlaytButton, buttonText.c_str());
                // ループ再生用のスレッドの作成
                std::thread(Play).detach();
            }
            else if (play == true){
                play = false;
            std::wstring buttonText = L"Stop";
            SetWindowText(PlaytButton, buttonText.c_str());
            }

            break;

        // 画面右側ボタン操作した時
        case 21:
            if (sita_po > -3) {
                for (int i = sita_po; i >= -3; i = i - 3) { sita_po = sita_po - 3; InvalidateRect(hWnd, NULL, TRUE); UpdateWindow(hWnd); } }
            beat = 0;
            break;
        case 22:
            if (sita_po > 25) { for (int i = sita_po; i > 25;  i = i - 2) { sita_po = sita_po - 2; InvalidateRect(hWnd, NULL, TRUE); UpdateWindow(hWnd); } }
            else if (sita_po < 24) { for (int i = sita_po; i < 24; i = i + 2) { sita_po = sita_po + 2; InvalidateRect(hWnd, NULL, TRUE); UpdateWindow(hWnd); } }
            beat = 1;
            break;
        case 23:
            if (sita_po > 45) { for (int i = sita_po; i > 45;  i = i - 2) { sita_po = sita_po - 2; InvalidateRect(hWnd, NULL, TRUE); UpdateWindow(hWnd); } }
            else if (sita_po < 44) { for (int i = sita_po; i < 44; i = i + 2) { sita_po = sita_po + 2; InvalidateRect(hWnd, NULL, TRUE); UpdateWindow(hWnd); } }
            beat = 2;
            break;
        case 24:
            if (sita_po > 65) { for (int i = sita_po; i > 65;  i = i - 2) { sita_po = sita_po - 2; InvalidateRect(hWnd, NULL, TRUE); UpdateWindow(hWnd); } }
            else if (sita_po < 64) { for (int i = sita_po; i < 64; i = i + 2) { sita_po = sita_po + 2; InvalidateRect(hWnd, NULL, TRUE); UpdateWindow(hWnd); } }
            beat = 3;
            break;
        case 25:
            if (sita_po > 85) { for (int i = sita_po; i > 85;  i = i - 2) { sita_po = sita_po - 2; InvalidateRect(hWnd, NULL, TRUE); UpdateWindow(hWnd); } }
            else if (sita_po < 84) { for (int i = sita_po; i < 84; i = i + 2) { sita_po = sita_po + 2; InvalidateRect(hWnd, NULL, TRUE); UpdateWindow(hWnd); } }
            beat = 4;
            break;
        case 26:
            if (sita_po > 105) { for (int i = sita_po; i > 105;  i = i - 2) { sita_po = sita_po - 2; InvalidateRect(hWnd, NULL, TRUE); UpdateWindow(hWnd); } }
            else if (sita_po < 104) { for (int i = sita_po; i < 104; i = i + 2) { sita_po = sita_po + 2; InvalidateRect(hWnd, NULL, TRUE); UpdateWindow(hWnd); } }
            beat = 5;
            break;
        case 27:
            if (sita_po > 125) { for (int i = sita_po; i > 125;  i = i - 2) { sita_po = sita_po - 2; InvalidateRect(hWnd, NULL, TRUE); UpdateWindow(hWnd); } }
            else if (sita_po < 124) { for (int i = sita_po; i < 124; i = i + 2) { sita_po = sita_po + 2; InvalidateRect(hWnd, NULL, TRUE); UpdateWindow(hWnd); } }
            beat = 6;
            break;
        case 28:
            if (sita_po > 145) { for (int i = sita_po; i > 145;  i = i - 2) { sita_po = sita_po - 2; InvalidateRect(hWnd, NULL, TRUE); UpdateWindow(hWnd); } }
            else if (sita_po < 144) { for (int i = sita_po; i < 144; i = i + 2) { sita_po = sita_po + 2; InvalidateRect(hWnd, NULL, TRUE); UpdateWindow(hWnd); } }
            beat = 7;
            break;
        case 29:
            if (sita_po > 165) { for (int i = sita_po; i > 165;  i = i - 2) { sita_po = sita_po - 2; InvalidateRect(hWnd, NULL, TRUE); UpdateWindow(hWnd); } }
            else if (sita_po < 164) { for (int i = sita_po; i < 164; i = i + 2) { sita_po = sita_po + 2; InvalidateRect(hWnd, NULL, TRUE); UpdateWindow(hWnd); } }
            beat = 8;
            break;
        case 30:
            if (migi_po == 55) migi_po = 25; InvalidateRect(hWnd, NULL, TRUE); UpdateWindow(hWnd);
            soundMode = 0;
           break;
        case 31:
            if (migi_po == 25) migi_po = 55; InvalidateRect(hWnd, NULL, TRUE); UpdateWindow(hWnd);
            soundMode = 1;
            break;

        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    
    case WM_HSCROLL:  //トラックバーの操作をした時
    {
        HWND hTrackbar = (HWND)lParam; // 操作されたトラックバーのハンドル
        if (hTrackbar == hNoteLengthBar) {
            noteLength = SendMessage(hNoteLengthBar, TBM_GETPOS, 0, 0);
        }
        if (hTrackbar == hTempoBar) {
            tempo = 120 + SendMessage(hTempoBar, TBM_GETPOS, 0, 0);
            cycleTime = 1000 * 60 / tempo;
        }
        InvalidateRect(hWnd, NULL, TRUE);
        UpdateWindow(hWnd);

        
    }
    break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        // バックバッファ用のメモリDCを作成
        HDC hdcMem = CreateCompatibleDC(hdc);
        HBITMAP hbmMem = CreateCompatibleBitmap(hdc, ps.rcPaint.right - ps.rcPaint.left, ps.rcPaint.bottom - ps.rcPaint.top);
        HGDIOBJ hOld = SelectObject(hdcMem, hbmMem);
        // 背景を白で塗りつぶす
        FillRect(hdcMem, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////画面左側描画
        //楽譜系表示
        //楽譜部分
        int a[4] = { 22, 12, -40, 440 }; //五線譜の {幅, 上からの位置, 左位置, 長さ}
        Rectangle(hdcMem, onpu_pojition[0] + a[2], onpu_pojition[1] + a[1], onpu_pojition[0] + a[3], onpu_pojition[1] + a[1] + 1);
        Rectangle(hdcMem, onpu_pojition[0] + a[2], onpu_pojition[1] + a[1] + a[0], onpu_pojition[0] + a[3], onpu_pojition[1] + a[1] + 1 + a[0]);
        Rectangle(hdcMem, onpu_pojition[0] + a[2], onpu_pojition[1] + a[1] + 2 * a[0], onpu_pojition[0] + a[3], onpu_pojition[1] + a[1] + 1 + 2 * a[0]);
        Rectangle(hdcMem, onpu_pojition[0] + a[2], onpu_pojition[1] + a[1] + 3 * a[0], onpu_pojition[0] + a[3], onpu_pojition[1] + a[1] + 1 + 3 * a[0]);
        Rectangle(hdcMem, onpu_pojition[0] + a[2], onpu_pojition[1] + a[1] + 4 * a[0], onpu_pojition[0] + a[3], onpu_pojition[1] + a[1] + 1 + 4 * a[0]);
        Rectangle(hdcMem, onpu_pojition[0] - 11, onpu_pojition[1] + a[1], onpu_pojition[0] - 10, onpu_pojition[1] + a[1] + 1 + 4 * a[0]);

        //音符部分
        HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
        Graphics onpu(hdcMem);
        Gdiplus::Image image(L"png/onpu.png");

        onpu.DrawImage(&image, onpu_pojition[0], onpu_pojition[1], image.GetWidth(), image.GetHeight());
        SelectObject(hdcMem, hBrush);
        Rectangle(hdcMem, onpu_pojition[0] + 22, onpu_pojition[1] - 10, onpu_pojition[0] + (noteLength / 6) + 22 + 4, onpu_pojition[1]);
        DeleteObject(hBrush);
        onpu.DrawImage(&image, onpu_pojition[0] + (noteLength / 6), onpu_pojition[1], image.GetWidth(), image.GetHeight());

        onpu.DrawImage(&image, onpu_pojition[0] + (1000 / 6), onpu_pojition[1], image.GetWidth(), image.GetHeight());
        SelectObject(hdcMem, hBrush);
        Rectangle(hdcMem, onpu_pojition[0] + 22 + (1000 / 6), onpu_pojition[1] - 10, onpu_pojition[0] + (noteLength / 6) + (1000 / 6) + 22 + 4, onpu_pojition[1]);
        DeleteObject(hBrush);
        onpu.DrawImage(&image, onpu_pojition[0] + (noteLength / 6) + (1000 / 6), onpu_pojition[1], image.GetWidth(), image.GetHeight());

        Graphics namisen(hdcMem);
        Gdiplus::Image image2(L"png/namisen.png");
        namisen.DrawImage(&image2, onpu_pojition[0] + (1000 / 6) * 2 + 50, onpu_pojition[1] + 2 * a[0], image2.GetWidth(), image2.GetHeight());


        //テキストの表示
        SelectObject(hdcMem, hFont);//フォントの設定
        std::wstringstream barValue;
        barValue << L"Timing: " << noteLength / 10;
        std::wstring text = barValue.str();
        TextOut(hdcMem, TB[0] + 100, TB[1] + 35, text.c_str(), text.length());


        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////画面右側描画
        //文字部分描画
        Rectangle(hdcMem, Beat[0], Beat[1], Beat[2], Beat[3]);
        TextOut(hdcMem, (Beat[2] - Beat[0] + Beat[4]) / 2 + Beat[0]- 23, Beat[1] - 13, L"Beat", 4);
        Rectangle(hdcMem, Beat[0] + Beat[4], Beat[1], Beat[2] + Beat[4], Beat[3]);

        Rectangle(hdcMem, ST[0], ST[1], ST[2], ST[3]);
        TextOut(hdcMem, (ST[2] - ST[0] + ST[4]) / 2 + ST[0] - 55, ST[1] - 13, L"SoundType", 9);
        Rectangle(hdcMem, ST[0] + ST[4], ST[1], ST[2] + ST[4], ST[3]);

        SelectObject(hdcMem, hFontS);//フォントの設定
        std::wstringstream tempoValue;
        tempoValue << L"Tempo: " << tempo;
        std::wstring text4 = tempoValue.str();
        TextOut(hdcMem, TpB[0], TpB[1] - 25, text4.c_str(), text4.length());


        //下・右向きの矢印画像の表示
        Graphics sita(hdcMem);
        Gdiplus::Image image3(L"png/sita.png");
        sita.DrawImage(&image3, Beat[0] + sita_po, Beat[1] + 20, image3.GetWidth()/2, image3.GetHeight()/2);
        Graphics migi(hdcMem);
        Gdiplus::Image image4(L"png/migi.png");
        migi.DrawImage(&image4, ST[0],ST[1] + migi_po, image4.GetWidth()/2, image4.GetHeight()/2);

        

        // ダブルバッファリング用
        // バックバッファから画面にコピー
        BitBlt(hdc, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - ps.rcPaint.left, ps.rcPaint.bottom - ps.rcPaint.top, hdcMem, 0, 0, SRCCOPY);
        // リソースの解放
        SelectObject(hdcMem, hOld);
        DeleteObject(hbmMem);
        DeleteDC(hdcMem);


        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        DeleteObject(hFont);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////音を再生する関数




void Play() {

    sf::Music sound1;
    sf::Music sound2;
    sf::Music sound3;
    sf::Music sound4;
    sf::Music sound5;
    sf::Music sound6;
    sf::Music sound7;
    sf::Music sound8;
    sf::Music sound9;

    sound1.openFromFile("sound/guaid1.wav");
    sound2.openFromFile("sound/guaid2.wav");
    sound3.openFromFile("sound/snare2.wav");
    sound4.openFromFile("sound/snare4.wav");
    sound5.openFromFile("sound/kick2.wav");
    sound6.openFromFile("sound/click1.wav");
    sound7.openFromFile("sound/click2.wav");
    sound8.openFromFile("sound/click1+guaid1.wav");
    sound9.openFromFile("sound/click1+guaid2.wav");


    while (play) {

    int a = cycleTime * noteLength / 1000;
    int b = cycleTime - a;

    if (soundMode == 0 && beat == 0) {
        sound3.play();
        sound5.play();
        Sleep(a); // 待機
        sound4.play();
        Sleep(b); // 待機
    }
    else if (soundMode == 0 && beat != 0) {
        sound1.play();
        sound3.play();
        sound5.play();
        Sleep(a); // 待機
        sound4.play();
        Sleep(b); // 待機
        for (int i = 0; i < beat - 1; i++) {
            sound2.play();
            sound3.play();
            sound5.play();
            Sleep(a); // 待機
            sound4.play();
            Sleep(b); // 待機
        }
    }
    else if (soundMode == 1 && beat == 0) {
        sound6.play();
        Sleep(a); // 待機
        sound7.play();
        Sleep(b); // 待機
    }
    else if (soundMode == 1 && beat != 0) {
        sound8.play();
        Sleep(a); // 待機
        sound7.play();
        Sleep(b); // 待機
        for (int i = 0; i < beat - 1; i++) {
            sound9.play();
            Sleep(a); // 待機
            sound7.play();
            Sleep(b); // 待機
        }
    }


    }
    std::this_thread::sleep_for(std::chrono::seconds(10));

    return;
}

