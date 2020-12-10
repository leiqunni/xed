/************************************************************************/
/*																		*/
/*	データベースファイル登録	by H.Watanabe							*/
/*	Ver1.00	2009/04/05 13:52:04	作成									*/
/*																		*/
/************************************************************************/

#	ifndef	_LARGEFILE64_SOURCE
#	define	_LARGEFILE64_SOURCE
#	define	_FILE_OFFSET_BITS	64
#	endif

#	include	<stdio.h>													/* 標準入出力関数*/
#	include	<string.h>													/* 標準文字列関数*/
#	include	<time.h>													/* 時刻関数*/
#	include	<sys/stat.h>												/* ファイルステータス*/
#	ifdef	unix
#		include	<unistd.h>												/* UNIX スタンダード*/
#		include	<dirent.h>												/* ディレクトリ操作関数*/
#		include	<utime.h>
#		ifdef	linux
#			include	<sys/types.h>
#		endif
#	endif
#	ifdef	_WIN32
#		include	<direct.h>
#		include	<io.h>													/* ローレベルファイルハンドラ*/
#		if	defined(__BORLANDC__)										/* ボーランドC++*/
#			include	<utime.h>
#		else
#			include	<sys/utime.h>
#		endif
#	endif
#	include	"path2.h"													/* パス名関連*/
#	include	"filedb.h"													/* データベースファイル登録*/

/************************************************************************/
/*	ローカル定義														*/
/************************************************************************/

#	define	PATHMAX			1024
#	define	NAMMAX			256
#	define	EXTMAX			256

//	漢字チェック

#	define	RNGTSTB(C,L,H)	((unsigned char)((C)-(L))<=(unsigned char)((H)-(L)))				/* 符号無し８ビットでレンジチェックをする*/
#	if	defined(_WIN32) || defined(__CYGWIN__)							/* SJIS*/
#		define	KANJI(C)	(RNGTSTB((C),0x81,0x9f)||RNGTSTB((C),0xe0,0xfc))
#	else																/* EUC*/
#		define	KANJI(C)	(((C)&0x80)>>7)
#	endif

/************************************************************************/
/*																		*/
/*	ファイル( 基本的には file.c として独立させるべきもの )				*/
/*																		*/
/************************************************************************/
/************************************************************************/
/*	パス名の最終ディレクトリセパレータを削除する						*/
/*	WINDOWSが最後のディレクトリセパレータをまともに評価してくれない		*/
/************************************************************************/

static char	*fileCutEndDirSep( char *dstp, const char *txtp ){

	unsigned char	bakchr=0;											// １つ前の文字
	unsigned char	chr=0;												// テキストから読み込んだ文字

	while( *txtp ){
		bakchr=chr;
		chr=*txtp++;
		*dstp++=chr;
		if( KANJI( chr ) && *txtp ){
			*dstp++=*txtp++;
		}
	}
	dstp-=( bakchr&&( chr=='/' || chr=='\\' ) );
	*dstp=0;
	return( dstp );
}

/************************************************************************/
/*	アトリビュート文字列作成											*/
/************************************************************************/

char	*fileStrAtr( char *dstadr, const stFILE *filep ){

	char	*dstp;

	dstp=dstadr;
	if( filep->atr&FILEATR_LNK ){
		*dstp++='l';
	}else{
		*dstp++=(char)((filep->atr&FILEATR_DIR )?'d':'-');
	}
	*dstp++=(char)((filep->atr&FILEATR_RUSR)?'r':'-');
	*dstp++=(char)((filep->atr&FILEATR_WUSR)?'w':'-');
	*dstp++=(char)((filep->atr&FILEATR_XUSR)?'x':'-');
	*dstp++=(char)((filep->atr&FILEATR_RGRP)?'r':'-');
	*dstp++=(char)((filep->atr&FILEATR_WGRP)?'w':'-');
	*dstp++=(char)((filep->atr&FILEATR_XGRP)?'x':'-');
	*dstp++=(char)((filep->atr&FILEATR_ROTH)?'r':'-');
	*dstp++=(char)((filep->atr&FILEATR_WOTH)?'w':'-');
	*dstp++=(char)((filep->atr&FILEATR_XOTH)?'x':'-');
	*dstp=0;
	return( dstadr );
}

/************************************************************************/
/*	タイムスタンプ文字列作成											*/
/************************************************************************/

char	*fileStrTimS( char *dstadr, unsigned long tim2 ){

	struct tm	*tmp;
	time_t		mtim;

	mtim=tim2;
	tmp=localtime( &mtim );
	sprintf( dstadr, "%04d/%02d/%02d %02d:%02d:%02d", tmp->tm_year+1900, tmp->tm_mon+1, tmp->tm_mday, tmp->tm_hour, tmp->tm_min, tmp->tm_sec );
	return( dstadr );
}

/************************************************************************/
/*	タイムスタンプ文字列作成											*/
/************************************************************************/

char	*fileStrTim( char *dstadr, const stFILE *filep ){

	struct tm	*tmp;
	time_t		mtim;

	mtim=filep->mtim;
	tmp=localtime( &mtim );
	sprintf( dstadr, "%04d/%02d/%02d %02d:%02d:%02d", tmp->tm_year+1900, tmp->tm_mon+1, tmp->tm_mday, tmp->tm_hour, tmp->tm_min, tmp->tm_sec );
	return( dstadr );
}

/************************************************************************/
/*	ファイル情報を得る													*/
/************************************************************************/

#	ifdef	_WIN32
#		if	_MSC_VER>1310
#			define	STAT			_stat64
			typedef struct _stat64	stSTAT;
#		else
#			define	STAT			stat
			typedef struct stat		stSTAT;
#		endif
#	else
#		define	STAT			lstat
		typedef struct stat		stSTAT;
#	endif

int		fileStat( stFILE *filep, const char *pacnamp, int fileatr ){

	stSTAT			st;
	stFILE			file;												// ユーザーがバッファを指定しない場合のテンポラリ
	int				res=0;
	int				atr=0;
	char			*namextp;
	int				staterr;
	char			tmpa[PATHMAX];

	if( !filep ){														// ユーザーバッファが指定荒れていない
		filep=&file;													// ユーザーバッファポインタはカレントテンポラリにセット
	}
	fileCutEndDirSep( tmpa, pacnamp );									// パス名の最終ディレクトリセパレータを削除する
	pacnamp=tmpa;
	memset( filep, 0, sizeof(stFILE) );									// メモリクリア
	staterr=STAT( pacnamp, &st );
	if( !staterr ){														// ファイルは存在する
		namextp=pathSeekNam( 0, pacnamp );
		if( namextp[0]=='.' )			atr|=FILEATR_DOT;
		filep->mtim		= (unsigned long)st.st_mtime;					// 最終変更時刻
		filep->atim		= (unsigned long)st.st_atime;					// 最終アクセス時刻
		filep->siz		= (unsigned long)st.st_size;					// ファイルサイズ64ビット
		filep->sizll	= st.st_size;									// ファイルサイズ64ビット
		filep->mode		= st.st_mode;									// モード
#	ifdef	unix
		filep->ino		= st.st_ino;									// iノード番号
		filep->dev		= st.st_dev;									// 装置番号( ファイルシステム )
		filep->rdev		= st.st_rdev;									// 特殊ファイルの装置番号
		filep->nlink	= st.st_nlink;
		filep->uid		= st.st_uid;									// ユーザーID
		filep->gid		= st.st_gid;									// グループID
		filep->blksize	= st.st_blksize;								// ブロックサイズ
		filep->blocks	= st.st_blocks;									// ブロック数
		if( S_ISREG(  st.st_mode ) )	atr|=FILEATR_REG;
		if( S_ISDIR(  st.st_mode ) )	atr|=FILEATR_DIR;
		if( S_ISLNK(  st.st_mode ) )	atr|=FILEATR_LNK;
		if( S_ISFIFO( st.st_mode ) )	atr|=FILEATR_FIFO;
		if( S_ISCHR(  st.st_mode ) )	atr|=FILEATR_CHR;
		if( S_ISBLK(  st.st_mode ) )	atr|=FILEATR_BLK;
		if( S_ISSOCK( st.st_mode ) )	atr|=FILEATR_SOCK;
		if( S_IRUSR&( st.st_mode ) )	atr|=FILEATR_RUSR;
		if( S_IWUSR&( st.st_mode ) )	atr|=FILEATR_WUSR;
		if( S_IXUSR&( st.st_mode ) )	atr|=FILEATR_XUSR;
		if( S_IRGRP&( st.st_mode ) )	atr|=FILEATR_RGRP;
		if( S_IWGRP&( st.st_mode ) )	atr|=FILEATR_WGRP;
		if( S_IXGRP&( st.st_mode ) )	atr|=FILEATR_XGRP;
		if( S_IROTH&( st.st_mode ) )	atr|=FILEATR_ROTH;
		if( S_IWOTH&( st.st_mode ) )	atr|=FILEATR_WOTH;
		if( S_IXOTH&( st.st_mode ) )	atr|=FILEATR_XOTH;
#		ifdef	__sgi													/* SGI IRIX*/
		memcpy( filep->fstype, st.st_fstype, sizeof(filep->fstype) );
		if( !strcmp( st.st_fstype, "nfs" ) ){
			atr|=FILEATR_NFS;
		}
#		endif
#	else
		if( st.st_mode&_S_IFDIR		)	atr|=FILEATR_DIR;				// ディレクトリ
		if( st.st_mode&_S_IFCHR		)	atr|=FILEATR_CHR;				// キャラクタファイル
		if( st.st_mode&_S_IFIFO		)	atr|=FILEATR_FIFO;				// パイプ
		if( st.st_mode&_S_IFREG		)	atr|=FILEATR_REG;				// 通常ファイル
		if( st.st_mode&_S_IREAD		)	atr|=FILEATR_RALL;				// リードパーミッション
		if( st.st_mode&_S_IWRITE	)	atr|=FILEATR_WALL;				// ライトパーミッション
		if( st.st_mode&_S_IEXEC		)	atr|=FILEATR_XALL;				// 実行パーミッション
#	endif
		if( atr&FILEATR_DOT ){											// .で始まるファイル名ならば
			atr&=~FILEATR_REG;											// 通常ファイルアトリビュート解除
		}
		if( atr&FILEATR_LNK ){											// リンクファイル
			atr&=~FILEATR_REG;											// 通常ファイルアトリビュート解除
		}
		if( atr&fileatr ){
			filep->atr=atr;
			res=1;
		}
	}else{
		const char	*p;
		char		chr=0;

		p=pacnamp;
		while( *p ){
			chr=*p++;
		}
		if( chr==':' ){													// ドライブ文字列
			filep->atr=FILEATR_DIR;
			if( fileatr&FILEATR_DIR ){
				res=1;
			}
		}else if( fileatr&FILEATR_NON ){								// 無い場合でも有功
			filep->atr=FILEATR_NON;
			res=1;
		}
	}
	return( res );
}

/************************************************************************/
/*	出力ファイル名作成													*/
/************************************************************************/

void	fileMakDst( char *dstp, const char *sounamp, const char *pathp, const char *extp ){

	stFILE		fil;													// ファイルステータス構造体
	char		path[PATHMAX],nam[NAMMAX],namext[EXTMAX];
	char		*tmpextp;

	if( !extp ){														// 新しい拡張子名は指定されていない
		extp=pathSeekExt( 0, 0, sounamp );								// 拡張氏名はソースファイルの拡張子
	}
	if( !*pathp || fileStat( &fil, pathp, FILEATR_DIR ) ){				// ディストネーションパス名はディレクトリ
		pathSeekExt( 0, nam, sounamp );
	}else{																// ディストネーションパス名はディレクトリではない
		tmpextp=pathSeekExt( path, nam, pathp );						// ディストネーションパス名の拡張子シーク
		if( *tmpextp ){													// ディストネーションパス名に拡張子がある
			extp=tmpextp;												// 拡張子名はディストネーションパス付属のものを使用する
		}
		pathp=path;
	}
	sprintf( namext, "%s%s", nam, extp );								// 名前＋拡張子
	pathCat( dstp, pathp, namext );										// パス名＋名前＋拡張子
}

/************************************************************************/
/*																		*/
/*	DB 登録( 基本的には filedb.c として独立させるべきもの )				*/
/*																		*/
/************************************************************************/
/************************************************************************/
/*	ワイルドカードチェック												*/
/************************************************************************/

int		filedbWildChk( const char *soup, const char *wldp ){

	unsigned int	wld,sou;											// char で良いのだが 高速化・メモリ節約のため int とする

	for(;;){
		wld=*wldp++;													// ワイルドカードを得る
		if( wld=='*' ){													// ワイルドカード
			if( !*wldp ){												// 終端なのでソースは何でも良い
				return( 1 );											// 一致した
			}
			for(;;){
				if( filedbWildChk( soup, wldp ) ){						// 一致した
					return( 1 );										// 一致した
				}
				if( !*soup++ ){											// ソースが終端
					break;												// 一致しない
				}
			}
			break;														// 一致しない
		}
		sou=*soup++;													// ソースから一文字読み込む
		if( !(wld|sou) ){												// 共に０
			return( 1 );												// 一致した
		}
#	ifdef	_WIN32
		if( wld>='A' && wld<='Z' ){										// 大文字である
			wld+=('a'-'A');												// 小文字に変換
		}
		if( sou>='A' && sou<='Z' ){										// 大文字である
			sou+=('a'-'A');												// 小文字に変換
		}
#	endif
		if( wld!='?' && wld!=sou ){
			break;														// 一致しない
		}
	}
	return( 0 );														// 一致しない
}

/************************************************************************/
/*	ファイルDBオープン													*/
/************************************************************************/

stDB	*filedbOpen(void){
	return( dbOpen( DBSIZ_FNAM, sizeof(stFILE), 256, 4096 ) );
}

/************************************************************************/
/*	指定ファイルをDBに登録する( ワイルドカード使用可 )					*/
/************************************************************************/

stDB	*filedbAlc( stDB *dbp, const char *pacnamp, int fileatr ){

	char	dirnam[PATHMAX];											// ディレクトリ名
	char	pacnam[PATHMAX];											// パック名
	stFILE	filedb;														// ファイル情報構造体
	char	*txtp;														// テンポラリテキストポインタ

	if( !dbp ){
		dbp=filedbOpen();
	}
	if( pacnamp ){
		if( !strpbrk( pacnamp, "*?" ) ){								// ワイルドカード不使用
			if( !dbSch( dbp, pacnamp ) ){								// 未登録
				if( fileStat( &filedb, pacnamp, fileatr ) ){			// ファイルステータス取得
					dbAlc( dbp, pacnamp, &filedb );						// 登録
				}
			}
		}else{															// ワイルドカードを使用している

#	ifdef	unix
			DIR				*dirp;										// ディレクトリ管理ポインタ
			struct dirent	*direntp;
			char			*namextp;									// 名前＋拡張子ポインタ

			namextp=pathSeekNam( dirnam, pacnamp );
			if( !dirnam[0] ){											// ディレクトリ名はない
				dirp=opendir( "." );									// カレントディレクトリオープン
			}else{														// ディレクトリ名が存在する
				dirp=opendir( dirnam );									// 指定ディレクトリオープン
			}
			if( dirp ){													// ディレクトリオープン成功
				while( ( direntp=readdir( dirp ) ) ){					// ディレクトリ検査クループ
					txtp=direntp->d_name;								// txtp=名前＋拡張子
					if( !(fileatr&FILEATR_DOTDIR) ){
						while( *txtp=='.' ) txtp++;						// '.'をスキップする
					}
					if( *txtp ){										// ".",".."ではない
						if( filedbWildChk( direntp->d_name, namextp ) ){// ワイルドカードチェック
							pathCat( pacnam, dirnam, direntp->d_name );	// パス名を合成する
							if( !dbSch( dbp, pacnam ) ){				// 未登録
								if( fileStat( &filedb, pacnam, fileatr ) ){		// ファイル情報を得る
									dbAlc( dbp, pacnam, &filedb );		// 登録
								}
							}
						}
					}
				}
				closedir( dirp );										// ディレクトリクローズ
			}
#	else
			long				findh;									// ファインドハンドラ
			struct _finddata_t	finddata;								// ファインドバッファ

			pathSeekNam( dirnam, pacnamp );								// ディレクトリ名を分離しておく
			findh=_findfirst( (char*)pacnamp, &finddata );				// 最初に一致するファイルの検索
			if( findh!=-1 ){											// 成功
				for(;;){
					txtp=finddata.name;									// txtp=名前＋拡張子
					if( !(fileatr&FILEATR_DOTDIR) ){
						while( *txtp=='.' ) txtp++;						// '.'をスキップする
					}
					if( *txtp ){										// ".",".."ではない
						pathCat( pacnam, dirnam, finddata.name );		// パス名を合成する
						if( !dbSch( dbp, pacnam ) ){					// 未登録
							if( fileStat( &filedb, pacnam, fileatr ) ){	// ファイル情報を得る
								dbAlc( dbp, pacnam, &filedb );			// 登録
							}
						}
					}
					if( _findnext( findh, &finddata ) ) break;			// 次に一致するファイル名の検索
				}
				_findclose( findh );									// ファインドハンドラクローズ
			}
#	endif
		}
	}
	return( dbp );
}

/************************************************************************/
/*	DBからFILEDBに登録する												*/
/************************************************************************/

stDB	*filedbAlcDB( const stDB *argdbp, int fileatr ){

	stDB	*dbp=0;
	char	*argp;

	dbFOR( argdbp, argp, char* ){
		dbp=filedbAlc( dbp, argp, fileatr );
	}
	return( dbp );
}

/************************************************************************/
/*	ディレクトリをオープンする											*/
/************************************************************************/

stDB	*filedbDirOpen( const char *dirnamp, int fileatr ){

	stDB	*filedbp;
	char	tmpa[1024];													// ワイルドカード名作成用バッファ

	pathCat( tmpa, dirnamp, "*" );										// ディレクトリ中のファイルを総て登録するためにワイルドカードを作成する
	filedbp=filedbAlc( 0, tmpa, fileatr );								// ディレクトリ中のファイルを総て登録
	return( filedbp );
}

/************************************************************************/
/*	ディレクトリ存在チェック											*/
/************************************************************************/

int		filedbChkDir( const char *dirnamp ){

	struct stat	st;
	int			res=0;

#	ifdef	unix

	if( !stat( dirnamp, &st ) ){										// ファイルは存在する
		res=S_ISDIR( st.st_mode );
	}

#	endif

#	ifdef	_WIN32

	char		tmpa[PATHMAX],*tmpp;
	char		chr=0;

	fileCutEndDirSep( tmpa, dirnamp );									// パス名の最終ディレクトリセパレータを削除する
	tmpp=tmpa;
	while( *tmpp ){
		chr=*tmpp++;
	}
	if( chr==':' ){														// 最終文字が':'なのでドライブ名
		res=1;
	}else{
		if( !stat( tmpa, &st ) ){										// ファイルは存在する
			res=st.st_mode&_S_IFDIR;
		}
	}

#	endif

	return( res );
}

/************************************************************************/
/*	ディレクトリ作成													*/
/************************************************************************/

int		filedbMakDir( const char *dirnamp ){

#	ifdef	_WIN32
	return( _mkdir( dirnamp )==0 );
#	else
	return( mkdir( dirnamp, S_IRWXU|S_IRWXG|S_IRWXO )==0 );
#	endif
}

/************************************************************************/
/*	再帰的にディレクトリ作成											*/
/************************************************************************/

int		filedbMakDirR( const char *dirnamp ){

	int				err=0;												// エラーフラグ( 論理反転したリザルトステータス )
	const char		*txtp;												// テキストポインタ
	unsigned char	chr;												// テキストから得た文字
	char			tmpa[PATHMAX],*tmpp;								// テンポラリ
	int				kanlen;												// 漢字文字列長

	if( dirnamp[0] ){													// ディレクトリ名は有効
		if( !filedbChkDir( dirnamp ) ){									// ディレクトリは存在しない
			txtp=dirnamp;												// ディレクトリ名ポインタ
			tmpp=tmpa;													// テンポラリポインタ
			for(;;){
				chr=*txtp++;											// テキストから１文字得る
				kanlen=KANJI( chr );									// 漢字文字列長を得る
				if( !chr || chr=='/' || chr=='\\' ){					// テキスト終端またはディレクトリセパレータ
					*tmpp=0;											// 一旦０終端とする
					if( tmpa[0] ){										// 有効なディレクトリ名
						if( !filedbChkDir( tmpa ) ){					// ディレクトリは存在しない
							err=!filedbMakDir( tmpa );					// ディレクトリ作成
							if( err ){									// ディレクトリ作成エラー
								break;									// ループ終了
							}
						}
					}
				}
				*tmpp++=chr;											// テンポラリに１文字ストア
				if( !chr ){												// 文字列終端
					break;												// ループ終了
				}
				if( kanlen && *txtp ){
					*tmpp++=*txtp++;
				}
			}
		}
	}
	return( !err );
}

/************************************************************************/
/*	ファイルモード変更													*/
/************************************************************************/

int		filedbChgMod( const char *fnamp, int mtim, int fatr ){

	int				res=1;
	struct utimbuf	utim;

	if( mtim!=-1 ){
		utim.actime		= mtim;
		utim.modtime	= mtim;
		if( utime( fnamp, &utim ) ){
			res=0;														// エラー
		}
	}
	if( fatr!=-1 ){														// ファイルアトリビュートが存在する
#	ifdef	unix

		mode_t	mode=0;

		if( fatr&FILEATR_RUSR ) mode|=S_IRUSR;							// ユーザー読出 許可
		if( fatr&FILEATR_WUSR ) mode|=S_IWUSR;							// ユーザー書込 許可
		if( fatr&FILEATR_XUSR ) mode|=S_IXUSR;							// ユーザー実行 許可
		if( fatr&FILEATR_RGRP ) mode|=S_IRGRP;							// グループ読出 許可
		if( fatr&FILEATR_WGRP ) mode|=S_IWGRP;							// グループ書込 許可
		if( fatr&FILEATR_XGRP ) mode|=S_IXGRP;							// グループ実行 許可
		if( fatr&FILEATR_ROTH ) mode|=S_IROTH;							// 一般	   読出 許可
		if( fatr&FILEATR_WOTH ) mode|=S_IWOTH;							// 一般	   書込 許可
		if( fatr&FILEATR_XOTH ) mode|=S_IXOTH;							// 一般	   実行 許可
		if( chmod( fnamp, mode ) ){										// ファイルモード変更実行
			res=0;														// エラー
		}
#	endif
	}
	return( res );
}

/************************************************************************/
/*	ファイル削除メイン													*/
/************************************************************************/

#	ifndef	STDERR
#		ifdef	_WIN32													/* Windows の場合*/
#			define	STDERR	stdout										/* エラー出力は標準出力( パイプ等で問題が多いため )*/
#		else															/* その他*/
#			define	STDERR	stderr										/* エラー出力は標準エラー出力*/
#		endif
#	endif

#	ifdef	_WIN32
#		define	RMDIR		_rmdir
#	else
#		define	RMDIR		rmdir
#	endif

int		filedbRemove( const char *fnamp, int opta, int optt ){

	int		errn=0;														// エラー数
	stDB	*dirdbp;													// ディレクトリＤＢ
	stFILE	*p;															// テンポラリファイルポインタ
	stFILE	file;

	if( fileStat( &file, fnamp, FILEATR_ALL ) ){
		if( opta && !(file.atr&FILEATR_WUSR) ){							// 書き込み許可がない
			if( !optt ){												// テストモードではない
#	ifdef	unix
				errn=chmod( fnamp, file.mode|FILEATR_WUSR );
#	endif
#	ifdef	_WIN32
				errn=_chmod( fnamp, _S_IWRITE );
#	endif
			}
			if( errn ){
				fprintf( STDERR, "リードオンリーアトリビュート変更エラー:%s\n", fnamp );
			}
		}
		if( file.atr&FILEATR_DIR ){										// ディレクトリ
			dirdbp=filedbDirOpen( fnamp, FILEATR_ALL );					// ディレクトリ中のファイルを総て登録
			dbFOR( dirdbp, p, stFILE* ){
				errn+=!filedbRemove( (char*)dbKEYADR( p ), opta, optt );// 再帰コール
			}
			dbClose( dirdbp );											// ディレクトリＤＢクローズ
			printf( "rmdir	%s\n", fnamp );
			if( !optt ){												// テストモードではない
				errn=RMDIR( fnamp );									// ディレクトリ削除
			}
			if( errn ){
				fprintf( STDERR, "ディレクトリ削除エラー:%s\n", fnamp );
			}
		}else{															// 通常ファイル
			printf( "rm	%s\n", fnamp );
			if( !optt ){												// テストモードではない
				errn=remove( fnamp );									// ファイル削除エラー
			}
			if( errn ){
				fprintf( STDERR, "ファイル削除エラー:%s\n", fnamp );
			}
		}
	}else{
		fprintf( STDERR, "ファイルが見つかりません:%s\n", fnamp );
		errn++;
	}
	return( !errn );													// 0:エラー, 1:正常終了
}

