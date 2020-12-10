/************************************************************************/
/*																		*/
/*	�w���v						by H.Watanabe							*/
/*	Ver1.00	2009/04/04 13:49:05	�쐬�J�n								*/
/*																		*/
/************************************************************************/

#	include	<assert.h>													/* �A�T�[�V����*/
#	include	<stdio.h>													/* �W�����o�͊֐�*/
#	include	<string.h>													/* �W��������֐�*/
#	include	"std/txtdl.h"												/* �e�L�X�g�G�f�B�^�̂��߂̃f�B�X�v���C���X�g�ϊ�*/
#	include	"main.h"													/* ���C���C���N���[�h�t�@�C��*/

/************************************************************************/
/*	���[�J����`														*/
/************************************************************************/

#	define	HELPXW	64													/* �w���v����*/
#	define	HELPYW	22													/* �w���v�c��*/

	static 	const char		**g_helppp;									// �w���v�|�C���^

/************************************************************************/
/*	�w���v�f�[�^														*/
/************************************************************************/

	static const char *helpAsc[]={
	   "				�� �`�r�b�h�h�R�[�h�ꗗ ��",
	   "",
	   "		0x 1x 2x 3x 4x 5x 6x 7x 8x 9x Ax Bx Cx Dx Ex Fx",
	   "	0     dle     0  @  P  `  p  .  .  .  �  �  �  .  .",
	   "	1 INS dc1  !  1  A  Q  a  q  .  .  �  �  �  �  .  .",
	   "	2 stx dc2  \"  2  B  R  b  r  .  .  �  �  �  �  .  .",
	   "	3 BRK dc3  #  3  C  S  c  s  .  .  �  �  �  �  .  .",
	   "	4 eot dc4  $  4  D  T  d  t  .  .  �  �  �  �  .  .",
	   "	5 enq UND  %  5  E  U  e  u  .  .  �  �  �  �  .  .",
	   "	6 ack syn  &  6  F  V  f  v  .  .  �  �  �  �  .  .",
	   "	7 BEL etb  '  7  G  W  g  w  .  .  �  �  �  �  .  .",
	   "	8 BS  can  (  8  H  X  h  x  .  .  �  �  �  �  .  .",
	   "	9 HT  em   )  9  I  Y  i  y  .  .  �  �  �  �  .  .",
	   "	A LF  sub  *  :  J  Z  j  z  .  .  �  �  �  �  .  .",
	   "	B HOM ESC  +  ;  K  [  k  {  .  .  �  �  �  �  .  .",
	   "	C CLR ��   ,  <  L  \\  l  |  .  .  �  �  �  �  .  .",
	   "	D CR  ��   -  =  M  ]  m  }  .  .  �  �  �  �  .  .",
	   "	E RUP ��   .  >  N  ^  n  ~  .  .  �  �  �  �  .  .",
	   "	F RDO ��   /  ?  O  _  o  DEL.  .  �  �  �  �  .  .",
		0
	};

	static const char *helpCtrl[]={
	   "				�� �b�s�q�k�L�[����ꗗ ��",
	   "",
	   "CTRL+A	�P�����Ɉړ�			CTRL+Q	���[�Ɉړ�",
	   "CTRL+B	���s�[�Ɉړ�			CTRL+R	��ʂ����[���_�E��",
	   "CTRL+C	��ʂ����[���A�b�v		CTRL+S	�P��������Ɉړ�",
	   "CTRL+D	�P�����O���Ɉړ�		CTRL+T	�P��폜",
	   "CTRL+E	�P�s��Ɉړ�			CTRL+U	�s������폜",
	   "CTRL+F	�P��O���Ɉړ�			CTRL+V	���ڃR�[�h����",
	   "CTRL+G	�P�����폜				CTRL+W	�P�s���[���_�E��",
	   "CTRL+H	�o�b�N�X�y�[�X			CTRL+X	�P�s���Ɉړ�",
	   "CTRL+I	�����^�u				CTRL+Y	�P�s�폜",
	   "CTRL+J	�w���v��ʂ̕\��		CTRL+Z	�P�s���[���A�b�v",
	   "CTRL+K	�s���܂ō폜			CTRL+[	�d�r�b�R�}���h",
	   "CTRL+L	�폜�o�b�t�@����}��	CTRL+\\	���{�����",
	   "CTRL+M	���s(�s����)			CTRL+]	�啶���E�������ϊ�",
	   "CTRL+N	�P�s�}��				CTRL+^	�J�����g���[�h�O������",
	   "CTRL+O	�}�����[�h�؂�ւ�		CTRL+_	�t�@�C���I�[�L��",
	   "CTRL+P	�E�[�Ɉړ�",
		0
	};

	static const char *helpEsc[]={
	   "				�� �d�r�b�L�[����ꗗ ��",
	   "",
	   "ESC+NUM	�w��s�ԍ��ւ̈ړ�		ESC+<���l>T �^�u�X�e�b�v�ύX",
	   "ESC+@	�L�[�{�[�h�}�N���̒�`	ESC+<���l>N �s�ԍ��\��",
	   "ESC+A	�e�L�X�g�̏����؂�ւ�	ESC+Q	�S�e�L�X�g��j�����I��",
	   "ESC+B	�擪�ֈړ�				ESC+R	�O���u��",
	   "ESC+C	�q�v���Z�X�̎��s		ESC+S	�������",
	   "ESC+D	�t�@�B���̍~���؂�ւ�	ESC+T	�t�@�C�����ύX",
	   "ESC+E	�ҏW����				ESC+U	����u��",
	   "ESC+F	�V�����e�L�X�g�̕ҏW	ESC+V	�^�O�W�����v",
	   "ESC+G	���g�p					ESC+W	���g�p",
	   "ESC+H	���e�L�X�g�̃Z�[�u		ESC+X	���e�L�X�g�̊���",
	   "ESC+I	�^�u�����̕\���؂�ւ�	ESC+Y	�t�@�B���̓ǂݍ���",
	   "ESC+J	�O���m�F�u��			ESC+Z	�����ֈړ�",
	   "ESC+K	���e�L�X�g�̔j��		ESC+[	�J�����g���[�h�O���u��",
	   "ESC+L	����m�F�u��			ESC+\\	�J�����g���[�h����u��",
	   "ESC+M	���s�����̕\���^��\��	ESC+]	�啶���E�������ϊ��ؑ�",
	   "ESC+N	�O������				ESC+^	�O���u��(�\���Ȃ�)",
	   "ESC+O	���e�L�X�g�̍ă��[�h	ESC+_	����u��(�\���Ȃ�)",
	   "ESC+P	���g�p					ESC+-	�I�v�V����",
		0
	};

	static const char *helpAlt[]={
	   "				�� �`�k�s�L�[����ꗗ ��",
	   "",
#	ifdef	_WIN32
	   "ALT+F5			�E�B���h�E���̃T�C�Y�ɖ߂�",
	   "ALT+F9			�E�B���h�E�ŏ���",
	   "ALT+F10			�E�B���h�E�ő剻",
	   "",
#	endif
	   "ALT+'/'			16�i�d��",
	   "ALT+'*'			���������d��",
	   "ALT+'+'			�J�����g�s�V�F�����s",
		0
	};

	static const char *helpFn[]={
	   "				�� �t�@���N�V�����L�[����ꗗ ��",
	   "",
	   "F1		�擪�ֈړ�				F11		�L�[�{�[�h�}�N���̒�`",
	   "F2		�����ֈړ�				F12		�L�[�{�[�h�}�N���̎��s",
	   "F3		�O���A���u��			F13		���g�p",
	   "F4		�J�����g���[�h�O������	F14		���g�p",
	   "F5		�p���O������			F15		���g�p",
	   "F6		�͈͎w��J�n			F16		��`�͈͎w��J�n",
	   "F7		�J�b�g�o�b�t�@�ֈړ�	F17		F7 �Ɠ���",
	   "F8		�J�b�g�o�b�t�@�֕���	F18		F8 �Ɠ���",
	   "F9		�J�b�g�o�b�t�@����}��	F19		��`�}��",
	   "F10		�s�̂Q�d��				F20		���g�p",
	   "TAB		�����^�u				��		�P�s��Ɉړ�",
	   "BS		�o�b�N�X�y�[�X			��		�P�����E�Ɉړ�",
	   "CR		���s�i�s�����j			��		�P�������E�Ɉړ�",
	   "ESC		�d�r�b�R�}���h			��		�P�s���Ɉړ�",
	   "R_UP	��ʂ����[���A�b�v",
	   "R_DOWN	��ʂ����[���_�E��		END		������ʂ̕\��",
	   "INS		�}�����[�h�ؑ�			HOME	�e�L�X�g�̏����؂�ւ�",
	   "DEL		�P�����폜",
		0
	};

	static const char *helpOpt[]={
	   "				�� �N�����̃I�v�V���� ��",
	   "",
	   "-h<���l>	�^�u�����w�� ( 2 / 4 / [8] )",
	   "-n			.bak �t�@�C�����쐬���Ȃ�",
	   "-u			�����p�ꃂ�[�h",
	   "-e			�t�@�B���I�[�L����\�����Ȃ�",
	   "-l			���s������\�����Ȃ�",
	   "-lno<���l>	�ҏW�s�w��",
		0
	};

	static const char *helpGreek[]={
	   "				�� �M���V�������̒Ԃ� ��",
	   "",
	   "����:alpha	 �A���t�@	����:nu		 �j���[",
	   "����:beta	 �x�[�^		����:xi		 �O�U�C",
	   "����:gamma	 �K���}		����:omicron �I�~�N����",
	   "����:delta	 �f���^		����:pi		 �p�C",
	   "����:epsilon �C�v�V����	����:rho	 ���[",
	   "����:zeta	 �W�[�^		����:sigma	 �V�O�}",
	   "����:eta	 �C�[�^		����:tau	 �^�E",
	   "����:theta	 �V�[�^		����:upsilon �E�v�V����",
	   "����:iota	 �C�I�^		����:phi	 �t�@�C",
	   "����:kappa	 �J�b�p		����:chi	 �J�C",
	   "����:lambda	 �����_		����:psi	 �v�T�C",
	   "����:mu		 �~���[		����:omega	 �I���K",
		0
	};

	static const char	**HlpStr[]={
		helpAsc,														// �`�r�b�h�h�R�[�h�ꗗ
		helpGreek,														// �M���V�������̒Ԃ�
		helpCtrl,														// �b�s�q�k�L�[����ꗗ
		helpEsc,														// �d�r�b�L�[����ꗗ
		helpAlt,														// �`�k�s�L�[����ꗗ
		helpFn,															// �t�@���N�V�����L�[����ꗗ
		helpOpt,														// �N�����̃I�v�V����
		0																// �I�[�R�[�h
	};

/************************************************************************/
/*	�w���v���W�𓾂�													*/
/************************************************************************/

static void	helpCrd( int *xxcp, int *yycp, int *xxdp, int *yydp ){
	*xxdp=HELPXW;
	*xxcp=g_xxw-1-HELPXW;
	*yydp=HELPYW;
	*yycp=g_yyw-1-HELPYW;
}

/************************************************************************/
/*	���ʕ\��															*/
/************************************************************************/

static void	helps( int xxc, int yyc, int xxd, int yyd, const char **helppp ){

	const char	*helpp="";												// �w���v��s�|�C���^
	int			xx, yy;													// ���W
	stDB		*dldbp;													// �e�L�X�g�f�B�X�v���C���X�gDB
	stTXTDL		*dlp;													// �e�L�X�g�f�B�X�v���C���X�g�|�C���^

	for( yy=0; yy<yyd; yy++ ){
		if( helpp ){													// �܂��I�[�ɒB���Ă��Ȃ�
			helpp=helppp[yy];											// �|�C���^�𓾂�
		}
		xx=0;															// �w���W�N���A
		dldbp=txtdl( helpp?helpp:"", -1, g_tabstp, g_tabdsp, 0, 0, 0, 0, xxd );	// �e�L�X�g�f�B�X�v���C���X�g�R���p�C��
		dbFOR( dldbp, dlp, stTXTDL* ){									// �f�B�X�v���C���X�g������
			if( dbKEYSIZ( dlp ) ){
				drwTxt( xxc+xx, yyc+yy, (char*)dbKEYADR( dlp ), dbKEYSIZ( dlp ), CR_BLACK, CR_WHITE );
			}else{
				drwFil( xxc+xx, yyc+yy, xxc+xx+dlp->dspsiz, yyc+yy+1, CR_BLACK );
			}
			xx+=dlp->dspsiz;											// ���W�X�V
		}
		dbClose( dldbp );												// �f�B�X�v���C���X�gDB�N���[�Y
		if( xx<xxd ){													// �󂫃G���A�����݂���
			drwFil( xxc+xx, yyc+yy, xxc+xxd, yyc+yy+1, CR_BLACK );		// �N���A
		}
	}
}

/************************************************************************/
/*	�w���v�ĕ\��														*/
/************************************************************************/

void	helpReDsp(void){

	int			xxc, yyc, xxd, yyd;										// �w���v���W

	if( g_helppp ){														// �w���v�\���Œ�
		helpCrd( &xxc, &yyc, &xxd, &yyd );								// �w���v���W�𓾂�
		displayChrBox( xxc, yyc, xxd, yyd, "Hlep" );					// �L�����N�^�{�b�N�X�`��
		helps( xxc+1, yyc+1, xxd-2, yyd-2, g_helppp );					// �w���v�`��
	}
}

/************************************************************************/
/*	�w���v��ʂ̕\��													*/
/************************************************************************/

void	help(void){

	int			helpno=0;												// �w���v�ԍ�
	const char	**helppp;												// �w���v�y�[�W�|�C���^
	int			key;

	while( !g_flgend ){													// �I���t���O�������Ă��Ȃ��ԃ��[�v
		helppp=HlpStr[helpno];											// �w���v�ԍ�����w���v�y�[�W�|�C���^�𓾂�
		if( !helppp ){													// �I�[
			helpno--;
			continue;
		}
		g_helppp=helppp;												// �O���[�o�����[�N�Z�b�g
		invalidateHlp();												// �w���v�ĕ\��
		key=getKey();													// �L�[�𓾂�
		if( key==MKEY_PGUP || key==MKEY_UP || key==MKEY_LEFT ){
			helpno--;
			if( helpno<0 ){
				helpno=0;
			}
		}else if( key==' ' || key==MKEY_PGDOWN || key==MKEY_RIGHT || key==MKEY_DOWN ){
			helpno++;													// �w���v�ԍ����Z
		}else{
			break;
		}
	}
	g_helppp=0;															// �O���[�o�����[�N�N���A
	invalidateAll();													// �S�čĕ\��
}

