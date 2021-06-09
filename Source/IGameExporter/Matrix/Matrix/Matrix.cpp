#include "stdafx.h"
#include "../../InvertMatrix.h"

int main(int argc, char* argv[])
{
/*
	Mat4f mat;
    mat[0][0]=16;
	mat[0][1]=-120;
	mat[0][2]=240;
	mat[0][3]=-140;

    mat[1][0]=-120;
	mat[1][1]=1200;
	mat[1][2]=-2700;
	mat[1][3]=1680;

    mat[2][0]=240;
	mat[2][1]=-2700;
	mat[2][2]=6480;
	mat[2][3]=-4200;

    mat[3][0]=-140;
	mat[3][1]=1680;
	mat[3][2]=-4200;
	mat[3][3]=2800;

	Mat4f inv=mat;
	invertmatrix(inv);

	Mat4f ID=inv*mat;

	Mat4f out,out_simply,out_delta;
	MinQuadMatrix(out,12);
	MinQuadMatrixSimply(out_simply,12);

	for(int i=0;i<4;i++)
	for(int j=0;j<4;j++)
		out_delta[i][j]=out[i][j]-out_simply[i][j];
*/

	float a0,a1,a2,a3;
	vector<float> data;
	data.push_back(1+1);
	data.push_back(8+4);
	data.push_back(27+9);
	data.push_back(64+16);
//	data.push_back(25);
//	data.push_back(36);
	CalcLagrange(data,a0,a1,a2,a3);

	for(int i=0;i<data.size();i++)
	{
		float t=i/float(data.size()-1)-0.5f;
		float f=a0+a1*t+a2*t*t+a3*t*t*t;
		int k=0;
	}

	return 0;
}

