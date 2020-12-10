/************************************************************************/
/*																		*/
/*	セレクション管理			by H.Watanabe							*/
/*	Ver1.00	2009/04/05 13:52:04	作成									*/
/*																		*/
/************************************************************************/

#	include	<stdio.h>													/* 標準入出力関数*/
#	include	<stdlib.h>													/* 標準ライブラリ関数*/
#	include	<string.h>													/* 標準文字列関数*/
#	include	<time.h>													/* 時刻関数*/

#	ifndef	CURSES

#	ifdef	unix

#	include	<X11/Xlocale.h>
#	include	<X11/Xresource.h>
#	include	<X11/Xutil.h>
#	include	<X11/Xatom.h>
#	include	"readdb.h"													/* クリップボード等からの行変換*/
#	include	"xsel.h"													/* セレクション管理*/

/************************************************************************/
/*	ローカル定義														*/
/************************************************************************/

	struct	tgXSEL{
		Display				*dpyp;
		Window				win;
		int					select;
		Atom				a_targets, a_ct, a_prop;
	};

/************************************************************************/
/*	セレクションオープン												*/
/************************************************************************/

stXSEL	*xselOpen( Display *dpyp, Window win ){

	stXSEL	*xselp;														// 管理構造体

	xselp=(stXSEL*)malloc( sizeof(stXSEL) );							// 管理構造体メモリ確保
	memset( xselp, 0, sizeof(stXSEL) );									// 管理構造体クリア
	xselp->dpyp=dpyp;
	xselp->win=win;
	xselp->a_targets = XInternAtom( dpyp, "TARGETS", 		False );
	xselp->a_ct		 = XInternAtom( dpyp, "COMPOUND_TEXT",	False );
	xselp->a_prop	 = XInternAtom( dpyp, "__COPY_TEXT",	False );
	return( xselp );													// 管理構造体を持ってリターン
}

/************************************************************************/
/*	セレクションクローズ												*/
/************************************************************************/

void	xselClose( stXSEL *xselp ){
	if( xselp ){
		free( xselp );													// 管理構造体メモリ開放
	}
}

/************************************************************************/
/*	セレクションオーナー宣言											*/
/************************************************************************/

int		xselOwner( stXSEL *xselp, unsigned long tim2 ){
	XSetSelectionOwner( xselp->dpyp, XA_PRIMARY, xselp->win, tim2 );
	xselp->select=( XGetSelectionOwner( xselp->dpyp, XA_PRIMARY ) == xselp->win );
	return( xselp->select );
}

/************************************************************************/
/*	セレクションクリア													*/
/************************************************************************/

void	xselClear( stXSEL *xselp ){
	xselp->select=0;
}

/************************************************************************/
/*	セレクション要求													*/
/************************************************************************/

void	xselDemand( stXSEL *xselp, unsigned long tim2 ){
	XConvertSelection( xselp->dpyp, XA_PRIMARY, xselp->a_ct, xselp->a_prop, xselp->win, tim2 );
}

/************************************************************************/
/*	セレクションリクエスト( 送信 )										*/
/************************************************************************/

void	xselRequest( stXSEL *xselp, const XEvent *eventp, const stDB *cbdbp ){

	XSelectionEvent se={ 0 };
	char			*tmpalc, *tmpp;
	int				tmpsiz=1;
	char			*cbp;

	se.type			= SelectionNotify;
	se.requestor	= eventp->xselectionrequest.requestor;
	se.selection	= eventp->xselectionrequest.selection;
	se.target		= eventp->xselectionrequest.target;
	se.time			= eventp->xselectionrequest.time;
	se.property		= eventp->xselectionrequest.property;
	if( eventp->xselectionrequest.selection == XA_PRIMARY ){
		dbFOR( cbdbp, cbp, char* ){
			tmpsiz+=strlen( cbp )+1;
		}
		tmpalc=tmpp=(char*)malloc( tmpsiz );
		dbFOR( cbdbp, cbp, char* ){
			strcpy( tmpp, cbp );
			tmpp+=strlen( tmpp );
			if( dbNxt( cbp ) ){
				*tmpp++='\n';
			}
		}
		*tmpp=0;
		XChangeProperty( xselp->dpyp, se.requestor, se.property, se.target, 8, PropModeReplace, (unsigned char*)tmpalc, tmpsiz );
		free( tmpalc );
	}
	XSendEvent( xselp->dpyp, se.requestor, False, 0, (XEvent*)&se );
}

/************************************************************************/
/*	セレクションデータを受け取る										*/
/************************************************************************/

stDB	*xselNotify( stXSEL *xselp, const XEvent *eventp ){

	stDB			*dbp=0;
	Atom			type;
	unsigned char	*bufp=0;
	int				fmt;
	unsigned long	nitems, left;
	int				err;
	int				offset=0;
	stREADDB		*rhp;

	if( eventp->xselection.selection == XA_PRIMARY && eventp->xselection.property == xselp->a_prop ){
		rhp=readdbOpen();												// リードDBオープン
		for(;;){
			err=XGetWindowProperty( xselp->dpyp, xselp->win, xselp->a_prop, offset, 4096/4, False, xselp->a_ct, &type, &fmt, &nitems, &left, &bufp );
			if( err!=Success ){
				break;
			}
			readdbAlc( rhp, bufp, nitems );								// リードDB登録
			XFree( bufp );
			offset+=nitems/4;
			if( !left ){
				break;
			}
		}
		dbp=readdbClose( rhp );											// リードDBクローズ
	}
	return( dbp );
}

#	endif

#	endif

