#include <windows.h>

//定数定義
const LPCSTR NAME = "ウィンドウ";//このアプリケーションのクラス名
const short WINDOW_WIDTH = 640; //ウィンドウ幅
const short WINDOW_HEIGHT = 480; //ウィンドウ高さ

//グローバル変数
HWND g_hWnd = NULL; //メインウィンドウのウィンドウハンドル

//関数プロトタイプ宣言
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); //ウィンドウプロシージャ関数
void App();

//
//エントリー関数
//

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR szStr, INT iCmdShow) {
	//ウィンドウ作成
	WNDCLASSEX wc = { 0 }; //ウィンドウクラス登録用の構造体
	wc.cbSize = sizeof(WNDCLASSEX); //構造体のサイズ
	//サイズは絶対sizeof(WNDCLASSEX)にする!!
	wc.style = 0; //スタイル、なんか色々あるけどとりあえずはいらないので0
	wc.lpfnWndProc = WndProc; //ウィンドウプロシージャ関数へのポインタ
	//ウィンドウプロシージャ->メッセージを処理する専用のルーチン
	//メッセージループで取得したメッセージをウィンドウプロシージャにディスパッチし
	//受け取ったメッセージをウィンドウプロシージャで処理
	//WndProcで書くのでそっちを参照
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;//補助メモリを追加するためのサイズを指定
	//補助メモリ->データ構造体の末尾に追加するメモリのことで、 ここで指定したサイズ分のメモリが内部で確保されることになります
	//cbClsExtra->システムが内部で管理するWNDCLASS構造体に追加するメモリのサイズ
	//cbWndExtra->ウインドウを表す内部データ構造体に追加するメモリ
	//この内部データ構造体というのはHWND型で識別される
	wc.hInstance = hInst;//インスタンスハンドルとウインドウクラスを関連付ける仕組み
	//hInstanceは、WinMainの第1引数を指定
	wc.hIcon = (HICON)LoadImage(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_SHARED);
	//左上のアイコンを変えるやつ、変えたいときに学べばいいので今は無視
	wc.hCursor = (HCURSOR)LoadImage(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
	//カーソル変えるやつ、変えたいときに学べばいいので今は無視
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	//ウィンドウの背景色
	wc.lpszMenuName = NULL; //メニュー関連についての奴、今回は無視
	wc.lpszClassName = NAME; //ウィンドウクラスを識別するための文字列
	wc.hIconSm = (HICON)LoadImage(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_SHARED);
	//最小化時にタスクバーで表示されるアイコン
	//hIconと同じ書き方

	if (!RegisterClassEx(&wc)) { return 1; }
	//RegisterClassEx関数によりウィンドウの作成に使用できるウィンドウクラスを登録する
	g_hWnd = CreateWindow(NAME, NAME, WS_OVERLAPPEDWINDOW, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, hInst, 0);
	//HWNDへの登録(ウィンドウクラス名,ウィンドウ名,位置x座標,位置y座標,サイズX,サイズY,親ウィンドウのハンドル,メニューのハンドル,インスタンスハンドル,lPVOID)
	ShowWindow(g_hWnd, SW_SHOW);//(対象となるハンドル,SW_SHOWで表示)
	UpdateWindow(g_hWnd);//再描画命令

	//メッセージループ
	MSG msg = { 0 };//メッセージキューから取得したメッセージを格納
	while (TRUE)
	{
		//キューにメッセージがあればいつもの処理
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))//PeekMessageでメッセージを取得
		//(msgの構造体のポインタ,メッセージを受け取るウィンドウのハンドル,受け取るメッセージの最小値,最大値,メッセージの処理法)
		//PM_NOREMOVE->PeekMessage 関数がメッセージを処理した後、そのメッセージをキューから削除しません
		//PM_REMOVE　->PeekMessage 関数がメッセージを処理した後、そのメッセージをキューから削除します
		{
			//メッセージがWM_QUITならループを抜ける
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			//キーボード入力したものは仮想キーメッセージとしてシステムに伝えられるが、これでは何の文字か分からない。
			//これを分かる文字にするためにTranslateMessageを利用する
			DispatchMessage(&msg);
			//取得したメッセージを適切なウィンドウプロシージャに渡す
		}
		//キューにメッセージがなければ随時処理
		else {
			App();
		}
	}
	return 0;
}

//
//ウィンドウプロシージャ関数
//hWnd->メッセージが発生したウィンドウのハンドルを受け取る
//iMsg->メッセージが入る
//wParam,lParam->メッセージの付加情報が入る
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_DESTROY://閉じるを選択したり、xボタンが押される
		PostQuitMessage(0);//終了
		break;
	}

	return DefWindowProc(hWnd, iMsg, wParam, lParam);
	//他の面倒な処理をやってくれる
	//まあ書いとけ
}

//
//
//
void App()
{

}