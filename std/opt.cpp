/************************************************************************/
/*																		*/
/*	オプション処理				by H.Watanabe							*/
/*	Ver1.00	2009/04/05 13:52:04	作成									*/
/*																		*/
/************************************************************************/

#	include	<stdio.h>													/* 標準入出力関数*/
#	include	<stdlib.h>													/* 標準ライブラリ関数*/
#	include	<string.h>													/* 標準文字列関数*/
#	include	"path2.h"													/* パス名関連*/
#	include	"opt.h"														/* オプション処理*/

/************************************************************************/
/*	ローカル定義														*/
/************************************************************************/

#	define	SKPSP(P)		((P)+strspn(P,"\t "))						/* スキップスペース*/

	enum{																// タイプ定義
		OPTTYP_INC,														// インクリメント
		OPTTYP_SET,														// 値をセットする
		OPTTYP_INT,														// 整数値指定
		OPTTYP_FPTYP,													// 浮動小数指定
		OPTTYP_STRING,													// 文字列指定
		OPTTYP_DB,														// DB指定
		OPTTYP_DEFINE,													// 定義指定
		OPTTYP_USR,														// ユーザー処理指定
		OPTTYP_N
	};

	typedef struct{
		int		typ;													// タイプ
		union{
			int			*intp;											// イント
			FPTYP		*fptypp;										// 浮動小数
			char		*charp;											// 文字列
			void		*voidp;											// ボイド
			stDB		*dbp;											// DB
			tpOPTIONUSR *prcp;											// ユーザー処理関数
		}	u;
		int		extra;													// 拡張データ
		char	*commentp;												// コメントポインタ
	}	stOPT;

	enum{																// 検索タイプ定義
		SCHTYP_PATHNAMEXT,												// パス＋名前＋拡張子指定
		SCHTYP_NAMEXT,													// 名前＋拡張子指定
		SCHTYP_EXT,														// 拡張子のみ指定
		SCHTYP_N
	};

	typedef struct{
		int		schtyp;													// 検索タイプ
	}	stLST;

/************************************************************************/
/*	文字列を数値に変換する												*/
/************************************************************************/

static char	*s2ip( int *valp, const char *txtp, int radix ){

	char	chr;														// テキスト文字
	int		uni;														// 単位
	int		val=0;														// 数値表価値

	for(;;){
		chr=*txtp++;													// テキストポインタから１文字を得る
		if( chr<='9' ){													// '9'以下
			uni=(unsigned char)( chr-'0' );								// 文字から'0'を引いて unsigned char でキャスト
		}else{															// '9'以下ではない
			uni=( (unsigned char)( ( chr|0x20)-'a' ) )+10;				// 文字から'a'を引いて unsigned char でキャストし、10を加算する
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

	static char	*chkoptVal( int *dstp, const char *txtp );

static char	*chkoptUni( int *dstp, const char *txtp ){

	unsigned char	chr;
	const char		*bakp;												// 評価エラーチェックのためのポインタ
	int				val;

	for(;;){															// スキップスペース
		chr=*txtp++;													// テキストポインタから１文字を得る
		if( chr!='+' ){													// '+'ならばループする
			break;
		}
	}
	switch( chr ){
		case '-':														// - 単項演算子
			txtp=chkoptUni( &val, txtp );								// 再帰コール
			*dstp=-val;													// NEG 処理
			break;
		case '(':
			if( ( txtp=chkoptVal( dstp, txtp ) )!=0 ){					// 再帰コール
				if( *txtp++!=')' ){
					return( 0 );										// エラー終了
				}
			}
			break;
		case '\'':
			val=0;
			while( ( chr=*txtp++ )!=0 ){
				if( chr=='\'' ){
					break;
				}
				val=(val<<8)+chr;
			}
			*dstp=val;
			break;
		case '0':														// 先頭文字は'0'
			if( *txtp=='x' || *txtp=='X' ){								// 0x または 0X で始まる１６進数
				txtp=s2ip( dstp, txtp+1, 16 );							// １６進数値評価
			}else{														// ８進数または１０進数の０
				txtp=s2ip( dstp, txtp, 8 );								// ８進数値評価
			}
			break;
		default:														// １０進数または数値でない
			txtp=s2ip( dstp, bakp=txtp-1, 10 );							// １０進数値評価
			if( txtp==bakp ){											// 評価不能.
				return( 0 );
			}
			break;
	}
	return( (char*)txtp );												// テキストポインタを持って正常終了
}

/************************************************************************/
/*	数式評価( Evaluation of Expressions )								*/
/************************************************************************/

	static int chkoptAdd( int dst, int sou ){ return( dst+sou );	}	// 加算
	static int chkoptSub( int dst, int sou ){ return( dst-sou );	}	// 減算
	static int chkoptMul( int dst, int sou ){ return( dst*sou );	}	// 乗算
	static int chkoptDiv( int dst, int sou ){ return( dst/sou );	}	// 除算

static char	*chkoptVal( int *dstp, const char *txtp ){

	struct{																// 演算プライオリティ処理のための評価構造体
		int		pri;													// プライオリティ
		int		(*prcp)( int , int );									// 処理アドレス
		int		val;													// 演算結果
	}			va[2+1],*vp;											// 評価配列と評価スタックポインタ
	int			(*prcp)( int , int );									// 処理アドレス
	int			pri;													// プライオリティ

	vp=va;																// 評価スタックポインタ初期化
	va[0].pri=-2;														// 門番
	for(;;){															// 評価ループ( VC5 バグのため do while の使用禁止 )
		txtp=chkoptUni( &vp->val, txtp );								// 単項式の評価
		if( !txtp ){
			return( 0 );												// エラー終了
		}
		switch( *txtp++ ){												// テキストポインタから二項演算子を得る
			case '+':	pri=12; prcp=chkoptAdd;			break;			// 加算
			case '-':	pri=12; prcp=chkoptSub;			break;			// 減算
			case '*':	pri=13; prcp=chkoptMul;			break;			// 乗算
			case '/':	pri=13; prcp=chkoptDiv;			break;			// 除算
			default:	pri=-1;	prcp=0;			txtp--;	break;			// 終了
		}
		while( vp->pri>=pri ){											// 評価スタックのプライオリティがカレントプライオリティ以上の間ループ
			(vp-1)->val=vp->prcp( (vp-1)->val, vp->val );				// 演算実行
			vp--;														// 評価スタックポインタ減算
		}
		vp++;															// 評価スタックポインタ加算
		vp->prcp=prcp;													// 演算処理アドレス
		if( (vp->pri=pri)<0 ){											// テーブル終端の プライオリティ=-1 を見つけるまで
			break;
		}
	}
	*dstp=va[0].val;													// 計算結果の書き込み
	return( (char*)txtp );												// テキストポインタを持って正常終了
}

/************************************************************************/
/*	コンストラクタ														*/
/************************************************************************/

cOPT::cOPT(){
	m_argdbp=dbOpen( 0, DBSIZ_STR, 0, 256 );							// 引数DB
	m_optdbp=dbOpen( DBSIZ_STR, sizeof(stOPT), 256, 256 );				// -オプションDB
	m_plsdbp=dbOpen( DBSIZ_STR, sizeof(stOPT), 256, 256 );				// +オプションDB
	m_titlep="テスト";													// コマンドタイトル
	m_verp="0.00";														// バージョン
	m_ownerp="H.Watanabe";												// オーナー
	m_linep="";															// ライン情報
	m_extrap=0;															// 追加オプション
	m_exedbp=dbOpen( DBSIZ_STR, sizeof(stLST), 0, 0 );					// 一致すれば実行するワイルドカード
	m_igndbp=dbOpen( DBSIZ_STR, sizeof(stLST), 0, 0 );					// 一致すれば無視するワイルドカード
}

/************************************************************************/
/*	デストラクタ														*/
/************************************************************************/

cOPT::~cOPT(){

	stOPT	*optp;

	dbFOR( m_optdbp, optp, stOPT* ){
		switch( optp->typ ){
			case OPTTYP_DB:
			case OPTTYP_DEFINE:
				dbClose( optp->u.dbp );
				break;
		}
	}
	dbClose( m_optdbp );												// -オプションDB
	dbClose( m_plsdbp );												// +オプションDB
	dbClose( m_argdbp );												// 引数DB
	dbClose( m_exedbp );												// 一致すれば実行するワイルドカード
	dbClose( m_igndbp );												// 一致すれば無視するワイルドカード
}

/************************************************************************/
/*	タイトル設定														*/
/************************************************************************/

void	cOPT::SetTitle( const char *strp ){
	m_titlep=dbStrAlc( m_optdbp, strp );
}

/************************************************************************/
/*	バージョン設定														*/
/************************************************************************/

void	cOPT::SetVer( const char *strp ){
	m_verp=dbStrAlc( m_optdbp, strp );
}

/************************************************************************/
/*	オーナー設定														*/
/************************************************************************/

void	cOPT::SetOwner( const char *strp ){
	m_ownerp=dbStrAlc( m_optdbp, strp );
}

/************************************************************************/
/*	コマンドライン設定													*/
/************************************************************************/

void	cOPT::SetLine( const char *strp ){
	m_linep=dbStrAlc( m_optdbp, strp );
}

/************************************************************************/
/*	拡張オプション設定													*/
/************************************************************************/

void	cOPT::SetExtra( const char *strp ){
	m_extrap=dbStrAlc( m_optdbp, strp );
}

/************************************************************************/
/*	オプション登録														*/
/************************************************************************/

static stOPT	*Alloc( cOPT *optionp, int typ, const char *idp, void *voidp, int extra, const char *commentp ){

	stDB	*optdbp;
	stOPT	*optp;

	if( *idp=='+' ){
		idp++;
		optdbp=optionp->m_plsdbp;
	}else{
		optdbp=optionp->m_optdbp;
	}
	optp=(stOPT*)dbSch( optdbp, idp );
	if( optp ){
		dbFre( optdbp, optp );
	}
	optp=(stOPT*)dbAlc( optdbp, idp, 0 );
	optp->typ=typ;
	optp->u.voidp=voidp;
	optp->extra=extra;
	optp->commentp=dbStrAlc( optdbp, commentp );
	return( optp );
}

/************************************************************************/
/*	インクリメントオプション登録										*/
/************************************************************************/

void	cOPT::AllocInc( const char *idp, int *varp, const char *commentp ){
	Alloc( this, OPTTYP_INC, idp, varp, 0, commentp );
}

/************************************************************************/
/*	イントオプション登録												*/
/************************************************************************/

void	cOPT::AllocInt( const char *idp, int *varp, const char *commentp ){
	Alloc( this, OPTTYP_INT, idp, varp, 0, commentp );
}

/************************************************************************/
/*	セットオプション登録												*/
/************************************************************************/

void	cOPT::AllocSet( const char *idp, int *varp, int var, const char *commentp ){
	Alloc( this, OPTTYP_SET, idp, varp, var, commentp );
}

/************************************************************************/
/*	浮動小数オプション登録												*/
/************************************************************************/

void	cOPT::AllocFPTYP( const char *idp, FPTYP *varp, const char *commentp ){
	Alloc( this, OPTTYP_FPTYP, idp, varp, 0, commentp );
}

/************************************************************************/
/*	文字列オプション登録												*/
/************************************************************************/

void	cOPT::AllocStr( const char *idp, char *varp, int siz, const char *commentp ){
	Alloc( this, OPTTYP_STRING, idp, varp, siz, commentp );
}

/************************************************************************/
/*	DBオプション登録													*/
/************************************************************************/

void	cOPT::AllocDB( const char *idp, stDB **varp, const char *commentp ){
	*varp=dbOpen( 0, DBSIZ_STR, 0, 256 );
	Alloc( this, OPTTYP_DB, idp, *varp, 0, commentp );
}

/************************************************************************/
/*	定義オプション登録													*/
/************************************************************************/

void	cOPT::AllocDef( const char *idp, stDB **varp, const char *commentp ){
	*varp=dbOpen( DBSIZ_STR, DBSIZ_STR, 256, 256 );
	Alloc( this, OPTTYP_DEFINE, idp, *varp, 0, commentp );
}

/************************************************************************/
/*	ユーザー処理オプション登録											*/
/************************************************************************/

void	cOPT::AllocUsr( const char *idp, tpOPTIONUSR *prcp, void *usrp, const char *commentp ){
	Alloc( this, OPTTYP_USR, idp, (void*)prcp, (int)usrp, commentp );
}

/************************************************************************/
/*	ダブルクォーテーションで囲まれた文字列を得る						*/
/************************************************************************/

static char	*getdqc( char *dstp, int dstsiz, const char *txtp ){

	static const char	*sepa[2]={ "\r\n\t ,", "\"" };					// ターミネータ文字列配列
	int					dq;												// ダブルコーテーションフラグ
	const char			*nxtp;											// 次のテキストポインタ
	int					siz;

	dq	=( *txtp=='"' );												// ダブルコーテーションフラグセット
	txtp+=dq;															// もし、先頭がダブルクォーテーションならばテキストポインタをインクリメント
	nxtp=strpbrk( txtp, sepa[dq] );										// セパレータ検索
	if( !nxtp ){														// セパレータが見つからない
		nxtp=txtp+strlen( txtp );										// ネクストポインタを文字列終端にセット
		dq	=0;															// セパレータが見つからない場合はダブルクォーテーション無視
	}
	siz=nxtp-txtp;														// 転送サイズ
	if( siz>dstsiz-1 ){													// バッファサイズ－１より大きい
		siz=dstsiz-1;													// クランプ
	}
	memcpy( dstp, txtp, siz );											// ディストネーションバッファに文字列をコピー
	dstp[siz]=0;														// ディストネーションバッファ０ターミネート
	return( (char*)( nxtp+dq ) );										// テキストポインタを持ってリターン
}

/************************************************************************/
/*	-w オプションチェック												*/
/************************************************************************/

static char	*prcoptw( cOPT *optionp, const char *txtp, int childa ){

	stDB		*dbp;													// セレクト用
	char		tmpa[1024];												// ファイル名用テンポラリ
	stLST		*lstp;													// 登録用ポインタ
	char		*namextp;												// 名前＋拡張子ポインタ
	const char	*nxtp;

	if( !childa ){
		dbp=optionp->m_exedbp;											// 一致すれば実行するワイルドカード
	}else{
		dbp=optionp->m_igndbp;											// 一致すれば無視するワイルドカード
	}
	for(;;){
		txtp=SKPSP( txtp );												// スキップスペース
		txtp=getdqc( tmpa, sizeof(tmpa), txtp );						// ファイル名を得る
		if( tmpa[0] ){
			lstp=(stLST*)dbAlc( dbp, tmpa, 0 );							// 登録
			namextp=pathSeekNam( 0, tmpa );								// ファイル名までテキストポインタを進める
			if( namextp>tmpa ){											// パスが存在する
				lstp->schtyp=SCHTYP_PATHNAMEXT;							// パス＋名前＋拡張子指定
			}else{														// パスは存在しない
				if( *namextp=='.' ){
					lstp->schtyp=SCHTYP_EXT;							// 拡張子指定
				}else{
					lstp->schtyp=SCHTYP_NAMEXT;							// 名前＋拡張子指定
				}
			}
		}
		nxtp=SKPSP( txtp );												// スキップスペース
		if( *nxtp!=',' ){
			break;
		}
		txtp=nxtp+1;
	}
	return( (char*)txtp );												// 帰り値はテキストポインタ
}

static char	*option_w( void *optionp, char *txtp ){
	return( prcoptw( (cOPT*)optionp, txtp, 0 ) );
}

static char	*option_wc( void *optionp, char *txtp ){
	return( prcoptw( (cOPT*)optionp, txtp, 1 ) );
}

/************************************************************************/
/*	-w 処理オプション登録												*/
/************************************************************************/

void	cOPT::AllocW( const char *idp, const char *idcp ){
	Alloc( this, OPTTYP_USR, idp,  (void*)option_w,  (int)this, "一致すれば実行するワイルドカード( default=* )" );
	Alloc( this, OPTTYP_USR, idcp, (void*)option_wc, (int)this, "一致すれば無視するワイルドカード" );
}

/************************************************************************/
/*	使用法リスト														*/
/************************************************************************/

static stDB	*UsageDB( cOPT *optionp ){

	static const char	*stra[]={ "+", "-" };
	stDB				*usagedbp;
	stOPT				*optp;
	char				factor[256];
	char				tmpa[256];
	stDB				*dbpa[2], *dbp;
	int					i;

	dbpa[0]=optionp->m_plsdbp;
	dbpa[1]=optionp->m_optdbp;
	usagedbp=dbOpen( DBSIZ_STR, DBSIZ_STR, 0, 4096 );
	for( i=0; i<2; i++ ){
		dbp=dbpa[i];
		dbFOR( dbp, optp, stOPT* ){
			factor[0]=0;
			switch( optp->typ ){
				case OPTTYP_INT:
					strcpy( factor, "<数値>" );
					break;
				case OPTTYP_FPTYP:
					strcpy( factor, "<浮動小数>" );
					break;
				case OPTTYP_STRING:
				case OPTTYP_DB:
				case OPTTYP_USR:
					strcpy( factor, "<文字列>" );
					break;
				case OPTTYP_DEFINE:
					strcpy( factor, "<文字列>[=<定義>]" );
					break;
			}
			sprintf( tmpa, "%s%s %s", stra[i], (char*)dbKEYADR( optp ), factor );
			dbAlc( usagedbp, tmpa, optp->commentp );
		}
	}
	return( usagedbp );
}

/************************************************************************/
/*	使用法表示															*/
/************************************************************************/

void	cOPT::Usage( const char *cmdp ){

	stDB	*usagedbp;													// 使用法リスト
	char	*commentp;													// コメント
	char	*factorp;													// 引数
	int		factorlen=0;												// 引数文字列長最大
	int		len;														// 引数文字列長

	usagedbp=UsageDB( this );											// 使用法リスト
	fprintf( STDERR, "\n" );
	fprintf( STDERR, "%s Ver%s by %s\n", m_titlep, m_verp, m_ownerp );
	fprintf( STDERR, "\n" );
	if( dbAlcN( usagedbp ) ){
		fprintf( STDERR, "使用法:%s [<オプション>] %s\n", cmdp, m_linep );
		fprintf( STDERR, "\n" );
		dbFOR( usagedbp, commentp, char* ){
			factorp=(char*)dbKEYADR( commentp );
			len=strlen( factorp );
			if( factorlen<len ){
				factorlen=len;
			}
		}
		dbFOR( usagedbp, commentp, char* ){
			factorp=(char*)dbKEYADR( commentp );
			fprintf( STDERR, "    %-*s: %s\n", factorlen+1, factorp, commentp );
		}
	}else{
		fprintf( STDERR, "使用法:%s %s\n", cmdp, m_linep );
	}
	if( m_extrap ){
		fprintf( STDERR, "%s", m_extrap );
	}
#	ifndef	_WIN32
	fprintf( STDERR, "\n" );
#	endif
	dbClose( usagedbp );
}

/************************************************************************/
/*	ワイルドカードチェック												*/
/************************************************************************/

static int	wildchk( const char *txtp, const char *wildp ){

	int		card, chr;													// char で良いのだが 高速化・メモリ節約のため int とする

	for(;;){
		card=*wildp++;
		chr=*txtp++;
		if( !card && !chr ) return( 1 );								// 一致
		if( card=='*' ){
			txtp--;
			for(;;){
				if( wildchk( txtp, wildp ) ) return( 1 );				// 一致
				if( !*txtp ) return( 0 );								// 不一致
				txtp++;
			}
		}
#	ifdef	_WIN32
		if( card>='A' && card<='Z' ){
			card|='a'-'A';
		}
		if( chr>='A' && chr<='Z' ){
			chr|='a'-'A';
		}
#	endif
		if( card!='?' && card!=chr ) break;								// 不一致
	}
	return( 0 );														// 不一致
}

/************************************************************************/
/*	ワイルドカードチェック												*/
/************************************************************************/

static int	wildchkdb( const stDB *wilddbp, const char *fnamp ){

	stLST		*lstp;
	const char	*schpa[SCHTYP_N];

	schpa[SCHTYP_PATHNAMEXT]=fnamp;										// パス＋名前＋拡張子指定
	schpa[SCHTYP_NAMEXT]=pathSeekNam( 0, fnamp );						// 名前＋拡張子指定
	schpa[SCHTYP_EXT]=pathSeekExt( 0, 0, fnamp );						// 拡張子のみ指定
	dbFOR( wilddbp, lstp, stLST* ){
		if( wildchk( schpa[lstp->schtyp], (char*)dbKEYADR( lstp ) ) ){	// 一致した
			return( 1 );
		}
	}
	return( 0 );														// 一致しない
}

/************************************************************************/
/*	条件判定															*/
/************************************************************************/

int		cOPT::optwTst( const char *fnamp, int dirflg ){

	if( wildchkdb( m_igndbp, fnamp ) ){									// 一致すれば無視するワイルドカード
		return( 0 );													// 条件不成立
	}
	if( dirflg ){														// ディレクトリの場合
		return( 1 );
	}
	if( !dbAlcN( m_exedbp ) || wildchkdb( m_exedbp, fnamp ) ){			// 一致すれば実行するワイルドカード
		return( 1 );													// 条件成立
	}
	return( 0 );														// 条件不成立
}

/************************************************************************/
/*	qsort のためのコールバック関数										*/
/************************************************************************/

static int	qsortCalBak( const void *dstv, const void *souv ){

	const stOPT		*dstoptp=*(const stOPT**)dstv;
	const stOPT		*souoptp=*(const stOPT**)souv;
	char			*dstnamp;
	char			*sounamp;
	int				sou, dst;

	dstnamp=(char*)dbKEYADR( dstoptp );
	sounamp=(char*)dbKEYADR( souoptp );
	dst=strlen( dstnamp );
	sou=strlen( sounamp );
	return( sou-dst );													// 長い順
}

/************************************************************************/
/*	チェックオプション													*/
/************************************************************************/

stDB	*cOPT::PrcLin( char *txtp ){

	stOPT		**optpp, **plspp, **pp, *optp;							// オプションポインタ
	char		*idp=0;													// オプション文字列ポインタ
	char		*errtxtp;												// エラーテキストポインタ
	char		*nxtp;													// テキストポインタワーク
	char		tmpa[256];												// テンポラリバッファ
	char		def[256];												// 定義バッファ
	int			i;														// ループカウンタ

	optpp=(stOPT**)dbAryOpen( m_optdbp, 0 );							// -オプション
	plspp=(stOPT**)dbAryOpen( m_plsdbp, 0 );							// +オプション
	qsort( optpp, dbAlcN( m_optdbp ), sizeof(stOPT*), qsortCalBak );	// 文字列の長い順に並べる
	qsort( plspp, dbAlcN( m_plsdbp ), sizeof(stOPT*), qsortCalBak );	// 文字列の長い順に並べる
	for(;;){															// オプション解析ループ
		txtp=errtxtp=SKPSP( txtp );										// スペース、タブのスキップ
		if( !*txtp ){													// 終端発見
			break;
		}
		pp=0;
		if( *txtp=='-' ){
			pp=optpp;
		}else if( *txtp=='+' ){
			pp=plspp;
		}
		if( pp ){														// '-'または'+' 
			txtp++;														// '-'または'+' スキップ
			for(;;){													// 連続オプション文字解析ループ
				for( i=0; ( optp=pp[i] )!=0; i++ ){
					idp=(char*)dbKEYADR( optp );
					if( !memcmp( txtp, idp, strlen( idp ) ) ){
						break;
					}
				}
				if( !optp ){											// 見つからなかった
					nxtp=SKPSP( txtp );									// スペース、タブのスキップ
					if( *nxtp ){
						fprintf( STDERR, "オプションエラー:%s\n", errtxtp );
					}
					return( 0 );
				}
				txtp+=strlen( idp );									// オプション文字列サイズ分シーク
				nxtp=SKPSP( txtp );										// スペース、タブのスキップ
				switch( optp->typ ){
					case OPTTYP_INC:									// インクリメント
						(*optp->u.intp)++;
						break;
					case OPTTYP_SET:									// 値セット
						*optp->u.intp=optp->extra;
						break;
					case OPTTYP_INT:									// 数値
						txtp=chkoptVal( optp->u.intp, nxtp );			// 数式評価
						if( txtp<=nxtp ){
							fprintf( STDERR, "数式シンタックスエラー:%s\n", errtxtp );
							return( 0 );
						}
						break;
					case OPTTYP_FPTYP:									// 浮動小数
						*optp->u.fptypp=(FPTYP)strtod( nxtp, (char**)&txtp );
						if( txtp<=nxtp ){
							fprintf( STDERR, "浮動小数シンタックスエラー:%s\n", errtxtp );
							return( 0 );
						}
						break;
					case OPTTYP_STRING:									// 文字列
						if( *nxtp && *nxtp!='-' ){						// オプション指定文字ではない
							txtp=dbGetDQ( optp->u.charp, optp->extra, nxtp );	// ファイル名を得る
						}else{
							strcpy( optp->u.charp, "\x1" );
						}
						break;
					case OPTTYP_DB:										// DB
						if( *nxtp!='-' ){								// オプション指定文字ではない
							txtp=dbGetDQ( tmpa, sizeof(tmpa), nxtp );	// 文字列を得る
						}else{
							tmpa[0]=0;
						}
						dbAlc( optp->u.dbp, tmpa, 0 );					// 登録
						break;
					case OPTTYP_DEFINE:									// 定義
						txtp=dbGetLbl( tmpa, sizeof(tmpa), nxtp );		// 漢字ラベルを得る
						if( !tmpa[0] ){									// ラベルが存在する
							fprintf( STDERR, "ラベルがありません:%s\n", txtp );
							return( 0 );								// エラー終了
						}
						nxtp=SKPSP( txtp );
						if( *nxtp=='=' ){
							txtp=dbGetDQ( def, sizeof(def), SKPSP( nxtp+1 ) );	// コマンドラインの文字列を得る
						}else{
							strcpy( def, "1" );
						}
						dbAlc( optp->u.dbp, tmpa, def );				// 登録
						break;
					case OPTTYP_USR:									// ユーザー処理
						txtp=optp->u.prcp( (void*)optp->extra, nxtp );	// コールバック
						if( !txtp ){
							return( 0 );								// エラー終了
						}
						break;
				}
				if( *txtp<=' ' ){										// ０、スペース、タブの場合
					break;												// オプションの連続処理を抜ける
				}
			}
		}else{
			txtp=dbGetDQ( tmpa, sizeof(tmpa), txtp );
			dbAlc( m_argdbp, tmpa );
		}
	}
	dbAryClose( optpp );
	dbAryClose( plspp );
	return( m_argdbp );
}

/************************************************************************/
/*	arg からコマンドラインへ変換										*/
/************************************************************************/

static char	*arg2lin( int argc, char *argv[] ){

	char			*linalc=0;											// メモリ確保ポインタ
	char			*argp;												// アーギュメントポインタ
	int				linsiz=0;											// -W4 ワーニング対策
	int				i;
	int				envsiz;
	char			*envp;
	char			tmpa[256];
	unsigned int	chr;
	int				pass;												// パス
	int				dq;													// スペースを含んでおり"で始まっていない

	for( pass=0; pass<2; pass++ ){										// ２回ループする
		linsiz=0;														// 文字数リセット
		for( i=1; i<argc; i++ ){										// 引数個数分ループ
			argp=argv[i];												// 引数ポインタを得る
			dq=( strpbrk( argp, "\t " )!=0 );							// スペース又はタブを含んでいる
			if( dq ){
				if( pass ){												// PASS1
					linalc[linsiz]='"';									// １文字ストア
				}
				linsiz++;												// 文字数加算
			}
			while( ( chr=*argp++ )!=0 ){								// 引数ポインタから１文字を得る
				if( chr=='$' ){											// 環境変数指定識別子
					argp=dbGetLbl( tmpa+1, sizeof(tmpa)-1, argp );		// 漢字ラベルを得る
					envp=getenv( tmpa+1 );								// 環境変数のポインタを得る
					if( !envp ){										// 見つからなかった
						tmpa[0]='$';
						envp=tmpa;										// 置換文字列はオリジナル文字列
					}
					envsiz=strlen( envp );								// 置換文字列サイズ
					if( pass ){											// PASS1
						memcpy( linalc+linsiz, envp, envsiz );			// 置換文字列コピー
					}
					linsiz+=envsiz;										// 文字数加算
				}else{
					if( pass ){											// PASS1
						linalc[linsiz]=(char )chr;						// １文字ストア
					}
					linsiz++;											// 文字数加算
				}
			}
			if( dq ){
				if( pass ){												// PASS1
					linalc[linsiz]='"';									// １文字ストア
				}
				linsiz++;												// 文字数加算
			}
			if( pass ){													// PASS1
				linalc[linsiz]=' ';										// スペースストア
			}
			linsiz++;													// 文字数加算
		}
		if( !pass ){													// PASS0
			linalc=(char*)malloc( linsiz+1 );							// ０ターミネートするため＋１する
		}
	}
	linalc[linsiz]=0;													// ０ターミネート
	return( linalc );													// メモリ確保したポインタをもってリターンする
}

/************************************************************************/
/*	チェックオプション													*/
/************************************************************************/

stDB	*cOPT::PrcArg( int argc, char *argv[] ){

	stDB		*argdbp;
	char		*txtalc;												// テキストポインタ

	txtalc=arg2lin( argc, argv );
	argdbp=PrcLin( txtalc );
	free( txtalc );
	return( argdbp );
}

