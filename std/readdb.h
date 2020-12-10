/************************************************************************/
/*																		*/
/*	クリップボード等からの行変換	by H.Watanabe						*/
/*	Ver1.00	2009/04/05 13:52:04	作成									*/
/*																		*/
/************************************************************************/

#	ifndef	READDB_H
#	define	READDB_H
#	ifdef	__cplusplus
	extern "C"{
#	endif

#	include	"db2.h"														/* プログラム内汎用データベースシステム*/

	typedef struct tgREADDB	stREADDB;									// 管理構造体定義

	stREADDB	*readdbOpen(void);										// オープン
	stDB	*readdbClose( stREADDB *rhp );								// クローズ
	void	readdbAlc( stREADDB *rhp, const void *bufadr, int bufsiz );	// 登録

#	ifdef	__cplusplus
	}
#	endif
#	endif

