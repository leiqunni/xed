/************************************************************************/
/*																		*/
/*	オプション処理				by H.Watanabe							*/
/*	Ver1.00	2009/04/05 13:52:04	作成									*/
/*																		*/
/************************************************************************/

#	ifndef	OPT_H
#	define	OPT_H

#	include	"db2.h"														/* プログラム内汎用データベースシステム*/

#	ifndef	FPTYP														/* FPTYP が定義されていない*/
#		ifdef	_WIN32													/* Windows の場合*/
#			define	FPTYP	double										/* 単精度を使用すると不思議なことがいろいろ起きるので倍精度を使用する*/
#			define	FPSIZ	8
#		else															/* Windows 以外の場合*/
#			define	FPTYP	float										/* WIN32 以外ではメモリ効率を重視して単精度を使用する*/
#			define	FPSIZ	4
#		endif
#	endif

#	ifndef	STDERR
#		ifdef	_WIN32													/* Windows の場合*/
#			define	STDERR	stdout										/* エラー出力は標準出力( パイプ等で問題が多いため )*/
#		else															/* その他*/
#			define	STDERR	stderr										/* エラー出力は標準エラー出力*/
#		endif
#	endif

	typedef char	*tpOPTIONUSR( void *, char * );						// ユーザー処理関数

	class	cOPT{
	public:
		stDB	*m_argdbp;												// 引数DB
		stDB	*m_optdbp;												// -オプションDB
		stDB	*m_plsdbp;												// +オプションDB
		char	*m_titlep;												// コマンドタイトル
		char	*m_verp;												// バージョン
		char	*m_ownerp;												// オーナー
		char	*m_linep;												// ライン情報
		char	*m_extrap;												// 追加オプション
		stDB	*m_exedbp;												// 一致すれば実行するワイルドカード
		stDB	*m_igndbp;												// 一致すれば無視するワイルドカード
	public:
		cOPT();															// コンストラクタ
		~cOPT();														// デストラクタ
		void	SetTitle( const char *strp );							// タイトルセット
		void	SetVer( const char *strp );								// バージョンセット
		void	SetOwner( const char *strp );							// オーナーセット
		void	SetLine( const char *strp );							// 引数情報セット
		void	SetExtra( const char *strp );							// 拡張情報セット
		void	AllocInc( const char *idp, int *varp, const char *commentp );	// インクリメントオプション登録
		void	AllocInt( const char *idp, int *varp, const char *commentp );	// イントオプション登録
		void	AllocSet( const char *idp, int *varp, int var, const char *commentp );			// セットオプション登録
		void	AllocFPTYP( const char *idp, FPTYP *varp, const char *commentp );				// 浮動小数オプション登録
		void	AllocStr( const char *idp, char *varp, int siz, const char *commentp );			// 文字列オプション登録
		void	AllocDB( const char *idp, stDB **varp, const char *commentp );	// DBオプション登録
		void	AllocDef( const char *idp, stDB **varp, const char *commentp );	// 定義オプション登録
		void	AllocW( const char *idp, const char *idcp );			// -w 処理登録
		void	AllocUsr( const char *idp, tpOPTIONUSR *prcp, void *usrp, const char *commentp );				// ユーザーオプション登録
		void	Usage( const char *cmdp );								// 使用法の表示
		int		optwTst( const char *fnamp, int dirflg );				// -w 処理テスト
		stDB	*PrcLin( char *txtp );									// コマンドライン処理
		stDB	*PrcArg( int argc, char *argv[] );						// 引数処理
	};

#	endif

