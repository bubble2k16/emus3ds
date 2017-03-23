#ifndef	__PNGWRITE_INCLUDED__
#define	__PNGWRITE_INCLUDED__

#include <stdio.h>
#include <stdlib.h>
#include "zlib.h"
#include "typedef.h"
#include "macro.h"

class	PNGWRITE
{
public:
	bool	Write( const char* fname, INT nWidth, INT nHeight, RGBQUAD* pRGB, LPBYTE lpBitmap, DWORD dwScan )
	{
		FILE*	fp = NULL;
		LPBYTE	pBits = NULL;
		LPBYTE	pZbuf = NULL;
		unsigned long	zlibbuffersize;
		INT	i;

		if( !(fp = ::fopen( fname, "wb" )) )
			goto	error_exit;

		if( !WriteSignature( fp ) )
			goto	error_exit;

		if( !(pBits = (LPBYTE)::malloc( (nWidth+1)*nHeight*sizeof(BYTE) )) )
			goto	error_exit;

		LPBYTE	pSrc, pDst;
		pSrc = lpBitmap;
		pDst = pBits;

		// No Filter
		for( i = 0; i < nHeight; i++ ) {
			*(pDst++) = 0;
			::memcpy( pDst, pSrc, nWidth );
			pSrc += dwScan;
			pDst += nWidth;
		}

		zlibbuffersize = ((nWidth+1)*nHeight)*1.1+12;
		if( !(pZbuf = (LPBYTE)::malloc( zlibbuffersize )) )
			goto	error_exit;

		if( compress( pZbuf, &zlibbuffersize, pBits, ((nWidth+1)*nHeight) ) != Z_OK )
			goto	error_exit;

		// write IHDR
		{
			BYTE	temp[13];

			// Write Length
			ConvertNetworkOrder( nWidth, temp );
			ConvertNetworkOrder( nHeight, temp+4 );
			*(temp+ 8) = 8;	// 8bpp
			*(temp+ 9) = 3;	// Indexed color
			*(temp+10) = 0;	// Compression method
			*(temp+11) = 0;	// Filter method
			*(temp+12) = 0;	// Interace method

			if( !WriteChunk( fp, PNG_CHUNK_IHDR, temp, 13 ) )
				goto	error_exit;
		}

		// write PLTE
		{
			BYTE	pal[256*3];
			for( INT i = 0; i < 256; i++ ) {
				pal[i*3+0] = pRGB[i].rgbRed;
				pal[i*3+1] = pRGB[i].rgbGreen;
				pal[i*3+2] = pRGB[i].rgbBlue;
			}

			if( !WriteChunk( fp, PNG_CHUNK_PLTE, pal, 256*3 ) )
				goto	error_exit;
		}

		// write IDAT
		if( !WriteChunk( fp, PNG_CHUNK_IDAT, pZbuf, zlibbuffersize ) )
			goto	error_exit;

		// write IEND
		if( !WriteChunk( fp, PNG_CHUNK_IEND, NULL, 0 ) )
			goto	error_exit;

		FREE( pBits );
		FREE( pZbuf );
		FCLOSE( fp );
		return	true;
	error_exit:
		FREE( pBits );
		FREE( pZbuf );
		FCLOSE( fp );
		return	false;
	}

protected:
	void	ConvertNetworkOrder( UINT uData, BYTE* pBuf )
	{
		pBuf[0] = (BYTE)(uData>>24);
		pBuf[1] = (BYTE)(uData>>16);
		pBuf[2] = (BYTE)(uData>>8);
		pBuf[3] = (BYTE)(uData);
	}

	bool	WriteSignature( FILE* fp )
	{
		const char	PNG_SIGNATURE[] = { "\x89\x50\x4E\x47\x0D\x0A\x1A\x0A" };

		if( ::fwrite( PNG_SIGNATURE, 8, 1, fp ) != 1 )
			return	false;
		return	true;
	}

	bool	WriteChunk( FILE* fp, UINT uType, BYTE* pData, UINT uLength )
	{
		BYTE	temp[4];

		// Write Length
		ConvertNetworkOrder( uLength, temp );
		if( ::fwrite( temp, sizeof(temp), 1, fp ) != 1 )
			return	false;

		// Write Chunk Type
		ConvertNetworkOrder( uType, temp );
		if( ::fwrite( temp, sizeof(temp), 1, fp ) != 1 )
			return	false;

		UINT	crc = crc32( 0, temp, sizeof(temp) );
		if( uLength ) {
			if( ::fwrite( pData, uLength, 1, fp ) != 1 )
				return	false;

			crc = crc32( crc, pData, uLength );
		}

		// Write CRC32
		ConvertNetworkOrder( crc, temp );
		if( ::fwrite( temp, sizeof(temp), 1, fp ) != 1 )
			return	false;

		return	true;
	}
private:
	enum {
		PNG_CHUNK_IHDR = 0x49484452,
		PNG_CHUNK_PLTE = 0x504C5445,
		PNG_CHUNK_IDAT = 0x49444154,
		PNG_CHUNK_IEND = 0x49454E44,
	};
};

#endif	// !__PNGWRITE_INCLUDED__
