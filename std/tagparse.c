/************************************************************************/
/*																		*/
/*	�^�O���					by H.Watanabe							*/
/*	Ver1.00	2009/04/05 13:52:04	�쐬									*/
/*																		*/
/************************************************************************/

#	include	<stdio.h>													/* �W�����o�͊֐�*/
#	include	<stdlib.h>													/* �W�����C�u�����֐�*/
#	include	<string.h>													/* �W��������֐�*/
#	include	<sys/stat.h>												/* �t�@�C���X�e�[�^�X*/
#	include	"getfmt2.h"													/* �t�H�[�}�b�g�t����������*/
#	include	"path2.h"													/* �p�X���֘A*/
#	include	"tagparse.h"												/* �^�O���*/

/************************************************************************/
/*	���[�J����`														*/
/************************************************************************/

#	define	SKPSP(P)		((P)+strspn(P,"\t "))						/* �X�L�b�v�X�y�[�X*/

/************************************************************************/
/*	�t�@�C�����𓾂�													*/
/************************************************************************/

static int		filchk( const char *pacnamp ){

	struct stat	st;
	int			errn;

#	ifdef	unix
	errn=lstat( pacnamp, &st );
#	else
	errn=stat( pacnamp, &st );
#	endif
	return( !errn );
}

/************************************************************************/
/*	�C���N���[�h�t�@�C���T�[�`											*/
/************************************************************************/

static int	includeSchSys( char *dstp, const char *fnamp ){

	int			res=0;
	const char	*txtp,*nxtp;
	char		tmpa[256];
	char		fnam[256];

	*dstp=0;
#	ifdef	unix
	txtp	="/usr/include";
#	else
	txtp	=getenv( "include" );
#	endif
	if( txtp ){
		while( *txtp ){
			nxtp	=strchr( txtp, ';' );
			if( !nxtp ){
				nxtp	=txtp+strlen( txtp );
			}
			memcpy( tmpa, txtp, nxtp-txtp );
			tmpa[nxtp-txtp]=0;
			txtp=nxtp;
			if( tmpa[0] ){
				pathCat( fnam, tmpa, fnamp );
				if( filchk( fnam ) ){
					strcpy( dstp, fnam );
					res=1;
					break;
				}
			}
			if( *txtp!=';' ){
				break;
			}
			txtp++;
		}
	}
	return( res );
}

/************************************************************************/
/*	�^�O���															*/
/************************************************************************/

int		tagparse( int *lnop, char *dstp, const char *txtadr, const char *curfnamp ){

	char	*txtp,fnam[256],tmpa[256],curpath[256];
	int		lno=1;

	if( txtadr[0]=='/' && txtadr[1]=='/' ){
		txtadr=SKPSP( txtadr+2 );
	}
	txtadr=SKPSP( txtadr );
	txtp=getfmt( txtadr, "%s %d:", fnam, &lno );
	if( !txtp ){
		txtp=getfmt( txtadr, "%s:%d", fnam, &lno );
		if( !txtp ){
			txtp=getfmt( txtadr, "\"%s\" %d:", fnam, &lno );
			if( !txtp ){
				txtp=getfmt( txtadr, "%s %s %s %d:", 0, 0, fnam, &lno );
				if( !txtp ){
					txtp=getfmt( txtadr, "%s:%d:", fnam, &lno );
					if( !txtp ){
						txtp=getfmt( txtadr, "%s: %s: %s, line %d:", 0, 0, fnam, &lno );
						if( !txtp ){
							txtp=getfmt( txtadr, "%s: %s: %s: %d:", 0, 0, fnam, &lno );
							if( !txtp ){
								txtp=getfmt( txtadr, "%s(%d) :", fnam, &lno );
								if( !txtp ){
									txtp=getfmt( txtadr, "\"%s\", line %d:", fnam, &lno );
									if( !txtp ){
										txtp=getfmt( txtadr, "# include \"%s\"", tmpa );
										if( txtp ){
											pathSeekNam( curpath, curfnamp );
											pathCat( fnam, curpath, tmpa );
										}else{
											txtp=getfmt( txtadr, "# include <%s>", tmpa );
											if( txtp ){
												includeSchSys( fnam, tmpa );
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	if( !txtp ){
#	ifdef	unix
		txtp=getfmt( txtadr, "%s %d %d/%d/%d %d:%d:%d ", 0, 0, 0, 0, 0, 0, 0, 0 );
#	else
		txtp=getfmt( txtadr, " %d %d/%d/%d %d:%d:%d ", 0, 0, 0, 0, 0, 0, 0 );
#	endif
		if( txtp ){
			strcpy( fnam, txtp );
			lno=1;
		}
	}
	if( !txtp ){
		txtp=getfmt( txtadr, " %d ", 0 );
		if( txtp ){
			strcpy( fnam, txtp );
			lno=1;
		}
	}
	if( txtp ){
		*lnop=lno;
		strcpy( dstp, fnam );
	}
	return( txtp!=0 );
}

