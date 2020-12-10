/************************************************************************/
/*																		*/
/*	ラインマップ				by H.Watanabe							*/
/*	Ver1.00	2009/04/05 13:23:49	作成									*/
/*																		*/
/************************************************************************/

#	include	<string.h>													/* 標準文字列関数*/
#	include	"lm.h"														/* ラインマップ*/

/************************************************************************/
/*	ローカル定義														*/
/************************************************************************/

#	ifndef	EUC
#		if	defined(_WIN32) || defined(__CYGWIN__)						/* SJIS*/
#			define	EUC			0
#		else															/* EUC*/
#			define	EUC			1
#		endif
#	endif

#	define	RNGTSTB(C,L,H)	((unsigned char)((C)-(L))<=(unsigned char)((H)-(L)))				/* 符号無し８ビットでレンジチェックをする*/

	typedef unsigned char	uchar;

/************************************************************************/
/*	ラインマップ作成													*/
/************************************************************************/

int		lmMak( stLM *lmp, const char *txtadr, int tab, int wordflg ){

	enum{
		NO,																// 無効
		CD,																// 記号
		SP,																// スペースまたはタブ
		LB,																// ラベル
		KJ																// 漢字
	};
	static const char	typa[]={										// ラベルキBャラクOタテーブル
		CD,CD,CD,CD,CD,CD,CD,CD,CD,SP,CD,CD,CD,CD,CD,CD,				// 0x00
		CD,CD,CD,CD,CD,CD,CD,CD,CD,CD,CD,CD,CD,CD,CD,CD,				// 0x10
		SP,CD,CD,CD,CD,CD,CD,CD,CD,CD,CD,CD,CD,CD,CD,CD,				// 0x00
		LB,LB,LB,LB,LB,LB,LB,LB,LB,LB,CD,CD,CD,CD,CD,CD,				// 0x00
		CD,LB,LB,LB,LB,LB,LB,LB,LB,LB,LB,LB,LB,LB,LB,LB,				// 0x40
		LB,LB,LB,LB,LB,LB,LB,LB,LB,LB,LB,CD,CD,CD,CD,LB,				// 0x50
		CD,LB,LB,LB,LB,LB,LB,LB,LB,LB,LB,LB,LB,LB,LB,LB,				// 0x60
		LB,LB,LB,LB,LB,LB,LB,LB,LB,LB,LB,CD,CD,CD,CD,CD,				// 0x70
	};
	uchar		*txtp;
	int			chr;
	tpLM	txtofs=0;
	int			typbak=NO;
	int			typ;
	int			pos=0;
	int			siz;
	int			i;

	txtp=(uchar*)txtadr;
	while( ( chr=*txtp++ )!=0 ){
		if( pos>=LM_MAX ){												// 行バッファオーバーフロー
			break;
		}
		if( !wordflg ){													// ワードフラグはたっていない
			txtofs=(tpLM)( (txtp-1)-(uchar*)txtadr );					// テキストオフセット計算
		}else{															// ワードフラグがたっている
			if( chr&0x80 ){												// 漢字
				typ=KJ;													// カレントタイプは漢字
			}else{														// ASCII
				typ=typa[chr];											// カレントタイプはコードまたはスペースまたはラベル
			}
			if( typbak!=typ || typ==CD ){								// 前タイDプと違うかまたはコード
				typbak=typ;												// 前タイDプ更新
				txtofs=(tpLM)( (txtp-1)-(uchar*)txtadr );				// テキストオフセット計算
			}
		}
		siz=1;															// デフォルトサイズセット
		if( chr=='\t' ){												// TAB
			siz=((pos+tab)&-tab)-pos;
		}else if( chr&0x80 ){											// 半角または全角コード
#	if	!EUC															// SJIS
			if( RNGTSTB(chr,0x81,0x9f)|RNGTSTB(chr,0xe0,0xfc) ){		// WIN32:SJIS
				siz=2;
				txtp++;
			}
#	else																// EUC
			if( chr!=0x8e ){											// 半角かなではない
				siz=2;
			}
			txtp++;
#	endif
		}
		for( i=0; i<siz; i++ ){
			lmp->txtofsa[pos++]=txtofs;
		}
	}
	lmp->txtofsa[pos]=(tpLM)( (txtp-1)-(uchar*)txtadr );
	lmp->siz=pos;
	return( pos );
}

/************************************************************************/
/*	ラインマップによるカーソルポジションの正規化						*/
/************************************************************************/

int		lmPosAdj( stLM *lmp, int pos, int ofs ){

	int		txtofs;

	if( pos>=lmp->siz ){
		pos=lmp->siz;
	}
	txtofs=lmp->txtofsa[pos];
	while( pos>0 && txtofs==lmp->txtofsa[pos-1] ){
		pos--;
	}
	if( ofs<0 ){														// 左移動
		if( pos ){
			pos--;
			txtofs=lmp->txtofsa[pos];
			while( pos>0 && txtofs==lmp->txtofsa[pos-1] ){
				pos--;
			}
		}
	}else if( ofs>0 ){													// 右移動
		while( pos<lmp->siz && txtofs==lmp->txtofsa[pos] ){
			pos++;
		}
	}
	return( pos );
}

/************************************************************************/
/*	ソースのオフセットからカーソルポジションを得る						*/
/************************************************************************/

int		lmOfs2Pos( stLM *lmp, int txtofs ){

	int		i;

	for( i=0; i<lmp->siz; i++ ){
		if( lmp->txtofsa[i]>=txtofs ){
			break;
		}
	}
	return( i );
}

/************************************************************************/
/*	カーソルポジションからソースのオフセットを得る						*/
/************************************************************************/

int		lmPos2Ofs( stLM *lmp, int pos ){

	if( pos>lmp->siz ) pos=lmp->siz;
	return( lmp->txtofsa[pos] );
}

/************************************************************************/
/*	文字列セット														*/
/************************************************************************/

int		lmSetStr( char *txtadr, int curxx, const char *strp, int tabstp, int flgins ){

	stLM	lm;															// 行マップ
	int			nxtxx;
	int			curofs;													// カレントオフセット
	int			nxtofs;													// カレントオフセット
	char		txta[LM_MAX+4];
	char		*txtp;

	lmMak( &lm, txtadr, tabstp, 0 );									// 行マップ作成
	curxx=lmPosAdj( &lm, curxx, 0 );									// カーソル位置正規化
	curofs=lm.txtofsa[curxx];											// カットオフセット
	nxtxx=lmPosAdj( &lm, curxx, flgins==0 );
	nxtofs=lm.txtofsa[nxtxx];
	txtp=txta;
	memcpy( txtp, txtadr, curofs );
	txtp+=curofs;
	strcpy( txtp, strp );
	txtp+=strlen( txtp );
	strcpy( txtp, txtadr+nxtofs );
	strcpy( txtadr, txta );
	lmMak( &lm, txtadr, tabstp, 0 );									// 行マップ作成
	curxx=lmPosAdj( &lm, curxx, 1 );									// カーソル位置正規化
	return( curxx );
}

/************************************************************************/
/*	一文字セット														*/
/************************************************************************/

int		lmSetChr( char *txtadr, int curxx, int xedk, int tabstp, int flgins ){

	char	tmpa[4];
	char	*tmpp=tmpa;

	if( xedk&0xff00 ){
		*tmpp++=(char)(xedk>>8);
	}
	*tmpp++=(char)xedk;
	*tmpp=0;
	return( lmSetStr( txtadr, curxx, tmpa, tabstp, flgins ) );
}

