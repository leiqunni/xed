/************************************************************************/
/*																		*/
/*	���C�����[�v				by H.Watanabe							*/
/*	Ver0.00	2007/05/09 21:51:53	�쐬�J�n								*/
/*																		*/
/************************************************************************/

#	include	"main.h"													/* ���C���C���N���[�h�t�@�C��*/

/************************************************************************/
/*	�R�}���h�e�[�u��													*/
/************************************************************************/

	typedef struct{
		int				xedk;											// �L�[
		void			(*prcp)(void);									// �����A�h���X
	}	stCMD;

	static const stCMD	s_cmda[]={
		{	0x0001,							ctrl_a		},				// CTRL+A		�P�����Ɉړ�
		{	0x0002,							ctrl_b		},				// CTRL+B		���s�[�Ɉړ�
		{	0x0003,							ctrl_c		},				// CTRL+C		��ʂ����[���A�b�v
		{	0x0004,							ctrl_d		},				// CTRL+D		�P�����O���Ɉړ�
		{	0x0005,							ctrl_e		},				// CTRL+E		�P�s��Ɉړ�
		{	0x0006,							ctrl_f		},				// CTRL+F		�P��O���Ɉړ�
		{	0x0007,							ctrl_g		},				// CTRL+G		�P�����폜
		{	0x0008,							ctrl_h		},				// CTRL+H		�o�b�N�X�y�[�X
		{	0x000a,							ctrl_j		},				// CTRL+J		�w���v��ʂ̕\��
		{	0x000b,							ctrl_k		},				// CTRL+K		�s���܂ō폜
		{	0x000c,							ctrl_l		},				// CTRL+L		�폜�o�b�t�@����}��
		{	0x000d,							ctrl_m		},				// CTRL+M		���s(�s����)
		{	0x000e,							ctrl_n		},				// CTRL+N		�P�s�}��
		{	0x000f,							ctrl_o		},				// CTRL+O		�}�����[�h�؂�ւ�
		{	0x0010,							ctrl_p		},				// CTRL+P		�E�[�Ɉړ�
		{	0x0011,							ctrl_q		},				// CTRL+Q		���[�Ɉړ�
		{	0x0012,							ctrl_r		},				// CTRL+R		��ʂ����[���_�E��
		{	0x0013,							ctrl_s		},				// CTRL+S		�P��������Ɉړ�
		{	0x0014,							ctrl_t		},				// CTRL+T		�P��폜
		{	0x0015,							ctrl_u		},				// CTRL+U		�s������폜
		{	0x0016,							ctrl_v		},				// CTRL+V		���ڃR�[�h����
		{	0x0017,							ctrl_w		},				// CTRL+W		��ʂ��P�s���[���_�E��
		{	0x0018,							ctrl_x		},				// CTRL+X		�P�s���Ɉړ�
		{	0x0019,							ctrl_y		},				// CTRL+Y		�P�s�폜
		{	0x001a,							ctrl_z		},				// CTRL+Z		�P�s���[���A�b�v
//		{	0x001b,							esc			},				// CTRL+[		ESC �R�}���h
		{	0x001c,							ctrl_yen	},				// CTRL+\		�J�����g���[�h�������
		{	0x001d,							ctrl_lbktc	},				// CTRL+]		�啶���E�������ϊ�
		{	0x001e,							ctrl_hat	},				// CTRL+^		�J�����g���[�h�O������
		{	0x001f,							ctrl__		},				// CTRL+_		�t�@�C���I�[�L��

		{	MKEY_ESC,						esc			},
		{	MKEY_ESC_ESC,					esc_esc		},

		{	MKEY_ESC_0,						esc_0		},
		{	MKEY_ESC_1,						esc_1		},
		{	MKEY_ESC_2,						esc_2		},
		{	MKEY_ESC_3,						esc_3		},
		{	MKEY_ESC_4,						esc_4		},
		{	MKEY_ESC_5,						esc_5		},
		{	MKEY_ESC_6,						esc_6		},
		{	MKEY_ESC_7,						esc_7		},
		{	MKEY_ESC_8,						esc_8		},
		{	MKEY_ESC_9,						esc_9		},

		{	MKEY_ESC_AT,					f11			},
		{	MKEY_ESC_A,						esc_a		},
		{	MKEY_ESC_B,						esc_b		},
		{	MKEY_ESC_C,						esc_c		},
		{	MKEY_ESC_D,						esc_d		},
		{	MKEY_ESC_E,						esc_e		},
		{	MKEY_ESC_F,						esc_f		},
//		{	MKEY_ESC_G,						esc_g		},
		{	MKEY_ESC_H,						esc_h		},
		{	MKEY_ESC_I,						esc_i		},
		{	MKEY_ESC_J,						esc_j		},
		{	MKEY_ESC_K,						esc_k		},
		{	MKEY_ESC_L,						esc_l		},
		{	MKEY_ESC_M,						esc_m		},
		{	MKEY_ESC_N,						esc_n		},
		{	MKEY_ESC_O,						esc_o		},
//		{	MKEY_ESC_P,						esc_p		},
		{	MKEY_ESC_Q,						esc_q		},
		{	MKEY_ESC_R,						esc_r		},
		{	MKEY_ESC_S,						esc_s		},
		{	MKEY_ESC_T,						esc_t		},
		{	MKEY_ESC_U,						esc_u		},
		{	MKEY_ESC_V,						esc_v		},
//		{	MKEY_ESC_W,						esc_c		},
		{	MKEY_ESC_X,						esc_x		},
		{	MKEY_ESC_Y,						esc_y		},
		{	MKEY_ESC_Z,						esc_z		},
		{	MKEY_ESC_LBO,					esc_lbo		},
		{	MKEY_ESC_YEN,					esc_yen		},
		{	MKEY_ESC_LBC,					esc_lbc		},
		{	MKEY_ESC_HAT,					esc_hat		},
		{	MKEY_ESC__,						esc__		},
	
		{	MKEY_ESC_a,						esc_a		},
		{	MKEY_ESC_b,						esc_b		},
		{	MKEY_ESC_c,						esc_c		},
		{	MKEY_ESC_d,						esc_d		},
		{	MKEY_ESC_e,						esc_e		},
		{	MKEY_ESC_f,						esc_f		},
//		{	MKEY_ESC_g,						esc_g		},
		{	MKEY_ESC_h,						esc_h		},
		{	MKEY_ESC_i,						esc_i		},
		{	MKEY_ESC_j,						esc_j		},
		{	MKEY_ESC_k,						esc_k		},
		{	MKEY_ESC_l,						esc_l		},
		{	MKEY_ESC_m,						esc_m		},
		{	MKEY_ESC_n,						esc_n		},
		{	MKEY_ESC_o,						esc_o		},
//		{	MKEY_ESC_p,						esc_p		},
		{	MKEY_ESC_q,						esc_q		},
		{	MKEY_ESC_r,						esc_r		},
		{	MKEY_ESC_s,						esc_s		},
		{	MKEY_ESC_t,						esc_t		},
		{	MKEY_ESC_u,						esc_u		},
		{	MKEY_ESC_v,						esc_v		},
		{	MKEY_ESC_w,						esc_c		},
		{	MKEY_ESC_x,						esc_x		},
		{	MKEY_ESC_y,						esc_y		},
		{	MKEY_ESC_z,						esc_z		},

		{	MKEY_INSERT,					ctrl_o		},				// CTRL+O		�}�����[�h�؂�ւ�
		{	MKEY_DELETE,					ctrl_g		},				// CTRL+G		�P�����폜
		{	MKEY_LEFT,						ctrl_s		},				// CTRL+S		�P��������Ɉړ�
		{	MKEY_UP,						ctrl_e		},				// CTRL+E		�P�s��Ɉړ�
		{	MKEY_RIGHT,						ctrl_d		},				// CTRL+D		�P�����O���Ɉړ�
		{	MKEY_DOWN,						ctrl_x		},				// CTRL+X		�P�s���Ɉړ�
		{	MKEY_HOME,						esc_a		},				// HOME			�e�L�X�g�̏����؂�ւ�
		{	MKEY_END,						ctrl_j		},				// CTRL+J		�w���v��ʂ̕\��
		{	MKEY_PGDOWN,					ctrl_c		},				// CTRL+C		��ʂ����[���A�b�v
		{	MKEY_PGUP,						ctrl_r		},				// CTRL+R		��ʂ����[���_�E��
		{	MKEY_DELETE,					ctrl_g		},				// CTRL+G		�P�����폜
		{	MKEY_F1,						esc_b		},				// ESC+B		�擪�ֈړ�
		{	MKEY_F2,						esc_z		},				// ESC+Z		�����ֈړ�
		{	MKEY_F3,						f3			},				// F3			�J�����g���[�h�O���u��
		{	MKEY_F4,						ctrl_hat	},				// CTRL+^		�J�����g���[�h�O������
		{	MKEY_F5,						f5			},				// F5			�p���O������
		{	MKEY_F6,						f6			},				// F6			�͈͎w��J�n
		{	MKEY_F7,						f7			},				// F7			�J�b�g�o�b�t�@�ֈړ�
		{	MKEY_F8,						f8			},				// F8			�J�b�g�o�b�t�@�֕���
		{	MKEY_F9,						f9			},				// F9			�J�b�g�o�b�t�@����}��
		{	MKEY_F10,						f10			},				// F10			�s����
		{	MKEY_F11,						f11			},				// ESC+@		�L�[�{�[�h�}�N���̒�`
		{	MKEY_F12,						f12			},				// UNDO			�}�N�����s
		{	MKEY_F5		| MKEY_SHIFT,		shift_f5	},				// SHIFT+F5		�p���O������
		{	MKEY_F6		| MKEY_SHIFT,		shift_f6	},				// SHIFT+F6		�͈͎w��J�n
		{	MKEY_F7		| MKEY_SHIFT,		shift_f7	},				// SHIFT+F7		�J�b�g�o�b�t�@�ֈړ�
		{	MKEY_F8		| MKEY_SHIFT,		shift_f8	},				// SHIFT+F8		�J�b�g�o�b�t�@�֕���
		{	MKEY_F9		| MKEY_SHIFT,		shift_f9	},				// SHIFT+F9		�J�b�g�o�b�t�@����}��
		{	0x0009		| MKEY_SHIFT,		ctrl_tab	},				// SHIFT+TAB	�����^�u
		{	0x0009		| MKEY_CTRL,		ctrl_tab	},				// CTRL+I		�����^�u
		{	MKEY_F5		| MKEY_ALT,			alt_f5		},				// ALT+F5		�E�B���h�E���̃T�C�Y�ɖ߂�
		{	MKEY_F9		| MKEY_ALT,			alt_f9		},				// ALT+F9		�E�B���h�E�ŏ���
		{	MKEY_F10	| MKEY_ALT,			alt_f10		},				// ALT+F10		�E�B���h�E�ő剻
		{	'/'			| MKEY_ALT,			alt_slash	},				// ALT+/		�d��
		{	'*'			| MKEY_ALT,			alt_ast		},				// ALT+*		���������d��
		{	'+'			| MKEY_ALT,			alt_plus	},				// ALT++		�V�F���R�}���h
#	ifndef	_WIN32
		{	0x0020		| MKEY_SHIFT,		shift_space	},				// SHIFT+SPACE	FEP�I�[�v��
		{	0x0020		| MKEY_CTRL,		shift_space	},				// SHIFT+SPACE	FEP�I�[�v��
		{	MKEY_F9		| MKEY_CTRL,		ctrl_f9		},				// CTRL+F9		�Z���N�V�����ǂݍ���
#	endif
		{	0,								0			}				// �I�[�R�[�h
	};

/************************************************************************/
/*	���C�����[�v														*/
/************************************************************************/

void	xedmain(void){

	stDB		*cmddbp;												// �R�}���hDB
	int			keyatrsym;												// �A�g���r���[�g�{�L�[
	int			keysym;													// �L�[
	int			curbak;													// �J�[�\���X�C�b�`�o�b�N�A�b�v
	const stCMD	*cmdp;													// �R�}���h�|�C���^

	cmddbp=dbOpen( sizeof(int), sizeof(stCMD), 4096, 4096 );			// �R�}���hDB�I�[�v��
	for( cmdp=s_cmda; cmdp->xedk; cmdp++ ){								// �e�[�u�������胋�[�v
		dbAlc( cmddbp, &cmdp->xedk, cmdp );								// �R�}���h�o�^
	}
	if( g_optlno ){														// �I�v�V�����ōs�ԍ����w�肳��Ă���
		lnoset( g_optlno-1 );											// �s�ԍ��Z�b�g
	}
	curSw( 1 );															// �J�[�\���I��
	while( !g_flgend ){													// �I���t���O�������Ă��Ȃ��ԃ��[�v
		keyatrsym=getKey();												// �L�[�𓾂�
		msgSet( "" );													// ���b�Z�[�W�N���A
		if( !keyatrsym ){												// �L�[����������
			break;														// �I��
		}
		curbak=curSw( 0 );												// �J�[�\���I�t
		keysym=keyatrsym&0x0000ffff;									// �A�g���r���[�g���������L�[�V���{��
		cmdp=(stCMD*)dbSch( cmddbp, &keyatrsym );						// �A�g���r���[�g�{�L�[����
		if( !cmdp ){													// ������Ȃ�
			cmdp=(stCMD*)dbSch( cmddbp, &keysym );						// �L�[�݂̂Ō���
		}
		if( cmdp ){														// �R�}���h����������
			cmdp->prcp();												// �R�}���h����
		}else{															// �R�}���h��������Ȃ�
			if( !( keyatrsym&0xffff0000 ) ){							// �A�g���r���[�g�͂Ȃ�
				setchr( keysym );										// �ꕶ���Z�b�g
			}
		}
		if( !g_flgend ){												// �I���ł͂Ȃ�
			curSw( curbak );											// �J�[�\���I��
			if( CUTCURP && CUTTYP ){									// ��`�I�����[�h
				invalidateAll();										// �S�ĕ\��
			}else{
				invalidateInf();										// �C���t�H���[�V�����ĕ`��
			}
		}
		g_scdbiz=0;														// �����\���T�C�Y�N���A
	}
	dbClose( cmddbp );													// �R�}���hDB�N���[�Y
}

