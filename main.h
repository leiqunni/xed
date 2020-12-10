/************************************************************************/
/*																		*/
/*	X-Window & Windows テキストエディタ	by H.Watanabe					*/
/*	Ver3.00	2009/04/04 13:49:05	作成開始								*/
/*																		*/
/************************************************************************/

#	ifndef	MAIN_H
#	define	MAIN_H

#	ifdef	_WIN32
#	include	<windows.h>													/* Windows ヘッダーファイル*/
#	else
#		ifndef	CURSES
#			include <X11/Xlib.h>
#			include	"std/xsel.h"										/* セレクション管理*/
#		else
#			define	_XOPEN_SOURCE_EXTENDED	1
#			include	<curses.h>
#		endif
#	endif
#	include	<sys/stat.h>												/* ファイルステータス*/
#	include	"std/db2.h"													/* プログラム内汎用データベースシステム*/
#	include	"std/lm.h"													/* ラインマップ*/
#	include	"std/mkey.h"												/* マルチキー管理*/

/************************************************************************/
/*	ローカル定義														*/
/************************************************************************/

#	define	CRTL			"xed Ver2.00"								/* クリエートツール名*/
#	define	SKPSP(P)		((P)+strspn(P,"\t "))						/* スペース、タブのスキップ*/
#	define	CLAMP(X,L,H)	(((X)>(H))?(H):(((X)<(L))?(L):(X)))			/* クランプ*/
#	define	RNGTSTB(C,L,H)	((unsigned char)((C)-(L))<=(unsigned char)((H)-(L)))				/* 符号無し８ビットでレンジチェックをする*/

#	ifdef	_WIN32
#		define	CLPBRDP		g_hwnd										/* クリップボードに必要な情報*/
#		define	XColor		int
#	else
#		define	RGB(R,G,B)	((R<<16)+(G<<8)+(B))
#		ifndef	CURSES
#			define	CLPBRDP	g_dpyp										/* クリップボードに必要な情報*/
#		else
#			define	CLPBRDP	0											/* クリップボードに必要な情報*/
#			define	XColor	int
#		endif
#	endif

	typedef struct{
		XColor			col;											// XColor
	}	stXCOL;															// XColor 構造体

#	define	YN_CANCEL		0											/* キャンセル*/
#	define	YN_NO			1											/* ノー*/
#	define	YN_YES			2											/* イエス*/

#	define	CR_BLACK		RGB(0x00,0x00,0x00)							/* 黒*/
#	define	CR_WHITE		RGB(0xe0,0xe0,0xe0)							/* 白*/
#	define	CR_LB			RGB(0x88,0xa0,0xb8)							/* ライトブルー*/
#	define	CR_LNO			RGB(0xa0,0xa0,0xa0)							/* 行番号*/
#	define	CR_LNO_BK		RGB(0x28,0x28,0x28)							/* 行番号背景色*/
#	define	CR_MSG_BK		RGB(0x88,0xa0,0xbc)							/* メッセージ背景色*/
#	define	CR_MOD_BK		RGB(0x60,0x98,0xc0)							/* モード背景色*/
#	define	CR_OVER			RGB(0xd0,0x30,0x30)							/* 上書きカーソル色*/

#	define	CRTYP_WIN		0											/* 0d0a*/
#	define	CRTYP_UNIX		1											/* 0a*/

#	if	defined(_WIN32) || defined(__CYGWIN__)							/* WINDOWS*/
#		define	CRTYP_DEFAULT	CRTYP_WIN								/* デフォルト改行タイプ*/
#	else
#		define	CRTYP_DEFAULT	CRTYP_UNIX								/* デフォルト改行タイプ*/
#	endif

	typedef struct{
		int					chgflg;										// 変更フラグ
	}	stLIN;															// 行構造体

	typedef struct{
		stDB				*lindbp;									// 行DB
		stLIN				*pgp;										// ページ先頭
		int					pgxx, pgyy;									// ページ座標
		stLIN				*curp;										// 編集中の行
		int					curxx, curyy;								// カーソル座標
		int					chgflg;										// 変更フラグ
		int					cuttyp;										// カットタイプ( 0:行, 1:矩形 )
		stLIN				*cutpgp;									// カット先頭ページポインタ
		stLIN				*cutcurp;									// カット先頭ポインタ
		int					cutpgxx, cutpgyy;							// カット先頭ページ座標
		int					cutcurxx, cutcuryy;							// カット先頭座標
		int					crtyp;										// 改行タイプ
		int					statflg;									// １になっていれば st 有効
		struct stat			st;											// ファイルステータス
	}	stCTX;															// コンテキスト構造体

#	ifdef	MAIN_CPP
#		define	EXTERN
#	else
#		define	EXTERN	extern
#	endif

#	ifdef	_WIN32

	EXTERN	HWND			g_hwnd;										// メインウィンドウハンドル
	EXTERN	HFONT			g_hfont;									// フォントハンドル
	EXTERN	HDC				g_hdc;										// デバイスコンテキスト

#	else

#		ifndef	CURSES

	EXTERN	Display			*g_dpyp;									// ディスプレイ
	EXTERN	Window			g_win;										// ウィンドウ
	EXTERN	Colormap		g_cmap;										// カラーマップ
	EXTERN	GC				g_gc;										// ＧＣ
	EXTERN	XFontSet		g_fs;										// 日本語モードフォント
	EXTERN	XIM				g_im;
	EXTERN	XIMStyle		g_style;
	EXTERN	XIC				g_ic;
	EXTERN	int				g_fep;										// FEPオープンフラグ
	EXTERN	unsigned long	g_tim;
	EXTERN	int				g_fntascent;								// フォントアセント
	EXTERN	int				g_jp;										// 日本語モードフラグ
	EXTERN	stXSEL			*g_xselp;									// セレクション管理

#		endif

#	endif

	EXTERN	int				g_optv;										// バーボーズモード( うるさいモード )
	EXTERN	int				g_optz;										// デバッグモード
	EXTERN	int				g_optn;										// *.bak ファイルを作成しない
	EXTERN	int				g_optl;										// [EOL]を表示しない
	EXTERN	int				g_opte;										// [EOF]を表示しない
	EXTERN	char			g_optfn[256];								// フォント名
	EXTERN	int				g_optfx, g_optfy;							// フォントサイズ
	EXTERN	int				g_optxn, g_optyn;							// 画面横幅、縦幅( キャラクタ座標 )
	EXTERN	int				g_optlno;									// 行番号指定
	EXTERN	int				g_optu;										// 強制英語モード

	EXTERN	int				g_frmxw, g_frmyw;							// ウィンドウフレームサイズ
	EXTERN	int				g_fxw, g_fyw;								// フォント横幅、縦幅
	EXTERN	int				g_xw, g_yw;									// ウィンドウ横幅、縦幅
	EXTERN	int				g_xxw, g_yyw;								// ウィンドウ横幅、縦幅( キャラクタ座標系 )
	EXTERN	int				g_txxc, g_tyyc, g_txxd, g_tyyd;				// テキスト座標( キャラクタ座標系 )
	EXTERN	stDB			*g_ctxdbp;									// コンテキストDB
	EXTERN	stCTX			*g_ctxp;									// コンテキストポインタ
	EXTERN	stMKEY			*g_mkeyp;									// マルチキー管理構造体ポインタ
	EXTERN	int				g_flgend;									// 終了フラグ
	EXTERN	int				g_tabstp;									// タブステップ
	EXTERN	int				g_tabdsp;									// タブ表示フラグ
	EXTERN	int				g_lnosiz;									// 行番号表示サイズ
	EXTERN	int				g_cursw;									// カーソル表示スイッチ
	EXTERN	int				g_flgeof;									// [EOF]表示フラグ
	EXTERN	int				g_flgcr;									// キャリッジリターン表示フラグ
	EXTERN	int				g_flgins;									// インサートフラグ
	EXTERN	int				g_flgl2c;									// 小文字から大文字変換フラグ
	EXTERN	char			g_msgstr[256];								// メッセージ文字列
	EXTERN	char			g_cutlin[LM_MAX+4];							// 一行カットバッファ
	EXTERN	int				g_flglbl;									// 検索、置換時のラベル認識
	EXTERN	int				g_flgh;										// 行番号１６進表示
	EXTERN	int				g_flgwld;									// 検索、置換時のワイルドカード有効フラグ
	EXTERN	int				g_flgc2l;									// 検索時の大文字小文字区別フラグ
	EXTERN	char			g_reporg[256], g_repnew[256];				// 置換文字列
	EXTERN	int				g_scdbiz;									// 検索表示サイズ
	EXTERN	stDB			*g_macalcdbp;								// キーマクロ登録DB
	EXTERN	stDB			*g_macrsvdbp;								// キーマクロ予約DB
	EXTERN	stDB			*g_macexedbp;								// キーマクロ実行DB

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

