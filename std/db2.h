/************************************************************************/
/*																		*/
/*	プログラム内汎用データベースシステム by H.Watanabe					*/
/*	Ver0.00	2009/01/09 21:15:32	作成開始								*/
/*																		*/
/************************************************************************/

#	ifndef	DB2_H
#	define	DB2_H
#	ifdef	__cplusplus
	extern "C"{
#	endif

#	ifndef	DBALIGN														/* アラインが設定されていない*/
#		if		defined(__MWERKS__)										/* PS2(コードウォーリア)*/
#			define	DBALIGN		16										/* アラインサイズ*/
#		elif	defined(__gdb__) || defined(__MIPSEL__)					/* GreenHills, GCC MIPS3 for TX4923*/
#			define	DBALIGN		8										/* アラインサイズ*/
#		else															/* 特にターゲットの指定は無い*/
#			define	DBALIGN		4										/* アラインサイズ*/
#		endif
#	endif

#	define	DBSIZ_VAR		(-1)										/* 可変サイズ*/
#	define	DBSIZ_STR		(-2)										/* 文字列*/
#	define	DBSIZ_RSV		(-3)										/* 大文字小文字を区別しない文字列*/
#	define	DBSIZ_LBL		(-4)										/* ラベル*/
#	define	DBSIZ_INT		(-5)										/* 数値*/
#	define	DBSIZ_PTR		(-6)										/* ポインタ*/

#	if	defined(_WIN32) || defined(__CYGWIN__)							/* Windows*/
#		define	DBSIZ_FNAM	DBSIZ_RSV									/* 大文字小文字を区別しない*/
#	else																/* unix*/
#		define	DBSIZ_FNAM	DBSIZ_STR									/* 大文字小文字を区別する*/
#	endif

#	define	dbLNKP(P)		(((stDBLNK*)(P))-1)							/* 内部マクロ*/
#	define	dbALG(V)		(((V)+(DBALIGN-1))&((unsigned)(-DBALIGN)))	/* 内部マクロ*/
#	define	dbLSTSIZ(P)		(dbLNKP(P)->lstsiz)							/* ユーザリストサイズを得る*/
#	define	dbKEYSIZ(P)		(dbLNKP(P)->keysiz)							/* ユーザキーサイズを得る*/
#	define	dbKEYADR(P)		((void*)(((char*)(P))+dbALG(dbLSTSIZ(P))))	/* ユーザキーアドレスを得る*/
#	define	dbFOR(H,P,C)	for(P=(C)dbTop(H);P;P=(C)dbNxt(P))			/* ユーザーデータ総当りループ*/
#	define	dbFOR2(H,P,N,C)	for(P=(C)dbTop(H);N=(C)dbNxt(P),P;P=N)		/* ユーザーデータ総当りループ*/
#	define	dbBAK(H,P,C)	for(P=(C)dbEnd(H);P;P=(C)dbBak(P))			/* ユーザーデータ総当りループ逆順*/
#	define	dbBAK2(H,P,N,C)	for(P=(C)dbEnd(H);N=(C)dbBak(P),P;P=N)		/* ユーザーデータ総当りループ逆順*/

	typedef struct tgDB		stDB;										// データベース管理構造体

	typedef struct	tgDBLNK{											// リンク構造体
		struct tgDBLNK		*nxtp, *bakp;								// 双方向リストポインタ
		int					lstsiz;										// ユーザーリストサイズ
		int					keysiz;										// ユーザーキーサイズ
	}	stDBLNK;

	stDB	*dbOpen( int keysiz, int lstsiz, int hashn, int hpsiz );	// データベースをオープンします
	void	dbClose( stDB *dbp );										// データベースをクローズします
	void	*dbAlcS( stDB *dbp, const void *keyadr, int keysiz, const void *lstadr, int lstsiz );				// データベースにデータをフルスペックで登録します
	void	*dbAlc( stDB *dbp, ... );									// データベースにデータを登録します
	void	*dbSch( const stDB *dbp, ... );								// データベースからキーにしたがって検索します
	void	dbFre( stDB *dbp, void *lstp );								// データベースからデータを開放します
	int		dbAlcN( const stDB *dbp );									// データベースに登録されている個数を返します
	void	*dbTop( const stDB *dbp );									// データベースに登録されている先頭データのポインタを返します
	void	*dbEnd( const stDB *dbp );									// データベースに登録されている終端データのポインタを返します
	void	*dbNxt( const void *lstp );									// 次のデータポインタを返します
	void	*dbBak( const void *lstp );									// 前のデータポインタを返します
	void	*dbAryOpen( stDB *dbp, void *lstpa );						// データポインタの配列を作成します
	void	dbAryClose( void *lstpa );									// データポインタの配列を開放します
	void	*dbMemAlc( stDB *dbp, int siz );							// データベースからローカルメモリを確保します( データベースをクローズすると自動的に開放します )
	void	dbMemFre( void *usrp );										// データベースから確保したローカルメモリを開放します
	void	*dbMemAlcClr( stDB *dbp, int siz );							// ローカルメモリ確保をしてクリアする
	void	*dbMemDup( stDB *dbp, const void *bufadr, int bufsiz );		// ローカルメモリ確保をしてコピーする( bufadr が NULL の場合はクリア )
	int		dbMemSiz( const stDB *dbp );								// メモリアロケーションしたサイズ合計を求める
	char	*dbStrAlc( stDB *dbp, const char *strp );					// 文字列登録
	stDB	*dbDup( const stDB *dbp );									// データベースの複製を作成する
	void	*dbLstAdr( const stDB *dbp, int lstno );					// Ｎ番目のリストアドレスを得る
	void	dbMov( stDB *dbp, void *lstp, void *nxtp );					// データベースの登録順番を変更する
	void	*dbSeek( int *seeknp, const void *lstp, int seekofs );		// 指定したオフセット分シークする
	void	*dbSchNxt( const stDB *dbp, const void *lstp );				// 次に一致するものを検索
	void	dbAddDB( stDB *dstdbp, const stDB *adddbp );				// 追加
	void	dbClear( stDB *dbp );										// クリア
	int		dbLstNo( stDB *dbp, const void *lstp );						// リストの番号を得る
	int		dbLblLen( const char *txtadr, int bufsiz );					// 漢字ラベル長を求める
	char	*dbGetLbl( char *bufadr, int bufsiz, const char *txtp );	// 漢字ラベルを得る
	char	*dbGetDQ( char *dstp, int dstsiz, const char *txtp );		// ダブルクォーテーションで囲まれた文字列を得る
	int		dbMemCaseCmp( const void *dstadr, const void *souadr, int siz );	// 大文字小文字を区別しない漢字対応メモリ比較
	int		dbStrCaseCmp( const char *dstp, const char *soup );			// 大文字小文字を区別しない漢字対応文字列比較

#	ifdef	__cplusplus
	}
#	endif
#	endif

