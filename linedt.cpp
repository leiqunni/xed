/************************************************************************/
/*																		*/
/*	一行編集					by H.Watanabe							*/
/*	Ver1.00	2009/04/04 13:49:05	作成開始								*/
/*																		*/
/************************************************************************/

#	include	<stdio.h>													/* 標準入出力関数*/
#	include	<string.h>													/* 標準文字列関数*/
#	include	"std/txtdl.h"												/* テキストエディタのためのディスプレイリスト変換*/
#	include	"std/clpbrd.h"												/* クリップボード管理*/
#	include	"main.h"													/* メインインクルードファイル*/

/************************************************************************/
/*	ローカル定義														*/
/************************************************************************/

#	define	LEW			64

	static int			g_leflg;										// 一行編集フラグ
	static char			g_lettl[256];									// 一行編集タイトル
	static char			g_lebuf[256];									// 一行編集バッファ
	static int			g_lexxc, g_leyyc, g_lexxd, g_leyyd;				// 一行編集座標
	static int			g_lecurxx;										// 一行編集カーソルポジション

/************************************************************************/
/*	一行編集再表示														*/
/************************************************************************/

void	linedtReDsp(void){

	static const unsigned long	cola[2]={
		CR_WHITE,														// ホワイト
		CR_LB,															// ライトブルー
	};
	unsigned long	bakforcola[2]={ CR_BLACK, CR_WHITE };				// 背景色、前景色
	stDB			*dldbp;												// テキストディスプレイリストDB
	stTXTDL			*dlp;												// テキストディスプレイリストポインタ
	int				xx=0;												// 横座標
	int				xx2;												// 右座標

	if( g_leflg ){
		xx2=g_lexxd-2;													// 右座標
		displayChrBox( g_lexxc, g_leyyc, g_lexxd, g_leyyd, g_lettl );	// キャラクタボックス表示
		dldbp=txtdl( g_lebuf, g_lecurxx, g_tabstp, g_tabdsp, 0, 0, 0, 0, xx2 );	// テキストディスプレイリストコンパイル
		dbFOR( dldbp, dlp, stTXTDL* ){									// テキストディスプレイリスト総当り
			if( dlp->cur ){												// カーソル
				bakforcola[1]=cola[0];									// テキストカラーセット
			}else{
				bakforcola[1]=cola[dlp->col];							// テキストカラーセット
			}
			if( dbKEYSIZ( dlp ) ){
				drwTxt( g_lexxc+1+xx, g_leyyc+1, (char*)dbKEYADR( dlp ), dbKEYSIZ( dlp ), bakforcola[dlp->rvs], bakforcola[!dlp->rvs] );
			}else{
				drwFil( g_lexxc+1+xx, g_leyyc+1, g_lexxc+1+xx+dlp->dspsiz, g_leyyc+2, bakforcola[dlp->rvs] );
			}
			xx+=dlp->dspsiz;											// 座標更新
		}
		dbClose( dldbp );												// ディスプレイリストDBクローズ
		if( xx<xx2 ){													// 空きエリアが存在する
			drwFil( g_lexxc+1+xx, g_leyyc+1, g_lexxc+1+xx2, g_leyyc+2, CR_BLACK );				// クリア
		}
	}
}

/************************************************************************/
/*	一行編集座標計算													*/
/************************************************************************/

static void	linedtCrd( int *xxcp, int *yycp, int *xxdp, int *yydp ){

	int		xxw;
	int		xx1, yy1, xx2, yy2;

	xxw=g_xxw-2;
	if( xxw>LEW ){
		xxw=LEW;
	}
	xx1=g_txxc+CURXX-PGXX-1;
	if( xx1<g_txxc ){
		xx1=g_txxc;
	}
	yy1=g_tyyc+CURYY+1;												
	xx2=xx1+xxw+2;
	yy2=yy1+3;
	if( xx2>g_xxw ){
		xx2=g_xxw;
		xx1=xx2-xxw-2;
	}
	if( yy2>g_yyw ){
		yy1=CURYY-3;
		yy2=yy1+3;
	}
	*xxcp=xx1;
	*yycp=yy1;
	*xxdp=xx2-xx1;
	*yydp=yy2-yy1;
}

/************************************************************************/
/*	１行スクリーンエディット											*/
/************************************************************************/

static int	linedts(void){

	stLM		lm;
	int			curtmp;
	int			keysym;
	int			ctrlv=0;												// CTRL+Vフラグ
	char		*cutp;
	stDB		*cbdbp;													// クリップボードDB
	char		tmpa[256];

	while( !g_flgend ){
		lmMak( &lm, g_lebuf, g_tabstp, 0 );
		invalidateLinedt();
		keysym=getKey()&0xffff;
		if( !ctrlv ){													// 前のキーはCTRL+Vではなかった
			switch( keysym ){
				case 0x0016:											// CTRL+V
					ctrlv=1;											// CTRL+Vフラグセット
					break;
				case 0x0003:											// CTRL+C
				case 0x001b:											// ESC
				case MKEY_ESC_ESC:										// ESC
					return( 0 );										// キャンセル
				case 0x000d:											// CR
					return( 1 );										// 編集終了
				case 0x001e:
					getcurword( tmpa );
					g_lecurxx=lmSetStr( g_lebuf, g_lecurxx, tmpa, g_tabstp, g_flgins );
					break;
				case 0x0008:											// BS
					g_lecurxx=lmPosAdj( &lm, g_lecurxx, 0 );
					if( g_lecurxx ){
						curtmp=lmPosAdj( &lm, g_lecurxx, -1 );
						strcpy( g_lebuf+lm.txtofsa[curtmp], g_lebuf+lm.txtofsa[g_lecurxx] );
						g_lecurxx=curtmp;
					}
					break;
				case MKEY_DELETE:										// DEL
					g_lecurxx=lmPosAdj( &lm, g_lecurxx, 0 );
					if( g_lecurxx!=lm.siz ){
						curtmp=lmPosAdj( &lm, g_lecurxx, 1 );
						strcpy( g_lebuf+lm.txtofsa[g_lecurxx], g_lebuf+lm.txtofsa[curtmp] );
					}
					break;
				case MKEY_RIGHT:										// 右
					g_lecurxx=lmPosAdj( &lm, g_lecurxx, 0 );
					if( g_lecurxx!=lm.siz ){
						g_lecurxx=lmPosAdj( &lm, g_lecurxx, 1 );
					}
					break;
				case MKEY_LEFT:											// 左
					g_lecurxx=lmPosAdj( &lm, g_lecurxx, 0 );
					if( g_lecurxx ){
						g_lecurxx=lmPosAdj( &lm, g_lecurxx, -1 );
					}
					break;
				case MKEY_F9:
					cbdbp=clpbrdGet( CLPBRDP, CB_TXT );					// クリップボードゲット
					if( cbdbp ){
						cutp=(char*)dbTop( cbdbp );
						if( cutp ){
							strcpy( g_lebuf, cutp );
							g_lecurxx=lmMak( &lm, g_lebuf, g_tabstp, 0 );
						}
						dbClose( cbdbp );								// クリップボードDBクローズ
					}
					break;
				default:
					g_lecurxx=lmSetChr( g_lebuf, g_lecurxx, keysym, g_tabstp, g_flgins );
					break;
			}
		}else{															// 前のキーはCTRL+V
			ctrlv=0;													// CTRL+Vフラグクリア
			g_lecurxx=lmSetChr( g_lebuf, g_lecurxx, keysym, g_tabstp, g_flgins );
		}
	}
	return( 0 );
}

/************************************************************************/
/*	一行編集															*/
/************************************************************************/

int		linedt( char *bufadr, const char *ttlp ){

	int			res;													// リザルト
	stLM		lm;														// 行マップ

	strcpy( g_lebuf, bufadr );											// 一行編集バッファ
	strcpy( g_lettl, ttlp );											// 一行編集タイトル
	linedtCrd( &g_lexxc, &g_leyyc, &g_lexxd, &g_leyyd );				// 一行編集座標計算
	g_lecurxx=lmMak( &lm, g_lebuf, g_tabstp, 0 );						// カーソルを行終端にセット
	g_leflg=1;															// 一行編集フラグセット
	res=linedts();														// １行スクリーンエディット
	g_leflg=0;															// 一行編集フラグクリア
	if( res ){															// 正常終了した
		strcpy( bufadr, g_lebuf );										// 元バッファにコピー
	}
	invalidateAll();													// 全て再描画
	return( res );
}

/************************************************************************/
/*	一行編集を２回														*/
/************************************************************************/

int		linedt2( char *buf1adr, const char *ttl1p, char *buf2adr, const char *ttl2p ){

	int		res=0;

	if( linedt( buf1adr, ttl1p ) ){										// 一行編集
		if( buf1adr[0] ){
			res=linedt( buf2adr, ttl2p );								// 一行編集
		}
	}
	return( res );
}

