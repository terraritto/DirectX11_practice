#define STRICT //型チェックを厳密に行う
#define WIN32_LEAN_AND_MEAN //ヘッダーからあまり使われない関数を省く
#define SAFE_RELEASE(x) {if(x) {(x)->Release(); (x)=NULL;}}

#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <D3Dcompiler.h>
#include <DxErr.h>
#include <D3DX10.h>

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dx10.lib")
#pragma comment(lib,"d3dx11.lib")
#pragma comment(lib,"d3dCompiler.lib")
#pragma comment(lib,"dxerr.lib")

//定数定義
const LPCSTR NAME = "ウィンドウ";//このアプリケーションのクラス名
const short WINDOW_WIDTH = 640; //ウィンドウ幅
const short WINDOW_HEIGHT = 480; //ウィンドウ高さ

//グローバル変数
HWND g_hWnd = NULL; //メインウィンドウのウィンドウハンドル
ID3D11Device *g_pDevice; //デバイス
ID3D11DeviceContext *g_pDeviceContext; //デバイス・コンテキスト
IDXGISwapChain *g_pSwapChain; //スワップ・チェイン
ID3D11RenderTargetView *g_pRTV; //描画ターゲットビュー
ID3D11Texture2D *g_pDS; //深度/ステンシル・テクスチャを受け取る変数
ID3D11DepthStencilView *g_pDSV; //深度/ステンシル・ビュー

//関数プロトタイプ宣言
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); //ウィンドウプロシージャ関数
void App();
HRESULT InitDirect3D();
void ReleaseDirect3D();
bool CheckDevice(ID3D11Device *g_pDevice);

//
//エントリー関数
//

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR szStr, INT iCmdShow) 
{
	//ウィンドウ作成
	WNDCLASSEX wc = { 0 }; 
	wc.cbSize = sizeof(WNDCLASSEX); 
	wc.style = 0; 
	wc.lpfnWndProc = WndProc; 
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;
	wc.hIcon = (HICON)LoadImage(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_SHARED);
	wc.hCursor = (HCURSOR)LoadImage(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = NAME;
	wc.hIconSm = (HICON)LoadImage(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_SHARED);

	if (!RegisterClassEx(&wc)) { return 1; }
	g_hWnd = CreateWindow(NAME, NAME, WS_OVERLAPPEDWINDOW, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, hInst, 0);
	ShowWindow(g_hWnd, SW_SHOW);
	UpdateWindow(g_hWnd);
	
	//Direct3Dの作成
	if (FAILED(InitDirect3D())) { return 1; };

	//メッセージループ
	MSG msg = { 0 };//メッセージキューから取得したメッセージを格納
	while (TRUE)
	{
		//キューにメッセージがあればいつもの処理
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))//PeekMessageでメッセージを取得
		{
			//メッセージがWM_QUITならループを抜ける
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			//取得したメッセージを適切なウィンドウプロシージャに渡す
		}
		//キューにメッセージがなければ随時処理
		else {
			App();
		}

		CheckDevice(g_pDevice); //デバイスのチェック
	}

	ReleaseDirect3D(); //Direct3Dを解放する

	return 0;
}

//
//ウィンドウプロシージャ関数
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

HRESULT InitDirect3D()
{
	//機能レベルを定義
	//機能レベルはGPUがサポートする機能セットの厳密な定義
	//とりあえず今回勉強したいのはDirectX11なので11で設定
	D3D_FEATURE_LEVEL pFeatureLevels = D3D_FEATURE_LEVEL_11_0;
	D3D_FEATURE_LEVEL *pFeatureLevel = NULL;

	//デバイスとスワップチェーンの作成
	//スワップチェインはフロント・バックバッファを含むバッファの集まりとその切り替えをする仕組み
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd)); //構造体の値を初期化(必要な場合)
	sd.BufferCount = 1; //スワップ・チェイン内のバック・バッファの数
	sd.BufferDesc.Width = WINDOW_WIDTH; //バック・バッファの幅
	sd.BufferDesc.Height = WINDOW_HEIGHT; //バック・バッファの高さ
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //ディスプレイ・フォーマット,色々あるけど必要なときに学べばよい
	sd.BufferDesc.RefreshRate.Numerator = 60; //リフレッシュレート(Hz)単位 分母
	sd.BufferDesc.RefreshRate.Denominator = 1; //リフレッシュレート(Hz)単位 分子
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; //スキャン・ライン描画モードの設定 とりあえずはUNSPECIFIEDを設定しとけばよい
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; //スケーリング・モード,とりあえず今は表示領域は問題ないのでUNSPECIFIED
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; //バック・バッファの使用法 Direct3D描画先として使うならTARGET_OUTPUT
	sd.OutputWindow = g_hWnd; //関連付けるウィンドウ
	sd.SampleDesc.Count = 1; //マルチ・サンプルの数 アンチエイリアス関連
	sd.SampleDesc.Quality = 0; //マルチ・サンプルのクオリティ 小→高パフォーマンス低クオリティ 大→高クオリティ低パフォーマンス
	sd.Windowed = TRUE; //ウィンドウモード→TRUE フルスクリーンモード→FALSE
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; //作成するスワップ・チェインの持つ機能を設定 今回は自動切換えに設定

	//関数を利用しデバイス,デバイス・コンテキスト,スワップ・チェインを同時に作る
	D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &pFeatureLevels,
		1, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pDevice, pFeatureLevel, &g_pDeviceContext);

	//バックバッファのレンダーターゲットビュー(RTV)を作成
	//簡単に言うとスワップチェインからバックバッファを取得
	ID3D11Texture2D *pBack;//バッファのアクセスに使うインターフェース
	g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBack);
	
	//バックバッファの描画ターゲット・ビューを作る
	g_pDevice->CreateRenderTargetView(pBack, NULL, &g_pRTV);
	//以降、バックバッファは直接使わないので解放
	pBack->Release();

	//デプスステンシルビュー(DSV)を作成
	//隠面処理関連
	//深度/ステンシル・テクスチャの作成
	D3D11_TEXTURE2D_DESC descDepth; 
	descDepth.Width = WINDOW_WIDTH; //幅
	descDepth.Height = WINDOW_HEIGHT; //高さ
	descDepth.MipLevels = 1; //ミップマップレベル数
	descDepth.ArraySize = 1; //配列サイズ
	descDepth.Format = DXGI_FORMAT_D32_FLOAT; //フォーマット(深度のみ)
	descDepth.SampleDesc.Count = 1; //マルチサンプリング個数
	descDepth.SampleDesc.Quality = 0; //マルチサンプリングの品質
	descDepth.Usage = D3D11_USAGE_DEFAULT; //デフォルト使用法
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL; //深度/ステンシルとして使用
	descDepth.CPUAccessFlags = 0; //CPUからはアクセスしない
	descDepth.MiscFlags = 0; //その他の設定なし
	g_pDevice->CreateTexture2D(&descDepth, NULL, &g_pDS);

	//深度/ステンシルビューの作成
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = descDepth.Format; //ビューのフォーマット
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Flags = 0;
	descDSV.Texture2D.MipSlice = 0;

	g_pDevice->CreateDepthStencilView(g_pDS, &descDSV, &g_pDSV);

	//ビューポートの設定
	//描画ターゲットの描画領域に関する設定
	D3D11_VIEWPORT vp; //ビューポートの設定
	vp.Width = WINDOW_WIDTH; //ビューポート領域の幅
	vp.Height = WINDOW_HEIGHT; //ビューポート領域の高さ
	vp.MinDepth = 0.0f; //ビューポート領域の深度値の最小値
	vp.MaxDepth = 1.0f; //ビューポート領域の深度値の最大値
	vp.TopLeftX = 0; //ビューポート領域の左上X座標
	vp.TopLeftY = 0; //ビューポート領域の左上Y座標
	//ラスタライザにビューポートを設定
	g_pDeviceContext->RSSetViewports(1, &vp);

	//レンダーターゲットビューとデプスステンシルビューをセット
	//描画ターゲット・ビューを出力マネージャーの描画ターゲットとして設定
	g_pDeviceContext->OMSetRenderTargets(1, &g_pRTV, g_pDSV);

	return S_OK;
}

//
//
//
void App()
{
	//描画ターゲットのクリア
	float ClearColor[4] = { 0.0f,0.5f,0.5f,1.0f }; //クリア色作成 RGBAの順
	g_pDeviceContext->ClearRenderTargetView(g_pRTV, ClearColor);//カラーバッファクリア
	
	//深度/ステンシル値のクリア
	g_pDeviceContext->ClearDepthStencilView(g_pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);//デプスステンシルバッファクリア
	
	//描画(省略)
	
	//バック・バッファの表示
	g_pSwapChain->Present(0, 0);
}

//Direct3Dの解放
void ReleaseDirect3D() {
	//デバイス・ステートのクリア
	if (g_pDeviceContext) 
	{
		g_pDeviceContext->ClearState();
	}

	//取得したインターフェースの解放
	SAFE_RELEASE(g_pRTV);
	SAFE_RELEASE(g_pSwapChain);
	SAFE_RELEASE(g_pDeviceContext);
	SAFE_RELEASE(g_pDevice);
}


//デバイスの消失
bool CheckDevice(ID3D11Device *g_pDevice) 
{
	HRESULT hr = g_pDevice->GetDeviceRemovedReason();

	switch (hr) 
	{
	case S_OK:
		break; //正常

	case DXGI_ERROR_DEVICE_HUNG:
	case DXGI_ERROR_DEVICE_RESET:
		ReleaseDirect3D(); //direct3Dの解放(アプリケーションの定義)
		hr = InitDirect3D(); //direct3Dの初期化(アプリケーションの初期化)
		if (FAILED(hr)) 
		{
			return false; //失敗 アプリケーションの終了
		}
		break;

	case DXGI_ERROR_DEVICE_REMOVED:
	case DXGI_ERROR_INVALID_CALL:
	default:
		return false; //どうしようもないので、アプリケーションを終了
	};
}
