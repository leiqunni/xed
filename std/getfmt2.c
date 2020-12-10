/************************************************************************/
/*																		*/
/*	フォーマット付き文字列解析	by H.Watanabe							*/
/*	Ver1.00	2009/04/05 13:52:04	作成									*/
/*																		*/
/************************************************************************/

#	include	<stdarg.h>													/* 可変引数処理定義*/
#	include	<stdlib.h>													/* 標準ライブラリ関数*/
#	include	<string.h>													/* 標準文字列関数*/
#	include	"db2.h"														/* プログラム内汎用データベースシステム*/
#	include	"getfmt2.h"													/* フォーマット付き文字列解析*/

/************************************************************************/
/*	フォーマット付き文字列解析											*/
/************************************************************************/

char	*getfmt( const char *txtp, const char *format, ... ){

    va_list			ap;													// 可変引数ワーク
	unsigned char	cmd;												// フォーマットコマンド
	unsigned char	uni;												// 数値評価ワーク
	const char		*bakp;												// テキストポインタバックアップ
	int				bufsiz;												// バッファサイズ

	va_start( ap, format );												// 可変引数オープン
	while( txtp ){														// エラーが起こればブレーク
		cmd=*format++;													// フォーマット文字列からコマンドを得る
		if( !cmd ){														// フォーマット文字列終端
			break;														// ループ終了
		}
		switch( cmd ){													// フォーマットコマンド文字列による分岐
			case ' ':													// スキップスペース
				txtp+=strspn( txtp, "\t\r\n " );						// 第二引数で指定した文字群にない文字が第一引数内に最初に現れた位置のインデックスを返します。
				break;													// switch 終了
			case '%':
				bufsiz=0;												// バッファサイズクリア
				while( ( uni=(unsigned char)( ( cmd=*format++ )-'0' ) )<=9 ){	// フィールドサイズ読み込み
					bufsiz=bufsiz*10+uni;								// １０倍して加算する
				}
				if( cmd=='*' ){											// フィールドサイズは可変引数から得る
					bufsiz=va_arg( ap, int );							// 可変引数から int 値を読み込み
					cmd=*format++;										// 次の制御文字を得る
				}
				if( !bufsiz ){											// バッファサイズは指定されていない
					bufsiz=256;											// デフォルトサイズ
				}
				switch( cmd ){
					case 'x':											// １６進数
					case 'd':{											// １０進数

						int		val;									// 数値評価ワーク
						int		*ip;									// デストネーション数値ポインタ

						val=strtol( bakp=txtp, (char**)&txtp, (cmd=='x')?16:10 );				// 文字列を数値に変換する
						if( txtp==bakp ){								// フォーマットエラー
							txtp=0;										// エラーセット
							break;										// switch 終了
						}
						if( ( ip=va_arg( ap, int * ) )!=0 ){			// 可変引数を得る( ディストネーションポインタ )
							*ip=val;
						}
						break;											// switch 終了
					}
					case 'f':{											// 浮動小数

						FPTYP	val;
						FPTYP	*fp;

						val	=strtod( bakp=txtp, (char**)&txtp );
						if( txtp<=bakp ){
							txtp=0;										// エラーセット
							break;										// switch 終了
						}
						if( ( fp=va_arg( ap, FPTYP * ) )!=0 ){			// 可変引数を得る( ディストネーションポインタ )
							*fp=val;
						}
						break;											// switch 終了
					}
					case 's':{											// 文字列

						int		len;									// 文字列長
						char	*cp;									// デストネーション文字列ポインタ

						bakp=txtp;										// テキストポインタバックアップセット
						if( *format==' ' ){								// スペース
							txtp=strpbrk( txtp, "\t\r\n " );			// タブ、改行、スペースを検索する
						}else{											// スペース以外
							txtp=strchr( txtp, *format );				// 第一引数中で 第二引数が最初に現れた位置へのポインタを返します。文字が見つからないと NULL を返します。
						}
						if( !txtp ){									// 見つからない場合
							txtp=bakp+strlen( bakp );					// 文字列終端にシークする
						}
						len=txtp-bakp;
						if( len>bufsiz-1 ){
							len=bufsiz-1;
						}
						if( ( cp=va_arg( ap, char * ) )!=0 ){			// 可変引数を得る( ディストネーションポインタ )
							memcpy( cp, bakp, len );					// 文字列をコピー
							cp[len]=0;									// ０ターミネート
						}
						break;											// switch 終了
					}
					case 'l':{											// ラベル

						int		len;									// ラベル長
						char	*cp;									// デストネーション文字列ポインタ

						len=dbLblLen( txtp, bufsiz );					// 漢字ラベル長を求める
						if( ( cp=va_arg( ap, char * ) )!=0 ){			// 可変引数を得る( ディストネーションポインタ )
							memcpy( cp, txtp, len );					// 文字列をコピー
							cp[len]=0;									// ０ターミネート
						}
						txtp+=len;
						break;											// switch 終了
					}
					case 'q':{											// ダブルコーテーション文字列

						char	*cp;									// デストネーション文字列ポインタ

						cp=va_arg( ap, char * );						// 可変引数を得る( ディストネーションポインタ )
						txtp=dbGetDQ( cp, bufsiz, txtp );				// ダブルクォーテーションで囲まれた文字列を得る
						break;											// switch 終了
					}
					default:{											// 認識できないキャラクタ( システムエラー )
						if( *txtp++=='%' ){								// コマンド以外
							format--;									// 一文字分戻す
						}else{											// 一致しない
							txtp=0;										// エラーセット
						}
						break;											// switch 終了
					}
				}
				break;													// switch 終了
			default:
				if( cmd!=*txtp++ ){										// 違う文字
					txtp=0;												// エラーセット
				}
				break;													// switch 終了
		}
	}
	va_end( ap );														// 可変引数クローズ
	return( (char*)( txtp ) );											// テキストポインタを持ってリターン
}

