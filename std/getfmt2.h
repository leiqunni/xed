/************************************************************************/
/*																		*/
/*	�t�H�[�}�b�g�t����������	by H.Watanabe							*/
/*	Ver1.00	2009/04/05 13:52:04	�쐬									*/
/*																		*/
/************************************************************************/

#	ifndef	GETFMT2_H
#	define	GETFMT2_H
#	ifdef	__cplusplus
	extern "C"{
#	endif

#	ifndef	FPTYP														/* FPTYP ����`����Ă��Ȃ�*/
#		ifdef	_WIN32													/* Windows �̏ꍇ*/
#			define	FPTYP	double										/* �P���x���g�p����ƕs�v�c�Ȃ��Ƃ����낢��N����̂Ŕ{���x���g�p����*/
#			define	FPSIZ	8
#		else															/* Windows �ȊO�̏ꍇ*/
#			define	FPTYP	float										/* WIN32 �ȊO�ł̓������������d�����ĒP���x���g�p����*/
#			define	FPSIZ	4
#		endif
#	endif

	char	*getfmt( const char *txtp, const char *format, ... );

#	ifdef	__cplusplus
	}
#	endif
#	endif

