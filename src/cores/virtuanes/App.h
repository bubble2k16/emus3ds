//
// �A�v���P�[�V�����T�|�[�g�N���X
//

#ifndef	__CAPP_INCLUDED__
#define	__CAPP_INCLUDED__

#define	WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "Typedef.h"


#define	ERRORSTRING_MAX	32

class CApp {
public:
/*
	// �N�����ɐݒ肷��
	static	void	SetInstance( HINSTANCE hInstance )	{ m_hInstance = hInstance; }
	static	void	SetPrevInstance( HINSTANCE hInstance )	{ m_hPrevInstance = hInstance; }
	static	void	SetCmdLine( LPSTR lpCmdLine )		{ m_lpCmdLine = lpCmdLine; }
	static	void	SetCmdShow( INT nCmdShow )		{ m_nCmdShow = nCmdShow; }

	static	void	SetHWnd( HWND hWnd )			{ m_hWnd = hWnd; }
	static	void	SetMenu( HMENU hMenu )			{ m_hMenu = hMenu; }
	static	void	SetPlugin( HINSTANCE hPlugin )		{ m_hPlugin = hPlugin; LoadErrorString(); }

	static	void	SetModulePath( LPCSTR lpModulePath )	{ ::strcpy( m_szModulePath, lpModulePath ); }

	// �A�v�������Ŏg�p����
	static	HINSTANCE	GetInstance()		{ return m_hInstance; }
	static	HINSTANCE	GetPrevInstance()	{ return m_hPrevInstance; }
	static	LPTSTR		GetCmdLine()		{ return m_lpCmdLine; }
	static	INT		GetCmdShow()		{ return m_nCmdShow; }

	static	HWND		GetHWnd()		{ return m_hWnd; }
	static	HMENU		GetMenu()		{ return m_hMenu; }
	static	HINSTANCE	GetPlugin()		{ return m_hPlugin; }

	static	LPCSTR		GetModulePath()		{ return m_szModulePath; }

	static	HMENU		LoadMenu( UINT uID )	{ return ::LoadMenu( m_hPlugin, MAKEINTRESOURCE(uID) ); }
	static	HICON		LoadIcon( UINT uID )	{ return ::LoadIcon( m_hInstance, MAKEINTRESOURCE(uID) ); }
	static	INT		LoadString( UINT uID, LPTSTR lpBuffer, INT nBufferMax ) {
					return	::LoadString( m_hPlugin, uID, lpBuffer, nBufferMax ); }
*/
	// �G���[�����p
	static	void	LoadErrorString();
	static	CHAR*	GetErrorString( INT nID );

protected:
/*
	// WinMain �����̈������̂܂�
	static	HINSTANCE	m_hInstance;
	static	HINSTANCE	m_hPrevInstance;
	static	LPSTR		m_lpCmdLine;
	static	INT		m_nCmdShow;

	// ���ꃊ�\�[�X�v���O�C���̃C���X�^���X�n���h��
	static	HINSTANCE	m_hPlugin;
	// ���C���E�C���h�E�̃E�C���h�E�n���h��
	static	HWND		m_hWnd;
	// ���C���E�C���h�E�̃��j���[�n���h��
	static	HMENU		m_hMenu;

	// �v���O�����̃p�X(�N�����̃p�X)
	static	CHAR		m_szModulePath[_MAX_PATH];

	// �G���[�����p*/
	
	static	INT	m_ErrorStringTableID[ERRORSTRING_MAX];
	static	CHAR	m_ErrorString[ERRORSTRING_MAX][256];
private:
};

// �G���[���b�Z�[�W�p�e���|����
extern	CHAR	szErrorString[256];

#endif	// !__CAPP_INCLUDED__
