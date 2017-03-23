//
// CRC�`�F�b�N�p���C�u�����N���X
//
#include "Typedef.h"
#include "Crclib.h"

BOOL	CRC::m_Init = FALSE;
BOOL	CRC::m_InitRev = FALSE;
DWORD	CRC::m_CrcTable[ UCHAR_MAX+1 ];
DWORD	CRC::m_CrcTableRev[ UCHAR_MAX+1 ];

#define CRCPOLY1 0x04C11DB7UL
#define CRCPOLY2 0xEDB88320UL

DWORD	CRC::Crc( INT size, LPBYTE c )
{
	if( !m_Init ) {
		MakeTable();
		m_Init = TRUE;
	}

	DWORD	r = 0xFFFFFFFFUL;
	while( --size >= 0 ) {
		r = (r << CHAR_BIT) ^ m_CrcTable[(BYTE)(r >> (32 - CHAR_BIT)) ^ *c++];
	}
	return ~r & 0xFFFFFFFFUL;
}

DWORD	CRC::CrcRev( INT size, LPBYTE c )
{
	if( !m_InitRev ) {
		MakeTableRev();
		m_InitRev = TRUE;
	}

	DWORD	r = 0xFFFFFFFFUL;
	while( --size >= 0 ) {
		r = (r >> CHAR_BIT) ^ m_CrcTableRev[(BYTE)r ^ *c++];
	}
	return r ^ 0xFFFFFFFFUL;
}

void	CRC::MakeTable()
{
INT	i, j;
DWORD	r;

	for( i = 0; i <= UCHAR_MAX; i++ ) {
		r = (DWORD)i << (32 - CHAR_BIT);
		for( j = 0; j < CHAR_BIT; j++ ) {
			if( r & 0x80000000UL ) r = (r << 1) ^ CRCPOLY1;
			else                   r <<= 1;
		}
		m_CrcTable[i] = r & 0xFFFFFFFFUL;
	}
}

void	CRC::MakeTableRev()
{
INT	i, j;
DWORD	r;

	for( i = 0; i <= UCHAR_MAX; i++ ) {
		r = i;
		for( j = 0; j < CHAR_BIT; j++ ) {
			if( r & 1 ) r = (r >> 1) ^ CRCPOLY2;
			else        r >>= 1;
		}
		m_CrcTableRev[i] = r;
	}
}

