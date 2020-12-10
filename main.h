/************************************************************************/
/*																		*/
/*	X-Window & Windows �e�L�X�g�G�f�B�^	by H.Watanabe					*/
/*	Ver3.00	2009/04/04 13:49:05	�쐬�J�n								*/
/*																		*/
/************************************************************************/

#	ifndef	MAIN_H
#	define	MAIN_H

#	ifdef	_WIN32
#	include	<windows.h>													/* Windows �w�b�_�[�t�@�C��*/
#	else
#		ifndef	CURSES
#			include <X11/Xlib.h>
#			include	"std/xsel.h"										/* �Z���N�V�����Ǘ�*/
#		else
#			define	_XOPEN_SOURCE_EXTENDED	1
#			include	<curses.h>
#		endif
#	endif
#	include	<sys/stat.h>												/* �t�@�C���X�e�[�^�X*/
#	include	"std/db2.h"													/* �v���O�������ėp�f�[�^�x�[�X�V�X�e��*/
#	include	"std/lm.h"													/* ���C���}�b�v*/
#	include	"std/mkey.h"												/* �}���`�L�[�Ǘ�*/

/************************************************************************/
/*	���[�J����`														*/
/************************************************************************/

#	define	CRTL			"xed Ver2.00"								/* �N���G�[�g�c�[����*/
#	define	SKPSP(P)		((P)+strspn(P,"\t "))						/* �X�y�[�X�A�^�u�̃X�L�b�v*/
#	define	CLAMP(X,L,H)	(((X)>(H))?(H):(((X)<(L))?(L):(X)))			/* �N�����v*/
#	define	RNGTSTB(C,L,H)	((unsigned char)((C)-(L))<=(unsigned char)((H)-(L)))				/* ���������W�r�b�g�Ń����W�`�F�b�N������*/

#	ifdef	_WIN32
#		define	CLPBRDP		g_hwnd										/* �N���b�v�{�[�h�ɕK�v�ȏ��*/
#		define	XColor		int
#	else
#		define	RGB(R,G,B)	((R<<16)+(G<<8)+(B))
#		ifndef	CURSES
#			define	CLPBRDP	g_dpyp										/* �N���b�v�{�[�h�ɕK�v�ȏ��*/
#		else
#			define	CLPBRDP	0											/* �N���b�v�{�[�h�ɕK�v�ȏ��*/
#			define	XColor	int
#		endif
#	endif

	typedef struct{
		XColor			col;											// XColor
	}	stXCOL;															// XColor �\����

#	define	YN_CANCEL		0											/* �L�����Z��*/
#	define	YN_NO			1											/* �m�[*/
#	define	YN_YES			2											/* �C�G�X*/

#	define	CR_BLACK		RGB(0x00,0x00,0x00)							/* ��*/
#	define	CR_WHITE		RGB(0xe0,0xe0,0xe0)							/* ��*/
#	define	CR_LB			RGB(0x88,0xa0,0xb8)							/* ���C�g�u���[*/
#	define	CR_LNO			RGB(0xa0,0xa0,0xa0)							/* �s�ԍ�*/
#	define	CR_LNO_BK		RGB(0x28,0x28,0x28)							/* �s�ԍ��w�i�F*/
#	define	CR_MSG_BK		RGB(0x88,0xa0,0xbc)							/* ���b�Z�[�W�w�i�F*/
#	define	CR_MOD_BK		RGB(0x60,0x98,0xc0)							/* ���[�h�w�i�F*/
#	define	CR_OVER			RGB(0xd0,0x30,0x30)							/* �㏑���J�[�\���F*/

#	define	CRTYP_WIN		0											/* 0d0a*/
#	define	CRTYP_UNIX		1											/* 0a*/

#	if	defined(_WIN32) || defined(__CYGWIN__)							/* WINDOWS*/
#		define	CRTYP_DEFAULT	CRTYP_WIN								/* �f�t�H���g���s�^�C�v*/
#	else
#		define	CRTYP_DEFAULT	CRTYP_UNIX								/* �f�t�H���g���s�^�C�v*/
#	endif

	typedef struct{
		int					chgflg;										// �ύX�t���O
	}	stLIN;															// �s�\����

	typedef struct{
		stDB				*lindbp;									// �sDB
		stLIN				*pgp;										// �y�[�W�擪
		int					pgxx, pgyy;									// �y�[�W���W
		stLIN				*curp;										// �ҏW���̍s
		int					curxx, curyy;								// �J�[�\�����W
		int					chgflg;										// �ύX�t���O
		int					cuttyp;										// �J�b�g�^�C�v( 0:�s, 1:��` )
		stLIN				*cutpgp;									// �J�b�g�擪�y�[�W�|�C���^
		stLIN				*cutcurp;									// �J�b�g�擪�|�C���^
		int					cutpgxx, cutpgyy;							// �J�b�g�擪�y�[�W���W
		int					cutcurxx, cutcuryy;							// �J�b�g�擪���W
		int					crtyp;										// ���s�^�C�v
		int					statflg;									// �P�ɂȂ��Ă���� st �L��
		struct stat			st;											// �t�@�C���X�e�[�^�X
	}	stCTX;															// �R���e�L�X�g�\����

#	ifdef	MAIN_CPP
#		define	EXTERN
#	else
#		define	EXTERN	extern
#	endif

#	ifdef	_WIN32

	EXTERN	HWND			g_hwnd;										// ���C���E�B���h�E�n���h��
	EXTERN	HFONT			g_hfont;									// �t�H���g�n���h��
	EXTERN	HDC				g_hdc;										// �f�o�C�X�R���e�L�X�g

#	else

#		ifndef	CURSES

	EXTERN	Display			*g_dpyp;									// �f�B�X�v���C
	EXTERN	Window			g_win;										// �E�B���h�E
	EXTERN	Colormap		g_cmap;										// �J���[�}�b�v
	EXTERN	GC				g_gc;										// �f�b
	EXTERN	XFontSet		g_fs;										// ���{�ꃂ�[�h�t�H���g
	EXTERN	XIM				g_im;
	EXTERN	XIMStyle		g_style;
	EXTERN	XIC				g_ic;
	EXTERN	int				g_fep;										// FEP�I�[�v���t���O
	EXTERN	unsigned long	g_tim;
	EXTERN	int				g_fntascent;								// �t�H���g�A�Z���g
	EXTERN	int				g_jp;										// ���{�ꃂ�[�h�t���O
	EXTERN	stXSEL			*g_xselp;									// �Z���N�V�����Ǘ�

#		endif

#	endif

	EXTERN	int				g_optv;										// �o�[�{�[�Y���[�h( ���邳�����[�h )
	EXTERN	int				g_optz;										// �f�o�b�O���[�h
	EXTERN	int				g_optn;										// *.bak �t�@�C�����쐬���Ȃ�
	EXTERN	int				g_optl;										// [EOL]��\�����Ȃ�
	EXTERN	int				g_opte;										// [EOF]��\�����Ȃ�
	EXTERN	char			g_optfn[256];								// �t�H���g��
	EXTERN	int				g_optfx, g_optfy;							// �t�H���g�T�C�Y
	EXTERN	int				g_optxn, g_optyn;							// ��ʉ����A�c��( �L�����N�^���W )
	EXTERN	int				g_optlno;									// �s�ԍ��w��
	EXTERN	int				g_optu;										// �����p�ꃂ�[�h

	EXTERN	int				g_frmxw, g_frmyw;							// �E�B���h�E�t���[���T�C�Y
	EXTERN	int				g_fxw, g_fyw;								// �t�H���g�����A�c��
	EXTERN	int				g_xw, g_yw;									// �E�B���h�E�����A�c��
	EXTERN	int				g_xxw, g_yyw;								// �E�B���h�E�����A�c��( �L�����N�^���W�n )
	EXTERN	int				g_txxc, g_tyyc, g_txxd, g_tyyd;				// �e�L�X�g���W( �L�����N�^���W�n )
	EXTERN	stDB			*g_ctxdbp;									// �R���e�L�X�gDB
	EXTERN	stCTX			*g_ctxp;									// �R���e�L�X�g�|�C���^
	EXTERN	stMKEY			*g_mkeyp;									// �}���`�L�[�Ǘ��\���̃|�C���^
	EXTERN	int				g_flgend;									// �I���t���O
	EXTERN	int				g_tabstp;									// �^�u�X�e�b�v
	EXTERN	int				g_tabdsp;									// �^�u�\���t���O
	EXTERN	int				g_lnosiz;									// �s�ԍ��\���T�C�Y
	EXTERN	int				g_cursw;									// �J�[�\���\���X�C�b�`
	EXTERN	int				g_flgeof;									// [EOF]�\���t���O
	EXTERN	int				g_flgcr;									// �L�����b�W���^�[���\���t���O
	EXTERN	int				g_flgins;									// �C���T�[�g�t���O
	EXTERN	int				g_flgl2c;									// ����������啶���ϊ��t���O
	EXTERN	char			g_msgstr[256];								// ���b�Z�[�W������
	EXTERN	char			g_cutlin[LM_MAX+4];							// ��s�J�b�g�o�b�t�@
	EXTERN	int				g_flglbl;									// �����A�u�����̃��x���F��
	EXTERN	int				g_flgh;										// �s�ԍ��P�U�i�\��
	EXTERN	int				g_flgwld;									// �����A�u�����̃��C���h�J�[�h�L���t���O
	EXTERN	int				g_flgc2l;									// �������̑啶����������ʃt���O
	EXTERN	char			g_reporg[256], g_repnew[256];				// �u��������
	EXTERN	int				g_scdbiz;									// �����\���T�C�Y
	EXTERN	stDB			*g_macalcdbp;								// �L�[�}�N���o�^DB
	EXTERN	stDB			*g_macrsvdbp;								// �L�[�}�N���\��DB
	EXTERN	stDB			*g_macexedbp;								// �L�[�}�N�����sDB

#	undef	EXTERN

#	define	LINDBP			g_ctxp->lindbp
#	define	CURXX			g_ctxp->curxx
#	define	CURYY			g_ctxp->curyy
#	define	CURP			g_ctxp->curp
#	define	PGP				g_ctxp->pgp
#	define	PGXX			g_ctxp->pgxx
#	define	PGYY			g_ctxp->pgyy
#	define	FILCHG			g_ctxp->chgflg
#	define	FNAMP			((char*)dbKEYADR( g_ctxp ))
#	define	CURTXTP			((char*)dbKEYADR( CURP ))
#	define	CUTTYP			g_ctxp->cuttyp
#	define	CUTPGP			g_ctxp->cutpgp
#	define	CUTCURP			g_ctxp->cutcurp
#	define	CUTPGXX			g_ctxp->cutpgxx
#	define	CUTPGYY			g_ctxp->cutpgyy
#	define	CUTCURXX		g_ctxp->cutcurxx
#	define	CUTCURYY		g_ctxp->cutcuryy

//	xed.cpp

	void	SetScroll(void);
	int		getKey(void);
	void	fepopen(void);

//	ctx.cpp

	stCTX	*ctxOpen( const char *fnamp );
	void	ctxClose( stCTX *ctxp );
	int		ctxSave( const stCTX *ctxp );
	stCTX	*ctxRename( stCTX *ctxp, const char *fnamp );

//	help.cpp

	void	helpReDsp(void);
	void	help(void);

//	lin.cpp

	int		linAlcSiz( int siz );
	int		linAlcSizStr( const char *txtadr );
	stLIN	*linAlc( stDB *lindbp, const char *txtadr, int txtsiz );
	stLIN	*linAlcStr( stDB *lindbp, const char *txtadr );
	int		linAdjScr(void);

//	display.cpp

	void	displayStartUp(void);
	void	displayCleanUp(void);
	stXCOL	*colorref2xcolor( unsigned long colorref );
	void	drwTxt( int xx, int yy, const char *txtp, int txtsiz, unsigned long bakcol, unsigned long forcol );
	void	drwFil( int xx1, int yy1, int xx2, int yy2, unsigned long col );
	void	displayChrBox( int xxc, int yyc, int xxd, int yyd, const char *msgp );
	void	display( int x1, int y1, int x2, int y2 );
	void	invalidate( int xxc, int yyc, int xxd, int yyd );
	void	invalidateTxt( int xxc, int yyc, int xxd, int yyd );
	void	invalidateAll(void);
	void	invalidateMod(void);
	void	invalidateMsg(void);
	void	invalidateFil(void);
	void	invalidateInf(void);
	void	invalidateLin( int yy );
	void	invalidateLno( int yy );
	void	invalidateHlp(void);
	void	invalidateLinedt(void);
	int		curSw( int cursw );
	void	dspConfig( int xw, int yw );
	void	msgSet( const char *strp );
	int		YorN( const char *msgstr );

//	xedmain.cpp

	void	xedmain(void);

//	linedt.cpp

	void	linedtReDsp(void);
	int		linedt( char *bufadr, const char *ttlp );
	int		linedt2( char *buf1adr, const char *ttl1p, char *buf2adr, const char *ttl2p );

//	replace.cpp

	void	replace( const char *orgp, const char *newp, int bakflg, int chkflg, int notdsp );

//	cmdprc.cpp

	void	curset( int musx, int musy );
	void	linchg( stLIN *linp );
	stLIN	*linflush( stLIN *orgp, const char *txtp );
	stLIN	*renewtxt( stLIN *orgp, int yy, const char *txtp );
	void	lnoset( int lno );
	void	pgscroll( int ofs );
	void	setchr( int xedk );
	void	getcurword( char *dstp );
	void	esc_esc(void);
	void	esc(void);
	void	esc_0(void);
	void	esc_1(void);
	void	esc_2(void);
	void	esc_3(void);
	void	esc_4(void);
	void	esc_5(void);
	void	esc_6(void);
	void	esc_7(void);
	void	esc_8(void);
	void	esc_9(void);
	void	esc_a(void);
	void	esc_b(void);
	void	esc_c(void);
	void	esc_d(void);
	void	esc_e(void);
	void	esc_f(void);
	void	esc_h(void);
	void	esc_i(void);
	void	esc_j(void);
	void	esc_k(void);
	void	esc_l(void);
	void	esc_m(void);
	void	esc_n(void);
	void	esc_o(void);
	void	esc_q(void);
	void	esc_r(void);
	void	esc_s(void);
	void	esc_t(void);
	void	esc_u(void);
	void	esc_v(void);
	void	esc_x(void);
	void	esc_y(void);
	void	esc_z(void);
	void	esc_lbo(void);
	void	esc_yen(void);
	void	esc_lbc(void);
	void	esc_hat(void);
	void	esc__(void);
	void	ctrl_a(void);
	void	ctrl_b(void);
	void	ctrl_c(void);
	void	ctrl_d(void);
	void	ctrl_e(void);
	void	ctrl_f(void);
	void	ctrl_g(void);
	void	ctrl_h(void);
	void	ctrl_j(void);
	void	ctrl_k(void);
	void	ctrl_l(void);
	void	ctrl_m(void);
	void	ctrl_n(void);
	void	ctrl_o(void);
	void	ctrl_p(void);
	void	ctrl_q(void);
	void	ctrl_r(void);
	void	ctrl_s(void);
	void	ctrl_t(void);
	void	ctrl_u(void);
	void	ctrl_v(void);
	void	ctrl_w(void);
	void	ctrl_x(void);
	void	ctrl_y(void);
	void	ctrl_z(void);
	void	ctrl_yen(void);
	void	ctrl_lbktc(void);
	void	ctrl_hat(void);
	void	ctrl__(void);
	void	f3(void);
	void	f5(void);
	void	f6(void);
	void	f7(void);
	void	f8(void);
	void	f9(void);
	void	f10(void);
	void	f11(void);
	void	f12(void);
	void	shift_f5(void);
	void	shift_f6(void);
	void	shift_f7(void);
	void	shift_f8(void);
	void	shift_f9(void);
	void	ctrl_tab(void);
	void	alt_f5(void);
	void	alt_f9(void);
	void	alt_f10(void);
	void	alt_slash(void);
	void	alt_ast(void);
	void	alt_plus(void);
	void	shift_space(void);
	void	ctrl_f9calbak( stDB *cbdbp );
	void	ctrl_f9(void);

#	endif

