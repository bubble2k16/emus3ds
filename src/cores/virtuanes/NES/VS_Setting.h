	{
	switch( rom->GetPROM_CRC() ) {
		case	0xeb2dba63:	// VS TKO Boxing
		case	0x9818f656:
			pad->SetVSType( PAD::VS_TYPE0 );
			goto	vsexit;
		case	0xed588f00:	// VS Duck Hunt
			pad->SetVSType( PAD::VS_TYPEZ );
			goto	vsexit;
		case	0x8c0c2df5:	// VS Top Gun
			ppu->SetVSSecurity( 0x1B );
			pad->SetVSType( PAD::VS_TYPE0 );
			goto	vsexit;
		case	0x16d3f469:	// VS Ninja Jajamaru Kun (J)
			ppu->SetVSSecurity( 0x1B );
			pad->SetVSType( PAD::VS_TYPE3 );
			goto	vsexit;
		case	0x8850924b:	// VS Tetris
			pad->SetVSType( PAD::VS_TYPE1 );
			goto	vsexit;
		case	0xcf36261e:	// VS Sky Kid
			pad->SetVSType( PAD::VS_TYPE3 );
			goto	vsexit;
		case	0xe1aa8214:	// VS Star Luster
			pad->SetVSType( PAD::VS_TYPE0 );
			goto	vsexit;
		case	0xec461db9:	// VS Pinball
			ppu->SetVSColorMap( 0 );
			pad->SetVSType( PAD::VS_TYPE0 );
			goto	vsexit;
		case	0xe528f651:	// VS Pinball (alt)
			pad->SetVSType( PAD::VS_TYPE0 );
			goto	vsexit;
		case	0x17ae56be:	// VS Freedom Force
//			ppu->SetVSColorMap( 0 );
			ppu->SetVSColorMap( 4 );
			pad->SetVSType( PAD::VS_TYPEZ );
			goto	vsexit;
		case	0xe2c0a2be:	// VS Platoon
			ppu->SetVSColorMap( 0 );
			pad->SetVSType( PAD::VS_TYPE0 );
			goto	vsexit;
		case	0xff5135a3:	// VS Hogan's Alley
			ppu->SetVSColorMap( 0 );
			pad->SetVSType( PAD::VS_TYPEZ );
			goto	vsexit;
		case	0x70901b25:	// VS Slalom
			ppu->SetVSColorMap( 1 );
			pad->SetVSType( PAD::VS_TYPE0 );
			goto	vsexit;
		case	0x0b65a917:	// VS Mach Rider(Endurance Course)
		case	0x8a6a9848:
			ppu->SetVSColorMap( 1 );
			pad->SetVSType( PAD::VS_TYPE0 );
			goto	vsexit;
		case	0xae8063ef:	// VS Mach Rider(Japan, Fighting Course)
			ppu->SetVSColorMap( 0 );
			pad->SetVSType( PAD::VS_TYPE0 );
			goto	vsexit;
		case	0xcc2c4b5d:	// VS Golf
			ppu->SetVSColorMap( 1 );
//			pad->SetVSType( PAD::VS_TYPE0 );
			pad->SetVSType( PAD::VS_TYPE6 );
			goto	vsexit;
		case	0xa93a5aee:	// VS Stroke and Match Golf
			pad->SetVSType( PAD::VS_TYPE1 );
			goto	vsexit;
		case	0x86167220:	// VS Lady Golf
			ppu->SetVSColorMap( 1 );
			pad->SetVSType( PAD::VS_TYPE1 );
			goto	vsexit;
		case	0xffbef374:	// VS Castlevania
			ppu->SetVSColorMap( 1 );
			pad->SetVSType( PAD::VS_TYPE0 );
			goto	vsexit;
		case	0x135adf7c:	// VS Atari RBI Baseball
			ppu->SetVSColorMap( 2 );
			pad->SetVSType( PAD::VS_TYPE1 );
			goto	vsexit;
		case	0xd5d7eac4:	// VS Dr. Mario
			ppu->SetVSColorMap( 2 );
			pad->SetVSType( PAD::VS_TYPE1 );
			goto	vsexit;
		case	0x46914e3e:	// VS Soccer
			ppu->SetVSColorMap( 2 );
			pad->SetVSType( PAD::VS_TYPE1 );
			goto	vsexit;
		case	0x70433f2c:	// VS Battle City
		case	0x8d15a6e6:	// VS bad .nes
			ppu->SetVSColorMap( 2 );
			pad->SetVSType( PAD::VS_TYPE1 );
			goto	vsexit;
		case	0x1e438d52:	// VS Goonies
			ppu->SetVSColorMap( 2 );
			pad->SetVSType( PAD::VS_TYPE0 );
			goto	vsexit;
		case	0xcbe85490:	// VS Excitebike
			ppu->SetVSColorMap( 2 );
			pad->SetVSType( PAD::VS_TYPE0 );
			goto	vsexit;
		case	0x29155e0c:	// VS Excitebike (alt)
			ppu->SetVSColorMap( 3 );
			pad->SetVSType( PAD::VS_TYPE0 );
			goto	vsexit;
		case	0x07138c06:	// VS Clu Clu Land
			ppu->SetVSColorMap( 3 );
			pad->SetVSType( PAD::VS_TYPE1 );
			goto	vsexit;
		case	0x43a357ef:	// VS Ice Climber
			ppu->SetVSColorMap( 3 );
			pad->SetVSType( PAD::VS_TYPE1 );
			goto	vsexit;
		case	0x737dd1bf:	// VS Super Mario Bros
		case	0x4bf3972d:	// VS Super Mario Bros
		case	0x8b60cc58:	// VS Super Mario Bros
		case	0x8192c804:	// VS Super Mario Bros
			ppu->SetVSColorMap( 3 );
			pad->SetVSType( PAD::VS_TYPE0 );
			goto	vsexit;
		case	0xd99a2087:	// VS Gradius
			ppu->SetVSColorMap( 4 );
			pad->SetVSType( PAD::VS_TYPE1 );
			goto	vsexit;
		case	0xf9d3b0a3:	// VS Super Xevious
		case	0x9924980a:	// VS Super Xevious
		case	0x66bb838f:	// VS Super Xevious
			ppu->SetVSColorMap( 4 );
			pad->SetVSType( PAD::VS_TYPE0 );
			goto	vsexit;

		case	0xc99ec059:	// VS Raid on Bungeling Bay(J)
			ppu->SetVSColorMap( 1 );
			pad->SetVSType( PAD::VS_TYPE5 );
			goto	vsexit;
		case	0xca85e56d:	// VS Mighty Bomb Jack(J)
			ppu->SetVSSecurity( 0x3D );
			pad->SetVSType( PAD::VS_TYPE0 );
			goto	vsexit;

	vsexit:
			ppu->SetVSMode( TRUE );
			break;
	}
	}
