/************************************************************************/
/*																		*/
/*	数式評価					by H.Watanabe							*/
/*	Ver1.00	2009/04/04 13:44:21	作成									*/
/*																		*/
/************************************************************************/

#	include	<math.h>													/* 数学関数*/
#	include	<stdlib.h>													/* 標準ライブラリ関数*/
#	include	"evalf.h"													/* 数式評価*/

/************************************************************************/
/*	ローカル定義														*/
/************************************************************************/

	typedef struct{
		FPTYP			(*prc)( FPTYP, FPTYP );							// 値の評価をするコールバック関数
		signed char		pri;											// 演算子プライオリティ
		char			calnam[3];										// 演算子名
	}	stCODTAB;

/************************************************************************/
/*	各処理とデータテーブル												*/
/************************************************************************/

static FPTYP	evalfAnd(		FPTYP dst, FPTYP sou ){	return( (int )dst &(int )sou );	}
static FPTYP	evalfOr(		FPTYP dst, FPTYP sou ){	return( (int )dst |(int )sou );	}
static FPTYP	evalfUpa(		FPTYP dst, FPTYP sou ){	return( (int )dst ^(int )sou );	}
static FPTYP	evalfLsl(		FPTYP dst, FPTYP sou ){	return( (int )dst<<(int )sou );	}
static FPTYP	evalfLsr(		FPTYP dst, FPTYP sou ){	return( (int )dst>>(int )sou );	}
static FPTYP	evalfAdd(		FPTYP dst, FPTYP sou ){	return( dst +sou );	}
static FPTYP	evalfSub(		FPTYP dst, FPTYP sou ){	return( dst -sou );	}
static FPTYP	evalfPow(		FPTYP dst, FPTYP sou ){	return( pow( dst, sou ) );	}
static FPTYP	evalfMul(		FPTYP dst, FPTYP sou ){	return( dst *sou );	}
static FPTYP	evalfDiv(		FPTYP dst, FPTYP sou ){	return( dst /sou );	}
static FPTYP	evalfMod(		FPTYP dst, FPTYP sou ){	return( (int )dst %(int )sou );	}
static FPTYP	evalfEq(		FPTYP dst, FPTYP sou ){	return( dst==sou );	}
static FPTYP	evalfNe(		FPTYP dst, FPTYP sou ){	return( dst!=sou );	}
static FPTYP	evalfLe(		FPTYP dst, FPTYP sou ){	return( dst<=sou );	}
static FPTYP	evalfLt(		FPTYP dst, FPTYP sou ){	return( dst <sou );	}
static FPTYP	evalfGe(		FPTYP dst, FPTYP sou ){	return( dst>=sou );	}
static FPTYP	evalfGt(		FPTYP dst, FPTYP sou ){	return( dst >sou );	}
static FPTYP	evalfAndAnd(	FPTYP dst, FPTYP sou ){	return( dst&&sou );	}
static FPTYP	evalfOrOr(		FPTYP dst, FPTYP sou ){	return( dst||sou );	}

	static const stCODTAB	CodTab[]={
		{	 0,				-1, "//"	},								// コメント
		{	 0,				-1, "/*"	},								// コメント
		{	 evalfAndAnd,	 0, "&&"	},								// &&
		{	 evalfOrOr,		 0, "||"	},								// ||
		{	 evalfAnd,		 3, "&"		},								// &	and group
		{	 evalfOr,		 2, "|"		},								// |
		{	 evalfUpa,		 2, "^"		},								// ^
		{	 evalfLsl,		 5, "<<"	},								// <<	sft group
		{	 evalfLsr,		 5, ">>"	},								// >>
		{	 evalfAdd,		 4, "+"		},								// +	add group
		{	 evalfSub,		 4, "-"		},								// -
		{	 evalfPow,		 6, "**"	},								// べき乗
		{	 evalfMul,		 5, "*"		},								// *	mul grup
		{	 evalfDiv,		 5, "/"		},								// /
		{	 evalfMod,		 5, "%"		},								// %
		{	 evalfEq,		 1, "=="	},								// ==
		{	 evalfNe,		 1, "!="	},								// !=
		{	 evalfLe,		 1, "<="	},								// <=
		{	 evalfLt,		 1, "<"		},								// <
		{	 evalfGe,		 1, ">="	},								// >=
		{	 evalfGt,		 1, ">"		},								// >
		{	 0,				-1, ""		}								// end
	};

/************************************************************************/
/*	単項式評価															*/
/************************************************************************/

char	*evalfUni( FPTYP *dstp, const char *txtp, tpEVALDVAL *valprcp, void *usrp ){

	unsigned char	chr,uni;											// テキストから読み込んだ文字
	char			*nxtp;												// 次のテキストポインタ
	FPTYP			val=0.0;											// 表価値

	for(;;){															// スキップスペース
		chr=*txtp++;													// テキストポインタから１文字を得る
		if( chr!='\t' && chr!=' ' && chr!='+' ) break;					// タブ・スペース・＋のならばループする
	}
	switch( chr ){
		case '-':														// - 単項演算子
			txtp=evalfUni( &val, txtp, valprcp, usrp );					// 再帰コール
			*dstp=-val;													// NEG 処理
			break;
		case '~':														// ~ 単項演算子
			txtp=evalfUni( &val, txtp, valprcp, usrp );					// 再帰コール
			*dstp=~(int )val;											// NOT 処理
			break;
		case '(':
			txtp=evalf( dstp, txtp, valprcp, usrp );					// 再帰コール
			if( !txtp ) return( 0 );									// エラー終了
			if( *txtp++!=')' ) return( 0 );								// エラー終了
			break;
		case '0':														// １６進数,８進数,１０進数の０のいずれか
			if( (*txtp|0x20)=='x' ){									// 0x または 0X で始まる１６進数
				txtp++;													// 'x' または 'X' をスキップ
				for(;;){
					chr=*txtp++;										// テキストポインタから１文字を得る
					if( ( uni=(unsigned char)( chr-'0' ) )>=10 ){		// '0'～'9' の範囲ではない
						if( ( uni=(unsigned char)( (chr|0x20)-'a' ) )>=6 ) break;				// 'a'～'f' の範囲ではない
						uni+=10;										// 'a'～'f' の範囲なので10を加算する
					}
					val=val*16.0+(FPTYP)uni;							// １６倍して値を加算する
				}
				txtp--;													// テキストポインタは１つ進んでいるので戻す
			}else{														// 浮動小数
				val=strtod( txtp-1, (char**)&txtp );
				if( (*txtp|0x20)=='f' ){								// 数値終端の 'f' または 'F' は無視
					txtp++;
				}
			}
			*dstp=val;
			break;
		case '1': case '2': case '3':
		case '4': case '5': case '6':
		case '7': case '8': case '9':									// １０進数
			val=strtod( txtp-1, (char**)&txtp );
			if( (*txtp|0x20)=='f' ){									// 数値終端の 'f' または 'F' は無視
				txtp++;
			}
			*dstp=val;
			break;
		default:
			if( !valprcp ) return( 0 );									// ユーザー指定処理無しの場合はエラー
			nxtp=valprcp( dstp, txtp-1, usrp );							// ユーザー指定処理コール
			if( nxtp<txtp ) return( 0 );								// 評価不能.
			txtp=nxtp;													// テキストポインタ更新
			break;
	}
	return( (char*)txtp );												// 正常終了
}

/************************************************************************/
/*	数式評価( Evaluation of Expressions )								*/
/************************************************************************/

char	*evalf( FPTYP *dstp, const char *txtp, tpEVALDVAL *valprcp, void *usrp ){

	struct{																// 演算プライオリティ処理のための評価構造体
		signed char pri;												// プライオリティ
		FPTYP		(*prc)( FPTYP, FPTYP );								// 処理アドレス
		FPTYP		val;												// 演算結果
	}				va[6+1],*vp;										// 評価配列と評価スタックポインタ
	const stCODTAB	*codp;												// 予約語テーブルポインタ
	const char		*curp;												// 演算子検索のためのポインタ
	const char		*namp;												// 演算子予約語ポインタ
	int				chr;												// char で良いのだが 高速化・メモリ節約のため int とする

	vp=va;																// 評価スタックポインタ初期化
	va[0].pri=-2;														// 門番
	for(;;){															// 評価ループ( VC5 バグのため do while の使用禁止 )
		txtp=evalfUni( &vp->val, txtp, valprcp, usrp );					// 単項式の評価
		if( !txtp ) return( 0 );										// エラー終了
		for(;;){														// スキップスペース
			chr=*txtp++;												// テキストポインタから１文字を得る
			if( chr!='\t' && chr!=' ' ) break;							// タブ・スペースならばループする
		}
		curp=txtp-1;													// テキストポインタは１つ進んでいるので戻す
		for( codp=CodTab; ; codp++ ){									// 演算子サーチループ
			txtp=curp;													// テキストポインタをカレントにセット
			namp=codp->calnam;											// テーブル中の名前
			while( ( chr=*namp++ )!=0 ){
				if( chr!=*txtp++ ) break;								// 文字不一致
			}
			if( !chr ) break;											// 見つかった
		}
		while( vp->pri>=codp->pri ){									// 評価スタックのプライオリティがカレントプライオリティ以上の間ループ
			(vp-1)->val=vp->prc( (vp-1)->val, vp->val );				// 演算実行
			vp--;														// 評価スタックポインタ減算
		}
		vp++;															// 評価スタックポインタ加算
		if( (vp->pri=codp->pri)<0 ) break;								// テーブル終端の プライオリティ=-1 を見つけるまで
		vp->prc=codp->prc;												// 演算処理アドレス
	}
	*dstp=va[0].val;													// 計算結果の書き込み
	return( (char*)txtp );												// 正常終了
}

