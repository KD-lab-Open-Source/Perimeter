
#include "stdafx.h"
#include "terra.h"

const int DeltaAlt = 20;

inline void _PutAlt(int x, int y, int da)
{
	vMap.voxSet(vMap.XCYCL(x), vMap.YCYCL(y), -da);
}


RECT& geoLine(int xbeg, int ybeg, int sx, int sy, int width, RECT& r)
{
	int x_cur=xbeg<<16;
	int y_cur=ybeg<<16;

	if(abs(sx)>=abs(sy)){
		int stepdy=sy*(1<<16)/abs(sx);
		int stepdx;
		if(sx>0)stepdx=1; else stepdx=-1;
		for(int i=0; i!=sx; i+=stepdx, x_cur+=stepdx<<16, y_cur+=stepdy)
		{
			_PutAlt(x_cur>>16, y_cur>>16, DeltaAlt);
			//vMap.voxSet(vMap.XCYCL(x_cur>>16), vMap.YCYCL(y_cur>>16), -6);

			for(int k=1; k<width; k++){
				_PutAlt(x_cur>>16, (y_cur>>16)-k, DeltaAlt);
				_PutAlt(x_cur>>16, (y_cur>>16)+k, DeltaAlt);
				//vMap.voxSet(vMap.XCYCL(x_cur>>16), vMap.YCYCL((y_cur>>16)-k), -6);
				//vMap.voxSet(vMap.XCYCL(x_cur>>16), vMap.YCYCL((y_cur>>16)+k), -6);
			}
		}
	}
	else { // sy_real> sx_real
		if(abs(sy)>abs(sx)){
			int stepdx=sx*(1<<16)/abs(sy);
			int stepdy;
			if(sy>0)stepdy=1; else stepdy=-1;
			for(int i=0; i!=sy; i+=stepdy, y_cur+=stepdy<<16, x_cur+=stepdx)
			{
				_PutAlt(x_cur>>16, y_cur>>16, DeltaAlt);
				//vMap.voxSet(vMap.XCYCL(x_cur>>16), vMap.YCYCL(y_cur>>16), -6);

				for(int k=1; k<width; k++){
					_PutAlt((x_cur>>16)-k, y_cur>>16, DeltaAlt);
					_PutAlt((x_cur>>16)+k, y_cur>>16, DeltaAlt);
					//vMap.voxSet(vMap.XCYCL((x_cur>>16)-k), vMap.YCYCL(y_cur>>16), -6);
					//vMap.voxSet(vMap.XCYCL((x_cur>>16)+k), vMap.YCYCL(y_cur>>16), -6);
				}
			}
		}
	}

	int xL, yT, xR, yD;
	if(sx>0) { xL=vMap.XCYCL(xbeg-width); xR=vMap.XCYCL(xbeg+sx+width); }
	else{ xL=vMap.XCYCL(xbeg+sx-width); xR=vMap.XCYCL(xbeg+width); }
	if(sy>0) { yT=vMap.YCYCL(ybeg-width); yD=vMap.YCYCL(ybeg+sy+width); }
	else { yT=vMap.YCYCL(ybeg+sy-width); yD=vMap.YCYCL(ybeg+width); }
	vMap.recalcArea2Grid(xL, yT, xR, yD );
	vMap.regRender(xL, yT, xR, yD);
	r.left = xL; r.right = xR;
	r.top = yT; r.bottom = yD;

	return r;
};

RECT& LineAA(int Xd, int Yd, int Xf, int Yf, RECT& r)
{
  int dX, dY;
  int Xinc, Yinc;
  int X, Y;
  int DeltaX, DeltaY;
  int i;
  int D;
  int Xs, Ys;
  double Cp;
  int I_p;//яркость вспомогательной точки
  int I_s;//яркость основной точки

  int Imax = DeltaAlt;

  dX = Xf - Xd; dY = Yf - Yd;
  DeltaX = abs(dX); DeltaY = abs(dY);
  if (dX > 0) Xinc = +1; else Xinc = -1;
  if (dY > 0) Yinc = +1; else Yinc = -1;

  X = Xd; Y = Yd;
  _PutAlt(X, Y, Imax);
  if (DeltaX >= DeltaY)
  {
    D = 0;
    for (i = 1; i<DeltaX; i++)
	{
      X += Xinc;
      D += DeltaY;
      if (D <= 0)
        Ys = Y - Yinc;
      else
	  {
        if (D - DeltaX < 0)
		{
          if (2 * D - DeltaX <= 0)
            Ys = Y + Yinc;
          else
		  {
            Ys = Y;
            Y += Yinc;
            D -= DeltaX;
		  }
		}
        else
		{
			Y+= Yinc;
			Ys = Y + Yinc;
			D -= DeltaX;
		}
	  }
      Cp = abs( D / DeltaX );
      I_s = round(Cp * Imax);
      I_p = Imax - I_s;
      _PutAlt(X, Y, I_p);
      _PutAlt(X, Ys, I_s);
	}
  }
  else
  {
    D = 0;
    for(i = 1; i<DeltaY; i++)
	{
      Y+= Yinc;
      D+= DeltaX;
      if (D <= 0)
        Xs = X - Xinc;
      else
	  {
        if (D - DeltaY < 0)
		{
          if (2 * D - DeltaY <= 0)
            Xs = X + Xinc;
          else
		  {
            Xs = X;
            X+=Xinc;
            D-= DeltaY;
		  }
		}
        else
		{
          X+=Xinc;
          Xs = X + Xinc;
          D-= DeltaY;
		}
	  }
      Cp = abs(D / DeltaY);
      I_s = round(Cp * Imax);
      I_p = Imax - I_s;
      _PutAlt(X, Y, I_p);
      _PutAlt(Xs, Y, I_s);
	}
  }

	int xL, yT, xR, yD;
	if(Xd < Xf) { xL=vMap.XCYCL(Xd); xR=vMap.XCYCL(Xf); }
	else{ xL=vMap.XCYCL(Xf); xR=vMap.XCYCL(Xd); }
	if(Yd < Yf) { yT=vMap.YCYCL(Yd); yD=vMap.YCYCL(Yf); }
	else { yT=vMap.YCYCL(Yf); yD=vMap.YCYCL(Yd); }
	vMap.recalcArea2Grid(xL, yT, xR, yD );
	vMap.regRender(xL, yT, xR, yD);
	r.left = xL; r.right = xR;
	r.top = yT; r.bottom = yD;

	return r;
}


void V_Bre (int xn, int yn, int xk, int yk)
{  
	int  dx, dy, s, sx, sy, kl, swap, incr1, incr2;

	int Xd = xn, Xf = xk;
	int Yd = yn, Yf = yk;
/* Вычисление приращений и шагов */
   sx= 0;
   if ((dx= xk-xn) < 0) {dx= -dx; --sx;} else if (dx>0) ++sx;
   sy= 0;
   if ((dy= yk-yn) < 0) {dy= -dy; --sy;} else if (dy>0) ++sy;
/* Учет наклона */
   swap= 0;
   if ((kl= dx) < (s= dy)) {
      dx= s;  dy= kl;  kl= s; ++swap;
   }
   s= (incr1= 2*dy)-dx; /* incr1 - констан. перевычисления */
                        /* разности если текущее s < 0  и  */
                        /* s - начальное значение разности */
   incr2= 2*dx;         /* Константа для перевычисления    */
                        /* разности если текущее s >= 0    */
   _PutAlt(xn,yn,DeltaAlt); /* Первый  пиксел вектора       */
   while (--kl >= 0) {
      if (s >= 0) {
         if (swap) xn+= sx; else yn+= sy;
         s-= incr2;
      }
      if (swap) yn+= sy; else xn+= sx;
      s+=  incr1;
      _PutAlt(xn,yn,DeltaAlt);/* Текущая  точка  вектора   */
   }

	int xL, yT, xR, yD;
	if(Xd < Xf) { xL=vMap.XCYCL(Xd); xR=vMap.XCYCL(Xf); }
	else{ xL=vMap.XCYCL(Xf); xR=vMap.XCYCL(Xd); }
	if(Yd < Yf) { yT=vMap.YCYCL(Yd); yD=vMap.YCYCL(Yf); }
	else { yT=vMap.YCYCL(Yf); yD=vMap.YCYCL(Yd); }
	vMap.recalcArea2Grid(xL, yT, xR, yD );
	vMap.regRender(xL, yT, xR, yD);
}
