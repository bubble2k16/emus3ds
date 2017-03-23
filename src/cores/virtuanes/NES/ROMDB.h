//
// NES ROMDB class
//
#ifndef	__ROMDB_INCLUDED__
#define	__ROMDB_INCLUDED__

#pragma warning(disable:4786)
#include <list>
#include <map>
#include <string>
using namespace std;

#include "typedef.h"
#include "macro.h"

#include "rom.h"

//
// ROM DATABASE (NESToy&NNNesterJ database)
//
class	ROMDB
{
public:
	DWORD	crcall;		// ALL CRC
	DWORD	crc;		// PRG CRC

	BYTE	control1;
	BYTE	control2;
	BYTE	prg_size;
	BYTE	chr_size;

	string	title;
	string	country;
	string	manufacturer;
	string	saledate;
	string	price;
	string	genre;

	BOOL	bNTSC;		// NTSC:TRUE  PAL:FALSE
};

class	ROMDBCMP
{
public:
	bool	operator()( const DWORD& x, const DWORD& y ) const
	{
		return	x < y;
	}
};

class	ROMDATABASE
{
public:
	INT	HeaderCheck  ( NESHEADER& hdr, DWORD crcall, DWORD crc, ROMDB& data );
	BOOL	HeaderCorrect( NESHEADER& hdr, DWORD crcall, DWORD crc );

protected:
	void	LoadDatabase();

	map<DWORD, ROMDB, ROMDBCMP>	m_DataBaseList;

private:
};

extern	ROMDATABASE	romdatabase;

#endif	// !__ROMDB_INCLUDED__
