/************************************************************************/
/*																		*/
/*	数式評価( Evaluation of Expressions )	by H.Watanabe				*/
/*	Ver1.00	2009/04/05 13:52:04	作成									*/
/*																		*/
/************************************************************************/

#	ifndef	EVAL_H
#	define	EVAL_H
#	ifdef	__cplusplus
	extern "C"{
#	endif

#	ifndef	LLONG
#		ifdef	_WIN32
#			define	LLONG		__int64
#			define	LL			"I64"
#			define	LONGLONG(I)	(I##i64)
#		else
#			define	LLONG		long long
#			define	LL			"q"
#			define	LONGLONG(I)	(I##LL)
#		endif
#	endif

#	ifndef	EVALTYP
#		define	EVALTYP			LLONG									/* 型*/
#	endif

#	define	EVALERR_LSR_OF		"左シフト値範囲外"
#	define	EVALERR_LSL_OF		"右シフト値範囲外"
#	define	EVALERR_DIV_0		"０除算"
#	define	EVALERR_DIV_OF		"除算オーバーフロー"
#	define	EVALERR_MOD_0		"０剰余"
#	define	EVALERR_MOD_OF		"剰余オーバーフロー"

	typedef char *tpEVALCB( EVALTYP *dstp, const char *txtp, void *usrp );		// ユーザー処理関数

//	dstp:	処理結果を格納するアドレスです。( 0 の場合はエラー情報です )
//	txtp:	dstp が0以外の場合にはテキストがセットされています。dstp が 0 の場合にはエラー文字列がセットされています。
//	usrp:	ユーザーが指定するポインタです。
//	帰値:	処理したテキストの次を返してください。処理不能だった場合は0を返してください。

	char	*eval( EVALTYP *dstp, const char *txtp, tpEVALCB *valprcp, void *usrp );

//	dstp:	処理結果を格納するアドレスです。
//	txtp:	数式文字列をセットします。
//	valprcp:拡張処理関数アドレスです。必要がなければ0をセットしてください。
//	usrp:	ユーザーが指定するポインタです。必要がなければ0をセットしてください。
//	帰値:	処理したテキストの次が帰ります。エラーの場合は0が帰ります。

#	ifdef	__cplusplus
	}
#	endif
#	endif

