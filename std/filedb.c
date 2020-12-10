/************************************************************************/
/*																		*/
/*	�f�[�^�x�[�X�t�@�C���o�^	by H.Watanabe							*/
/*	Ver1.00	2009/04/05 13:52:04	�쐬									*/
/*																		*/
/************************************************************************/

#	ifndef	_LARGEFILE64_SOURCE
#	define	_LARGEFILE64_SOURCE
#	define	_FILE_OFFSET_BITS	64
#	endif

#	include	<stdio.h>													/* �W�����o�͊֐�*/
#	include	<string.h>													/* �W��������֐�*/
#	include	<time.h>													/* �����֐�*/
#	include	<sys/stat.h>												/* �t�@�C���X�e�[�^�X*/
#	ifdef	unix
#		include	<unistd.h>												/* UNIX �X�^���_�[�h*/
#		include	<dirent.h>												/* �f�B���N�g������֐�*/
#		include	<utime.h>
#		ifdef	linux
#			include	<sys/types.h>
#		endif
#	endif
#	ifdef	_WIN32
#		include	<direct.h>
#		include	<io.h>													/* ���[���x���t�@�C���n���h��*/
#		if	defined(__BORLANDC__)										/* �{�[�����hC++*/
#			include	<utime.h>
#		else
#			include	<sys/utime.h>
#		endif
#	endif
#	include	"path2.h"													/* �p�X���֘A*/
#	include	"filedb.h"													/* �f�[�^�x�[�X�t�@�C���o�^*/

/************************************************************************/
/*	���[�J����`														*/
/************************************************************************/

#	define	PATHMAX			1024
#	define	NAMMAX			256
#	define	EXTMAX			256

//	�����`�F�b�N

#	define	RNGTSTB(C,L,H)	((unsigned char)((C)-(L))<=(unsigned char)((H)-(L)))				/* ���������W�r�b�g�Ń����W�`�F�b�N������*/
#	if	defined(_WIN32) || defined(__CYGWIN__)							/* SJIS*/
#		define	KANJI(C)	(RNGTSTB((C),0x81,0x9f)||RNGTSTB((C),0xe0,0xfc))
#	else																/* EUC*/
#		define	KANJI(C)	(((C)&0x80)>>7)
#	endif

/************************************************************************/
/*																		*/
/*	�t�@�C��( ��{�I�ɂ� file.c �Ƃ��ēƗ�������ׂ����� )				*/
/*																		*/
/************************************************************************/
/************************************************************************/
/*	�p�X���̍ŏI�f�B���N�g���Z�p���[�^���폜����						*/
/*	WINDOWS���Ō�̃f�B���N�g���Z�p���[�^���܂Ƃ��ɕ]�����Ă���Ȃ�		*/
/************************************************************************/

static char	*fileCutEndDirSep( char *dstp, const char *txtp ){

	unsigned char	bakchr=0;											// �P�O�̕���
	unsigned char	chr=0;												// �e�L�X�g����ǂݍ��񂾕���

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
/*	�A�g���r���[�g������쐬											*/
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
/*	�^�C���X�^���v������쐬											*/
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
/*	�^�C���X�^���v������쐬											*/
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
/*	�t�@�C�����𓾂�													*/
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
	stFILE			file;												// ���[�U�[���o�b�t�@���w�肵�Ȃ��ꍇ�̃e���|����
	int				res=0;
	int				atr=0;
	char			*namextp;
	int				staterr;
	char			tmpa[PATHMAX];

	if( !filep ){														// ���[�U�[�o�b�t�@���w��r��Ă��Ȃ�
		filep=&file;													// ���[�U�[�o�b�t�@�|�C���^�̓J�����g�e���|�����ɃZ�b�g
	}
	fileCutEndDirSep( tmpa, pacnamp );									// �p�X���̍ŏI�f�B���N�g���Z�p���[�^���폜����
	pacnamp=tmpa;
	memset( filep, 0, sizeof(stFILE) );									// �������N���A
	staterr=STAT( pacnamp, &st );
	if( !staterr ){														// �t�@�C���͑��݂���
		namextp=pathSeekNam( 0, pacnamp );
		if( namextp[0]=='.' )			atr|=FILEATR_DOT;
		filep->mtim		= (unsigned long)st.st_mtime;					// �ŏI�ύX����
		filep->atim		= (unsigned long)st.st_atime;					// �ŏI�A�N�Z�X����
		filep->siz		= (unsigned long)st.st_size;					// �t�@�C���T�C�Y64�r�b�g
		filep->sizll	= st.st_size;									// �t�@�C���T�C�Y64�r�b�g
		filep->mode		= st.st_mode;									// ���[�h
#	ifdef	unix
		filep->ino		= st.st_ino;									// i�m�[�h�ԍ�
		filep->dev		= st.st_dev;									// ���u�ԍ�( �t�@�C���V�X�e�� )
		filep->rdev		= st.st_rdev;									// ����t�@�C���̑��u�ԍ�
		filep->nlink	= st.st_nlink;
		filep->uid		= st.st_uid;									// ���[�U�[ID
		filep->gid		= st.st_gid;									// �O���[�vID
		filep->blksize	= st.st_blksize;								// �u���b�N�T�C�Y
		filep->blocks	= st.st_blocks;									// �u���b�N��
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
		if( st.st_mode&_S_IFDIR		)	atr|=FILEATR_DIR;				// �f�B���N�g��
		if( st.st_mode&_S_IFCHR		)	atr|=FILEATR_CHR;				// �L�����N�^�t�@�C��
		if( st.st_mode&_S_IFIFO		)	atr|=FILEATR_FIFO;				// �p�C�v
		if( st.st_mode&_S_IFREG		)	atr|=FILEATR_REG;				// �ʏ�t�@�C��
		if( st.st_mode&_S_IREAD		)	atr|=FILEATR_RALL;				// ���[�h�p�[�~�b�V����
		if( st.st_mode&_S_IWRITE	)	atr|=FILEATR_WALL;				// ���C�g�p�[�~�b�V����
		if( st.st_mode&_S_IEXEC		)	atr|=FILEATR_XALL;				// ���s�p�[�~�b�V����
#	endif
		if( atr&FILEATR_DOT ){											// .�Ŏn�܂�t�@�C�����Ȃ��
			atr&=~FILEATR_REG;											// �ʏ�t�@�C���A�g���r���[�g����
		}
		if( atr&FILEATR_LNK ){											// �����N�t�@�C��
			atr&=~FILEATR_REG;											// �ʏ�t�@�C���A�g���r���[�g����
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
		if( chr==':' ){													// �h���C�u������
			filep->atr=FILEATR_DIR;
			if( fileatr&FILEATR_DIR ){
				res=1;
			}
		}else if( fileatr&FILEATR_NON ){								// �����ꍇ�ł��L��
			filep->atr=FILEATR_NON;
			res=1;
		}
	}
	return( res );
}

/************************************************************************/
/*	�o�̓t�@�C�����쐬													*/
/************************************************************************/

void	fileMakDst( char *dstp, const char *sounamp, const char *pathp, const char *extp ){

	stFILE		fil;													// �t�@�C���X�e�[�^�X�\����
	char		path[PATHMAX],nam[NAMMAX],namext[EXTMAX];
	char		*tmpextp;

	if( !extp ){														// �V�����g���q���͎w�肳��Ă��Ȃ�
		extp=pathSeekExt( 0, 0, sounamp );								// �g�������̓\�[�X�t�@�C���̊g���q
	}
	if( !*pathp || fileStat( &fil, pathp, FILEATR_DIR ) ){				// �f�B�X�g�l�[�V�����p�X���̓f�B���N�g��
		pathSeekExt( 0, nam, sounamp );
	}else{																// �f�B�X�g�l�[�V�����p�X���̓f�B���N�g���ł͂Ȃ�
		tmpextp=pathSeekExt( path, nam, pathp );						// �f�B�X�g�l�[�V�����p�X���̊g���q�V�[�N
		if( *tmpextp ){													// �f�B�X�g�l�[�V�����p�X���Ɋg���q������
			extp=tmpextp;												// �g���q���̓f�B�X�g�l�[�V�����p�X�t���̂��̂��g�p����
		}
		pathp=path;
	}
	sprintf( namext, "%s%s", nam, extp );								// ���O�{�g���q
	pathCat( dstp, pathp, namext );										// �p�X���{���O�{�g���q
}

/************************************************************************/
/*																		*/
/*	DB �o�^( ��{�I�ɂ� filedb.c �Ƃ��ēƗ�������ׂ����� )				*/
/*																		*/
/************************************************************************/
/************************************************************************/
/*	���C���h�J�[�h�`�F�b�N												*/
/************************************************************************/

int		filedbWildChk( const char *soup, const char *wldp ){

	unsigned int	wld,sou;											// char �ŗǂ��̂��� �������E�������ߖ�̂��� int �Ƃ���

	for(;;){
		wld=*wldp++;													// ���C���h�J�[�h�𓾂�
		if( wld=='*' ){													// ���C���h�J�[�h
			if( !*wldp ){												// �I�[�Ȃ̂Ń\�[�X�͉��ł��ǂ�
				return( 1 );											// ��v����
			}
			for(;;){
				if( filedbWildChk( soup, wldp ) ){						// ��v����
					return( 1 );										// ��v����
				}
				if( !*soup++ ){											// �\�[�X���I�[
					break;												// ��v���Ȃ�
				}
			}
			break;														// ��v���Ȃ�
		}
		sou=*soup++;													// �\�[�X����ꕶ���ǂݍ���
		if( !(wld|sou) ){												// ���ɂO
			return( 1 );												// ��v����
		}
#	ifdef	_WIN32
		if( wld>='A' && wld<='Z' ){										// �啶���ł���
			wld+=('a'-'A');												// �������ɕϊ�
		}
		if( sou>='A' && sou<='Z' ){										// �啶���ł���
			sou+=('a'-'A');												// �������ɕϊ�
		}
#	endif
		if( wld!='?' && wld!=sou ){
			break;														// ��v���Ȃ�
		}
	}
	return( 0 );														// ��v���Ȃ�
}

/************************************************************************/
/*	�t�@�C��DB�I�[�v��													*/
/************************************************************************/

stDB	*filedbOpen(void){
	return( dbOpen( DBSIZ_FNAM, sizeof(stFILE), 256, 4096 ) );
}

/************************************************************************/
/*	�w��t�@�C����DB�ɓo�^����( ���C���h�J�[�h�g�p�� )					*/
/************************************************************************/

stDB	*filedbAlc( stDB *dbp, const char *pacnamp, int fileatr ){

	char	dirnam[PATHMAX];											// �f�B���N�g����
	char	pacnam[PATHMAX];											// �p�b�N��
	stFILE	filedb;														// �t�@�C�����\����
	char	*txtp;														// �e���|�����e�L�X�g�|�C���^

	if( !dbp ){
		dbp=filedbOpen();
	}
	if( pacnamp ){
		if( !strpbrk( pacnamp, "*?" ) ){								// ���C���h�J�[�h�s�g�p
			if( !dbSch( dbp, pacnamp ) ){								// ���o�^
				if( fileStat( &filedb, pacnamp, fileatr ) ){			// �t�@�C���X�e�[�^�X�擾
					dbAlc( dbp, pacnamp, &filedb );						// �o�^
				}
			}
		}else{															// ���C���h�J�[�h���g�p���Ă���

#	ifdef	unix
			DIR				*dirp;										// �f�B���N�g���Ǘ��|�C���^
			struct dirent	*direntp;
			char			*namextp;									// ���O�{�g���q�|�C���^

			namextp=pathSeekNam( dirnam, pacnamp );
			if( !dirnam[0] ){											// �f�B���N�g�����͂Ȃ�
				dirp=opendir( "." );									// �J�����g�f�B���N�g���I�[�v��
			}else{														// �f�B���N�g���������݂���
				dirp=opendir( dirnam );									// �w��f�B���N�g���I�[�v��
			}
			if( dirp ){													// �f�B���N�g���I�[�v������
				while( ( direntp=readdir( dirp ) ) ){					// �f�B���N�g�������N���[�v
					txtp=direntp->d_name;								// txtp=���O�{�g���q
					if( !(fileatr&FILEATR_DOTDIR) ){
						while( *txtp=='.' ) txtp++;						// '.'���X�L�b�v����
					}
					if( *txtp ){										// ".",".."�ł͂Ȃ�
						if( filedbWildChk( direntp->d_name, namextp ) ){// ���C���h�J�[�h�`�F�b�N
							pathCat( pacnam, dirnam, direntp->d_name );	// �p�X������������
							if( !dbSch( dbp, pacnam ) ){				// ���o�^
								if( fileStat( &filedb, pacnam, fileatr ) ){		// �t�@�C�����𓾂�
									dbAlc( dbp, pacnam, &filedb );		// �o�^
								}
							}
						}
					}
				}
				closedir( dirp );										// �f�B���N�g���N���[�Y
			}
#	else
			long				findh;									// �t�@�C���h�n���h��
			struct _finddata_t	finddata;								// �t�@�C���h�o�b�t�@

			pathSeekNam( dirnam, pacnamp );								// �f�B���N�g�����𕪗����Ă���
			findh=_findfirst( (char*)pacnamp, &finddata );				// �ŏ��Ɉ�v����t�@�C���̌���
			if( findh!=-1 ){											// ����
				for(;;){
					txtp=finddata.name;									// txtp=���O�{�g���q
					if( !(fileatr&FILEATR_DOTDIR) ){
						while( *txtp=='.' ) txtp++;						// '.'���X�L�b�v����
					}
					if( *txtp ){										// ".",".."�ł͂Ȃ�
						pathCat( pacnam, dirnam, finddata.name );		// �p�X������������
						if( !dbSch( dbp, pacnam ) ){					// ���o�^
							if( fileStat( &filedb, pacnam, fileatr ) ){	// �t�@�C�����𓾂�
								dbAlc( dbp, pacnam, &filedb );			// �o�^
							}
						}
					}
					if( _findnext( findh, &finddata ) ) break;			// ���Ɉ�v����t�@�C�����̌���
				}
				_findclose( findh );									// �t�@�C���h�n���h���N���[�Y
			}
#	endif
		}
	}
	return( dbp );
}

/************************************************************************/
/*	DB����FILEDB�ɓo�^����												*/
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
/*	�f�B���N�g�����I�[�v������											*/
/************************************************************************/

stDB	*filedbDirOpen( const char *dirnamp, int fileatr ){

	stDB	*filedbp;
	char	tmpa[1024];													// ���C���h�J�[�h���쐬�p�o�b�t�@

	pathCat( tmpa, dirnamp, "*" );										// �f�B���N�g�����̃t�@�C���𑍂ēo�^���邽�߂Ƀ��C���h�J�[�h���쐬����
	filedbp=filedbAlc( 0, tmpa, fileatr );								// �f�B���N�g�����̃t�@�C���𑍂ēo�^
	return( filedbp );
}

/************************************************************************/
/*	�f�B���N�g�����݃`�F�b�N											*/
/************************************************************************/

int		filedbChkDir( const char *dirnamp ){

	struct stat	st;
	int			res=0;

#	ifdef	unix

	if( !stat( dirnamp, &st ) ){										// �t�@�C���͑��݂���
		res=S_ISDIR( st.st_mode );
	}

#	endif

#	ifdef	_WIN32

	char		tmpa[PATHMAX],*tmpp;
	char		chr=0;

	fileCutEndDirSep( tmpa, dirnamp );									// �p�X���̍ŏI�f�B���N�g���Z�p���[�^���폜����
	tmpp=tmpa;
	while( *tmpp ){
		chr=*tmpp++;
	}
	if( chr==':' ){														// �ŏI������':'�Ȃ̂Ńh���C�u��
		res=1;
	}else{
		if( !stat( tmpa, &st ) ){										// �t�@�C���͑��݂���
			res=st.st_mode&_S_IFDIR;
		}
	}

#	endif

	return( res );
}

/************************************************************************/
/*	�f�B���N�g���쐬													*/
/************************************************************************/

int		filedbMakDir( const char *dirnamp ){

#	ifdef	_WIN32
	return( _mkdir( dirnamp )==0 );
#	else
	return( mkdir( dirnamp, S_IRWXU|S_IRWXG|S_IRWXO )==0 );
#	endif
}

/************************************************************************/
/*	�ċA�I�Ƀf�B���N�g���쐬											*/
/************************************************************************/

int		filedbMakDirR( const char *dirnamp ){

	int				err=0;												// �G���[�t���O( �_�����]�������U���g�X�e�[�^�X )
	const char		*txtp;												// �e�L�X�g�|�C���^
	unsigned char	chr;												// �e�L�X�g���瓾������
	char			tmpa[PATHMAX],*tmpp;								// �e���|����
	int				kanlen;												// ����������

	if( dirnamp[0] ){													// �f�B���N�g�����͗L��
		if( !filedbChkDir( dirnamp ) ){									// �f�B���N�g���͑��݂��Ȃ�
			txtp=dirnamp;												// �f�B���N�g�����|�C���^
			tmpp=tmpa;													// �e���|�����|�C���^
			for(;;){
				chr=*txtp++;											// �e�L�X�g����P��������
				kanlen=KANJI( chr );									// ���������񒷂𓾂�
				if( !chr || chr=='/' || chr=='\\' ){					// �e�L�X�g�I�[�܂��̓f�B���N�g���Z�p���[�^
					*tmpp=0;											// ��U�O�I�[�Ƃ���
					if( tmpa[0] ){										// �L���ȃf�B���N�g����
						if( !filedbChkDir( tmpa ) ){					// �f�B���N�g���͑��݂��Ȃ�
							err=!filedbMakDir( tmpa );					// �f�B���N�g���쐬
							if( err ){									// �f�B���N�g���쐬�G���[
								break;									// ���[�v�I��
							}
						}
					}
				}
				*tmpp++=chr;											// �e���|�����ɂP�����X�g�A
				if( !chr ){												// ������I�[
					break;												// ���[�v�I��
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
/*	�t�@�C�����[�h�ύX													*/
/************************************************************************/

int		filedbChgMod( const char *fnamp, int mtim, int fatr ){

	int				res=1;
	struct utimbuf	utim;

	if( mtim!=-1 ){
		utim.actime		= mtim;
		utim.modtime	= mtim;
		if( utime( fnamp, &utim ) ){
			res=0;														// �G���[
		}
	}
	if( fatr!=-1 ){														// �t�@�C���A�g���r���[�g�����݂���
#	ifdef	unix

		mode_t	mode=0;

		if( fatr&FILEATR_RUSR ) mode|=S_IRUSR;							// ���[�U�[�Ǐo ����
		if( fatr&FILEATR_WUSR ) mode|=S_IWUSR;							// ���[�U�[���� ����
		if( fatr&FILEATR_XUSR ) mode|=S_IXUSR;							// ���[�U�[���s ����
		if( fatr&FILEATR_RGRP ) mode|=S_IRGRP;							// �O���[�v�Ǐo ����
		if( fatr&FILEATR_WGRP ) mode|=S_IWGRP;							// �O���[�v���� ����
		if( fatr&FILEATR_XGRP ) mode|=S_IXGRP;							// �O���[�v���s ����
		if( fatr&FILEATR_ROTH ) mode|=S_IROTH;							// ���	   �Ǐo ����
		if( fatr&FILEATR_WOTH ) mode|=S_IWOTH;							// ���	   ���� ����
		if( fatr&FILEATR_XOTH ) mode|=S_IXOTH;							// ���	   ���s ����
		if( chmod( fnamp, mode ) ){										// �t�@�C�����[�h�ύX���s
			res=0;														// �G���[
		}
#	endif
	}
	return( res );
}

/************************************************************************/
/*	�t�@�C���폜���C��													*/
/************************************************************************/

#	ifndef	STDERR
#		ifdef	_WIN32													/* Windows �̏ꍇ*/
#			define	STDERR	stdout										/* �G���[�o�͕͂W���o��( �p�C�v���Ŗ�肪�������� )*/
#		else															/* ���̑�*/
#			define	STDERR	stderr										/* �G���[�o�͕͂W���G���[�o��*/
#		endif
#	endif

#	ifdef	_WIN32
#		define	RMDIR		_rmdir
#	else
#		define	RMDIR		rmdir
#	endif

int		filedbRemove( const char *fnamp, int opta, int optt ){

	int		errn=0;														// �G���[��
	stDB	*dirdbp;													// �f�B���N�g���c�a
	stFILE	*p;															// �e���|�����t�@�C���|�C���^
	stFILE	file;

	if( fileStat( &file, fnamp, FILEATR_ALL ) ){
		if( opta && !(file.atr&FILEATR_WUSR) ){							// �������݋����Ȃ�
			if( !optt ){												// �e�X�g���[�h�ł͂Ȃ�
#	ifdef	unix
				errn=chmod( fnamp, file.mode|FILEATR_WUSR );
#	endif
#	ifdef	_WIN32
				errn=_chmod( fnamp, _S_IWRITE );
#	endif
			}
			if( errn ){
				fprintf( STDERR, "���[�h�I�����[�A�g���r���[�g�ύX�G���[:%s\n", fnamp );
			}
		}
		if( file.atr&FILEATR_DIR ){										// �f�B���N�g��
			dirdbp=filedbDirOpen( fnamp, FILEATR_ALL );					// �f�B���N�g�����̃t�@�C���𑍂ēo�^
			dbFOR( dirdbp, p, stFILE* ){
				errn+=!filedbRemove( (char*)dbKEYADR( p ), opta, optt );// �ċA�R�[��
			}
			dbClose( dirdbp );											// �f�B���N�g���c�a�N���[�Y
			printf( "rmdir	%s\n", fnamp );
			if( !optt ){												// �e�X�g���[�h�ł͂Ȃ�
				errn=RMDIR( fnamp );									// �f�B���N�g���폜
			}
			if( errn ){
				fprintf( STDERR, "�f�B���N�g���폜�G���[:%s\n", fnamp );
			}
		}else{															// �ʏ�t�@�C��
			printf( "rm	%s\n", fnamp );
			if( !optt ){												// �e�X�g���[�h�ł͂Ȃ�
				errn=remove( fnamp );									// �t�@�C���폜�G���[
			}
			if( errn ){
				fprintf( STDERR, "�t�@�C���폜�G���[:%s\n", fnamp );
			}
		}
	}else{
		fprintf( STDERR, "�t�@�C����������܂���:%s\n", fnamp );
		errn++;
	}
	return( !errn );													// 0:�G���[, 1:����I��
}

