/************************************************************************/
/*																		*/
/*	�f�[�^�x�[�X�t�@�C���o�^	by H.Watanabe							*/
/*	Ver1.00	2009/04/05 13:52:04	�쐬									*/
/*																		*/
/************************************************************************/

#	ifndef	FILEDB_H
#	define	FILEDB_H
#	ifdef	__cplusplus
	extern "C"{
#	endif

#	include	"db2.h"														/* �v���O�������ėp�f�[�^�x�[�X�V�X�e��*/

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

#	define	FILEATR_DIR			(1<<0)									/* �f�B���N�g��*/
#	define	FILEATR_REG			(1<<1)									/* �ʏ�t�@�C��*/
#	define	FILEATR_SYS			(1<<2)									/* �V�X�e���t�@�C��*/
#	define	FILEATR_NON			(1<<3)									/* ���݂��Ȃ��t�@�C��*/
#	define	FILEATR_LNK			(1<<4)									/* �����N�t�@�C��( UNIX �̂� )*/
#	define	FILEATR_WLD			(1<<5)									/* ���C���h�J�[�h���g�p����Ă���*/
#	define	FILEATR_DOT			(1<<6)									/* . �ł͂��܂�t�@�C��( UNIX �̂� )*/
#	define	FILEATR_FIFO		(1<<7)									/* FIFO �t�@�C��( UNIX �̂� )*/
#	define	FILEATR_CHR			(1<<8)									/* �L�����N�^�f�o�C�X�t�@�C��( UNIX �̂� )*/
#	define	FILEATR_BLK			(1<<9)									/* �u���b�N�f�o�C�X�t�@�C��( UNIX �̂� )*/
#	define	FILEATR_SOCK		(1<<10)									/* �\�P�b�g�t�@�C��( UNIX �̂� )*/
#	define	FILEATR_NFS			(1<<11)									/* NFS �t�@�C��*/
#	define	FILEATR_RUSR		(1<<16)									/* ���[�U�[���[�h����*/
#	define	FILEATR_WUSR		(1<<17)									/* ���[�U�[���C�g����*/
#	define	FILEATR_XUSR		(1<<18)									/* ���[�U�[���s	 ����*/
#	define	FILEATR_RGRP		(1<<19)									/* �O���[�v���[�h����*/
#	define	FILEATR_WGRP		(1<<20)									/* �O���[�v���C�g����*/
#	define	FILEATR_XGRP		(1<<21)									/* �O���[�v���s	 ����*/
#	define	FILEATR_ROTH		(1<<22)									/* �N�ł�  ���[�h����*/
#	define	FILEATR_WOTH		(1<<23)									/* �N�ł�  ���C�g����*/
#	define	FILEATR_XOTH		(1<<24)									/* �N�ł�  ���s	 ����*/
#	define	FILEATR_DOTDIR		(1<<25)									/* . .. ���o�^����*/

#	define	FILEATR_ALL			(FILEATR_DIR|FILEATR_REG|FILEATR_SYS|FILEATR_NON|FILEATR_LNK|FILEATR_WLD|FILEATR_DOT|FILEATR_FIFO|FILEATR_CHR|FILEATR_BLK|FILEATR_SOCK|FILEATR_NFS)
#	define	FILEATR_IO			(FILEATR_FIFO|FILEATR_CHR|FILEATR_BLK|FILEATR_SOCK)
#	define	FILEATR_RALL		(FILEATR_RUSR|FILEATR_RGRP|FILEATR_ROTH)/* ����	 ���[�h����*/
#	define	FILEATR_WALL		(FILEATR_WUSR|FILEATR_WGRP|FILEATR_WOTH)/* ����	 ���C�g����*/
#	define	FILEATR_XALL		(FILEATR_XUSR|FILEATR_XGRP|FILEATR_XOTH)/* ����	 ���s  ����*/
#	define	FILEATR_REGDIR		(FILEATR_REG|FILEATR_DIR)

	typedef struct{
		long			atr;											// �t�@�C���A�g���r���[�g
		unsigned long	mtim;											// �ŏI�C���^�C���X�^���v
		unsigned long	atim;											// �ŏI�A�N�Z�X�^�C��
		unsigned long	siz;											// �t�@�C���T�C�Y
		LLONG			sizll;											// �t�@�C���T�C�Y
		long			mode;											// ���[�h
#	ifdef	unix
		long			ino;											// i�m�[�h�ԍ�
		long			dev;											// ���u�ԍ�( �t�@�C���V�X�e�� )
		long			rdev;											// ����t�@�C���̑��u�ԍ�
		long			nlink;											// �����N�J�E���g
		long			uid;											// ���[�U�[ID
		long			gid;											// �O���[�vID
		long			blksize;										// �u���b�N�T�C�Y
		long			blocks;											// �u���b�N��
		char			fstype[16];										// �t�@�C���V�X�e����
#	endif
	}	stFILE;

	char	*fileStrAtr( char *dstadr, const stFILE *filep );			// �A�g���r���[�g������쐬
	char	*fileStrTimS( char *dstadr, unsigned long tim2 );			// �^�C���X�^���v������쐬
	char	*fileStrTim( char *dstadr, const stFILE *filep );			// �^�C���X�^���v������쐬
	int		fileStat( stFILE *filep, const char *pacnamp, int fileatr );// �t�@�C�����𓾂�
	void	fileMakDst( char *dstp, const char *sounamp, const char *pathp, const char *extp );	// �o�̓t�@�C�����쐬
	int		filedbWildChk( const char *soup, const char *wldp );		// ���C���h�J�[�h�`�F�b�N
	stDB	*filedbOpen(void);											// �t�@�C��DB�I�[�v��
	stDB	*filedbAlc( stDB *dbp, const char *pacnamp, int fileatr );	// �w��t�@�C����DB�ɓo�^����( ���C���h�J�[�h�g�p�� )
	stDB	*filedbAlcDB( const stDB *argdbp, int fileatr );			// DB����o�^����
	stDB	*filedbDirOpen( const char *dirnamp, int fileatr );			// �f�B���N�g�����I�[�v������
	int		filedbChkDir( const char *dirnamp );						// �f�B���N�g�����݃`�F�b�N
	int		filedbMakDir( const char *dirnamp );						// �f�B���N�g���쐬
	int		filedbMakDirR( const char *dirnamp );						// �ċA�I�Ƀf�B���N�g���쐬
	int		filedbChgMod( const char *fnamp, int mtim, int fatr );		// �t�@�C�����[�h�ύX
	int		filedbRemove( const char *fnamp, int opta, int optt );		// �t�@�C�����̓f�B���N�g���폜

#	ifdef	__cplusplus
	}
#	endif
#	endif

