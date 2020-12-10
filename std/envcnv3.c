/************************************************************************/
/*																		*/
/*	環境変数変換				by H.Watanabe							*/
/*	Ver1.00	2009/04/05 13:52:04	作成									*/
/*																		*/
/************************************************************************/

#	include	<stdlib.h>													/* 標準ライブラリ関数*/
#	include	<string.h>													/* 標準文字列関数*/
#	include	"envcnv3.h"													/* 環境変数変換*/

/************************************************************************/
/*	ローカル定義														*/
/************************************************************************/

#	define	RNGCHK(C,L,H)	((unsigned)((C)-(L))<=(unsigned)((H)-(L)))
#	define	LBLTOPCHR(C)	(RNGCHK(((C)|0x20),'a','z')||((C)=='_'))
#	define	LBLNXTCHR(C)	(RNGCHK((C),'0','9')||LBLTOPCHR(C))

/************************************************************************/
/*	環境変数変換														*/
/************************************************************************/

void	envcnv( char *dstp, const char *txtp ){

	char			*envp;
	char			stra[4096], *strp;
	char			tmpa[4096];
	unsigned int	chr;

	strcpy( tmpa, txtp );
	txtp=tmpa;
	while( ( chr=*txtp++ )!=0 ){										// 引数ポインタから１文字を得る
		if( chr=='$' ){													// 環境変数指定識別子
			strp=stra+1;												// 有効ラベル先頭は str+1
			stra[0]=(char)chr;											// 後処理のため先頭は'$'をセット
			if( LBLTOPCHR( *txtp ) ){									// ラベル先頭に使用できる文字
				for(;;){												// ラベルを得るループ
					*strp++=*txtp++;									// 文字コピー
					if( !LBLNXTCHR( *txtp ) ){							// ラベル中で使用できない文字が現れた
						break;
					}
				}
			}
			*strp=0;													// ラベル終端を０ターミネート
			envp=getenv( stra+1 );										// 環境変数のポインタを得る
			if( !envp ){												// 見つからなかった
				envp=stra;												// 置換文字列はオリジナル文字列
			}
			strcpy( dstp, envp );
			dstp+=strlen( dstp );
		}else{
			*dstp++=(char)chr;
		}
	}
	*dstp=0;															// ０ターミネート
}

