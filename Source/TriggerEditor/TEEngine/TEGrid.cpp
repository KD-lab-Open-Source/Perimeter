// TEGrid.cpp: implementation of the TEGrid class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "te_helpers.h"
#include "TEGrid.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TEGrid::TEGrid():
  iCellWidth_(0)
, iCellHeight_(0)
, iAmongCellsW_(0)
, iAmongCellsH_(0)
{

}
TEGrid::TEGrid(int cellWidth, int cellHeight, int amongCellWidth, int amongCellHeight):
  iCellWidth_(cellWidth)
, iCellHeight_(cellHeight)
, iAmongCellsW_(amongCellWidth)
, iAmongCellsH_(amongCellHeight)

{

}

TEGrid::~TEGrid()
{

}

int TEGrid::getCellWidth() const{
	return iCellWidth_;
}

int TEGrid::getCellHeight() const{
	return iCellHeight_;
}

void TEGrid::setCellWidth(int value){
	iCellWidth_ = value;
}

void TEGrid::setCellHeight(int value){
	iCellHeight_ = value;
}

int TEGrid::getAmongCellsW() const{
	return iAmongCellsW_;
}

int TEGrid::getAmongCellsH() const{
	return iAmongCellsH_;
}

void TEGrid::setAmongCellsW(int value){
	iAmongCellsW_ = value;
}

void TEGrid::setAmongCellsH(int value){
	iAmongCellsH_ = value;
}

int TEGrid::toCellEdge(int x, int cell_sz, int among_cells_sz)
{
	int const len = cell_sz + among_cells_sz;
	if (x >= 0)
		return x/len * len; 
	else
		return (x/len - 1)*len;
}

int TEGrid::getCellIndex(int x, int cell_sz)
{
	if (x < -1)
		x += 1;
	int const res = x/cell_sz;
	if (x < 0)
		return res - 1;
	return res;
}

int TEGrid::indexToCellEdge(int idx, int cell_sz){
	return idx* cell_sz;
}

int TEGrid::toCellLeft(int x) const{
	return toCellEdge(x, iCellWidth_, iAmongCellsW_);
}

int TEGrid::toCellTop(int y) const{
	return toCellEdge(y, iCellHeight_, iAmongCellsH_);
}

void TEGrid::toCellLeftTop(sPoint* point) const
{
	point->x = toCellLeft(point->x);
	point->y = toCellTop(point->y);
}

CPoint const TEGrid::getCellByPoint(sPoint const& p) const{
	CPoint const sz(getHorzCellIndex(p.x), getVertCellIndex(p.y));
	return sz;
}

int TEGrid::getVertCellIndex(int y) const{
	return getCellIndex(y, iCellHeight_+iAmongCellsH_);
}

int TEGrid::getHorzCellIndex(int x) const{
	return getCellIndex(x, iCellWidth_+iAmongCellsW_);
}

int TEGrid::getCellTopByIndex(int idx) const{
	return indexToCellEdge(idx, iCellHeight_+iAmongCellsH_);
}

int TEGrid::getCellLeftByIndex(int idx) const{
	return indexToCellEdge(idx, iCellWidth_+iAmongCellsW_);
}

int TEGrid::getFullCellWidth() const{
	return getCellWidth() + getAmongCellsW();
}

int TEGrid::getFullCellHeight() const{
	return getCellHeight() + getAmongCellsH();
}

