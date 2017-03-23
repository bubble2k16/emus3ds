//////////////////////////////////////////////////////////////////////////
//                                                                      //
//      Namcot N106                                                     //
//                                                           Norix      //
//                                               written     2001/09/18 //
//                                               last modify ----/--/-- //
//////////////////////////////////////////////////////////////////////////
#include "APU_N106.h"
#include "state.h"

#include "DebugOut.h"

#define	CHANNEL_VOL_SHIFT	6

APU_N106::APU_N106()
{
	// ç≈èâÇæÇØTONEÇÃèâä˙âªÇçsÇ§
	::ZeroMemory( tone, sizeof(tone) );

	// âºê›íË
	cpu_clock = APU_CLOCK;
	cycle_rate = (DWORD)(cpu_clock*12.0f*(1<<20)/(45.0f*22050.0f));
}

APU_N106::~APU_N106()
{
}

void	APU_N106::Reset( FLOAT fClock, INT nRate )
{
	for( INT i = 0; i < 8; i++ ) {
		::ZeroMemory( &op[i], sizeof(op[i]) );
		op[i].tonelen = 0x10<<18;
	}

	address = 0;
	addrinc = 1;
	channel_use = 8;

	Setup( fClock, nRate );

	// TONEÇÃèâä˙âªÇÕÇµÇ»Ç¢...
}

void	APU_N106::Setup( FLOAT fClock, INT nRate )
{
	cpu_clock = fClock;
	cycle_rate = (DWORD)(cpu_clock*12.0f*(1<<20)/(45.0f*nRate));
}

void	APU_N106::Write( WORD addr, BYTE data )
{
	if( addr == 0x4800 ) {
//		tone[address*2+0] = (INT)(data&0x0F);
//		tone[address*2+1] = (INT)(data  >>4);
		tone[address*2+0] = data&0x0F;
		tone[address*2+1] = data>>4;

		if( address >= 0x40 ) {
			INT	no = (address-0x40)>>3;
			DWORD	tonelen;
			CHANNEL& ch = op[no];

			switch( address & 7 ) {
				case	0x00:
					ch.freq = (ch.freq&~0x000000FF)|(DWORD)data;
					break;
				case	0x02:
					ch.freq = (ch.freq&~0x0000FF00)|((DWORD)data<<8);
					break;
				case	0x04:
					ch.freq = (ch.freq&~0x00030000)|(((DWORD)data&0x03)<<16);
					tonelen = (0x20-(data&0x1c))<<18;
					ch.databuf = (data&0x1c)>>2;
					if( ch.tonelen != tonelen ) {
						ch.tonelen = tonelen;
						ch.phase = 0;
					}
					break;
				case	0x06:
					ch.toneadr = data;
					break;
				case	0x07:
					ch.vol = data&0x0f;
					ch.volupdate = 0xFF;
					if( no == 7 )
						channel_use = ((data>>4)&0x07)+1;
					break;
			}
		}

		if( addrinc ) {
			address = (address+1)&0x7f;
		}
	} else if( addr == 0xF800 ) {
		address = data&0x7F;
		addrinc = data&0x80;
	}
}

BYTE	APU_N106::Read( WORD addr )
{
	// $4800 dummy read!!
	if( addr == 0x0000 ) {
		if( addrinc ) {
			address = (address+1)&0x7F;
		}
	}

	return	(BYTE)(addr>>8);
}

INT	APU_N106::Process( INT channel )
{
	if( channel >= (8-channel_use) && channel < 8 ) {
		return	ChannelRender( op[channel] );
	}

	return	0;
}

INT	APU_N106::GetFreq( INT channel )
{
	if( channel < 8 ) {
		channel &= 7;
		if( channel < (8-channel_use) )
			return	0;

		CHANNEL* ch = &op[channel&0x07];
		if( !ch->freq || !ch->vol )
			return	0;
		INT	temp = channel_use*(8-ch->databuf)*4*45;
		if( !temp )
			return	0;
		return	(INT)(256.0*(double)cpu_clock*12.0*ch->freq/((double)0x40000*temp));
	}

	return	0;
}

INT	APU_N106::ChannelRender( CHANNEL& ch )
{
DWORD	phasespd = channel_use<<20;

	ch.phaseacc -= cycle_rate;
	if( ch.phaseacc >= 0 ) {
		if( ch.volupdate ) {
			ch.output = ((INT)tone[((ch.phase>>18)+ch.toneadr)&0xFF]*ch.vol)<<CHANNEL_VOL_SHIFT;
			ch.volupdate = 0;
		}
		return	ch.output;
	}

	while( ch.phaseacc < 0 ) {
		ch.phaseacc += phasespd;
		ch.phase += ch.freq;
	}
	while( ch.tonelen && (ch.phase >= ch.tonelen)) {
		ch.phase -= ch.tonelen;
	}

	ch.output = ((INT)tone[((ch.phase>>18)+ch.toneadr)&0xFF]*ch.vol)<<CHANNEL_VOL_SHIFT;

	return	ch.output;
}

INT	APU_N106::GetStateSize()
{
	return	3*sizeof(BYTE) + 8*sizeof(CHANNEL) + sizeof(tone);
}

void	APU_N106::SaveState( LPBYTE p )
{
	SETBYTE( p, addrinc );
	SETBYTE( p, address );
	SETBYTE( p, channel_use );

	SETBLOCK( p, op, sizeof(op) );
	SETBLOCK( p, tone, sizeof(tone) );
}

void	APU_N106::LoadState( LPBYTE p )
{
	GETBYTE( p, addrinc );
	GETBYTE( p, address );
	GETBYTE( p, channel_use );

	GETBLOCK( p, op, sizeof(op) );
	GETBLOCK( p, tone, sizeof(tone) );
}

