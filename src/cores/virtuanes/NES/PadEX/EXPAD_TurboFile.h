//////////////////////////////////////////////////////////////////////////
// TurboFile                                                            //
//////////////////////////////////////////////////////////////////////////
class	EXPAD_TurboFile : public EXPAD
{
public:
	EXPAD_TurboFile( NES* parent ) : EXPAD( parent ) {}

	void	Reset();

	BYTE	Read4017();
	void	Write4016( BYTE data );

protected:
	INT	tf_address;
	BYTE	tf_dataold;
	BYTE	tf_databit;
	BYTE	tf_data;

private:
};
