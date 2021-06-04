#pragma once
bool invertmatrix(Mat4f& b);
void MinQuadMatrix(Mat4f& out,int num);
void MinQuadMatrixSimply(Mat4f& out,int num);
void CalcLagrange(vector<float>& data,float& a0,float& a1,float& a2,float& a3);