/************************************************************************/
/*																		*/
/*	���C���}�b�v				by H.Watanabe							*/
/*	Ver1.00	2009/04/05 13:23:49	�쐬									*/
/*																		*/
/************************************************************************/

#	ifndef	LM_H
#	define	LM_H
#	ifdef	__cplusplus
	extern "C"{
#	endif

#	ifndef	LM_MAX
#	define	LM_MAX		4096
#	endif

#	if	LM_MAX<256
	typedef unsigned char	tpLM;
#	elif LM_MAX<65536
	typedef unsigned short	tpLM;
#	else
	typedef unsigned int	tpLM;
#	endif

	typedef struct{
		int					siz;
		tpLM				txtofsa[LM_MAX+1];
	}	stLM;

	int		lmMak( stLM *lmp, const char *txtadr, int tab, int wordflg );		// ���C���}�b�v�쐬
	int		lmPosAdj( stLM *lmp, int pos, int ofs );					// ���C���}�b�v�ɂ��J�[�\���|�W�V�����̐��K��
	int		lmOfs2Pos( stLM *lmp, int txtofs );							// �\�[�X�̃I�t�Z�b�g����J�[�\���|�W�V�����𓾂�
	int		lmPos2Ofs( stLM *lmp, int pos );							// �J�[�\���|�W�V��������\�[�X�̃I�t�Z�b�g�𓾂�
	int		lmSetStr( char *txtadr, int curxx, const char *strp, int tabstp, int flgins );
	int		lmSetChr( char *txtadr, int curxx, int xedk, int tabstp, int flgins );

#	ifdef	__cplusplus
	}
#	endif
#	endif

