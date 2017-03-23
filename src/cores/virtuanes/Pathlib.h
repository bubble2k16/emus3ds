//
// �p�X���C�u�����N���X
//
#ifndef	__CPATHLIB_INCLUDED__
#define	__CPATHLIB_INCLUDED__

#define	WIN32_LEAN_AND_MEAN
#include "Typedef.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
//#include <shlobj.h>
//#include <shlwapi.h>

#include <string>
using namespace std;

class	CPathlib
{
public:
	// �p�X�{�t�@�C���l�[�������̃p�X�擾
	static	string	SplitPath( LPCSTR lpszPath );
	// �p�X�{�t�@�C���l�[�������̃t�@�C�����擾(�g���q�Ȃ�)
	static	string	SplitFname( LPCSTR lpszPath );
	// �p�X�{�t�@�C���l�[�������̃t�@�C�����擾(�g���q����)
	static	string	SplitFnameExt( LPCSTR lpszPath );
	// �p�X�{�t�@�C���l�[�������̊g���q�擾
	static	string	SplitExt( LPCSTR lpszPath );

	// �p�X�C�t�@�C���������̃p�X�쐬(�g���q����/�Ȃ����p)
	static	string	MakePath( LPCSTR lpszPath, LPCSTR lpszFname );
	// �p�X�C�t�@�C�����C�g���q�����̃p�X�쐬(�g���q���ʂɎw��)
	static	string	MakePathExt( LPCSTR lpszPath, LPCSTR lpszFname, LPCSTR lpszExt );

	// �x�[�X�p�X�����ǉ��p�X�̎��ނ𒲂ׂăp�X���쐬����
	// �ǉ��p�X�����΃p�X�Ȃ��΂��̂܂܁C���΃p�X�Ȃ��΃x�[�X�p�X�ɒǉ�
	static	string	CreatePath( LPCSTR lpszBasePath, LPCSTR lpszPath );

	// �t�H���_�I��
	//static	BOOL	SelectFolder( HWND hWnd, LPCSTR lpszTitle, LPSTR lpszFolder );

protected:
	//static INT CALLBACK BffCallback( HWND hWnd, UINT uMsg, LPARAM lParam, WPARAM wParam );
private:
};

#endif	// !__CPATHLIB_INCLUDED__

