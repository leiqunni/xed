/************************************************************************/
/*																		*/
/*	�N���b�v�{�[�h�Ǘ�			by H.Watanabe							*/
/*	Ver1.00	2009/04/05 13:52:04	�쐬									*/
/*																		*/
/************************************************************************/

#	ifdef	_WIN32
#	pragma	warning(disable:4115)
#	include	<windows.h>													/* Windows �w�b�_�[�t�@�C��*/
#	endif
#	include	<stdlib.h>													/* �W�����C�u�����֐�*/
#	include	<string.h>													/* �W��������֐�*/
#	include	"clpbrd.h"													/* �N���b�v�{�[�h�Ǘ�*/

#	ifndef	CURSES

/************************************************************************/
/*	���[�J����`														*/
/************************************************************************/

#	ifdef	_WIN32
#		define	CF_SEARCH	(CF_PRIVATEFIRST+2)
#	else
#		define	CF_TEXT		1
#		define	CF_SEARCH	2
#	endif

/************************************************************************/
/*	�_���ԍ�����N���b�v�{�[�h�ԍ��ɕϊ�								*/
/************************************************************************/

static int	cb2cf( int cbno ){

	int		cfno;

	switch( cbno ){
		case CB_SEARCH:													// ����
			cfno=CF_SEARCH;
			break;
		default:
			cfno=CF_TEXT;
			break;
	}
	return( cfno );
}

/************************************************************************/
/*	�P�s�ǂݍ���														*/
/************************************************************************/

static char	*getlin( char *dstp, const char *txtp ){

	const char	*nxtp;													// ���̃e�L�X�g�|�C���^

	nxtp=strpbrk( txtp, "\r\n\x1a" );									// ������𑖍����A�w�肳�ꂽ�����Z�b�g�Ɋ܂܂�镶���������܂�
	if( !nxtp ){														// ������Ȃ�
		nxtp=txtp+strlen( txtp );										// ������I�[�ɃV�[�N
	}
	memcpy( dstp, txtp, nxtp-txtp );									// �����f�[�^�R�s�[
	dstp[nxtp-txtp]=0;													// �O�^�[�~�l�[�g
	if( *nxtp=='\r' ) nxtp++;											// 0x0d �Ȃ�΃X�L�b�v
	if( *nxtp=='\n' ) nxtp++;											// 0x0a �Ȃ�΃X�L�b�v
	if( *nxtp==0x1a ) nxtp++;											// 0x1a �Ȃ�΃X�L�b�v
	return( (char*)nxtp );												// �A��l�͎��̃e�L�X�g�|�C���^
}

/************************************************************************/
/*	�N���b�v�{�[�h�Q�b�g												*/
/************************************************************************/

stDB	*clpbrdGet( tpCLPBRD hwnd, int cbno ){

#	ifdef	_WIN32

	stDB	*dbp=0;
	int		clpbufsiz;
	HANDLE	clph;
	char	*clpbufp;
	char	linbuf[0x10000];
	int		cfno;														// �N���b�v�{�[�h�ԍ�

	cfno=cb2cf( cbno );													// �_���ԍ�����N���b�v�{�[�h�ԍ��ɕϊ�
	if( OpenClipboard( hwnd ) ){
		dbp=dbOpen( 0, DBSIZ_STR, 0, 0x4000 );
		clph=GetClipboardData( cfno );
		if( clph ){
			clpbufsiz=GlobalSize( clph );
			if( clpbufsiz ){
				clpbufp=GlobalLock( clph );
				while( *clpbufp ){
					clpbufp=getlin( linbuf, clpbufp );
					dbAlc( dbp, linbuf );
				}
				GlobalUnlock( clph );
			}
		}
		CloseClipboard();
	}
	return( dbp );

#	else

	stDB	*dbp=0;
	int		clpbufsiz;
	char	*clpbufp, *p;
	char	linbuf[0x10000];
	int		cfno;														// �N���b�v�{�[�h�ԍ�

	cfno=cb2cf( cbno );													// �_���ԍ�����N���b�v�{�[�h�ԍ��ɕϊ�
	clpbufp=XFetchBuffer( hwnd, &clpbufsiz, cfno );
	if( clpbufp ){
		dbp=dbOpen( 0, DBSIZ_STR, 0, 0x4000 );
		p=clpbufp;
		while( *p ){
			p=getlin( linbuf, p );
			dbAlc( dbp, linbuf );
		}
		XFree( clpbufp );
	}
	return( dbp );

#	endif
}

/************************************************************************/
/*	�N���b�v�{�[�h�v�b�g												*/
/************************************************************************/

int		clpbrdPut( tpCLPBRD hwnd, int cbno, const stDB *dbp, int rctmod, int clrflg ){

#	ifdef	_WIN32

	int		res=0;
	char	*p;
	int		clpbufsiz=1;												// �Ō��NUL�̂���
	HANDLE	clph;
	char	*clpbufp;
	int		cfno;														// �N���b�v�{�[�h�ԍ�

	cfno=cb2cf( cbno );													// �_���ԍ�����N���b�v�{�[�h�ԍ��ɕϊ�
	dbFOR( dbp, p, char* ){
		clpbufsiz+=strlen( p )+2;
	}
	clph=GlobalAlloc( GMEM_MOVEABLE, clpbufsiz );						// �O���[�o���������m��
	if( clph ){
		clpbufp=GlobalLock( clph );										// �O���[�o�����������b�N
		dbFOR( dbp, p, char* ){
			strcpy( clpbufp, p );
			clpbufp+=strlen( clpbufp );
			if( dbNxt( p ) || !rctmod ){
				strcpy( clpbufp, "\r\n" );
				clpbufp+=strlen( clpbufp );
			}
		}
		GlobalUnlock( clph );											// �O���[�o���������A�����b�N
		if( OpenClipboard( hwnd ) ){									// �N���b�v�{�[�h�I�[�v��
			if( clrflg ){
				EmptyClipboard();
			}
			SetClipboardData( cfno, clph );								//	1:�N���b�v�{�[�h�f�[�^�ݒ�
			CloseClipboard();											// �N���b�v�{�[�h�N���[�Y
			res=1;
		}
	}
	return( res );

#	else

	int		res=0;
	char	*p;
	int		clpbufsiz=1;												// �Ō��NUL�̂���
	char	*clpbufadr;
	char	*clpbufp;
	int		cfno;														// �N���b�v�{�[�h�ԍ�

	cfno=cb2cf( cbno );													// �_���ԍ�����N���b�v�{�[�h�ԍ��ɕϊ�
	dbFOR( dbp, p, char* ){
		clpbufsiz+=strlen( p )+1;
	}
	clpbufadr=(char*)malloc( clpbufsiz );								// �O���[�o���������m��
	if( clpbufadr ){
		clpbufp=clpbufadr;
		dbFOR( dbp, p, char* ){
			strcpy( clpbufp, p );
			clpbufp+=strlen( clpbufp );
			if( dbNxt( p ) || !rctmod ){
				strcpy( clpbufp, "\n" );
				clpbufp+=strlen( clpbufp );
			}
		}
		XStoreBuffer( hwnd, clpbufadr, clpbufsiz, cfno );
		free( clpbufadr );
		res=1;
	}
	return( res );

#	endif
}

#	else

/************************************************************************/
/*	���[�J����`														*/
/************************************************************************/

	stDB	*g_cbdbpa[2];

/************************************************************************/
/*	�X�^�[�g�A�b�v														*/
/************************************************************************/

void	clpbrdStartUp(void){
	g_cbdbpa[0]=dbOpen( 0, DBSIZ_STR, 0, 0x10000 );
	g_cbdbpa[1]=dbOpen( 0, DBSIZ_STR, 0, 0x10000 );
}

/************************************************************************/
/*	�N���[���i�b�v														*/
/************************************************************************/

void	clpbrdCleanUp(void){
	dbClose( g_cbdbpa[0] );
	dbClose( g_cbdbpa[1] );
}

/************************************************************************/
/*	�N���b�v�{�[�h�Q�b�g												*/
/************************************************************************/

stDB	*clpbrdGet( tpCLPBRD hwnd, int cbno ){
	return( dbDup( g_cbdbpa[cbno] ) );
}

/************************************************************************/
/*	�N���b�v�{�[�h�v�b�g												*/
/************************************************************************/

int		clpbrdPut( tpCLPBRD hwnd, int cbno, const stDB *dbp, int rctmod, int clrflg ){

	(void)&rctmod, (void)&clrflg;
	dbClear( g_cbdbpa[cbno] );
	dbAddDB( g_cbdbpa[cbno], dbp );										// �ǉ�
	return( 1 );
}

#	endif

