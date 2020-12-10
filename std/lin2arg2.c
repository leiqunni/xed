/************************************************************************/
/*																		*/
/*	�R�}���h���C����������z��ւ̕ϊ�									*/
/*	Ver1.00	2009/04/04 13:17:57	�쐬									*/
/*																		*/
/************************************************************************/

#	include	<stdlib.h>													/* �W�����C�u�����֐�*/
#	include	<string.h>													/* �W��������֐�*/
#	include	"db2.h"														/* �v���O�������ėp�f�[�^�x�[�X�V�X�e��*/
#	include	"lin2arg2.h"												/* �R�}���h���C����������z��ւ̕ϊ�*/

/************************************************************************/
/*	���[�J����`														*/
/************************************************************************/

#	define	SKPSP(P)		((P)+strspn(P,"\t "))						/* �X�y�[�X�A�^�u�̃X�L�b�v*/

/************************************************************************/
/*	�R�}���h���C����������z��ւ̕ϊ�									*/
/************************************************************************/

char	**lin2arg( const char *cmdlin ){

	const char	*txtp;
	char		*strp, **argv;
	char		*stre;
	int			argn=0;
	int			i;

	txtp=cmdlin;
	for(;;){
		txtp=SKPSP( txtp );
		if( !*txtp ){
			break;
		}
		txtp=dbGetDQ( 0, 0, txtp );										// �_�u���N�H�[�e�[�V�����ň͂܂ꂽ������̎��𓾂�
		argn++;
	}
	argv=malloc( (argn+1)*4 );
	strp=malloc( txtp-cmdlin+1 );
	stre=strp+(txtp-cmdlin+1);
	txtp=cmdlin;
	for( i=0; i<argn; i++ ){
		argv[i]=strp;
		txtp=dbGetDQ( strp, stre-strp, SKPSP( txtp ) );					// �_�u���N�H�[�e�[�V�����ň͂܂ꂽ������𓾂�
		while( *strp++ );
	}
	argv[i]=0;
	return( argv );
}

