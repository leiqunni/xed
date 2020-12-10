/************************************************************************/
/*																		*/
/*	�v���O�������ėp�f�[�^�x�[�X�V�X�e�� by H.Watanabe					*/
/*	Ver0.00	2009/01/09 21:15:32	�쐬�J�n								*/
/*																		*/
/************************************************************************/

#	ifndef	DB2_H
#	define	DB2_H
#	ifdef	__cplusplus
	extern "C"{
#	endif

#	ifndef	DBALIGN														/* �A���C�����ݒ肳��Ă��Ȃ�*/
#		if		defined(__MWERKS__)										/* PS2(�R�[�h�E�H�[���A)*/
#			define	DBALIGN		16										/* �A���C���T�C�Y*/
#		elif	defined(__gdb__) || defined(__MIPSEL__)					/* GreenHills, GCC MIPS3 for TX4923*/
#			define	DBALIGN		8										/* �A���C���T�C�Y*/
#		else															/* ���Ƀ^�[�Q�b�g�̎w��͖���*/
#			define	DBALIGN		4										/* �A���C���T�C�Y*/
#		endif
#	endif

#	define	DBSIZ_VAR		(-1)										/* �σT�C�Y*/
#	define	DBSIZ_STR		(-2)										/* ������*/
#	define	DBSIZ_RSV		(-3)										/* �啶������������ʂ��Ȃ�������*/
#	define	DBSIZ_LBL		(-4)										/* ���x��*/
#	define	DBSIZ_INT		(-5)										/* ���l*/
#	define	DBSIZ_PTR		(-6)										/* �|�C���^*/

#	if	defined(_WIN32) || defined(__CYGWIN__)							/* Windows*/
#		define	DBSIZ_FNAM	DBSIZ_RSV									/* �啶������������ʂ��Ȃ�*/
#	else																/* unix*/
#		define	DBSIZ_FNAM	DBSIZ_STR									/* �啶������������ʂ���*/
#	endif

#	define	dbLNKP(P)		(((stDBLNK*)(P))-1)							/* �����}�N��*/
#	define	dbALG(V)		(((V)+(DBALIGN-1))&((unsigned)(-DBALIGN)))	/* �����}�N��*/
#	define	dbLSTSIZ(P)		(dbLNKP(P)->lstsiz)							/* ���[�U���X�g�T�C�Y�𓾂�*/
#	define	dbKEYSIZ(P)		(dbLNKP(P)->keysiz)							/* ���[�U�L�[�T�C�Y�𓾂�*/
#	define	dbKEYADR(P)		((void*)(((char*)(P))+dbALG(dbLSTSIZ(P))))	/* ���[�U�L�[�A�h���X�𓾂�*/
#	define	dbFOR(H,P,C)	for(P=(C)dbTop(H);P;P=(C)dbNxt(P))			/* ���[�U�[�f�[�^�����胋�[�v*/
#	define	dbFOR2(H,P,N,C)	for(P=(C)dbTop(H);N=(C)dbNxt(P),P;P=N)		/* ���[�U�[�f�[�^�����胋�[�v*/
#	define	dbBAK(H,P,C)	for(P=(C)dbEnd(H);P;P=(C)dbBak(P))			/* ���[�U�[�f�[�^�����胋�[�v�t��*/
#	define	dbBAK2(H,P,N,C)	for(P=(C)dbEnd(H);N=(C)dbBak(P),P;P=N)		/* ���[�U�[�f�[�^�����胋�[�v�t��*/

	typedef struct tgDB		stDB;										// �f�[�^�x�[�X�Ǘ��\����

	typedef struct	tgDBLNK{											// �����N�\����
		struct tgDBLNK		*nxtp, *bakp;								// �o�������X�g�|�C���^
		int					lstsiz;										// ���[�U�[���X�g�T�C�Y
		int					keysiz;										// ���[�U�[�L�[�T�C�Y
	}	stDBLNK;

	stDB	*dbOpen( int keysiz, int lstsiz, int hashn, int hpsiz );	// �f�[�^�x�[�X���I�[�v�����܂�
	void	dbClose( stDB *dbp );										// �f�[�^�x�[�X���N���[�Y���܂�
	void	*dbAlcS( stDB *dbp, const void *keyadr, int keysiz, const void *lstadr, int lstsiz );				// �f�[�^�x�[�X�Ƀf�[�^���t���X�y�b�N�œo�^���܂�
	void	*dbAlc( stDB *dbp, ... );									// �f�[�^�x�[�X�Ƀf�[�^��o�^���܂�
	void	*dbSch( const stDB *dbp, ... );								// �f�[�^�x�[�X����L�[�ɂ��������Č������܂�
	void	dbFre( stDB *dbp, void *lstp );								// �f�[�^�x�[�X����f�[�^���J�����܂�
	int		dbAlcN( const stDB *dbp );									// �f�[�^�x�[�X�ɓo�^����Ă������Ԃ��܂�
	void	*dbTop( const stDB *dbp );									// �f�[�^�x�[�X�ɓo�^����Ă���擪�f�[�^�̃|�C���^��Ԃ��܂�
	void	*dbEnd( const stDB *dbp );									// �f�[�^�x�[�X�ɓo�^����Ă���I�[�f�[�^�̃|�C���^��Ԃ��܂�
	void	*dbNxt( const void *lstp );									// ���̃f�[�^�|�C���^��Ԃ��܂�
	void	*dbBak( const void *lstp );									// �O�̃f�[�^�|�C���^��Ԃ��܂�
	void	*dbAryOpen( stDB *dbp, void *lstpa );						// �f�[�^�|�C���^�̔z����쐬���܂�
	void	dbAryClose( void *lstpa );									// �f�[�^�|�C���^�̔z����J�����܂�
	void	*dbMemAlc( stDB *dbp, int siz );							// �f�[�^�x�[�X���烍�[�J�����������m�ۂ��܂�( �f�[�^�x�[�X���N���[�Y����Ǝ����I�ɊJ�����܂� )
	void	dbMemFre( void *usrp );										// �f�[�^�x�[�X����m�ۂ������[�J�����������J�����܂�
	void	*dbMemAlcClr( stDB *dbp, int siz );							// ���[�J���������m�ۂ����ăN���A����
	void	*dbMemDup( stDB *dbp, const void *bufadr, int bufsiz );		// ���[�J���������m�ۂ����ăR�s�[����( bufadr �� NULL �̏ꍇ�̓N���A )
	int		dbMemSiz( const stDB *dbp );								// �������A���P�[�V���������T�C�Y���v�����߂�
	char	*dbStrAlc( stDB *dbp, const char *strp );					// ������o�^
	stDB	*dbDup( const stDB *dbp );									// �f�[�^�x�[�X�̕������쐬����
	void	*dbLstAdr( const stDB *dbp, int lstno );					// �m�Ԗڂ̃��X�g�A�h���X�𓾂�
	void	dbMov( stDB *dbp, void *lstp, void *nxtp );					// �f�[�^�x�[�X�̓o�^���Ԃ�ύX����
	void	*dbSeek( int *seeknp, const void *lstp, int seekofs );		// �w�肵���I�t�Z�b�g���V�[�N����
	void	*dbSchNxt( const stDB *dbp, const void *lstp );				// ���Ɉ�v������̂�����
	void	dbAddDB( stDB *dstdbp, const stDB *adddbp );				// �ǉ�
	void	dbClear( stDB *dbp );										// �N���A
	int		dbLstNo( stDB *dbp, const void *lstp );						// ���X�g�̔ԍ��𓾂�
	int		dbLblLen( const char *txtadr, int bufsiz );					// �������x���������߂�
	char	*dbGetLbl( char *bufadr, int bufsiz, const char *txtp );	// �������x���𓾂�
	char	*dbGetDQ( char *dstp, int dstsiz, const char *txtp );		// �_�u���N�H�[�e�[�V�����ň͂܂ꂽ������𓾂�
	int		dbMemCaseCmp( const void *dstadr, const void *souadr, int siz );	// �啶������������ʂ��Ȃ������Ή���������r
	int		dbStrCaseCmp( const char *dstp, const char *soup );			// �啶������������ʂ��Ȃ������Ή��������r

#	ifdef	__cplusplus
	}
#	endif
#	endif

