/************************************************************************/
/*																		*/
/*	�u���E����					by H.Watanabe							*/
/*	Ver1.00	2009/04/04 13:49:05	�쐬�J�n								*/
/*																		*/
/************************************************************************/

#	include	<stdio.h>													/* �W�����o�͊֐�*/
#	include	<stdlib.h>													/* �W�����C�u�����֐�*/
#	include	<string.h>													/* �W��������֐�*/
#	include	"std/search2.h"												/* ����*/
#	include	"std/lm.h"													/* ���C���}�b�v*/
#	include	"main.h"													/* ���C���C���N���[�h�t�@�C��*/

/************************************************************************/
/*	������u��															*/
/************************************************************************/

static void	repstr( char *dstp, const char *txtadr, int txtofs, int txtsiz, const char *newp ){
	memcpy( dstp, txtadr, txtofs );
	dstp+=txtofs;
	strcpy( dstp, newp );
	dstp+=strlen( dstp );
	strcpy( dstp, txtadr+txtofs+txtsiz );
}

/************************************************************************/
/*	�u���E����															*/
/************************************************************************/

void	replace( const char *orgp, const char *newp, int bakflg, int chkflg, int notdsp ){

	stLIN		*linp;													// �s�|�C���^
	char		*txtadr;												// �e�L�X�g�擪�A�h���X
	char		*txtp;													// �e�L�X�g�|�C���^
	stLM		lm;														// �s�}�b�v
	char		tmpa[0x10000];											// �e���|�����o�b�t�@
	int			curxx;													// �J�[�\�������W
	int			lno;													// �s�ԍ�
	int			yn;														// �C�G�X�A�m�[���U���g
	int			txtofs, newofs;											// �e�L�X�g�I�t�Z�b�g
	int			searchlen;												// ����������
	int			curbak;													// �J�[�\���\�����o�b�N�A�b�v

	linp	=CURP;														// �s�|�C���^
	txtadr	=CURTXTP;													// �e�L�X�g�A�h���X
	lmMak( &lm, txtadr, g_tabstp, 0 );									// �s�}�b�v�쐬
	curxx	=lmPosAdj( &lm, CURXX, 0 );									// �J�[�\���ʒu���K��
	lno=PGYY+CURYY;														// �s�ԍ�
	txtp=txtadr+lm.txtofsa[curxx];										// �e�L�X�g�|�C���^�Z�b�g
	if( !newp ){														// �����̏ꍇ
		if( !bakflg ){													// �O��
			if( *txtp ){												// ����������
				txtp++;													// �e�L�X�g�|�C���^�C���N�������g
			}
		}else{															// ���
			if( curxx ){												// �s�擪�ł͂Ȃ�
				txtp--;													// ����
			}else{														// �s�擪
				txtadr=txtp="";											// �k���s���|�C���g
			}
		}
	}
	for(;;){
		txtp=searchLin( txtadr, txtp, orgp, bakflg, g_flglbl, g_flgwld, g_flgc2l );
		if( txtp ){														// ��������
			if( g_flgwld ){												// ���C���h�J�[�h�t���O�������Ă���
				searchlen=searchWld( txtp, orgp, g_flgc2l )-txtp;		// ����������
			}else{														// ���C���h�J�[�h����
				searchlen=strlen( orgp );								// ����������
			}
			g_scdbiz=searchlen;											// �O���[�o�����[�N�Z�b�g
			lnoset( lno );												// �s�ԍ��Z�b�g
			if( !notdsp ){												// �\���L��
				lmMak( &lm, txtadr, g_tabstp, 0 );						// �s�}�b�v�쐬
				CURXX=lmOfs2Pos( &lm, txtp-txtadr );					// �J�[�\�����W�𓾂�
				linAdjScr();											// ���X�N���[�����K��
				invalidateAll();										// �S�čĕ\��
			}
			if( !newp ){												// �����̏ꍇ
				break;													// ���[�v�I��
			}
			yn=YN_YES;
			if( chkflg ){												// �m�F�L��
				curbak=curSw( 1 );										// �J�[�\���I��
				yn=YorN( "Replace ? (Y/N)" );							// �m�F
				curSw( curbak );										// �J�[�\���I�t
				if( yn==YN_CANCEL ){									// �L�����Z���̏ꍇ
					break;												// ���[�v�I��
				}
			}
			if( yn==YN_YES ){											// �ϊ����s
				txtofs=txtp-txtadr;										// �e�L�X�g�I�t�Z�b�g
				newofs=txtofs+strlen( newp );							// �V�����I�t�Z�b�g
				repstr( tmpa, txtadr, txtp-txtadr, searchlen, newp );	// ������u��
				linp=linflush( linp, tmpa );							// �e�L�X�g�X�V
				linchg( linp );											// �s�ύX�Z�b�g
				txtadr=(char*)dbKEYADR( linp );							// �e�L�X�g�A�h���X�X�V
				if( !bakflg ){											// �O��
					txtp=txtadr+newofs;
				}else{													// ���
					if( txtofs ){										// �s�擪�ł͂Ȃ�
						txtp=txtadr+txtofs-1;							// �e�L�X�g�|�C���^�Z�b�g
					}else{												// �s�擪�̏ꍇ
						txtadr=txtp="";									// �k���s���|�C���g
					}
				}
				if( !notdsp ){											// �\���L��
					invalidateAll();									// �S�čĕ\��
				}
			}else{
				if( !bakflg ){											// �O��
					txtp+=searchlen;									// �e�L�X�g�|�C���^���Z
				}else{													// ���
					txtp-=searchlen;									// �e�L�X�g�|�C���^���Z
				}
			}
		}else{															// ������Ȃ�����
			if( !bakflg ){												// �O��
				linp=(stLIN*)dbNxt( linp );								// �t�H���[�h
				if( !linp ){											// �t�@�C���I�[
					break;												// �I��
				}
				txtadr=(char*)dbKEYADR( linp );							// �e�L�X�g�擪�A�h���X�Z�b�g
				txtp=txtadr;											// �e�L�X�g�|�C���^���s�擪�ɃZ�b�g
				lno++;													// �s�ԍ��C���N�������g
			}else{														// ���
				linp=(stLIN*)dbBak( linp );								// �o�b�N
				if( !linp ){											// �t�@�C���擪
					break;												// �I��
				}
				txtadr=(char*)dbKEYADR( linp );							// �e�L�X�g�擪�A�h���X�Z�b�g
				txtp=txtadr+strlen( txtadr );							// �e�L�X�g�|�C���^���s�I�[�Ƀ|�C���g
				lno--;													// �s�ԍ��f�N�������g
			}
		}
	}
	if( notdsp ){
		invalidateAll();												// �S�čĕ\��
	}
}

