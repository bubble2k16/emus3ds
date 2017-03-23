//////////////////////////////////////////////////////////////////////////
// VS-Unisystem                                                         //
//////////////////////////////////////////////////////////////////////////
class	EXPAD_VSUnisystem : public EXPAD
{
public:
	EXPAD_VSUnisystem( NES* parent ) : EXPAD( parent ) {}

	void	Reset();

	BYTE	Read4016();
	BYTE	Read4017();

protected:
private:
};
