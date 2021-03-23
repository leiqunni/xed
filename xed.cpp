/************************************************************************/
/*																		*/
/*	X-Window & Windows �e�L�X�g�G�f�B�^	by H.Watanabe					*/
/*	Ver2.00	2009/04/04 13:49:05	�쐬�J�n								*/
/*																		*/
/************************************************************************/

#	define	MAIN_CPP
#	include	<assert.h>													/* �A�T�[�V����*/
#	include	<stdio.h>													/* �W�����o�͊֐�*/
#	include	<stdlib.h>													/* �W�����C�u�����֐�*/
#	include	<string.h>													/* �W��������֐�*/
#	include	"std/about2.h"												/* �o�[�W�������{�b�N�X�p���b�Z�[�W�n���h��*/
#	include	"std/opt.h"													/* �I�v�V��������*/
#	include	"std/filedb.h"												/* �f�[�^�x�[�X�t�@�C���o�^*/
#	include	"std/clpbrd.h"												/* �N���b�v�{�[�h�Ǘ�*/
#	include	"resource.h"												/* ���\�[�X�t�@�C��*/
#	include	"main.h"													/* ���C���C���N���[�h�t�@�C��*/
#	ifndef	_WIN32
#		include	<unistd.h>												/* UNIX �X�^���_�[�h*/
#		include	<locale.h>												/* ���[�J���C�[�[�V����*/
#		ifndef	CURSES
#			include	<X11/keysym.h>
#			include	<X11/Xatom.h>
#		else
#			include	<signal.h>											/* signal����*/
#		endif
#	endif

/************************************************************************/
/*	�X�^�[�g�A�b�v														*/
/************************************************************************/

static void	startup(void){
	g_mkeyp=mkeyOpen();													// �}���`�L�[�Ǘ��I�[�v��
	displayStartUp();													// �f�B�X�v���C�X�^�[�g�A�b�v
}

/************************************************************************/
/*	�N���[���i�b�v														*/
/************************************************************************/

static void	cleanup(void){
	mkeyClose( g_mkeyp );												// �}���`�L�[�Ǘ��N���[�Y
	dbClose( g_macalcdbp );												// �L�[�}�N���o�^DB
	dbClose( g_macrsvdbp );												// �L�[�}�N���\��DB
	dbClose( g_macexedbp );												// �L�[�}�N�����sDB
	displayCleanUp();													// �f�B�X�v���C�N���[���i�b�v
}

/************************************************************************/
/*	�R�}���h���C������													*/
/************************************************************************/

static int	cmdlinprc( int argc, char *argv[] ){

	int			errn=0;
	cOPT		opt;
	stDB		*argdbp;
	stDB		*filedbp;												// �R�}���h���C���t�@�C��DB
	stFILE		*filep;													// �t�@�C���\���̃|�C���^

//	���[�N������

	g_optxn=96;															// �L�����N�^����
	g_optyn=56;															// �L�����N�^�c��
	g_tabstp=4;															// �^�u�X�e�b�v
	g_lnosiz=5;															// �s�ԍ��\���T�C�Y
	g_flgins=1;															// �C���T�[�g�t���O
	g_flgl2c=1;															// ����������啶���ϊ��t���O
	strcpy( g_optfn, "�l�r �S�V�b�N" );									// �t�H���g��
	g_optfx=8;															// �t�H���g�T�C�Y��
	g_optfy=16;															// �t�H���g�T�C�Y�c

//	�R�}���h���C������

	opt.SetTitle( "X-Window & Windows Text Editor " );					// �R�}���h�^�C�g��
	opt.SetVer(   "3.00" );												// �o�[�W����
	opt.SetLine(  "<�t�@�C��>..." );									// ���C�����
	opt.AllocInc( "v", &g_optv, "Verbose Mode" );						// �C���N�������g�I�v�V�����o�^
	opt.AllocInc( "z", &g_optz, "Debug Mode" );							// �C���N�������g�I�v�V�����o�^
	opt.AllocInc( "n", &g_optn, "*.bak �t�@�C�����쐬���Ȃ�" );			// �C���N�������g�I�v�V�����o�^
	opt.AllocInt( "h", &g_tabstp, "TAB Step( default=4 )" );			// �C���g�I�v�V�����o�^
#	ifndef	_WIN32
	opt.AllocInc( "u", &g_optu, "English Mode" );						// �C���N�������g�I�v�V�����o�^
#	endif
	opt.AllocInc( "l", &g_optl, "Display [EOL]" );						// �C���N�������g�I�v�V�����o�^
	opt.AllocInc( "e", &g_opte, "Display [EOF]" );						// �C���N�������g�I�v�V�����o�^
	opt.AllocInt( "fx", &g_optfx, "FONT SIZE X" );						// �C���g�I�v�V�����o�^
	opt.AllocInt( "fy", &g_optfy, "FONT SIZE Y" );						// �C���g�I�v�V�����o�^
	opt.AllocInt( "xn", &g_optxn, "char of X" );						// �C���g�I�v�V�����o�^
	opt.AllocInt( "yn", &g_optyn, "char of Y" );						// �C���g�I�v�V�����o�^
	opt.AllocInt( "lno", &g_optlno, "�s�ԍ�" );							// �C���g�I�v�V�����o�^

	argdbp=opt.PrcArg( argc, argv );									// �I�v�V�����A��������
	if( argdbp ){
		filedbp=filedbAlcDB( argdbp, FILEATR_ALL );
		if( !dbAlcN( filedbp ) ){
			filedbp=filedbAlc( filedbp, "noname", FILEATR_REG|FILEATR_NON );	// �t�@�C�����o�^
		}
		g_flgcr=!g_optl;												// ���s������\��
		g_flgeof=!g_opte;												// [EOF]��\��
		g_ctxdbp=dbOpen( DBSIZ_STR, sizeof(stCTX), 256, 4096 );			// �R���e�L�X�gDB�I�[�v��
		dbFOR( filedbp, filep, stFILE* ){								// �t�@�C��������
			ctxOpen( (char*)dbKEYADR( filep ) );						// �R���e�L�X�g�I�[�v��
		}
		g_ctxp	=(stCTX*)dbTop( g_ctxdbp );								// �J�����g�R���e�L�X�g�|�C���^�Z�b�g
		dbClose( filedbp );												// �t�@�C��DB�N���[�Y
	}else{
		opt.Usage( argv[0] );											// �g�p�@�\��
		errn++;
	}
	return( !errn );
}

/************************************************************************/
/*	�}�N�������t�L�[�𓾂�												*/
/************************************************************************/

int		getmackey(void){

	int		*keyp, key;													// �L�[�|�C���^�A�L�[

	key=mkeyGet( g_mkeyp );												// �L�[�𓾂�
	if( key ){															// �L�[�|�C���^�͗L��
		if( dbAlcN( g_macexedbp )==0 ){									// �}�N�����ł͂Ȃ�
			if( g_macalcdbp ){											// �}�N���o�^��
				if( dbAlcN( g_macalcdbp )<4096 ){						// �}�N���͒������Ȃ�
					dbAlc( g_macalcdbp, &key );							// �}�N���o�^
				}else{													// �}�N������������
					msgSet( "Macro Too Long" );
				}
			}
			return( key );												// �L�[�������ă��^�[��
		}
		if( key==0x1b ){												// ESC�}�N�����f
			dbClose( g_macexedbp );
			g_macexedbp=0;
			msgSet( "MACRO ABORT" );
		}
	}
	keyp=(int*)dbTop( g_macexedbp );
	if( keyp ){
		key=*keyp;
		dbFre( g_macexedbp, keyp );
		return( key );
	}
	return( 0 );
}

#	ifdef	_WIN32

/************************************************************************/
/*																		*/
/*	WINDOWS																*/
/*																		*/
/************************************************************************/
/*	�X�N���[���C���t�H���[�V�����Z�b�g									*/
/************************************************************************/

void	SetScroll(void){

	SCROLLINFO	si={ 0 };												// �X�N���[���C���t�H���[�V�����\����

	si.cbSize	=sizeof(si);											// �\���̃T�C�Y
	si.fMask	=SIF_PAGE | SIF_RANGE | SIF_POS;						// �L���t���O
	si.nMin		=0;														// �X�N���[���ŏ��l
	si.nMax		=dbAlcN( LINDBP );										// �X�N���[���ő�l
	si.nPage	=g_tyyd;												// �y�[�W�T�C�Y
	si.nPos		=PGYY;													// �X�N���[���ʒu
	SetScrollInfo( g_hwnd, SB_VERT, &si, TRUE );						// �X�N���[���C���t�H���[�V�����Z�b�g
}

/************************************************************************/
/*	�X�N���[������														*/
/************************************************************************/

static void	setscroll( int sb, int thumb ){

	int		yyw;
	int		yymax;
	int		yy;
	int		sgn;														// ����

	yyw		=g_tyyd;
	yymax	=dbAlcN( LINDBP )-yyw;
	if( yymax<0 ){
		yymax=0;
	}
	yy		=PGYY;
	switch( sb ){
		case SB_LINEUP:			yy -=1;		break;						// ���C�����Z
		case SB_LINEDOWN:		yy +=1; 	break;						// ���C�����Z
		case SB_PAGEUP:			yy -=yyw;	break;						// �y�[�W���Z
		case SB_PAGEDOWN:		yy +=yyw; 	break;						// �y�[�W���Z
		case SB_THUMBPOSITION:	yy =thumb;	break;						// �w�胉�C��
		case SB_THUMBTRACK:		yy =thumb;	break;						// �w�胉�C��( �g���b�L���O�� )
		case WM_MOUSEWHEEL:{											// �}�E�X�z�C�[��
			sgn=1-( thumb<0 )*2;										// �����Z�b�g( -1 or 1 )
			yy-=((thumb*sgn)>>5)*sgn;
			break;
		}
	}
	yy	=CLAMP( yy, 0, yymax );
	if( yy!=PGYY ){
		pgscroll( yy-PGYY );
		SetScroll();													// �X�N���[���C���t�H���[�V�����Z�b�g
		InvalidateRect( g_hwnd, NULL, FALSE );							// �E�B���h�E�̍X�V�̈�ɁA�����`��ǉ����܂��B�X�V�̈�Ƃ́A�E�B���h�E�̃N���C�A���g�̈�̂����A�ĕ`�悵�Ȃ���΂Ȃ�Ȃ������̂��Ƃł��B
	}
}

/************************************************************************/
/*	�t�H���g�I���_�C�A���O												*/
/************************************************************************/

void	FontDiaog(void){

	LOGFONT		lf={ 0 };
	CHOOSEFONT	cf={ 0 };
	HDC			hdc;
	HFONT		hfont;
	int			fxw, fyw;

	GetObject( g_hfont, sizeof(lf), &lf );								// LOGFONT �����擾
	hdc=GetDC( g_hwnd );
	cf.lStructSize	=sizeof(cf);
	cf.hDC			=hdc;
	cf.hwndOwner	=g_hwnd;
	cf.lpLogFont	=&lf;
	cf.Flags		=CF_SCREENFONTS | CF_FIXEDPITCHONLY | CF_INITTOLOGFONTSTRUCT | CF_NOVERTFONTS;
	cf.nFontType	=SCREEN_FONTTYPE;
	if( ChooseFont( &cf ) ){
		fyw=lf.lfHeight;
		if( fyw<0 ){
			fyw=-fyw;
		}
		fxw=fyw/2;
		hfont	=CreateFont(											// �_���t�H���g���쐬���܂��B
			-fyw,														// �t�H���g�̍�����_���P�ʂŎw�肵�܂��B
			fxw,														// �t�H���g�̕��ϕ�������_���P�ʂŎw�肵�܂��B0���w�肷��ƓK���ȕ����I������܂�
			0,															// ��������̕����� X ���Ƃ̊p�x���A10 ���� 1 �x�P�ʂŎw�肵�܂��B��������̕����́A�e�L�X�g�̗�̃x�[�X���C���ƕ��s�ł��B
			0,															// �e�����̃x�[�X���C���� X ���Ƃ̊p�x���A10 ���� 1 �x�P�ʂŎw�肵�܂��B
			FW_DONTCARE,												// �t�H���g�̑������A0 ���� 1000 �܂ł̒l�Ŏw�肵�܂��B0 ���w�肷��ƁA�f�t�H���g�̑������I������܂��B
			0,															// �C�^���b�N�̂ɂ��邩�ǂ������w�肵�܂��BTRUE ���w�肷��ƁA�C�^���b�N�̂��I������܂��B
			0,															// �����t���̃t�H���g��I�����邩�ǂ������w�肵�܂��BTRUE ���w�肷��ƁA�����t���t�H���g���I������܂��B
			0,															// �ł��������t���t�H���g��I�����邩�ǂ������w�肵�܂��BTRUE ���w�肷��ƁA�ł��������t���̃t�H���g���I������܂��B
			DEFAULT_CHARSET,											// �t�H���g�̕����Z�b�g���w�肵�܂��B
			OUT_DEFAULT_PRECIS,											// �o�͐��x���w�肵�܂��B���̒l�̂����ꂩ���w�肵�܂��B
			CLIP_DEFAULT_PRECIS,										// �N���b�s���O���x���w�肵�܂��B�N���b�s���O���x�Ƃ́A�����̈ꕔ���N���b�s���O�̈�̊O�ɂ͂ݏo���Ƃ��ɁA���̕������N���b�v������@���`������̂ł��B
			DRAFT_QUALITY,												// �o�͕i�����w�肵�܂��B�o�͕i���Ƃ́AGDI ���A�_���t�H���g�̑����Ǝ��ۂ̕����t�H���g�̑����Ƃ��ǂ̒��x�܂ň�v�����邩���`������̂ł��B
			FIXED_PITCH | FF_DONTCARE,									// �t�H���g�̃s�b�`�ƃt�@�~�����w�肵�܂��B���� 2 �r�b�g�Ńt�H���g�̃s�b�`���w�肵�܂��B( DEFAULT_PITCH, FIXED_PITCH, VARIABLE_PITCH )
			lf.lfFaceName												// �t�H���g�̃^�C�v�t�F�C�X���������� NULL �ŏI��镶����ւ̃|�C���^���w�肵�܂��B������̒����́A�I�[�� NULL �������܂߂� 32 �����ȉ��ɂ��Ă��������BNULL �|�C���^��A��̕�����ւ̃|�C���^���w�肷��ƁA�ق��̃p�����[�^�Ŏw�肵�������ɍ����t�H���g�̒�����ŏ��Ɍ������t�H���g���I������܂��B���ݗ��p�\�ȃt�H���g�̃^�C�v�t�F�C�X����񋓂���ɂ́AEnumFontFamilies �֐����g���܂��B
		);																// �߂�l:�֐�����������ƁA�_���t�H���g�̃n���h�����Ԃ�܂��B�֐������s����ƁANULL ���Ԃ�܂��B�g���G���[�����擾����ɂ́AGetLastError �֐����g���܂��B
		if( hfont ){
			DeleteObject( g_hfont );									// �t�H���g�j��
			g_hfont=hfont;
			g_fxw=fxw;
			g_fyw=fyw;
			dspConfig( g_xw, g_yw );
			InvalidateRect( g_hwnd, 0, FALSE );							// �ĕ`��
		}
	}
	ReleaseDC( g_hwnd, hdc );
}

/************************************************************************/
/*	���C�� �E�B���h�E�̃��b�Z�[�W���������܂��B							*/
/************************************************************************/

static LRESULT CALLBACK WndProc( HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam ){

	LRESULT				res=0;											// �A��l( 0:��������, 1:������ )
	PAINTSTRUCT			ps;												// �y�C���g�\����
	static int			xedatr=0;
	int					xedk;											// ���̓L�[�V���{��

	switch( message ) {
		case WM_CREATE:{												// 0001:�E�C���h�E���쐬��
			g_hwnd=hwnd;												// �E�B���h�E�n���h���Z�b�g
			SetClassLong( hwnd, GCL_HBRBACKGROUND, 0 );					// �w�i�F�𖳂��ɂ���( �����G���A��w�i�F�ŏ��������Ȃ��悤�ɂ��� )
			break;
		}
		case WM_COMMAND:{												// �A�v���P�[�V���� ���j���[�̏���
			switch( LOWORD( wparam ) ){									// ���j���[�A�C�e���A�R���g���[���A�A�N�Z�����[�^�̎��ʎq
				case IDM_ABOUT:{
					DialogBox( GetModuleHandle( 0 ), ( LPCTSTR )IDD_ABOUTBOX, hwnd, ( DLGPROC )about );
					break;
				}
				case IDM_EXIT:{											// �I��
					DestroyWindow( hwnd );								// ���̊֐��́AWM_DESTROY ���b�Z�[�W��WM_NCDESTROY ���b�Z�[�W���E�B���h�E�ɑ���A�L�[�{�[�h�̓��̓t�H�[�J�X���E�B���h�E�����菜���܂��B�܂��A�E�B���h�E���j���[�̔j���A�X���b�h�̃��b�Z�[�W�L���[�̃t���b�V���A�^�C�}�[�̔j���A�N���b�v�{�[�h�̏��L���̉���A�N���b�v�{�[�h�r���[�A�`�F�C���̐ؒf���s���܂��B
					break;
				}
				case ID_FONT:{											// �t�H���g�_�C�A���O
					FontDiaog();
					break;
				}
				default:{												// ���̑�
					res=DefWindowProc( hwnd, message, wparam, lparam );
					break;
				}
			}
			break;
		}
		case WM_WINDOWPOSCHANGING:{										// �E�B���h�E�ʒu�T�C�Y���ύX�����O�ɃN���C�A���g�ɑ����Ă��郁�b�Z�[�W

			WINDOWPOS	*winposp=( WINDOWPOS* )lparam;
			int			xw, yw;											// �N���C�A���g�G���A�̉���,�c��

			xw=winposp->cx-g_frmxw;										// �N���C�A���g�G���AX��
			yw=winposp->cy-g_frmyw;										// �N���C�A���g�G���AY��
			winposp->cx = (xw/g_fxw)*g_fxw+g_frmxw;						// �t�H���g�P�ʂɃA�W���X�g���ꂽX��
			winposp->cy = (yw/g_fyw)*g_fyw+g_frmyw;						// �t�H���g�P�ʂɃA�W���X�g���ꂽY��
			break;
		}
		case WM_SIZE:{													// �E�B���h�E�T�C�Y�ύX���b�Z�[�W
			dspConfig( LOWORD( lparam ), HIWORD( lparam ) );			// �\���R���t�B�O���[�V����
			break;
		}
		case WM_PAINT:{													// ���C���E�B���h�E�̕`��

//			RECT			rect;										// ��`�\����

			g_hdc =BeginPaint( hwnd, &ps );								// �f�o�C�X�R���e�L�X�g�����蓖�ĂĂ����V�̈�Ɋւ������Ԃ��܂��B
			SelectObject( g_hdc, g_hfont );
//			GetClipBox( g_hdc, &rect );									// �E�B���h�E�̕\���͈͂��͂ލŏ��̒����`���擾���܂��B�����ł����\���͈͂́A�E�B���h�E�̃I�[�o�[���b�v��ԁA�N���b�s���O�̈�A�N���b�v�p�X�ɂ���Ē�`����܂��B
//			display( rect.left, rect.top, rect.right, rect.bottom );	// �`��
			display( 0, 0, g_xw, g_yw );
			EndPaint( hwnd, &ps );										// �y�C���g�����̏I�����w�����܂��B
			g_hdc=0;
			break;
		}
		case WM_SYSKEYDOWN:												// �R���g���[���L�[�������ꂽ
		case WM_KEYDOWN:												// �R���g���[���L�[�������ꂽ
			xedk=0;
			switch( wparam ){
				case VK_SHIFT:		xedatr|=MKEY_SHIFT;		break;
				case VK_CONTROL:	xedatr|=MKEY_CTRL;		break;
				case VK_MENU:		xedatr|=MKEY_ALT;		break;
				case VK_PRIOR:		xedk =MKEY_PGUP;		break;
				case VK_NEXT:		xedk =MKEY_PGDOWN;		break;
				case VK_END:		xedk =MKEY_END;			break;
				case VK_HOME:		xedk =MKEY_HOME;		break;
				case VK_LEFT:		xedk =MKEY_LEFT;		break;
				case VK_UP:			xedk =MKEY_UP;			break;
				case VK_RIGHT:		xedk =MKEY_RIGHT;		break;
				case VK_DOWN:		xedk =MKEY_DOWN;		break;
				case VK_INSERT:		xedk =MKEY_INSERT;		break;
				case VK_DELETE:		xedk =MKEY_DELETE;		break;
				case VK_HELP:		xedk =MKEY_HELP;		break;
				case VK_F1:			xedk =MKEY_F1;			break;
				case VK_F2:			xedk =MKEY_F2;			break;
				case VK_F3:			xedk =MKEY_F3;			break;
				case VK_F4:			xedk =MKEY_F4;			break;
				case VK_F5:			xedk =MKEY_F5;			break;
				case VK_F6:			xedk =MKEY_F6;			break;
				case VK_F7:			xedk =MKEY_F7;			break;
				case VK_F8:			xedk =MKEY_F8;			break;
				case VK_F9:			xedk =MKEY_F9;			break;
				case VK_F10:		xedk =MKEY_F10;			break;
				case VK_F11:		xedk =MKEY_F11;			break;
				case VK_F12:		xedk =MKEY_F12;			break;
				case 0x09:
					if( xedatr&MKEY_CTRL ){
						xedk=0x09;
					}
					break;
				case 0x36:
					if( xedatr&MKEY_CTRL ){
						xedk=0x1e;
					}
					break;
				case 0xbd:
					if( xedatr&MKEY_CTRL ){
						xedk=0x1f;
					}
					break;
				case 0xde:												// for PCG-505 CTRL+'^'
					if( xedatr&MKEY_CTRL ){
						xedk=0x1e;
					}
					break;
			}
			if( xedk ){
				xedk|=xedatr;
				mkeyAlc( g_mkeyp, xedk );
			}
			break;
		case WM_SYSKEYUP:												// �R���g���[���L�[�������ꂽ
		case WM_KEYUP:													// �R���g���[���L�[�������ꂽ
			switch( wparam ){
				case VK_SHIFT:		xedatr&=~MKEY_SHIFT;	break;
				case VK_CONTROL:	xedatr&=~MKEY_CTRL;		break;
				case VK_MENU:		xedatr&=~MKEY_ALT;		break;
			}
			break;
		case WM_SYSCHAR:
		case WM_CHAR:{
			if( wparam==9 ){
				xedk=wparam|xedatr;
			}else{
				xedk=wparam|(xedatr&MKEY_ALT);
			}
			if( xedk==0x1b ){
				msgSet( "[ESC]" );										// ���b�Z�[�W�\��
			}else{
				msgSet( "" );											// ���b�Z�[�W�\��
			}
			mkeyPut( g_mkeyp, xedk );
			break;
		}
		case WM_VSCROLL:{
			setscroll( LOWORD( wparam ), HIWORD( wparam ) );
			break;
		}
		case WM_LBUTTONDOWN:{											// ���{�^���������ꂽ
			curset( (short)LOWORD( lparam ), (short)HIWORD( lparam ) );
			break;
		}
		case WM_MOUSEWHEEL:{											// �}�E�X�z�B�[��
			setscroll( WM_MOUSEWHEEL, (short)HIWORD(wparam) );
			break;
		}
		case WM_DROPFILES:{												// �h���b�O�h���b�v

			stCTX	*ctxp;												// �R���e�L�X�g�|�C���^
			char	fnam[4096];											// �t�@�C����
			int		n, i;

			n=DragQueryFile( (HDROP)wparam, 0xffffffff, NULL, 0 );		// �h���b�v���ꂽ�t�@�C����
			for( i=0; i<n; i++ ){										// �h���b�v���ꂽ�t�@�C���������[�v
				DragQueryFile( (HDROP)wparam, i, fnam, sizeof(fnam) );	// �h���b�v���ꂽ�t�@�C�����𓾂�
				ctxp=ctxOpen( fnam );									// �R���e�L�X�g�I�[�v��
				if( !i ){												// �擪
					g_ctxp=ctxp;										// �J�����g�R���e�L�X�g�|�C���^�Z�b�g
				}
			}
			DragFinish( (HDROP)wparam );								// �h���b�O�h���b�v���\�[�X�j��
			InvalidateRect( hwnd, 0, FALSE );							// �ĕ`��
			break;
		}
		case WM_DESTROY:{												// �I�����b�Z�[�W�̒ʒm�ƃ��^�[��
			SetWindowLong( hwnd, GWL_USERDATA, 0 );						// �E�B���h�E�\���̃|�C���^���E�B���h�E�ɃZ�b�g����
			PostQuitMessage( 0 );										// Windows �ɃX���b�h�̏I����v�����܂�( �p�����[�^�͏I���R�[�h )
			break;
		}
		default:{														// ���̑�
			res=DefWindowProc( hwnd, message, wparam, lparam );			// ����̃E�B���h�E�v���V�[�W�����Ăяo���āA�A�v���P�[�V�������������Ȃ��E�B���h�E���b�Z�[�W�ɑ΂��āA����̏�����񋟂��܂��B
			break;
		}
	}
	return( res );
}

/************************************************************************/
/*	(windows)	�L�[�𓾂�												*/
/************************************************************************/

int		getKey(void){

	MSG		msg;
	int		key;

	for(;;){
		if( !PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) ){			// ���b�Z�[�W�͓͂��Ă��Ȃ�
			key=getmackey();											// �}�N�������t�L�[�𓾂�
			if( key ){
				break;
			}
		}
		if( !GetMessage( &msg, NULL, 0, 0 ) ){
			g_flgend=1;													// �I���t���O�Z�b�g
			return( 0 );
		}
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}
	return( key );
}

/************************************************************************/
/*	�E�B���h�E�N���X�o�^												*/
/************************************************************************/

static int	Register( HINSTANCE hinstance ){

	WNDCLASSEX	wc={ 0 };												// �E�B���h�E�N���X�o�^�p�\����

	wc.cbSize		=sizeof( WNDCLASSEX );								// �\���̃T�C�Y( Windows �̃��C�u�������ς���Ă��Ή��ł��邽�߂̔z�� )
	wc.style		=CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;				// �E�B���h�E�N���X�X�^�C��
	wc.lpfnWndProc	=WndProc;											// ���b�Z�[�W���������邽�߂̃E�B���h�E�R�[���o�b�N�֐��̃|�C���^
	wc.cbClsExtra	=0;													// �E�B���h�E�N���X�\���̂̌��Ɋ��蓖�Ă�⑫�o�C�g���������܂��B
	wc.cbWndExtra	=sizeof(void*);										// �E�B���h�E�C���X�^���X�̌��Ɋ��蓖�Ă�⑫�o�C�g���������܂��B
	wc.hInstance	=hinstance;											// ���̃N���X�̂��߂̃E�B���h�E�v���V�[�W��������C���X�^���X�n���h���ł�
	wc.hIcon		=LoadIcon( hinstance, (LPCSTR)IDI_CONTOUR );		// ���̃E�B���h�E�N���X�Ɏg�p����A�C�R���̃n���h�����w�肵�܂��B
	wc.hCursor		=LoadCursor( NULL, IDC_ARROW );						// ���̃E�B���h�E�N���X�Ɏg�p����J�[�\���̃n���h�����w�肵�܂��B
	wc.hbrBackground=( HBRUSH )( COLOR_WINDOW );						// �E�B���h�E�̔w�i���y�C���g���邽�߂̃u���V�̃n���h�����w�肵�܂��B
	wc.lpszMenuName	=( LPCSTR )IDC_CONTOUR;								// ���̃N���X�̃f�t�H���g���j���[�����w�肵�܂��B
	wc.lpszClassName=CRTL;												// �N���X�����w�肵�܂��B
	wc.hIconSm		=LoadIcon( hinstance, (LPCSTR)IDI_CONTOUR );		// ���̃N���X���g���č쐬�����^�C�g���o�[�Ŏg����A�C�R���̃n���h����
	return( RegisterClassEx( &wc ) );									// �E�B���h�E�N���X�̓o�^
}

/************************************************************************/
/*	�E�B���h�E�A�v���P�[�V�����G���g���[								*/
/************************************************************************/

#	define	WS	( WS_OVERLAPPEDWINDOW | WS_VSCROLL )					/* �E�B���h�E�X�^�C��*/

int		main( int argc, char *argv[] ){

	int			errn=0;													// �G���[��
	HINSTANCE	hinstance;
	RECT		rect;													// ��`�\����
	HWND		hwnd;													// �E�B���h�E�n���h��
	int			xw, yw;

	hinstance=GetModuleHandle( 0 );
	if( cmdlinprc( argc, argv ) ){										// �R�}���h���C������
		if( Register( hinstance ) ){									// �E�B���h�E�N���X�o�^
			startup();													// �X�^�[�g�A�b�v
			g_fxw	=g_optfx;											// �t�H���g����
			g_fyw	=g_optfy;											// �t�H���g�c��
			g_hfont	=CreateFont(										// �_���t�H���g���쐬���܂��B
				-g_fyw,													// �t�H���g�̍�����_���P�ʂŎw�肵�܂��B
				g_fxw,													// �t�H���g�̕��ϕ�������_���P�ʂŎw�肵�܂��B0���w�肷��ƓK���ȕ����I������܂�
				0,														// ��������̕����� X ���Ƃ̊p�x���A10 ���� 1 �x�P�ʂŎw�肵�܂��B��������̕����́A�e�L�X�g�̗�̃x�[�X���C���ƕ��s�ł��B
				0,														// �e�����̃x�[�X���C���� X ���Ƃ̊p�x���A10 ���� 1 �x�P�ʂŎw�肵�܂��B
				FW_DONTCARE,											// �t�H���g�̑������A0 ���� 1000 �܂ł̒l�Ŏw�肵�܂��B0 ���w�肷��ƁA�f�t�H���g�̑������I������܂��B
				0,														// �C�^���b�N�̂ɂ��邩�ǂ������w�肵�܂��BTRUE ���w�肷��ƁA�C�^���b�N�̂��I������܂��B
				0,														// �����t���̃t�H���g��I�����邩�ǂ������w�肵�܂��BTRUE ���w�肷��ƁA�����t���t�H���g���I������܂��B
				0,														// �ł��������t���t�H���g��I�����邩�ǂ������w�肵�܂��BTRUE ���w�肷��ƁA�ł��������t���̃t�H���g���I������܂��B
				DEFAULT_CHARSET,										// �t�H���g�̕����Z�b�g���w�肵�܂��B
				OUT_DEFAULT_PRECIS,										// �o�͐��x���w�肵�܂��B���̒l�̂����ꂩ���w�肵�܂��B
				CLIP_DEFAULT_PRECIS,									// �N���b�s���O���x���w�肵�܂��B�N���b�s���O���x�Ƃ́A�����̈ꕔ���N���b�s���O�̈�̊O�ɂ͂ݏo���Ƃ��ɁA���̕������N���b�v������@���`������̂ł��B
				DRAFT_QUALITY,											// �o�͕i�����w�肵�܂��B�o�͕i���Ƃ́AGDI ���A�_���t�H���g�̑����Ǝ��ۂ̕����t�H���g�̑����Ƃ��ǂ̒��x�܂ň�v�����邩���`������̂ł��B
				FIXED_PITCH | FF_DONTCARE,								// �t�H���g�̃s�b�`�ƃt�@�~�����w�肵�܂��B���� 2 �r�b�g�Ńt�H���g�̃s�b�`���w�肵�܂��B( DEFAULT_PITCH, FIXED_PITCH, VARIABLE_PITCH )
				g_optfn													// �t�H���g�̃^�C�v�t�F�C�X���������� NULL �ŏI��镶����ւ̃|�C���^���w�肵�܂��B������̒����́A�I�[�� NULL �������܂߂� 32 �����ȉ��ɂ��Ă��������BNULL �|�C���^��A��̕�����ւ̃|�C���^���w�肷��ƁA�ق��̃p�����[�^�Ŏw�肵�������ɍ����t�H���g�̒�����ŏ��Ɍ������t�H���g���I������܂��B���ݗ��p�\�ȃt�H���g�̃^�C�v�t�F�C�X����񋓂���ɂ́AEnumFontFamilies �֐����g���܂��B
			);															// �߂�l:�֐�����������ƁA�_���t�H���g�̃n���h�����Ԃ�܂��B�֐������s����ƁANULL ���Ԃ�܂��B�g���G���[�����擾����ɂ́AGetLastError �֐����g���܂��B
			assert( g_hfont );
			xw=g_optxn*g_fxw;
			yw=g_optyn*g_fyw;
			SetRect( &rect, 0, 0, xw, yw );								// ��`�\���̂ɒl�Z�b�g
			AdjustWindowRectEx( &rect, WS, 1, 0 );						// �w�肳�ꂽ�N���C�A���g�̈���m�ۂ��邽�߂ɕK�v�ȃE�B���h�E���W���v�Z���܂��B
			g_frmxw=( rect.right - rect.left )-xw;
			g_frmyw=( rect.bottom - rect.top )-yw;
			hwnd=CreateWindow(											// �E�B���h�E�쐬
				CRTL,													// �E�B���h�E�N���X�̖��O���w�肵�܂��B
				CRTL,													// �E�B���h�E�����������ANULL �ŏI��镶����ւ̃|�C���^���w�肵�܂��B
				WS,														// �쐬�����E�B���h�E�̃X�^�C�����w�肵�܂��B
				CW_USEDEFAULT,											// �E�B���h�E�� x ���W���w�肵�܂��B
				CW_USEDEFAULT,											// �E�B���h�E�� y ���W���w�肵�܂��B
				rect.right - rect.left,									// �E�B���h�E�̕����w�肵�܂��B
				rect.bottom - rect.top,									// �E�B���h�E�̍������w�肵�܂��B
				NULL,													// �쐬�����E�B���h�E�̐e�E�B���h�E�܂��̓I�[�i�[�E�B���h�E�̃n���h��
				NULL,													// ���j���[�܂��͎q�E�B���h�E ID ���w�肵�܂��B
				hinstance,												// �E�B���h�E�Ɋ֘A�t����ꂽ���W���[���̃C���X�^���X�n���h�����w�肵�܂��B
				0														// WM_CREATE ���b�Z�[�W�� lParam �p�����[�^�Ƃ��ēn�����ACREATESTRUCT �\���̂ւ̃|�C���^���w�肵�܂��B
			);
			if( hwnd ){													// �E�B���h�E�쐬����
				DragAcceptFiles( g_hwnd, TRUE );						// �h���b�O�h���b�v����
				ShowWindow( hwnd, SW_SHOW );							// �w�肳�ꂽ�E�B���h�E�̕\����Ԃ�ݒ肵�܂�
				UpdateWindow( hwnd );									// �E�B���h�E�̍X�V�̈悪��łȂ��ꍇ�A���̃N���C�A���g�̈���X�V���܂��B
				SetScroll();											// �X�N���[���C���t�H���[�V�����Z�b�g
				xedmain();												// ���C�����[�v
				DragAcceptFiles( g_hwnd, FALSE );						// �h���b�O�h���b�v�֎~
			}else{														// �E�B���h�E�쐬���s
				fprintf( STDERR, "�E�B���h�E�쐬�G���[\n" );
				errn++;													// �G���[�����Z
			}
			DeleteObject( g_hfont );									// �t�H���g�j��
			ctxClose( 0 );												// �R���e�L�X�g�N���[�Y
			cleanup();													// �N���[���i�b�v
		}else{															// �E�B���h�E�N���X�o�^�G���[
			fprintf( STDERR, "�E�B���h�E�N���X�o�^�G���[:%s\n", CRTL );
			errn++;														// �G���[�����Z
		}
	}else{																// �I�v�V�����G���[
		errn++;															// �G���[�����Z
	}
	dbClose( g_ctxdbp );												// �R���e�L�X�gDB�N���[�Y
	return( errn );														// �G���[���������ă��^�[��
}

/************************************************************************/
/*	�E�B���h�E�A�v���P�[�V�����G���g���[								*/
/************************************************************************/

#	ifdef	WINDOWS

int APIENTRY	WinMain( HINSTANCE, HINSTANCE, LPSTR, int ){
	return( main( __argc, __argv ) );									// �G���[���������ă��^�[��
}

#	endif

/************************************************************************/
/*	�R���\�[���A�v���P�[�V�������C���G���g���[							*/
/************************************************************************/

#	else

#		ifndef	CURSES

/************************************************************************/
/*																		*/
/*	unix																*/
/*																		*/
/************************************************************************/

#	define	SELECTMSK	( ExposureMask | KeyPressMask | EnterWindowMask | LeaveWindowMask | StructureNotifyMask | ButtonPressMask )

//	�����炢�̃t�H���g

	static const char	*fntnampaJM[]={									// ���{��
#	ifdef	__CYGWIN__
		"-*-fixed-medium-r-normal--16-*-*-*",
#	else
		"-*-msg16-medium-r-normal-*-16-150-75-75-m-*",
		"-*-fixed-medium-r-normal--16-*",
#	endif
		0
	};
	static const char	*fntnampaUM[]={									// �p��
		"screen16",
		"8x16",
		0
	};

/************************************************************************/
/*	���̓X�^�C����I��													*/
/************************************************************************/

XIMStyle	SelectInputStyle(void){

	static const XIMStyle	preedit[]={	XIMPreeditArea, XIMPreeditNothing, 0 };
	static const XIMStyle	status[]={	XIMStatusArea,  XIMStatusNothing,  0 };
	XIMStyles				*im_styles;
	int						i, j, k;

	XGetIMValues( g_im, XNQueryInputStyle, &im_styles, NULL );
	for( i=0; preedit[i]; i++ ){
		for( j=0; status[j]; j++ ){
			for( k=0; k<im_styles->count_styles; k++ ){
				if( (preedit[i] & im_styles->supported_styles[k] ) &&
					(status[j]	& im_styles->supported_styles[k] ) ){
					return( im_styles->supported_styles[k] );
				}
			}
		}
	}
	fprintf( stderr, "Input Style dos not match\n" );
	return( 0 );
}

/************************************************************************/
/*	���̓R���e�L�X�g�쐬												*/
/************************************************************************/

XIC		CreateInputContext( XIM im, XIMStyle ximstyle, XFontSet fs, Window win ){

	XIC				ic;
	XVaNestedList	list;
	XPoint			spot;

	spot.x=10;
	spot.y=30;
	list=XVaCreateNestedList( 0, XNFontSet, fs, XNSpotLocation, &spot, NULL );
	ic = XCreateIC( im, XNInputStyle,			ximstyle,
						XNClientWindow,			win,
						XNPreeditAttributes,	list,
						XNStatusAttributes,		list,
						NULL );											// �C���v�b�g�R���e�L�X�g�̍쐬
	if( ic == NULL ){
		fprintf( stderr, "Cannot create input context\n" );
	}
	XFree( list );
	return( ic );
}

/************************************************************************/
/*	���͈ʒu�̐ݒ�														*/
/************************************************************************/

#	define	SW	(g_fxw*5)

void	SetGeometry( XIMStyle ximstyle, char *attr ){

	XRectangle		area;
	XVaNestedList	list;

	area.height=g_fyw;
	if( ximstyle == XIMPreeditArea ){
		area.width=g_xw-SW;
	}else{
		if( ximstyle == XIMStatusArea ){
			area.width=SW;
		}
	}
	list=XVaCreateNestedList( 0, XNAreaNeeded, &area, NULL );
	XSetICValues( g_ic, attr, list, NULL );
	XGetICValues( g_ic, attr, list, NULL );
	XFree( list );
	area.y=g_yw-g_fyw;
	if( ximstyle == XIMPreeditArea ){
		area.x=SW;
	}else{
		if( ximstyle == XIMStatusArea ){
			area.x=0;
		}
	}
	list = XVaCreateNestedList( 0, XNArea, &area, NULL );
	XSetICValues( g_ic, attr, list, NULL );
	XFree( list );
}

/************************************************************************/
/*	FEP �I�[�v��														*/
/************************************************************************/

void	fepopen(void){

	int		mask;

	if( g_jp ){															// ���{�ꃂ�[�h
		if( !g_fep ){													// FEP�͂܂��I�[�v������Ă��Ȃ�
			if( !XSetLocaleModifiers( "" ) ){							// ���[�U�w��̃��f�B�t�@�C�A��A������
				fprintf( stderr, "Cannot open input modifires\n" );
				return;
			}
			if( !( g_im=XOpenIM( g_dpyp, 0, 0, 0 ) ) ){					// �C���v�b�g���\�b�h���I�[�v������
				fprintf( stderr, "Cannot open input method\n" );
				return;
			}
			g_style=SelectInputStyle();									// ���̓X�^�C����I��
			g_ic=CreateInputContext( g_im, g_style, g_fs, g_win );		// ���̓R���e�L�X�g�쐬
			XGetICValues( g_ic, XNFilterEvents, &mask, NULL );			// �C���v�b�g���\�b�h���K�v�Ƃ���C�x���g��₢���킹��
			XSelectInput( g_dpyp, g_win, SELECTMSK|mask );				// X�T�[�o����ʒm���Ă��炤�C�x���g���w��
			if( g_style & XIMPreeditArea ){
				SetGeometry( XIMPreeditArea, XNPreeditAttributes );		// ���͈ʒu�̐ݒ�
			}
			if( g_style & XIMStatusArea ){
				SetGeometry( XIMStatusArea, XNStatusAttributes );		// ���͈ʒu�̐ݒ�
			}
			XSetICValues( g_ic, XNFocusWindow, g_win, NULL );
			XSetICFocus( g_ic );
			g_fep=1;
		}
	}
}

/************************************************************************/
/*	(unix)	�L�[�𓾂�													*/
/************************************************************************/

int		getKey(void){

	int				key;												// �L�[
	KeySym			keysym;
	XComposeStatus	cs;
	int				implen;
	XEvent			event;
	int				xedk, xedatr, xedkey;
	unsigned char	keybuf[256];										// �L�[�o�b�t�@
	Status			st;
	stDB			*cbdbp;												// �N���b�v�{�[�hDB
	int				xc, yc, xd, yd;
	int				i;

	for(;;){
		if( !XEventsQueued( g_dpyp, QueuedAlready ) ){					// �C�x���g�͂Ȃ�
			key=getmackey();											// �}�N�������t�L�[�𓾂�
			if( key ){
				break;
			}
		}
		XNextEvent( g_dpyp, &event );									// ���̃C�x���g�𓾂�
		if( XFilterEvent( &event, None ) ){								// FEP �����̏ꍇ
			continue;
		}
		g_tim=event.xbutton.time;
		switch( event.type ){
			case EnterNotify:											// �}�E�X��������
				if( g_fep ){
					XSetICValues( g_ic, XNFocusWindow, g_win, NULL );
					XSetICFocus( g_ic );
				}
				break;
			case LeaveNotify:											// �}�E�X�����ꂽ
				if( g_fep ){
					XUnsetICFocus( g_ic );
				}
				break;
			case MapNotify:												// �}�b�s���O���ꂽ
				break;
			case ConfigureNotify:										// �T�C�Y�ύX��
				xc=event.xconfigure.x;
				yc=event.xconfigure.y;
				xd=event.xconfigure.width;
				yd=event.xconfigure.height;
				if( g_xw!=xd || g_yw!=yd ){
					dspConfig( xd, yd );								// �\���R���t�B�O���[�V����
					invalidateAll();
				}
				if( g_fep ){
					if( g_style & XIMPreeditArea ){
						SetGeometry( XIMPreeditArea, XNPreeditAttributes );		// �\���ʒu�̐ݒ�
					}
					if( g_style & XIMStatusArea ){
						SetGeometry( XIMStatusArea, XNStatusAttributes );		// �\���ʒu�̐ݒ�
					}
				}
				break;
			case Expose:												// �ĕ\���w��
				display( event.xexpose.x, event.xexpose.y, event.xexpose.x+event.xexpose.width, event.xexpose.y+event.xexpose.height );
				break;
			case ButtonPress:											// �}�E�X�{�^���������ꂽ
				break;
			case SelectionNotify:{										// �Z���N�V�����f�[�^���Z�b�g���ꂽ
				cbdbp=xselNotify( g_xselp, &event );					// �Z���N�V�����f�[�^���󂯎��
				if( cbdbp ){
					ctrl_f9calbak( cbdbp );
					dbClose( cbdbp );
				}
				break;
			}
			case SelectionRequest:{										// �Z���N�V�������N�G�X�g( ���M )
				cbdbp=clpbrdGet( CLPBRDP, CB_TXT );						// �N���b�v�{�[�h�Q�b�g
				if( cbdbp ){											// �N���b�v�{�[�h�Q�b�g����
					xselRequest( g_xselp, &event, cbdbp );				// �Z���N�V�������N�G�X�g( ���M )
					dbClose( cbdbp );									// �N���b�v�{�[�hDB�N���[�Y
				}
				break;
			}
			case SelectionClear:										// �Z���N�V�����N���A
				xselClear( g_xselp );									// �Z���N�V�����N���A
				break;
			case KeyPress:												// �L�[�������ꂽ
				xedatr=xedkey=0;
				if( event.xkey.state&(1<<0) ) xedatr|=MKEY_SHIFT;
				if( event.xkey.state&(1<<2) ) xedatr|=MKEY_CTRL;
				if( event.xkey.state&(1<<3) ) xedatr|=MKEY_ALT;
				if( !g_fep ){											// FEP �͎g�p���Ă��Ȃ�
					implen=XLookupString( (XKeyEvent *)&event, (char*)keybuf, sizeof( keybuf ), &keysym, &cs );
					if( implen ){
						xedkey=keybuf[0];
						switch( xedkey ){
							case XK_Insert:		xedkey=MKEY_INSERT;		break;
							case 0x007f:		xedkey=MKEY_DELETE;		break;
						}
						if( xedkey<0x20 || xedkey==MKEY_DELETE ){
							xedk=xedkey|xedatr;
						}else if( xedkey==0x20 && xedatr&MKEY_SHIFT ){
							xedk=xedkey|xedatr;
						}else{
							xedk=xedkey|(xedatr&MKEY_ALT);
						}
						mkeyPut( g_mkeyp, xedk );
						if( xedk==0x1b ){
							msgSet( "[ESC]" );							// ���b�Z�[�W�\��
						}else{
							msgSet( "" );								// ���b�Z�[�W�\��
						}
					}
				}else{													// FEP �g�p��
					implen=XmbLookupString( g_ic, (XKeyPressedEvent *)(&event), (char*)keybuf, sizeof(keybuf), &keysym, &st );	// ���͕�����̎擾
					if( implen ){
						switch( st ){
							case XLookupBoth:
							case XLookupChars:
								for( i=0; i<implen; i++ ){
									xedkey=keybuf[i];
									switch( xedkey ){
										case XK_Insert:	xedkey=MKEY_INSERT;	break;
										case 0x007f:	xedkey=MKEY_DELETE;	break;
									}
									if( xedkey<0x20 || xedkey==MKEY_DELETE ){
										xedkey|=xedatr;
									}else{
										xedkey|=(xedatr&MKEY_ALT);
									}
									mkeyPut( g_mkeyp, xedkey );
									if( xedkey==0x1b ){
										msgSet( "[ESC]" );				// ���b�Z�[�W�\��
									}else{
										msgSet( "" );					// ���b�Z�[�W�\��
									}
								}
								break;
						}
					}
				}
				if( !implen ){
					xedkey=0;
					switch( keysym ){
						case XK_Home:		xedkey=MKEY_HOME;		break;
						case XK_Left:		xedkey=MKEY_LEFT;		break;
						case XK_Up:			xedkey=MKEY_UP;			break;
						case XK_Right:		xedkey=MKEY_RIGHT;		break;
						case XK_Down:		xedkey=MKEY_DOWN;		break;
						case XK_Page_Up:	xedkey=MKEY_PGUP;		break;
						case XK_Page_Down:	xedkey=MKEY_PGDOWN;		break;
						case XK_End:		xedkey=MKEY_END;		break;
						case XK_Insert:		xedkey=MKEY_INSERT;		break;
						case XK_Delete:		xedkey=MKEY_DELETE;		break;
						case XK_F1:			xedkey=MKEY_F1;			break;
						case XK_F2:			xedkey=MKEY_F2;			break;
						case XK_F3:			xedkey=MKEY_F3;			break;
						case XK_F4:			xedkey=MKEY_F4;			break;
						case XK_F5:			xedkey=MKEY_F5;			break;
						case XK_F6:			xedkey=MKEY_F6;			break;
						case XK_F7:			xedkey=MKEY_F7;			break;
						case XK_F8:			xedkey=MKEY_F8;			break;
						case XK_F9:			xedkey=MKEY_F9;			break;
						case XK_F10:		xedkey=MKEY_F10;		break;
						case XK_F11:		xedkey=MKEY_F11;		break;
						case XK_F12:		xedkey=MKEY_F12;		break;
					}
					if( xedkey ){
						xedk=xedatr|xedkey;
						mkeyAlc( g_mkeyp, xedk );
					}
				}
				break;
		}
	}
	return( key );
}

/************************************************************************/
/*	(unix)	X-Window �W�I���g�����𓾂�								*/
/************************************************************************/

static int	xGetGeo( Display *dpyp, int *xwp, int *ywp, int *musxp, int *musyp ){

	Window			rootwin,win,cwin;
	int				xc,yc,musx,musy,lmusx,lmusy;
	unsigned int	xd,yd,bw,zw,muscc;

	rootwin=RootWindow( dpyp, 0 );
	if( !XGetGeometry( dpyp, rootwin, &win, &xc, &yc, &xd, &yd, &bw, &zw ) ){
		printf( "XGetGeometry Error\n" );
		return( 0 );
	}
	if( !XQueryPointer( dpyp, rootwin, &win, &cwin, &musx, &musy, &lmusx, &lmusy, &muscc ) ){
		printf( "XQueryPointer Error\n" );
		return( 0 );
	}
	*xwp=xd;
	*ywp=yd;
	*musxp=musx;
	*musyp=musy;
	return( 1 );
}

/************************************************************************/
/*	(unix)	X-Window Error Handler										*/
/************************************************************************/

static int	mainErrPrc( Display *dpyp ){
	XCloseDisplay( dpyp );
	return( 0 );
}

/************************************************************************/
/*	(unix)	main														*/
/************************************************************************/

int		main( int argc, char *argv[] ){

	int			errn=0;													// �G���[��
	XSizeHints	xsh={ 0 };
	XFontStruct	*fs=0;
	char		*langp;
	int			i, j;
	int			xc, yc, xd, yd;
	int			rootxw, rootyw, musx, musy;
	int			forkid;
	const char	**fntnampa, *namp;
	stXCOL		*xcolp;

	if( cmdlinprc( argc, argv ) ){										// �R�}���h���C������
		startup();														// �X�^�[�g�A�b�v
		forkid=fork();													// �R���\�[�����J�����邽�߂Ƀt�H�[�N����
		if( forkid<0 ){													// �t�H�[�N�G���[
			printf( "fork error" );
			exit( 1 );													// �G���[�I��
		}
		if( forkid ){													// �e
			exit( 0 );													// �I�����ăR���\�[�����J������
		}
		if( !g_optu ){													// �����p��w��ł͂Ȃ�
			langp=getenv( "LANG" );										// ���ϐ��𓾂�
			if( langp ){												// "LANG"�͐ݒ肳��Ă���
				if( !strcmp( langp, "ja_JP.EUC-JP" ) || !strcmp( langp, "ja_JP.EUC" ) || !strcmp( langp, "ja_JP.eucJP" ) || !strcmp( langp, "ja_JP.ujis" ) ){
					g_jp=1;												// ���{�ꃂ�[�h�Z�b�g
				}
			}
		}
		if( g_jp ){														// ���{�ꃂ�[�h
			if( XSupportsLocale()==False ){								// Xlib �����݂̃��P�[���������邩�ǂ����𔻒f����
				fprintf( stderr, "XSupportsLocale ERROR\n" );
				exit( 1 );												// �G���[�I��
			}
		}

//		X-Window �����J�n

		XSetIOErrorHandler( mainErrPrc );								// �G���[�n���h���Z�b�g
		setlocale( LC_ALL, "" );										// ���P�[����ݒ肷��(���݃V�X�e���ɐݒ肳��Ă��郍�P�[�����g�p)
		g_dpyp=XOpenDisplay( 0 );										// X�T�[�o�Ɛڑ�����
		if( !g_dpyp ){													// X�T�[�o�ڑ��G���[
			printf( "XOpenDisplay:ERROR\n" );
			exit( 1 );													// �G���[�I��
		}
		if( !xGetGeo( g_dpyp, &rootxw, &rootyw, &musx, &musy ) ){
			exit( 1 );													// �G���[�I��
		}
		if( g_jp ){														// ���{�ꃂ�[�h
			fntnampa=fntnampaJM;										// �����炢�̃t�H���g
		}else{															// �p�ꃂ�[�h
			fntnampa=fntnampaUM;										// �����ȃt�H���g
		}
		if( g_jp ){														// ���{�ꃂ�[�h

			XFontSetExtents	*fsep;
			char			**miss, *def;
			int				n_miss;

			for( i=0; ( namp=fntnampa[i] )!=0; i++ ){

				g_fs = XCreateFontSet( g_dpyp, namp, &miss, &n_miss, &def );

				if( g_optv ){
					printf( "%s:%d\n", namp, g_fs!=0 );
				}
				if( g_fs ){
					break;
				}
				if( g_optv ){											// �o�[�{�[�Y���[�h
					for( j=0; j<n_miss; j++ ){
						printf( "Font Not Found %s\n", miss[j] );
					}
				}
			}
			if( !g_fs ){
				printf( "All Font Not Found\n" );
				exit( 1 );												// �G���[�I��
			}
			fsep=XExtentsOfFontSet( g_fs );
			g_fxw		= fsep->max_ink_extent.width>>1;				// �t�H���g����
			g_fyw		= fsep->max_ink_extent.height;					// �t�H���g�c��
			g_fntascent	=-fsep->max_ink_extent.y;						// �A�Z���g
		}else{															// �p�ꃂ�[�h
			for( i=0; ( namp=fntnampa[i] )!=0; i++ ){
				fs=XLoadQueryFont( g_dpyp, fntnampa[i] );
				if( fs ){
					break;
				}
				if( g_optv ){											// �o�[�{�[�Y���[�h
					printf( "Font Not Found %s\n", fntnampa[i] );
				}
			}
			if( !fs ){
				printf( "All Font Not Found\n" );
				exit( 1 );												// �G���[�I��
			}
			g_fxw=fs->max_bounds.width;									// �t�H���g����
			g_fyw=fs->ascent+fs->descent;								// �t�H���g�c��
			g_fntascent=fs->ascent;										// �A�Z���g
		}

		xd=g_optxn*g_fxw;
		yd=g_optyn*g_fyw;
		xc=CLAMP( musx, 0, rootxw-xd-16 );
		yc=CLAMP( musy, 0, rootyw-yd-32 );
		xsh.flags=USPosition;
		xsh.x=xc;
		xsh.y=yc;

		g_cmap	=DefaultColormap( g_dpyp, DefaultScreen( g_dpyp ) );
		g_win	=XCreateSimpleWindow( g_dpyp, RootWindow( g_dpyp, 0 ), xc, yc, xd, yd, 0, 0, 0 );
		XSetNormalHints(	g_dpyp, g_win, &xsh );
		XStoreName(			g_dpyp, g_win, CRTL );
		dspConfig( xd, yd );											// �\���R���t�B�O���[�V����
		g_gc=XCreateGC( g_dpyp, g_win, 0, 0 );							// �f�b�쐬
		xcolp=colorref2xcolor( CR_BLACK );								// �q�f�a����w�J���[�ɕϊ�����
		XSetWindowBackground( g_dpyp, g_win, xcolp->col.pixel );		// �w�i�F�ݒ�
		if( !g_jp ){													// �p�ꃂ�[�h
			XSetFont( g_dpyp, g_gc, fs->fid );							// �f�b�Ƀt�H���g���Z�b�g����
		}
		fepopen();														// FEP�I�[�v��
		XSelectInput(	g_dpyp, g_win, SELECTMSK );						// X�T�[�o����ʒm���Ă��炤�C�x���g���w��
		XMapWindow(		g_dpyp, g_win );								// �E�B���h�E����ʂɕ\������(�}�b�v����)�B
//		XMapRaised(		g_dpyp, g_win );								// �E�B���h�E����ʂɕ\������(�}�b�v����)�B
//		XRaiseWindow(	g_dpyp, g_win );								// ���ʂ��Ȃ�
		XFlush(			g_dpyp );										// X�T�[�o�ւ̃��N�G�X�g�o�b�t�@���t���b�V������B
		g_xselp=xselOpen( g_dpyp, g_win );								// �Z���N�V�����Ǘ��I�[�v��
		xedmain();														// ���C�����[�v
		xselClose( g_xselp );											// �Z���N�V�����Ǘ��N���[�Y
		XFreeGC( g_dpyp, g_gc );										// �f�b�̔j��
		XSelectInput(	g_dpyp, g_win, 0 );								// X�T�[�o����ʒm���Ă��炤�C�x���g���w��
		XCloseDisplay(	g_dpyp );
		cleanup();														// �N���[���i�b�v
	}else{																// �I�v�V�����G���[
		errn++;															// �G���[�����Z
	}
	dbClose( g_ctxdbp );												// �R���e�L�X�gDB�N���[�Y
	return( errn );
}

#		else

/************************************************************************/
/*																		*/
/*	curses																*/
/*																		*/
/************************************************************************/
/*	(curses)	�L�[�𓾂�												*/
/************************************************************************/

int		getKey(void){

	int			xedk;

	for(;;){
		xedk=getmackey();												// �}�N�������t�L�[�𓾂�
		if( xedk ){
			break;
		}
		xedk=getch();
		if( xedk<=0 ){
			continue;
		}
		if( xedk==0x1b ){
			msgSet( "[ESC]" );											// ���b�Z�[�W�\��
		}else{
			msgSet( "" );												// ���b�Z�[�W�\��
		}
		switch( xedk ){
			case 0x7f:			xedk=MKEY_DELETE;			break;
			case KEY_BACKSPACE:	xedk=0x0008;				break;		// CTRL+H		�o�b�N�X�y�[�X
			case KEY_ENTER:		xedk=0x0d;					break;
			case KEY_IC:		xedk=MKEY_INSERT;			break;
			case KEY_DC:		xedk=MKEY_DELETE;			break;
			case KEY_LEFT:		xedk=MKEY_LEFT;				break;
			case KEY_UP:		xedk=MKEY_UP;				break;
			case KEY_RIGHT:		xedk=MKEY_RIGHT;			break;
			case KEY_DOWN:		xedk=MKEY_DOWN;				break;
			case KEY_HOME:		xedk=MKEY_HOME;				break;
			case KEY_END:		xedk=MKEY_END;				break;
			case KEY_NPAGE:		xedk=MKEY_PGDOWN;			break;
			case KEY_PPAGE:		xedk=MKEY_PGUP;				break;
			case KEY_F(1):		xedk=MKEY_F1;				break;
			case KEY_F(2):		xedk=MKEY_F2;				break;
			case KEY_F(3):		xedk=MKEY_F3;				break;
			case KEY_F(4):		xedk=MKEY_F4;				break;
			case KEY_F(5):		xedk=MKEY_F5;				break;
			case KEY_F(6):		xedk=MKEY_F6;				break;
			case KEY_F(7):		xedk=MKEY_F7;				break;
			case KEY_F(8):		xedk=MKEY_F8;				break;
			case KEY_F(9):		xedk=MKEY_F9;				break;
			case KEY_F(10):		xedk=MKEY_F10;				break;
			case KEY_F(11):		xedk=MKEY_F11;				break;
			case KEY_F(12):		xedk=MKEY_F12;				break;
			case KEY_F(13):		xedk=MKEY_F3 | MKEY_SHIFT;	break;
			case KEY_F(14):		xedk=MKEY_F4 | MKEY_SHIFT;	break;
			case KEY_F(15):		xedk=MKEY_F5 | MKEY_SHIFT;	break;
			case KEY_F(16):		xedk=MKEY_F6 | MKEY_SHIFT;	break;
			case KEY_F(17):		xedk=MKEY_F7 | MKEY_SHIFT;	break;
			case KEY_F(18):		xedk=MKEY_F8 | MKEY_SHIFT;	break;
			case KEY_F(19):		xedk=MKEY_F9 | MKEY_SHIFT;	break;
			case KEY_F(20):		xedk=MKEY_F10| MKEY_SHIFT;	break;
			case KEY_F(21):		xedk=MKEY_F11| MKEY_SHIFT;	break;
			case KEY_F(22):		xedk=MKEY_F12| MKEY_SHIFT;	break;
		}
		mkeyPut( g_mkeyp, xedk );
	}
	return( xedk );
}

/************************************************************************/
/*	�V�O�i���G���[														*/
/************************************************************************/

static void sigint( int signo ){
	mkeyAlc( g_mkeyp, 3 );
}

/************************************************************************/
/*	CTRL+Z																*/
/************************************************************************/

static void sigctrlz( int signo ){
	mkeyAlc( g_mkeyp, 0x1a );
}

/************************************************************************/
/*	CTRL+\																*/
/************************************************************************/

static void sigquit( int signo ){
	mkeyAlc( g_mkeyp, 0x1c );
}

/************************************************************************/
/*	CURSES ���C���v���O����												*/
/************************************************************************/

int		main( int argc, char *argv[] ){

	int			errn=0;													// �G���[��
	int			yy, xx;													// �E�B���h�E�T�C�Y

	if( cmdlinprc( argc, argv ) ){										// �R�}���h���C������
		setlocale( LC_ALL, "" );										// ���P�[����ݒ肷��(���݃V�X�e���ɐݒ肳��Ă��郍�P�[�����g�p)
		clpbrdStartUp();												// �N���b�v�{�[�h�X�^�[�g�A�b�v
		startup();														// xed �̃X�^�[�g�A�b�v
		signal( SIGINT, sigint );										// �V�O�i���Z�b�g
		signal( SIGTSTP, sigctrlz );									// �V�O�i���Z�b�g
		signal( SIGQUIT, sigquit );										// �V�O�i���Z�b�g
		initscr();														// �J�[�Z�X������
		timeout( 1 );													// 1ms �u���b�N���ǂݍ��݂��s���A�Ȃ����͂������Ƃ��͐���ɓ��삵�܂���
		nonl();															// ���s�����^�[���ƃ��C���t�B�[�h�ɕϊ����܂���D 
		raw();
		leaveok( stdscr, FALSE );										// �E�B���h�E�̍X�V��ɁA�J�[�\�������݂̍��W�ɔz�u����悤�w�����܂�
		noecho();														// �L�[���^�C�v���Ă��C�X�N���[���ɃG�R�[���Ȃ����[�h�ɂ��܂��D 
		cbreak();														// �L�[�̓��͂�Raw Mode�ɂ��܂��D���̃��[�h�ł́C�L�[���͂̓o�b�t�@���ꂸ�C�����Ƀv���O�����ɓn����܂� 
		keypad( stdscr, TRUE );											// bf��TRUE���w�肵���ꍇ�Cwgetch���ɂ����āC�t�@���N�V�����L�[���������Ƃ��Ƀt�@���N�V�����L�[�̃R�[�h��Ԃ��悤�ɂȂ�܂��D
		getmaxyx( stdscr, yy, xx );										// �w�肳�ꂽ�E�B���h�E�̃T�C�Y���擾���܂��D
		g_fxw=8;														// �t�H���g�����Z�b�g
		g_fyw=16;														// �t�H���g�c���Z�b�g
		dspConfig( xx*g_fxw, yy*g_fyw );								// �\���R���t�B�O���[�V����
		invalidateAll();												// �S�čĕ`��
		xedmain();														// xed ���C�����[�v
		endwin();														// �J�[�Z�X�I��
		cleanup();														// xed �̃N���[���i�b�v
		clpbrdCleanUp();												// �N���b�v�{�[�h�N���[���i�b�v
	}else{																// �I�v�V�����G���[
		errn++;															// �G���[�����Z
	}
	return( errn );														// �v���O�����I��
}

#		endif

#	endif

