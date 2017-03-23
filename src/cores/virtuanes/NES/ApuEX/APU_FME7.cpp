//////////////////////////////////////////////////////////////////////////
//                                                                      //
//      SunSoft FME7                                                    //
//                                                           Norix      //
//                                               written     2001/09/18 //
//                                               last modify ----/--/-- //
//////////////////////////////////////////////////////////////////////////
#include "APU_FME7.h"

#define	CHANNEL_VOL_SHIFT	8

// Envelope tables
BYTE	APU_FME7::envelope_pulse0[] = {
	0x1F, 0x1E, 0x1D, 0x1C, 0x1B, 0x1A, 0x19, 0x18,
	0x17, 0x16, 0x15, 0x14, 0x13, 0x12, 0x11, 0x10,
	0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09, 0x08,
	0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00
};
BYTE	APU_FME7::envelope_pulse1[] = {
	      0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
	0x00
};
BYTE	APU_FME7::envelope_pulse2[] = {
	0x1F, 0x1E, 0x1D, 0x1C, 0x1B, 0x1A, 0x19, 0x18,
	0x17, 0x16, 0x15, 0x14, 0x13, 0x12, 0x11, 0x10,
	0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09, 0x08,
	0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x1F
};
BYTE	APU_FME7::envelope_pulse3[] = {
	      0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
	0x1F
};
SBYTE	APU_FME7::envstep_pulse[] = {
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 0
};

BYTE	APU_FME7::envelope_sawtooth0[] = {
	0x1F, 0x1E, 0x1D, 0x1C, 0x1B, 0x1A, 0x19, 0x18,
	0x17, 0x16, 0x15, 0x14, 0x13, 0x12, 0x11, 0x10,
	0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09, 0x08,
	0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00
};
BYTE	APU_FME7::envelope_sawtooth1[] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F
};
SBYTE	APU_FME7::envstep_sawtooth[] = {
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, -15
};

BYTE	APU_FME7::envelope_triangle0[] = {
	0x1F, 0x1E, 0x1D, 0x1C, 0x1B, 0x1A, 0x19, 0x18,
	0x17, 0x16, 0x15, 0x14, 0x13, 0x12, 0x11, 0x10,
	0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09, 0x08,
	0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00,
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F
};
BYTE	APU_FME7::envelope_triangle1[] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
	0x1F, 0x1E, 0x1D, 0x1C, 0x1B, 0x1A, 0x19, 0x18,
	0x17, 0x16, 0x15, 0x14, 0x13, 0x12, 0x11, 0x10,
	0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09, 0x08,
	0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00
};
SBYTE	APU_FME7::envstep_triangle[] = {
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, -31
};

LPBYTE	APU_FME7::envelope_table[16] = {
	envelope_pulse0,    envelope_pulse0, envelope_pulse0,    envelope_pulse0,
	envelope_pulse1,    envelope_pulse1, envelope_pulse1,    envelope_pulse1,
	envelope_sawtooth0, envelope_pulse0, envelope_triangle0, envelope_pulse2,
	envelope_sawtooth1, envelope_pulse3, envelope_triangle1, envelope_pulse1
};
LPSBYTE	APU_FME7::envstep_table[16] = {
	envstep_pulse,    envstep_pulse, envstep_pulse,    envstep_pulse,
	envstep_pulse,    envstep_pulse, envstep_pulse,    envstep_pulse,
	envstep_sawtooth, envstep_pulse, envstep_triangle, envstep_pulse,
	envstep_sawtooth, envstep_pulse, envstep_triangle, envstep_pulse
};

APU_FME7::APU_FME7()
{
	// âºê›íË
	Reset( APU_CLOCK, 22050 );
}

APU_FME7::~APU_FME7()
{
}

void	APU_FME7::Reset( FLOAT fClock, INT nRate )
{
	INT	i;

	::ZeroMemory( &envelope, sizeof(envelope) );
	::ZeroMemory( &noise, sizeof(noise) );

	for( i = 0; i < 3; i++ ) {
		::ZeroMemory( &op[i], sizeof(op[i]) );
	}

	envelope.envtbl  = envelope_table[0];
	envelope.envstep = envstep_table[0];

	noise.noiserange = 1;
	noise.noiseout   = 0xFF;

	address = 0;

	// Volume to voltage
	double	out = 0x1FFF;
	for( i = 31; i > 1; i-- ) {
		vol_table[i] = (INT)(out+0.5);
		out /= 1.188502227;	/* = 10 ^ (1.5/20) = 1.5dB */
	}
	vol_table[1] = 0;
	vol_table[0] = 0;

	Setup( fClock, nRate );
}

void	APU_FME7::Setup( FLOAT fClock, INT nRate )
{
	cpu_clock = fClock;
	cycle_rate = (INT)((fClock/16.0f)*(1<<16)/nRate);
}

void	APU_FME7::Write( WORD addr, BYTE data )
{
	if( addr == 0xC000 ) {
		address = data;
	} else if( addr == 0xE000 ) {
		BYTE	chaddr = address;
		switch( chaddr ) {
			case	0x00: case	0x01:
			case	0x02: case	0x03:
			case	0x04: case	0x05:
				{
				CHANNEL& ch = op[chaddr>>1];
				ch.reg[chaddr&0x01] = data;
				ch.freq = INT2FIX(((INT)(ch.reg[1]&0x0F)<<8)+ch.reg[0]+1);
				}
				break;
			case	0x06:
				noise.freq = INT2FIX((INT)(data&0x1F)+1);
				break;
			case	0x07:
				{
				for( INT i = 0; i < 3; i++ ) {
					op[i].enable   = data&(1<<i);
					op[i].noise_on = data&(8<<i);
				}
				}
				break;
			case	0x08:
			case	0x09:
			case	0x0A:
				{
				CHANNEL& ch = op[chaddr&3];
				ch.reg[2] = data;
				ch.env_on = data & 0x10;
				ch.volume = (data&0x0F)*2;
				}
				break;
			case	0x0B:
			case	0x0C:
				envelope.reg[chaddr-0x0B] = data;
				envelope.freq = INT2FIX(((INT)(envelope.reg[1]&0x0F)<<8)+envelope.reg[0]+1);
				break;
			case	0x0D:
				envelope.envtbl  = envelope_table[data&0x0F];
				envelope.envstep = envstep_table [data&0x0F];
				envelope.envadr  = 0;
				break;
		}
	}
}

INT	APU_FME7::Process( INT channel )
{
	if( channel < 3 ) {
		return	ChannelRender( op[channel] );
	} else if( channel == 3 ) {
		// ïKÇ∏ch3Ç1âÒåƒÇÒÇ≈Ç©ÇÁch0-2ÇåƒÇ‘éñ
		EnvelopeRender();
		NoiseRender();
	}

	return	0;
}

INT	APU_FME7::GetFreq( INT channel )
{
	if( channel < 3 ) {
		CHANNEL* ch = &op[channel];

		if( ch->enable || !ch->freq )
			return	0;
		if( ch->env_on ) {
			if( !envelope.volume )
				return	0;
		} else {
			if( !ch->volume )
				return	0;
		}

		return	(INT)(256.0f*cpu_clock/((FLOAT)FIX2INT(ch->freq)*16.0f));
	}

	return	0;
}

void	APU_FME7::EnvelopeRender()
{
	if( !envelope.freq )
		return;
	envelope.phaseacc -= cycle_rate;
	if( envelope.phaseacc >= 0 )
		return;
	while( envelope.phaseacc < 0 ) {
		envelope.phaseacc += envelope.freq;
		envelope.envadr += envelope.envstep[envelope.envadr];
	}
	envelope.volume = envelope.envtbl[envelope.envadr];
}

void	APU_FME7::NoiseRender()
{
	if( !noise.freq )
		return;
	noise.phaseacc -= cycle_rate;
	if( noise.phaseacc >= 0 )
		return;
	while( noise.phaseacc < 0 ) {
		noise.phaseacc += noise.freq;
		if( (noise.noiserange+1) & 0x02 )
			noise.noiseout = ~noise.noiseout;
		if( noise.noiserange & 0x01 )
			noise.noiserange ^= 0x28000;
		noise.noiserange >>= 1;
	}
}

INT	APU_FME7::ChannelRender( CHANNEL& ch )
{
INT	output, volume;

	if( ch.enable )
		return	0;
	if( !ch.freq )
		return	0;

	ch.phaseacc -= cycle_rate;
	while( ch.phaseacc < 0 ) {
		ch.phaseacc += ch.freq;
		ch.adder++;
	}

	output = volume = 0;
	volume = ch.env_on?vol_table[envelope.volume]:vol_table[ch.volume+1];

	if( ch.adder & 0x01 ) {
		output += volume;
	} else {
		output -= volume;
	}
	if( !ch.noise_on ) {
		if( noise.noiseout )
			output += volume;
		else
			output -= volume;
	}

	ch.output_vol = output;

	return	ch.output_vol;
}

INT	APU_FME7::GetStateSize()
{
	return	sizeof(BYTE) + sizeof(envelope) + sizeof(noise) + 3*sizeof(op);
}

void	APU_FME7::SaveState( LPBYTE p )
{
	SETBYTE( p, address );

	SETBLOCK( p, &envelope, sizeof(envelope) );
	SETBLOCK( p, &noise, sizeof(noise) );
	SETBLOCK( p, op, 3*sizeof(op) );
}

void	APU_FME7::LoadState( LPBYTE p )
{
	GETBYTE( p, address );

	GETBLOCK( p, &envelope, sizeof(envelope) );
	GETBLOCK( p, &noise, sizeof(noise) );
	GETBLOCK( p, op, 3*sizeof(op) );
}

