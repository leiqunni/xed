/************************************************************************/
/*																		*/
/*	�p�X���֘A					by H.Watanabe							*/
/*	Ver1.00	2009/03/29 10:03:05	�쐬									*/
/*																		*/
/************************************************************************/

#	ifndef	PATH2_H
#	define	PATH2_H
#	ifdef	__cplusplus
	extern "C"{
#	endif

	char	*pathSeekNam( char *pathp, const char *txtadr );			// �m�[�h���܂ŃV�[�N����
	char	*pathSeekExt( char *pathp, char *namp, const char *txtadr );// �g���q���܂ŃV�[�N����
	void	pathCat( char *dstp, const char *pathp, const char *namp );	// �p�X���ƃt�@�C������A������
	void	pathRepExt( char *dstp, const char *soup, const char *extp );		// �g���q���ύX

#	ifdef	__cplusplus
	}
#	endif
#	endif

