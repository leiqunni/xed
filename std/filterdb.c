/************************************************************************/
/*																		*/
/*	�t�B���^�[����				by H.Watanabe							*/
/*	Ver1.00	2009/04/05 13:52:04	�쐬									*/
/*																		*/
/************************************************************************/

#	include	<stdio.h>													/* �W�����o�͊֐�*/
#	include	<stdlib.h>													/* �W�����C�u�����֐�*/
#	include	"lin2arg2.h"												/* �R�}���h���C����������z��ւ̕ϊ�*/
#	include	"readdb.h"													/* �N���b�v�{�[�h������̍s�ϊ�*/
#	include	"filterdb.h"												/* �t�B���^�[����*/

/************************************************************************/
/*	���[�J����`														*/
/************************************************************************/

	enum{
		PIPE_READ,														// ���[�h
		PIPE_WRITE,														// ���C�g
		PIPE_N
	};
	enum{
		IO_IN,															// ����
		IO_OUT,															// �o��
		IO_N
	};

#	ifdef	unix

/************************************************************************/
/*																		*/
/*	UNIX																*/
/*																		*/
/************************************************************************/

#	include	<signal.h>													/* signal����*/
#	include	<unistd.h>													/* UNIX �X�^���_�[�h*/
#	include	<sys/wait.h>

/************************************************************************/
/*	�V�O�i���G���[														*/
/************************************************************************/

static void sig_pipe( int signo ){

	if( signo!=13 ){													// �N�������ǂ܂Ȃ������ł͂Ȃ�
		fprintf( stderr, "SIGPIPE caught %d\n", signo );
	}
	exit( 1 );
}

/************************************************************************/
/*	�t�B���^�[����( unix )												*/
/************************************************************************/

stDB	*filterdb( const char *cmdlin, const stDB *indbp ){

	stDB		*outdbp=0;												// �o��DB
	char		tmpa[0x10000];											// �e���|�����o�b�t�@
	int			io[IO_N][PIPE_N];										// in,out �͐e���猩������
	pid_t		pid, pid2;												// �v���Z�XIS
	int			linlen;													// �s�T�C�Y
	char		**argvp;												// �����z��|�C���^
	char		*inp;													// ���̓|�C���^
	int			readn;													// ���[�h�T�C�Y
	stREADDB	*rhp;													// ���[�hDB�|�C���^

	if( signal( SIGPIPE, sig_pipe )==SIG_ERR ){							// �V�O�i���Z�b�g
		fprintf( stderr, "signal error" );
		return( 0 );
	}
	if( pipe( io[IO_OUT] )<0 || pipe( io[IO_IN] )<0 ){					// �p�C�v�I�[�v��
		fprintf( stderr, "pipe error" );
		return( 0 );
	}
	if( (pid=fork())<0 ){												// �q�v���Z�X����
		fprintf( stderr, "fork error" );
		return( 0 );
	}
	if( pid ){															// �e
		close( io[IO_OUT][PIPE_READ] );									// �o�̓��[�h�N���[�Y
		close( io[IO_IN][PIPE_WRITE] );									// ���̓��C�g�N���[�Y
		if( (pid2=fork())<0 ){											// �q�v���Z�X����
			fprintf( stderr, "fork error" );
			return( 0 );
		}
		if( !pid2 ){													// �q(�o��)
			close( io[IO_IN][PIPE_READ] );								// ���̓��[�h�N���[�Y
			dbFOR( indbp, inp, char* ){									// ����DB�����胋�[�v
				linlen=sprintf( tmpa, "%s\n", inp );					// �s�T�C�Y
				if( write( io[IO_OUT][PIPE_WRITE], tmpa, linlen )!=linlen ){
					fprintf( stderr, "write error\n" );
					exit( 0 );
				}
			}
			close( io[IO_OUT][PIPE_WRITE] );							// �o�̓��C�g�N���[�Y
			exit( 0 );													// �q�I��
		}
		close( io[IO_OUT][PIPE_WRITE] );								// �o�̓��C�g�N���[�Y
		rhp=readdbOpen();												// ���[�hDB�I�[�v��
		for(;;){
			readn=read( io[IO_IN][PIPE_READ], tmpa, sizeof(tmpa) );
			if( !readn ){
				break;
			}
			readdbAlc( rhp, tmpa, readn );								// ���[�hDB�o�^
		}
		outdbp=readdbClose( rhp );										// ���[�hDB�N���[�Y
		wait( 0 );														// �q�v���Z�X�̏I���ʒm���󂯎��
		wait( 0 );														// ��������s���Ȃ���<defunct>�v���Z�X���c���Ă��܂�
		close( io[IO_IN][PIPE_READ] );									// ���̓��[�h�N���[�Y
	}else{																// �q(���s)
		close( io[IO_OUT][PIPE_WRITE] );								// �o�̓��C�g�N���[�Y
		close( io[IO_IN][PIPE_READ] );									// ���̓��[�h�N���[�Y
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
		argvp=lin2arg( cmdlin );										// �R�}���h���C����������z��ւ̕ϊ�
		if( execvp( argvp[PIPE_READ], argvp )<0 ){						// �v���Z�X���s
			fprintf( stderr, "exec error:%s\n", cmdlin );
			exit( 0 );
		}																// �����ɂ͗��Ȃ�
	}
	return( outdbp );
}

#	else

/************************************************************************/
/*																		*/
/*	WINDOWS																*/
/*																		*/
/************************************************************************/

#	include	<windows.h>													/* Windows �w�b�_�[�t�@�C��*/
#	include	<io.h>														/* ���[���x���t�@�C���n���h��*/
#	include	<fcntl.h>													/* �t�@�C���R���g���[����`*/
#	include	<process.h>													/* �v���Z�X�Ǘ�*/
#	include	"exepath2.h"												/* ���s�t�@�C���p�X����*/

/************************************************************************/
/*	�X���b�h�Ԃ̃f�[�^�󂯓n���\����									*/
/************************************************************************/

	typedef struct{														// �X���b�h�Ԃ̃f�[�^�󂯓n���\����
		HANDLE		io[IO_N][PIPE_N];									// ���o�̓n���h��
		OVERLAPPED	olr;												// OVERLAPPED�\���͔񓯊��ȓ��o��(���o��)�Ŏg�p���������܂�ł��܂��B
		OVERLAPPED	olw;												// OVERLAPPED�\���͔񓯊��ȓ��o��(���o��)�Ŏg�p���������܂�ł��܂��B
		const char	*exenamp;											// �t���p�X���s�t�@�C����
		const char	*cmdlinp;											// �R�}���h���C���|�C���^
		const stDB	*indbp;												// ����DB
	}	stENV;

/************************************************************************/
/*	�������݃X���b�h													*/
/************************************************************************/

static void	wrt_thread( void *voidp ){

	stENV				*envp=(stENV*)( voidp );						// �X���b�h�Ԃ̃f�[�^�󂯓n���\����
	char				tmpa[0x10000];
	int					tmpsiz;
	BOOL				res;
	DWORD				writesiz;
	char				*inp;

	dbFOR( envp->indbp, inp, char* ){
		tmpsiz=sprintf( tmpa, "%s\n", inp );
		res=WriteFile(
			envp->io[IO_IN][PIPE_WRITE],								// �t�@�C���̃n���h��
			tmpa,														// �f�[�^�o�b�t�@
			tmpsiz,														// �������ݑΏۂ̃o�C�g��
			&writesiz,													// �������񂾃o�C�g��
			&envp->olw													// �I�[�o�[���b�v�\���̂̃o�b�t�@
		);
		if( !res ){
			break;
		}
	}
	tmpa[0]=0x1a;
	WriteFile(
		envp->io[IO_IN][PIPE_WRITE],									// �t�@�C���̃n���h��
		tmpa,															// �f�[�^�o�b�t�@
		1,																// �������ݑΏۂ̃o�C�g��
		&writesiz,														// �������񂾃o�C�g��
		&envp->olw														// �I�[�o�[���b�v�\���̂̃o�b�t�@
	);
	CloseHandle( envp->io[IO_IN][PIPE_WRITE] );							// �J���Ă���I�u�W�F�N�g�n���h������܂��B
}

/************************************************************************/
/*	���s�X���b�h														*/
/************************************************************************/

static void	exe_thread( void *voidp ){

	stENV				*envp=(stENV*)( voidp );						// �X���b�h�Ԃ̃f�[�^�󂯓n���\����
	STARTUPINFO			startinf={ 0 };									// �X�^�[�g�A�b�v�C���t�H���[�V�����\����
	PROCESS_INFORMATION procinf;										// �v���Z�X�C���t�H���[�V�����\����
	BOOL				res;
	char				tmpa[4096];
	DWORD				writesiz;
	DWORD				readsiz;										// ���[�h�T�C�Y
	DWORD				exitcode;										// �v���Z�X�̏I���R�[�h

//	STARTUPINFO�\���̂̃Z�b�g�A�b�v

	startinf.cb			=sizeof( STARTUPINFO );							// �\���̃T�C�Y�Z�b�g
	startinf.dwFlags	=STARTF_USESTDHANDLES|STARTF_USESHOWWINDOW;
	startinf.hStdInput	=envp->io[IO_IN][PIPE_READ];					// �W������
	startinf.hStdOutput	=envp->io[IO_OUT][PIPE_WRITE];					// �W���o��
	startinf.hStdError	=envp->io[IO_OUT][PIPE_WRITE];					// �W���G���[�o��
	startinf.wShowWindow=SW_HIDE;										// �R���\�[���E�B���h�E��\�����Ȃ�

//	�`���C���h�v���Z�X�̐���

	sprintf( tmpa, "%s %s", envp->exenamp, envp->cmdlinp );
	res=CreateProcess(													// �V�����v���Z�X�ƁA���̃v���C�}���X���b�h���쐬���܂�.�V�����v���Z�X�́A�w�肳�ꂽ���s�\�t�@�C�������s���܂�
		envp->exenamp,													// ���s���郂�W���[���̖��O
		tmpa,															// �R�}���h���C��
		0,																// �擾�����n���h���̎q�v���Z�X�ւ̌p���������邩�ǂ��������߂�ASECURITY_ATTRIBUTES �\���̂ւ̃|�C���^���w�肵�܂�.
		0,																// �擾�����n���h���̎q�X���b�h�ւ̌p���������邩�ǂ��������߂�ASECURITY_ATTRIBUTES �\���̂ւ̃|�C���^���w�肵�܂�.
		TRUE,															// �V�����v���Z�X�ɁA�Ăяo�����v���Z�X�̃n���h�����p�������邩�ǂ������w�肵�܂�.
		0,																// �v���Z�X�쐬�Ɋւ��鐧��t���O�ƁA�D�揇�ʃN���X���w�肵�܂�
		0,																// �V�����v���Z�X�̊��u���b�N�ւ̃|�C���^���w�肵�܂�.
		0,																// �V�����v���Z�X�̃J�����g�f�B���N�g����
		&startinf,														// �V�����v���Z�X�̃��C���E�B���h�E�̕\����Ԃ�������STARTUPINFO �\���̂ւ̃|�C���^���w�肵�܂�.
		&procinf														// PROCESS_INFORMATION �\���̂ւ̃|�C���^���w�肵�܂�.
	);
	if( res ){
		WaitForSingleObject( procinf.hProcess, INFINITE );				// �v���Z�X���I������̂�҂�
		GetExitCodeProcess( procinf.hProcess, &exitcode );				// return termination code and exit code -- note we return the full exit code
		CloseHandle( procinf.hProcess );
		CloseHandle( procinf.hThread );
	}else{
		sprintf( tmpa, "�`���C���h�v���Z�X�쐬�G���[:%s %s\n", envp->exenamp, envp->cmdlinp );
		WriteFile(
			envp->io[IO_OUT][PIPE_WRITE],								// �t�@�C���̃n���h��
			tmpa,														// �f�[�^�o�b�t�@
			strlen( tmpa ),												// �������ݑΏۂ̃o�C�g��
			&writesiz,													// �������񂾃o�C�g��
			&envp->olw													// �I�[�o�[���b�v�\���̂̃o�b�t�@
		);
	}
	for(;;){															// �`���C���h�v���Z�X���W�����͂�ǂ݂����Ă��Ȃ��ꍇ�̓f�b�h���b�N����̂Ń_�~�[���[�h����
		res	=ReadFile(													// �t�@�C������f�[�^��ǂݎ��܂�
			envp->io[IO_IN][PIPE_READ],									// �t�@�C���̃n���h��
			tmpa,														// �f�[�^�o�b�t�@
			sizeof(tmpa),												// �ǂݎ��Ώۂ̃o�C�g��
			&readsiz,													// �ǂݎ�����o�C�g��
			&envp->olr													// �I�[�o�[���b�v�\���̂̃o�b�t�@
		);
		if( !res || !readsiz ){											// �I��
			break;
		}
	}
	CloseHandle( envp->io[IO_IN][PIPE_READ] );							// �J���Ă���I�u�W�F�N�g�n���h������܂��B
	CloseHandle( envp->io[IO_OUT][PIPE_WRITE] );						// �J���Ă���I�u�W�F�N�g�n���h������܂��B
}

/************************************************************************/
/*	�t�B���^�[����( windows )											*/
/************************************************************************/

stDB	*filterdb( const char *cmdlin, const stDB *indbp ){

	stDB				*outdbp=0;										// �o��DB
	stENV				env={ 0 };										// �X���b�h�Ԃ̃f�[�^�󂯓n���\����
	BOOL				res;											// ���U���g�X�e�[�^�X
	SECURITY_ATTRIBUTES	sa={ 0 };										// �Z�L�����e�B�A�g���r���[�g�\����
	const char			*txtp;											// �e�L�X�g�|�C���^
	char				exenam[4096];									// ���s�t�@�C����
	char				pacnam[4096];									// �p�b�N���ꂽ���s�t�@�C����
	char				tmpa[4096];										// �e���|�����o�b�t�@
	int					wrt_thread_id;									// �������݃X���b�h
	int					exe_thread_id;									// ���s�X���b�h
	int					i, j;
	stREADDB			*rhp;											// ���[�hDB�|�C���^
	DWORD				readsiz;										// ���[�h�T�C�Y

	txtp=strpbrk( cmdlin, "\t " );										// �^�u�A�X�y�[�X�̌���
	if( txtp ){															// �R�}���h���C���I�v�V���������݂���
		memcpy( exenam, cmdlin, txtp-cmdlin );
		exenam[txtp-cmdlin]=0;											// �O�^�[�~�l�[�g
	}else{																// �R�}���h���C���I�v�V�����͑��݂��Ȃ�
		strcpy( exenam, cmdlin );
		txtp="";
	}
	if( exepath2( pacnam, exenam, 0 ) ){								// ���s�t�@�C���p�X�𓾂�
		env.exenamp	=pacnam;											// ���s�t�@�C�����|�C���^
		env.cmdlinp	=txtp;												// �R�}���h���C��������|�C���^
		env.indbp	=indbp;
		sa.nLength			= sizeof( sa );
		sa.bInheritHandle	= TRUE;
		for( i=0; i<IO_N; i++ ){
			if( !CreatePipe( &env.io[i][PIPE_READ], &env.io[i][PIPE_WRITE], &sa, 0 ) ){			// ���O�Ȃ��p�C�v���쐬���A���̃p�C�v�̓ǂݎ�葤�Ə������ݑ��̗����̃n���h�����擾���܂��B
				break;
			}
		}
		if( i==IO_N ){													// �p�C�v�I�[�v������
			exe_thread_id=_beginthread( exe_thread, 0, &env );			// ���s�X���b�h
			if( exe_thread_id>=0 ){										// ���s�X���b�h�N������
				Sleep( 0 );												// �w�肳�ꂽ���Ԃɂ킽���āA���݂̃X���b�h�̎��s�𒆒f���܂�
				wrt_thread_id=_beginthread( wrt_thread, 0, &env );		// �������݃X���b�h
				if( wrt_thread_id>=0 ){									// �������݃X���b�h�N������
					Sleep( 0 );											// �w�肳�ꂽ���Ԃɂ킽���āA���݂̃X���b�h�̎��s�𒆒f���܂�
					rhp=readdbOpen();									// ���[�hDB�I�[�v��
					for(;;){
						Sleep( 0 );										// �w�肳�ꂽ���Ԃɂ킽���āA���݂̃X���b�h�̎��s�𒆒f���܂�
						res	=ReadFile(									// �t�@�C������f�[�^��ǂݎ��܂�
							env.io[IO_OUT][PIPE_READ],					// �t�@�C���̃n���h��
							tmpa,										// �f�[�^�o�b�t�@
							sizeof(tmpa),								// �ǂݎ��Ώۂ̃o�C�g��
							&readsiz,									// �ǂݎ�����o�C�g��
							&env.olr									// �I�[�o�[���b�v�\���̂̃o�b�t�@
						);
						if( !res || !readsiz ){							// �I��
							break;
						}
						readdbAlc( rhp, tmpa, readsiz );				// �o�^
					}
					outdbp=readdbClose( rhp );							// ���[�hDB�N���[�Y
				}else{
					printf( "_beginthread:�o�̓X���b�h�쐬�G���[\n" );
				}
			}else{
				printf( "_beginthread:���s�X���b�h�쐬�G���[\n" );
			}
			CloseHandle( env.io[IO_OUT][PIPE_READ] );					// �J���Ă���I�u�W�F�N�g�n���h������܂��B
		}else{															// �p�C�v�I�[�v�����s
			for( i=0; i<IO_N; i++ ){
				for( j=0; j<PIPE_N; j++ ){
					if( env.io[i][j] ){									// �J����Ă���
						CloseHandle( env.io[i][j] );
					}
				}
			}
			printf( "CreatePipe:�G���[\n" );
		}
	}else{
		printf( "�v���O���������G���[:%s\n", exenam );
	}
	return( outdbp );
}

#	endif

