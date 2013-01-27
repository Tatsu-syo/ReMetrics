// AppliBase.cpp : �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"
#include "commctrl.h"

#include "DialogAppliBase.h"

#define MAX_LOADSTRING 100

// �O���[�o���ϐ�:
TCHAR szTitle[MAX_LOADSTRING];					// �^�C�g�� �o�[�̃e�L�X�g
TCHAR szWindowClass[MAX_LOADSTRING];			// ���C�� �E�B���h�E �N���X��

BaseDialog *baseDialog;

// �A�v���P�[�V�����ŗL�̃O���[�o���ϐ�
DialogAppliBase *appObj;

// ���̃R�[�h ���W���[���Ɋ܂܂��֐��̐錾��]�����܂�:

/**
 * �A�v���P�[�V�������C�����[�`��
 *
 * @param hInstance �A�v���P�[�V�����̃C���X�^���X�n���h��
 * @param hPrevInstance ���g�p
 * @param lpCmdLine �R�}���h���C��
 * @param nCmdShow �\�����
 * @return �v���O�����I���R�[�h
 */
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	int result;
	INITCOMMONCONTROLSEX iccx;

	hInst = hInstance;
	iccx.dwSize = sizeof(INITCOMMONCONTROLSEX);
	iccx.dwICC = ICC_TAB_CLASSES;
	if (!InitCommonControlsEx(&iccx)) {
         return FALSE;
	}

	// initModelessCallback();

	// TODO: �����ɃA�v���P�[�V�����N���X�̃C���X�^���X��ݒ肵�Ă��������B
	appObj = createAppli();
	baseDialog = appObj->createBaseDialog();

	// �E�C���h�E�\���O�̃A�v���P�[�V�����̏��������s���B
	appObj->OnAppliStart(lpCmdLine);

	// �_�C�A���O��\������B
	result = DialogBox(hInstance,MAKEINTRESOURCE(IDD_MAIN_DIALOG),NULL,(DLGPROC)MainDialogProc);

	// �A�v���P�[�V�����̌�n�����s���B
	appObj->OnAppliEnd();

	delete appObj;
	return result;
}

/**
 * �f�X�g���N�^
 */
DialogAppliBase::~DialogAppliBase()
{
}

/**
 * �A�v���P�[�V�����E�C���h�E�N���X�̃C���X�^���X�𐶐����܂��B
 *
 * @return �A�v���P�[�V�����E�C���h�E�N���X�̃C���X�^���X
 */
BaseDialog *DialogAppliBase::createBaseDialog()
{
	return new BaseDialog();
}

/**
 * �A�v���P�[�V�����̊J�n���������s���܂��B
 *
 * @return �\��
 */
int DialogAppliBase::OnAppliStart(TCHAR *lpCmdLine)
{
	// ���̊֐����I�[�o�[���C�h���ăA�v���ŗL�̏��������s���܂��B
	return 0;
}

/**
 * �E�C���h�E���\�����ꂽ�Ƃ��̏��������s���܂��B<br>
 * �E�C���h�E�x�[�X�A�v���ƃC���^�t�F�[�X�����킹�邽�߂ɗp�ӂ��Ă��܂��B
 *
 * @return �\��
 */
int DialogAppliBase::OnWindowShow()
{

	// ���̊֐����I�[�o�[���C�h���āA����̕\�����̏������s���܂��B
	// ���̃^�C�~���O�Ń_�C�A���O�����݂���̂ŁA�����ɏ��������邱�Ƃ�
	// �_�C�A���O�������ԂŋN�����̏������������s�����Ƃ��ł��܂��B

	return 0;
}

/**
 * �A�v���P�[�V�����̏I�����������s���܂��B
 *
 * @return �\��
 */
int DialogAppliBase::OnAppliEnd()
{
	// ���̊֐����I�[�o�[���C�h���ăA�v���ŗL�̌㏈�����s���܂��B
	return 0;
}

INT_PTR DialogAppliBase::OnInitDialog()
{
	// �e�N���X�̃_�C�A���O�������������Ăяo���B
	BaseDialog::OnInitDialog();

	// �_�C�A���O�\�����̏������������s���B
	OnWindowShow();

	return (INT_PTR)FALSE;
}
