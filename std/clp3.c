/************************************************************************/
/*																		*/
/*	クリッピング				by H.Watanabe							*/
/*	Ver1.00	2009/04/05 13:52:04	作成									*/
/*																		*/
/************************************************************************/

#	include	"clp3.h"													/* クリッピング*/

/************************************************************************/
/*	ローカル定義														*/
/************************************************************************/

#	define	CLPIRNG(C1,C2,S1,S2,W1,W2)	(C1=(((S1)>(W1))?(S1):(W1)),C2=(((S2)<(W2))?(S2):(W2)),(C2)>(C1))
#	define	SUB(A,B)					((A)-(B))
#	define	CLPCAL(X,X1,Y1,X2,Y2)		((Y1)+SUB(Y2,Y1)*SUB(X,X1)/SUB(X2,X1))
#	define	CLPCC(X,Y,XL,YL,XH,YH)		((((X)<(XL))<<0)+(((X)>(XH))<<1)+(((Y)<(YL))<<2)+(((Y)>(YH))<<3))

/************************************************************************/
/*	線分クリッピング													*/
/************************************************************************/

int		clpLin( int *x1p, int *y1p, int *x2p, int *y2p, int x1, int y1, int x2, int y2, int wxc, int wyc, int wxd, int wyd ){

	int		da[2][2];													// 演算用頂点配列
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
	for( i=0; i<2; i++ ){												// 両頂点の状態計算
		cca[i]=CLPCC( da[i][0], da[i][1], wxc, wyc, wx2, wy2 );			// コンディションコードの計算
	}
	if(	cca[0]&cca[1] ) return( 0 );									// 完全範囲外
	if( cca[0]|cca[1] ){												// クリッピング必要
		for( i=0; i<4; i++ ){
			ccmsk=1<<i;													// チェックマスク
			if( (cca[0]|cca[1])&ccmsk ){								// クリップすべき境界である
				pno=!(cca[0]&ccmsk);									// クリップすべき頂点番号
				iy=( ( ix=i>>1 )==0 );									// ix=0,0,1,1, iy=1,1,0,0 と変化
				lh=( i&1 );												// 0,1,0,1 と変化
				da[pno][iy]=CLPCAL( clpa[lh][ix], da[0][ix], da[0][iy], da[1][ix], da[1][iy] );
				da[pno][ix]=clpa[lh][ix];
				cca[pno]=CLPCC( da[pno][0], da[pno][1], wxc, wyc, wx2, wy2 );	// コンディションコードの計算
				if( cca[0]&cca[1] ) return( 0 );						// クリップの結果、完全範囲外となった
			}
		}
	}
	*x1p=da[0][0];
	*y1p=da[0][1];
	*x2p=da[1][0];
	*y2p=da[1][1];
	return( 1 );														// 完全範囲内となった
}

/************************************************************************/
/*	１次元クリッピング													*/
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
/*	矩形クリッピング													*/
/************************************************************************/

int		clpBox( int *xcp, int *ycp, int *xdp, int *ydp, int xc, int yc, int xd, int yd, int wxc, int wyc, int wxd, int wyd ){

	int		cx1,cy1,cx2,cy2;

	if( CLPIRNG( cx1, cx2, xc, xc+xd, wxc, wxc+wxd ) ){
		if( CLPIRNG( cy1, cy2, yc, yc+yd, wyc, wyc+wyd ) ){
			if( xcp ){													// サイズと速度の問題で先頭のみのチェックとする
				*xcp=cx1;
				*ycp=cy1;
				*xdp=cx2-cx1;
				*ydp=cy2-cy1;
			}
			return( 1 );												// 範囲内
		}
	}
	return( 0 );														// 範囲外
}

/************************************************************************/
/*	頂点チェック														*/
/************************************************************************/

int		clpPos( int *xp, int *yp, int x, int y, int wxc, int wyc, int wxd, int wyd ){

	unsigned int		dx,dy;

	dx=x-wxc;
	dy=y-wyc;
	if( xp ){															// サイズと速度の問題で先頭のみのチェックとする
		*xp=dx;
		*yp=dy;
	}
	return( dx<( unsigned int )wxd && dy<( unsigned int )wyd );
}

