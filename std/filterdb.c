/************************************************************************/
/*																		*/
/*	フィルター処理				by H.Watanabe							*/
/*	Ver1.00	2009/04/05 13:52:04	作成									*/
/*																		*/
/************************************************************************/

#	include	<stdio.h>													/* 標準入出力関数*/
#	include	<stdlib.h>													/* 標準ライブラリ関数*/
#	include	"lin2arg2.h"												/* コマンドラインから引数配列への変換*/
#	include	"readdb.h"													/* クリップボード等からの行変換*/
#	include	"filterdb.h"												/* フィルター処理*/

/************************************************************************/
/*	ローカル定義														*/
/************************************************************************/

	enum{
		PIPE_READ,														// リード
		PIPE_WRITE,														// ライト
		PIPE_N
	};
	enum{
		IO_IN,															// 入力
		IO_OUT,															// 出力
		IO_N
	};

#	ifdef	unix

/************************************************************************/
/*																		*/
/*	UNIX																*/
/*																		*/
/************************************************************************/

#	include	<signal.h>													/* signal処理*/
#	include	<unistd.h>													/* UNIX スタンダード*/
#	include	<sys/wait.h>

/************************************************************************/
/*	シグナルエラー														*/
/************************************************************************/

static void sig_pipe( int signo ){

	if( signo!=13 ){													// 誰も何も読まなかったではない
		fprintf( stderr, "SIGPIPE caught %d\n", signo );
	}
	exit( 1 );
}

/************************************************************************/
/*	フィルター処理( unix )												*/
/************************************************************************/

stDB	*filterdb( const char *cmdlin, const stDB *indbp ){

	stDB		*outdbp=0;												// 出力DB
	char		tmpa[0x10000];											// テンポラリバッファ
	int			io[IO_N][PIPE_N];										// in,out は親から見たもの
	pid_t		pid, pid2;												// プロセスIS
	int			linlen;													// 行サイズ
	char		**argvp;												// 引数配列ポインタ
	char		*inp;													// 入力ポインタ
	int			readn;													// リードサイズ
	stREADDB	*rhp;													// リードDBポインタ

	if( signal( SIGPIPE, sig_pipe )==SIG_ERR ){							// シグナルセット
		fprintf( stderr, "signal error" );
		return( 0 );
	}
	if( pipe( io[IO_OUT] )<0 || pipe( io[IO_IN] )<0 ){					// パイプオープン
		fprintf( stderr, "pipe error" );
		return( 0 );
	}
	if( (pid=fork())<0 ){												// 子プロセス生成
		fprintf( stderr, "fork error" );
		return( 0 );
	}
	if( pid ){															// 親
		close( io[IO_OUT][PIPE_READ] );									// 出力リードクローズ
		close( io[IO_IN][PIPE_WRITE] );									// 入力ライトクローズ
		if( (pid2=fork())<0 ){											// 子プロセス生成
			fprintf( stderr, "fork error" );
			return( 0 );
		}
		if( !pid2 ){													// 子(出力)
			close( io[IO_IN][PIPE_READ] );								// 入力リードクローズ
			dbFOR( indbp, inp, char* ){									// 入力DB総当りループ
				linlen=sprintf( tmpa, "%s\n", inp );					// 行サイズ
				if( write( io[IO_OUT][PIPE_WRITE], tmpa, linlen )!=linlen ){
					fprintf( stderr, "write error\n" );
					exit( 0 );
				}
			}
			close( io[IO_OUT][PIPE_WRITE] );							// 出力ライトクローズ
			exit( 0 );													// 子終了
		}
		close( io[IO_OUT][PIPE_WRITE] );								// 出力ライトクローズ
		rhp=readdbOpen();												// リードDBオープン
		for(;;){
			readn=read( io[IO_IN][PIPE_READ], tmpa, sizeof(tmpa) );
			if( !readn ){
				break;
			}
			readdbAlc( rhp, tmpa, readn );								// リードDB登録
		}
		outdbp=readdbClose( rhp );										// リードDBクローズ
		wait( 0 );														// 子プロセスの終了通知を受け取る
		wait( 0 );														// これを実行しないと<defunct>プロセスが残ってしまう
		close( io[IO_IN][PIPE_READ] );									// 入力リードクローズ
	}else{																// 子(実行)
		close( io[IO_OUT][PIPE_WRITE] );								// 出力ライトクローズ
		close( io[IO_IN][PIPE_READ] );									// 入力リードクローズ
		if( io[IO_OUT][PIPE_READ] != STDIN_FILENO ){
			if( dup2( io[IO_OUT][PIPE_READ], STDIN_FILENO ) != STDIN_FILENO ){
				fprintf( stderr, "dup2 error to stdin" );
				return( 0 );
			}
			close( io[IO_OUT][PIPE_READ] );
		}
		if( io[IO_IN][PIPE_WRITE] != STDOUT_FILENO ){
			if( dup2( io[IO_IN][PIPE_WRITE], STDOUT_FILENO ) != STDOUT_FILENO ){
				fprintf( stderr, "dup2 error to stdout" );
				return( 0 );
			}
			close( io[IO_IN][PIPE_WRITE] );
		}
		close( STDERR_FILENO );
		dup(   STDOUT_FILENO );
		argvp=lin2arg( cmdlin );										// コマンドラインから引数配列への変換
		if( execvp( argvp[PIPE_READ], argvp )<0 ){						// プロセス実行
			fprintf( stderr, "exec error:%s\n", cmdlin );
			exit( 0 );
		}																// ここには来ない
	}
	return( outdbp );
}

#	else

/************************************************************************/
/*																		*/
/*	WINDOWS																*/
/*																		*/
/************************************************************************/

#	include	<windows.h>													/* Windows ヘッダーファイル*/
#	include	<io.h>														/* ローレベルファイルハンドラ*/
#	include	<fcntl.h>													/* ファイルコントロール定義*/
#	include	<process.h>													/* プロセス管理*/
#	include	"exepath2.h"												/* 実行ファイルパス得る*/

/************************************************************************/
/*	スレッド間のデータ受け渡し構造体									*/
/************************************************************************/

	typedef struct{														// スレッド間のデータ受け渡し構造体
		HANDLE		io[IO_N][PIPE_N];									// 入出力ハンドル
		OVERLAPPED	olr;												// OVERLAPPED構造は非同期な入出力(入出力)で使用される情報を含んでいます。
		OVERLAPPED	olw;												// OVERLAPPED構造は非同期な入出力(入出力)で使用される情報を含んでいます。
		const char	*exenamp;											// フルパス実行ファイル名
		const char	*cmdlinp;											// コマンドラインポインタ
		const stDB	*indbp;												// 入力DB
	}	stENV;

/************************************************************************/
/*	書き込みスレッド													*/
/************************************************************************/

static void	wrt_thread( void *voidp ){

	stENV				*envp=(stENV*)( voidp );						// スレッド間のデータ受け渡し構造体
	char				tmpa[0x10000];
	int					tmpsiz;
	BOOL				res;
	DWORD				writesiz;
	char				*inp;

	dbFOR( envp->indbp, inp, char* ){
		tmpsiz=sprintf( tmpa, "%s\n", inp );
		res=WriteFile(
			envp->io[IO_IN][PIPE_WRITE],								// ファイルのハンドル
			tmpa,														// データバッファ
			tmpsiz,														// 書き込み対象のバイト数
			&writesiz,													// 書き込んだバイト数
			&envp->olw													// オーバーラップ構造体のバッファ
		);
		if( !res ){
			break;
		}
	}
	tmpa[0]=0x1a;
	WriteFile(
		envp->io[IO_IN][PIPE_WRITE],									// ファイルのハンドル
		tmpa,															// データバッファ
		1,																// 書き込み対象のバイト数
		&writesiz,														// 書き込んだバイト数
		&envp->olw														// オーバーラップ構造体のバッファ
	);
	CloseHandle( envp->io[IO_IN][PIPE_WRITE] );							// 開いているオブジェクトハンドルを閉じます。
}

/************************************************************************/
/*	実行スレッド														*/
/************************************************************************/

static void	exe_thread( void *voidp ){

	stENV				*envp=(stENV*)( voidp );						// スレッド間のデータ受け渡し構造体
	STARTUPINFO			startinf={ 0 };									// スタートアップインフォメーション構造体
	PROCESS_INFORMATION procinf;										// プロセスインフォメーション構造体
	BOOL				res;
	char				tmpa[4096];
	DWORD				writesiz;
	DWORD				readsiz;										// リードサイズ
	DWORD				exitcode;										// プロセスの終了コード

//	STARTUPINFO構造体のセットアップ

	startinf.cb			=sizeof( STARTUPINFO );							// 構造体サイズセット
	startinf.dwFlags	=STARTF_USESTDHANDLES|STARTF_USESHOWWINDOW;
	startinf.hStdInput	=envp->io[IO_IN][PIPE_READ];					// 標準入力
	startinf.hStdOutput	=envp->io[IO_OUT][PIPE_WRITE];					// 標準出力
	startinf.hStdError	=envp->io[IO_OUT][PIPE_WRITE];					// 標準エラー出力
	startinf.wShowWindow=SW_HIDE;										// コンソールウィンドウを表示しない

//	チャイルドプロセスの生成

	sprintf( tmpa, "%s %s", envp->exenamp, envp->cmdlinp );
	res=CreateProcess(													// 新しいプロセスと、そのプライマリスレッドを作成します.新しいプロセスは、指定された実行可能ファイルを実行します
		envp->exenamp,													// 実行するモジュールの名前
		tmpa,															// コマンドライン
		0,																// 取得したハンドルの子プロセスへの継承を許可するかどうかを決める、SECURITY_ATTRIBUTES 構造体へのポインタを指定します.
		0,																// 取得したハンドルの子スレッドへの継承を許可するかどうかを決める、SECURITY_ATTRIBUTES 構造体へのポインタを指定します.
		TRUE,															// 新しいプロセスに、呼び出し側プロセスのハンドルを継承させるかどうかを指定します.
		0,																// プロセス作成に関する制御フラグと、優先順位クラスを指定します
		0,																// 新しいプロセスの環境ブロックへのポインタを指定します.
		0,																// 新しいプロセスのカレントディレクトリ名
		&startinf,														// 新しいプロセスのメインウィンドウの表示状態が入ったSTARTUPINFO 構造体へのポインタを指定します.
		&procinf														// PROCESS_INFORMATION 構造体へのポインタを指定します.
	);
	if( res ){
		WaitForSingleObject( procinf.hProcess, INFINITE );				// プロセスが終了するのを待つ
		GetExitCodeProcess( procinf.hProcess, &exitcode );				// return termination code and exit code -- note we return the full exit code
		CloseHandle( procinf.hProcess );
		CloseHandle( procinf.hThread );
	}else{
		sprintf( tmpa, "チャイルドプロセス作成エラー:%s %s\n", envp->exenamp, envp->cmdlinp );
		WriteFile(
			envp->io[IO_OUT][PIPE_WRITE],								// ファイルのハンドル
			tmpa,														// データバッファ
			strlen( tmpa ),												// 書き込み対象のバイト数
			&writesiz,													// 書き込んだバイト数
			&envp->olw													// オーバーラップ構造体のバッファ
		);
	}
	for(;;){															// チャイルドプロセスが標準入力を読みきっていない場合はデッドロックするのでダミーリードする
		res	=ReadFile(													// ファイルからデータを読み取ります
			envp->io[IO_IN][PIPE_READ],									// ファイルのハンドル
			tmpa,														// データバッファ
			sizeof(tmpa),												// 読み取り対象のバイト数
			&readsiz,													// 読み取ったバイト数
			&envp->olr													// オーバーラップ構造体のバッファ
		);
		if( !res || !readsiz ){											// 終了
			break;
		}
	}
	CloseHandle( envp->io[IO_IN][PIPE_READ] );							// 開いているオブジェクトハンドルを閉じます。
	CloseHandle( envp->io[IO_OUT][PIPE_WRITE] );						// 開いているオブジェクトハンドルを閉じます。
}

/************************************************************************/
/*	フィルター処理( windows )											*/
/************************************************************************/

stDB	*filterdb( const char *cmdlin, const stDB *indbp ){

	stDB				*outdbp=0;										// 出力DB
	stENV				env={ 0 };										// スレッド間のデータ受け渡し構造体
	BOOL				res;											// リザルトステータス
	SECURITY_ATTRIBUTES	sa={ 0 };										// セキュリティアトリビュート構造体
	const char			*txtp;											// テキストポインタ
	char				exenam[4096];									// 実行ファイル名
	char				pacnam[4096];									// パックされた実行ファイル名
	char				tmpa[4096];										// テンポラリバッファ
	int					wrt_thread_id;									// 書き込みスレッド
	int					exe_thread_id;									// 実行スレッド
	int					i, j;
	stREADDB			*rhp;											// リードDBポインタ
	DWORD				readsiz;										// リードサイズ

	txtp=strpbrk( cmdlin, "\t " );										// タブ、スペースの検索
	if( txtp ){															// コマンドラインオプションが存在する
		memcpy( exenam, cmdlin, txtp-cmdlin );
		exenam[txtp-cmdlin]=0;											// ０ターミネート
	}else{																// コマンドラインオプションは存在しない
		strcpy( exenam, cmdlin );
		txtp="";
	}
	if( exepath2( pacnam, exenam, 0 ) ){								// 実行ファイルパスを得る
		env.exenamp	=pacnam;											// 実行ファイル名ポインタ
		env.cmdlinp	=txtp;												// コマンドライン文字列ポインタ
		env.indbp	=indbp;
		sa.nLength			= sizeof( sa );
		sa.bInheritHandle	= TRUE;
		for( i=0; i<IO_N; i++ ){
			if( !CreatePipe( &env.io[i][PIPE_READ], &env.io[i][PIPE_WRITE], &sa, 0 ) ){			// 名前なしパイプを作成し、そのパイプの読み取り側と書き込み側の両方のハンドルを取得します。
				break;
			}
		}
		if( i==IO_N ){													// パイプオープン成功
			exe_thread_id=_beginthread( exe_thread, 0, &env );			// 実行スレッド
			if( exe_thread_id>=0 ){										// 実行スレッド起動成功
				Sleep( 0 );												// 指定された時間にわたって、現在のスレッドの実行を中断します
				wrt_thread_id=_beginthread( wrt_thread, 0, &env );		// 書き込みスレッド
				if( wrt_thread_id>=0 ){									// 書き込みスレッド起動成功
					Sleep( 0 );											// 指定された時間にわたって、現在のスレッドの実行を中断します
					rhp=readdbOpen();									// リードDBオープン
					for(;;){
						Sleep( 0 );										// 指定された時間にわたって、現在のスレッドの実行を中断します
						res	=ReadFile(									// ファイルからデータを読み取ります
							env.io[IO_OUT][PIPE_READ],					// ファイルのハンドル
							tmpa,										// データバッファ
							sizeof(tmpa),								// 読み取り対象のバイト数
							&readsiz,									// 読み取ったバイト数
							&env.olr									// オーバーラップ構造体のバッファ
						);
						if( !res || !readsiz ){							// 終了
							break;
						}
						readdbAlc( rhp, tmpa, readsiz );				// 登録
					}
					outdbp=readdbClose( rhp );							// リードDBクローズ
				}else{
					printf( "_beginthread:出力スレッド作成エラー\n" );
				}
			}else{
				printf( "_beginthread:実行スレッド作成エラー\n" );
			}
			CloseHandle( env.io[IO_OUT][PIPE_READ] );					// 開いているオブジェクトハンドルを閉じます。
		}else{															// パイプオープン失敗
			for( i=0; i<IO_N; i++ ){
				for( j=0; j<PIPE_N; j++ ){
					if( env.io[i][j] ){									// 開かれている
						CloseHandle( env.io[i][j] );
					}
				}
			}
			printf( "CreatePipe:エラー\n" );
		}
	}else{
		printf( "プログラム検索エラー:%s\n", exenam );
	}
	return( outdbp );
}

#	endif

