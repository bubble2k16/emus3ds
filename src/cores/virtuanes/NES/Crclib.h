//
// CRCチェック用ライブラリクラス
//
#define	WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <limits.h>

class	CRC
{
public:
	static	DWORD	Crc( INT size, LPBYTE c );
	static	DWORD	CrcRev( INT size, LPBYTE c );	// 逆順

protected:
	static	void	MakeTable();
	static	void	MakeTableRev();	// 逆順

	static	BOOL	m_Init;
	static	BOOL	m_InitRev;
	static	DWORD	m_CrcTable[ UCHAR_MAX+1 ];
	static	DWORD	m_CrcTableRev[ UCHAR_MAX+1 ];
private:
};

