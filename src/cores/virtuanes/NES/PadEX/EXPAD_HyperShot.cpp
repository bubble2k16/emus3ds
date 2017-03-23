//////////////////////////////////////////////////////////////////////////
// Hyper Shot                                                           //
//////////////////////////////////////////////////////////////////////////
void	EXPAD_HyperShot::Reset()
{
	shotbits = 0;
}

void	EXPAD_HyperShot::Strobe()
{
	shotbits = 0;
	shotbits |= (nes->pad->pad1bit&0x03)<<1;
	shotbits |= (nes->pad->pad2bit&0x03)<<3;
}

BYTE	EXPAD_HyperShot::Read4017()
{
	return	shotbits;
}
