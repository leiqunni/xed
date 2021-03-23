/************************************************************************/
/*																		*/
/*	�R���e�L�X�g�Ǘ�			by H.Watanabe							*/
/*	Ver1.00	2009/04/04 13:49:05	�쐬�J�n								*/
/*																		*/
/************************************************************************/

#	include	<stdio.h>													/* �W�����o�͊֐�*/
#	include	<string.h>													/* �W��������֐�*/
#	include	"main.h"													/* ���C���C���N���[�h�t�@�C��*/

/************************************************************************/
/*	���s�R�[�h�𒲂ׂ�													*/
/************************************************************************/

static int	crtypchk( const char *fnamp ){

	int		crtyp=CRTYP_DEFAULT;										// ���s�^�C�v
	FILE	*fp;														// �t�@�C���|�C���^
	int		iosiz;														// ���o�̓T�C�Y
	char	txta[4096];													// �o�b�t�@
	void	*txtp;														// �e�L�X�g�|�C���^

	fp=fopen( fnamp, "rb" );											// �t�@�C���I�[�v��
	if( fp ){															// �t�@�C���I�[�v������
		while( !feof( fp ) ){											// �t�@�C�����I�[�łȂ��ԃ��[�v
			iosiz=fread( txta, 1, sizeof(txta), fp );					// �t�@�C�����[�h
			if( !iosiz ){												// �t�@�C���I��
				break;
			}
			txtp=memchr( txta, 0x0d, iosiz );							// 0x0d������
			if( txtp ){													// 0x0d����������
				crtyp=CRTYP_WIN;										// Windows
				break;
			}
			txtp=memchr( txta, 0x0a, iosiz );							// 0x0a������
			if( txtp ){													// 0x0a����������
				crtyp=CRTYP_UNIX;										// Linux
				break;
			}
		}
		fclose( fp );													// �t�@�C���N���[�Y
	}
	return( crtyp );													// ���s�^�C�v�������ă��^�[��
}

/************************************************************************/
/*	�R���e�L�X�g�I�[�v��												*/
/************************************************************************/

stCTX	*ctxOpen( const char *fnamp ){

	stCTX	*ctxp;														// �Ǘ��\����
	FILE	*fp;														// �t�@�C���|�C���^
	char	txta[0x4000], *txtp;										// �e�L�X�g�o�b�t�@�A�|�C���^
	int		crflg=1;													// ���s�R�[�h�t���O

	ctxp=(stCTX*)dbAlc( g_ctxdbp, fnamp, 0 );							// �R���e�L�X�g�o�^
	ctxp->lindbp=dbOpen( DBSIZ_VAR, sizeof(stLIN), 0, 0x10000 );		// �sDB�I�[�v��
	fp=fopen( fnamp, "r" );												// �t�@�C���I�[�v��
	if( fp ){															// �t�@�C���I�[�v������
		while( !feof( fp ) ){											// �t�@�C���I�[�ɒB���Ȃ��ԃ��[�v
			if( !fgets( txta, sizeof(txta), fp ) ){						// �o�b�t�@�͖���
				continue;
			}
			txtp=strpbrk( txta, "\x1a" );								// [EOF]����
			if( txtp ){													// ��������
				*txtp=0;												// [EOF]�N���A
			}
			txtp=strpbrk( txta, "\r\n" );								// ���s�R�[�h����
			if( txtp ){													// ��������
				*txtp=0;												// ���s�R�[�h�N���A
			}
			crflg=( txtp!=0 );											// ���s�R�[�h��������΂P�ɂȂ�
			linAlcStr( ctxp->lindbp, txta );							// �s�o�^
		}
		fclose( fp );													// �t�@�C���N���[�Y

#	ifdef	unix
		if( !stat( fnamp , &ctxp->st ) ){								// �t�@�C���͑��݂��Ă���
			ctxp->statflg=1;											// �t�@�C�����݃t���O�Z�b�g
		}
#	endif

	}
	if( crflg ){														// �ŏI�s�ɉ��s�R�[�h�����݂���
		linAlcStr( ctxp->lindbp, "" );									// [EOF]�s�o�^
	}
	ctxp->crtyp=crtypchk( fnamp );										// ���s�R�[�h�𒲂ׂ�
	ctxp->pgp=ctxp->curp=(stLIN*)dbTop( ctxp->lindbp );
	return( ctxp );														// �Ǘ��\���̂������ă��^�[��
}

/************************************************************************/
/*	�R���e�L�X�g�N���[�Y												*/
/************************************************************************/

void	ctxClose( stCTX *ctxp ){

	stCTX	*p, *nxtp;													// �R���e�L�X�g�|�C���^

	if( ctxp ){															// ��̃R���e�L�X�g�폜
		if( ctxp==g_ctxp ){												// �N���[�Y���悤�Ƃ��Ă���̂̓J�����g
			g_ctxp=(stCTX*)dbNxt( g_ctxp );								// �J�����g�͎�
			if( !g_ctxp ){												// ���͖�������
				g_ctxp=(stCTX*)dbTop( g_ctxdbp );						// �J�����g�͐擪( NULL �̏ꍇ������̂Œ��� )
			}
		}
		dbClose( ctxp->lindbp );										// �R���e�L�X�g�̍sDB�j��
		dbFre( g_ctxdbp, ctxp );										// �Ǘ��\���̃������J��
	}else{																// �S�č폜
		dbFOR2( g_ctxdbp, p, nxtp, stCTX* ){							// �R���e�L�X�g�����胋�[�v
			ctxClose( p );												// �R���e�L�X�g�N���[�Y
		}
	}
}

/************************************************************************/
/*	�R���e�L�X�g�Z�[�u													*/
/************************************************************************/

int		ctxSave( const stCTX *ctxp ){

	int		errn=0;														// �G���[��
	char	*fnamp;														// �t�@�C�����|�C���^
	char	baknam[256];												// �o�b�N�A�b�v�t�@�C����
	FILE	*fp;														// �t�@�C���|�C���^
	stLIN	*linp;														// �s�|�C���^
	char	*txtp;														// �e�L�X�g�|�C���^

	fnamp=(char*)dbKEYADR( ctxp );										// �t�@�C����
	if( !g_optn ){														// *.bak �t�@�C�����쐬����
		sprintf( baknam, "%s.bak", fnamp );								// �o�b�N�A�b�v�t�@�C�����쐬
		remove( baknam );												// ���t�@�C���폜
		rename( fnamp, baknam );										// ���l�[��
	}
	fp=fopen( fnamp, "wb" );											// �t�@�C���o�̓I�[�v��
	if( fp ){															// �t�@�C���o�̓I�[�v������
		dbFOR( ctxp->lindbp, linp, stLIN* ){							// �`�����胋�[�v
			txtp=(char*)dbKEYADR( linp );
			if( dbNxt( linp ) ){										// �I�[�s�ł͂Ȃ�
				if( ctxp->crtyp==CRTYP_UNIX ){							// UNIX
					fprintf( fp, "%s\n", txtp );						// 0a
				}else{													// WINDOWS
					fprintf( fp, "%s\r\n", txtp );						// 0d0a
				}
			}else{														// �I�[�s
				fprintf( fp, "%s", (char*)dbKEYADR( linp ) );
			}
		}
		fclose( fp );													// �t�@�C���o�̓N���[�Y
#	ifdef	unix
		if( ctxp->statflg ){											// �O�̃t�@�C�������݂��Ă���
			chmod( fnamp, ctxp->st.st_mode );							// �t�@�C�����[�h�ύX
		}
#	endif

	}else{
		errn++;															// �G���[�����Z
	}
	return( !errn );													// 0:�G���[, 1:����I��
}

/************************************************************************/
/*	�R���e�L�X�g���l�[��												*/
/************************************************************************/

stCTX	*ctxRename( stCTX *ctxp, const char *fnamp ){

	stCTX	*newp;														// �V�����R���e�L�X�g�|�C���^

	newp=(stCTX*)dbAlc( g_ctxdbp, fnamp, 0 );							// �R���e�L�X�g�o�^
	memcpy( newp, ctxp, sizeof(stCTX) );								// ���\���̂�V�\���̂ɃR�s�[
	dbFre( g_ctxdbp, ctxp );											// ���R���e�L�X�g�j��
	return( newp );														// �V�����R���e�L�X�g�|�C���^�������ă��^�[��
}

