/************************************************************************/
/*																		*/
/*	X-Window & Windows テキストエディタ	by H.Watanabe					*/
/*	Ver2.00	2009/04/04 13:49:05	作成開始								*/
/*																		*/
/************************************************************************/

#	define	MAIN_CPP
#	include	<assert.h>													/* アサーション*/
#	include	<stdio.h>													/* 標準入出力関数*/
#	include	<stdlib.h>													/* 標準ライブラリ関数*/
#	include	<string.h>													/* 標準文字列関数*/
#	include	"std/about2.h"												/* バージョン情報ボックス用メッセージハンドラ*/
#	include	"std/opt.h"													/* オプション処理*/
#	include	"std/filedb.h"												/* データベースファイル登録*/
#	include	"std/clpbrd.h"												/* クリップボード管理*/
#	include	"resource.h"												/* リソースファイル*/
#	include	"main.h"													/* メインインクルードファイル*/
#	ifndef	_WIN32
#		include	<unistd.h>												/* UNIX スタンダード*/
#		include	<locale.h>												/* ローカライゼーション*/
#		ifndef	CURSES
#			include	<X11/keysym.h>
#			include	<X11/Xatom.h>
#		else
#			include	<signal.h>											/* signal処理*/
#		endif
#	endif

/************************************************************************/
/*	スタートアップ														*/
/************************************************************************/

static void	startup(void){
	g_mkeyp=mkeyOpen();													// マルチキー管理オープン
	displayStartUp();													// ディスプレイスタートアップ
}

/************************************************************************/
/*	クリーンナップ														*/
/************************************************************************/

static void	cleanup(void){
	mkeyClose( g_mkeyp );												// マルチキー管理クローズ
	dbClose( g_macalcdbp );												// キーマクロ登録DB
	dbClose( g_macrsvdbp );												// キーマクロ予約DB
	dbClose( g_macexedbp );												// キーマクロ実行DB
	displayCleanUp();													// ディスプレイクリーンナップ
}

/************************************************************************/
/*	コマンドライン処理													*/
/************************************************************************/

static int	cmdlinprc( int argc, char *argv[] ){

	int			errn=0;
	cOPT		opt;
	stDB		*argdbp;
	stDB		*filedbp;												// コマンドラインファイルDB
	stFILE		*filep;													// ファイル構造体ポインタ

//	ワーク初期化

	g_optxn=96;															// キャラクタ横個数
	g_optyn=56;															// キャラクタ縦個数
	g_tabstp=4;															// タブステップ
	g_lnosiz=5;															// 行番号表示サイズ
	g_flgins=1;															// インサートフラグ
	g_flgl2c=1;															// 小文字から大文字変換フラグ
	strcpy( g_optfn, "ＭＳ ゴシック" );									// フォント名
	g_optfx=8;															// フォントサイズ横
	g_optfy=16;															// フォントサイズ縦

//	コマンドライン処理

	opt.SetTitle( "X-Window & Windows Text Editor " );					// コマンドタイトル
	opt.SetVer(   "3.00" );												// バージョン
	opt.SetLine(  "<ファイル>..." );									// ライン情報
	opt.AllocInc( "v", &g_optv, "Verbose Mode" );						// インクリメントオプション登録
	opt.AllocInc( "z", &g_optz, "Debug Mode" );							// インクリメントオプション登録
	opt.AllocInc( "n", &g_optn, "*.bak ファイルを作成しない" );			// インクリメントオプション登録
	opt.AllocInt( "h", &g_tabstp, "TAB Step( default=4 )" );			// イントオプション登録
#	ifndef	_WIN32
	opt.AllocInc( "u", &g_optu, "English Mode" );						// インクリメントオプション登録
#	endif
	opt.AllocInc( "l", &g_optl, "Display [EOL]" );						// インクリメントオプション登録
	opt.AllocInc( "e", &g_opte, "Display [EOF]" );						// インクリメントオプション登録
	opt.AllocInt( "fx", &g_optfx, "FONT SIZE X" );						// イントオプション登録
	opt.AllocInt( "fy", &g_optfy, "FONT SIZE Y" );						// イントオプション登録
	opt.AllocInt( "xn", &g_optxn, "char of X" );						// イントオプション登録
	opt.AllocInt( "yn", &g_optyn, "char of Y" );						// イントオプション登録
	opt.AllocInt( "lno", &g_optlno, "行番号" );							// イントオプション登録

	argdbp=opt.PrcArg( argc, argv );									// オプション、引数処理
	if( argdbp ){
		filedbp=filedbAlcDB( argdbp, FILEATR_ALL );
		if( !dbAlcN( filedbp ) ){
			filedbp=filedbAlc( filedbp, "noname", FILEATR_REG|FILEATR_NON );	// ファイル名登録
		}
		g_flgcr=!g_optl;												// 改行文字非表示
		g_flgeof=!g_opte;												// [EOF]非表示
		g_ctxdbp=dbOpen( DBSIZ_STR, sizeof(stCTX), 256, 4096 );			// コンテキストDBオープン
		dbFOR( filedbp, filep, stFILE* ){								// ファイル総当り
			ctxOpen( (char*)dbKEYADR( filep ) );						// コンテキストオープン
		}
		g_ctxp	=(stCTX*)dbTop( g_ctxdbp );								// カレントコンテキストポインタセット
		dbClose( filedbp );												// ファイルDBクローズ
	}else{
		opt.Usage( argv[0] );											// 使用法表示
		errn++;
	}
	return( !errn );
}

/************************************************************************/
/*	マクロ処理付キーを得る												*/
/************************************************************************/

int		getmackey(void){

	int		*keyp, key;													// キーポインタ、キー

	key=mkeyGet( g_mkeyp );												// キーを得る
	if( key ){															// キーポインタは有効
		if( dbAlcN( g_macexedbp )==0 ){									// マクロ中ではない
			if( g_macalcdbp ){											// マクロ登録中
				if( dbAlcN( g_macalcdbp )<4096 ){						// マクロは長すぎない
					dbAlc( g_macalcdbp, &key );							// マクロ登録
				}else{													// マクロが長すぎる
					msgSet( "Macro Too Long" );
				}
			}
			return( key );												// キーを持ってリターン
		}
		if( key==0x1b ){												// ESCマクロ中断
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
/*	スクロールインフォメーションセット									*/
/************************************************************************/

void	SetScroll(void){

	SCROLLINFO	si={ 0 };												// スクロールインフォメーション構造体

	si.cbSize	=sizeof(si);											// 構造体サイズ
	si.fMask	=SIF_PAGE | SIF_RANGE | SIF_POS;						// 有効フラグ
	si.nMin		=0;														// スクロール最小値
	si.nMax		=dbAlcN( LINDBP );										// スクロール最大値
	si.nPage	=g_tyyd;												// ページサイズ
	si.nPos		=PGYY;													// スクロール位置
	SetScrollInfo( g_hwnd, SB_VERT, &si, TRUE );						// スクロールインフォメーションセット
}

/************************************************************************/
/*	スクロール処理														*/
/************************************************************************/

static void	setscroll( int sb, int thumb ){

	int		yyw;
	int		yymax;
	int		yy;
	int		sgn;														// 符号

	yyw		=g_tyyd;
	yymax	=dbAlcN( LINDBP )-yyw;
	if( yymax<0 ){
		yymax=0;
	}
	yy		=PGYY;
	switch( sb ){
		case SB_LINEUP:			yy -=1;		break;						// ライン減算
		case SB_LINEDOWN:		yy +=1; 	break;						// ライン加算
		case SB_PAGEUP:			yy -=yyw;	break;						// ページ減算
		case SB_PAGEDOWN:		yy +=yyw; 	break;						// ページ加算
		case SB_THUMBPOSITION:	yy =thumb;	break;						// 指定ライン
		case SB_THUMBTRACK:		yy =thumb;	break;						// 指定ライン( トラッキング中 )
		case WM_MOUSEWHEEL:{											// マウスホイール
			sgn=1-( thumb<0 )*2;										// 符号セット( -1 or 1 )
			yy-=((thumb*sgn)>>5)*sgn;
			break;
		}
	}
	yy	=CLAMP( yy, 0, yymax );
	if( yy!=PGYY ){
		pgscroll( yy-PGYY );
		SetScroll();													// スクロールインフォメーションセット
		InvalidateRect( g_hwnd, NULL, FALSE );							// ウィンドウの更新領域に、長方形を追加します。更新領域とは、ウィンドウのクライアント領域のうち、再描画しなければならない部分のことです。
	}
}

/************************************************************************/
/*	フォント選択ダイアログ												*/
/************************************************************************/

void	FontDiaog(void){

	LOGFONT		lf={ 0 };
	CHOOSEFONT	cf={ 0 };
	HDC			hdc;
	HFONT		hfont;
	int			fxw, fyw;

	GetObject( g_hfont, sizeof(lf), &lf );								// LOGFONT 情報を取得
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
		hfont	=CreateFont(											// 論理フォントを作成します。
			-fyw,														// フォントの高さを論理単位で指定します。
			fxw,														// フォントの平均文字幅を論理単位で指定します。0を指定すると適当な幅が選択されます
			0,															// 文字送りの方向と X 軸との角度を、10 分の 1 度単位で指定します。文字送りの方向は、テキストの列のベースラインと平行です。
			0,															// 各文字のベースラインと X 軸との角度を、10 分の 1 度単位で指定します。
			FW_DONTCARE,												// フォントの太さを、0 から 1000 までの値で指定します。0 を指定すると、デフォルトの太さが選択されます。
			0,															// イタリック体にするかどうかを指定します。TRUE を指定すると、イタリック体が選択されます。
			0,															// 下線付きのフォントを選択するかどうかを指定します。TRUE を指定すると、下線付きフォントが選択されます。
			0,															// 打ち消し線付きフォントを選択するかどうかを指定します。TRUE を指定すると、打ち消し線付きのフォントが選択されます。
			DEFAULT_CHARSET,											// フォントの文字セットを指定します。
			OUT_DEFAULT_PRECIS,											// 出力精度を指定します。次の値のいずれかを指定します。
			CLIP_DEFAULT_PRECIS,										// クリッピング精度を指定します。クリッピング精度とは、文字の一部がクリッピング領域の外にはみ出たときに、その文字をクリップする方法を定義するものです。
			DRAFT_QUALITY,												// 出力品質を指定します。出力品質とは、GDI が、論理フォントの属性と実際の物理フォントの属性とをどの程度まで一致させるかを定義するものです。
			FIXED_PITCH | FF_DONTCARE,									// フォントのピッチとファミリを指定します。下位 2 ビットでフォントのピッチを指定します。( DEFAULT_PITCH, FIXED_PITCH, VARIABLE_PITCH )
			lf.lfFaceName												// フォントのタイプフェイス名が入った NULL で終わる文字列へのポインタを指定します。文字列の長さは、終端の NULL 文字も含めて 32 文字以下にしてください。NULL ポインタや、空の文字列へのポインタを指定すると、ほかのパラメータで指定した条件に合うフォントの中から最初に見つけたフォントが選択されます。現在利用可能なフォントのタイプフェイス名を列挙するには、EnumFontFamilies 関数を使います。
		);																// 戻り値:関数が成功すると、論理フォントのハンドルが返ります。関数が失敗すると、NULL が返ります。拡張エラー情報を取得するには、GetLastError 関数を使います。
		if( hfont ){
			DeleteObject( g_hfont );									// フォント破棄
			g_hfont=hfont;
			g_fxw=fxw;
			g_fyw=fyw;
			dspConfig( g_xw, g_yw );
			InvalidateRect( g_hwnd, 0, FALSE );							// 再描画
		}
	}
	ReleaseDC( g_hwnd, hdc );
}

/************************************************************************/
/*	メイン ウィンドウのメッセージを処理します。							*/
/************************************************************************/

static LRESULT CALLBACK WndProc( HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam ){

	LRESULT				res=0;											// 帰り値( 0:処理した, 1:未処理 )
	PAINTSTRUCT			ps;												// ペイント構造体
	static int			xedatr=0;
	int					xedk;											// 入力キーシンボル

	switch( message ) {
		case WM_CREATE:{												// 0001:ウインドウが作成中
			g_hwnd=hwnd;												// ウィンドウハンドルセット
			SetClassLong( hwnd, GCL_HBRBACKGROUND, 0 );					// 背景色を無しにする( 無効エリアを背景色で初期化しないようにする )
			break;
		}
		case WM_COMMAND:{												// アプリケーション メニューの処理
			switch( LOWORD( wparam ) ){									// メニューアイテム、コントロール、アクセラレータの識別子
				case IDM_ABOUT:{
					DialogBox( GetModuleHandle( 0 ), ( LPCTSTR )IDD_ABOUTBOX, hwnd, ( DLGPROC )about );
					break;
				}
				case IDM_EXIT:{											// 終了
					DestroyWindow( hwnd );								// この関数は、WM_DESTROY メッセージとWM_NCDESTROY メッセージをウィンドウに送り、キーボードの入力フォーカスをウィンドウから取り除きます。また、ウィンドウメニューの破棄、スレッドのメッセージキューのフラッシュ、タイマーの破棄、クリップボードの所有権の解放、クリップボードビューアチェインの切断も行います。
					break;
				}
				case ID_FONT:{											// フォントダイアログ
					FontDiaog();
					break;
				}
				default:{												// その他
					res=DefWindowProc( hwnd, message, wparam, lparam );
					break;
				}
			}
			break;
		}
		case WM_WINDOWPOSCHANGING:{										// ウィンドウ位置サイズが変更される前にクライアントに送られてくるメッセージ

			WINDOWPOS	*winposp=( WINDOWPOS* )lparam;
			int			xw, yw;											// クライアントエリアの横幅,縦幅

			xw=winposp->cx-g_frmxw;										// クライアントエリアX幅
			yw=winposp->cy-g_frmyw;										// クライアントエリアY幅
			winposp->cx = (xw/g_fxw)*g_fxw+g_frmxw;						// フォント単位にアジャストされたX幅
			winposp->cy = (yw/g_fyw)*g_fyw+g_frmyw;						// フォント単位にアジャストされたY幅
			break;
		}
		case WM_SIZE:{													// ウィンドウサイズ変更メッセージ
			dspConfig( LOWORD( lparam ), HIWORD( lparam ) );			// 表示コンフィグレーション
			break;
		}
		case WM_PAINT:{													// メインウィンドウの描画

//			RECT			rect;										// 矩形構造体

			g_hdc =BeginPaint( hwnd, &ps );								// デバイスコンテキストを割り当ててこう新領域に関する情報を返します。
			SelectObject( g_hdc, g_hfont );
//			GetClipBox( g_hdc, &rect );									// ウィンドウの表示範囲を囲む最小の長方形を取得します。ここでいう表示範囲は、ウィンドウのオーバーラップ状態、クリッピング領域、クリップパスによって定義されます。
//			display( rect.left, rect.top, rect.right, rect.bottom );	// 描画
			display( 0, 0, g_xw, g_yw );
			EndPaint( hwnd, &ps );										// ペイント処理の終了を指示します。
			g_hdc=0;
			break;
		}
		case WM_SYSKEYDOWN:												// コントロールキーが押された
		case WM_KEYDOWN:												// コントロールキーが押された
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
		case WM_SYSKEYUP:												// コントロールキーが押された
		case WM_KEYUP:													// コントロールキーが押された
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
				msgSet( "[ESC]" );										// メッセージ表示
			}else{
				msgSet( "" );											// メッセージ表示
			}
			mkeyPut( g_mkeyp, xedk );
			break;
		}
		case WM_VSCROLL:{
			setscroll( LOWORD( wparam ), HIWORD( wparam ) );
			break;
		}
		case WM_LBUTTONDOWN:{											// 左ボタンが押された
			curset( (short)LOWORD( lparam ), (short)HIWORD( lparam ) );
			break;
		}
		case WM_MOUSEWHEEL:{											// マウスホィール
			setscroll( WM_MOUSEWHEEL, (short)HIWORD(wparam) );
			break;
		}
		case WM_DROPFILES:{												// ドラッグドロップ

			stCTX	*ctxp;												// コンテキストポインタ
			char	fnam[4096];											// ファイル名
			int		n, i;

			n=DragQueryFile( (HDROP)wparam, 0xffffffff, NULL, 0 );		// ドロップされたファイル個数
			for( i=0; i<n; i++ ){										// ドロップされたファイル個数分ループ
				DragQueryFile( (HDROP)wparam, i, fnam, sizeof(fnam) );	// ドロップされたファイル名を得る
				ctxp=ctxOpen( fnam );									// コンテキストオープン
				if( !i ){												// 先頭
					g_ctxp=ctxp;										// カレントコンテキストポインタセット
				}
			}
			DragFinish( (HDROP)wparam );								// ドラッグドロップリソース破棄
			InvalidateRect( hwnd, 0, FALSE );							// 再描画
			break;
		}
		case WM_DESTROY:{												// 終了メッセージの通知とリターン
			SetWindowLong( hwnd, GWL_USERDATA, 0 );						// ウィンドウ構造体ポインタをウィンドウにセットする
			PostQuitMessage( 0 );										// Windows にスレッドの終了を要求します( パラメータは終了コード )
			break;
		}
		default:{														// その他
			res=DefWindowProc( hwnd, message, wparam, lparam );			// 既定のウィンドウプロシージャを呼び出して、アプリケーションが処理しないウィンドウメッセージに対して、既定の処理を提供します。
			break;
		}
	}
	return( res );
}

/************************************************************************/
/*	(windows)	キーを得る												*/
/************************************************************************/

int		getKey(void){

	MSG		msg;
	int		key;

	for(;;){
		if( !PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) ){			// メッセージは届いていない
			key=getmackey();											// マクロ処理付キーを得る
			if( key ){
				break;
			}
		}
		if( !GetMessage( &msg, NULL, 0, 0 ) ){
			g_flgend=1;													// 終了フラグセット
			return( 0 );
		}
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}
	return( key );
}

/************************************************************************/
/*	ウィンドウクラス登録												*/
/************************************************************************/

static int	Register( HINSTANCE hinstance ){

	WNDCLASSEX	wc={ 0 };												// ウィンドウクラス登録用構造体

	wc.cbSize		=sizeof( WNDCLASSEX );								// 構造体サイズ( Windows のライブラリが変わっても対応できるための配慮 )
	wc.style		=CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;				// ウィンドウクラススタイル
	wc.lpfnWndProc	=WndProc;											// メッセージを処理するためのウィンドウコールバック関数のポインタ
	wc.cbClsExtra	=0;													// ウィンドウクラス構造体の後ろに割り当てる補足バイト数を示します。
	wc.cbWndExtra	=sizeof(void*);										// ウィンドウインスタンスの後ろに割り当てる補足バイト数を示します。
	wc.hInstance	=hinstance;											// このクラスのためのウィンドウプロシージャがあるインスタンスハンドルです
	wc.hIcon		=LoadIcon( hinstance, (LPCSTR)IDI_CONTOUR );		// このウィンドウクラスに使用するアイコンのハンドルを指定します。
	wc.hCursor		=LoadCursor( NULL, IDC_ARROW );						// このウィンドウクラスに使用するカーソルのハンドルを指定します。
	wc.hbrBackground=( HBRUSH )( COLOR_WINDOW );						// ウィンドウの背景をペイントするためのブラシのハンドルを指定します。
	wc.lpszMenuName	=( LPCSTR )IDC_CONTOUR;								// このクラスのデフォルトメニュー名を指定します。
	wc.lpszClassName=CRTL;												// クラス名を指定します。
	wc.hIconSm		=LoadIcon( hinstance, (LPCSTR)IDI_CONTOUR );		// このクラスを使って作成されるタイトルバーで使われるアイコンのハンドルを
	return( RegisterClassEx( &wc ) );									// ウィンドウクラスの登録
}

/************************************************************************/
/*	ウィンドウアプリケーションエントリー								*/
/************************************************************************/

#	define	WS	( WS_OVERLAPPEDWINDOW | WS_VSCROLL )					/* ウィンドウスタイル*/

int		main( int argc, char *argv[] ){

	int			errn=0;													// エラー数
	HINSTANCE	hinstance;
	RECT		rect;													// 矩形構造体
	HWND		hwnd;													// ウィンドウハンドル
	int			xw, yw;

	hinstance=GetModuleHandle( 0 );
	if( cmdlinprc( argc, argv ) ){										// コマンドライン処理
		if( Register( hinstance ) ){									// ウィンドウクラス登録
			startup();													// スタートアップ
			g_fxw	=g_optfx;											// フォント横幅
			g_fyw	=g_optfy;											// フォント縦幅
			g_hfont	=CreateFont(										// 論理フォントを作成します。
				-g_fyw,													// フォントの高さを論理単位で指定します。
				g_fxw,													// フォントの平均文字幅を論理単位で指定します。0を指定すると適当な幅が選択されます
				0,														// 文字送りの方向と X 軸との角度を、10 分の 1 度単位で指定します。文字送りの方向は、テキストの列のベースラインと平行です。
				0,														// 各文字のベースラインと X 軸との角度を、10 分の 1 度単位で指定します。
				FW_DONTCARE,											// フォントの太さを、0 から 1000 までの値で指定します。0 を指定すると、デフォルトの太さが選択されます。
				0,														// イタリック体にするかどうかを指定します。TRUE を指定すると、イタリック体が選択されます。
				0,														// 下線付きのフォントを選択するかどうかを指定します。TRUE を指定すると、下線付きフォントが選択されます。
				0,														// 打ち消し線付きフォントを選択するかどうかを指定します。TRUE を指定すると、打ち消し線付きのフォントが選択されます。
				DEFAULT_CHARSET,										// フォントの文字セットを指定します。
				OUT_DEFAULT_PRECIS,										// 出力精度を指定します。次の値のいずれかを指定します。
				CLIP_DEFAULT_PRECIS,									// クリッピング精度を指定します。クリッピング精度とは、文字の一部がクリッピング領域の外にはみ出たときに、その文字をクリップする方法を定義するものです。
				DRAFT_QUALITY,											// 出力品質を指定します。出力品質とは、GDI が、論理フォントの属性と実際の物理フォントの属性とをどの程度まで一致させるかを定義するものです。
				FIXED_PITCH | FF_DONTCARE,								// フォントのピッチとファミリを指定します。下位 2 ビットでフォントのピッチを指定します。( DEFAULT_PITCH, FIXED_PITCH, VARIABLE_PITCH )
				g_optfn													// フォントのタイプフェイス名が入った NULL で終わる文字列へのポインタを指定します。文字列の長さは、終端の NULL 文字も含めて 32 文字以下にしてください。NULL ポインタや、空の文字列へのポインタを指定すると、ほかのパラメータで指定した条件に合うフォントの中から最初に見つけたフォントが選択されます。現在利用可能なフォントのタイプフェイス名を列挙するには、EnumFontFamilies 関数を使います。
			);															// 戻り値:関数が成功すると、論理フォントのハンドルが返ります。関数が失敗すると、NULL が返ります。拡張エラー情報を取得するには、GetLastError 関数を使います。
			assert( g_hfont );
			xw=g_optxn*g_fxw;
			yw=g_optyn*g_fyw;
			SetRect( &rect, 0, 0, xw, yw );								// 矩形構造体に値セット
			AdjustWindowRectEx( &rect, WS, 1, 0 );						// 指定されたクライアント領域を確保するために必要なウィンドウ座標を計算します。
			g_frmxw=( rect.right - rect.left )-xw;
			g_frmyw=( rect.bottom - rect.top )-yw;
			hwnd=CreateWindow(											// ウィンドウ作成
				CRTL,													// ウィンドウクラスの名前を指定します。
				CRTL,													// ウィンドウ名が入った、NULL で終わる文字列へのポインタを指定します。
				WS,														// 作成されるウィンドウのスタイルを指定します。
				CW_USEDEFAULT,											// ウィンドウの x 座標を指定します。
				CW_USEDEFAULT,											// ウィンドウの y 座標を指定します。
				rect.right - rect.left,									// ウィンドウの幅を指定します。
				rect.bottom - rect.top,									// ウィンドウの高さを指定します。
				NULL,													// 作成されるウィンドウの親ウィンドウまたはオーナーウィンドウのハンドル
				NULL,													// メニューまたは子ウィンドウ ID を指定します。
				hinstance,												// ウィンドウに関連付けられたモジュールのインスタンスハンドルを指定します。
				0														// WM_CREATE メッセージの lParam パラメータとして渡される、CREATESTRUCT 構造体へのポインタを指定します。
			);
			if( hwnd ){													// ウィンドウ作成成功
				DragAcceptFiles( g_hwnd, TRUE );						// ドラッグドロップ許可
				ShowWindow( hwnd, SW_SHOW );							// 指定されたウィンドウの表示状態を設定します
				UpdateWindow( hwnd );									// ウィンドウの更新領域が空でない場合、そのクライアント領域を更新します。
				SetScroll();											// スクロールインフォメーションセット
				xedmain();												// メインループ
				DragAcceptFiles( g_hwnd, FALSE );						// ドラッグドロップ禁止
			}else{														// ウィンドウ作成失敗
				fprintf( STDERR, "ウィンドウ作成エラー\n" );
				errn++;													// エラー数加算
			}
			DeleteObject( g_hfont );									// フォント破棄
			ctxClose( 0 );												// コンテキストクローズ
			cleanup();													// クリーンナップ
		}else{															// ウィンドウクラス登録エラー
			fprintf( STDERR, "ウィンドウクラス登録エラー:%s\n", CRTL );
			errn++;														// エラー数加算
		}
	}else{																// オプションエラー
		errn++;															// エラー数加算
	}
	dbClose( g_ctxdbp );												// コンテキストDBクローズ
	return( errn );														// エラー個数を持ってリターン
}

/************************************************************************/
/*	ウィンドウアプリケーションエントリー								*/
/************************************************************************/

#	ifdef	WINDOWS

int APIENTRY	WinMain( HINSTANCE, HINSTANCE, LPSTR, int ){
	return( main( __argc, __argv ) );									// エラー個数を持ってリターン
}

#	endif

/************************************************************************/
/*	コンソールアプリケーションメインエントリー							*/
/************************************************************************/

#	else

#		ifndef	CURSES

/************************************************************************/
/*																		*/
/*	unix																*/
/*																		*/
/************************************************************************/

#	define	SELECTMSK	( ExposureMask | KeyPressMask | EnterWindowMask | LeaveWindowMask | StructureNotifyMask | ButtonPressMask )

//	中くらいのフォント

	static const char	*fntnampaJM[]={									// 日本語
#	ifdef	__CYGWIN__
		"-*-fixed-medium-r-normal--16-*-*-*",
#	else
		"-*-msg16-medium-r-normal-*-16-150-75-75-m-*",
		"-*-fixed-medium-r-normal--16-*",
#	endif
		0
	};
	static const char	*fntnampaUM[]={									// 英語
		"screen16",
		"8x16",
		0
	};

/************************************************************************/
/*	入力スタイルを選択													*/
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
/*	入力コンテキスト作成												*/
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
						NULL );											// インプットコンテキストの作成
	if( ic == NULL ){
		fprintf( stderr, "Cannot create input context\n" );
	}
	XFree( list );
	return( ic );
}

/************************************************************************/
/*	入力位置の設定														*/
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
/*	FEP オープン														*/
/************************************************************************/

void	fepopen(void){

	int		mask;

	if( g_jp ){															// 日本語モード
		if( !g_fep ){													// FEPはまだオープンされていない
			if( !XSetLocaleModifiers( "" ) ){							// ユーザ指定のモディファイアを連結する
				fprintf( stderr, "Cannot open input modifires\n" );
				return;
			}
			if( !( g_im=XOpenIM( g_dpyp, 0, 0, 0 ) ) ){					// インプットメソッドをオープンする
				fprintf( stderr, "Cannot open input method\n" );
				return;
			}
			g_style=SelectInputStyle();									// 入力スタイルを選択
			g_ic=CreateInputContext( g_im, g_style, g_fs, g_win );		// 入力コンテキスト作成
			XGetICValues( g_ic, XNFilterEvents, &mask, NULL );			// インプットメソッドが必要とするイベントを問い合わせる
			XSelectInput( g_dpyp, g_win, SELECTMSK|mask );				// Xサーバから通知してもらうイベントを指定
			if( g_style & XIMPreeditArea ){
				SetGeometry( XIMPreeditArea, XNPreeditAttributes );		// 入力位置の設定
			}
			if( g_style & XIMStatusArea ){
				SetGeometry( XIMStatusArea, XNStatusAttributes );		// 入力位置の設定
			}
			XSetICValues( g_ic, XNFocusWindow, g_win, NULL );
			XSetICFocus( g_ic );
			g_fep=1;
		}
	}
}

/************************************************************************/
/*	(unix)	キーを得る													*/
/************************************************************************/

int		getKey(void){

	int				key;												// キー
	KeySym			keysym;
	XComposeStatus	cs;
	int				implen;
	XEvent			event;
	int				xedk, xedatr, xedkey;
	unsigned char	keybuf[256];										// キーバッファ
	Status			st;
	stDB			*cbdbp;												// クリップボードDB
	int				xc, yc, xd, yd;
	int				i;

	for(;;){
		if( !XEventsQueued( g_dpyp, QueuedAlready ) ){					// イベントはない
			key=getmackey();											// マクロ処理付キーを得る
			if( key ){
				break;
			}
		}
		XNextEvent( g_dpyp, &event );									// 次のイベントを得る
		if( XFilterEvent( &event, None ) ){								// FEP 処理の場合
			continue;
		}
		g_tim=event.xbutton.time;
		switch( event.type ){
			case EnterNotify:											// マウスが入った
				if( g_fep ){
					XSetICValues( g_ic, XNFocusWindow, g_win, NULL );
					XSetICFocus( g_ic );
				}
				break;
			case LeaveNotify:											// マウスが離れた
				if( g_fep ){
					XUnsetICFocus( g_ic );
				}
				break;
			case MapNotify:												// マッピングされた
				break;
			case ConfigureNotify:										// サイズ変更等
				xc=event.xconfigure.x;
				yc=event.xconfigure.y;
				xd=event.xconfigure.width;
				yd=event.xconfigure.height;
				if( g_xw!=xd || g_yw!=yd ){
					dspConfig( xd, yd );								// 表示コンフィグレーション
					invalidateAll();
				}
				if( g_fep ){
					if( g_style & XIMPreeditArea ){
						SetGeometry( XIMPreeditArea, XNPreeditAttributes );		// 表示位置の設定
					}
					if( g_style & XIMStatusArea ){
						SetGeometry( XIMStatusArea, XNStatusAttributes );		// 表示位置の設定
					}
				}
				break;
			case Expose:												// 再表示指令
				display( event.xexpose.x, event.xexpose.y, event.xexpose.x+event.xexpose.width, event.xexpose.y+event.xexpose.height );
				break;
			case ButtonPress:											// マウスボタンが押された
				break;
			case SelectionNotify:{										// セレクションデータがセットされた
				cbdbp=xselNotify( g_xselp, &event );					// セレクションデータを受け取る
				if( cbdbp ){
					ctrl_f9calbak( cbdbp );
					dbClose( cbdbp );
				}
				break;
			}
			case SelectionRequest:{										// セレクションリクエスト( 送信 )
				cbdbp=clpbrdGet( CLPBRDP, CB_TXT );						// クリップボードゲット
				if( cbdbp ){											// クリップボードゲット成功
					xselRequest( g_xselp, &event, cbdbp );				// セレクションリクエスト( 送信 )
					dbClose( cbdbp );									// クリップボードDBクローズ
				}
				break;
			}
			case SelectionClear:										// セレクションクリア
				xselClear( g_xselp );									// セレクションクリア
				break;
			case KeyPress:												// キーが押された
				xedatr=xedkey=0;
				if( event.xkey.state&(1<<0) ) xedatr|=MKEY_SHIFT;
				if( event.xkey.state&(1<<2) ) xedatr|=MKEY_CTRL;
				if( event.xkey.state&(1<<3) ) xedatr|=MKEY_ALT;
				if( !g_fep ){											// FEP は使用していない
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
							msgSet( "[ESC]" );							// メッセージ表示
						}else{
							msgSet( "" );								// メッセージ表示
						}
					}
				}else{													// FEP 使用中
					implen=XmbLookupString( g_ic, (XKeyPressedEvent *)(&event), (char*)keybuf, sizeof(keybuf), &keysym, &st );	// 入力文字列の取得
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
										msgSet( "[ESC]" );				// メッセージ表示
									}else{
										msgSet( "" );					// メッセージ表示
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
/*	(unix)	X-Window ジオメトリ環境を得る								*/
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

	int			errn=0;													// エラー数
	XSizeHints	xsh={ 0 };
	XFontStruct	*fs=0;
	char		*langp;
	int			i, j;
	int			xc, yc, xd, yd;
	int			rootxw, rootyw, musx, musy;
	int			forkid;
	const char	**fntnampa, *namp;
	stXCOL		*xcolp;

	if( cmdlinprc( argc, argv ) ){										// コマンドライン処理
		startup();														// スタートアップ
		forkid=fork();													// コンソールを開放するためにフォークする
		if( forkid<0 ){													// フォークエラー
			printf( "fork error" );
			exit( 1 );													// エラー終了
		}
		if( forkid ){													// 親
			exit( 0 );													// 終了してコンソールを開放する
		}
		if( !g_optu ){													// 強制英語指定ではない
			langp=getenv( "LANG" );										// 環境変数を得る
			if( langp ){												// "LANG"は設定されている
				if( !strcmp( langp, "ja_JP.EUC-JP" ) || !strcmp( langp, "ja_JP.EUC" ) || !strcmp( langp, "ja_JP.eucJP" ) || !strcmp( langp, "ja_JP.ujis" ) ){
					g_jp=1;												// 日本語モードセット
				}
			}
		}
		if( g_jp ){														// 日本語モード
			if( XSupportsLocale()==False ){								// Xlib が現在のロケールを扱えるかどうかを判断する
				fprintf( stderr, "XSupportsLocale ERROR\n" );
				exit( 1 );												// エラー終了
			}
		}

//		X-Window 処理開始

		XSetIOErrorHandler( mainErrPrc );								// エラーハンドラセット
		setlocale( LC_ALL, "" );										// ロケールを設定する(現在システムに設定されているロケールを使用)
		g_dpyp=XOpenDisplay( 0 );										// Xサーバと接続する
		if( !g_dpyp ){													// Xサーバ接続エラー
			printf( "XOpenDisplay:ERROR\n" );
			exit( 1 );													// エラー終了
		}
		if( !xGetGeo( g_dpyp, &rootxw, &rootyw, &musx, &musy ) ){
			exit( 1 );													// エラー終了
		}
		if( g_jp ){														// 日本語モード
			fntnampa=fntnampaJM;										// 中くらいのフォント
		}else{															// 英語モード
			fntnampa=fntnampaUM;										// 小さなフォント
		}
		if( g_jp ){														// 日本語モード

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
				if( g_optv ){											// バーボーズモード
					for( j=0; j<n_miss; j++ ){
						printf( "Font Not Found %s\n", miss[j] );
					}
				}
			}
			if( !g_fs ){
				printf( "All Font Not Found\n" );
				exit( 1 );												// エラー終了
			}
			fsep=XExtentsOfFontSet( g_fs );
			g_fxw		= fsep->max_ink_extent.width>>1;				// フォント横幅
			g_fyw		= fsep->max_ink_extent.height;					// フォント縦幅
			g_fntascent	=-fsep->max_ink_extent.y;						// アセント
		}else{															// 英語モード
			for( i=0; ( namp=fntnampa[i] )!=0; i++ ){
				fs=XLoadQueryFont( g_dpyp, fntnampa[i] );
				if( fs ){
					break;
				}
				if( g_optv ){											// バーボーズモード
					printf( "Font Not Found %s\n", fntnampa[i] );
				}
			}
			if( !fs ){
				printf( "All Font Not Found\n" );
				exit( 1 );												// エラー終了
			}
			g_fxw=fs->max_bounds.width;									// フォント横幅
			g_fyw=fs->ascent+fs->descent;								// フォント縦幅
			g_fntascent=fs->ascent;										// アセント
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
		dspConfig( xd, yd );											// 表示コンフィグレーション
		g_gc=XCreateGC( g_dpyp, g_win, 0, 0 );							// ＧＣ作成
		xcolp=colorref2xcolor( CR_BLACK );								// ＲＧＢからＸカラーに変換する
		XSetWindowBackground( g_dpyp, g_win, xcolp->col.pixel );		// 背景色設定
		if( !g_jp ){													// 英語モード
			XSetFont( g_dpyp, g_gc, fs->fid );							// ＧＣにフォントをセットする
		}
		fepopen();														// FEPオープン
		XSelectInput(	g_dpyp, g_win, SELECTMSK );						// Xサーバから通知してもらうイベントを指定
		XMapWindow(		g_dpyp, g_win );								// ウィンドウを画面に表示する(マップする)。
//		XMapRaised(		g_dpyp, g_win );								// ウィンドウを画面に表示する(マップする)。
//		XRaiseWindow(	g_dpyp, g_win );								// 効果がない
		XFlush(			g_dpyp );										// Xサーバへのリクエストバッファをフラッシュする。
		g_xselp=xselOpen( g_dpyp, g_win );								// セレクション管理オープン
		xedmain();														// メインループ
		xselClose( g_xselp );											// セレクション管理クローズ
		XFreeGC( g_dpyp, g_gc );										// ＧＣの破壊
		XSelectInput(	g_dpyp, g_win, 0 );								// Xサーバから通知してもらうイベントを指定
		XCloseDisplay(	g_dpyp );
		cleanup();														// クリーンナップ
	}else{																// オプションエラー
		errn++;															// エラー数加算
	}
	dbClose( g_ctxdbp );												// コンテキストDBクローズ
	return( errn );
}

#		else

/************************************************************************/
/*																		*/
/*	curses																*/
/*																		*/
/************************************************************************/
/*	(curses)	キーを得る												*/
/************************************************************************/

int		getKey(void){

	int			xedk;

	for(;;){
		xedk=getmackey();												// マクロ処理付キーを得る
		if( xedk ){
			break;
		}
		xedk=getch();
		if( xedk<=0 ){
			continue;
		}
		if( xedk==0x1b ){
			msgSet( "[ESC]" );											// メッセージ表示
		}else{
			msgSet( "" );												// メッセージ表示
		}
		switch( xedk ){
			case 0x7f:			xedk=MKEY_DELETE;			break;
			case KEY_BACKSPACE:	xedk=0x0008;				break;		// CTRL+H		バックスペース
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
/*	シグナルエラー														*/
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
/*	CURSES メインプログラム												*/
/************************************************************************/

int		main( int argc, char *argv[] ){

	int			errn=0;													// エラー数
	int			yy, xx;													// ウィンドウサイズ

	if( cmdlinprc( argc, argv ) ){										// コマンドライン処理
		setlocale( LC_ALL, "" );										// ロケールを設定する(現在システムに設定されているロケールを使用)
		clpbrdStartUp();												// クリップボードスタートアップ
		startup();														// xed のスタートアップ
		signal( SIGINT, sigint );										// シグナルセット
		signal( SIGTSTP, sigctrlz );									// シグナルセット
		signal( SIGQUIT, sigquit );										// シグナルセット
		initscr();														// カーセス初期化
		timeout( 1 );													// 1ms ブロック化読み込みを行い、なお入力が無いときは正常に動作しません
		nonl();															// 改行をリターンとラインフィードに変換しません． 
		raw();
		leaveok( stdscr, FALSE );										// ウィンドウの更新後に、カーソルを現在の座標に配置するよう指示します
		noecho();														// キーをタイプしても，スクリーンにエコーしないモードにします． 
		cbreak();														// キーの入力をRaw Modeにします．このモードでは，キー入力はバッファされず，すぐにプログラムに渡されます 
		keypad( stdscr, TRUE );											// bfにTRUEを指定した場合，wgetch等において，ファンクションキーを押したときにファンクションキーのコードを返すようになります．
		getmaxyx( stdscr, yy, xx );										// 指定されたウィンドウのサイズを取得します．
		g_fxw=8;														// フォント横幅セット
		g_fyw=16;														// フォント縦幅セット
		dspConfig( xx*g_fxw, yy*g_fyw );								// 表示コンフィグレーション
		invalidateAll();												// 全て再描画
		xedmain();														// xed メインループ
		endwin();														// カーセス終了
		cleanup();														// xed のクリーンナップ
		clpbrdCleanUp();												// クリップボードクリーンナップ
	}else{																// オプションエラー
		errn++;															// エラー数加算
	}
	return( errn );														// プログラム終了
}

#		endif

#	endif

