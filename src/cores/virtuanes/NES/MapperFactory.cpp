/*----------------------------------------------------------------------*/
/*                                                                      */
/*      NES Mapeers Factory                                             */
/*                                                           Norix      */
/*                                               written     2003/09/04 */
/*                                               last modify ----/--/-- */
/*----------------------------------------------------------------------*/
#define	WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "DebugOut.h"
#include "typedef.h"
#include "macro.h"

#include "3dsdbg.h"
#include "nes.h"
#include "mmu_fceux.h" 
#include "mmu.h" 
#include "cpu.h"
#include "ppu.h"
#include "apu.h"
#include "pad.h"
#include "rom.h"

#include "mapper.h"

#include "Config.h"

//////////////////////////////////////////////////////////////////////////
#include "MapperFCEUX.h"
#include "Mapper000.h"
#include "Mapper001.h"
#include "Mapper002.h"
#include "Mapper003.h"
#include "Mapper004.h"
#include "Mapper005.h"
#include "Mapper006.h"
#include "Mapper007.h"
#include "Mapper008.h"
#include "Mapper009.h"
#include "Mapper010.h"
#include "Mapper011.h"
#include "Mapper013.h"
#include "Mapper015.h"
#include "Mapper016.h"
#include "Mapper017.h"
#include "Mapper018.h"
#include "Mapper019.h"
#include "MapperFDS.h"	// 020
#include "Mapper021.h"
#include "Mapper022.h"
#include "Mapper023.h"
#include "Mapper024.h"
#include "Mapper025.h"
#include "Mapper026.h"
#include "Mapper028.h"
#include "Mapper032.h"
#include "Mapper033.h"
#include "Mapper034.h"
#include "Mapper036.h"
#include "Mapper040.h"
#include "Mapper041.h"
#include "Mapper042.h"
#include "Mapper043.h"
#include "Mapper048.h"
#include "Mapper064.h"
#include "Mapper065.h"
#include "Mapper066.h"
#include "Mapper067.h"
#include "Mapper068.h"
#include "Mapper069.h"
#include "Mapper070.h"
#include "Mapper071.h"
#include "Mapper072.h"
#include "Mapper073.h"
#include "Mapper075.h"
#include "Mapper076.h"
#include "Mapper077.h"
#include "Mapper078.h"
#include "Mapper079.h"
#include "Mapper080.h"
#include "Mapper082.h"
#include "Mapper083.h"
#include "Mapper085.h"
#include "Mapper086.h"
#include "Mapper087.h"
#include "Mapper088.h"
#include "Mapper089.h"
#include "Mapper090.h"
#include "Mapper091.h"
#include "Mapper092.h"
#include "Mapper093.h"
#include "Mapper094.h"
#include "Mapper095.h"
#include "Mapper096.h"
#include "Mapper097.h"
#include "Mapper100.h"
#include "Mapper101.h"
#include "Mapper107.h"
#include "Mapper111.h"
#include "Mapper113.h"
#include "Mapper118.h"
#include "Mapper119.h"
#include "Mapper122.h"
#include "Mapper175.h"
#include "Mapper180.h"
#include "Mapper181.h"
#include "Mapper185.h"
#include "Mapper187.h"
#include "Mapper188.h"
#include "Mapper189.h"
#include "Mapper243.h"

#include "MapperNSF.h"

#include "Mapper044.h"
#include "Mapper045.h"
#include "Mapper046.h"
#include "Mapper047.h"
#include "Mapper050.h"
#include "Mapper051.h"
#include "Mapper057.h"
#include "Mapper058.h"
#include "Mapper060.h"
#include "Mapper062.h"
#include "Mapper074.h"
#include "Mapper105.h"
#include "Mapper108.h"
#include "Mapper109.h"
#include "Mapper110.h"
#include "Mapper112.h"
#include "Mapper114.h"
#include "Mapper115.h"
#include "Mapper116.h"
#include "Mapper117.h"
#include "Mapper133.h"
#include "Mapper134.h"
#include "Mapper135.h"
#include "Mapper140.h"
#include "Mapper142.h"
#include "Mapper160.h"
#include "Mapper162.h"
#include "Mapper182.h"
#include "Mapper183.h"
#include "Mapper190.h"
#include "Mapper191.h"
#include "Mapper193.h"
#include "Mapper194.h"
#include "Mapper198.h"
#include "Mapper199.h"
#include "Mapper222.h"
#include "Mapper225.h"
#include "Mapper226.h"
#include "Mapper227.h"
#include "Mapper228.h"
#include "Mapper229.h"
#include "Mapper230.h"
#include "Mapper231.h"
#include "Mapper232.h"
#include "Mapper233.h"
#include "Mapper234.h"
#include "Mapper235.h"
#include "Mapper236.h"
#include "Mapper240.h"
#include "Mapper241.h"
#include "Mapper242.h"
#include "Mapper244.h"
#include "Mapper245.h"
#include "Mapper246.h"
#include "Mapper248.h"
#include "Mapper249.h"
#include "Mapper251.h"
#include "Mapper252.h"
#include "Mapper253.h"
#include "Mapper254.h"
#include "Mapper255.h"

#include "Mapper099.h"
#include "Mapper151.h"

#include "Mapper012.h"

#include "Mapper200.h"
#include "Mapper201.h"
#include "Mapper202.h"

#include "Mapper061.h"

#include "Mapper027.h"

#include "Mapper163.h"
#include "Mapper164.h"
#include "Mapper165.h"
#include "Mapper167.h"

//////////////////////////////////////////////////////////////////////////

#include "Mapper000.cpp"
#include "Mapper001.cpp"
#include "Mapper002.cpp"
#include "Mapper003.cpp"
#include "Mapper004.cpp"
#include "Mapper163.cpp"
#include "Mapper005.cpp"
#include "Mapper006.cpp"
#include "Mapper007.cpp"
#include "Mapper008.cpp"
#include "Mapper009.cpp"
#include "Mapper010.cpp"
#include "Mapper011.cpp"
#include "Mapper013.cpp"
#include "Mapper015.cpp"
#include "Mapper016.cpp"
#include "Mapper017.cpp"
#include "Mapper018.cpp"
#include "Mapper019.cpp"
#include "MapperFDS.cpp"	// 020
#include "Mapper021.cpp"
#include "Mapper022.cpp"
#include "Mapper023.cpp"
#include "Mapper024.cpp"
#include "Mapper025.cpp"
#include "Mapper026.cpp"
#include "Mapper032.cpp"
#include "Mapper033.cpp"
#include "Mapper034.cpp"
#include "Mapper040.cpp"
#include "Mapper041.cpp"
#include "Mapper042.cpp"
#include "Mapper043.cpp"
#include "Mapper048.cpp"
#include "Mapper064.cpp"
#include "Mapper065.cpp"
#include "Mapper066.cpp"
#include "Mapper067.cpp"
#include "Mapper068.cpp"
#include "Mapper069.cpp"
#include "Mapper070.cpp"
#include "Mapper071.cpp"
#include "Mapper072.cpp"
#include "Mapper073.cpp"
#include "Mapper075.cpp"
#include "Mapper076.cpp"
#include "Mapper077.cpp"
#include "Mapper078.cpp"
#include "Mapper079.cpp"
#include "Mapper080.cpp"
#include "Mapper082.cpp"
#include "Mapper083.cpp"
#include "Mapper085.cpp"
#include "Mapper086.cpp"
#include "Mapper087.cpp"
#include "Mapper088.cpp"
#include "Mapper089.cpp"
#include "Mapper090.cpp"
#include "Mapper091.cpp"
#include "Mapper092.cpp"
#include "Mapper093.cpp"
#include "Mapper094.cpp"
#include "Mapper095.cpp"
#include "Mapper096.cpp"
#include "Mapper097.cpp"
#include "Mapper100.cpp"
#include "Mapper101.cpp"
#include "Mapper107.cpp"
#include "Mapper111.cpp"
#include "Mapper113.cpp"
#include "Mapper118.cpp"
#include "Mapper119.cpp"
#include "Mapper122.cpp"
#include "Mapper175.cpp"
#include "Mapper180.cpp"
#include "Mapper181.cpp"
#include "Mapper185.cpp"
#include "Mapper187.cpp"
#include "Mapper188.cpp"
#include "Mapper189.cpp"
#include "Mapper243.cpp"

#include "MapperNSF.cpp"

#include "Mapper044.cpp"
#include "Mapper045.cpp"
#include "Mapper046.cpp"
#include "Mapper047.cpp"
#include "Mapper050.cpp"
#include "Mapper051.cpp"
#include "Mapper057.cpp"
#include "Mapper058.cpp"
#include "Mapper060.cpp"
#include "Mapper062.cpp"
#include "Mapper074.cpp"
#include "Mapper105.cpp"
#include "Mapper108.cpp"
#include "Mapper109.cpp"
#include "Mapper110.cpp"
#include "Mapper112.cpp"
#include "Mapper114.cpp"
#include "Mapper115.cpp"
#include "Mapper116.cpp"
#include "Mapper117.cpp"
#include "Mapper133.cpp"
#include "Mapper134.cpp"
#include "Mapper135.cpp"
#include "Mapper140.cpp"
#include "Mapper142.cpp"
#include "Mapper160.cpp"
#include "Mapper162.cpp"
#include "Mapper182.cpp"
#include "Mapper183.cpp"
#include "Mapper190.cpp"
#include "Mapper191.cpp"
#include "Mapper193.cpp"
#include "Mapper194.cpp"
#include "Mapper198.cpp"
#include "Mapper199.cpp"
#include "Mapper222.cpp"
#include "Mapper225.cpp"
#include "Mapper226.cpp"
#include "Mapper227.cpp"
#include "Mapper228.cpp"
#include "Mapper229.cpp"
#include "Mapper230.cpp"
#include "Mapper231.cpp"
#include "Mapper232.cpp"
#include "Mapper233.cpp"
#include "Mapper234.cpp"
#include "Mapper235.cpp"
#include "Mapper236.cpp"
#include "Mapper240.cpp"
#include "Mapper241.cpp"
#include "Mapper242.cpp"
#include "Mapper244.cpp"
#include "Mapper245.cpp"
#include "Mapper246.cpp"
#include "Mapper248.cpp"
#include "Mapper249.cpp"
#include "Mapper251.cpp"
#include "Mapper252.cpp"
#include "Mapper253.cpp"
#include "Mapper254.cpp"
#include "Mapper255.cpp"

#include "Mapper099.cpp"
#include "Mapper151.cpp"

#include "Mapper012.cpp"

#include "Mapper200.cpp"
#include "Mapper201.cpp"
#include "Mapper202.cpp"

#include "Mapper061.cpp"

#include "Mapper027.cpp"

#include "Mapper164.cpp"
#include "Mapper165.cpp"
#include "Mapper167.cpp"

//////////////////////////////////////////////////////////////////////////
// Mapper Factory
//////////////////////////////////////////////////////////////////////////
Mapper*	CreateMapper( NES* parent, INT no )
{
	switch( no ) {
		case	0:
			return new Mapper000(parent);
		case	1:
			return new Mapper001(parent);
		case	2:
			return new Mapper002(parent);
		case	3:
			return new Mapper003(parent);
		case	4:
			return new Mapper004(parent);
		case	5:
			return new Mapper005(parent);
		case	6:
			return new Mapper006(parent);
		case	7:
			return new Mapper007(parent);
		case	8:
			return new Mapper008(parent);
		case	9:
			return new Mapper009(parent);
		case	10:
			return new Mapper010(parent);
		case	11:
			return new Mapper011(parent);
		case	13:
			return new Mapper013(parent);
		case	15:
			return new Mapper015(parent);
		case	16:
			return new Mapper016(parent);
		case	17:
			return new Mapper017(parent);
		case	18:
			return new Mapper018(parent);
		case	19:
			return new Mapper019(parent);
		case	20:
			return new Mapper020(parent);
		case	21:
			return new Mapper021(parent);
		case	22:
			return new Mapper022(parent);
		case	23:
			return new Mapper023(parent);
		case	24:
			return new Mapper024(parent);
		case	25:
			return new Mapper025(parent);
		case	26:
			return new Mapper026(parent);
		case	28:
			return new MapperFCEUX(parent, 
				fceux_mapper028::M28Power, 
				fceux_mapper028::M28Reset, 
				fceux_mapper028::StateRestore,
				fceux_mapper028::StateRegs);
		case	32:
			return new Mapper032(parent);
		case	33:
			return new Mapper033(parent);
		case	34:
			return new Mapper034(parent);
		case	35:
			return new Mapper035(parent);
		case	36:
			return new MapperFCEUX(parent, 
				fceux_mapper036::M36Power, 
				NULL, 
				fceux_mapper036::M36Restore,
				fceux_mapper036::StateRegs);
		case	40:
			return new Mapper040(parent);
		case	41:
			return new Mapper041(parent);
		case	42:
			return new Mapper042(parent);
		case	43:
			return new Mapper043(parent);
		case	48:
			return new Mapper048(parent);
		case	64:
			return new Mapper064(parent);
		case	65:
			return new Mapper065(parent);
		case	66:
			return new Mapper066(parent);
		case	67:
			return new Mapper067(parent);
		case	68:
			return new Mapper068(parent);
		case	69:
			return new Mapper069(parent);
		case	70:
			return new Mapper070(parent);
		case	71:
			return new Mapper071(parent);
		case	72:
			return new Mapper072(parent);
		case	73:
			return new Mapper073(parent);
		case	75:
			return new Mapper075(parent);
		case	76:
			return new Mapper076(parent);
		case	77:
			return new Mapper077(parent);
		case	78:
			return new Mapper078(parent);
		case	79:
			return new Mapper079(parent);
		case	80:
			return new Mapper080(parent);
		case	82:
			return new Mapper082(parent);
		case	83:
			return new Mapper083(parent);
		case	85:
			return new Mapper085(parent);
		case	86:
			return new Mapper086(parent);
		case	87:
			return new Mapper087(parent);
		case	88:
			return new Mapper088(parent);
		case	89:
			return new Mapper089(parent);
		case	90:
			return new Mapper090(parent);
		case	91:
			return new Mapper091(parent);
		case	92:
			return new Mapper092(parent);
		case	93:
			return new Mapper093(parent);
		case	94:
			return new Mapper094(parent);
		case	95:
			return new Mapper095(parent);
		case	96:
			return new Mapper096(parent);
		case	97:
			return new Mapper097(parent);
		case	100:
			return new Mapper100(parent);
		case	101:
			return new Mapper101(parent);
		case	107:
			return new Mapper107(parent);
		case	111:
			return new Mapper111(parent);
		case	113:
			return new Mapper113(parent);
		case	118:
			return new Mapper118(parent);
		case	119:
			return new Mapper119(parent);
		case	122:
		case	184:
			return new Mapper122(parent);
		case	175:
			return new Mapper175(parent);
		//case	176:
		//	return new Mapper176(parent);
		case	177:
			return new Mapper177(parent);
		case	178:
			return new Mapper178(parent);		
		case	180:
			return new Mapper180(parent);
		case	181:
			return new Mapper181(parent);
		case	185:
			return new Mapper185(parent);
		case	187:
			return new Mapper187(parent);
		case	188:
			return new Mapper188(parent);
		case	189:
			return new Mapper189(parent);
		case	216:
			return new Mapper216(parent);
		case	243:
			return new Mapper243(parent);
		case	0x100:
			return new MapperNSF(parent);

		case	44:
			return new Mapper044(parent);
		case	45:
			return new Mapper045(parent);
		case	46:
			return new Mapper046(parent);
		case	47:
			return new Mapper047(parent);
		case	50:
			return new Mapper050(parent);
		case	51:
			return new Mapper051(parent);
		case	57:
			return new Mapper057(parent);
		case	58:
			return new Mapper058(parent);
		case	60:
			return new Mapper060(parent);
		case	62:
			return new Mapper062(parent);
		case	74:
			return new Mapper074(parent);
		case	105:
			return new Mapper105(parent);
		case	108:
			return new Mapper108(parent);
		case	109:
			return new Mapper109(parent);
		case	110:
			return new Mapper110(parent);
		case	112:
			return new Mapper112(parent);
		case	114:
			return new Mapper114(parent);
		case	115:
			return new Mapper115(parent);
		case	116:
			return new Mapper116(parent);
		case	117:
			return new Mapper117(parent);
		case	133:
			return new Mapper133(parent);
		case	134:
			return new Mapper134(parent);
		case	135:
			return new Mapper135(parent);
		case	140:
			return new Mapper140(parent);
		case	142:
			return new Mapper142(parent);
		case	160:
			return new Mapper160(parent);
		case	162:
			return new Mapper162(parent);
		case	182:
			return new Mapper182(parent);
		case	183:
			return new Mapper183(parent);
		case	190:
			return new Mapper190(parent);
		case	191:
			return new Mapper191(parent);
		case	192:
			return new Mapper192(parent);
		case	193:
			return new Mapper193(parent);
		case	194:
			return new Mapper194(parent);
		case	195:
			return new Mapper195(parent);
		case	198:
			return new Mapper198(parent);
			//return new fceuMMC3(parent,198);
		case	199:
			//return new fceuMMC3(parent,199);
			return new Mapper199(parent);
		case	209:
			return new Mapper209(parent);
		case	211:
			return new Mapper211(parent);
		case	220:
			return new Mapper220(parent);
		case	222:
			return new Mapper222(parent);
		case	225:
			return new Mapper225(parent);
		case	226:
			return new Mapper226(parent);
		case	227:
			return new Mapper227(parent);
		case	228:
			return new Mapper228(parent);
		case	229:
			return new Mapper229(parent);
		case	230:
			return new Mapper230(parent);
		case	231:
			return new Mapper231(parent);
		case	232:
			return new Mapper232(parent);
		case	233:
			return new Mapper233(parent);
		case	234:
			return new Mapper234(parent);
		case	235:
			return new Mapper235(parent);
		case	236:
			return new Mapper236(parent);
		case	240:
			return new Mapper240(parent);
		case	241:
			return new Mapper241(parent);
		case	242:
			return new Mapper242(parent);
		case	244:
			return new Mapper244(parent);
		case	245:
			return new Mapper245(parent);
		case	246:
			return new Mapper246(parent);
		case	248:
			return new Mapper248(parent);
		case	249:
			return new Mapper249(parent);
		case	251:
			return new Mapper251(parent);
		case	252:
			return new Mapper252(parent);
		case	253:
			return new Mapper253(parent);
		case	254:
			return new Mapper254(parent);
		case	255:
			return new Mapper255(parent);

		case	99:
			return new Mapper099(parent);
		case	151:
			return new Mapper151(parent);
		case	156:
			return new Mapper156(parent);

		case	12:
			return new Mapper012(parent);

		case	200:
			return new Mapper200(parent);
		case	201:
			return new Mapper201(parent);
		case	202:
			return new Mapper202(parent);

		case	61:
			return new Mapper061(parent);

		case	27:
			return new Mapper027(parent);

		case	163:
			return new Mapper163(parent);
		case	164:
			return new Mapper164(parent);
		case	165:
			return new Mapper165(parent);
		case	167:
			return new Mapper167(parent);

		default:
			break;
	}

	return	NULL;
}

