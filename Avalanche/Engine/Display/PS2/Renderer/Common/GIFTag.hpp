
inline GIFTag::GIFTag( )
{
	d_GIFBits._NLOOP = 0;
	d_GIFBits._EOP = 0;
	d_GIFBits._PRE = 0;
	d_GIFBits._PRIM = 0;
	d_GIFBits._FLG = 0;
	d_GIFBits._NREG = 0;

	d_GIFRegs[0] = 0;
	d_GIFRegs[1] = 0;
}

inline void GIFTag::setReg( unsigned int RegNo, RegisterID RegVal)
{
	unsigned int		reg, shift;
	unsigned int		mask;

	reg = RegNo >> 3;
	shift = (RegNo & 0x07) << 2;

	mask = (unsigned int)(0xf << shift);

	d_GIFRegs[reg] = (d_GIFRegs[reg] & ~mask) | ((RegVal & 0x0f) << shift);
}

inline GIFTag::GIFTag( unsigned int defNLOOP, unsigned int defEOP, unsigned int defPRE,
				unsigned int defPRIM, unsigned int defFLG, unsigned int defNREGS, unsigned int *pRegs)
{

	d_GIFBits._NLOOP = defNLOOP;
	d_GIFBits._EOP = defEOP;
	d_GIFBits._PRE = defPRE;
	d_GIFBits._PRIM = defPRIM;
	d_GIFBits._FLG = defFLG;
	d_GIFBits._NREG = defNREGS;

	for( unsigned int i = 0; i < 16; i++)
		setReg( i, (RegisterID) pRegs[i]);
}

inline GIFTag::GIFTag( unsigned int defNLOOP, unsigned int defEOP, unsigned int defPRE,
				unsigned int defPRIM, unsigned int defFLG, unsigned int defNREGS, unsigned int Regs1, unsigned int Regs2)
{

	d_GIFBits._NLOOP = defNLOOP;
	d_GIFBits._EOP = defEOP;
	d_GIFBits._PRE = defPRE;
	d_GIFBits._PRIM = defPRIM;
	d_GIFBits._FLG = defFLG;
	d_GIFBits._NREG = defNREGS;

	d_GIFRegs[0] = Regs1;
	d_GIFRegs[1] = Regs2;
}

inline void GIFTag::set( const GIFTag &defTag)
{
	d_GIFBits = defTag.d_GIFBits;

	d_GIFRegs[0] = defTag.d_GIFRegs[0];
	d_GIFRegs[1] = defTag.d_GIFRegs[1];

}

inline GIFTag::GIFTag( const GIFTag &defTag)
{
	set( defTag);
}

inline GIFTag & GIFTag::operator=( const GIFTag &defTag)
{
	set( defTag);
	return *this;
}

inline GIFTag::RegisterID GIFTag::getReg( unsigned int RegNo) const
{
unsigned int		reg, shift;

	reg = RegNo / 8;
	shift = RegNo % 8;

	return (RegisterID) ((d_GIFRegs[reg] >> shift) & 0x0f);
}

