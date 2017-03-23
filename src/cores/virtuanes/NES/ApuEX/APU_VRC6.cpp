//////////////////////////////////////////////////////////////////////////
//                                                                      //
//      Konami VRC6                                                     //
//                                                           Norix      //
//                                               written     2001/09/18 //
//                                               last modify ----/--/-- //
//////////////////////////////////////////////////////////////////////////
#include "APU_VRC6.h"
#include "state.h"

#define	RECTANGLE_VOL_SHIFT	8
#define	SAWTOOTH_VOL_SHIFT	6

APU_VRC6::APU_VRC6()
{
	// ‰¼Ý’è
	Reset( APU_CLOCK, 22050 );
}

APU_VRC6::~APU_VRC6()
{
}

void	APU_VRC6::Reset( FLOAT fClock, INT nRate )
{
	ZeroMemory( &ch0, sizeof(ch0) );
	ZeroMemory( &ch1, sizeof(ch1) );
	ZeroMemory( &ch2, sizeof(ch2) );

	Setup( fClock, nRate );
}

void	APU_VRC6::Setup( FLOAT fClock, INT nRate )
{
	cpu_clock = fClock;
	cycle_rate = (INT)(fClock*65536.0f/(float)nRate);
}

void	APU_VRC6::Write( WORD addr, BYTE data )
{
	switch( addr ) {
		// VRC6 CH0 rectangle
		case	0x9000:
			ch0.reg[0] = data;
			ch0.gate     = data&0x80;
			ch0.volume   = data&0x0F;
			ch0.duty_pos = (data>>4)&0x07;
			break;
		case	0x9001:
			ch0.reg[1] = data;
			ch0.freq = INT2FIX( (((ch0.reg[2]&0x0F)<<8)|data)+1 );
			break;
		case	0x9002:
			ch0.reg[2] = data;
			ch0.enable = data&0x80;
			ch0.freq   = INT2FIX( (((data&0x0F)<<8)|ch0.reg[1])+1 );
			break;
		// VRC6 CH1 rectangle
		case	0xA000:
			ch1.reg[0] = data;
			ch1.gate     = data&0x80;
			ch1.volume   = data&0x0F;
			ch1.duty_pos = (data>>4)&0x07;
			break;
		case	0xA001:
			ch1.reg[1] = data;
			ch1.freq = INT2FIX( (((ch1.reg[2]&0x0F)<<8)|data)+1 );
			break;
		case	0xA002:
			ch1.reg[2] = data;
			ch1.enable = data&0x80;
			ch1.freq   = INT2FIX( (((data&0x0F)<<8)|ch1.reg[1])+1 );
			break;
		// VRC6 CH2 sawtooth
		case	0xB000:
			ch2.reg[1] = data;
			ch2.phaseaccum = data&0x3F;
			break;
		case	0xB001:
			ch2.reg[1] = data;
			ch2.freq = INT2FIX( (((ch2.reg[2]&0x0F)<<8)|data)+1 );
			break;
		case	0xB002:
			ch2.reg[2] = data;
			ch2.enable = data&0x80;
			ch2.freq   = INT2FIX( (((data&0x0F)<<8)|ch2.reg[1])+1 );
//			ch2.adder = 0;	// ƒNƒŠƒA‚·‚é‚ÆƒmƒCƒY‚ÌŒ´ˆö‚É‚È‚é
//			ch2.accum = 0;	// ƒNƒŠƒA‚·‚é‚ÆƒmƒCƒY‚ÌŒ´ˆö‚É‚È‚é
			break;
	}
}

INT	APU_VRC6::Process( INT channel )
{
	switch( channel ) {
		case	0:
			return	RectangleRender( ch0 );
			break;
		case	1:
			return	RectangleRender( ch1 );
			break;
		case	2:
			return	SawtoothRender( ch2 );
			break;
	}

	return	0;
}

INT	APU_VRC6::GetFreq( INT channel )
{
	if( channel == 0 || channel == 1 ) {
		RECTANGLE*	ch;
		if( channel == 0 ) ch = &ch0;
		else		   ch = &ch1;
		if( !ch->enable || ch->gate || !ch->volume )
			return	0;
		if( ch->freq < INT2FIX( 8 ) )
			return	0;
		return	(INT)(256.0f*cpu_clock/((FLOAT)FIX2INT(ch->freq)*16.0f));
	}
	if( channel == 2 ) {
		SAWTOOTH*	ch = &ch2;
		if( !ch->enable || !ch->phaseaccum )
			return	0;
		if( ch->freq < INT2FIX( 8 ) )
			return	0;
		return	(INT)(256.0f*cpu_clock/((FLOAT)FIX2INT(ch->freq)*14.0f));
	}

	return	0;
}

INT	APU_VRC6::RectangleRender( RECTANGLE& ch )
{
	// Enable?
	if( !ch.enable ) {
		ch.output_vol = 0;
		ch.adder = 0;
		return	ch.output_vol;
	}

	// Digitized output
	if( ch.gate ) {
		ch.output_vol = ch.volume<<RECTANGLE_VOL_SHIFT;
		return	ch.output_vol;
	}

	// ˆê’èˆÈã‚ÌŽü”g”‚Íˆ—‚µ‚È‚¢(–³‘Ê)
	if( ch.freq < INT2FIX( 8 ) ) {
		ch.output_vol = 0;
		return	ch.output_vol;
	}

	ch.phaseacc -= cycle_rate;
	if( ch.phaseacc >= 0 )
		return	ch.output_vol;

	INT	output = ch.volume<<RECTANGLE_VOL_SHIFT;

	if( ch.freq > cycle_rate ) {
	// add 1 step
		ch.phaseacc += ch.freq;
		ch.adder = (ch.adder+1)&0x0F;
		if( ch.adder <= ch.duty_pos )
			ch.output_vol = output;
		else
			ch.output_vol = -output;
	} else {
	// average calculate
		INT	num_times, total;
		num_times = total = 0;
		while( ch.phaseacc < 0 ) {
			ch.phaseacc += ch.freq;
			ch.adder = (ch.adder+1)&0x0F;
			if( ch.adder <= ch.duty_pos )
				total += output;
			else
				total += -output;
			num_times++;
		}
		ch.output_vol = total/num_times;
	}

	return	ch.output_vol;
}

INT	APU_VRC6::SawtoothRender( SAWTOOTH& ch )
{
	// Digitized output
	if( !ch.enable ) {
		ch.output_vol = 0;
		return	ch.output_vol;
	}

	// ˆê’èˆÈã‚ÌŽü”g”‚Íˆ—‚µ‚È‚¢(–³‘Ê)
	if( ch.freq < INT2FIX( 9 ) ) {
		return	ch.output_vol;
	}

	ch.phaseacc -= cycle_rate/2;
	if( ch.phaseacc >= 0 )
		return	ch.output_vol;

	if( ch.freq > cycle_rate/2 ) {
	// add 1 step
		ch.phaseacc += ch.freq;
		if( ++ch.adder >= 7 ) {
			ch.adder = 0;
			ch.accum = 0;
		}
		ch.accum += ch.phaseaccum;
		ch.output_vol = ch.accum<<SAWTOOTH_VOL_SHIFT;
	} else {
	// average calculate
		INT	num_times, total;
		num_times = total = 0;
		while( ch.phaseacc < 0 ) {
			ch.phaseacc += ch.freq;
			if( ++ch.adder >= 7 ) {
				ch.adder = 0;
				ch.accum = 0;
			}
			ch.accum += ch.phaseaccum;
			total += ch.accum<<SAWTOOTH_VOL_SHIFT;
			num_times++;
		}
		ch.output_vol = (total/num_times);
	}

	return	ch.output_vol;
}

INT	APU_VRC6::GetStateSize()
{
	return	sizeof(ch0) + sizeof(ch1) + sizeof(ch2);
}

void	APU_VRC6::SaveState( LPBYTE p )
{
	SETBLOCK( p, &ch0, sizeof(ch0) );
	SETBLOCK( p, &ch1, sizeof(ch1) );
	SETBLOCK( p, &ch2, sizeof(ch2) );
}

void	APU_VRC6::LoadState( LPBYTE p )
{
	GETBLOCK( p, &ch0, sizeof(ch0) );
	GETBLOCK( p, &ch1, sizeof(ch1) );
	GETBLOCK( p, &ch2, sizeof(ch2) );
}

