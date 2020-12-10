/************************************************************************/
/*																		*/
/*	�N���b�s���O				by H.Watanabe							*/
/*	Ver1.00	2009/04/05 13:52:04	�쐬									*/
/*																		*/
/************************************************************************/

#	include	"clp3.h"													/* �N���b�s���O*/

/************************************************************************/
/*	���[�J����`														*/
/************************************************************************/

#	define	CLPIRNG(C1,C2,S1,S2,W1,W2)	(C1=(((S1)>(W1))?(S1):(W1)),C2=(((S2)<(W2))?(S2):(W2)),(C2)>(C1))
#	define	SUB(A,B)					((A)-(B))
#	define	CLPCAL(X,X1,Y1,X2,Y2)		((Y1)+SUB(Y2,Y1)*SUB(X,X1)/SUB(X2,X1))
#	define	CLPCC(X,Y,XL,YL,XH,YH)		((((X)<(XL))<<0)+(((X)>(XH))<<1)+(((Y)<(YL))<<2)+(((Y)>(YH))<<3))

/************************************************************************/
/*	�����N���b�s���O													*/
/************************************************************************/

int		clpLin( int *x1p, int *y1p, int *x2p, int *y2p, int x1, int y1, int x2, int y2, int wxc, int wyc, int wxd, int wyd ){

	int		da[2][2];													// ���Z�p���_�z��
	int		cca[2];														// Clip Code Ary
	int		clpa[2][2];													// Clip area Ary
	int		wx2,wy2;
	int		ix,iy,lh,ccmsk;
	int		i,pno;

	clpa[0][0]=wxc;
	clpa[0][1]=wyc;
	clpa[1][0]=wx2=wxc+wxd-1;
	clpa[1][1]=wy2=wyc+wyd-1;
	da[0][0]=x1;
	da[0][1]=y1;
	da[1][0]=x2;
	da[1][1]=y2;
	for( i=0; i<2; i++ ){												// �����_�̏�Ԍv�Z
		cca[i]=CLPCC( da[i][0], da[i][1], wxc, wyc, wx2, wy2 );			// �R���f�B�V�����R�[�h�̌v�Z
	}
	if(	cca[0]&cca[1] ) return( 0 );									// ���S�͈͊O
	if( cca[0]|cca[1] ){												// �N���b�s���O�K�v
		for( i=0; i<4; i++ ){
			ccmsk=1<<i;													// �`�F�b�N�}�X�N
			if( (cca[0]|cca[1])&ccmsk ){								// �N���b�v���ׂ����E�ł���
				pno=!(cca[0]&ccmsk);									// �N���b�v���ׂ����_�ԍ�
				iy=( ( ix=i>>1 )==0 );									// ix=0,0,1,1, iy=1,1,0,0 �ƕω�
				lh=( i&1 );												// 0,1,0,1 �ƕω�
				da[pno][iy]=CLPCAL( clpa[lh][ix], da[0][ix], da[0][iy], da[1][ix], da[1][iy] );
				da[pno][ix]=clpa[lh][ix];
				cca[pno]=CLPCC( da[pno][0], da[pno][1], wxc, wyc, wx2, wy2 );	// �R���f�B�V�����R�[�h�̌v�Z
				if( cca[0]&cca[1] ) return( 0 );						// �N���b�v�̌��ʁA���S�͈͊O�ƂȂ���
			}
		}
	}
	*x1p=da[0][0];
	*y1p=da[0][1];
	*x2p=da[1][0];
	*y2p=da[1][1];
	return( 1 );														// ���S�͈͓��ƂȂ���
}

/************************************************************************/
/*	�P�����N���b�s���O													*/
/************************************************************************/

int		clp1( int *c1p, int *c2p, int s1, int s2, int w1, int w2 ){

	int		res;
	int		c1, c2;

	res=CLPIRNG( c1, c2, s1, s2, w1, w2 );
	if( c1p ){
		*c1p=c1;
		*c2p=c2;
	}
	return( res );
}

/************************************************************************/
/*	��`�N���b�s���O													*/
/************************************************************************/

int		clpBox( int *xcp, int *ycp, int *xdp, int *ydp, int xc, int yc, int xd, int yd, int wxc, int wyc, int wxd, int wyd ){

	int		cx1,cy1,cx2,cy2;

	if( CLPIRNG( cx1, cx2, xc, xc+xd, wxc, wxc+wxd ) ){
		if( CLPIRNG( cy1, cy2, yc, yc+yd, wyc, wyc+wyd ) ){
			if( xcp ){													// �T�C�Y�Ƒ��x�̖��Ő擪�݂̂̃`�F�b�N�Ƃ���
				*xcp=cx1;
				*ycp=cy1;
				*xdp=cx2-cx1;
				*ydp=cy2-cy1;
			}
			return( 1 );												// �͈͓�
		}
	}
	return( 0 );														// �͈͊O
}

/************************************************************************/
/*	���_�`�F�b�N														*/
/************************************************************************/

int		clpPos( int *xp, int *yp, int x, int y, int wxc, int wyc, int wxd, int wyd ){

	unsigned int		dx,dy;

	dx=x-wxc;
	dy=y-wyc;
	if( xp ){															// �T�C�Y�Ƒ��x�̖��Ő擪�݂̂̃`�F�b�N�Ƃ���
		*xp=dx;
		*yp=dy;
	}
	return( dx<( unsigned int )wxd && dy<( unsigned int )wyd );
}

