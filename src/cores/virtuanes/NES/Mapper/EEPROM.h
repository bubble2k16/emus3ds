class	X24C01
{
public:
	X24C01() {
		now_state  = X24C01_IDLE;
		next_state = X24C01_IDLE;
		addr       = 0;
		data       = 0;
		sda        = 0xFF;
		scl_old    = 0;
		sda_old    = 0;

		pEEPDATA   = NULL;
	}

	void	Reset( LPBYTE ptr ) {
		now_state  = X24C01_IDLE;
		next_state = X24C01_IDLE;
		addr       = 0;
		data       = 0;
		sda        = 0xFF;
		scl_old    = 0;
		sda_old    = 0;

		pEEPDATA   = ptr;
	}

	void	Write( BYTE scl_in, BYTE sda_in ) {
		// Clock line
		BYTE	scl_rise = ~scl_old & scl_in;
		BYTE	scl_fall = scl_old & ~scl_in;
		// Data line
		BYTE	sda_rise = ~sda_old & sda_in;
		BYTE	sda_fall = sda_old & ~sda_in;

		BYTE	scl_old_temp = scl_old;
		BYTE	sda_old_temp = sda_old;

		scl_old = scl_in;
		sda_old = sda_in;

		// Start condition?
		if( scl_old_temp && sda_fall ) {
			now_state = X24C01_ADDRESS;
			bitcnt    = 0;
			addr      = 0;
			sda       = 0xFF;
			return;
		}

		// Stop condition?
		if( scl_old_temp && sda_rise ) {
			now_state = X24C01_IDLE;
			sda       = 0xFF;
			return;
		}

		// SCL ____---- RISE
		if( scl_rise ) {
			switch( now_state ) {
				case	X24C01_ADDRESS:
					if( bitcnt < 7 ) {
						// –{—ˆ‚ÍMSB->LSB
						addr &= ~(1<<bitcnt);
						addr |= (sda_in?1:0)<<bitcnt;
					} else {
						if( sda_in ) {
							next_state = X24C01_READ;
							data       = pEEPDATA[addr&0x7F];
						} else {
							next_state = X24C01_WRITE;
						}
					}
					bitcnt++;
					break;
				case	X24C01_ACK:
					sda = 0;	// ACK
					break;
				case	X24C01_READ:
					if( bitcnt < 8 ) {
						// –{—ˆ‚ÍMSB->LSB
						sda = (data&(1<<bitcnt))?1:0;
					}
					bitcnt++;
					break;
				case	X24C01_WRITE:
					if( bitcnt < 8 ) {
						// –{—ˆ‚ÍMSB->LSB
						data &= ~(1<<bitcnt);
						data |= (sda_in?1:0)<<bitcnt;
					}
					bitcnt++;
					break;

				case	X24C01_ACK_WAIT:
					if( !sda_in ) {
						next_state = X24C01_IDLE;
					}
					break;
			}
		}

		// SCL ----____ FALL
		if( scl_fall ) {
			switch( now_state ) {
				case	X24C01_ADDRESS:
					if( bitcnt >= 8 ) {
						now_state = X24C01_ACK;
						sda       = 0xFF;
					}
					break;
				case	X24C01_ACK:
					now_state = next_state;
					bitcnt    = 0;
					sda       = 0xFF;
					break;
				case	X24C01_READ:
					if( bitcnt >= 8 ) {
						now_state = X24C01_ACK_WAIT;
						addr      = (addr+1)&0x7F;
					}
					break;
				case	X24C01_WRITE:
					if( bitcnt >= 8 ) {
						now_state  = X24C01_ACK;
						next_state = X24C01_IDLE;
						pEEPDATA[addr&0x7F] = data;
						addr       = (addr+1)&0x7F;
					}
					break;
			}
		}
	}

	BYTE	Read() {
		return	sda;
	}

	// For State save
	void	Load( LPBYTE p )
	{
		now_state  = *((INT*)&p[0]);
		next_state = *((INT*)&p[4]);
		bitcnt     = *((INT*)&p[8]);
		addr       = p[12];
		data       = p[13];
		sda        = p[14];
		scl_old    = p[15];
		sda_old    = p[16];
	}

	void	Save( LPBYTE p )
	{
		*((INT*)&p[0]) = now_state;
		*((INT*)&p[4]) = next_state;
		*((INT*)&p[8]) = bitcnt;
		p[12]          = addr;
		p[13]          = data;
		p[14]          = sda;
		p[15]          = scl_old;
		p[16]          = sda_old;
	}

protected:
	enum	{
		X24C01_IDLE = 0,	// Idle
		X24C01_ADDRESS,		// Address set
		X24C01_READ,		// Read
		X24C01_WRITE,		// Write
		X24C01_ACK,		// Acknowledge
		X24C01_ACK_WAIT,	// Acknowledge wait
	};

	INT	now_state, next_state;
	INT	bitcnt;
	BYTE	addr, data;
	BYTE	sda;
	BYTE	scl_old, sda_old;

	LPBYTE	pEEPDATA;
private:
};

class	X24C02
{
public:
	X24C02() {
		now_state  = X24C02_IDLE;
		next_state = X24C02_IDLE;
		addr       = 0;
		data       = 0;
		rw         = 0;
		sda        = 0xFF;
		scl_old    = 0;
		sda_old    = 0;

		pEEPDATA   = NULL;
	}

	void	Reset( LPBYTE ptr ) {
		now_state  = X24C02_IDLE;
		next_state = X24C02_IDLE;
		addr       = 0;
		data       = 0;
		rw         = 0;
		sda        = 0xFF;
		scl_old    = 0;
		sda_old    = 0;

		pEEPDATA   = ptr;
	}

	void	Write( BYTE scl_in, BYTE sda_in ) {
		// Clock line
		BYTE	scl_rise = ~scl_old & scl_in;
		BYTE	scl_fall = scl_old & ~scl_in;
		// Data line
		BYTE	sda_rise = ~sda_old & sda_in;
		BYTE	sda_fall = sda_old & ~sda_in;

		BYTE	scl_old_temp = scl_old;
		BYTE	sda_old_temp = sda_old;

		scl_old = scl_in;
		sda_old = sda_in;

		// Start condition?
		if( scl_old_temp && sda_fall ) {
			now_state = X24C02_DEVADDR;
			bitcnt    = 0;
			sda       = 0xFF;
			return;
		}

		// Stop condition?
		if( scl_old_temp && sda_rise ) {
			now_state = X24C02_IDLE;
			sda       = 0xFF;
			return;
		}

		// SCL ____---- RISE
		if( scl_rise ) {
			switch( now_state ) {
				case	X24C02_DEVADDR:
					if( bitcnt < 8 ) {
						data &= ~(1<<(7-bitcnt));
						data |= (sda_in?1:0)<<(7-bitcnt);
					}
					bitcnt++;
					break;
				case	X24C02_ADDRESS:
					if( bitcnt < 8 ) {
						addr &= ~(1<<(7-bitcnt));
						addr |= (sda_in?1:0)<<(7-bitcnt);
					}
					bitcnt++;
					break;
				case	X24C02_READ:
					if( bitcnt < 8 ) {
						sda = (data&(1<<(7-bitcnt)))?1:0;
					}
					bitcnt++;
					break;
				case	X24C02_WRITE:
					if( bitcnt < 8 ) {
						data &= ~(1<<(7-bitcnt));
						data |= (sda_in?1:0)<<(7-bitcnt);
					}
					bitcnt++;
					break;
				case	X24C02_NAK:
					sda = 0xFF;	// NAK
					break;
				case	X24C02_ACK:
					sda = 0;	// ACK
					break;
				case	X24C02_ACK_WAIT:
					if( !sda_in ) {
						next_state = X24C02_READ;
						data       = pEEPDATA[addr];
					}
					break;
			}
		}

		// SCL ----____ FALL
		if( scl_fall ) {
			switch( now_state ) {
				case	X24C02_DEVADDR:
					if( bitcnt >= 8 ) {
						if( (data & 0xA0) == 0xA0 ) {
							now_state  = X24C02_ACK;
							rw         = data & 0x01;
							sda        = 0xFF;
							if( rw ) {
								// Now address read
								next_state = X24C02_READ;
								data       = pEEPDATA[addr];
							} else {
								next_state = X24C02_ADDRESS;
							}
							bitcnt = 0;
						} else {
							now_state  = X24C02_NAK;
							next_state = X24C02_IDLE;
							sda        = 0xFF;
						}
					}
					break;
				case	X24C02_ADDRESS:
					if( bitcnt >= 8 ) {
						now_state = X24C02_ACK;
						sda       = 0xFF;
						if( rw ) {
							// Read‚Å‚Íâ‘Î—ˆ‚È‚¢‚ª”O‚Ìˆ×
							next_state = X24C02_IDLE;
						} else {
							// to Data Write
							next_state = X24C02_WRITE;
						}
						bitcnt = 0;
					}
					break;
				case	X24C02_READ:
					if( bitcnt >= 8 ) {
						now_state = X24C02_ACK_WAIT;
						addr      = (addr+1)&0xFF;
					}
					break;
				case	X24C02_WRITE:
					if( bitcnt >= 8 ) {
						pEEPDATA[addr] = data;
						now_state  = X24C02_ACK;
						next_state = X24C02_WRITE;
						addr       = (addr+1)&0xFF;
						bitcnt = 0;
					}
					break;
				case	X24C02_NAK:
					now_state = X24C02_IDLE;
					bitcnt    = 0;
					sda       = 0xFF;
					break;
				case	X24C02_ACK:
					now_state = next_state;
					bitcnt    = 0;
					sda       = 0xFF;
					break;
				case	X24C02_ACK_WAIT:
					now_state = next_state;
					bitcnt    = 0;
					sda       = 0xFF;
					break;
			}
		}
	}

	BYTE	Read() {
		return	sda;
	}

	// For State save
	void	Load( LPBYTE p )
	{
		now_state  = *((INT*)&p[0]);
		next_state = *((INT*)&p[4]);
		bitcnt     = *((INT*)&p[8]);
		addr       = p[12];
		data       = p[13];
		rw         = p[14];
		sda        = p[15];
		scl_old    = p[16];
		sda_old    = p[17];
	}

	void	Save( LPBYTE p )
	{
		*((INT*)&p[0]) = now_state;
		*((INT*)&p[4]) = next_state;
		*((INT*)&p[8]) = bitcnt;
		p[12]          = addr;
		p[13]          = data;
		p[14]          = rw;
		p[15]          = sda;
		p[16]          = scl_old;
		p[17]          = sda_old;
	}

protected:
	enum	{
		X24C02_IDLE = 0,	// Idle
		X24C02_DEVADDR,		// Device address set
		X24C02_ADDRESS,		// Address set
		X24C02_READ,		// Read
		X24C02_WRITE,		// Write
		X24C02_ACK,		// Acknowledge
		X24C02_NAK,		// Not Acknowledge
		X24C02_ACK_WAIT,	// Acknowledge wait
	};

	INT	now_state, next_state;
	INT	bitcnt;
	BYTE	addr, data, rw;
	BYTE	sda;
	BYTE	scl_old, sda_old;

	LPBYTE	pEEPDATA;
private:
};

