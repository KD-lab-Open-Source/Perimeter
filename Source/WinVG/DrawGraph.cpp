#include "StdAfx.h"
#include "DrawGraph.h"

extern cInterfaceRenderDevice* gb_IRenderDevice;
DrawGraph::DrawGraph()
{
	wxmin=0;
	wxmax=1;
	wymin=0;
	wymax=1;
	xmin=0;
	xmax=1;
	ymin=0;
	ymax=1;
}

void DrawGraph::SetWindow(float xmin_,float xmax_,float ymin_, float ymax_)
{
	wxmin=xmin_;
	wxmax=xmax_;
	wymin=ymin_;
	wymax=ymax_;
}

void DrawGraph::SetArgumentRange(float xmin_,float xmax_,float ymin_,float ymax_)
{
	xmin=xmin_;
	xmax=xmax_;
	ymin=ymin_;
	ymax=ymax_;
}

void DrawGraph::Draw(DrawFunctor& f)
{
	sColor4c color(255,255,255);
	float fxmin=gb_IRenderDevice->GetSizeX()*wxmin;
	float fxrange=gb_IRenderDevice->GetSizeX()*(wxmax-wxmin);
	int ixmin=round(fxmin);
	int ixmax=round(gb_IRenderDevice->GetSizeX()*wxmax);

	int xold=0,yold=0;
	for(int ix=ixmin;ix<=ixmax;ix++)
	{
		float x=(ix-fxmin)/fxrange;
		x=x*(xmax-xmin)+xmin;
		float y=f.get(x,&color);

		float fy=(y-ymin)/(ymax-ymin);
		int iy;
		fy=fy*(wymax-wymin)+wymin;
		iy=round((1-fy)*gb_IRenderDevice->GetSizeY());

		if(ix>ixmin)
			gb_IRenderDevice->DrawLine(xold,yold,ix,iy,color);

		xold=ix;
		yold=iy;
	}
}
