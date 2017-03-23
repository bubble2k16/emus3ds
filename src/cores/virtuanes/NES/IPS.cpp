#include "Typedef.h"
#include "App.h"
#include "Pathlib.h"
#include "Config.h"
#include "Debugout.h"

//#include "unzip.h"
#include "ips.h"

#include <vector>
using namespace std;

static	BOOL	PatchIPS( LPBYTE pIPS, LPBYTE pROM, LONG imagesize, LONG ipssize )
{
	INT	index = 0;

	// IPS patch?
	if( ::strncmp( (const char *)pIPS, "PATCH", 5 ) != 0 ) {
		return	FALSE;
	}
	
	index += 5;

	INT	offset, length;
	while( TRUE ) {
		// read patch address offset
		if( index+3 > ipssize ) {
			return	FALSE;
		}
		offset = ((INT)pIPS[index]<<16)|((INT)pIPS[index+1]<<8)|((INT)pIPS[index+2]);
		index += 3;

		// END?
		if( offset == 0x00454f46 ) {
			break;
		}

		// read patch length
		if( index+2 > ipssize ) {
			return	FALSE;
		}
		length = ((INT)pIPS[index]<<8)|((INT)pIPS[index+1]);
		index += 2;

		if( length ) {
			while( length-- ) {
				if( offset < imagesize ) {
					pROM[ offset++ ] = pIPS[ index ];
				}
				if( ++index > ipssize ) {
					return	FALSE;
				}
			}
		} else {
			if( index+2 > ipssize ) {
				return	FALSE;
			}
			length = ((INT)pIPS[index]<<8)|((INT)pIPS[index+1]);
			index += 2;

			BYTE	data = pIPS[ index ];
			if( ++index > ipssize ) {
				return	FALSE;
			}

			while( length-- ) {
				if( offset < imagesize ) {
					pROM[ offset++ ] = data;
				}
			}
		}
	}

	return	TRUE;
}

BOOL	ApplyIPS( const char* filename, LPBYTE pROM, LONG imagesize )
{
	string	pathstr, tempstr;
	/*if( Config.path.bIpsPath ) {
		pathstr = CPathlib::CreatePath( CApp::GetModulePath(), Config.path.szIpsPath );
	} else*/ {
		pathstr = CPathlib::SplitPath( filename );
	}
	string	fname = CPathlib::SplitFname( filename );

	tempstr = CPathlib::MakePathExt( pathstr.c_str(), fname.c_str(), "ips" );
DEBUGOUT( "IPS name:%s\n", tempstr.c_str() );

	FILE*	fp = NULL;
	vector<BYTE> tempbuf;
	LONG	ipssize = 0;

DEBUGOUT( "Open ips\n" );
	if( (fp = ::fopen( tempstr.c_str(), "rb" )) ) {
		LONG	ipssize;
		::fseek( fp, 0, SEEK_END );
		ipssize = ::ftell( fp );
		::fseek( fp, 0, SEEK_SET );

		tempbuf.reserve( ipssize );

		// load IPS file
		if( ::fread( (void*)&tempbuf[0], ipssize, 1, fp ) != 1 ) {
DEBUGOUT( "IPS load error.\n" );
			FCLOSE( fp );
			return	FALSE;
		}

		FCLOSE( fp );

DEBUGOUT( "IPS patching.\n" );
		return	PatchIPS( (LPBYTE)&tempbuf[0], pROM, imagesize, ipssize );
	}

/*
	string	ext = CPathlib::SplitExt( filename );
	if( ext != ".zip" ) {
DEBUGOUT( "Not zip\n" );
		return	FALSE;
	}

DEBUGOUT( "Open zip\n" );

	unzFile		unzipFile = NULL;
	unz_global_info	unzipGlobalInfo;
	unz_file_info	unzipFileInfo;
	char		fname_buf[256];

	if( !(unzipFile = unzOpen( (const char*)filename )) )
		return	FALSE;

	if( unzGetGlobalInfo( unzipFile, &unzipGlobalInfo ) != UNZ_OK ) {
		unzClose( unzipFile );
		return	FALSE;
	}

	for( uLong i = 0; i < unzipGlobalInfo.number_entry; i++ ) {
		if( unzGetCurrentFileInfo( unzipFile, &unzipFileInfo, fname_buf, sizeof(fname_buf), NULL, 0, NULL, 0 ) != UNZ_OK )
			break;

		char*	pExt = ::PathFindExtension( fname_buf );
		if( _stricmp( pExt, ".ips" ) == 0 ) {

			if( unzipFileInfo.uncompressed_size ) {
				if( unzOpenCurrentFile( unzipFile ) != UNZ_OK )
					break;

				if( unzipFileInfo.uncompressed_size > 0 ) {
					tempbuf.reserve( unzipFileInfo.uncompressed_size );

					uInt	size = unzReadCurrentFile( unzipFile, &tempbuf[0], unzipFileInfo.uncompressed_size );
					if( size != unzipFileInfo.uncompressed_size )
						break;
				}
				ipssize = unzipFileInfo.uncompressed_size;

				if( unzCloseCurrentFile( unzipFile ) != UNZ_OK )
					break;
				unzClose( unzipFile );

				return	PatchIPS( (LPBYTE)&tempbuf[0], pROM, imagesize, ipssize );
			}
		}

		// Next file
		if( (i+1) < unzipGlobalInfo.number_entry ) {
			if( unzGoToNextFile( unzipFile ) != UNZ_OK ) {
				break;
			}
		}
	}

	unzClose( unzipFile );
*/
	return	FALSE;
}
