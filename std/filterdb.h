/************************************************************************/
/*																		*/
/*	フィルター処理				by H.Watanabe							*/
/*	Ver1.00	2009/04/05 13:52:04	作成									*/
/*																		*/
/************************************************************************/

#	ifndef	FILTERDB_H
#	define	FILTERDB_H
#	ifdef	__cplusplus
	extern "C"{
#	endif

#	include	"db2.h"														/* プログラム内汎用データベースシステム*/

	stDB	*filterdb( const char *cmdlin, const stDB *indbp );

#	ifdef	__cplusplus
	}
#	endif
#	endif

