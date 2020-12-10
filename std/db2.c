/************************************************************************/
/*																		*/
/*	�v���O�������ėp�f�[�^�x�[�X�V�X�e�� by H.Watanabe					*/
/*	Ver0.00	2009/01/09 21:15:32	�쐬�J�n								*/
/*																		*/
/************************************************************************/
/*
��������������������������������������������������������������������������
��	�T��																��
��������������������������������������������������������������������������
	���̃��W���[���́A�v���O�������Ŏg�p����ėp�f�[�^�x�[�X�V�X�e���ł��B
	���Ƃ��Ƃ́A�A�Z���u���̂��߂̃n�b�V���e�[�u���Ǘ���Ɨ����������̂ł��B
	�������A���܂�Ɏg�����肪�悢���߁A���ɑ����̏�ʂŎg�p���Ă��܂��B
	Windows, unix �ǂ���ł��R���p�C���ł��܂��B
	�܂��Amalloc, free ���������Ă���Αg�ݍ��ݗp�ł��C�y�Ɏg�p�ł��܂��B
��������������������������������������������������������������������������
��	�p�r																��
��������������������������������������������������������������������������
	�E�ϒ��z��Ǘ��B
		�P���ȉϒ��z��Ǘ��Ƃ��Ďg�p�ł��܂��B
	�E���[�J���q�[�v�Ǘ�
		�O���[�o���q�[�v������T�C�Y�Ń��������m�ۂ���
		��������[�J���q�[�v�Ƃ��Ďg�p���邱�Ƃ��o���܂��B
		����ɂ��O���[�o���q�[�v�̃t���O�����e�[�V����(�f�Љ�)���h�~�ł��܂��B
		�܂�malloc���g�p������������ł��B
	�E�������Ǘ��B
		dbMemAlc���g�p���ă������m�ۂ������
		dbClose ���Ă΂ꂽ���_�ŐӔC�������Ď����J�������܂��B
	�E���O�t�X�^�b�N�Ǘ��B
		�����L�[�œo�^����ƍŌ�ɓo�^�������̂��L���ɂȂ�܂��B
		�Ō�ɓo�^�����f�[�^���J������ƁA���̑O�̃f�[�^���L���ɂȂ�܂��B
	�E�����B
		�A�Z���u�������ۂɂ͖����Ă͂Ȃ�܂���B
		���x���\���̍��������A�o�^�A�폜���ȒP�ɍs���܂��B
	�E�����̃f�[�^�������悭�����邽�߁A�֐��̈����A�A��l�ɂ��C�y�Ɏg�p�ł��܂��B
��������������������������������������������������������������������������
��	�g�p�@																��
��������������������������������������������������������������������������
	�܂��AdbOpen �֐��Ńf�[�^�x�[�X�Ǘ��\���̂��I�[�v�����܂��B
	dbAlc �֐��Ńf�[�^��o�^���܂�( dbFre�֐��ō폜�ł��܂� )�B
	dbSch �֐��ŃL�[���烊�X�g�������ł��܂��B
	�܂��AdbTop,dbEnd,dbNxt,dbBak �֐��œo�^����Ă�����̂𑍓���ł��܂��B
	���X�g����L�[�̃|�C���^�⃊�X�g�T�C�Y�A�L�[�T�C�Y�����߂�ɂ�,
	dbKEYADR,dbLSTSIZ,dbKEYSIZ�}�N�����g�p���܂��B
	�p���ς񂾂�
	dbClose �֐����R�[�����Ċm�ۂ��Ă������������J�����܂��B
��������������������������������������������������������������������������
��	�L�[�ƃ��X�g�̊Ǘ�													��
��������������������������������������������������������������������������
	�L�[�ƃ��X�g����g�ɂ��ĊǗ����Ă��܂��B
	 (�I�[�v�����̃I�v�V�����ɂ���āA�L�[���A���X�g�����I���ł��܂��B)
	��������̃C���[�W�͈ȉ��̂悤�ɂȂ��Ă��܂��B
	  ���������������������������������������������� ��O�̃|�C���^( �o�������X�g�Ǘ� )
	  ��  ������������������������������������������ ���̃|�C���^( �o�������X�g�Ǘ� )
	  ��  ��  �������������������������������������� ���X�g�T�C�Y
	  ��  ��  ��  ���������������������������������� �L�[�T�C�Y
	  ��  ��  ��  ��				���������������� �A���C��( �L�[�f�[�^�̐擪���m�o�C�g���E�ɍ����悤�� )
	  ��  ��  ��  ��				��
	  ��  ��  ��  ��				��
	������������������������������������������������
	���@���@���@���@�����X�g�f�[�^���@���L�[�f�[�^��
	������������������������������������������������
					��
					��
					�������������������������������� ���[�U�[�ɓn����郊�X�g�|�C���^
��������������������������������������������������������������������������
��	�}�N�����t�@�����X													��
��������������������������������������������������������������������������
	��������������������������������������������������������������������
	dbLSTSIZ(P)		���[�U���X�g�T�C�Y�𓾂�
	��������������������������������������������������������������������
	����:
		P:		���X�g�|�C���^
	���:
		���X�g�|�C���^���烊�X�g�̃T�C�Y���Q�Ƃ��܂��B

	��������������������������������������������������������������������
	dbKEYSIZ(P)		���[�U�L�[�T�C�Y�𓾂�
	��������������������������������������������������������������������
	����:
		P:		���X�g�|�C���^
	���:
		���X�g�|�C���^����L�[�̃T�C�Y���Q�Ƃ��܂��B

	��������������������������������������������������������������������
	dbKEYADR(P)		���[�U�L�[�A�h���X�𓾂�
	��������������������������������������������������������������������
	����:
		P:		���X�g�|�C���^
	���:
		���X�g�|�C���^����L�[�A�h���X���Q�Ƃ��܂��B

	��������������������������������������������������������������������
	dbFOR(H,P,C)	���[�U�[�f�[�^�����胋�[�v
	��������������������������������������������������������������������
	����:
		H:		�f�[�^�x�[�X�|�C���^
		P:		���X�g�|�C���^�ϐ�
		C:		�^��
	���:
		for( P=( C )dbTop( H );P;P=( C )dbNxt( P ) ) �ɓW�J����܂��B
		�o�^����Ă���f�[�^�����Ԃɑ����肷��ꍇ�Ɏg�p���܂��B
		�A���A���X�g�|�C���^�ϐ����g�p���āA���̃��X�g�|�C���^�����߂Ă��邽��
		���̃��[�v�̒��ŁAdbFre( ���X�g�|�C���^�ϐ� )�͎w��ł��܂���B
		���̏ꍇ�� dbFOR2 �}�N�����g�p���Ă��������B

	��������������������������������������������������������������������
	dbFOR2(H,P,N,C)	���[�U�[�f�[�^�����胋�[�v
	��������������������������������������������������������������������
	����:
		H:		�f�[�^�x�[�X�|�C���^
		P:		���X�g�|�C���^�ϐ�
		N:		�e���|�������X�g�|�C���^�ϐ�( ���̃��X�g�|�C���^�������܂� )
		C:		�^��
	���:
		for( P=( C )dbTop( H ); N=( C )dbNxt( P ), P; P=N ) �ɓW�J����܂��B
		�o�^����Ă���f�[�^�����Ԃɑ����肷��ꍇ�Ɏg�p���܂��B
		dbFOR* �}�N���̒��ł͈�Ԕėp���������̂ł����A
		�e���|�������X�g�|�C���^���������Ȃ���΂Ȃ�Ȃ��̂ŏ��X�ʓ|�ł��B
		���̃��[�v�̒��ŁAdbFre( ���X�g�|�C���^�ϐ� )�����s����ꍇ�̂ݎg�p���Ă��܂��B
*/

#	include	<assert.h>													/* �A�T�[�V����*/
#	include	<stdarg.h>													/* �ψ���������`*/
#	include	<stdlib.h>													/* �W�����C�u�����֐�*/
#	include	<string.h>													/* �W��������֐�*/
#	include	"db2.h"														/* �v���O�������ėp�f�[�^�x�[�X�V�X�e��*/

/************************************************************************/
/*	�������m�ہE�J����`												*/
/************************************************************************/

#	ifdef	DB_MALLOC
		void	*DB_MALLOC( int );
		void	DB_FREE( void * );
#	else
#		define	DB_MALLOC	malloc
#		define	DB_FREE		free
#	endif

/************************************************************************/
/*	���[�J����`														*/
/************************************************************************/

#	define	dbLNK(BAK,NXT)		((BAK)->nxtp=(NXT),(NXT)->bakp=(BAK))	/* �o�������X�g�����N*/
#	define	dbDEL(LST)			dbLNK((LST)->bakp,(LST)->nxtp)			/* �o�������X�g�폜*/
#	define	dbINI(TOP,END)		((TOP)->bakp=(END)->nxtp=0,dbLNK(TOP,END))		/* �o�������X�g������*/
#	define	dbINSBAK(NEW,NXT)	(dbLNK((NXT)->bakp,NEW),dbLNK(NEW,NXT))	/* �o�������X�g�}��*/
#	define	dbINSNXT(BAK,NEW)	(dbLNK(NEW,(BAK)->nxtp),dbLNK(BAK,NEW))	/* �o�������X�g����}��*/
#	define	dbUDELNXT(BAK)		((BAK)->slp=(BAK)->slp->slp)			/* �P�������X�g�폜*/
#	define	dbUINSNXT(BAK,NEW)	((NEW)->slp=(BAK)->slp,(BAK)->slp=(NEW))/* �P�������X�g�}��*/

	typedef struct	tgMCB{												// �������R���g���[���u���b�N�\����
		struct tgMCB	*nxtp, *bakp;									// �o�������X�g
		stDB			*dbp;											// ���̃u���b�N���m�ۂ����q�[�v�Ǘ��\���̃|�C���^
		int				blk;											// �u���b�N��( �ꕔ�Ńo�C�g�T�C�Y�Ƃ��Ďg�p���Ă��� )
	}	stMCB;

	typedef struct tgMLST	stMLST;
	struct	tgMLST{														// �������Ǘ��̂��߂̍\����
		stMLST			*nxtp, *bakp;									// �o�������X�g�|�C���^
		stDB			*dbp;											// ���̃u���b�N���m�ۂ����q�[�v�Ǘ��\���̃|�C���^
		int				siz;											// �m�ۃT�C�Y
	};

	typedef struct tgSL	stSL;
	struct	tgSL{														// �P�������X�g�Ǘ�
		stSL			*slp;											// �P�������X�g�|�C���^
	};

	struct	tgDB{														// �f�[�^�x�[�X�|�C���^
		stSL			*slpadr;										// �n�b�V���e�[�u���A�h���X
		stDBLNK			lnktop, lnkend;									// �o�������X�g�_�~�[�擪�A�I�[
		stMLST			mlsttop, mlstend;								// �������m�ۑo�������X�g�_�~�[�擪�A�I�[
		int				tblmsk;											// �n�b�V���l�}�X�N
		int				keysiz;											// ���[�U�[�L�[�T�C�Y
		int				lstsiz;											// ���[�U�[���X�g�T�C�Y
		int				alcn;											// �o�^��
		unsigned int	hpsiz;											// ���[�J���q�[�v�T�C�Y
		stMCB			fretop, freend;									// ��	 ( �o�������X�g�_�~�[�擪�E�I�[ )
		stMCB			usetop, useend;									// �g�p��( �o�������X�g�_�~�[�擪�E�I�[ )
		stMCB			alctop, alcend;									// �O���[�o���������m��( �o�������X�g�_�~�[�擪�E�I�[ )
	};

/************************************************************************/
/*	�q�[�v���															*/
/************************************************************************/

static void	memFre( stDB *dbp, void *memp ){

	stMCB		*mcbp, *bakp, *p, *nxtp;								// �o�������X�g�A�N�Z�X���[�N

	if( memp ){															// �L���ȃ������A�h���X
		if( dbp->hpsiz ){												// ���[�J���q�[�v�L��
			mcbp=(stMCB*)memp-1;										// ���[�U�ɓn���l�͊Ǘ��|�C���^�{�P
			assert( mcbp->dbp==dbp );									// �q�[�v�Ǘ��|�C���^�`�F�b�N
			assert( mcbp->nxtp->dbp==dbp );								// ���̃q�[�v�Ǘ��|�C���^�`�F�b�N
			assert( mcbp->nxtp->bakp==mcbp );							// ���̑o�������X�g�����N�`�F�b�N
			assert( mcbp->bakp->dbp==dbp );								// �O�̃q�[�v�Ǘ��|�C���^�`�F�b�N
			assert( mcbp->bakp->nxtp==mcbp );							// �O�̑o�������X�g�����N�`�F�b�N
			dbDEL( mcbp );												// �g�p�����X�g����̊J��
			for( p=dbp->fretop.nxtp; ( nxtp=p->nxtp )!=0; p=nxtp ){		// �󂫃G���A����
				if( mcbp<p ){
					break;
				}
			}
			bakp=p->bakp;
			if( nxtp ){													// �Ō���ł͂Ȃ�
				assert( !bakp->bakp || bakp+bakp->blk<=mcbp );
				dbINSBAK( mcbp, p );									// �Ƃ肠�����}��
				if( mcbp+mcbp->blk==p ){								// ��u���b�N�̒��O
					dbLNK( mcbp, p->nxtp );								// ��u���b�N�폜
					mcbp->blk+=p->blk;									// �J�����g�u���b�N�T�C�Y���Z
				}
				if( bakp+bakp->blk==mcbp ){								// �O�u���b�N�̒���
					dbLNK( bakp, mcbp->nxtp );							// �J�����g�u���b�N�폜
					bakp->blk+=mcbp->blk;								// �O�u���b�N�T�C�Y���Z
				}
			}else{														// �Ō��
				if( bakp+bakp->blk==mcbp ){								// �O�u���b�N�̒���
					bakp->blk+=mcbp->blk;								// �O�u���b�N�T�C�Y���Z
				}else{
					dbINSBAK( mcbp, p );								// �Ō���ɑ}��
				}
			}
		}else{															// ���[�J���q�[�v����
			DB_FREE( memp );											// �O���[�o���������J��
		}
	}
}

/************************************************************************/
/*	�q�[�v�t���[�̈��ǉ�												*/
/************************************************************************/

static void	memAddFre( stDB *dbp, stMCB *mcbp, int siz ){

	mcbp->blk=siz/sizeof(stMCB);										// �u���b�N�T�C�Y�̌v�Z
	mcbp->dbp=dbp;														// ���̃u���b�N���m�ۂ����q�[�v�Ǘ��|�C���^
	dbINSBAK( mcbp, &dbp->useend );										// ��ŉ�������̂Ń_�~�[�o�^
	memFre( dbp, mcbp+1 );												// ���
}

/************************************************************************/
/*	�q�[�v�m��															*/
/************************************************************************/

static void	*memAlc( stDB *dbp, unsigned int siz ){

	stMCB			*mcbp=0;											// �������R���g���[���u���b�N�|�C���^
	stMCB			*p, *nxtp;											// �o�������X�g�A�N�Z�X���[�N
	int				blk, modblk;										// �u���b�N��
	unsigned int	alcsiz;												// �m�ۃT�C�Y

	if( siz ){															// �L���ȃT�C�Y
		if( dbp->hpsiz ){												// ���[�J���q�[�v�L��
			blk=( siz+(sizeof(stMCB)*2-1) )/sizeof( stMCB );			// �K�v�T�C�Y�̌v�Z
			for( p=dbp->fretop.nxtp; ( nxtp=p->nxtp )!=0; p=nxtp ){		// �擪���猟��
				if( p->blk>=blk ){										// �K�v�T�C�Y�ȏ�
					break;												// ���[�v�I��
				}
			}
			if( nxtp ){													// �󂫂���
				mcbp=p;													// �������R���g���[���u���b�N�|�C���^�Z�b�g
				modblk=p->blk-blk;										// �]��󂫃T�C�Y
				if( modblk>=2 ){										// �Q�u���b�N�ȏ�]�v�ɂ���̂ŕ�������
					nxtp=mcbp+blk;										// �V�����쐬����u���b�N
					nxtp->blk=modblk;									// �V�����쐬����u���b�N�T�C�Y
					mcbp->blk=blk;										// ���݂̃u���b�N�T�C�Y�ύX
					dbINSNXT( mcbp, nxtp );								// �V�u���b�N�}��
				}
				dbDEL( mcbp );											// �󂫃��X�g����폜
				dbINSBAK( mcbp, &dbp->useend );							// �g�p�����X�g�ɒǉ�
				mcbp->dbp=dbp;											// �m�ۂ����Ǘ����[�N�A�h���X�Z�b�g
				mcbp++;													// ���[�U�ɓn���l�̓������R���g���[���u���b�N�|�C���^�{�P
			}else{														// �󂫖���
				alcsiz=((siz+3*sizeof(stMCB)-1)/sizeof(stMCB))*sizeof(stMCB);
				if( alcsiz<dbp->hpsiz ){
					alcsiz=dbp->hpsiz;
				}
				mcbp=(stMCB*)DB_MALLOC( alcsiz );						// �O���[�o���������m��
				mcbp->blk=alcsiz;										// �������m�ۃT�C�Y�Ƃ��Ďg�p���Ă���
				mcbp->dbp=0;											// �O�̂���
				dbINSBAK( mcbp, &dbp->alcend );							// �m�ے����X�g�ɒǉ�
				memAddFre( dbp, mcbp+1, alcsiz-sizeof(stMCB) );			// �q�[�v�t���[�̈��ǉ�
				mcbp=(stMCB*)memAlc( dbp, siz );						// �ċA�R�[��
			}
		}else{															// ���[�J���q�[�v����
			mcbp=(stMCB*)DB_MALLOC( siz );								// �O���[�o���������m��
		}
	}
	return( mcbp );														// 0:�m�ەs�\, 0�ȊO�m�ۂ����������A�h���X
}

/************************************************************************/
/*	�n�b�V���l�v�Z���ăe�[�u���̐擪�����߂�							*/
/************************************************************************/

static stSL	*dbHash( const stDB *dbp, const char *keyp, int keysiz ){

	int		hash=0;														// �n�b�V���l�v�Z�p���[�N
	int		i;															// ���[�v�J�E���^

	if( dbp->keysiz==DBSIZ_RSV ){										// �啶������������ʂ��Ȃ�������
		for( i=0; i<keysiz; i++ ){										// ���[�U�[�L�[�T�C�Y�����[�v
			hash=((*keyp++)|0x20)+(hash<<6)-hash;						// ���̒l��63�{���ĐV�����l�𑫂�
		}
	}else{																// �o�C�i���[�܂��͑啶������������ʂ��镶����
		for( i=0; i<keysiz; i++ ){										// ���[�U�[�L�[�T�C�Y�����[�v
			hash=(*keyp++)+(hash<<6)-hash;								// ���̒l��63�{���ĐV�����l�𑫂�
		}
	}
	return( &dbp->slpadr[hash&dbp->tblmsk] );							// �P�������X�g�擪
}

/************************************************************************/
/*	�f�[�^�x�[�X���I�[�v�����܂�										*/
/************************************************************************/

//	����:
//		keysiz:	�L�[�T�C�Y�܂��̓L�[�^�C�v���w�肵�܂��B
//			���̒l�̏ꍇ�̓L�[�T�C�Y�̎w��ł��B
//				0���w�肷��ƃL�[���ɂȂ�܂��B
//				����ȊO�͌Œ蒷�̃L�[�T�C�Y�Ƃ��ĔF������܂��B
//			���̒l�̏ꍇ�͗\��l�ł�:
//				DBSIZ_VAR:	�o�^�A�����̏ꍇ�ɃA�h���X�̌�ɃT�C�Y���w�肵�܂��B
//				DBSIZ_STR:	������Ƃ��Ĉ����܂��B
//				DBSIZ_RSV:	�啶������������ʂ��Ȃ�������Ƃ��Ĉ����܂�( Windows�̃t�@�C�����Ǘ��ɂ悭�g�p���܂� )�B
//				DBSIZ_LBL:	���x��������Ƃ��Ĉ����܂�
//				DBSIZ_INT:	���l�Ƃ��Ĉ����܂�
//				DBSIZ_PTR:	�|�C���^�Ƃ��Ĉ����܂�
//		lstsiz:	���X�g�T�C�Y�܂��̓��X�g�^�C�v���w�肵�܂��B
//			���̒l�̏ꍇ�̓��X�g�T�C�Y�̎w��ł��B
//				0���w�肷��ƃ��X�g���ɂȂ�܂��B
//				����ȊO�͌Œ蒷�̃��X�g�T�C�Y�Ƃ��ĔF������܂��B
//			���̒l�̏ꍇ�͗\��l�ł�:
//				DBSIZ_VAR:	�o�^�A�����̏ꍇ�ɃA�h���X�̌�ɃT�C�Y���w�肵�܂��B
//				DBSIZ_STR:	������Ƃ��Ĉ����܂��B
//				DBSIZ_RSV:	�啶������������ʂ��Ȃ�������Ƃ��Ĉ����܂��B
//				DBSIZ_LBL:	���x��������Ƃ��Ĉ����܂�
//				DBSIZ_INT:	���l�Ƃ��Ĉ����܂�
//				DBSIZ_PTR:	�|�C���^�Ƃ��Ĉ����܂�
//		hashn:	�n�b�V���e�[�u�����̎w��ł��B
//				0���w�肷��ƃn�b�V���Ǘ����s���܂���B
//				( ���̏ꍇ�Ɍ�������ƃ��j�A���������܂� )
//				0�łȂ��ꍇ�͂Q�̂m�搔�łȂ���΂Ȃ�܂���B
//				2�̂m�搔�łȂ��ꍇ�̓A�T�[�V�����ƂȂ�܂��B
//				�I�[�v���n�b�V���@���g�p���Ă��܂��̂ŁA
//				�e�[�u���T�C�Y�͓o�^�������������Ă����܂��܂���B
//				���̏ꍇ�A256,1024,4096���悭�g�p���܂��B
//		hpsiz:	���[�J���q�[�v�T�C�Y
//				0���w�肷��ƁA���[�J���q�[�v���g�p���܂���B
//				( �o�^�̂��тɃO���[�o�����烁�����m�ۂ��܂� )
//				0�łȂ��ꍇ�� hpsiz ���܂Ƃ߂ă��[�J���q�[�v���m�ۂ���
//				�o�^�̓��[�J���q�[�v���g�p���܂��B
//	�߂�l:
//		���������ꍇ�̓f�[�^�x�[�X�|�C���^���A��܂��B
//		���s�����ꍇ��NULL���A��܂��B
//	���:
//		�w�肳�ꂽ�L�[�T�C�Y�A���X�g�T�C�Y�A�n�b�V���e�[�u�����ɏ]����
//		�f�[�^�x�[�X���m�ۂ��ď��������܂��B
//	��:
//		typedef struct{
//			int		val;
//		}	stLST;
//		stDB	*dbp;
//
//		�E�L�[�͕�����A���X�g�T�C�Y�͌Œ�T�C�Y�A�n�b�V���e�[�u������256�ŃI�[�v������
//
//		dbp=dbOpen( DBSIZ_STR, sizeof(stLST), 256, 0 );
//
//		�E�L�[�͑啶������������ʂ��Ȃ�������A���X�g�T�C�Y�͖��A�n�b�V���e�[�u������4096�ŃI�[�v������
//
//		dbp=dbOpen( DBSIZ_RSV, 0, 4096, 0 );
//
//		�E�L�[�͖��A���X�g�T�C�Y�͓o�^�E�������Ɏw��A�n�b�V���e�[�u�����A�u���b�N�T�C�Y0x4000�o�C�g�ŃI�[�v������
//		  ( �P�Ȃ�ϒ��z��Ǘ��Ƃ��Ďg�p�������ꍇ�ɑ����w��ł��B)
//
//		dbp=dbOpen( 0, DBSIZ_VAR, 0, 0x4000 );

stDB	*dbOpen( int keysiz, int lstsiz, int hashn, int hpsiz ){

	stDB	*dbp;														// �f�[�^�x�[�X�|�C���^

	assert( hashn==(hashn&-hashn) );									// �n�b�V���e�[�u���T�C�Y�͂Q�̂m�搔�ł���
	dbp=(stDB*)DB_MALLOC( sizeof(stDB)+sizeof(stSL*)*hashn );			// �f�[�^�x�[�X�Ǘ��\���̂ƃn�b�V���e�[�u���������m��
	assert( dbp );														// �������m�ۃ`�F�b�N
	memset( dbp, 0, sizeof(stDB)+sizeof(stSL*)*hashn );					// �f�[�^�x�[�X�Ǘ��\���̂ƃn�b�V���e�[�u���������N���A
	dbINI( &dbp->lnktop, &dbp->lnkend );								// �o�������X�g������
	dbINI( &dbp->mlsttop, &dbp->mlstend );								// �o�������X�g������
	dbp->keysiz=keysiz;													// ���[�U�[�L�[�T�C�Y
	dbp->lstsiz=lstsiz;													// ���[�U�[���X�g�T�C�Y
	dbp->tblmsk=hashn-1;												// �n�b�V���e�[�u���}�X�N�Z�b�g
	if( hashn ){														// �n�b�V���e�[�u�����g�p����
		dbp->slpadr=(stSL*)(dbp+1);										// �n�b�V���e�[�u���A�h���X�Z�b�g
	}
	if( hpsiz ){														// ���[�J���q�[�v�L��
		dbp->hpsiz=hpsiz;												// ���[�J���q�[�v
		dbINI( &dbp->fretop, &dbp->freend );							// �󂫃������R���g���[���u���b�N���X�g������
		dbINI( &dbp->usetop, &dbp->useend );							// �g�p�������R���g���[���u���b�N���X�g������
		dbINI( &dbp->alctop, &dbp->alcend );							// ���[�J���q�[�v�������R���g���[���u���b�N������
		dbp->fretop.dbp=dbp;											// �������J�����̃`�F�b�N�p
		dbp->freend.dbp=dbp;											// �������J�����̃`�F�b�N�p
		dbp->usetop.dbp=dbp;											// �������J�����̃`�F�b�N�p
		dbp->useend.dbp=dbp;											// �������J�����̃`�F�b�N�p
	}
	return( dbp );														// �f�[�^�x�[�X�|�C���^�������ă��^�[��
}

/************************************************************************/
/*	�f�[�^�x�[�X���N���[�Y���܂�										*/
/************************************************************************/

//	����:
//		dbp:	�f�[�^�x�[�X�|�C���^
//	���:
//		�m�ۂ��Ă�����������S�ĊJ�����܂��B
//		dbp �� NULL ���w�肷��Ɖ������܂���B

void	dbClose( stDB *dbp ){

	stDBLNK		*p, *nxtp;												// �o�������X�g�|�C���^
	stMLST		*mp, *nxtmp;											// �o�������X�g�|�C���^
	stMCB		*mcbp, *nxtmcbp;										// ���[�J���q�[�v�m�ۑo�������X�g�|�C���^

	if( dbp ){															// �L���ȃf�[�^�x�[�X�|�C���^
		if( dbp->hpsiz ){												// ���[�J���q�[�v�L��
			for( mcbp=dbp->alctop.nxtp; (nxtmcbp=mcbp->nxtp)!=0; mcbp=nxtmcbp ){				// �������m�ۑ����胋�[�v
				DB_FREE( mcbp );										// �O���[�o���������J��
			}
		}else{															// ���[�J���q�[�v����
			for( mp=dbp->mlsttop.nxtp; (nxtmp=mp->nxtp)!=0; mp=nxtmp ){	// ���[�J���������Ǘ��\���̃��[�v
				DB_FREE( mp );											// ���[�J���������J��
			}
			for( p=dbp->lnktop.nxtp; (nxtp=p->nxtp)!=0; p=nxtp ){		// �o�������X�g���[�v
				if( dbp->slpadr ){										// �n�b�V���\���g�p����Ă���
					DB_FREE( ((char *)p)-dbALG(sizeof(stSL)) );			// �P�������X�g���g�p���Ă���̂�-1�����l���J��
				}else{													// �n�b�V���\�͎g�p���Ă��Ȃ�
					DB_FREE( p );										// �����N�|�C���^���̂܂܊J��
				}
			}
		}
#	ifndef	NDEBUG
		memset( dbp, 0, sizeof(stDB) );
#	endif
		DB_FREE( dbp );													// �f�[�^�x�[�X�Ǘ��\���̃������J��
	}
}

/************************************************************************/
/*	�f�[�^�x�[�X�Ƀf�[�^���t���X�y�b�N�œo�^���܂�						*/
/************************************************************************/

//	����:
//		dbp:	�f�[�^�x�[�X�|�C���^
//		keyadr:	�L�[�A�h���X
//		keysiz:	�L�[�T�C�Y
//		lstadr:	���X�g�A�h���X
//		lstsiz:	���X�g�T�C�Y
//	�߂�l:
//		�o�^�������X�g�̃|�C���^���A��܂��B
//	���:
//		�f�[�^�x�[�X�Ƀt���X�y�b�N�Ńf�[�^��o�^���܂��B
//		dbAlc �֐��̉������ł��B
//		���̊֐����g�p���邱�Ƃɂ��AdbOpen �Ŏw�肵���L�[�⃊�X�g�𖳎�����
//		�o�^���邱�Ƃ��o���܂��B

void	*dbAlcS( stDB *dbp, const void *keyadr, int keysiz, const void *lstadr, int lstsiz ){

	void		*lstp;													// ���[�U�[���X�g�|�C���^
	int			keyalg, lstalg;											// �A���C�����ꂽ���[�U�[�L�[�T�C�Y�A���[�U�[���X�g�T�C�Y
	stSL		*bakp, *slp;											// �P�������X�g�|�C���^
	stDBLNK		*lnkp;													// �����N�|�C���^
	char		*wrtp;													// �������������݂̂��߂̃e���|�����|�C���^

	keyalg=dbALG( keysiz );												// �A���C���ς݂̃��[�U�[�L�[�T�C�Y
	lstalg=dbALG( lstsiz );												// �A���C���ς݂̃��[�U�[���X�g�T�C�Y
	if( dbp->slpadr ){													// �n�b�V���\���g�p����Ă���
		slp=(stSL*)memAlc( dbp, dbALG(sizeof(stSL))+sizeof(stDBLNK)+lstalg+keyalg );			// �P�������X�g��擪�Ƀ������m��
		assert( slp );
		lnkp=(stDBLNK*)((char*)slp+dbALG(sizeof(stSL)));				// �����N�|�C���^�͒P�������X�g�|�C���^�{�P
		bakp=dbHash( dbp, (char*)keyadr, keysiz );						// �n�b�V���l�v�Z���ăe�[�u���̐擪�����߂�
		dbUINSNXT( bakp, slp );											// �P�������X�g�}��
	}else{																// �n�b�V���\�͎g�p���Ă��Ȃ�
		lnkp=(stDBLNK*)memAlc( dbp, sizeof(stDBLNK)+lstalg+keyalg );	// �����N�|�C���^��擪�ɂ��ă������m��
		assert( lnkp );
	}
	lstp=wrtp=(char*)(lnkp+1);											// �A��l
	if( (lnkp->lstsiz=lstsiz)!=0 ){										// ���[�U�[���X�g�T�C�Y�͗L��
		if( lstadr ){													// ���[�U�[���X�g�A�h���X�͗L��
			memcpy( wrtp, lstadr, lstsiz );								// ���[�U�[���X�g�f�[�^�R�s�[
			memset( wrtp+lstsiz, 0, lstalg-lstsiz );					// �A���C���G���A�N���A
		}else{															// ���[�U�[���X�g�A�h���X�͖���
			memset( wrtp, 0, lstalg );									// ���[�U�[���X�g�������N���A
		}
		wrtp+=lstalg;													// �������݃A�h���X���Z
	}
	if( (lnkp->keysiz=keysiz)!=0 ){										// ���[�U�[�L�[�T�C�Y�͗L��
		if( keyadr ){													// ���[�U�[�L�[�A�h���X�͗L��
			memcpy( wrtp, keyadr, keysiz );								// ���[�U�[�L�[�f�[�^�R�s�[
			memset( wrtp+keysiz, 0, keyalg-keysiz );					// �A���C���G���A�N���A
		}else{															// ���[�U�[�L�[�A�h���X�͖���
			memset( wrtp, 0, keyalg );									// ���[�U�[�L�[�������N���A
		}
	}
	dbINSBAK( lnkp, &dbp->lnkend );										// �o�������X�g�}��
	dbp->alcn++;														// �o�^�����Z
	return( lstp );														// ���[�U�[���X�g�|�C���^���A��l�Ƃ���
}

/************************************************************************/
/*	�ψ������������ăf�[�^�x�[�X�Ƀf�[�^��o�^���܂�					*/
/************************************************************************/

//	����:
//		dbp:	�f�[�^�x�[�X�|�C���^
//		...:	�L�[�ƃ��X�g�̏������ԂŎw�肵�܂��B
//				�������A�ǂ̏��Ԃŉ����w�肷�邩�́AdbOpen �Ŏw�肵���L�[�ƃ��X�g�̃T�C�Y�ɂ���ĕς��܂��B
//				�T�C�Y���O�̏ꍇ:
//					�����̂�<�f�[�^�A�h���X>�A<�f�[�^�T�C�Y>�Ƃ��w�肵�܂���B
//				�T�C�Y���P�ȏ�̐��̒l�̏ꍇ:
//					<�f�[�^�T�C�Y>�͌Œ�T�C�Y�ł��łɕ������Ă���̂ŁA
//					<�f�[�^�A�h���X>�������w�肵�܂��B
//				�T�C�Y�� DBSIZ_VAR �̏ꍇ
//					<�f�[�^�A�h���X>��<�f�[�^�T�C�Y>���w�肵�܂��B
//				�T�C�Y�� DBSIZ_STR, DBSIZ_RSV�̏ꍇ
//					�������<�f�[�^�A�h���X>���w�肵�܂��B
//
//				�܂��A<�f�[�^�A�h���X>��NULL�̏ꍇ�ɂ̓[���N���A���ꂽ���������w�肳�ꂽ���̂Ƃ݂Ȃ��܂��B
//
//				��:
//					�Edbp=dbOpen( DBSIZ_VAR, DBSIZ_VAR, ???, ??? )�@�Ǝw�肵���ꍇ
//
//					dbAlc( dbp, <�L�[�A�h���X>, <�L�[�T�C�Y>, <���X�g�A�h���X>, <���X�g�T�C�Y> )
//
//					�Edbp=dbOpen( 0, DBSIZ_STR, ???, ??? )�@�Ǝw�肵���ꍇ
//
//					dbAlc( dbp, <���X�g�A�h���X> )
//
//	�߂�l:
//		�o�^�������X�g�̃|�C���^���A��܂��B
//	���:
//		�f�[�^�x�[�X�Ƀf�[�^��o�^���܂��B
//		���[�U�[�f�[�^�͓����Ǘ��������ɃR�s�[����܂��̂ŁA
//		���[�U�[�f�[�^�͕ێ����Ă����K�v�͂���܂���B

void	*dbAlc( stDB *dbp, ... ){

	va_list		ap;														// �ψ������[�N
	void		*lstp=0;												// �A��l
	char		*keyadr=0, *lstadr=0;									// ���[�U�[�L�[�A�h���X�A���[�U�[���X�g�A�h���X
	int			keysiz, lstsiz;											// ���[�U�[�L�[�T�C�Y�A���[�U�[���X�g�T�C�Y
	char		keybuf[1024], lstbuf[1024];								// ���x���o�b�t�@
	int			keyint, lstint;											// ���l

	if( dbp ){															// �f�[�^�x�[�X�|�C���^�`�F�b�N
		va_start( ap, dbp );											// �ψ����I�[�v��
		if( ( keysiz=dbp->keysiz )!=0 ){								// �L�[�T�C�Y�����݂���
			keyadr=va_arg( ap, char * );								// �������烆�[�U�[�L�[�A�h���X�ǂݍ���
			switch( keysiz ){											// ���ꃆ�[�U�[�L�[�T�C�Y�̔���
				case DBSIZ_VAR:											// �L�[�T�C�Y�͉�
					keysiz=va_arg( ap, int );							// �ψ�������L�[�T�C�Y�𓾂�
					break;
				case DBSIZ_STR:											// ������Ƃ��Ĉ����܂��B
				case DBSIZ_RSV:											// �啶������������ʂ��Ȃ�������Ƃ��Ĉ����܂�
					keysiz=1;											// �L�[�A�h���X��0�̏ꍇ�̃L�[�T�C�Y
					if( keyadr ){										// �L�[�A�h���X���w�肳��Ă���
						keysiz=strlen( keyadr )+1;						// ���[�U�[�L�[�T�C�Y�͕����񒷁{�P
					}
					break;
				case DBSIZ_LBL:											// ���x��
					keysiz=dbLblLen( keyadr, sizeof(keybuf) )+1;		// �L�[�T�C�Y
					memcpy( keybuf, keyadr, keysiz-1 );					// �o�b�t�@�ɃR�s�[
					keybuf[keysiz-1]=0;									// �O�^�[�~�l�[�g
					keyadr=keybuf;										// �L�[�A�h���X�̓o�b�t�@
					break;
				case DBSIZ_INT:											// ���l
				case DBSIZ_PTR:											// �|�C���^
					keyint=(int)keyadr;									// �e���|����
					keyadr=(char*)&keyint;								// �L�[�A�h���X
					keysiz=sizeof(int);									// �L�[�T�C�Y
					break;
			}
		}
		if( ( lstsiz=dbp->lstsiz )!=0 ){								// ���X�g�T�C�Y�����݂���
			lstadr=va_arg( ap, char * );								// �������烆�[�U�[���X�g�A�h���X�ǂݍ���
			switch( lstsiz ){											// ���ꃆ�[�U�[���X�g�T�C�Y�̔���
				case DBSIZ_VAR:											// ���[�U�[���X�g�T�C�Y�͓o�^���Ɏw��
					lstsiz=va_arg( ap, int );							// �ψ������烊�X�g�T�C�Y�𓾂�
					break;
				case DBSIZ_STR:											// ������Ƃ��Ĉ����܂��B
				case DBSIZ_RSV:											// �啶������������ʂ��Ȃ�������Ƃ��Ĉ����܂�
					lstsiz=1;											// ���X�g�A�h���X��0�̏ꍇ�̃��X�g�T�C�Y
					if( lstadr ){										// ���X�g�A�h���X���w�肳��Ă���
						lstsiz=strlen( lstadr )+1;						// ���[�U�[���X�g�T�C�Y�͕����񒷁{�P
					}
					break;
				case DBSIZ_LBL:											// ���x��
					lstsiz=dbLblLen( lstadr, sizeof(lstbuf) )+1;		// ���X�g�T�C�Y
					memcpy( lstbuf, lstadr, lstsiz-1 );					// �o�b�t�@�ɃR�s�[
					lstbuf[lstsiz-1]=0;									// �O�^�[�~�l�[�g
					lstadr=lstbuf;										// ���X�g�A�h���X�̓o�b�t�@
					break;
				case DBSIZ_INT:											// ���l
				case DBSIZ_PTR:											// �|�C���^
					lstint=(int)lstadr;									// �e���|����
					lstadr=(char*)&lstint;								// �L�[�A�h���X
					lstsiz=sizeof(int);									// �L�[�T�C�Y
					break;
			}
		}
		va_end( ap );													// �ψ����N���[�Y
		lstp=dbAlcS( dbp, keyadr, keysiz, lstadr, lstsiz );				// �f�[�^�x�[�X�Ƀf�[�^��o�^���܂�
	}
	return( lstp );
}

/************************************************************************/
/*	�L�[��r															*/
/************************************************************************/

static int	dbKeyCmp( const stDB *dbp, const void *dstadr, int dstsiz, const void *souadr, int sousiz ){

	int			res=0;													// 0:�Ⴄ, 1:����

	if( dstsiz==sousiz ){												// �����T�C�Y�ł���
		if( dbp->keysiz==DBSIZ_RSV ){									// �\���( �啶������������ʂ��Ȃ� )
			res=!dbMemCaseCmp( dstadr, souadr, sousiz );				// �啶������������ʂ��Ȃ������Ή���������r
		}else{															// �ʏ�f�[�^��
			res=!memcmp( dstadr, souadr, sousiz );						// �ׂ���r������
		}
	}
	return( res );														// �A��l( 0:�Ⴄ, 1:���� )
}

/************************************************************************/
/*	�f�[�^�x�[�X����L�[�ɂ��������Č������܂�							*/
/************************************************************************/

static void	*dbSchS( const stDB *dbp, const void *keyadr, int keysiz ){

	void		*lstp=0;												// ���[�U�[���X�g�|�C���^
	int			lstalg;													// �A���C�����ꂽ���[�U�[���X�g�T�C�Y
	stSL		*bakp, *slp;											// �P�������X�g�|�C���^
	stDBLNK		*p, *nxtp;												// �o�������X�g�|�C���^
	int			find;													// ���������t���O

	if( dbp->slpadr ){													// �n�b�V���\���g�p����Ă���
		bakp=dbHash( dbp, (char*)keyadr, keysiz );						// �n�b�V���l�v�Z���ăe�[�u���̐擪�����߂�
		while( (slp=bakp->slp)!=0 ){									// �P�������X�g�����N���[�v
			p=(stDBLNK *)((char*)slp+dbALG(sizeof(stSL)));				// �����N�|�C���^
			lstalg=dbALG( p->lstsiz );									// �A���C�����ꂽ���X�g�T�C�Y
			find=dbKeyCmp( dbp, (char*)(p+1)+lstalg, p->keysiz, keyadr, keysiz );
			if( find ){													// ��������
				lstp=(p+1);												// �A��l�Z�b�g
				break;													// ���[�v�I��
			}
			bakp=slp;													// �P�������X�g�|�C���^�X�V
		}
	}else{																// �n�b�V���\�͎g�p���Ă��Ȃ��̂Ńx�^�T��
		for( p=dbp->lnkend.bakp; (nxtp=p->bakp)!=0; p=nxtp ){			// �o�������X�g���[�v
			lstalg=dbALG( p->lstsiz );									// �A���C�����ꂽ���X�g�T�C�Y
			find=dbKeyCmp( dbp, (char*)(p+1)+lstalg, p->keysiz, keyadr, keysiz );
			if( find ){
				lstp=(p+1);												// �A��l�Z�b�g
				break;													// ���[�v�I��
			}
		}
	}
	return( lstp );														// ���X�g�|�C���^���A��l�Ƃ���
}

/************************************************************************/
/*	�ψ������������ăf�[�^�x�[�X����L�[�ɂ��������Č������܂�		*/
/************************************************************************/

//	����:
//		dbp:	�f�[�^�x�[�X�|�C���^
//		...:	�L�[�̏����w�肵�܂��B
//				�������A�ǂ̏��Ԃŉ����w�肷�邩�́AdbOpen �Ŏw�肵���L�[�̃T�C�Y�ɂ���ĕς��܂��B
//				�T�C�Y���O�̏ꍇ:
//					�����ł��B
//				�T�C�Y���P�ȏ�̐��̒l�̏ꍇ:
//					<�f�[�^�T�C�Y>�͌Œ�T�C�Y�ł��łɕ������Ă���̂ŁA
//					<�f�[�^�A�h���X>�������w�肵�܂��B
//				�T�C�Y�� DBSIZ_VAR �̏ꍇ
//					<�f�[�^�A�h���X>��<�f�[�^�T�C�Y>���w�肵�܂��B
//				�T�C�Y�� DBSIZ_STR, DBSIZ_RSV�̏ꍇ
//					�������<�f�[�^�A�h���X>���w�肵�܂��B
//				��:
//					�Edbp=dbOpen( DBSIZ_VAR, ???, ???, ??? )�@�Ǝw�肵���ꍇ
//
//					dbSch( dbp, <�L�[�A�h���X>, <�L�[�T�C�Y> )
//
//					�Edbp=dbOpen( DBSIZ_STR, ???, ???, ??? )�@�Ǝw�肵���ꍇ
//
//					dbSch( dbp, <������A�h���X> )
//
//	�߂�l:
//		�����������X�g�̃|�C���^���A��܂��B
//		������Ȃ��ꍇ�ɂ�NULL���A��܂��B
//	���:
//		�f�[�^�x�[�X�ɂ���L�[�ɓ��Ă͂܂�f�[�^���������܂��B
//		�����A�n�b�V���\���g�p���Ă��Ȃ��ꍇ�͐��`�T�����s���܂��B

void	*dbSch( const stDB *dbp, ... ){

	void		*lstp=0;												// ���[�U�[���X�g�|�C���^
	va_list		ap;														// �ψ������[�N
	char		*keyadr=0;												// ���[�U�[�L�[�A�h���X
	int			keysiz;													// ���[�U�[�L�[�T�C�Y
	char		label[1024];											// ���x��
	int			keyint;													// ���l

	if( dbp ){															// �f�[�^�x�[�X�|�C���^�`�F�b�N
		va_start( ap, dbp );											// �ψ����I�[�v��
		if( ( keysiz=dbp->keysiz )!=0 ){								// ���[�U�[�L�[�T�C�Y�͗L��
			keyadr=va_arg( ap, char * );								// �������烆�[�U�[�L�[�A�h���X�ǂݍ���
			switch( keysiz ){											// ���ꃆ�[�U�[�L�[�T�C�Y�̔���
				case DBSIZ_VAR:											// ���[�U�[�L�[�T�C�Y�͓o�^���Ɏw��
					keysiz=va_arg( ap, int );
					break;
				case DBSIZ_STR:											// ������
				case DBSIZ_RSV:											// �啶������������ʂ��Ȃ�������
					keysiz=strlen( keyadr )+1;							// ���[�U�[�L�[�T�C�Y�͕����񒷁{�P
					break;
				case DBSIZ_LBL:											// ���x��
					keysiz=dbLblLen( keyadr, sizeof(label) )+1;			// �L�[�T�C�Y
					memcpy( label, keyadr, keysiz-1 );					// �o�b�t�@�ɃR�s�[
					label[keysiz-1]=0;									// �O�^�[�~�l�[�g
					keyadr=label;										// �L�[�A�h���X�̓o�b�t�@
					break;
				case DBSIZ_INT:											// ���l
				case DBSIZ_PTR:											// �|�C���^
					keyint=(int)keyadr;
					keyadr=(char*)&keyint;
					keysiz=sizeof(int);
					break;
			}
		}
		va_end( ap );													// �ψ����N���[�Y
		lstp=dbSchS( dbp, keyadr, keysiz );								// �f�[�^�x�[�X����L�[�ɂ��������Č������܂�
	}
	return( lstp );														// ���X�g�|�C���^���A��l�Ƃ���
}

/************************************************************************/
/*	�f�[�^�x�[�X����f�[�^���J�����܂�									*/
/************************************************************************/

//	����:
//		dbp:	�f�[�^�x�[�X�|�C���^
//		lstp:	���X�g�|�C���^���w�肵�܂��B
//	���:
//		�f�[�^�x�[�X�ɂ���f�[�^���폜���܂��B

void	dbFre( stDB *dbp, void *lstp ){

	stSL		*bakp, *slp;											// �P�������X�g�|�C���^
	stDBLNK		*lnkp;													// �����N�|�C���^
	int			lstalg;													// �A���C�����ꂽ���[�U�[���X�g�T�C�Y

	assert( dbp );														// �f�[�^�x�[�X�|�C���^�`�F�b�N
	assert( lstp );														// ���[�U�[���X�g�|�C���^�`�F�b�N
	lnkp=dbLNKP(lstp);													// ���[�U�[���X�g�|�C���^���烊���N�|�C���^�ɕϊ�
	assert( lnkp->nxtp->bakp==lnkp );									// ���̑o�������X�g�����N�`�F�b�N
	assert( lnkp->bakp->nxtp==lnkp );									// �O�̑o�������X�g�����N�`�F�b�N
	if( dbp->slpadr ){													// �n�b�V���\���g�p����Ă���
		lstalg=dbALG( lnkp->lstsiz );									// �A���C�����ꂽ���[�U�[���X�g�T�C�Y
		bakp=dbHash( dbp, ((char*)lstp)+lstalg, lnkp->keysiz );			// �n�b�V���l�v�Z���ăe�[�u���̐擪�����߂�
		while( (slp=bakp->slp)!=0 ){									// �P�������X�g�������[�v
			if( lnkp==(stDBLNK *)(((char*)slp)+dbALG(sizeof(stSL))) ){	// ��������
				dbUDELNXT( bakp );										// �P�������X�g�폜
				dbDEL( lnkp );											// �o�������X�g�폜
				memFre( dbp, slp );										// �������J��
				break;													// ���[�v�I��
			}
			bakp=slp;													// �P�������X�g�|�C���^�X�V
		}
		assert( slp );													// ������Ȃ�����
	}else{																// �n�b�V���\�͎g�p���Ă��Ȃ�
		dbDEL( lnkp );													// �o�������X�g�폜
		memFre( dbp, lnkp );											// �������J��
	}
	dbp->alcn--;														// �o�^�����Z
}

/************************************************************************/
/*	�f�[�^�x�[�X�ɓo�^����Ă������Ԃ��܂�							*/
/************************************************************************/

//	����:
//		dbp:	�f�[�^�x�[�X�|�C���^
//	�߂�l:
//		���ݓo�^����Ă���f�[�^�����A��܂��B
//	���:
//		���ݓo�^����Ă���f�[�^����Ԃ��܂��B
//		dbp��NULL�̏ꍇ�͏��0���A��܂��B

int		dbAlcN( const stDB *dbp ){

	int		alcn=0;														// �o�^��

	if( dbp ){															// �L���ȃf�[�^�x�[�X�|�C���^�`�F�b�N
		alcn=dbp->alcn;													// �����Z�b�g
	}
	return( alcn );														// �o�^��
}

/************************************************************************/
/*	�f�[�^�x�[�X�ɓo�^����Ă���擪�f�[�^�̃|�C���^��Ԃ��܂�			*/
/************************************************************************/

//	����:
//		dbp:	�f�[�^�x�[�X�|�C���^
//	�߂�l:
//		���ݓo�^����Ă���擪�̃��X�g�|�C���^���A��܂��B
//	���:
//		���ݓo�^����Ă���擪�̃��X�g�|�C���^��Ԃ��܂��B
//		dbp �� NULL �̏ꍇ�͏�� NULL ���A��܂��B

void	*dbTop( const stDB *dbp ){

	stDBLNK		*lnkp;													// �����N�|�C���^
	void		*topp=0;												// �擪���[�U�[���X�g�|�C���^

	if( dbp ){															// �L���ȃf�[�^�x�[�X�|�C���^�`�F�b�N
		lnkp=dbp->lnktop.nxtp;											// ���X�g�̐擪�𓾂�
		if( lnkp->nxtp ){												// �_�~�[�I�[�ł͂Ȃ�
			topp=lnkp+1;												// ���[�U�ɓn���̂̓����N�|�C���^+1
		}
	}
	return( topp );														// 0:����, 0�ȊO:�L���ȃf�[�^�|�C���^
}

/************************************************************************/
/*	�f�[�^�x�[�X�ɓo�^����Ă���I�[�f�[�^�̃|�C���^��Ԃ��܂�			*/
/************************************************************************/

//	����:
//		dbp:	�f�[�^�x�[�X�|�C���^
//	�߂�l:
//		���ݓo�^����Ă���I�[�̃��X�g�|�C���^���A��܂��B
//	���:
//		���ݓo�^����Ă���I�[�̃��X�g�|�C���^��Ԃ��܂��B
//		dbp �� NULL �̏ꍇ�͏�� NULL ���A��܂��B

void	*dbEnd( const stDB *dbp ){

	stDBLNK		*lnkp;													// �����N�|�C���^
	void		*endp=0;												// �I�[���[�U�[���X�g�|�C���^

	if( dbp ){															// �L���ȃf�[�^�x�[�X�|�C���^�`�F�b�N
		lnkp=dbp->lnkend.bakp;											// ���X�g�̐擪�𓾂�
		if( lnkp->bakp ){												// �_�~�[�擪�ł͂Ȃ�
			endp=lnkp+1;												// ���[�U�ɓn���̂̓����N�|�C���^+1
		}
	}
	return( endp );														// 0:����, 0�ȊO:�L���ȃf�[�^�|�C���^
}

/************************************************************************/
/*	���̃f�[�^�|�C���^��Ԃ��܂�										*/
/************************************************************************/

//	����:
//		lstp:	���X�g�|�C���^
//	�߂�l:
//		���̃��X�g�|�C���^���A��܂��B
//	���:
//		���̃��X�g�|�C���^��Ԃ��܂��B
//		lstp �� NULL �̏ꍇ�͏��NULL���A��܂��B

void	*dbNxt( const void *lstp ){

	stDBLNK		*lnkp;													// �����N�|�C���^
	void		*nxtlstp=0;												// ��O�̃��[�U�[�|�C���^

	if( lstp ){															// �L����db2�Ǘ��|�C���^
		lnkp=dbLNKP(lstp)->nxtp;										// ���[�U�[���X�g�|�C���^���烊���N�|�C���^�ɕϊ�
		if( lnkp->nxtp ){												// �_�~�[�I�[�ł͂Ȃ�
			nxtlstp=lnkp+1;												// ���[�U�ɓn���̂̓����N�|�C���^+1
		}
	}
	return( nxtlstp );													// 0:����, 0�ȊO:�L���ȃf�[�^�|�C���^
}

/************************************************************************/
/*	�O�̃f�[�^�|�C���^��Ԃ��܂�										*/
/************************************************************************/

//	����:
//		lstp:	���X�g�|�C���^
//	�߂�l:
//		�O�̃��X�g�|�C���^���A��܂��B
//	���:
//		�O�̃��X�g�|�C���^��Ԃ��܂��B
//		lstp �� NULL �̏ꍇ�͏��NULL���A��܂��B

void	*dbBak( const void *lstp ){

	stDBLNK		*lnkp;													// �����N�|�C���^
	void		*baklstp=0;												// ��O�̃��[�U�[�|�C���^

	if( lstp ){															// �L����db2�Ǘ��|�C���^
		lnkp=dbLNKP(lstp)->bakp;										// ���[�U�[���X�g�|�C���^���烊���N�|�C���^�ɕϊ�
		if( lnkp->bakp ){												// �_�~�[�擪�ł͂Ȃ�
			baklstp=lnkp+1;												// ���[�U�ɓn���̂̓����N�|�C���^+1
		}
	}
	return( baklstp );													// 0:����, 0�ȊO:�L���ȃf�[�^�|�C���^
}

/************************************************************************/
/*	�f�[�^�|�C���^�̔z����쐬���܂�									*/
/************************************************************************/

//	����:
//		dbp:	�f�[�^�x�[�X�|�C���^
//		lstpa:	���X�g�|�C���^�̔z��
//	�߂�l:
//		lstpa �̒l���A��܂��B
//	���:
//		�����Alstpa �� NULL �̏ꍇ�͓����œo�^�f�[�^��+1�̃������m�ۂ�����
//		lstpa�Ɋm�ۂ����������̃|�C���^���Z�b�g����܂��B
//		lstpa �z��ɁA���X�g�|�C���^���i�[���A�I�[�� NULL ���Z�b�g���܂��B
//		( lstpa �̃T�C�Y�͓o�^�f�[�^��+1���K�v�ł� )
//		�ʏ�́A�������m�ۂ����ꍇ�AdbAryClose �ŊJ�����K�v�ł����A
//		�����A�J�����Ă��Ȃ��ꍇ�ł��AdbClose ���R�[�����ꂽ���_�Ŏ����I�ɊJ������܂��B

void	*dbAryOpen( stDB *dbp, void *lstpa ){

	stDBLNK	*p, *nxtp;													// �o�������X�g�A�N�Z�X���[�N
	void	**arypp;													// ���X�g�|�C���^�̃|�C���^

	assert( dbp );														// �f�[�^�x�[�X�|�C���^�`�F�b�N
	if( !lstpa ){														// �z��A�h���X�����Ă�����Ă��Ȃ�
		lstpa=dbMemAlc( dbp, ( dbAlcN( dbp )+1 )*sizeof(void*) );		// �K�v�������m��
	}
	arypp=(void**)lstpa;												// ���X�g�|�C���^�̃|�C���^
	for( p=dbp->lnktop.nxtp; (nxtp=p->nxtp)!=0; p=nxtp ){				// �o�^����Ă���f�[�^�̑�����
		*arypp++=( p+1 );												// ���[�U�[�f�[�^�|�C���^��������
	}
	*arypp=0;															// ���
	return( lstpa );
}

/************************************************************************/
/*	�f�[�^�|�C���^�̔z����J�����܂�									*/
/************************************************************************/

//	����:
//		lstpa:	���X�g�|�C���^�̔z��
//	���:
//		dbAryOpen �� lstpa �� NULL ���w�肵���ꍇ�ŁA
//		�����I�ɁA�m�ۂ����������̊J�����������ꍇ�ɃR�[�����܂��B

void	dbAryClose( void *lstpa ){
	dbMemFre( lstpa );													// �f�[�^�x�[�X�̃��[�J���������J��
}

/************************************************************************/
/*	���[�J���������m��( �f�[�^�x�[�X���N���[�Y����Ǝ����I�ɊJ�����܂� )*/
/************************************************************************/

//	����:
//		dbp:	�f�[�^�x�[�X�|�C���^
//		siz:	�m�ۂ���T�C�Y
//	�߂�l:
//		�m�ۂ����������|�C���^���A��܂��B
//	���:
//		�f�[�^�x�[�X��ʂ��ă������m�ۂ����܂��B
//		�P��malloc���R�[��������x���Ȃ�܂����A
//		dbClose ���R�[�����ꂽ���_�Ŏ����I�ɊJ�������̂�
//		���������[�N�̐S�z���Ȃ��Ȃ�܂��B

void	*dbMemAlc( stDB *dbp, int siz ){

	void	*alcp=0;													// ���[�U�[�ɓn���������m�ۃ|�C���^
	stMLST	*mlstp=0;													// �������Ǘ��\���̃|�C���^

	assert( dbp );														// �f�[�^�x�[�X�|�C���^�`�F�b�N
	if( dbp ){															// �L���ȃf�[�^�x�[�X�|�C���^�`�F�b�N
		mlstp=(stMLST*)memAlc( dbp, dbALG(sizeof(stMLST))+siz );		// �������Ǘ��\���̃T�C�Y+���[�U�[�m�ۃT�C�Y���m�ۂ���
		assert( mlstp );												// �������m�ۂ������������ǂ����̃`�F�b�N
	}
	if( mlstp ){														// �������m�ې���
		mlstp->dbp=dbp;													// ���̃��������m�ۂ����f�[�^�x�[�X�|�C���^
		mlstp->siz=siz;													// �m�ۃ������T�C�Y
		dbINSBAK( mlstp, &dbp->mlstend );								// �������Ǘ��p�o�������X�g�ɊǗ��|�C���^��}������
		alcp=((char*)mlstp)+dbALG(sizeof(stMLST));						// ���[�U�[�ɓn���������m�ۃ|�C���^
	}
	return( alcp );														// ���[�U�[�|�C���^��Ԃ�
}

/************************************************************************/
/*	���[�J���������J��													*/
/************************************************************************/

//	����:
//		usrp:	dbMemAlc�܂���dbMemAlcClr�Ŋm�ۂ����������|�C���^
//	���:
//		���������J�����܂��B

void	dbMemFre( void *usrp ){

	stMLST	*mlstp;														// �������Ǘ��\���̃|�C���^

	if( usrp ){															// �L���ȃ������|�C���^
		mlstp=(stMLST*)(((char*)usrp)-dbALG(sizeof(stMLST)));			// �������Ǘ��\���̃|�C���^�̓��[�U�[�|�C���^-1
		assert( mlstp->nxtp->bakp==mlstp );								// ���̑o�������X�g�����N�`�F�b�N
		assert( mlstp->bakp->nxtp==mlstp );								// �O�̑o�������X�g�����N�`�F�b�N
		dbDEL( mlstp );													// �������Ǘ��p�o�������X�g����폜����
		memFre( mlstp->dbp, mlstp );									// �������J��
	}
}

/************************************************************************/
/*	���[�J���������m�ۂ����ăN���A����									*/
/************************************************************************/

//	����:
//		dbp:	�f�[�^�x�[�X�|�C���^
//		siz:	�m�ۂ���T�C�Y
//	�߂�l:
//		�m�ۂ����������|�C���^���A��܂��B
//	���:
//		�f�[�^�x�[�X��ʂ��ă������m�ۂ����N���A���܂��B
//		( dbMemAlc���R�[�����ă��������N���A���Ă��烊�^�[�����܂��B)

void	*dbMemAlcClr( stDB *dbp, int siz ){

	void	*alcp;														// ���[�J���������m�ۃ|�C���^

	alcp=dbMemAlc( dbp, siz );											// ���[�J���������m��( �f�[�^�x�[�X���N���[�Y����Ǝ����I�ɊJ�����܂� )
	if( alcp ){															// �m�ۂł���
		memset( alcp, 0, siz );											// �������N���A
	}
	return( alcp );														// ���[�J���������m�ۃ|�C���^��Ԃ�
}

/************************************************************************/
/*	���[�J���������m�ۂ����ăR�s�[����( bufadr �� NULL �̏ꍇ�̓N���A )	*/
/************************************************************************/

void	*dbMemDup( stDB *dbp, const void *bufadr, int bufsiz ){

	void	*alcp;														// ���[�J���������m�ۃ|�C���^

	alcp=dbMemAlc( dbp, bufsiz );										// ���[�J���������m��( �f�[�^�x�[�X���N���[�Y����Ǝ����I�ɊJ�����܂� )
	if( alcp ){															// �m�ۂł���
		if( bufadr ){
			memcpy( alcp, bufadr, bufsiz );
		}else{
			memset( alcp, 0, bufsiz );									// �������N���A
		}
	}
	return( alcp );														// ���[�J���������m�ۃ|�C���^��Ԃ�
}

/************************************************************************/
/*	�������A���P�[�V���������T�C�Y���v�����߂�							*/
/************************************************************************/

//	����:
//		dbp:	�f�[�^�x�[�X�|�C���^
//	���:
//		�m�ۂ��Ă��郁�����̑��ʂ�Ԃ��܂��B
//		�f�o�b�O���Ȃǂɂ悭�g�p���܂��B

int		dbMemSiz( const stDB *dbp ){

	int		hashn;														// �n�b�V���e�[�u��
	int		slpsiz=0;													// �n�b�V�����g�p���Ă���ꍇ�ɂ͒P�������X�g�|�C���^�T�C�Y���K�v
	int		res=0;														// �������A���P�[�V���������T�C�Y���v
	void	*p;															// �o�������X�g�A�N�Z�X�|�C���^
	stMLST	*mp, *nxtmp;												// �o�������X�g�|�C���^
	stMCB	*mcbp, *nxtmcbp;											// ���[�J���q�[�v�m�ۑo�������X�g�|�C���^

	if( dbp ){															// �L���ȃf�[�^�x�[�X�|�C���^�`�F�b�N
		hashn=dbp->tblmsk+1;											// �n�b�V���e�[�u����
		if( dbp->slpadr ){												// �n�b�V���e�[�u�����g�p���Ă���
			slpsiz=dbALG(sizeof(stSL));									// �P�������X�g�|�C���^�T�C�Y
		}
		res=sizeof(stDB)+hashn*sizeof(stSL*);							// �f�[�^�x�[�X�\���̃T�C�Y�{�n�b�V���e�[�u���T�C�Y
		if( dbp->hpsiz ){												// ���[�J���q�[�v�L��
			for( mcbp=dbp->alctop.nxtp; (nxtmcbp=mcbp->nxtp)!=0; mcbp=nxtmcbp ){				// �O���[�o���������m�ۑ����胋�[�v
				res+=mcbp->blk;											// �������m�ۃT�C�Y���Z( �����ł� blk ���o�C�g�T�C�Y�Ƃ��Ďg�p���Ă��� )
			}
		}else{															// ���[�J���q�[�v����
			dbFOR( dbp, p, void* ){										// �o�^�f�[�^�����胋�[�v
				res	+=
					slpsiz+												// �P�������X�g�|�C���^�T�C�Y
					sizeof(stDBLNK)+									// �����N�Ǘ��\����
					dbALG(dbKEYSIZ(p))+									// �L�[�T�C�Y
					dbALG(dbLSTSIZ(p));									// ���X�g�T�C�Y
			}
			for( mp=dbp->mlsttop.nxtp; (nxtmp=mp->nxtp)!=0; mp=nxtmp ){	// ���[�J���������o�������X�g���[�v
				res	+=mp->siz;											// �������T�C�Y���Z
			}
		}
	}
	return( res );														// �������A���P�[�V���������T�C�Y���v�������ă��^�[��
}

/************************************************************************/
/*	������o�^															*/
/************************************************************************/

//	����:
//		dbp:	�f�[�^�x�[�X�|�C���^
//		strp:	������|�C���^
//	���:
//		��������m�ۂ��Ă��̃|�C���^���A���܂��B

char	*dbStrAlc( stDB *dbp, const char *strp ){

	char	*p=0;														// �������m�ۃ|�C���^

	assert( dbp );														// �L���ȃf�[�^�x�[�X�|�C���^�`�F�b�N
	if( strp ){															// �L���ȕ�����|�C���^
		p=(char*)dbMemAlc( dbp, strlen( strp )+1 );						// �������m��( ������T�C�Y+1 )
		assert( p );													// �������m�ۂ������������ǂ����̃`�F�b�N
		strcpy( p, strp );												// ������R�s�[
	}
	return( p );														// �������m�ۃ|�C���^�������ă��^�[��
}

/************************************************************************/
/*	�f�[�^�x�[�X�̕������쐬����										*/
/************************************************************************/

//	����:
//		dbp:	�f�[�^�x�[�X�|�C���^
//	���:
//		�f�[�^�x�[�X�̕������쐬���܂��B
//		dbp �� NULL �̏ꍇ�͏��NULL���A��܂��B

stDB	*dbDup( const stDB *dbp ){

	stDB		*newdbp=0;												// �V�����f�[�^�[�x�[�X�|�C���^
	const void	*p;														// �f�[�^�A�N�Z�X�p���[�N

	if( dbp ){															// �L���ȃf�[�^�x�[�X�|�C���^�`�F�b�N
		newdbp=dbOpen( dbp->keysiz, dbp->lstsiz, dbp->tblmsk+1, dbp->hpsiz );	// �V�����f�[�^�x�[�X�I�[�v��
		dbFOR( dbp, p, const void* ){									// �I���W�i���f�[�^������
			dbAlcS( newdbp, dbKEYADR( p ), dbKEYSIZ( p ), p, dbLSTSIZ( p ) );
		}
	}
	return( newdbp );													// �V�����f�[�^�x�[�X�������ă��^�[��
}

/************************************************************************/
/*	�m�Ԗڂ̃��X�g�A�h���X�𓾂�										*/
/************************************************************************/

//	����:
//		dbp:	�f�[�^�x�[�X�|�C���^
//		no:		�o�^�ԍ�
//	���:
//		�f�[�^�x�[�X�� no �Ԗڂ̃��X�g�A�h���X��Ԃ��܂��B
//		no ���o�^���ȏ�A�܂��� dbp �� NULL �̏ꍇ��NULL���A��܂��B

void	*dbLstAdr( const stDB *dbp, int lstno ){

	void			*lstp=0;											// ���[�U�[���X�g�|�C���^
	stDBLNK			*p;													// �����N�|�C���^
	unsigned int	lstnou=(unsigned int)lstno;							// �������������ɃL���X�g�������X�g�ԍ�
	unsigned int	lstn;												// �o�^��
	unsigned int	i;													// ���[�v�J�E���^

	if( dbp ){															// �L���ȃf�[�^�x�[�X�|�C���^�`�F�b�N
		lstn=dbAlcN( dbp );												// �o�^��
		if( lstnou<lstn ){												// �ԍ����o�^������������
			if( lstnou<(lstn>>1) ){										// �f�[�^�͑O���ɑ��݂���
				p=dbp->lnktop.nxtp;										// �����N�|�C���^��擪�ɃZ�b�g
				for( i=0; i<lstnou; i++ ){								// �擪���珇��
					p=p->nxtp;											// ���̃����N�|�C���^
				}
			}else{														// �f�[�^�͌㔼�ɑ��݂���
				p=dbp->lnkend.bakp;										// �����N�|�C���^���I�[�ɃZ�b�g
				for( i=lstn-1; i>lstnou; i-- ){							// �I�[���珇��
					p=p->bakp;											// �O�̃����N�|�C���^
				}
			}
			lstp=p+1;													// ���[�U�[���X�g�|�C���^�Z�b�g
		}
	}
	return( lstp );														// ���[�U�[���X�g�|�C���^�������ă��^�[��
}

/************************************************************************/
/*	�f�[�^�x�[�X�̓o�^���Ԃ�ύX����									*/
/************************************************************************/

//	����:
//		dbp:	�f�[�^�x�[�X�|�C���^
//		lstp:	�ړ�����f�[�^�|�C���^
//		nxtp:	�ړ���̎��̃f�[�^�|�C���^( 0 �̏ꍇ�͏I�[�̎����w�肳�ꂽ���̂Ƃ݂Ȃ��܂� )
//	���:
//		�f�[�^�x�[�X�̓o�^���Ԃ�ύX���܂��B
//		lstp �� nxtp �̑O�Ɉړ����܂��B

void	dbMov( stDB *dbp, void *lstp, void *nxtp ){

	stDBLNK		*lnkp, *nxtlnkp;										// �����N�|�C���^

	assert( dbp );
	assert( lstp );
	lnkp=dbLNKP(lstp);													// ���[�U�[���X�g�|�C���^���烊���N�|�C���^�ɕϊ�
	if( nxtp ){															// �����w�肳��Ă���
		nxtlnkp=dbLNKP(nxtp);											// ���[�U�[���X�g�|�C���^���烊���N�|�C���^�ɕϊ�
	}else{																// ���͎w�肳��Ă��Ȃ�
		nxtlnkp=&dbp->lnkend;											// �I�[
	}
	dbDEL( lnkp );														// �o�������X�g�폜
	dbINSBAK( lnkp, nxtlnkp );											// �o�������X�g�}��
}

/************************************************************************/
/*	�w�肵���I�t�Z�b�g���V�[�N����										*/
/************************************************************************/

//	����:
//		seeknp:	���ۂɃV�[�N�ł��������������܂��|�C���^( 0 �Ȃ�Ζ�������܂� )
//		lstp:	���X�g�|�C���^
//		seekofs:�V�[�N���w��
//	���:
//		�w�肵���I�t�Z�b�g���V�[�N�������X�g�|�C���^���A���܂��B

void	*dbSeek( int *seeknp, const void *lstp, int seekofs ){

	const stDBLNK	*p, *np;											// �����N�|�C���^
	int				seekn=0;											// ���ۂɃV�[�N������

	if( lstp ){															// ���X�g�|�C���^�͗L��
		p=dbLNKP(lstp);													// ���X�g�|�C���^���烊���N�|�C���^�ɕϊ�
		if( seekofs<0 ){												// ���
			for( seekn=0; seekn>seekofs; seekn-- ){
				np=p->bakp;												// �O�̃����N�|�C���^
				if( !np->bakp ){										// �����Ȃ�
					break;												// ���[�v�I��
				}
				p=np;													// �����N�|�C���^�X�V
			}
		}else{															// �O��
			for( seekn=0; seekn<seekofs; seekn++ ){
				np=p->nxtp;												// ���̃����N�|�C���^
				if( !np->nxtp ){										// �����Ȃ�
					break;												// ���[�v�I��
				}
				p=np;													// �����N�|�C���^�X�V
			}
		}
		lstp=p+1;														// �����N�|�C���^���烊�X�g�|�C���^�ɕϊ�
	}
	if( seeknp ){														// ���ۂɃV�[�N���������������܂��|�C���^�͗L��
		*seeknp=seekn;													// ���ۂɃV�[�N����������������
	}
	return( (void*)lstp );												// ���X�g�|�C���^�������ă��^�[��
}

/************************************************************************/
/*	���Ɉ�v������̂�����												*/
/************************************************************************/

//	����:
//		dbp:	�f�[�^�x�[�X�|�C���^
//		lstp:	���X�g�|�C���^
//	���:
//		���Ɉ�v������̂��������܂��B
//		��ʓI�ɂ͂��܂�g�p���܂��񂪁A
//		�����L�[�𕡐��o�^���Ă����đ����肷��Ƃ������g���������܂��B

void	*dbSchNxt( const stDB *dbp, const void *lstp ){

	void		*nxtlstp=0;												// ���[�U�[���X�g�|�C���^
	void		*keyadr;												// �L�[�A�h���X
	int			keysiz;													// �L�[�T�C�Y
	int			lstalg;													// �A���C�����ꂽ���[�U�[���X�g�T�C�Y
	stSL		*bakp, *slp;											// �P�������X�g�|�C���^
	stDBLNK		*lnkp;													// �o�������X�g�|�C���^
	int			find;													// ���������t���O

	if( lstp ){															// ���X�g�|�C���^�͗L��
		keyadr=dbKEYADR( lstp );										// �L�[�A�h���X
		keysiz=dbKEYSIZ( lstp );										// �L�[�T�C�Y
		if( dbp->slpadr ){												// �n�b�V���\���g�p����Ă���
			bakp=(stSL*)( ((char*)lstp)-(dbALG(sizeof(stSL))+sizeof(stDBLNK)) );
			while( (slp=bakp->slp)!=0 ){								// �P�������X�g�����N���[�v
				lnkp=(stDBLNK*)((char*)slp+dbALG(sizeof(stSL)));		// �����N�|�C���^
				lstalg=dbALG( lnkp->lstsiz );							// �A���C�����ꂽ���X�g�T�C�Y
				find=dbKeyCmp( dbp, (char*)(lnkp+1)+lstalg, lnkp->keysiz, keyadr, keysiz );
				if( find ){												// ��������
					nxtlstp=(lnkp+1);									// �A��l�Z�b�g
					break;												// ���[�v�I��
				}
				bakp=slp;												// �P�������X�g�|�C���^�X�V
			}
		}else{															// �n�b�V���\�͎g�p���Ă��Ȃ��̂Ńx�^�T��
			lnkp=(stDBLNK*)lstp-1;										// �J�����g�����N�|�C���^�Z�b�g
			for(;;){
				lnkp=lnkp->bakp;										// ��O�ɍX�V
				if( !lnkp ){											// �O�͂Ȃ�
					break;												// ���[�v�I��
				}
				lstalg=dbALG( lnkp->lstsiz );							// �A���C�����ꂽ���X�g�T�C�Y
				find=dbKeyCmp( dbp, (char*)(lnkp+1)+lstalg, lnkp->keysiz, keyadr, keysiz );
				if( find ){												// ��������
					nxtlstp=(lnkp+1);									// �A��l�Z�b�g
					break;												// ���[�v�I��
				}
			}
		}
	}
	return( nxtlstp );													// ���X�g�|�C���^���A��l�Ƃ���
}

/************************************************************************/
/*	�ǉ�																*/
/************************************************************************/

//	����:
//		dstdbp:	�f�B�X�g�l�[�V�����f�[�^�x�[�X�|�C���^
//		adddbp:	�\�[�X�f�[�^�x�[�X�|�C���^
//	���:
//		adddbp �̓��e�� dstdbp �ɕt�����܂��B

void	dbAddDB( stDB *dstdbp, const stDB *adddbp ){

	const void	*p;														// �f�[�^�A�N�Z�X�p���[�N

	dbFOR( adddbp, p, const void* ){									// �I���W�i���f�[�^������
		dbAlcS( dstdbp, dbKEYADR( p ), dbKEYSIZ( p ), p, dbLSTSIZ( p ) );
	}
}

/************************************************************************/
/*	�N���A																*/
/************************************************************************/

//	����:
//		dbp:	�f�[�^�x�[�X�|�C���^
//	���:
//		dbp ���N���A���܂��B
//		dbClose ���Ă��� dbOpen ������������ł��B

void	dbClear( stDB *dbp ){

	void		*p, *nxtp;												// ���X�g�|�C���^
	stMLST		*mp, *nxtmp;											// �o�������X�g�|�C���^
	stMCB		*mcbp, *nxtmcbp;										// ���[�J���q�[�v�m�ۑo�������X�g�|�C���^

	if( dbp ){															// �L���ȃf�[�^�x�[�X�|�C���^
		dbFOR2( dbp, p, nxtp, void* ){
			dbFre( dbp, p );
		}
		if( dbp->hpsiz ){												// ���[�J���q�[�v�L��
			for( mcbp=dbp->alctop.nxtp; (nxtmcbp=mcbp->nxtp)!=0; mcbp=nxtmcbp ){				// �������m�ۑ����胋�[�v
				DB_FREE( mcbp );										// �O���[�o���������J��
			}
			dbINI( &dbp->fretop, &dbp->freend );						// �󂫃������R���g���[���u���b�N���X�g������
			dbINI( &dbp->usetop, &dbp->useend );						// �g�p�������R���g���[���u���b�N���X�g������
			dbINI( &dbp->alctop, &dbp->alcend );						// ���[�J���q�[�v�������R���g���[���u���b�N������
		}else{															// ���[�J���q�[�v����
			for( mp=dbp->mlsttop.nxtp; (nxtmp=mp->nxtp)!=0; mp=nxtmp ){	// ���[�J���������Ǘ��\���̃��[�v
				DB_FREE( mp );											// ���[�J���������J��
			}
			dbINI( &dbp->mlsttop, &dbp->mlstend );						// �o�������X�g������
		}
	}
}

/************************************************************************/
/*	���X�g�̔ԍ��𓾂�													*/
/************************************************************************/

//	����:
//		dbp:	�f�[�^�x�[�X�|�C���^
//		lstp:	���X�g�|�C���^
//	���:
//		���X�g�|�C���^���烊�X�g�ԍ��𓾂܂��B
//		������Ȃ������ꍇ�ɂ�-1���A���܂��B

int		dbLstNo( stDB *dbp, const void *lstp ){

	void	*p;															// ���X�g�|�C���^
	int		i=0;														// ���X�g�ԍ�

	dbFOR( dbp, p, void* ){
		if( p==lstp ){													// ��������
			break;
		}
		i++;															// ���X�g�ԍ��C���N�������g
	}
	if( !p ){															// ������Ȃ�����
		i=-1;
	}
	return( i );														// ���X�g�ԍ��������ă��^�[��
}

/************************************************************************/
/*	�������x���������߂�												*/
/************************************************************************/

	static const char	lblchrlena[2][256]={							// ���x�������T�C�Y�e�[�u��
#	if	defined(_WIN32) || defined(__CYGWIN__)							/* SJIS*/
		{
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1, 0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,
			1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,1,
		},{
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,
			0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1, 0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,
			1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,1,
		}
#	else																/* EUC*/
		{
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1, 0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,
			2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
			2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
		},{
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,
			0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1, 0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,
			2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
			2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
		}
#	endif
	};

//	����:
//		txtadr:	������A�h���X
//		bufsiz:	�o�b�t�@�T�C�Y
//	���:
//		�������x�����𓾂܂��B
//		�����A�������x������ bufsiz �����傫���ꍇ�̓N�����v����܂��B

int		dbLblLen( const char *txtadr, int bufsiz ){

	const char	*txtp;													// �e�L�X�g�|�C���^
	const char	*lblp;													// ���x���|�C���^
	int			len;													// �����T�C�Y
	int			lbllen;													// ���x���T�C�Y

	txtp=txtadr;
	lblp=lblchrlena[0];													// ���x���擪�����e�[�u��
	for(;;){
		len=lblp[*(unsigned char*)txtp];								// �����T�C�Y
		if( !len ){														// ���x���ł͂Ȃ�
			break;
		}
		txtp+=len;
		lblp=lblchrlena[1];												// ���x�������e�[�u��
	}
	lbllen=txtp-txtadr;													// ���x���T�C�Y
	if( lbllen>bufsiz-1 ){												// ���x���T�C�Y���o�b�t�@�T�C�Y���傫��
		lbllen=bufsiz-1;												// �N�����v
	}
	return( lbllen );
}

/************************************************************************/
/*	�������x���𓾂�													*/
/************************************************************************/

//	bufadr:	�o�b�t�@�A�h���X
//	bufsiz:	�o�b�t�@�T�C�Y
//	txtp:	������A�h���X

char	*dbGetLbl( char *bufadr, int bufsiz, const char *txtp ){

	int		lbllen;														// �������x����

	lbllen=dbLblLen( txtp, bufsiz );									// �������x���������߂�
	memcpy( bufadr, txtp, lbllen );										// �o�b�t�@�ɃR�s�[
	bufadr[lbllen]=0;													// �O�^�[�~�l�[�g
	return( (char*)txtp+lbllen );
}

/************************************************************************/
/*	�_�u���N�H�[�e�[�V�����ň͂܂ꂽ������𓾂�						*/
/************************************************************************/

//	����:
//		dstp:	�o�b�t�@�A�h���X
//		dstsiz:	�o�b�t�@�T�C�Y
//		txtp:	������A�h���X
//	���:
//		�����A�����񂪃_�u���N�H�[�e�[�V�����ň͂܂�Ă���ꍇ�͂��̒��g�𓾂܂��B
//		�����łȂ��ꍇ�̓^�u�A�X�y�[�X�A���s�܂ł𓾂܂��B

char	*dbGetDQ( char *dstp, int dstsiz, const char *txtp ){

	static const char * const	sepa[2]={ "\r\n\t ", "\"" };			// �^�[�~�l�[�^������z��
	int							dq;										// �_�u���R�[�e�[�V�����t���O
	const char					*nxtp;									// ���̃e�L�X�g�|�C���^
	int							siz;

	dq	=( *txtp=='"' );												// �_�u���R�[�e�[�V�����t���O�Z�b�g
	txtp+=dq;															// �����A�擪���_�u���N�H�[�e�[�V�����Ȃ�΃e�L�X�g�|�C���^���C���N�������g
	nxtp=strpbrk( txtp, sepa[dq] );										// �Z�p���[�^����
	if( !nxtp ){														// �Z�p���[�^��������Ȃ�
		nxtp=txtp+strlen( txtp );										// �l�N�X�g�|�C���^�𕶎���I�[�ɃZ�b�g
		dq	=0;															// �Z�p���[�^��������Ȃ��ꍇ�̓_�u���N�H�[�e�[�V��������
	}
	siz=nxtp-txtp;														// �]���T�C�Y
	if( siz>dstsiz-1 ){													// �o�b�t�@�T�C�Y�|�P���傫��
		siz=dstsiz-1;													// �N�����v
	}
	if( dstp ){
		memcpy( dstp, txtp, siz );										// �f�B�X�g�l�[�V�����o�b�t�@�ɕ�������R�s�[
		dstp[siz]=0;													// �f�B�X�g�l�[�V�����o�b�t�@�O�^�[�~�l�[�g
	}
	return( (char*)( nxtp+dq ) );										// �e�L�X�g�|�C���^�������ă��^�[��
}

/************************************************************************/
/*	�啶������������ʂ��Ȃ������Ή���������r							*/
/************************************************************************/

//	dstadr:	��r�������A�h���X
//	souadr:	��r�������A�h���X
//	siz:	�������T�C�Y

int		dbMemCaseCmp( const void *dstadr, const void *souadr, int siz ){

	int					res=0;
	const unsigned char	*dstp=(const unsigned char*)dstadr;
	const unsigned char	*soup=(const unsigned char*)souadr;
	unsigned char		sou, dst;
	int					i;

	for( i=0; i<siz; i++ ){
		dst=*dstp++;													// �f�B�X�g�l�[�V����
		sou=*soup++;													// �\�[�X
		res=dst-sou;													// ���U���g
		if( res ){														// �Ⴄ
			if( dst>='A' && dst<='Z' ){									// �啶��
				dst|='a'-'A';											// �������ϊ�
			}
			if( sou>='A' && sou<='Z' ){									// �啶��
				sou|='a'-'A';											// �������ϊ�
			}
			res=dst-sou;												// ���U���g
			if( res ){													// �Ⴄ
				break;
			}
		}else{
			if( lblchrlena[0][sou]==2 && i<siz-1 ){						// �����P�o�C�g��
				dst=*dstp++;											// �f�B�X�g�l�[�V����
				sou=*soup++;											// �\�[�X
				res=dst-sou;											// ���U���g
				if( res ){
					break;
				}
				i++;
			}
		}
	}
	return( res );
}

/************************************************************************/
/*	�啶������������ʂ��Ȃ������Ή��������r							*/
/************************************************************************/

//	dstp:	��r������A�h���X
//	soup:	��r������A�h���X

int		dbStrCaseCmp( const char *dstp, const char *soup ){

	int		dstsiz;														// �f�B�X�g�l�[�V�����T�C�Y
	int		sousiz;														// �\�[�X�T�C�Y
	int		siz;														// ��r�T�C�Y

	dstsiz=strlen( dstp );												// �f�B�X�g�l�[�V�����T�C�Y
	sousiz=strlen( soup );												// �\�[�X�T�C�Y
	if( dstsiz==sousiz ){												// ����
		siz=dstsiz;
	}else if( dstsiz>sousiz ){
		siz=sousiz+1;													// �������ق�+1
	}else{
		siz=dstsiz+1;													// �������ق�+1
	}
	return( dbMemCaseCmp( dstp, soup, siz ) );							// �啶������������ʂ��Ȃ������Ή���������r
}

