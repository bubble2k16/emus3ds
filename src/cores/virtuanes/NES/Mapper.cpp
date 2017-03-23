/*----------------------------------------------------------------------*/
/*                                                                      */
/*      NES Mapeers                                                     */
/*                                                           Norix      */
/*                                               written     2000/02/05 */
/*                                               last modify ----/--/-- */
/*----------------------------------------------------------------------*/
/*--------------[ INCLUDE               ]-------------------------------*/
#define	WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "DebugOut.h"
#include "typedef.h"
#include "macro.h"

#include "nes.h"
#include "mmu.h"

#include "mapper.h"

/*--------------[ DEFINE                ]-------------------------------*/
/*--------------[ EXTERNAL PROGRAM      ]-------------------------------*/
/*--------------[ EXTERNAL WORK         ]-------------------------------*/
/*--------------[ WORK                  ]-------------------------------*/
/*--------------[ PROTOTYPE             ]-------------------------------*/
/*--------------[ CONST                 ]-------------------------------*/
/*--------------[ PROGRAM               ]-------------------------------*/

//////////////////////////////////////////////////////////////////////////
// Based class implement
//////////////////////////////////////////////////////////////////////////
Mapper::Mapper( NES* parent ) : nes(parent)
{
}

Mapper::~Mapper()
{
}

// $4100-$7FFF Lower Memory read
BYTE	Mapper::ReadLow( WORD addr )
{
	// $6000-$7FFF WRAM
	if( addr >= 0x6000 && addr <= 0x7FFF ) {
		return	CPU_MEM_BANK[addr>>13][addr&0x1FFF];
	}

	return	(BYTE)(addr>>8);
}

// $4100-$7FFF Lower Memory write
void	Mapper::WriteLow( WORD addr, BYTE data )
{
	// $6000-$7FFF WRAM
	if( addr >= 0x6000 && addr <= 0x7FFF ) {
		CPU_MEM_BANK[addr>>13][addr&0x1FFF] = data;
	}
}

