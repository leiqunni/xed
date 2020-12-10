/************************************************************************/
/*																		*/
/*	�\��						by H.Watanabe							*/
/*	Ver1.00	2009/04/04 13:49:05	�쐬�J�n								*/
/*																		*/
/************************************************************************/

#	include	<assert.h>													/* �A�T�[�V����*/
#	include	<stdio.h>													/* �W�����o�͊֐�*/
#	include	<string.h>													/* �W��������֐�*/
#	include	"std/txtdl.h"												/* �e�L�X�g�G�f�B�^�̂��߂̃f�B�X�v���C���X�g�ϊ�*/
#	include	"std/clp3.h"												/* �N���b�s���O*/
#	include	"main.h"													/* ���C���C���N���[�h�t�@�C��*/

/************************************************************************/
/*	���[�J����`														*/
/************************************************************************/

#	define	MOD_XXC			0											/* ���[�h*/
#	define	MOD_YYC			0
#	define	MOD_XXD			25
#	define	MOD_YYD			1

#	define	MSG_XXC			25											/* ���b�Z�[�W*/
#	define	MSG_YYC			0
#	define	MSG_XXD			23
#	define	MSG_YYD			1

#	define	FIL_XXC			48											/* �t�@�C����*/
#	define	FIL_YYC			0
#	define	FIL_XXD			23
#	define	FIL_YYD			1

	static stDB				*g_xcoldbp;									// XColor �ϊ�DB

/************************************************************************/
/*	�f�B�X�v���C�X�^�[�g�A�b�v											*/
/************************************************************************/

void	displayStartUp(void){
	g_xcoldbp=dbOpen( sizeof(unsigned long), sizeof(stXCOL), 256, 4096 );		// XColor �ϊ�DB�I�[�v��
}

/************************************************************************/
/*	�f�B�X�v���C�N���[���i�b�v											*/
/************************************************************************/

void	displayCleanUp(void){
	dbClose( g_xcoldbp );												// XColor �ϊ�DB�N���[�Y
}

/************************************************************************/
/*	�q�f�a����w�J���[�ɕϊ�����										*/
/************************************************************************/

#	ifndef	_WIN32
#		ifndef	CURSES

stXCOL	*colorref2xcolor( unsigned long colorref ){

	stXCOL	*xcolp;														// XColor �\����
	char	tmpa[256];													// �e���|�����o�b�t�@

	xcolp=(stXCOL*)dbSch( g_xcoldbp, &colorref );						// ����
	if( !xcolp ){														// ���o�^
		xcolp=(stXCOL*)dbAlc( g_xcoldbp, &colorref, 0 );				// �o�^
		sprintf( tmpa, "#%06x", (int)colorref );						// ������
		XParseColor( g_dpyp, g_cmap, tmpa, &xcolp->col );				// �J���[���
		XAllocColor( g_dpyp, g_cmap, &xcolp->col );						// �J���[�o�^
	}
	return( xcolp );
}

#		endif
#	endif

/************************************************************************/
/*	���C���`��															*/
/************************************************************************/

static void	lineOnTxt( int x1, int y1, int x2, int y2, unsigned long colorref ){

#	ifdef	_WIN32

	HPEN	hpen;														// �y���n���h��

	if( clpLin( &x1, &y1, &x2, &y2, x1, y1, x2, y2, 0, 0, g_txxd*g_fxw, g_tyyd*g_fyw ) ){
		hpen=CreatePen( PS_SOLID, 0, colorref );						// �y���쐬
		SelectObject(	g_hdc, hpen );									// �y���Z���N�g
		MoveToEx(		g_hdc, x1+g_txxc*g_fxw, y1+g_tyyc*g_fyw, 0 );	// �n�_�w��
		LineTo(			g_hdc, x2+g_txxc*g_fxw, y2+g_tyyc*g_fyw );		// ���C���`��
		DeleteObject(	hpen );											// �y���j��
	}

#	else
#		ifndef	CURSES

	stXCOL	*xcolp;

	if( clpLin( &x1, &y1, &x2, &y2, x1, y1, x2, y2, 0, 0, g_txxd*g_fxw, g_tyyd*g_fyw ) ){
		xcolp=colorref2xcolor( colorref );								// �q�f�a����w�J���[�ɕϊ�����
		XSetForeground(	g_dpyp, g_gc, xcolp->col.pixel );				// �O�i�F�Z�b�g
		XDrawLine( g_dpyp, g_win, g_gc, x1+g_txxc*g_fxw, y1+g_tyyc*g_fyw, x2+g_txxc*g_fxw, y2+g_tyyc*g_fyw );
	}
#		else
	(void)&x1, (void)&y1, (void)&x2, (void)&y2, (void)&colorref;
#		endif
#	endif
}

/************************************************************************/
/*	�{�b�N�X�`��														*/
/************************************************************************/

static void	boxOnTxt( int x1, int y1, int x2, int y2, unsigned long colorref ){
	lineOnTxt( x1, y1, x2, y1, colorref );
	lineOnTxt( x2, y1, x2, y2, colorref );
	lineOnTxt( x2, y2, x1, y2, colorref );
	lineOnTxt( x1, y2, x1, y1, colorref );
}

/************************************************************************/
/*	��`�I��\��														*/
/************************************************************************/

static void	displaySelect(void){

	int		xx1, yy1, xx2, yy2;											// �L�����N�^���W

	if( CUTCURP && CUTTYP ){											// ��`�I�����[�h
		xx1=CUTCURXX-PGXX;												// ��
		yy1=CUTPGYY+CUTCURYY-PGYY;										// ��
		xx2=CURXX-PGXX;													// �E
		yy2=CURYY;														// ��
		if( xx1<=xx2 && yy1<=yy2 ){
			boxOnTxt( xx1*g_fxw, yy1*g_fyw, xx2*g_fxw, (yy2+1)*g_fyw, CR_LB );
		}
	}
}

/************************************************************************/
/*	�e�L�X�g�`��														*/
/************************************************************************/

void	drwTxt( int xx, int yy, const char *txtp, int txtsiz, unsigned long bakcol, unsigned long forcol ){

#	ifdef	_WIN32

	int		x, y;														// �s�N�Z�����W�n
	RECT	rect={ 0 };

	x=xx*g_fxw;															// �s�N�Z�����W�n�w
	y=yy*g_fyw;															// �s�N�Z�����W�n�x
	rect.left=x;														// ��
	rect.right=x+txtsiz*g_fxw;											// �E
	rect.top=y;															// ��
	rect.bottom=y+g_fyw;												// ��

	SetBkColor( g_hdc, bakcol );										// �o�b�N�O���E���h�J���[
	SetTextColor( g_hdc, forcol );										// �t�H�A�O���E���h�J���[
	TextOut( g_hdc, x, y, txtp, txtsiz );								// ������`��

#	else
#		ifndef	CURSES

	stXCOL	*xcolp;														// �J���[�\���̃|�C���^
	int		x, y;														// �s�N�Z�����W�n

	x=xx*g_fxw;															// �s�N�Z�����W�n�w
	y=yy*g_fyw+g_fntascent;												// �s�N�Z�����W�n�x
	xcolp=colorref2xcolor( bakcol );									// �q�f�a����w�J���[�ɕϊ�����
	XSetBackground( g_dpyp, g_gc, xcolp->col.pixel );					// �w�i�F�Z�b�g
	xcolp=colorref2xcolor( forcol );									// �q�f�a����w�J���[�ɕϊ�����
	XSetForeground(	g_dpyp, g_gc, xcolp->col.pixel );					// �O�i�F�Z�b�g
	if( g_jp ){															// ���{�ꃂ�[�h
		XmbDrawImageString( g_dpyp, g_win, g_fs, g_gc, x, y, txtp, txtsiz );
	}else{																// �p�ꃂ�[�h
		XDrawImageString( g_dpyp, g_win, g_gc, x, y, txtp, txtsiz );
	}
#		else

	int			bakval, forval, rvs;
	char		tmpa[4096];												// �e���|�����o�b�t�@

	memcpy( tmpa, txtp, txtsiz );
	tmpa[txtsiz]=0;														// 0�^�[�~�l�[�g
	bakval=(bakcol&0xff);												// �o�b�N�O���E���h�J���[
	forval=(forcol&0xff);												// �t�H�A�O���E���h�J���[
	rvs=( forval<bakval );												// ���]����
	if( rvs ){															// ���]
		attrset( A_REVERSE );											// ���]
	}else{
		attrset( A_NORMAL );											// �m�[�}��
	}
	move( yy, xx );														// �J�[�\�����ړ�����
	addstr( tmpa );														// �\��

#		endif
#	endif

}

/************************************************************************/
/*	�t�B��																*/
/************************************************************************/

void	drwFil( int xx1, int yy1, int xx2, int yy2, unsigned long col ){

#	ifdef	_WIN32

	int		x1, y1, x2, y2;												// �s�N�Z�����W�n
	RECT	rect;														// ��`�\����
	HBRUSH	hbrush;														// �u���V

	x1=xx1*g_fxw;														// ��
	y1=yy1*g_fyw;														// ��
	x2=xx2*g_fxw;														// �E
	y2=yy2*g_fyw;														// ��
	SetRect( &rect, x1, y1, x2, y2 );									// ��`�\���̃Z�b�g
	hbrush=CreateSolidBrush( col );										// �u���V�쐬
	FillRect( g_hdc, &rect, hbrush );									// ��`�t�B��
	DeleteObject( hbrush );												// �u���V�j��

#	else
#		ifndef	CURSES

	int		x1, y1, x2, y2;												// �s�N�Z�����W�n
	stXCOL	*xcolp;														// �J���[�\���̃|�C���^

	x1=xx1*g_fxw;														// ��
	y1=yy1*g_fyw;														// ��
	x2=xx2*g_fxw;														// �E
	y2=yy2*g_fyw;														// ��
	xcolp=colorref2xcolor( col );										// �q�f�a����w�J���[�ɕϊ�����
	XSetForeground(	g_dpyp, g_gc, xcolp->col.pixel );					// �O�i�F�Z�b�g
	XFillRectangle( g_dpyp, g_win, g_gc, x1, y1, x2-x1, y2-y1 );		// ��`�t�B��

#		else

	static const char	spa[]="                                                                                                                                             ";
	int					yy;												// Y�L�����N�^���W
	int					rvs;											// ���]�t���O

	rvs=(col&0xff)>=0x80;												// ���]����
	for( yy=yy1; yy<yy2; yy++ ){
		drwTxt( xx1, yy, spa, xx2-xx1, rvs, !rvs );						// �e�L�X�g�`��
	}

#		endif
#	endif

}

/************************************************************************/
/*	�C���t�H���[�V�����\��												*/
/************************************************************************/

static void	displayInf(void){

	static const char	*overinsa[2]={
		"OVER",
		"INS"
	};
	static const char	*largesmalla[2]={
		"SMALL",
		"LARGE"
	};
	char			tmpa[4096];
	int				xxw;

//	���[�h

	sprintf( tmpa, "[%5d:%5d] %-4s %-5s ", PGYY+CURYY+1, CURXX+1, overinsa[g_flgins!=0], largesmalla[g_flgl2c!=0] );
	drwTxt( MOD_XXC, MOD_YYC, tmpa, strlen( tmpa ), CR_MOD_BK, CR_BLACK );		// �e�L�X�g�`��

//	���b�Z�[�W

	sprintf( tmpa, "%-*s", MSG_XXD, g_msgstr );
	drwTxt( MSG_XXC, MSG_YYC, tmpa, strlen( tmpa ), CR_MSG_BK, CR_BLACK );		// �e�L�X�g�`��

//	�t�@�C��

	xxw=g_xxw-FIL_XXC-2;
	if( xxw>0 ){
		sprintf( tmpa, "%c %-*s", FILCHG?'*':' ', xxw, FNAMP );
		drwTxt( FIL_XXC, FIL_YYC, tmpa, strlen( tmpa ), CR_MOD_BK, CR_BLACK );	// �e�L�X�g�`��
	}
}

/************************************************************************/
/*	�s�ԍ��\��															*/
/************************************************************************/

#	define	LNOXX		0
#	define	LNOYY		1

static void	displayLno( int x1, int y1, int x2, int y2 ){

	static const unsigned long	cola[2]={
		CR_LNO_BK,														// �w�i�F
		CR_LNO,															// �O�i�F
	};
	int				yy1, yy2;											// �L�����N�^���W
	stLIN			*linp;												// �s�|�C���^
	int				seekn;												// �V�[�N�ł�����
	int				yy;													// �L�����N�^���W
	char			tmpa[256];											// �e���|�����o�b�t�@

	(void)&x1, (void)&x2;
	if( g_lnosiz ){														// �s�ԍ��\���L��
		yy1=y1/g_fyw;													// ��
		yy2=(y2+g_fyw-1)/g_fyw;											// ��
		linp=(stLIN*)dbSeek( &seekn, PGP, yy1 );						// �w�肵���I�t�Z�b�g���V�[�N����
		if( seekn!=yy1 ){												// �V�[�N�ł��Ȃ�����
			linp=0;														// �s�|�C���^�͖���
		}
		for( yy=yy1; yy<yy2; yy++ ){
			if( linp ){													// �s�����݂���
				sprintf( tmpa, "%*d ", g_lnosiz-1, PGYY+yy+1 );
				drwTxt( LNOXX, (LNOYY+yy), tmpa, g_lnosiz, cola[linp->chgflg], cola[!linp->chgflg] );			// �e�L�X�g�`��
				linp=(stLIN*)dbNxt( linp );								// �s�|�C���^�V�[�N
			}else{														// �s�͑��݂��Ȃ�
				drwFil( LNOXX, LNOYY+yy, LNOXX+g_lnosiz, LNOYY+yy+1, cola[0] );
			}
		}
	}
}

/************************************************************************/
/*	�����m�[�}���C�Y													*/
/************************************************************************/

#	ifdef	CURSES

static int	kannrm( const char *txtadr, int siz ){

	const char	*txtp=txtadr;
	int			cursiz;
	int			i=0;

	for(;;){
		if( txtp[i]&0x80 ){
			cursiz=2;
		}else{
			cursiz=1;
		}
		if( i+cursiz>siz ){
			break;
		}
		i+=cursiz;
	}
	return( i );
}

#	endif

/************************************************************************/
/*	�e�L�X�g�\��														*/
/************************************************************************/

#	define	TXTXX		g_lnosiz
#	define	TXTYY		1

static void	displayTxt( int x1, int y1, int x2, int y2 ){

	static const unsigned long	cola[2]={
		CR_WHITE,														// �z���C�g
		CR_LB,															// ���C�g�u���[
	};
	int				xx1, yy1, xx2, yy2;									// �L�����N�^���W
	stLIN			*linp;												// �s�|�C���^
	stDB			*dldbp;												// �f�B�X�v���C���X�gDB
	stTXTDL			*dlp;												// �f�B�X�v���C���X�g�|�C���^
	int				seekn;												// �V�[�N�ł�����
	unsigned long	bakforcola[3]={ CR_BLACK, CR_WHITE };				// �w�i�F�A�O�ʐF
	int				curxx;												// �J�[�\���w���W
	int				xx, yy;												// �L�����N�^���W
	stLM			lm;													// �s�}�b�v
	int				eof;												// �I�[�s�t���O
	int				eofdsp;												// �I�[�\���t���O
	int				cutrvs;												// �s�P�ʃJ�b�g���[�h���]
	int				rvs;												// ���]�t���O
	int				lno;												// �s�ԍ�
	int				curyy=-1;											// �J�[�\�����C���\���̂��߂̃��[�N
	char			*keyadr;											// �`�敶����A�h���X
	int				keysiz;												// �`�敶����T�C�Y

	xx1=x1/g_fxw;														// ��
	yy1=y1/g_fyw;														// ��
	xx2=(x2+g_fxw-1)/g_fxw;												// �E
	yy2=(y2+g_fyw-1)/g_fyw;												// ��
	linp=(stLIN*)dbSeek( &seekn, PGP, yy1 );							// �w�肵���I�t�Z�b�g���V�[�N����
	if( seekn!=yy1 ){													// �V�[�N�ł��Ȃ�����
		linp=0;															// �s�|�C���^�𖳌��ɂ���
	}
	for( yy=yy1; yy<yy2; yy++ ){
		curxx=-1;														// �J�[�\���s�ł͂Ȃ�
		xx=0;															// �w���W�N���A
		if( linp ){														// �s�����݂���
			if( yy==CURYY && g_cursw ){									// �J�[�\���s
				lmMak( &lm, (char*)dbKEYADR( linp ), g_tabstp, 0 );		// �s�}�b�v�쐬
				curxx=lmPosAdj( &lm, CURXX, 0 );						// �J�[�\���ʒu���K��
			}
			if( dbNxt( linp ) ){										// �I�[�s�ł͂Ȃ�
				eof=0;													// [EOF]�\��
				eofdsp=g_flgcr;											// �I�[�\���t���O�͉��s�\���t���O
			}else{														// �I�[�s
				eof=1;													// [.]�\��
				eofdsp=g_flgeof;										// �I�[�\���t���O��[EOF]�\���t���O
			}
			cutrvs=0;													// �s�P�ʃJ�b�g���[�h���]�t���O�N���A
			if( CUTCURP && !CUTTYP ){									// �s�I�����[�h
				lno=PGYY+yy;											// �������̍s�ԍ�
				if( lno>=CUTPGYY+CUTCURYY && lno<PGYY+CURYY ){			// �͈͎w�蒆�Ɋ܂܂��
					cutrvs=1;											// �s�P�ʃJ�b�g���[�h���]�t���O�Z�b�g
				}
			}
			dldbp=txtdl( (char*)dbKEYADR( linp ), curxx, g_tabstp, g_tabdsp, eof, eofdsp, PGXX, g_scdbiz, xx2-xx1 );
			dbFOR( dldbp, dlp, stTXTDL* ){								// �f�B�X�v���C���X�g������
				if( clp1( 0, 0, xx-PGXX, xx-PGXX+dlp->dspsiz, xx1, xx2 ) ){		// �\���G���A�ɐG��Ă���
#	ifdef	CURSES
					rvs=cutrvs;											// �J�[�\����CURSES���o��
#	else
					rvs=dlp->rvs^cutrvs;								// ���]�t���O�v�Z
#	endif
					bakforcola[1]=cola[dlp->col];						// �e�L�X�g�J���[�Z�b�g
					if( dlp->cur && !g_flgins ){						// �㏑�����[�h
						bakforcola[1]=CR_OVER;							// �e�L�X�g�J���[�Z�b�g
					}
					keyadr=(char*)dbKEYADR( dlp );						// �L�[�A�h���X
					keysiz=dbKEYSIZ( dlp );								// �L�[�T�C�Y
#	ifdef	CURSES
					if( keysiz>xx2-xx+PGXX-1 ){							// CURSES �����s���Ȃ��悤�ɁA�ʏ���1���������Ȃ��N�����v����
						keysiz=xx2-xx+PGXX-1;
					}
					keysiz=kannrm( keyadr, keysiz );					// �����m�[�}���C�Y
#	endif
					if( keysiz ){										// �L�[�o�b�t�@�͗L��
						drwTxt( (TXTXX+xx-PGXX), (TXTYY+yy), keyadr, keysiz, bakforcola[rvs], bakforcola[!rvs] );				// �e�L�X�g�`��
					}else{												// �L�[�o�b�t�@�͖���
						drwFil( (TXTXX+xx-PGXX), (TXTYY+yy), (TXTXX+xx-PGXX)+dlp->dspsiz, (TXTYY+yy)+1, bakforcola[rvs] );
					}
					if( xx==PGXX-1 ){									// �����ɂ͂ݏo�Ă���
						displayLno( 0, (yy)*g_fyw, g_lnosiz*g_fxw, (yy+1)*g_fyw );				// �s�ԍ��ĕ`��
					}
				}
				xx+=dlp->dspsiz;										// ���W�X�V
			}
			dbClose( dldbp );											// �f�B�X�v���C���X�gDB�N���[�Y
			linp=(stLIN*)dbNxt( linp );									// �s�|�C���^�V�[�N
		}
		if( xx<PGXX ){
			xx=PGXX;
		}
		if( xx-PGXX<xx2 ){												// �󂫃G���A�����݂���
			drwFil( TXTXX+xx-PGXX, TXTYY+yy, TXTXX+xx2, TXTYY+yy+1, CR_BLACK );
		}
		if( curxx>=0 ){													// �J�[�\���s
			curyy=yy;													// �J�[�\�����C���\���̂��߂̃��[�N�Z�b�g
		}
	}
	if( curyy>=0 ){														// �J�[�\���s�����݂���
		lineOnTxt( 0, curyy*g_fyw+g_fyw, g_txxd*g_fxw, curyy*g_fyw+g_fyw, CR_LB );				// �J�[�\���s���C���\��
	}
	displaySelect();													// ��`�I��\��
}

/************************************************************************/
/*	�L�����N�^�{�b�N�X�\��												*/
/************************************************************************/

void	displayChrBox( int xxc, int yyc, int xxd, int yyd, const char *msgp ){

	int		yy;															// �x���W
	char	tmpa[256];													// �e���|�����o�b�t�@

	for( yy=0; yy<yyd; yy++ ){
		if( yy==0 || yy==yyd-1 ){										// �擪���͏I�[
			sprintf( tmpa, " %-*s", xxd-1, msgp );
			drwTxt( xxc, yyc+yy, tmpa, strlen( tmpa ), CR_LB, CR_BLACK );		// �e�L�X�g�`��
			msgp="";
		}else{
			drwTxt( xxc,       yyc+yy, tmpa, 1, CR_LB, CR_BLACK );		// �e�L�X�g�`��
			drwTxt( xxc+xxd-1, yyc+yy, tmpa, 1, CR_LB, CR_BLACK );		// �e�L�X�g�`��
		}
	}
}

/************************************************************************/
/*	�\��																*/
/************************************************************************/

void	display( int x1, int y1, int x2, int y2 ){

	int				cxc, cyc, cxd, cyd;									// �N���b�v���ꂽ���W

	if( clpBox( &cxc, &cyc, &cxd, &cyd, x1, y1, x1+x2, y1+y2, 0, 0, g_xw, g_fyw ) ){
		displayInf();													// �C���t�H���[�V�����\��
	}
	if( clpBox( &cxc, &cyc, &cxd, &cyd, x1, y1, x2-x1, y2-y1, 0, g_tyyc*g_fyw, g_lnosiz*g_fxw, g_tyyd*g_fyw ) ){
		displayLno( cxc, cyc-g_tyyc*g_fyw, cxc+cxd, cyc+cyd );			// �s�ԍ��\��
	}
	if( clpBox( &cxc, &cyc, &cxd, &cyd, x1, y1, x2-x1, y2-y1, g_txxc*g_fxw, g_tyyc*g_fyw, g_txxd*g_fxw, g_tyyd*g_fyw ) ){
		displayTxt( cxc-g_txxc*g_fxw, cyc-g_tyyc*g_fyw, cxc+cxd-g_txxc*g_fxw, cyc+cyd-g_tyyc*g_fyw );			// �e�L�X�g�\��
	}
	helpReDsp();														// �w���v�ĕ\��
	linedtReDsp();														// ��s�ҏW�ĕ\��
#	ifdef	CURSES
	{
		stLM			lm;												// �s�}�b�v
		int				curxx;											// �J�[�\���w���W
		int				xx;

		lmMak( &lm, (char*)dbKEYADR( CURP), g_tabstp, 0 );				// �s�}�b�v�쐬
		curxx=lmPosAdj( &lm, CURXX, 0 );								// �J�[�\���ʒu���K��
		xx=curxx-PGXX;
		if( xx<0 ){
			xx=0;
		}
		move( TXTYY+CURYY, TXTXX+xx );									// �J�[�\���ړ�
		refresh();
	}
#	endif
}

/************************************************************************/
/*	�ĕ`�惊�N�G�X�g( �O���[�o�� )										*/
/************************************************************************/

void	invalidate( int xxc, int yyc, int xxd, int yyd ){

#	ifdef	_WIN32
	g_hdc=GetDC( g_hwnd );
	if( g_hdc ){
		SelectObject( g_hdc, g_hfont );
		display( xxc*g_fxw, yyc*g_fyw, (xxc+xxd)*g_fxw, (yyc+yyd)*g_fyw );
		ReleaseDC( g_hwnd, g_hdc );
		g_hdc=0;
	}
#	else
	display( xxc*g_fxw, yyc*g_fyw, (xxc+xxd)*g_fxw, (yyc+yyd)*g_fyw );
#	endif
}

/************************************************************************/
/*	�ĕ`�惊�N�G�X�g( �e�L�X�g )										*/
/************************************************************************/

void	invalidateTxt( int xxc, int yyc, int xxd, int yyd ){
	invalidate( g_txxc+xxc, g_tyyc+yyc, xxd, yyd );						// �ĕ`�惊�N�G�X�g
}

/************************************************************************/
/*	�ĕ`�惊�N�G�X�g( �S�� )											*/
/************************************************************************/

void	invalidateAll(void){
	invalidate( 0, 0, g_xxw, g_yyw );									// �ĕ`�惊�N�G�X�g
#	ifdef	_WIN32
	SetScroll();
#	endif
}

/************************************************************************/
/*	�ĕ`�惊�N�G�X�g( ���[�h )											*/
/************************************************************************/

void	invalidateMod(void){
	invalidate( MOD_XXC, 0, MOD_XXD, 1 );								// �ĕ`�惊�N�G�X�g
}

/************************************************************************/
/*	�ĕ`�惊�N�G�X�g( ���b�Z�[�W )										*/
/************************************************************************/

void	invalidateMsg(void){
	invalidate( MSG_XXC, 0, MSG_XXD, 1 );								// �ĕ`�惊�N�G�X�g
}

/************************************************************************/
/*	�ĕ`�惊�N�G�X�g( �t�@�C�� )										*/
/************************************************************************/

void	invalidateFil(void){
	invalidate( FIL_XXC, 0, g_xxw-FIL_XXC, 1 );							// �ĕ`�惊�N�G�X�g
}

/************************************************************************/
/*	�ĕ`�惊�N�G�X�g( �C���t�H���[�V���� )								*/
/************************************************************************/

void	invalidateInf(void){
	invalidate( 0, 0, g_xxw, 1 );										// �ĕ`�惊�N�G�X�g
}

/************************************************************************/
/*	�ĕ`�惊�N�G�X�g( �s )												*/
/************************************************************************/

void	invalidateLin( int yy ){
	invalidateTxt( 0, yy, g_xxw, 1 );
}

/************************************************************************/
/*	�ĕ`�惊�N�G�X�g( �s�ԍ� )											*/
/************************************************************************/

void	invalidateLno( int yy ){
	invalidate( 0, g_tyyc+yy, g_lnosiz, 1 );							// �ĕ`�惊�N�G�X�g
}

/************************************************************************/
/*	�ĕ`�惊�N�G�X�g( �w���v )											*/
/************************************************************************/

void	invalidateHlp(void){
	invalidate( 0, 0, 0, 0 );											// �ĕ`�惊�N�G�X�g
}

/************************************************************************/
/*	�ĕ`�惊�N�G�X�g( ��s�ҏW )										*/
/************************************************************************/

void	invalidateLinedt(void){
	invalidate( 0, 0, 0, 0 );											// �ĕ`�惊�N�G�X�g
}

/************************************************************************/
/*	�J�[�\���\��														*/
/************************************************************************/

int		curSw( int cursw ){

	int		oldcursw;

	oldcursw=g_cursw;
	if( g_cursw!=cursw ){
		g_cursw =cursw;
		invalidateTxt( 0, CURYY, g_txxd, 1 );
	}
	return( oldcursw );
}

/************************************************************************/
/*	�\���R���t�B�O���[�V����											*/
/************************************************************************/

void	dspConfig( int xw, int yw ){

	g_xw	=xw;														// �N���C�A���g�G���A����
	g_yw	=yw;														// �N���C�A���g�G���A�c��
	g_xxw	=(g_xw+g_fxw-1)/g_fxw;
	g_yyw	=(g_yw+g_fyw-1)/g_fyw;
	g_txxc	=g_lnosiz;													// �s�ԍ��\���T�C�Y
	g_tyyc	=1;
	g_txxd	=g_xxw-g_lnosiz;
	g_tyyd	=g_yyw-1;
	if( CURYY>=g_tyyd ){												// �͂ݏo�Ă���
		CURYY=g_tyyd-1;													// �ŏI�s��
		CURP=(stLIN*)dbSeek( 0, PGP, CURYY );							// ���̃y�[�W�擪�܂ŃV�[�N���Ă݂�
	}
}

/************************************************************************/
/*	���b�Z�[�W�Z�b�g													*/
/************************************************************************/

void	msgSet( const char *strp ){
	if( strcmp( g_msgstr, strp ) ){										// �Ⴄ
		strcpy( g_msgstr, strp );										// �o�b�t�@�ɃR�s�[
		invalidateMsg();
	}
}

/************************************************************************/
/*	Y/N �I��															*/
/************************************************************************/

int		YorN( const char *msgstr ){

	int		res=YN_CANCEL;												// ���U���g
	int		keysym;														// �L�[�V���{��

	msgSet( msgstr );													// ���b�Z�[�W�Z�b�g
	for(;;){
		keysym=getKey();												// �L�[�𓾂�
		if( keysym==MKEY_ESC || keysym==0x03 ){							// �L�����Z��
			break;
		}
		if( keysym=='n' || keysym=='N' ){								// �m�[
			res=YN_NO;													// ���U���g���m�[�ɃZ�b�g
			break;
		}
		if( keysym=='y' || keysym=='Y' ){								// �C�G�X
			res=YN_YES;													// ���U���g���C�G�X�ɃZ�b�g
			break;
		}
	}
	msgSet( "" );														// ���b�Z�[�W�N���A
	return( res );														// ���U���g�������ă��^�[��
}

