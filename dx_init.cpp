#define STRICT //�^�`�F�b�N�������ɍs��
#define WIN32_LEAN_AND_MEAN //�w�b�_�[���炠�܂�g���Ȃ��֐����Ȃ�
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

//�萔��`
const LPCSTR NAME = "�E�B���h�E";//���̃A�v���P�[�V�����̃N���X��
const short WINDOW_WIDTH = 640; //�E�B���h�E��
const short WINDOW_HEIGHT = 480; //�E�B���h�E����

//�O���[�o���ϐ�
HWND g_hWnd = NULL; //���C���E�B���h�E�̃E�B���h�E�n���h��
ID3D11Device *g_pDevice; //�f�o�C�X
ID3D11DeviceContext *g_pDeviceContext; //�f�o�C�X�E�R���e�L�X�g
IDXGISwapChain *g_pSwapChain; //�X���b�v�E�`�F�C��
ID3D11RenderTargetView *g_pRTV; //�`��^�[�Q�b�g�r���[
ID3D11Texture2D *g_pDS; //�[�x/�X�e���V���E�e�N�X�`�����󂯎��ϐ�
ID3D11DepthStencilView *g_pDSV; //�[�x/�X�e���V���E�r���[

//�֐��v���g�^�C�v�錾
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); //�E�B���h�E�v���V�[�W���֐�
void App();
HRESULT InitDirect3D();
void ReleaseDirect3D();
bool CheckDevice(ID3D11Device *g_pDevice);

//
//�G���g���[�֐�
//

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR szStr, INT iCmdShow) 
{
	//�E�B���h�E�쐬
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
	
	//Direct3D�̍쐬
	if (FAILED(InitDirect3D())) { return 1; };

	//���b�Z�[�W���[�v
	MSG msg = { 0 };//���b�Z�[�W�L���[����擾�������b�Z�[�W���i�[
	while (TRUE)
	{
		//�L���[�Ƀ��b�Z�[�W������΂����̏���
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))//PeekMessage�Ń��b�Z�[�W���擾
		{
			//���b�Z�[�W��WM_QUIT�Ȃ烋�[�v�𔲂���
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			//�擾�������b�Z�[�W��K�؂ȃE�B���h�E�v���V�[�W���ɓn��
		}
		//�L���[�Ƀ��b�Z�[�W���Ȃ���ΐ�������
		else {
			App();
		}

		CheckDevice(g_pDevice); //�f�o�C�X�̃`�F�b�N
	}

	ReleaseDirect3D(); //Direct3D���������

	return 0;
}

//
//�E�B���h�E�v���V�[�W���֐�
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_DESTROY://�����I��������Ax�{�^�����������
		PostQuitMessage(0);//�I��
		break;
	}

	return DefWindowProc(hWnd, iMsg, wParam, lParam);
	//���̖ʓ|�ȏ���������Ă����
	//�܂������Ƃ�
}

HRESULT InitDirect3D()
{
	//�@�\���x�����`
	//�@�\���x����GPU���T�|�[�g����@�\�Z�b�g�̌����Ȓ�`
	//�Ƃ肠��������׋��������̂�DirectX11�Ȃ̂�11�Őݒ�
	D3D_FEATURE_LEVEL pFeatureLevels = D3D_FEATURE_LEVEL_11_0;
	D3D_FEATURE_LEVEL *pFeatureLevel = NULL;

	//�f�o�C�X�ƃX���b�v�`�F�[���̍쐬
	//�X���b�v�`�F�C���̓t�����g�E�o�b�N�o�b�t�@���܂ރo�b�t�@�̏W�܂�Ƃ��̐؂�ւ�������d�g��
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd)); //�\���̂̒l��������(�K�v�ȏꍇ)
	sd.BufferCount = 1; //�X���b�v�E�`�F�C�����̃o�b�N�E�o�b�t�@�̐�
	sd.BufferDesc.Width = WINDOW_WIDTH; //�o�b�N�E�o�b�t�@�̕�
	sd.BufferDesc.Height = WINDOW_HEIGHT; //�o�b�N�E�o�b�t�@�̍���
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //�f�B�X�v���C�E�t�H�[�}�b�g,�F�X���邯�ǕK�v�ȂƂ��Ɋw�ׂ΂悢
	sd.BufferDesc.RefreshRate.Numerator = 60; //���t���b�V�����[�g(Hz)�P�� ����
	sd.BufferDesc.RefreshRate.Denominator = 1; //���t���b�V�����[�g(Hz)�P�� ���q
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; //�X�L�����E���C���`�惂�[�h�̐ݒ� �Ƃ肠������UNSPECIFIED��ݒ肵�Ƃ��΂悢
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; //�X�P�[�����O�E���[�h,�Ƃ肠�������͕\���̈�͖��Ȃ��̂�UNSPECIFIED
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; //�o�b�N�E�o�b�t�@�̎g�p�@ Direct3D�`���Ƃ��Ďg���Ȃ�TARGET_OUTPUT
	sd.OutputWindow = g_hWnd; //�֘A�t����E�B���h�E
	sd.SampleDesc.Count = 1; //�}���`�E�T���v���̐� �A���`�G�C���A�X�֘A
	sd.SampleDesc.Quality = 0; //�}���`�E�T���v���̃N�I���e�B �������p�t�H�[�}���X��N�I���e�B �偨���N�I���e�B��p�t�H�[�}���X
	sd.Windowed = TRUE; //�E�B���h�E���[�h��TRUE �t���X�N���[�����[�h��FALSE
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; //�쐬����X���b�v�E�`�F�C���̎��@�\��ݒ� ����͎����؊����ɐݒ�

	//�֐��𗘗p���f�o�C�X,�f�o�C�X�E�R���e�L�X�g,�X���b�v�E�`�F�C���𓯎��ɍ��
	D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &pFeatureLevels,
		1, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pDevice, pFeatureLevel, &g_pDeviceContext);

	//�o�b�N�o�b�t�@�̃����_�[�^�[�Q�b�g�r���[(RTV)���쐬
	//�ȒP�Ɍ����ƃX���b�v�`�F�C������o�b�N�o�b�t�@���擾
	ID3D11Texture2D *pBack;//�o�b�t�@�̃A�N�Z�X�Ɏg���C���^�[�t�F�[�X
	g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBack);
	
	//�o�b�N�o�b�t�@�̕`��^�[�Q�b�g�E�r���[�����
	g_pDevice->CreateRenderTargetView(pBack, NULL, &g_pRTV);
	//�ȍ~�A�o�b�N�o�b�t�@�͒��ڎg��Ȃ��̂ŉ��
	pBack->Release();

	//�f�v�X�X�e���V���r���[(DSV)���쐬
	//�B�ʏ����֘A
	//�[�x/�X�e���V���E�e�N�X�`���̍쐬
	D3D11_TEXTURE2D_DESC descDepth; 
	descDepth.Width = WINDOW_WIDTH; //��
	descDepth.Height = WINDOW_HEIGHT; //����
	descDepth.MipLevels = 1; //�~�b�v�}�b�v���x����
	descDepth.ArraySize = 1; //�z��T�C�Y
	descDepth.Format = DXGI_FORMAT_D32_FLOAT; //�t�H�[�}�b�g(�[�x�̂�)
	descDepth.SampleDesc.Count = 1; //�}���`�T���v�����O��
	descDepth.SampleDesc.Quality = 0; //�}���`�T���v�����O�̕i��
	descDepth.Usage = D3D11_USAGE_DEFAULT; //�f�t�H���g�g�p�@
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL; //�[�x/�X�e���V���Ƃ��Ďg�p
	descDepth.CPUAccessFlags = 0; //CPU����̓A�N�Z�X���Ȃ�
	descDepth.MiscFlags = 0; //���̑��̐ݒ�Ȃ�
	g_pDevice->CreateTexture2D(&descDepth, NULL, &g_pDS);

	//�[�x/�X�e���V���r���[�̍쐬
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = descDepth.Format; //�r���[�̃t�H�[�}�b�g
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Flags = 0;
	descDSV.Texture2D.MipSlice = 0;

	g_pDevice->CreateDepthStencilView(g_pDS, &descDSV, &g_pDSV);

	//�r���[�|�[�g�̐ݒ�
	//�`��^�[�Q�b�g�̕`��̈�Ɋւ���ݒ�
	D3D11_VIEWPORT vp; //�r���[�|�[�g�̐ݒ�
	vp.Width = WINDOW_WIDTH; //�r���[�|�[�g�̈�̕�
	vp.Height = WINDOW_HEIGHT; //�r���[�|�[�g�̈�̍���
	vp.MinDepth = 0.0f; //�r���[�|�[�g�̈�̐[�x�l�̍ŏ��l
	vp.MaxDepth = 1.0f; //�r���[�|�[�g�̈�̐[�x�l�̍ő�l
	vp.TopLeftX = 0; //�r���[�|�[�g�̈�̍���X���W
	vp.TopLeftY = 0; //�r���[�|�[�g�̈�̍���Y���W
	//���X�^���C�U�Ƀr���[�|�[�g��ݒ�
	g_pDeviceContext->RSSetViewports(1, &vp);

	//�����_�[�^�[�Q�b�g�r���[�ƃf�v�X�X�e���V���r���[���Z�b�g
	//�`��^�[�Q�b�g�E�r���[���o�̓}�l�[�W���[�̕`��^�[�Q�b�g�Ƃ��Đݒ�
	g_pDeviceContext->OMSetRenderTargets(1, &g_pRTV, g_pDSV);

	return S_OK;
}

//
//
//
void App()
{
	//�`��^�[�Q�b�g�̃N���A
	float ClearColor[4] = { 0.0f,0.5f,0.5f,1.0f }; //�N���A�F�쐬 RGBA�̏�
	g_pDeviceContext->ClearRenderTargetView(g_pRTV, ClearColor);//�J���[�o�b�t�@�N���A
	
	//�[�x/�X�e���V���l�̃N���A
	g_pDeviceContext->ClearDepthStencilView(g_pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);//�f�v�X�X�e���V���o�b�t�@�N���A
	
	//�`��(�ȗ�)
	
	//�o�b�N�E�o�b�t�@�̕\��
	g_pSwapChain->Present(0, 0);
}

//Direct3D�̉��
void ReleaseDirect3D() {
	//�f�o�C�X�E�X�e�[�g�̃N���A
	if (g_pDeviceContext) 
	{
		g_pDeviceContext->ClearState();
	}

	//�擾�����C���^�[�t�F�[�X�̉��
	SAFE_RELEASE(g_pRTV);
	SAFE_RELEASE(g_pSwapChain);
	SAFE_RELEASE(g_pDeviceContext);
	SAFE_RELEASE(g_pDevice);
}


//�f�o�C�X�̏���
bool CheckDevice(ID3D11Device *g_pDevice) 
{
	HRESULT hr = g_pDevice->GetDeviceRemovedReason();

	switch (hr) 
	{
	case S_OK:
		break; //����

	case DXGI_ERROR_DEVICE_HUNG:
	case DXGI_ERROR_DEVICE_RESET:
		ReleaseDirect3D(); //direct3D�̉��(�A�v���P�[�V�����̒�`)
		hr = InitDirect3D(); //direct3D�̏�����(�A�v���P�[�V�����̏�����)
		if (FAILED(hr)) 
		{
			return false; //���s �A�v���P�[�V�����̏I��
		}
		break;

	case DXGI_ERROR_DEVICE_REMOVED:
	case DXGI_ERROR_INVALID_CALL:
	default:
		return false; //�ǂ����悤���Ȃ��̂ŁA�A�v���P�[�V�������I��
	};
}
