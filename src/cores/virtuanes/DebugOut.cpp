//
// Debug output
//
#include "TypeDef.h"
#include "DebugOut.h"

CDebugOut	Dbg;

static const CHAR szClassName[] = "DebugWindow_wndclass";

CDebugOut::CDebugOut()
{
#if	defined(_DEBUG) || defined(_DEBUGOUT)
	hWndDebugOutput = ::FindWindow( szClassName, NULL );
	if( !hWndDebugOutput ) {
		::OutputDebugString( "DebugWindow �������܂���\n" );
	}
#endif
}

void CDebugOut::Clear()
{
#if	defined(_DEBUG) || defined(_DEBUGOUT)
	if( hWndDebugOutput ) {
		if( ::IsWindow( hWndDebugOutput ) ) {
			::SendMessage( hWndDebugOutput, WM_APP+1, (WPARAM)NULL, (LPARAM)NULL );
		}
	}
#endif
}

void __cdecl CDebugOut::Out( LPSTR fmt, ... )
{
#if	defined(_DEBUG) || defined(_DEBUGOUT)
	CHAR	buf[1000];
	va_list	va;
	va_start( va, fmt );
	::vsprintf( buf, fmt, va );

	if( hWndDebugOutput ) {
		if( ::IsWindow( hWndDebugOutput ) ) {
			COPYDATASTRUCT	cds;
			cds.dwData = 0;
			cds.lpData = (void*)buf;
			cds.cbData = ::strlen(buf)+1; //  �I�[��NULL������
			//  �����񑗐M
			::SendMessage( hWndDebugOutput, WM_COPYDATA, (WPARAM)NULL, (LPARAM)&cds );
		} else {
			::OutputDebugString( buf );
		}
	} else {
		::OutputDebugString( buf );
	}
#endif
}

void CDebugOut::Out( const string& str )
{
#if	defined(_DEBUG) || defined(_DEBUGOUT)
	Out( (LPSTR)str.c_str() );
#endif
}

