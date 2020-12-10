/************************************************************************/
/*																		*/
/*	数式評価( Evaluation of Expressions )	by H.Watanabe				*/
/*	Ver1.00	2009/04/05 13:52:04	作成									*/
/*																		*/
/************************************************************************/

#	include	"eval.h"													/* 数式評価( Evaluation of Expressions )*/

/************************************************************************/
/*	ローカル定義														*/
/************************************************************************/

	typedef struct{														// 予約語テーブル
		int			cal;												// 演算タイプ
		signed char pri;												// 演算プライオリティ
		char		calnam[3];											// 演算子名
	}	stCODTAB;

/************************************************************************/
/*	各処理とデータテーブル												*/
/************************************************************************/

	enum{
		CAL_ANDAND,														// &&:論理積
		CAL_OROR,														// ||:論理和
		CAL_AND,														// & :ビットごとの AND
		CAL_OR,															// | :ビットごとの OR
		CAL_XOR,														// ^ :ビットごとの XOR
		CAL_LSL,														// <<:左シフト
		CAL_LSR,														// >>:右シフト
		CAL_ADD,														// + :加算
		CAL_SUB,														// - :減算
		CAL_MUL,														// * :乗算
		CAL_DIV,														// / :除算
		CAL_MOD,														// % :剰余
		CAL_EQ,															// ==:等しい
		CAL_NE,															// !=:等しくない
		CAL_LE,															// <=:小さいか等しい
		CAL_LT,															// < :小さい
		CAL_GE,															// >=:大きいか等しい
		CAL_GT,															// > :大きい
		CAL_N
	};

	static const stCODTAB CodTab[]={									// プライオリティはＣと同じ
		{	CAL_ANDAND,	 5,		"&&"	},								// &&:論理積
		{	CAL_OROR,	 4,		"||"	},								// ||:論理和
		{	CAL_AND,	 8,		"&"		},								// & :ビットごとの AND
		{	CAL_OR,		 6,		"|"		},								// | :ビットごとの OR
		{	CAL_XOR,	 7,		"^"		},								// ^ :ビットごとの XOR
		{	CAL_LSL,	11,		"<<"	},								// <<:左シフト
		{	CAL_LSR,	11,		">>"	},								// >>:右シフト
		{	CAL_ADD,	12,		"+"		},								// + :加算
		{	CAL_SUB,	12,		"-"		},								// - :減算
		{	CAL_MUL,	13,		"*"		},								// * :乗算
		{	CAL_DIV,	13,		"/"		},								// / :除算
		{	CAL_MOD,	13,		"%"		},								// % :剰余
		{	CAL_EQ,		 9,		"=="	},								// ==:等しい
		{	CAL_NE,		 9,		"!="	},								// !=:等しくない
		{	CAL_LE,		10,		"<="	},								// <=:小さいか等しい
		{	CAL_LT,		10,		"<"		},								// < :小さい
		{	CAL_GE,		10,		">="	},								// >=:大きいか等しい
		{	CAL_GT,		10,		">"		},								// > :大きい
		{	0,			-1,		""		}								// テーブルの終り
	};

/************************************************************************/
/*	文字列を数値に変換する												*/
/************************************************************************/

static char	*s2ip( EVALTYP *valp, const char *txtp, int radix ){

	char	chr;														// テキスト文字
	int		uni;														// 単位
	EVALTYP	val=0;														// 数値表価値

	for(;;){
		chr=*txtp++;													// テキストポインタから１文字を得る
		if( chr<='9' ){													// '9'以下
			uni=(unsigned char)( chr-'0' );								// 文字から'0'を引いて unsigned char でキャスト
		}else{															// '9'以下ではない
			uni=( (unsigned char)( (chr|0x20)-'a' ) )+10;				// 文字から'a'を引いて unsigned char でキャストし、10を加算する
		}
		if( uni>=radix ){												// 基数より大きい
			break;														// ループ終了
		}
		val=val*radix+uni;												// 基数倍して値を加算する
	}
	if( valp ){															// ディストネーションポインタが指定されている
		*valp=val;														// 値書き込み
	}
	return( (char*)( txtp-1 ) );										// テキストポインタは次の文字にシークしているので－１した値を返す
}

/************************************************************************/
/*	単項式の評価														*/
/************************************************************************/

static char	*evalUni( EVALTYP *dstp, const char *txtp, tpEVALCB *valprcp, void *usrp ){

	unsigned char	chr;												// テキスト文字
	EVALTYP			val;												// 数値評価ワーク
	const char		*bakp;												// テキストポインタバックアップ

	for(;;){															// スキップスペース
		chr=*txtp++;													// テキストポインタから１文字を得る
		if( chr!='\t' && chr!=' ' && chr!='\n' && chr!='+' ){			// タブ・スペース・改行コード・＋以外
			break;														// ループ終了
		}
	}
	switch( chr ){
		case '-':														// - 単項演算子
			txtp=evalUni( &val, txtp, valprcp, usrp );					// 再帰コール
			*dstp=-val;													// NEG 処理
			break;														// switch 終了
		case '~':														// ~ 単項演算子
			txtp=evalUni( &val, txtp, valprcp, usrp );					// 再帰コール
			*dstp=~val;													// NOT 処理
			break;														// switch 終了
		case '!':														// ~ 単項演算子
			txtp=evalUni( &val, txtp, valprcp, usrp );					// 再帰コール
			*dstp=( val==0 );											// NOT 処理
			break;														// switch 終了
		case '(':
			txtp=eval( dstp, txtp, valprcp, usrp );						// 再帰コール
			if( !txtp ){												// エラーが起こった
				return( 0 );											// エラー終了
			}
			if( *txtp++!=')' ){											// ')'が無い
				return( 0 );											// エラー終了
			}
			break;														// switch 終了
		case '\'':														// 文字定数
			val=0;
			while( ( chr=*txtp++ )!=0 ){
				if( chr=='\'' ){
					break;
				}
				val=(val<<8)+chr;
			}
			*dstp=val;
			break;
		case '0':
			if( *txtp=='x' || *txtp=='X' ){								// 0x または 0X で始まる１６進数
				txtp=s2ip( dstp, txtp+1, 16 );							// 文字列を数値に変換する
			}else{														// ８進数または１０進数の０
				txtp=s2ip( dstp, txtp, 8 );								// 文字列を数値に変換する
			}
			break;														// switch 終了
		case '1': case '2': case '3':
		case '4': case '5': case '6':
		case '7': case '8': case '9':									// １０進数
			txtp=s2ip( dstp, txtp-1, 10 );								// 文字列を数値に変換する
			break;														// switch 終了
		default:
			if( !valprcp ){												// ユーザー指定処理無しの場合
				return( 0 );											// エラーリターン
			}
			txtp=valprcp( dstp, bakp=txtp-1, usrp );					// ユーザー指定処理コール
			if( txtp<=bakp ){											// 評価不能.
				return( 0 );											// エラーリターン
			}
			break;														// switch 終了
	}
	return( (char*)txtp );												// 正常終了
}

/************************************************************************/
/*	数式評価( Evaluation of Expressions )								*/
/************************************************************************/

char	*eval( EVALTYP *dstp, const char *txtp, tpEVALCB *valprcp, void *usrp ){

	struct{																// 演算プライオリティ処理のための評価構造体
		signed char pri;												// プライオリティ
		int			cal;												// 演算タイプ
		EVALTYP		val;												// 演算結果
	}				va[10+1],*vp;										// 評価配列と評価スタックポインタ
	const stCODTAB	*codp;												// 予約語テーブルポインタ
	const char		*curp;												// 演算子検索のためのポインタ
	const char		*namp;												// 演算子予約語ポインタ
	char			chr;												// カレント文字
	EVALTYP			dst, sou;

	vp=va;																// 評価スタックポインタ初期化
	va[0].pri=-2;														// 門番
	while( txtp ){														// 評価ループ( VC5 バグのため do while の使用禁止 )
		txtp=evalUni( &vp->val, txtp, valprcp, usrp );					// 単項式の評価
		if( !txtp ){													// エラーが起こった
			return( 0 );												// エラー終了
		}
		for(;;){														// スキップスペース
			chr=*txtp++;												// テキストポインタから１文字を得る
			if( chr!='\t' && chr!=' ' ){								// タブ・スペースならばループする
				break;
			}
		}
		curp=txtp-1;													// テキストポインタは１つ進んでいるので戻す
		for( codp=CodTab; ; codp++ ){									// 演算子サーチループ
			txtp=curp;													// テキストポインタをカレントにセット
			namp=codp->calnam;											// テーブル中の名前
			while( ( chr=*namp++ )!=0 ){
				if( chr!=*txtp++ ){										// 文字不一致
					break;												// ループ終了
				}
			}
			if( !chr ){													// 見つかった
				break;													// ループ終了
			}
		}
		while( vp->pri>=codp->pri ){									// 評価スタックのプライオリティがカレントプライオリティ以上の間ループ
			dst	=(vp-1)->val;											// ディストネーション
			sou	=vp->val;												// ソース
			switch( vp->cal ){
				case CAL_AND:		dst=( dst & sou );	break;			// &
				case CAL_OR:		dst=( dst | sou );	break;			// |
				case CAL_XOR:		dst=( dst ^ sou );	break;			// ^
				case CAL_LSL:											// 左シフト
					if( sou<0 || sou>63 ){								// シフト範囲を超えている
						if( valprcp ){									// コールバック関数が指定されている
							valprcp( 0, EVALERR_LSL_OF, usrp );			// エラー通知コールバック
						}
						txtp=0;											// エラー
					}
					dst=( dst<< sou );									// 結果の書き込み
					break;
				case CAL_LSR:											// 右シフト
					if( sou<0 || sou>63 ){								// シフト範囲を超えている
						if( valprcp ){									// コールバック関数が指定されている
							valprcp( 0, EVALERR_LSR_OF, usrp );			// エラー通知コールバック
						}
						txtp=0;											// エラー
					}
					dst=( dst>> sou );									// 結果の書き込み
					break;
				case CAL_ADD:		dst=( dst + sou );		break;		// 加算
				case CAL_SUB:		dst=( dst - sou );		break;		// 減算
				case CAL_MUL:		dst=( dst * sou );		break;		// 乗算
				case CAL_DIV:											// 除算
					if( !sou ){											// ０除算
						if( valprcp ){									// コールバック関数が指定されている
							valprcp( 0, EVALERR_DIV_0, usrp );			// エラー通知コールバック
						}
						txtp=0;											// エラー
					}else{
						if( dst==0x80000000 && sou==-1 ){				// 表現できる負の最大数を－１で割ると、２の歩数で表した符号付整数の場合はオーバーフローとなることがある。
							if( valprcp ){								// コールバック関数が指定されている
								valprcp( 0, EVALERR_DIV_OF, usrp );		// エラー通知コールバック
							}
							txtp=0;										// エラー
						}else{
							dst=( dst / sou );							// 結果の書き込み
						}
					}
					break;
				case CAL_MOD:											// 剰余
					if( !sou ){											// ０剰余
						if( valprcp ){									// コールバック関数が指定されている
							valprcp( 0, EVALERR_MOD_0, usrp );			// エラー通知コールバック
						}
						txtp=0;											// エラー
					}else{
						if( sou<0 ){									// 剰余は正数を指定すべき
							if( valprcp ){								// コールバック関数が指定されている
								valprcp( 0, EVALERR_MOD_OF, usrp );		// エラー通知コールバック
							}
							txtp=0;										// エラー
						}else{
							dst=( dst % sou );							// 結果の書き込み
						}
					}
					break;
				case CAL_EQ:		dst=( dst == sou );	break;			// 等しい
				case CAL_NE:		dst=( dst != sou );	break;			// 等しくない
				case CAL_LE:		dst=( dst <= sou );	break;			// 以下
				case CAL_LT:		dst=( dst  < sou );	break;			// 未満
				case CAL_GE:		dst=( dst >= sou );	break;			// 以上
				case CAL_GT:		dst=( dst  > sou );	break;			// 大きい
				case CAL_ANDAND:	dst=( dst && sou );	break;			// &&
				case CAL_OROR:		dst=( dst || sou );	break;			// ||
			}
			(vp-1)->val=dst;											// 演算結果ストア
			vp--;														// 評価スタックポインタ減算
		}
		vp++;															// 評価スタックポインタ加算
		if( (vp->pri=codp->pri)<0 ){									// テーブル終端の プライオリティ=-1 を見つけた
			break;														// ループ終了
		}
		vp->cal=codp->cal;												// 演算処理アドレス
	}
	*dstp=va[0].val;													// 計算結果の書き込み
	return( (char*)txtp );												// 正常終了
}

