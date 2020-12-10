/************************************************************************/
/*																		*/
/*	データベースファイル登録	by H.Watanabe							*/
/*	Ver1.00	2009/04/05 13:52:04	作成									*/
/*																		*/
/************************************************************************/

#	ifndef	FILEDB_H
#	define	FILEDB_H
#	ifdef	__cplusplus
	extern "C"{
#	endif

#	include	"db2.h"														/* プログラム内汎用データベースシステム*/

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

#	define	FILEATR_DIR			(1<<0)									/* ディレクトリ*/
#	define	FILEATR_REG			(1<<1)									/* 通常ファイル*/
#	define	FILEATR_SYS			(1<<2)									/* システムファイル*/
#	define	FILEATR_NON			(1<<3)									/* 存在しないファイル*/
#	define	FILEATR_LNK			(1<<4)									/* リンクファイル( UNIX のみ )*/
#	define	FILEATR_WLD			(1<<5)									/* ワイルドカードが使用されている*/
#	define	FILEATR_DOT			(1<<6)									/* . ではじまるファイル( UNIX のみ )*/
#	define	FILEATR_FIFO		(1<<7)									/* FIFO ファイル( UNIX のみ )*/
#	define	FILEATR_CHR			(1<<8)									/* キャラクタデバイスファイル( UNIX のみ )*/
#	define	FILEATR_BLK			(1<<9)									/* ブロックデバイスファイル( UNIX のみ )*/
#	define	FILEATR_SOCK		(1<<10)									/* ソケットファイル( UNIX のみ )*/
#	define	FILEATR_NFS			(1<<11)									/* NFS ファイル*/
#	define	FILEATR_RUSR		(1<<16)									/* ユーザーリード許可*/
#	define	FILEATR_WUSR		(1<<17)									/* ユーザーライト許可*/
#	define	FILEATR_XUSR		(1<<18)									/* ユーザー実行	 許可*/
#	define	FILEATR_RGRP		(1<<19)									/* グループリード許可*/
#	define	FILEATR_WGRP		(1<<20)									/* グループライト許可*/
#	define	FILEATR_XGRP		(1<<21)									/* グループ実行	 許可*/
#	define	FILEATR_ROTH		(1<<22)									/* 誰でも  リード許可*/
#	define	FILEATR_WOTH		(1<<23)									/* 誰でも  ライト許可*/
#	define	FILEATR_XOTH		(1<<24)									/* 誰でも  実行	 許可*/
#	define	FILEATR_DOTDIR		(1<<25)									/* . .. も登録する*/

#	define	FILEATR_ALL			(FILEATR_DIR|FILEATR_REG|FILEATR_SYS|FILEATR_NON|FILEATR_LNK|FILEATR_WLD|FILEATR_DOT|FILEATR_FIFO|FILEATR_CHR|FILEATR_BLK|FILEATR_SOCK|FILEATR_NFS)
#	define	FILEATR_IO			(FILEATR_FIFO|FILEATR_CHR|FILEATR_BLK|FILEATR_SOCK)
#	define	FILEATR_RALL		(FILEATR_RUSR|FILEATR_RGRP|FILEATR_ROTH)/* 総て	 リード許可*/
#	define	FILEATR_WALL		(FILEATR_WUSR|FILEATR_WGRP|FILEATR_WOTH)/* 総て	 ライト許可*/
#	define	FILEATR_XALL		(FILEATR_XUSR|FILEATR_XGRP|FILEATR_XOTH)/* 総て	 実行  許可*/
#	define	FILEATR_REGDIR		(FILEATR_REG|FILEATR_DIR)

	typedef struct{
		long			atr;											// ファイルアトリビュート
		unsigned long	mtim;											// 最終修正タイムスタンプ
		unsigned long	atim;											// 最終アクセスタイム
		unsigned long	siz;											// ファイルサイズ
		LLONG			sizll;											// ファイルサイズ
		long			mode;											// モード
#	ifdef	unix
		long			ino;											// iノード番号
		long			dev;											// 装置番号( ファイルシステム )
		long			rdev;											// 特殊ファイルの装置番号
		long			nlink;											// リンクカウント
		long			uid;											// ユーザーID
		long			gid;											// グループID
		long			blksize;										// ブロックサイズ
		long			blocks;											// ブロック数
		char			fstype[16];										// ファイルシステム名
#	endif
	}	stFILE;

	char	*fileStrAtr( char *dstadr, const stFILE *filep );			// アトリビュート文字列作成
	char	*fileStrTimS( char *dstadr, unsigned long tim2 );			// タイムスタンプ文字列作成
	char	*fileStrTim( char *dstadr, const stFILE *filep );			// タイムスタンプ文字列作成
	int		fileStat( stFILE *filep, const char *pacnamp, int fileatr );// ファイル情報を得る
	void	fileMakDst( char *dstp, const char *sounamp, const char *pathp, const char *extp );	// 出力ファイル名作成
	int		filedbWildChk( const char *soup, const char *wldp );		// ワイルドカードチェック
	stDB	*filedbOpen(void);											// ファイルDBオープン
	stDB	*filedbAlc( stDB *dbp, const char *pacnamp, int fileatr );	// 指定ファイルをDBに登録する( ワイルドカード使用可 )
	stDB	*filedbAlcDB( const stDB *argdbp, int fileatr );			// DBから登録する
	stDB	*filedbDirOpen( const char *dirnamp, int fileatr );			// ディレクトリをオープンする
	int		filedbChkDir( const char *dirnamp );						// ディレクトリ存在チェック
	int		filedbMakDir( const char *dirnamp );						// ディレクトリ作成
	int		filedbMakDirR( const char *dirnamp );						// 再帰的にディレクトリ作成
	int		filedbChgMod( const char *fnamp, int mtim, int fatr );		// ファイルモード変更
	int		filedbRemove( const char *fnamp, int opta, int optt );		// ファイル又はディレクトリ削除

#	ifdef	__cplusplus
	}
#	endif
#	endif

