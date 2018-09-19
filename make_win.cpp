#include <windows.h>

//�萔��`
const LPCSTR NAME = "�E�B���h�E";//���̃A�v���P�[�V�����̃N���X��
const short WINDOW_WIDTH = 640; //�E�B���h�E��
const short WINDOW_HEIGHT = 480; //�E�B���h�E����

//�O���[�o���ϐ�
HWND g_hWnd = NULL; //���C���E�B���h�E�̃E�B���h�E�n���h��

//�֐��v���g�^�C�v�錾
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); //�E�B���h�E�v���V�[�W���֐�
void App();

//
//�G���g���[�֐�
//

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR szStr, INT iCmdShow) {
	//�E�B���h�E�쐬
	WNDCLASSEX wc = { 0 }; //�E�B���h�E�N���X�o�^�p�̍\����
	wc.cbSize = sizeof(WNDCLASSEX); //�\���̂̃T�C�Y
	//�T�C�Y�͐��sizeof(WNDCLASSEX)�ɂ���!!
	wc.style = 0; //�X�^�C���A�Ȃ񂩐F�X���邯�ǂƂ肠�����͂���Ȃ��̂�0
	wc.lpfnWndProc = WndProc; //�E�B���h�E�v���V�[�W���֐��ւ̃|�C���^
	//�E�B���h�E�v���V�[�W��->���b�Z�[�W�����������p�̃��[�`��
	//���b�Z�[�W���[�v�Ŏ擾�������b�Z�[�W���E�B���h�E�v���V�[�W���Ƀf�B�X�p�b�`��
	//�󂯎�������b�Z�[�W���E�B���h�E�v���V�[�W���ŏ���
	//WndProc�ŏ����̂ł��������Q��
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;//�⏕��������ǉ����邽�߂̃T�C�Y���w��
	//�⏕������->�f�[�^�\���̖̂����ɒǉ����郁�����̂��ƂŁA �����Ŏw�肵���T�C�Y���̃������������Ŋm�ۂ���邱�ƂɂȂ�܂�
	//cbClsExtra->�V�X�e���������ŊǗ�����WNDCLASS�\���̂ɒǉ����郁�����̃T�C�Y
	//cbWndExtra->�E�C���h�E��\�������f�[�^�\���̂ɒǉ����郁����
	//���̓����f�[�^�\���̂Ƃ����̂�HWND�^�Ŏ��ʂ����
	wc.hInstance = hInst;//�C���X�^���X�n���h���ƃE�C���h�E�N���X���֘A�t����d�g��
	//hInstance�́AWinMain�̑�1�������w��
	wc.hIcon = (HICON)LoadImage(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_SHARED);
	//����̃A�C�R����ς����A�ς������Ƃ��Ɋw�ׂ΂����̂ō��͖���
	wc.hCursor = (HCURSOR)LoadImage(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
	//�J�[�\���ς����A�ς������Ƃ��Ɋw�ׂ΂����̂ō��͖���
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	//�E�B���h�E�̔w�i�F
	wc.lpszMenuName = NULL; //���j���[�֘A�ɂ��Ă̓z�A����͖���
	wc.lpszClassName = NAME; //�E�B���h�E�N���X�����ʂ��邽�߂̕�����
	wc.hIconSm = (HICON)LoadImage(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_SHARED);
	//�ŏ������Ƀ^�X�N�o�[�ŕ\�������A�C�R��
	//hIcon�Ɠ���������

	if (!RegisterClassEx(&wc)) { return 1; }
	//RegisterClassEx�֐��ɂ��E�B���h�E�̍쐬�Ɏg�p�ł���E�B���h�E�N���X��o�^����
	g_hWnd = CreateWindow(NAME, NAME, WS_OVERLAPPEDWINDOW, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, hInst, 0);
	//HWND�ւ̓o�^(�E�B���h�E�N���X��,�E�B���h�E��,�ʒux���W,�ʒuy���W,�T�C�YX,�T�C�YY,�e�E�B���h�E�̃n���h��,���j���[�̃n���h��,�C���X�^���X�n���h��,lPVOID)
	ShowWindow(g_hWnd, SW_SHOW);//(�ΏۂƂȂ�n���h��,SW_SHOW�ŕ\��)
	UpdateWindow(g_hWnd);//�ĕ`�施��

	//���b�Z�[�W���[�v
	MSG msg = { 0 };//���b�Z�[�W�L���[����擾�������b�Z�[�W���i�[
	while (TRUE)
	{
		//�L���[�Ƀ��b�Z�[�W������΂����̏���
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))//PeekMessage�Ń��b�Z�[�W���擾
		//(msg�̍\���̂̃|�C���^,���b�Z�[�W���󂯎��E�B���h�E�̃n���h��,�󂯎�郁�b�Z�[�W�̍ŏ��l,�ő�l,���b�Z�[�W�̏����@)
		//PM_NOREMOVE->PeekMessage �֐������b�Z�[�W������������A���̃��b�Z�[�W���L���[����폜���܂���
		//PM_REMOVE�@->PeekMessage �֐������b�Z�[�W������������A���̃��b�Z�[�W���L���[����폜���܂�
		{
			//���b�Z�[�W��WM_QUIT�Ȃ烋�[�v�𔲂���
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			//�L�[�{�[�h���͂������͉̂��z�L�[���b�Z�[�W�Ƃ��ăV�X�e���ɓ`�����邪�A����ł͉��̕�����������Ȃ��B
			//����𕪂��镶���ɂ��邽�߂�TranslateMessage�𗘗p����
			DispatchMessage(&msg);
			//�擾�������b�Z�[�W��K�؂ȃE�B���h�E�v���V�[�W���ɓn��
		}
		//�L���[�Ƀ��b�Z�[�W���Ȃ���ΐ�������
		else {
			App();
		}
	}
	return 0;
}

//
//�E�B���h�E�v���V�[�W���֐�
//hWnd->���b�Z�[�W�����������E�B���h�E�̃n���h�����󂯎��
//iMsg->���b�Z�[�W������
//wParam,lParam->���b�Z�[�W�̕t����񂪓���
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

//
//
//
void App()
{

}