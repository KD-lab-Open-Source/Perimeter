#include "StdAfx.h"
#include "InvertMatrix.h"

/*
bool invertmatrix(Mat4f& b)
{
	float epsilon=1e-5f;
	int n=4;
    int j;
    Mat4f a=b;
	Mat4f out;
	for(int i=1;i<=n;i++)
	{
		for(int j=1;j<=n;j++)
			out(i,j)=0;
		out(i,i)=1;
	}

	//a(i,j) i-столец, j- строка

    for(i=1;i<=n;i++)
    {
        int k = i;
		float fkmax=a(k,i);
		int kmax=k;

        for(;k<=n;k++)
        {
            if( fabs(a(k,i))>fkmax )
            {
				fkmax=fabs(a(k,i));
				kmax=k;
            }
        }
		k=kmax;
		if(!(fabs(fkmax)>epsilon))
			return false;

        if( k!=i )
        {
            for(j=1;j<=n;j++)
            {
                swap(a(k,j),a(i,j));
				swap(out(k,j),out(i,j));
            }
        }

		for(j = 1;j<=n;j++)
		{
			out(i,j)/=a(i,i);
		}

		for(j = n;j>=i;j--)
        {
            a(i,j) = a(i,j)/a(i,i);
        }

        for(k=1;k<=n;k++)
        {
            if( k!=i )
            {
                for(j=1;j<=n;j++)
                {
                    out(k,j) = out(k,j)-out(i,j)*a(k,i);
                }

                for(j=n;j>=i;j--)
                {
                    a(k,j) = a(k,j)-a(i,j)*a(k,i);
                }
            }
        }
    }

	b=out;
    return true;
}
*/

bool invertmatrix(Mat4f& b)
{
	float epsilon=1e-5f;
	int n=4;
    int j;
    Mat4f a=b;
	Mat4f out;
	for(int i=0;i<n;i++)
	{
		for(int j=0;j<n;j++)
			out[i][j]=0;
		out[i][i]=1;
	}

	//a(i,j) i-столец, j- строка

    for(i=0;i<n;i++)
    {
        int k = i;
		float fkmax=a[k][i];
		int kmax=k;

        for(;k<n;k++)
        {
            if( fabs(a[k][i])>fkmax )
            {
				fkmax=fabs(a[k][i]);
				kmax=k;
            }
        }
		k=kmax;
		if(!(fabs(fkmax)>epsilon))
			return false;

        if( k!=i )
        {
            for(j=0;j<n;j++)
            {
                swap(a[k][j],a[i][j]);
				swap(out[k][j],out[i][j]);
            }
        }

		float inv_aii=1/a[i][i];
		for(j = 0;j<n;j++)
		{
			out[i][j]*=inv_aii;
		}

		for(j = n-1;j>=i;j--)
        {
            a[i][j] = a[i][j]*inv_aii;
        }

        for(k=0;k<n;k++)
        {
            if( k!=i )
            {
                for(j=0;j<n;j++)
                {
                    out[k][j] = out[k][j]-out[i][j]*a[k][i];
                }

                for(j=n-1;j>=i;j--)
                {
                    a[k][j] = a[k][j]-a[i][j]*a[k][i];
                }
            }
        }
    }

	b=out;
    return true;
}

//Матрица для среднеквадратического приближения для функции
//y=a0+a1*x+a2*x*x+a3*x*x*x на интервале -0.5..+0.5
void MinQuadMatrix(Mat4f& out,int num)
{
	int n=4;
	for(int i=0;i<n;i++)
	{
		for(int j=0;j<n;j++)
		{
			int kmax=i+j;

			float sum=0;
			for(int r=0;r<num;r++)
			{
				float f=r/float(num-1)-0.5f;
				float power=1;
				for(int k=0;k<kmax;k++)
					power*=f;
				sum+=power;
			}

			float acur=sum/num;
			out[i][j]=acur;
		}
	}

	invertmatrix(out);
}

void MinQuadMatrixSimply(Mat4f& out,int num)
{
//Используется тот факт, что у нечётных степеней сумма нулевая
//разбивается на 2 матрицы
	Mat2f m[2];
//m[0]
//00,02
//20,22
//m[1]
//11,13
//31,33
	for(int imat=0;imat<2;imat++)
	{
		for(int i=0;i<2;i++)
		{
			for(int j=0;j<2;j++)
			{
				int kmax=(i+j+imat)*2;

				float sum=0;
				for(int r=0;r<num;r++)
				{
					float f=r/float(num-1)-0.5f;
					float power=1;
					for(int k=0;k<kmax;k++)
						power*=f;
					sum+=power;
				}

				float acur=sum/num;
				m[imat][i][j]=acur;
			}
		}

		m[imat].Invert();
	}
	out[0][0]=m[0][0][0]; out[0][1]=0;
	out[0][2]=m[0][0][1]; out[0][3]=0;
	out[1][0]=0; out[1][1]=m[1][0][0];
	out[1][2]=0; out[1][3]=m[1][0][1];
	out[2][0]=m[0][1][0]; out[2][1]=0;
	out[2][2]=m[0][1][1]; out[2][3]=0;
	out[3][0]=0; out[3][1]=m[1][1][0];
	out[3][2]=0; out[3][3]=m[1][1][1];
}


void CalcLagrange(vector<float>& data,float& a0,float& a1,float& a2,float& a3)
{
	int size=data.size();
	Vect4f c;
	Vect4f func(0,0,0,0);
	Mat4f mat;
	MinQuadMatrixSimply(mat,size);

	float len=(float)(size-1);
	for(int i=0;i<size;i++)
	{
		float t=i/float(size-1)-0.5f;
		float f=data[i];
		func[0]+=f;
		func[1]+=f*t;
		func[2]+=f*t*t;
		func[3]+=f*t*t*t;
	}

	func[0]/=size;
	func[1]/=size;
	func[2]/=size;
	func[3]/=size;
	c=mat*func;

	a0=c[0];
	a1=c[1];
	a2=c[2];
	a3=c[3];
}
