/************************************************************************/
/*																		*/
/*	表示						by H.Watanabe							*/
/*	Ver1.00	2009/04/04 13:49:05	作成開始								*/
/*																		*/
/************************************************************************/

#	include	<assert.h>													/* アサーション*/
#	include	<stdio.h>													/* 標準入出力関数*/
#	include	<string.h>													/* 標準文字列関数*/
#	include	"std/txtdl.h"												/* テキストエディタのためのディスプレイリスト変換*/
#	include	"std/clp3.h"												/* クリッピング*/
#	include	"main.h"													/* メインインクルードファイル*/

/************************************************************************/
/*	ローカル定義														*/
/************************************************************************/

#	define	MOD_XXC			0											/* モード*/
#	define	MOD_YYC			0
#	define	MOD_XXD			25
#	define	MOD_YYD			1

#	define	MSG_XXC			25											/* メッセージ*/
#	define	MSG_YYC			0
#	define	MSG_XXD			23
#	define	MSG_YYD			1

#	define	FIL_XXC			48											/* ファイル名*/
#	define	FIL_YYC			0
#	define	FIL_XXD			23
#	define	FIL_YYD			1

	static stDB				*g_xcoldbp;									// XColor 変換DB

/************************************************************************/
/*	ディスプレイスタートアップ											*/
/************************************************************************/

void	displayStartUp(void){
	g_xcoldbp=dbOpen( sizeof(unsigned long), sizeof(stXCOL), 256, 4096 );		// XColor 変換DBオープン
}

/************************************************************************/
/*	ディスプレイクリーンナップ											*/
/************************************************************************/

void	displayCleanUp(void){
	dbClose( g_xcoldbp );												// XColor 変換DBクローズ
}

/************************************************************************/
/*	ＲＧＢからＸカラーに変換する										*/
/************************************************************************/

#	ifndef	_WIN32
#		ifndef	CURSES

stXCOL	*colorref2xcolor( unsigned long colorref ){

	stXCOL	*xcolp;														// XColor 構造体
	char	tmpa[256];													// テンポラリバッファ

	xcolp=(stXCOL*)dbSch( g_xcoldbp, &colorref );						// 検索
	if( !xcolp ){														// 未登録
		xcolp=(stXCOL*)dbAlc( g_xcoldbp, &colorref, 0 );				// 登録
		sprintf( tmpa, "#%06x", (int)colorref );						// 文字列化
		XParseColor( g_dpyp, g_cmap, tmpa, &xcolp->col );				// カラー解析
		XAllocColor( g_dpyp, g_cmap, &xcolp->col );						// カラー登録
	}
	return( xcolp );
}

#		endif
#	endif

/************************************************************************/
/*	ライン描画															*/
/************************************************************************/

static void	lineOnTxt( int x1, int y1, int x2, int y2, unsigned long colorref ){

#	ifdef	_WIN32

	HPEN	hpen;														// ペンハンドル

	if( clpLin( &x1, &y1, &x2, &y2, x1, y1, x2, y2, 0, 0, g_txxd*g_fxw, g_tyyd*g_fyw ) ){
		hpen=CreatePen( PS_SOLID, 0, colorref );						// ペン作成
		SelectObject(	g_hdc, hpen );									// ペンセレクト
		MoveToEx(		g_hdc, x1+g_txxc*g_fxw, y1+g_tyyc*g_fyw, 0 );	// 始点指定
		LineTo(			g_hdc, x2+g_txxc*g_fxw, y2+g_tyyc*g_fyw );		// ライン描画
		DeleteObject(	hpen );											// ペン破棄
	}

#	else
#		ifndef	CURSES

	stXCOL	*xcolp;

	if( clpLin( &x1, &y1, &x2, &y2, x1, y1, x2, y2, 0, 0, g_txxd*g_fxw, g_tyyd*g_fyw ) ){
		xcolp=colorref2xcolor( colorref );								// ＲＧＢからＸカラーに変換する
		XSetForeground(	g_dpyp, g_gc, xcolp->col.pixel );				// 前景色セット
		XDrawLine( g_dpyp, g_win, g_gc, x1+g_txxc*g_fxw, y1+g_tyyc*g_fyw, x2+g_txxc*g_fxw, y2+g_tyyc*g_fyw );
	}
#		else
	(void)&x1, (void)&y1, (void)&x2, (void)&y2, (void)&colorref;
#		endif
#	endif
}

/************************************************************************/
/*	ボックス描画														*/
/************************************************************************/

static void	boxOnTxt( int x1, int y1, int x2, int y2, unsigned long colorref ){
	lineOnTxt( x1, y1, x2, y1, colorref );
	lineOnTxt( x2, y1, x2, y2, colorref );
	lineOnTxt( x2, y2, x1, y2, colorref );
	lineOnTxt( x1, y2, x1, y1, colorref );
}

/************************************************************************/
/*	矩形選択表示														*/
/************************************************************************/

static void	displaySelect(void){

	int		xx1, yy1, xx2, yy2;											// キャラクタ座標

	if( CUTCURP && CUTTYP ){											// 矩形選択モード
		xx1=CUTCURXX-PGXX;												// 左
		yy1=CUTPGYY+CUTCURYY-PGYY;										// 上
		xx2=CURXX-PGXX;													// 右
		yy2=CURYY;														// 下
		if( xx1<=xx2 && yy1<=yy2 ){
			boxOnTxt( xx1*g_fxw, yy1*g_fyw, xx2*g_fxw, (yy2+1)*g_fyw, CR_LB );
		}
	}
}

/************************************************************************/
/*	テキスト描画														*/
/************************************************************************/

void	drwTxt( int xx, int yy, const char *txtp, int txtsiz, unsigned long bakcol, unsigned long forcol ){

#	ifdef	_WIN32

	int		x, y;														// ピクセル座標系
	RECT	rect={ 0 };

	x=xx*g_fxw;															// ピクセル座標系Ｘ
	y=yy*g_fyw;															// ピクセル座標系Ｙ
	rect.left=x;														// 左
	rect.right=x+txtsiz*g_fxw;											// 右
	rect.top=y;															// 上
	rect.bottom=y+g_fyw;												// 下

	SetBkColor( g_hdc, bakcol );										// バックグラウンドカラー
	SetTextColor( g_hdc, forcol );										// フォアグラウンドカラー
	TextOut( g_hdc, x, y, txtp, txtsiz );								// 文字列描画

#	else
#		ifndef	CURSES

	stXCOL	*xcolp;														// カラー構造体ポインタ
	int		x, y;														// ピクセル座標系

	x=xx*g_fxw;															// ピクセル座標系Ｘ
	y=yy*g_fyw+g_fntascent;												// ピクセル座標系Ｙ
	xcolp=colorref2xcolor( bakcol );									// ＲＧＢからＸカラーに変換する
	XSetBackground( g_dpyp, g_gc, xcolp->col.pixel );					// 背景色セット
	xcolp=colorref2xcolor( forcol );									// ＲＧＢからＸカラーに変換する
	XSetForeground(	g_dpyp, g_gc, xcolp->col.pixel );					// 前景色セット
	if( g_jp ){															// 日本語モード
		XmbDrawImageString( g_dpyp, g_win, g_fs, g_gc, x, y, txtp, txtsiz );
	}else{																// 英語モード
		XDrawImageString( g_dpyp, g_win, g_gc, x, y, txtp, txtsiz );
	}
#		else

	int			bakval, forval, rvs;
	char		tmpa[4096];												// テンポラリバッファ

	memcpy( tmpa, txtp, txtsiz );
	tmpa[txtsiz]=0;														// 0ターミネート
	bakval=(bakcol&0xff);												// バックグラウンドカラー
	forval=(forcol&0xff);												// フォアグラウンドカラー
	rvs=( forval<bakval );												// 反転判定
	if( rvs ){															// 反転
		attrset( A_REVERSE );											// 反転
	}else{
		attrset( A_NORMAL );											// ノーマル
	}
	move( yy, xx );														// カーソルを移動する
	addstr( tmpa );														// 表示

#		endif
#	endif

}

/************************************************************************/
/*	フィル																*/
/************************************************************************/

void	drwFil( int xx1, int yy1, int xx2, int yy2, unsigned long col ){

#	ifdef	_WIN32

	int		x1, y1, x2, y2;												// ピクセル座標系
	RECT	rect;														// 矩形構造体
	HBRUSH	hbrush;														// ブラシ

	x1=xx1*g_fxw;														// 左
	y1=yy1*g_fyw;														// 上
	x2=xx2*g_fxw;														// 右
	y2=yy2*g_fyw;														// 下
	SetRect( &rect, x1, y1, x2, y2 );									// 矩形構造体セット
	hbrush=CreateSolidBrush( col );										// ブラシ作成
	FillRect( g_hdc, &rect, hbrush );									// 矩形フィル
	DeleteObject( hbrush );												// ブラシ破棄

#	else
#		ifndef	CURSES

	int		x1, y1, x2, y2;												// ピクセル座標系
	stXCOL	*xcolp;														// カラー構造体ポインタ

	x1=xx1*g_fxw;														// 左
	y1=yy1*g_fyw;														// 上
	x2=xx2*g_fxw;														// 右
	y2=yy2*g_fyw;														// 下
	xcolp=colorref2xcolor( col );										// ＲＧＢからＸカラーに変換する
	XSetForeground(	g_dpyp, g_gc, xcolp->col.pixel );					// 前景色セット
	XFillRectangle( g_dpyp, g_win, g_gc, x1, y1, x2-x1, y2-y1 );		// 矩形フィル

#		else

	static const char	spa[]="                                                                                                                                             ";
	int					yy;												// Yキャラクタ座標
	int					rvs;											// 反転フラグ

	rvs=(col&0xff)>=0x80;												// 反転判定
	for( yy=yy1; yy<yy2; yy++ ){
		drwTxt( xx1, yy, spa, xx2-xx1, rvs, !rvs );						// テキスト描画
	}

#		endif
#	endif

}

/************************************************************************/
/*	インフォメーション表示												*/
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

//	モード

	sprintf( tmpa, "[%5d:%5d] %-4s %-5s ", PGYY+CURYY+1, CURXX+1, overinsa[g_flgins!=0], largesmalla[g_flgl2c!=0] );
	drwTxt( MOD_XXC, MOD_YYC, tmpa, strlen( tmpa ), CR_MOD_BK, CR_BLACK );		// テキスト描画

//	メッセージ

	sprintf( tmpa, "%-*s", MSG_XXD, g_msgstr );
	drwTxt( MSG_XXC, MSG_YYC, tmpa, strlen( tmpa ), CR_MSG_BK, CR_BLACK );		// テキスト描画

//	ファイル

	xxw=g_xxw-FIL_XXC-2;
	if( xxw>0 ){
		sprintf( tmpa, "%c %-*s", FILCHG?'*':' ', xxw, FNAMP );
		drwTxt( FIL_XXC, FIL_YYC, tmpa, strlen( tmpa ), CR_MOD_BK, CR_BLACK );	// テキスト描画
	}
}

/************************************************************************/
/*	行番号表示															*/
/************************************************************************/

#	define	LNOXX		0
#	define	LNOYY		1

static void	displayLno( int x1, int y1, int x2, int y2 ){

	static const unsigned long	cola[2]={
		CR_LNO_BK,														// 背景色
		CR_LNO,															// 前景色
	};
	int				yy1, yy2;											// キャラクタ座標
	stLIN			*linp;												// 行ポインタ
	int				seekn;												// シークできた数
	int				yy;													// キャラクタ座標
	char			tmpa[256];											// テンポラリバッファ

	(void)&x1, (void)&x2;
	if( g_lnosiz ){														// 行番号表示有効
		yy1=y1/g_fyw;													// 上
		yy2=(y2+g_fyw-1)/g_fyw;											// 下
		linp=(stLIN*)dbSeek( &seekn, PGP, yy1 );						// 指定したオフセット分シークする
		if( seekn!=yy1 ){												// シークできなかった
			linp=0;														// 行ポインタは無効
		}
		for( yy=yy1; yy<yy2; yy++ ){
			if( linp ){													// 行が存在する
				sprintf( tmpa, "%*d ", g_lnosiz-1, PGYY+yy+1 );
				drwTxt( LNOXX, (LNOYY+yy), tmpa, g_lnosiz, cola[linp->chgflg], cola[!linp->chgflg] );			// テキスト描画
				linp=(stLIN*)dbNxt( linp );								// 行ポインタシーク
			}else{														// 行は存在しない
				drwFil( LNOXX, LNOYY+yy, LNOXX+g_lnosiz, LNOYY+yy+1, cola[0] );
			}
		}
	}
}

/************************************************************************/
/*	漢字ノーマライズ													*/
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
/*	テキスト表示														*/
/************************************************************************/

#	define	TXTXX		g_lnosiz
#	define	TXTYY		1

static void	displayTxt( int x1, int y1, int x2, int y2 ){

	static const unsigned long	cola[2]={
		CR_WHITE,														// ホワイト
		CR_LB,															// ライトブルー
	};
	int				xx1, yy1, xx2, yy2;									// キャラクタ座標
	stLIN			*linp;												// 行ポインタ
	stDB			*dldbp;												// ディスプレイリストDB
	stTXTDL			*dlp;												// ディスプレイリストポインタ
	int				seekn;												// シークできた数
	unsigned long	bakforcola[3]={ CR_BLACK, CR_WHITE };				// 背景色、前面色
	int				curxx;												// カーソルＸ座標
	int				xx, yy;												// キャラクタ座標
	stLM			lm;													// 行マップ
	int				eof;												// 終端行フラグ
	int				eofdsp;												// 終端表示フラグ
	int				cutrvs;												// 行単位カットモード反転
	int				rvs;												// 反転フラグ
	int				lno;												// 行番号
	int				curyy=-1;											// カーソルライン表示のためのワーク
	char			*keyadr;											// 描画文字列アドレス
	int				keysiz;												// 描画文字列サイズ

	xx1=x1/g_fxw;														// 左
	yy1=y1/g_fyw;														// 上
	xx2=(x2+g_fxw-1)/g_fxw;												// 右
	yy2=(y2+g_fyw-1)/g_fyw;												// 下
	linp=(stLIN*)dbSeek( &seekn, PGP, yy1 );							// 指定したオフセット分シークする
	if( seekn!=yy1 ){													// シークできなかった
		linp=0;															// 行ポインタを無効にする
	}
	for( yy=yy1; yy<yy2; yy++ ){
		curxx=-1;														// カーソル行ではない
		xx=0;															// Ｘ座標クリア
		if( linp ){														// 行が存在する
			if( yy==CURYY && g_cursw ){									// カーソル行
				lmMak( &lm, (char*)dbKEYADR( linp ), g_tabstp, 0 );		// 行マップ作成
				curxx=lmPosAdj( &lm, CURXX, 0 );						// カーソル位置正規化
			}
			if( dbNxt( linp ) ){										// 終端行ではない
				eof=0;													// [EOF]表示
				eofdsp=g_flgcr;											// 終端表示フラグは改行表示フラグ
			}else{														// 終端行
				eof=1;													// [.]表示
				eofdsp=g_flgeof;										// 終端表示フラグは[EOF]表示フラグ
			}
			cutrvs=0;													// 行単位カットモード反転フラグクリア
			if( CUTCURP && !CUTTYP ){									// 行選択モード
				lno=PGYY+yy;											// 処理中の行番号
				if( lno>=CUTPGYY+CUTCURYY && lno<PGYY+CURYY ){			// 範囲指定中に含まれる
					cutrvs=1;											// 行単位カットモード反転フラグセット
				}
			}
			dldbp=txtdl( (char*)dbKEYADR( linp ), curxx, g_tabstp, g_tabdsp, eof, eofdsp, PGXX, g_scdbiz, xx2-xx1 );
			dbFOR( dldbp, dlp, stTXTDL* ){								// ディスプレイリスト総当り
				if( clp1( 0, 0, xx-PGXX, xx-PGXX+dlp->dspsiz, xx1, xx2 ) ){		// 表示エリアに触れている
#	ifdef	CURSES
					rvs=cutrvs;											// カーソルはCURSESが出す
#	else
					rvs=dlp->rvs^cutrvs;								// 反転フラグ計算
#	endif
					bakforcola[1]=cola[dlp->col];						// テキストカラーセット
					if( dlp->cur && !g_flgins ){						// 上書きモード
						bakforcola[1]=CR_OVER;							// テキストカラーセット
					}
					keyadr=(char*)dbKEYADR( dlp );						// キーアドレス
					keysiz=dbKEYSIZ( dlp );								// キーサイズ
#	ifdef	CURSES
					if( keysiz>xx2-xx+PGXX-1 ){							// CURSES が改行しないように、通常より1文字分少なくクランプする
						keysiz=xx2-xx+PGXX-1;
					}
					keysiz=kannrm( keyadr, keysiz );					// 漢字ノーマライズ
#	endif
					if( keysiz ){										// キーバッファは有効
						drwTxt( (TXTXX+xx-PGXX), (TXTYY+yy), keyadr, keysiz, bakforcola[rvs], bakforcola[!rvs] );				// テキスト描画
					}else{												// キーバッファは無効
						drwFil( (TXTXX+xx-PGXX), (TXTYY+yy), (TXTXX+xx-PGXX)+dlp->dspsiz, (TXTYY+yy)+1, bakforcola[rvs] );
					}
					if( xx==PGXX-1 ){									// 左側にはみ出ている
						displayLno( 0, (yy)*g_fyw, g_lnosiz*g_fxw, (yy+1)*g_fyw );				// 行番号再描画
					}
				}
				xx+=dlp->dspsiz;										// 座標更新
			}
			dbClose( dldbp );											// ディスプレイリストDBクローズ
			linp=(stLIN*)dbNxt( linp );									// 行ポインタシーク
		}
		if( xx<PGXX ){
			xx=PGXX;
		}
		if( xx-PGXX<xx2 ){												// 空きエリアが存在する
			drwFil( TXTXX+xx-PGXX, TXTYY+yy, TXTXX+xx2, TXTYY+yy+1, CR_BLACK );
		}
		if( curxx>=0 ){													// カーソル行
			curyy=yy;													// カーソルライン表示のためのワークセット
		}
	}
	if( curyy>=0 ){														// カーソル行が存在する
		lineOnTxt( 0, curyy*g_fyw+g_fyw, g_txxd*g_fxw, curyy*g_fyw+g_fyw, CR_LB );				// カーソル行ライン表示
	}
	displaySelect();													// 矩形選択表示
}

/************************************************************************/
/*	キャラクタボックス表示												*/
/************************************************************************/

void	displayChrBox( int xxc, int yyc, int xxd, int yyd, const char *msgp ){

	int		yy;															// Ｙ座標
	char	tmpa[256];													// テンポラリバッファ

	for( yy=0; yy<yyd; yy++ ){
		if( yy==0 || yy==yyd-1 ){										// 先頭又は終端
			sprintf( tmpa, " %-*s", xxd-1, msgp );
			drwTxt( xxc, yyc+yy, tmpa, strlen( tmpa ), CR_LB, CR_BLACK );		// テキスト描画
			msgp="";
		}else{
			drwTxt( xxc,       yyc+yy, tmpa, 1, CR_LB, CR_BLACK );		// テキスト描画
			drwTxt( xxc+xxd-1, yyc+yy, tmpa, 1, CR_LB, CR_BLACK );		// テキスト描画
		}
	}
}

/************************************************************************/
/*	表示																*/
/************************************************************************/

void	display( int x1, int y1, int x2, int y2 ){

	int				cxc, cyc, cxd, cyd;									// クリップされた座標

	if( clpBox( &cxc, &cyc, &cxd, &cyd, x1, y1, x1+x2, y1+y2, 0, 0, g_xw, g_fyw ) ){
		displayInf();													// インフォメーション表示
	}
	if( clpBox( &cxc, &cyc, &cxd, &cyd, x1, y1, x2-x1, y2-y1, 0, g_tyyc*g_fyw, g_lnosiz*g_fxw, g_tyyd*g_fyw ) ){
		displayLno( cxc, cyc-g_tyyc*g_fyw, cxc+cxd, cyc+cyd );			// 行番号表示
	}
	if( clpBox( &cxc, &cyc, &cxd, &cyd, x1, y1, x2-x1, y2-y1, g_txxc*g_fxw, g_tyyc*g_fyw, g_txxd*g_fxw, g_tyyd*g_fyw ) ){
		displayTxt( cxc-g_txxc*g_fxw, cyc-g_tyyc*g_fyw, cxc+cxd-g_txxc*g_fxw, cyc+cyd-g_tyyc*g_fyw );			// テキスト表示
	}
	helpReDsp();														// ヘルプ再表示
	linedtReDsp();														// 一行編集再表示
#	ifdef	CURSES
	{
		stLM			lm;												// 行マップ
		int				curxx;											// カーソルＸ座標
		int				xx;

		lmMak( &lm, (char*)dbKEYADR( CURP), g_tabstp, 0 );				// 行マップ作成
		curxx=lmPosAdj( &lm, CURXX, 0 );								// カーソル位置正規化
		xx=curxx-PGXX;
		if( xx<0 ){
			xx=0;
		}
		move( TXTYY+CURYY, TXTXX+xx );									// カーソル移動
		refresh();
	}
#	endif
}

/************************************************************************/
/*	再描画リクエスト( グローバル )										*/
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
/*	再描画リクエスト( テキスト )										*/
/************************************************************************/

void	invalidateTxt( int xxc, int yyc, int xxd, int yyd ){
	invalidate( g_txxc+xxc, g_tyyc+yyc, xxd, yyd );						// 再描画リクエスト
}

/************************************************************************/
/*	再描画リクエスト( 全て )											*/
/************************************************************************/

void	invalidateAll(void){
	invalidate( 0, 0, g_xxw, g_yyw );									// 再描画リクエスト
#	ifdef	_WIN32
	SetScroll();
#	endif
}

/************************************************************************/
/*	再描画リクエスト( モード )											*/
/************************************************************************/

void	invalidateMod(void){
	invalidate( MOD_XXC, 0, MOD_XXD, 1 );								// 再描画リクエスト
}

/************************************************************************/
/*	再描画リクエスト( メッセージ )										*/
/************************************************************************/

void	invalidateMsg(void){
	invalidate( MSG_XXC, 0, MSG_XXD, 1 );								// 再描画リクエスト
}

/************************************************************************/
/*	再描画リクエスト( ファイル )										*/
/************************************************************************/

void	invalidateFil(void){
	invalidate( FIL_XXC, 0, g_xxw-FIL_XXC, 1 );							// 再描画リクエスト
}

/************************************************************************/
/*	再描画リクエスト( インフォメーション )								*/
/************************************************************************/

void	invalidateInf(void){
	invalidate( 0, 0, g_xxw, 1 );										// 再描画リクエスト
}

/************************************************************************/
/*	再描画リクエスト( 行 )												*/
/************************************************************************/

void	invalidateLin( int yy ){
	invalidateTxt( 0, yy, g_xxw, 1 );
}

/************************************************************************/
/*	再描画リクエスト( 行番号 )											*/
/************************************************************************/

void	invalidateLno( int yy ){
	invalidate( 0, g_tyyc+yy, g_lnosiz, 1 );							// 再描画リクエスト
}

/************************************************************************/
/*	再描画リクエスト( ヘルプ )											*/
/************************************************************************/

void	invalidateHlp(void){
	invalidate( 0, 0, 0, 0 );											// 再描画リクエスト
}

/************************************************************************/
/*	再描画リクエスト( 一行編集 )										*/
/************************************************************************/

void	invalidateLinedt(void){
	invalidate( 0, 0, 0, 0 );											// 再描画リクエスト
}

/************************************************************************/
/*	カーソル表示														*/
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
/*	表示コンフィグレーション											*/
/************************************************************************/

void	dspConfig( int xw, int yw ){

	g_xw	=xw;														// クライアントエリア横幅
	g_yw	=yw;														// クライアントエリア縦幅
	g_xxw	=(g_xw+g_fxw-1)/g_fxw;
	g_yyw	=(g_yw+g_fyw-1)/g_fyw;
	g_txxc	=g_lnosiz;													// 行番号表示サイズ
	g_tyyc	=1;
	g_txxd	=g_xxw-g_lnosiz;
	g_tyyd	=g_yyw-1;
	if( CURYY>=g_tyyd ){												// はみ出ている
		CURYY=g_tyyd-1;													// 最終行へ
		CURP=(stLIN*)dbSeek( 0, PGP, CURYY );							// 次のページ先頭までシークしてみる
	}
}

/************************************************************************/
/*	メッセージセット													*/
/************************************************************************/

void	msgSet( const char *strp ){
	if( strcmp( g_msgstr, strp ) ){										// 違う
		strcpy( g_msgstr, strp );										// バッファにコピー
		invalidateMsg();
	}
}

/************************************************************************/
/*	Y/N 選択															*/
/************************************************************************/

int		YorN( const char *msgstr ){

	int		res=YN_CANCEL;												// リザルト
	int		keysym;														// キーシンボル

	msgSet( msgstr );													// メッセージセット
	for(;;){
		keysym=getKey();												// キーを得る
		if( keysym==MKEY_ESC || keysym==0x03 ){							// キャンセル
			break;
		}
		if( keysym=='n' || keysym=='N' ){								// ノー
			res=YN_NO;													// リザルトをノーにセット
			break;
		}
		if( keysym=='y' || keysym=='Y' ){								// イエス
			res=YN_YES;													// リザルトをイエスにセット
			break;
		}
	}
	msgSet( "" );														// メッセージクリア
	return( res );														// リザルトを持ってリターン
}

