/************************************************************************/
/*																		*/
/*	�I�v�V��������				by H.Watanabe							*/
/*	Ver1.00	2009/04/05 13:52:04	�쐬									*/
/*																		*/
/************************************************************************/

#	include	<stdio.h>													/* �W�����o�͊֐�*/
#	include	<stdlib.h>													/* �W�����C�u�����֐�*/
#	include	<string.h>													/* �W��������֐�*/
#	include	"path2.h"													/* �p�X���֘A*/
#	include	"opt.h"														/* �I�v�V��������*/

/************************************************************************/
/*	���[�J����`														*/
/************************************************************************/

#	define	SKPSP(P)		((P)+strspn(P,"\t "))						/* �X�L�b�v�X�y�[�X*/

	enum{																// �^�C�v��`
		OPTTYP_INC,														// �C���N�������g
		OPTTYP_SET,														// �l���Z�b�g����
		OPTTYP_INT,														// �����l�w��
		OPTTYP_FPTYP,													// ���������w��
		OPTTYP_STRING,													// ������w��
		OPTTYP_DB,														// DB�w��
		OPTTYP_DEFINE,													// ��`�w��
		OPTTYP_USR,														// ���[�U�[�����w��
		OPTTYP_N
	};

	typedef struct{
		int		typ;													// �^�C�v
		union{
			int			*intp;											// �C���g
			FPTYP		*fptypp;										// ��������
			char		*charp;											// ������
			void		*voidp;											// �{�C�h
			stDB		*dbp;											// DB
			tpOPTIONUSR *prcp;											// ���[�U�[�����֐�
		}	u;
		int		extra;													// �g���f�[�^
		char	*commentp;												// �R�����g�|�C���^
	}	stOPT;

	enum{																// �����^�C�v��`
		SCHTYP_PATHNAMEXT,												// �p�X�{���O�{�g���q�w��
		SCHTYP_NAMEXT,													// ���O�{�g���q�w��
		SCHTYP_EXT,														// �g���q�̂ݎw��
		SCHTYP_N
	};

	typedef struct{
		int		schtyp;													// �����^�C�v
	}	stLST;

/************************************************************************/
/*	������𐔒l�ɕϊ�����												*/
/************************************************************************/

static char	*s2ip( int *valp, const char *txtp, int radix ){

	char	chr;														// �e�L�X�g����
	int		uni;														// �P��
	int		val=0;														// ���l�\���l

	for(;;){
		chr=*txtp++;													// �e�L�X�g�|�C���^����P�����𓾂�
		if( chr<='9' ){													// '9'�ȉ�
			uni=(unsigned char)( chr-'0' );								// ��������'0'�������� unsigned char �ŃL���X�g
		}else{															// '9'�ȉ��ł͂Ȃ�
			uni=( (unsigned char)( ( chr|0x20)-'a' ) )+10;				// ��������'a'�������� unsigned char �ŃL���X�g���A10�����Z����
		}
		if( uni>=radix ){												// ����傫��
			break;														// ���[�v�I��
		}
		val=val*radix+uni;												// ��{���Ēl�����Z����
	}
	if( valp ){															// �f�B�X�g�l�[�V�����|�C���^���w�肳��Ă���
		*valp=val;														// �l��������
	}
	return( (char*)( txtp-1 ) );										// �e�L�X�g�|�C���^�͎��̕����ɃV�[�N���Ă���̂Ł|�P�����l��Ԃ�
}

/************************************************************************/
/*	�P�����̕]��														*/
/************************************************************************/

	static char	*chkoptVal( int *dstp, const char *txtp );

static char	*chkoptUni( int *dstp, const char *txtp ){

	unsigned char	chr;
	const char		*bakp;												// �]���G���[�`�F�b�N�̂��߂̃|�C���^
	int				val;

	for(;;){															// �X�L�b�v�X�y�[�X
		chr=*txtp++;													// �e�L�X�g�|�C���^����P�����𓾂�
		if( chr!='+' ){													// '+'�Ȃ�΃��[�v����
			break;
		}
	}
	switch( chr ){
		case '-':														// - �P�����Z�q
			txtp=chkoptUni( &val, txtp );								// �ċA�R�[��
			*dstp=-val;													// NEG ����
			break;
		case '(':
			if( ( txtp=chkoptVal( dstp, txtp ) )!=0 ){					// �ċA�R�[��
				if( *txtp++!=')' ){
					return( 0 );										// �G���[�I��
				}
			}
			break;
		case '\'':
			val=0;
			while( ( chr=*txtp++ )!=0 ){
				if( chr=='\'' ){
					break;
				}
				val=(val<<8)+chr;
			}
			*dstp=val;
			break;
		case '0':														// �擪������'0'
			if( *txtp=='x' || *txtp=='X' ){								// 0x �܂��� 0X �Ŏn�܂�P�U�i��
				txtp=s2ip( dstp, txtp+1, 16 );							// �P�U�i���l�]��
			}else{														// �W�i���܂��͂P�O�i���̂O
				txtp=s2ip( dstp, txtp, 8 );								// �W�i���l�]��
			}
			break;
		default:														// �P�O�i���܂��͐��l�łȂ�
			txtp=s2ip( dstp, bakp=txtp-1, 10 );							// �P�O�i���l�]��
			if( txtp==bakp ){											// �]���s�\.
				return( 0 );
			}
			break;
	}
	return( (char*)txtp );												// �e�L�X�g�|�C���^�������Đ���I��
}

/************************************************************************/
/*	�����]��( Evaluation of Expressions )								*/
/************************************************************************/

	static int chkoptAdd( int dst, int sou ){ return( dst+sou );	}	// ���Z
	static int chkoptSub( int dst, int sou ){ return( dst-sou );	}	// ���Z
	static int chkoptMul( int dst, int sou ){ return( dst*sou );	}	// ��Z
	static int chkoptDiv( int dst, int sou ){ return( dst/sou );	}	// ���Z

static char	*chkoptVal( int *dstp, const char *txtp ){

	struct{																// ���Z�v���C�I���e�B�����̂��߂̕]���\����
		int		pri;													// �v���C�I���e�B
		int		(*prcp)( int , int );									// �����A�h���X
		int		val;													// ���Z����
	}			va[2+1],*vp;											// �]���z��ƕ]���X�^�b�N�|�C���^
	int			(*prcp)( int , int );									// �����A�h���X
	int			pri;													// �v���C�I���e�B

	vp=va;																// �]���X�^�b�N�|�C���^������
	va[0].pri=-2;														// ���
	for(;;){															// �]�����[�v( VC5 �o�O�̂��� do while �̎g�p�֎~ )
		txtp=chkoptUni( &vp->val, txtp );								// �P�����̕]��
		if( !txtp ){
			return( 0 );												// �G���[�I��
		}
		switch( *txtp++ ){												// �e�L�X�g�|�C���^����񍀉��Z�q�𓾂�
			case '+':	pri=12; prcp=chkoptAdd;			break;			// ���Z
			case '-':	pri=12; prcp=chkoptSub;			break;			// ���Z
			case '*':	pri=13; prcp=chkoptMul;			break;			// ��Z
			case '/':	pri=13; prcp=chkoptDiv;			break;			// ���Z
			default:	pri=-1;	prcp=0;			txtp--;	break;			// �I��
		}
		while( vp->pri>=pri ){											// �]���X�^�b�N�̃v���C�I���e�B���J�����g�v���C�I���e�B�ȏ�̊ԃ��[�v
			(vp-1)->val=vp->prcp( (vp-1)->val, vp->val );				// ���Z���s
			vp--;														// �]���X�^�b�N�|�C���^���Z
		}
		vp++;															// �]���X�^�b�N�|�C���^���Z
		vp->prcp=prcp;													// ���Z�����A�h���X
		if( (vp->pri=pri)<0 ){											// �e�[�u���I�[�� �v���C�I���e�B=-1 ��������܂�
			break;
		}
	}
	*dstp=va[0].val;													// �v�Z���ʂ̏�������
	return( (char*)txtp );												// �e�L�X�g�|�C���^�������Đ���I��
}

/************************************************************************/
/*	�R���X�g���N�^														*/
/************************************************************************/

cOPT::cOPT(){
	m_argdbp=dbOpen( 0, DBSIZ_STR, 0, 256 );							// ����DB
	m_optdbp=dbOpen( DBSIZ_STR, sizeof(stOPT), 256, 256 );				// -�I�v�V����DB
	m_plsdbp=dbOpen( DBSIZ_STR, sizeof(stOPT), 256, 256 );				// +�I�v�V����DB
	m_titlep="�e�X�g";													// �R�}���h�^�C�g��
	m_verp="0.00";														// �o�[�W����
	m_ownerp="H.Watanabe";												// �I�[�i�[
	m_linep="";															// ���C�����
	m_extrap=0;															// �ǉ��I�v�V����
	m_exedbp=dbOpen( DBSIZ_STR, sizeof(stLST), 0, 0 );					// ��v����Ύ��s���郏�C���h�J�[�h
	m_igndbp=dbOpen( DBSIZ_STR, sizeof(stLST), 0, 0 );					// ��v����Ζ������郏�C���h�J�[�h
}

/************************************************************************/
/*	�f�X�g���N�^														*/
/************************************************************************/

cOPT::~cOPT(){

	stOPT	*optp;

	dbFOR( m_optdbp, optp, stOPT* ){
		switch( optp->typ ){
			case OPTTYP_DB:
			case OPTTYP_DEFINE:
				dbClose( optp->u.dbp );
				break;
		}
	}
	dbClose( m_optdbp );												// -�I�v�V����DB
	dbClose( m_plsdbp );												// +�I�v�V����DB
	dbClose( m_argdbp );												// ����DB
	dbClose( m_exedbp );												// ��v����Ύ��s���郏�C���h�J�[�h
	dbClose( m_igndbp );												// ��v����Ζ������郏�C���h�J�[�h
}

/************************************************************************/
/*	�^�C�g���ݒ�														*/
/************************************************************************/

void	cOPT::SetTitle( const char *strp ){
	m_titlep=dbStrAlc( m_optdbp, strp );
}

/************************************************************************/
/*	�o�[�W�����ݒ�														*/
/************************************************************************/

void	cOPT::SetVer( const char *strp ){
	m_verp=dbStrAlc( m_optdbp, strp );
}

/************************************************************************/
/*	�I�[�i�[�ݒ�														*/
/************************************************************************/

void	cOPT::SetOwner( const char *strp ){
	m_ownerp=dbStrAlc( m_optdbp, strp );
}

/************************************************************************/
/*	�R�}���h���C���ݒ�													*/
/************************************************************************/

void	cOPT::SetLine( const char *strp ){
	m_linep=dbStrAlc( m_optdbp, strp );
}

/************************************************************************/
/*	�g���I�v�V�����ݒ�													*/
/************************************************************************/

void	cOPT::SetExtra( const char *strp ){
	m_extrap=dbStrAlc( m_optdbp, strp );
}

/************************************************************************/
/*	�I�v�V�����o�^														*/
/************************************************************************/

static stOPT	*Alloc( cOPT *optionp, int typ, const char *idp, void *voidp, int extra, const char *commentp ){

	stDB	*optdbp;
	stOPT	*optp;

	if( *idp=='+' ){
		idp++;
		optdbp=optionp->m_plsdbp;
	}else{
		optdbp=optionp->m_optdbp;
	}
	optp=(stOPT*)dbSch( optdbp, idp );
	if( optp ){
		dbFre( optdbp, optp );
	}
	optp=(stOPT*)dbAlc( optdbp, idp, 0 );
	optp->typ=typ;
	optp->u.voidp=voidp;
	optp->extra=extra;
	optp->commentp=dbStrAlc( optdbp, commentp );
	return( optp );
}

/************************************************************************/
/*	�C���N�������g�I�v�V�����o�^										*/
/************************************************************************/

void	cOPT::AllocInc( const char *idp, int *varp, const char *commentp ){
	Alloc( this, OPTTYP_INC, idp, varp, 0, commentp );
}

/************************************************************************/
/*	�C���g�I�v�V�����o�^												*/
/************************************************************************/

void	cOPT::AllocInt( const char *idp, int *varp, const char *commentp ){
	Alloc( this, OPTTYP_INT, idp, varp, 0, commentp );
}

/************************************************************************/
/*	�Z�b�g�I�v�V�����o�^												*/
/************************************************************************/

void	cOPT::AllocSet( const char *idp, int *varp, int var, const char *commentp ){
	Alloc( this, OPTTYP_SET, idp, varp, var, commentp );
}

/************************************************************************/
/*	���������I�v�V�����o�^												*/
/************************************************************************/

void	cOPT::AllocFPTYP( const char *idp, FPTYP *varp, const char *commentp ){
	Alloc( this, OPTTYP_FPTYP, idp, varp, 0, commentp );
}

/************************************************************************/
/*	������I�v�V�����o�^												*/
/************************************************************************/

void	cOPT::AllocStr( const char *idp, char *varp, int siz, const char *commentp ){
	Alloc( this, OPTTYP_STRING, idp, varp, siz, commentp );
}

/************************************************************************/
/*	DB�I�v�V�����o�^													*/
/************************************************************************/

void	cOPT::AllocDB( const char *idp, stDB **varp, const char *commentp ){
	*varp=dbOpen( 0, DBSIZ_STR, 0, 256 );
	Alloc( this, OPTTYP_DB, idp, *varp, 0, commentp );
}

/************************************************************************/
/*	��`�I�v�V�����o�^													*/
/************************************************************************/

void	cOPT::AllocDef( const char *idp, stDB **varp, const char *commentp ){
	*varp=dbOpen( DBSIZ_STR, DBSIZ_STR, 256, 256 );
	Alloc( this, OPTTYP_DEFINE, idp, *varp, 0, commentp );
}

/************************************************************************/
/*	���[�U�[�����I�v�V�����o�^											*/
/************************************************************************/

void	cOPT::AllocUsr( const char *idp, tpOPTIONUSR *prcp, void *usrp, const char *commentp ){
	Alloc( this, OPTTYP_USR, idp, (void*)prcp, (int)usrp, commentp );
}

/************************************************************************/
/*	�_�u���N�H�[�e�[�V�����ň͂܂ꂽ������𓾂�						*/
/************************************************************************/

static char	*getdqc( char *dstp, int dstsiz, const char *txtp ){

	static const char	*sepa[2]={ "\r\n\t ,", "\"" };					// �^�[�~�l�[�^������z��
	int					dq;												// �_�u���R�[�e�[�V�����t���O
	const char			*nxtp;											// ���̃e�L�X�g�|�C���^
	int					siz;

	dq	=( *txtp=='"' );												// �_�u���R�[�e�[�V�����t���O�Z�b�g
	txtp+=dq;															// �����A�擪���_�u���N�H�[�e�[�V�����Ȃ�΃e�L�X�g�|�C���^���C���N�������g
	nxtp=strpbrk( txtp, sepa[dq] );										// �Z�p���[�^����
	if( !nxtp ){														// �Z�p���[�^��������Ȃ�
		nxtp=txtp+strlen( txtp );										// �l�N�X�g�|�C���^�𕶎���I�[�ɃZ�b�g
		dq	=0;															// �Z�p���[�^��������Ȃ��ꍇ�̓_�u���N�H�[�e�[�V��������
	}
	siz=nxtp-txtp;														// �]���T�C�Y
	if( siz>dstsiz-1 ){													// �o�b�t�@�T�C�Y�|�P���傫��
		siz=dstsiz-1;													// �N�����v
	}
	memcpy( dstp, txtp, siz );											// �f�B�X�g�l�[�V�����o�b�t�@�ɕ�������R�s�[
	dstp[siz]=0;														// �f�B�X�g�l�[�V�����o�b�t�@�O�^�[�~�l�[�g
	return( (char*)( nxtp+dq ) );										// �e�L�X�g�|�C���^�������ă��^�[��
}

/************************************************************************/
/*	-w �I�v�V�����`�F�b�N												*/
/************************************************************************/

static char	*prcoptw( cOPT *optionp, const char *txtp, int childa ){

	stDB		*dbp;													// �Z���N�g�p
	char		tmpa[1024];												// �t�@�C�����p�e���|����
	stLST		*lstp;													// �o�^�p�|�C���^
	char		*namextp;												// ���O�{�g���q�|�C���^
	const char	*nxtp;

	if( !childa ){
		dbp=optionp->m_exedbp;											// ��v����Ύ��s���郏�C���h�J�[�h
	}else{
		dbp=optionp->m_igndbp;											// ��v����Ζ������郏�C���h�J�[�h
	}
	for(;;){
		txtp=SKPSP( txtp );												// �X�L�b�v�X�y�[�X
		txtp=getdqc( tmpa, sizeof(tmpa), txtp );						// �t�@�C�����𓾂�
		if( tmpa[0] ){
			lstp=(stLST*)dbAlc( dbp, tmpa, 0 );							// �o�^
			namextp=pathSeekNam( 0, tmpa );								// �t�@�C�����܂Ńe�L�X�g�|�C���^��i�߂�
			if( namextp>tmpa ){											// �p�X�����݂���
				lstp->schtyp=SCHTYP_PATHNAMEXT;							// �p�X�{���O�{�g���q�w��
			}else{														// �p�X�͑��݂��Ȃ�
				if( *namextp=='.' ){
					lstp->schtyp=SCHTYP_EXT;							// �g���q�w��
				}else{
					lstp->schtyp=SCHTYP_NAMEXT;							// ���O�{�g���q�w��
				}
			}
		}
		nxtp=SKPSP( txtp );												// �X�L�b�v�X�y�[�X
		if( *nxtp!=',' ){
			break;
		}
		txtp=nxtp+1;
	}
	return( (char*)txtp );												// �A��l�̓e�L�X�g�|�C���^
}

static char	*option_w( void *optionp, char *txtp ){
	return( prcoptw( (cOPT*)optionp, txtp, 0 ) );
}

static char	*option_wc( void *optionp, char *txtp ){
	return( prcoptw( (cOPT*)optionp, txtp, 1 ) );
}

/************************************************************************/
/*	-w �����I�v�V�����o�^												*/
/************************************************************************/

void	cOPT::AllocW( const char *idp, const char *idcp ){
	Alloc( this, OPTTYP_USR, idp,  (void*)option_w,  (int)this, "��v����Ύ��s���郏�C���h�J�[�h( default=* )" );
	Alloc( this, OPTTYP_USR, idcp, (void*)option_wc, (int)this, "��v����Ζ������郏�C���h�J�[�h" );
}

/************************************************************************/
/*	�g�p�@���X�g														*/
/************************************************************************/

static stDB	*UsageDB( cOPT *optionp ){

	static const char	*stra[]={ "+", "-" };
	stDB				*usagedbp;
	stOPT				*optp;
	char				factor[256];
	char				tmpa[256];
	stDB				*dbpa[2], *dbp;
	int					i;

	dbpa[0]=optionp->m_plsdbp;
	dbpa[1]=optionp->m_optdbp;
	usagedbp=dbOpen( DBSIZ_STR, DBSIZ_STR, 0, 4096 );
	for( i=0; i<2; i++ ){
		dbp=dbpa[i];
		dbFOR( dbp, optp, stOPT* ){
			factor[0]=0;
			switch( optp->typ ){
				case OPTTYP_INT:
					strcpy( factor, "<���l>" );
					break;
				case OPTTYP_FPTYP:
					strcpy( factor, "<��������>" );
					break;
				case OPTTYP_STRING:
				case OPTTYP_DB:
				case OPTTYP_USR:
					strcpy( factor, "<������>" );
					break;
				case OPTTYP_DEFINE:
					strcpy( factor, "<������>[=<��`>]" );
					break;
			}
			sprintf( tmpa, "%s%s %s", stra[i], (char*)dbKEYADR( optp ), factor );
			dbAlc( usagedbp, tmpa, optp->commentp );
		}
	}
	return( usagedbp );
}

/************************************************************************/
/*	�g�p�@�\��															*/
/************************************************************************/

void	cOPT::Usage( const char *cmdp ){

	stDB	*usagedbp;													// �g�p�@���X�g
	char	*commentp;													// �R�����g
	char	*factorp;													// ����
	int		factorlen=0;												// ���������񒷍ő�
	int		len;														// ����������

	usagedbp=UsageDB( this );											// �g�p�@���X�g
	fprintf( STDERR, "\n" );
	fprintf( STDERR, "%s Ver%s by %s\n", m_titlep, m_verp, m_ownerp );
	fprintf( STDERR, "\n" );
	if( dbAlcN( usagedbp ) ){
		fprintf( STDERR, "�g�p�@:%s [<�I�v�V����>] %s\n", cmdp, m_linep );
		fprintf( STDERR, "\n" );
		dbFOR( usagedbp, commentp, char* ){
			factorp=(char*)dbKEYADR( commentp );
			len=strlen( factorp );
			if( factorlen<len ){
				factorlen=len;
			}
		}
		dbFOR( usagedbp, commentp, char* ){
			factorp=(char*)dbKEYADR( commentp );
			fprintf( STDERR, "    %-*s: %s\n", factorlen+1, factorp, commentp );
		}
	}else{
		fprintf( STDERR, "�g�p�@:%s %s\n", cmdp, m_linep );
	}
	if( m_extrap ){
		fprintf( STDERR, "%s", m_extrap );
	}
#	ifndef	_WIN32
	fprintf( STDERR, "\n" );
#	endif
	dbClose( usagedbp );
}

/************************************************************************/
/*	���C���h�J�[�h�`�F�b�N												*/
/************************************************************************/

static int	wildchk( const char *txtp, const char *wildp ){

	int		card, chr;													// char �ŗǂ��̂��� �������E�������ߖ�̂��� int �Ƃ���

	for(;;){
		card=*wildp++;
		chr=*txtp++;
		if( !card && !chr ) return( 1 );								// ��v
		if( card=='*' ){
			txtp--;
			for(;;){
				if( wildchk( txtp, wildp ) ) return( 1 );				// ��v
				if( !*txtp ) return( 0 );								// �s��v
				txtp++;
			}
		}
#	ifdef	_WIN32
		if( card>='A' && card<='Z' ){
			card|='a'-'A';
		}
		if( chr>='A' && chr<='Z' ){
			chr|='a'-'A';
		}
#	endif
		if( card!='?' && card!=chr ) break;								// �s��v
	}
	return( 0 );														// �s��v
}

/************************************************************************/
/*	���C���h�J�[�h�`�F�b�N												*/
/************************************************************************/

static int	wildchkdb( const stDB *wilddbp, const char *fnamp ){

	stLST		*lstp;
	const char	*schpa[SCHTYP_N];

	schpa[SCHTYP_PATHNAMEXT]=fnamp;										// �p�X�{���O�{�g���q�w��
	schpa[SCHTYP_NAMEXT]=pathSeekNam( 0, fnamp );						// ���O�{�g���q�w��
	schpa[SCHTYP_EXT]=pathSeekExt( 0, 0, fnamp );						// �g���q�̂ݎw��
	dbFOR( wilddbp, lstp, stLST* ){
		if( wildchk( schpa[lstp->schtyp], (char*)dbKEYADR( lstp ) ) ){	// ��v����
			return( 1 );
		}
	}
	return( 0 );														// ��v���Ȃ�
}

/************************************************************************/
/*	��������															*/
/************************************************************************/

int		cOPT::optwTst( const char *fnamp, int dirflg ){

	if( wildchkdb( m_igndbp, fnamp ) ){									// ��v����Ζ������郏�C���h�J�[�h
		return( 0 );													// �����s����
	}
	if( dirflg ){														// �f�B���N�g���̏ꍇ
		return( 1 );
	}
	if( !dbAlcN( m_exedbp ) || wildchkdb( m_exedbp, fnamp ) ){			// ��v����Ύ��s���郏�C���h�J�[�h
		return( 1 );													// ��������
	}
	return( 0 );														// �����s����
}

/************************************************************************/
/*	qsort �̂��߂̃R�[���o�b�N�֐�										*/
/************************************************************************/

static int	qsortCalBak( const void *dstv, const void *souv ){

	const stOPT		*dstoptp=*(const stOPT**)dstv;
	const stOPT		*souoptp=*(const stOPT**)souv;
	char			*dstnamp;
	char			*sounamp;
	int				sou, dst;

	dstnamp=(char*)dbKEYADR( dstoptp );
	sounamp=(char*)dbKEYADR( souoptp );
	dst=strlen( dstnamp );
	sou=strlen( sounamp );
	return( sou-dst );													// ������
}

/************************************************************************/
/*	�`�F�b�N�I�v�V����													*/
/************************************************************************/

stDB	*cOPT::PrcLin( char *txtp ){

	stOPT		**optpp, **plspp, **pp, *optp;							// �I�v�V�����|�C���^
	char		*idp=0;													// �I�v�V����������|�C���^
	char		*errtxtp;												// �G���[�e�L�X�g�|�C���^
	char		*nxtp;													// �e�L�X�g�|�C���^���[�N
	char		tmpa[256];												// �e���|�����o�b�t�@
	char		def[256];												// ��`�o�b�t�@
	int			i;														// ���[�v�J�E���^

	optpp=(stOPT**)dbAryOpen( m_optdbp, 0 );							// -�I�v�V����
	plspp=(stOPT**)dbAryOpen( m_plsdbp, 0 );							// +�I�v�V����
	qsort( optpp, dbAlcN( m_optdbp ), sizeof(stOPT*), qsortCalBak );	// ������̒������ɕ��ׂ�
	qsort( plspp, dbAlcN( m_plsdbp ), sizeof(stOPT*), qsortCalBak );	// ������̒������ɕ��ׂ�
	for(;;){															// �I�v�V������̓��[�v
		txtp=errtxtp=SKPSP( txtp );										// �X�y�[�X�A�^�u�̃X�L�b�v
		if( !*txtp ){													// �I�[����
			break;
		}
		pp=0;
		if( *txtp=='-' ){
			pp=optpp;
		}else if( *txtp=='+' ){
			pp=plspp;
		}
		if( pp ){														// '-'�܂���'+' 
			txtp++;														// '-'�܂���'+' �X�L�b�v
			for(;;){													// �A���I�v�V����������̓��[�v
				for( i=0; ( optp=pp[i] )!=0; i++ ){
					idp=(char*)dbKEYADR( optp );
					if( !memcmp( txtp, idp, strlen( idp ) ) ){
						break;
					}
				}
				if( !optp ){											// ������Ȃ�����
					nxtp=SKPSP( txtp );									// �X�y�[�X�A�^�u�̃X�L�b�v
					if( *nxtp ){
						fprintf( STDERR, "�I�v�V�����G���[:%s\n", errtxtp );
					}
					return( 0 );
				}
				txtp+=strlen( idp );									// �I�v�V����������T�C�Y���V�[�N
				nxtp=SKPSP( txtp );										// �X�y�[�X�A�^�u�̃X�L�b�v
				switch( optp->typ ){
					case OPTTYP_INC:									// �C���N�������g
						(*optp->u.intp)++;
						break;
					case OPTTYP_SET:									// �l�Z�b�g
						*optp->u.intp=optp->extra;
						break;
					case OPTTYP_INT:									// ���l
						txtp=chkoptVal( optp->u.intp, nxtp );			// �����]��
						if( txtp<=nxtp ){
							fprintf( STDERR, "�����V���^�b�N�X�G���[:%s\n", errtxtp );
							return( 0 );
						}
						break;
					case OPTTYP_FPTYP:									// ��������
						*optp->u.fptypp=(FPTYP)strtod( nxtp, (char**)&txtp );
						if( txtp<=nxtp ){
							fprintf( STDERR, "���������V���^�b�N�X�G���[:%s\n", errtxtp );
							return( 0 );
						}
						break;
					case OPTTYP_STRING:									// ������
						if( *nxtp && *nxtp!='-' ){						// �I�v�V�����w�蕶���ł͂Ȃ�
							txtp=dbGetDQ( optp->u.charp, optp->extra, nxtp );	// �t�@�C�����𓾂�
						}else{
							strcpy( optp->u.charp, "\x1" );
						}
						break;
					case OPTTYP_DB:										// DB
						if( *nxtp!='-' ){								// �I�v�V�����w�蕶���ł͂Ȃ�
							txtp=dbGetDQ( tmpa, sizeof(tmpa), nxtp );	// ������𓾂�
						}else{
							tmpa[0]=0;
						}
						dbAlc( optp->u.dbp, tmpa, 0 );					// �o�^
						break;
					case OPTTYP_DEFINE:									// ��`
						txtp=dbGetLbl( tmpa, sizeof(tmpa), nxtp );		// �������x���𓾂�
						if( !tmpa[0] ){									// ���x�������݂���
							fprintf( STDERR, "���x��������܂���:%s\n", txtp );
							return( 0 );								// �G���[�I��
						}
						nxtp=SKPSP( txtp );
						if( *nxtp=='=' ){
							txtp=dbGetDQ( def, sizeof(def), SKPSP( nxtp+1 ) );	// �R�}���h���C���̕�����𓾂�
						}else{
							strcpy( def, "1" );
						}
						dbAlc( optp->u.dbp, tmpa, def );				// �o�^
						break;
					case OPTTYP_USR:									// ���[�U�[����
						txtp=optp->u.prcp( (void*)optp->extra, nxtp );	// �R�[���o�b�N
						if( !txtp ){
							return( 0 );								// �G���[�I��
						}
						break;
				}
				if( *txtp<=' ' ){										// �O�A�X�y�[�X�A�^�u�̏ꍇ
					break;												// �I�v�V�����̘A�������𔲂���
				}
			}
		}else{
			txtp=dbGetDQ( tmpa, sizeof(tmpa), txtp );
			dbAlc( m_argdbp, tmpa );
		}
	}
	dbAryClose( optpp );
	dbAryClose( plspp );
	return( m_argdbp );
}

/************************************************************************/
/*	arg ����R�}���h���C���֕ϊ�										*/
/************************************************************************/

static char	*arg2lin( int argc, char *argv[] ){

	char			*linalc=0;											// �������m�ۃ|�C���^
	char			*argp;												// �A�[�M�������g�|�C���^
	int				linsiz=0;											// -W4 ���[�j���O�΍�
	int				i;
	int				envsiz;
	char			*envp;
	char			tmpa[256];
	unsigned int	chr;
	int				pass;												// �p�X
	int				dq;													// �X�y�[�X���܂�ł���"�Ŏn�܂��Ă��Ȃ�

	for( pass=0; pass<2; pass++ ){										// �Q�񃋁[�v����
		linsiz=0;														// ���������Z�b�g
		for( i=1; i<argc; i++ ){										// �����������[�v
			argp=argv[i];												// �����|�C���^�𓾂�
			dq=( strpbrk( argp, "\t " )!=0 );							// �X�y�[�X���̓^�u���܂�ł���
			if( dq ){
				if( pass ){												// PASS1
					linalc[linsiz]='"';									// �P�����X�g�A
				}
				linsiz++;												// ���������Z
			}
			while( ( chr=*argp++ )!=0 ){								// �����|�C���^����P�����𓾂�
				if( chr=='$' ){											// ���ϐ��w�莯�ʎq
					argp=dbGetLbl( tmpa+1, sizeof(tmpa)-1, argp );		// �������x���𓾂�
					envp=getenv( tmpa+1 );								// ���ϐ��̃|�C���^�𓾂�
					if( !envp ){										// ������Ȃ�����
						tmpa[0]='$';
						envp=tmpa;										// �u��������̓I���W�i��������
					}
					envsiz=strlen( envp );								// �u��������T�C�Y
					if( pass ){											// PASS1
						memcpy( linalc+linsiz, envp, envsiz );			// �u��������R�s�[
					}
					linsiz+=envsiz;										// ���������Z
				}else{
					if( pass ){											// PASS1
						linalc[linsiz]=(char )chr;						// �P�����X�g�A
					}
					linsiz++;											// ���������Z
				}
			}
			if( dq ){
				if( pass ){												// PASS1
					linalc[linsiz]='"';									// �P�����X�g�A
				}
				linsiz++;												// ���������Z
			}
			if( pass ){													// PASS1
				linalc[linsiz]=' ';										// �X�y�[�X�X�g�A
			}
			linsiz++;													// ���������Z
		}
		if( !pass ){													// PASS0
			linalc=(char*)malloc( linsiz+1 );							// �O�^�[�~�l�[�g���邽�߁{�P����
		}
	}
	linalc[linsiz]=0;													// �O�^�[�~�l�[�g
	return( linalc );													// �������m�ۂ����|�C���^�������ă��^�[������
}

/************************************************************************/
/*	�`�F�b�N�I�v�V����													*/
/************************************************************************/

stDB	*cOPT::PrcArg( int argc, char *argv[] ){

	stDB		*argdbp;
	char		*txtalc;												// �e�L�X�g�|�C���^

	txtalc=arg2lin( argc, argv );
	argdbp=PrcLin( txtalc );
	free( txtalc );
	return( argdbp );
}

