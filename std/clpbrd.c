/************************************************************************/
/*																		*/
/*	クリップボード管理			by H.Watanabe							*/
/*	Ver1.00	2009/04/05 13:52:04	作成									*/
/*																		*/
/************************************************************************/

#	ifdef	_WIN32
#	pragma	warning(disable:4115)
#	include	<windows.h>													/* Windows ヘッダーファイル*/
#	endif
#	include	<stdlib.h>													/* 標準ライブラリ関数*/
#	include	<string.h>													/* 標準文字列関数*/
#	include	"clpbrd.h"													/* クリップボード管理*/

#	ifndef	CURSES

/************************************************************************/
/*	ローカル定義														*/
/************************************************************************/

#	ifdef	_WIN32
#		define	CF_SEARCH	(CF_PRIVATEFIRST+2)
#	else
#		define	CF_TEXT		1
#		define	CF_SEARCH	2
#	endif

/************************************************************************/
/*	論理番号からクリップボード番号に変換								*/
/************************************************************************/

static int	cb2cf( int cbno ){

	int		cfno;

	switch( cbno ){
		case CB_SEARCH:													// 検索
			cfno=CF_SEARCH;
			break;
		default:
			cfno=CF_TEXT;
			break;
	}
	return( cfno );
}

/************************************************************************/
/*	１行読み込み														*/
/************************************************************************/

static char	*getlin( char *dstp, const char *txtp ){

	const char	*nxtp;													// 次のテキストポインタ

	nxtp=strpbrk( txtp, "\r\n\x1a" );									// 文字列を走査し、指定された文字セットに含まれる文字を見つけます
	if( !nxtp ){														// 見つからない
		nxtp=txtp+strlen( txtp );										// 文字列終端にシーク
	}
	memcpy( dstp, txtp, nxtp-txtp );									// 文字データコピー
	dstp[nxtp-txtp]=0;													// ０ターミネート
	if( *nxtp=='\r' ) nxtp++;											// 0x0d ならばスキップ
	if( *nxtp=='\n' ) nxtp++;											// 0x0a ならばスキップ
	if( *nxtp==0x1a ) nxtp++;											// 0x1a ならばスキップ
	return( (char*)nxtp );												// 帰り値は次のテキストポインタ
}

/************************************************************************/
/*	クリップボードゲット												*/
/************************************************************************/

stDB	*clpbrdGet( tpCLPBRD hwnd, int cbno ){

#	ifdef	_WIN32

	stDB	*dbp=0;
	int		clpbufsiz;
	HANDLE	clph;
	char	*clpbufp;
	char	linbuf[0x10000];
	int		cfno;														// クリップボード番号

	cfno=cb2cf( cbno );													// 論理番号からクリップボード番号に変換
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
	int		cfno;														// クリップボード番号

	cfno=cb2cf( cbno );													// 論理番号からクリップボード番号に変換
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
/*	クリップボードプット												*/
/************************************************************************/

int		clpbrdPut( tpCLPBRD hwnd, int cbno, const stDB *dbp, int rctmod, int clrflg ){

#	ifdef	_WIN32

	int		res=0;
	char	*p;
	int		clpbufsiz=1;												// 最後のNULのため
	HANDLE	clph;
	char	*clpbufp;
	int		cfno;														// クリップボード番号

	cfno=cb2cf( cbno );													// 論理番号からクリップボード番号に変換
	dbFOR( dbp, p, char* ){
		clpbufsiz+=strlen( p )+2;
	}
	clph=GlobalAlloc( GMEM_MOVEABLE, clpbufsiz );						// グローバルメモリ確保
	if( clph ){
		clpbufp=GlobalLock( clph );										// グローバルメモリロック
		dbFOR( dbp, p, char* ){
			strcpy( clpbufp, p );
			clpbufp+=strlen( clpbufp );
			if( dbNxt( p ) || !rctmod ){
				strcpy( clpbufp, "\r\n" );
				clpbufp+=strlen( clpbufp );
			}
		}
		GlobalUnlock( clph );											// グローバルメモリアンロック
		if( OpenClipboard( hwnd ) ){									// クリップボードオープン
			if( clrflg ){
				EmptyClipboard();
			}
			SetClipboardData( cfno, clph );								//	1:クリップボードデータ設定
			CloseClipboard();											// クリップボードクローズ
			res=1;
		}
	}
	return( res );

#	else

	int		res=0;
	char	*p;
	int		clpbufsiz=1;												// 最後のNULのため
	char	*clpbufadr;
	char	*clpbufp;
	int		cfno;														// クリップボード番号

	cfno=cb2cf( cbno );													// 論理番号からクリップボード番号に変換
	dbFOR( dbp, p, char* ){
		clpbufsiz+=strlen( p )+1;
	}
	clpbufadr=(char*)malloc( clpbufsiz );								// グローバルメモリ確保
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
/*	ローカル定義														*/
/************************************************************************/

	stDB	*g_cbdbpa[2];

/************************************************************************/
/*	スタートアップ														*/
/************************************************************************/

void	clpbrdStartUp(void){
	g_cbdbpa[0]=dbOpen( 0, DBSIZ_STR, 0, 0x10000 );
	g_cbdbpa[1]=dbOpen( 0, DBSIZ_STR, 0, 0x10000 );
}

/************************************************************************/
/*	クリーンナップ														*/
/************************************************************************/

void	clpbrdCleanUp(void){
	dbClose( g_cbdbpa[0] );
	dbClose( g_cbdbpa[1] );
}

/************************************************************************/
/*	クリップボードゲット												*/
/************************************************************************/

stDB	*clpbrdGet( tpCLPBRD hwnd, int cbno ){
	return( dbDup( g_cbdbpa[cbno] ) );
}

/************************************************************************/
/*	クリップボードプット												*/
/************************************************************************/

int		clpbrdPut( tpCLPBRD hwnd, int cbno, const stDB *dbp, int rctmod, int clrflg ){

	(void)&rctmod, (void)&clrflg;
	dbClear( g_cbdbpa[cbno] );
	dbAddDB( g_cbdbpa[cbno], dbp );										// 追加
	return( 1 );
}

#	endif

