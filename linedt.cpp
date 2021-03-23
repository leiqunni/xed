/************************************************************************/
/*																		*/
/*	��s�ҏW					by H.Watanabe							*/
/*	Ver1.00	2009/04/04 13:49:05	�쐬�J�n								*/
/*																		*/
/************************************************************************/

#	include	<stdio.h>													/* �W�����o�͊֐�*/
#	include	<string.h>													/* �W��������֐�*/
#	include	"std/txtdl.h"												/* �e�L�X�g�G�f�B�^�̂��߂̃f�B�X�v���C���X�g�ϊ�*/
#	include	"std/clpbrd.h"												/* �N���b�v�{�[�h�Ǘ�*/
#	include	"main.h"													/* ���C���C���N���[�h�t�@�C��*/

/************************************************************************/
/*	���[�J����`														*/
/************************************************************************/

#	define	LEW			64

	static int			g_leflg;										// ��s�ҏW�t���O
	static char			g_lettl[256];									// ��s�ҏW�^�C�g��
	static char			g_lebuf[256];									// ��s�ҏW�o�b�t�@
	static int			g_lexxc, g_leyyc, g_lexxd, g_leyyd;				// ��s�ҏW���W
	static int			g_lecurxx;										// ��s�ҏW�J�[�\���|�W�V����

/************************************************************************/
/*	��s�ҏW�ĕ\��														*/
/************************************************************************/

void	linedtReDsp(void){

	static const unsigned long	cola[2]={
		CR_WHITE,														// �z���C�g
		CR_LB,															// ���C�g�u���[
	};
	unsigned long	bakforcola[2]={ CR_BLACK, CR_WHITE };				// �w�i�F�A�O�i�F
	stDB			*dldbp;												// �e�L�X�g�f�B�X�v���C���X�gDB
	stTXTDL			*dlp;												// �e�L�X�g�f�B�X�v���C���X�g�|�C���^
	int				xx=0;												// �����W
	int				xx2;												// �E���W

	if( g_leflg ){
		xx2=g_lexxd-2;													// �E���W
		displayChrBox( g_lexxc, g_leyyc, g_lexxd, g_leyyd, g_lettl );	// �L�����N�^�{�b�N�X�\��
		dldbp=txtdl( g_lebuf, g_lecurxx, g_tabstp, g_tabdsp, 0, 0, 0, 0, xx2 );	// �e�L�X�g�f�B�X�v���C���X�g�R���p�C��
		dbFOR( dldbp, dlp, stTXTDL* ){									// �e�L�X�g�f�B�X�v���C���X�g������
			if( dlp->cur ){												// �J�[�\��
				bakforcola[1]=cola[0];									// �e�L�X�g�J���[�Z�b�g
			}else{
				bakforcola[1]=cola[dlp->col];							// �e�L�X�g�J���[�Z�b�g
			}
			if( dbKEYSIZ( dlp ) ){
				drwTxt( g_lexxc+1+xx, g_leyyc+1, (char*)dbKEYADR( dlp ), dbKEYSIZ( dlp ), bakforcola[dlp->rvs], bakforcola[!dlp->rvs] );
			}else{
				drwFil( g_lexxc+1+xx, g_leyyc+1, g_lexxc+1+xx+dlp->dspsiz, g_leyyc+2, bakforcola[dlp->rvs] );
			}
			xx+=dlp->dspsiz;											// ���W�X�V
		}
		dbClose( dldbp );												// �f�B�X�v���C���X�gDB�N���[�Y
		if( xx<xx2 ){													// �󂫃G���A�����݂���
			drwFil( g_lexxc+1+xx, g_leyyc+1, g_lexxc+1+xx2, g_leyyc+2, CR_BLACK );				// �N���A
		}
	}
}

/************************************************************************/
/*	��s�ҏW���W�v�Z													*/
/************************************************************************/

static void	linedtCrd( int *xxcp, int *yycp, int *xxdp, int *yydp ){

	int		xxw;
	int		xx1, yy1, xx2, yy2;

	xxw=g_xxw-2;
	if( xxw>LEW ){
		xxw=LEW;
	}
	xx1=g_txxc+CURXX-PGXX-1;
	if( xx1<g_txxc ){
		xx1=g_txxc;
	}
	yy1=g_tyyc+CURYY+1;												
	xx2=xx1+xxw+2;
	yy2=yy1+3;
	if( xx2>g_xxw ){
		xx2=g_xxw;
		xx1=xx2-xxw-2;
	}
	if( yy2>g_yyw ){
		yy1=CURYY-3;
		yy2=yy1+3;
	}
	*xxcp=xx1;
	*yycp=yy1;
	*xxdp=xx2-xx1;
	*yydp=yy2-yy1;
}

/************************************************************************/
/*	�P�s�X�N���[���G�f�B�b�g											*/
/************************************************************************/

static int	linedts(void){

	stLM		lm;
	int			curtmp;
	int			keysym;
	int			ctrlv=0;												// CTRL+V�t���O
	char		*cutp;
	stDB		*cbdbp;													// �N���b�v�{�[�hDB
	char		tmpa[256];

	while( !g_flgend ){
		lmMak( &lm, g_lebuf, g_tabstp, 0 );
		invalidateLinedt();
		keysym=getKey()&0xffff;
		if( !ctrlv ){													// �O�̃L�[��CTRL+V�ł͂Ȃ�����
			switch( keysym ){
				case 0x0016:											// CTRL+V
					ctrlv=1;											// CTRL+V�t���O�Z�b�g
					break;
				case 0x0003:											// CTRL+C
				case 0x001b:											// ESC
				case MKEY_ESC_ESC:										// ESC
					return( 0 );										// �L�����Z��
				case 0x000d:											// CR
					return( 1 );										// �ҏW�I��
				case 0x001e:
					getcurword( tmpa );
					g_lecurxx=lmSetStr( g_lebuf, g_lecurxx, tmpa, g_tabstp, g_flgins );
					break;
				case 0x0008:											// BS
					g_lecurxx=lmPosAdj( &lm, g_lecurxx, 0 );
					if( g_lecurxx ){
						curtmp=lmPosAdj( &lm, g_lecurxx, -1 );
						strcpy( g_lebuf+lm.txtofsa[curtmp], g_lebuf+lm.txtofsa[g_lecurxx] );
						g_lecurxx=curtmp;
					}
					break;
				case MKEY_DELETE:										// DEL
					g_lecurxx=lmPosAdj( &lm, g_lecurxx, 0 );
					if( g_lecurxx!=lm.siz ){
						curtmp=lmPosAdj( &lm, g_lecurxx, 1 );
						strcpy( g_lebuf+lm.txtofsa[g_lecurxx], g_lebuf+lm.txtofsa[curtmp] );
					}
					break;
				case MKEY_RIGHT:										// �E
					g_lecurxx=lmPosAdj( &lm, g_lecurxx, 0 );
					if( g_lecurxx!=lm.siz ){
						g_lecurxx=lmPosAdj( &lm, g_lecurxx, 1 );
					}
					break;
				case MKEY_LEFT:											// ��
					g_lecurxx=lmPosAdj( &lm, g_lecurxx, 0 );
					if( g_lecurxx ){
						g_lecurxx=lmPosAdj( &lm, g_lecurxx, -1 );
					}
					break;
				case MKEY_F9:
					cbdbp=clpbrdGet( CLPBRDP, CB_TXT );					// �N���b�v�{�[�h�Q�b�g
					if( cbdbp ){
						cutp=(char*)dbTop( cbdbp );
						if( cutp ){
							strcpy( g_lebuf, cutp );
							g_lecurxx=lmMak( &lm, g_lebuf, g_tabstp, 0 );
						}
						dbClose( cbdbp );								// �N���b�v�{�[�hDB�N���[�Y
					}
					break;
				default:
					g_lecurxx=lmSetChr( g_lebuf, g_lecurxx, keysym, g_tabstp, g_flgins );
					break;
			}
		}else{															// �O�̃L�[��CTRL+V
			ctrlv=0;													// CTRL+V�t���O�N���A
			g_lecurxx=lmSetChr( g_lebuf, g_lecurxx, keysym, g_tabstp, g_flgins );
		}
	}
	return( 0 );
}

/************************************************************************/
/*	��s�ҏW															*/
/************************************************************************/

int		linedt( char *bufadr, const char *ttlp ){

	int			res;													// ���U���g
	stLM		lm;														// �s�}�b�v

	strcpy( g_lebuf, bufadr );											// ��s�ҏW�o�b�t�@
	strcpy( g_lettl, ttlp );											// ��s�ҏW�^�C�g��
	linedtCrd( &g_lexxc, &g_leyyc, &g_lexxd, &g_leyyd );				// ��s�ҏW���W�v�Z
	g_lecurxx=lmMak( &lm, g_lebuf, g_tabstp, 0 );						// �J�[�\�����s�I�[�ɃZ�b�g
	g_leflg=1;															// ��s�ҏW�t���O�Z�b�g
	res=linedts();														// �P�s�X�N���[���G�f�B�b�g
	g_leflg=0;															// ��s�ҏW�t���O�N���A
	if( res ){															// ����I������
		strcpy( bufadr, g_lebuf );										// ���o�b�t�@�ɃR�s�[
	}
	invalidateAll();													// �S�čĕ`��
	return( res );
}

/************************************************************************/
/*	��s�ҏW���Q��														*/
/************************************************************************/

int		linedt2( char *buf1adr, const char *ttl1p, char *buf2adr, const char *ttl2p ){

	int		res=0;

	if( linedt( buf1adr, ttl1p ) ){										// ��s�ҏW
		if( buf1adr[0] ){
			res=linedt( buf2adr, ttl2p );								// ��s�ҏW
		}
	}
	return( res );
}

