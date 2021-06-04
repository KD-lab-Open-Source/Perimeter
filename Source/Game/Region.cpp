#include "stdafx.h"
#include "Region.h"
#include "PrmEdit.h"
#include "ForceField.h"
#include "Config.h"
#include "Terra.h"

//////////////////////////////////////////////////////////////////////////////////
//			Region
//////////////////////////////////////////////////////////////////////////////////
void Shape::circle(int radius)
{
	y0 = -radius;
	clear();
	reserve(radius*2 + 1);
	for(int y = -radius; y <= radius; y++){
		int x = round(sqrtf(sqr(radius) - sqr(y)));
		push_back(Interval(-x, 1 + x));
		}
}

void Shape::square(int sx, int sy)
{
	y0 = -sy;
	clear();
	reserve(sy*2 + 1);
	for(int y = -sy; y <= sy; y++)
		push_back(Interval(-sx, sx));
}


void Shape::move(const Vect2i& delta)
{
	y0 += delta.y;
	iterator i;
	FOR_EACH(*this, i){
		i->xl += delta.x;
		i->xr += delta.x;
		}
}

//////////////////////////////////////////////////////////////////////////////////
//			CellLine
//////////////////////////////////////////////////////////////////////////////////
Cell* Cell::cw(bool& by_left) 
{ 
	if(by_left) 
		if(l_cw != this){
			if(l_cw->y == y)
				by_left = false; 
			xassert(l_cw->y <= y);
			return l_cw; 
			}
		else{
			by_left = false; 
			return this; 
			}
	else  
		if(r_cw != this){
			if(r_cw->y == y)
				by_left = true; 
			xassert(r_cw->y >= y);
			return r_cw;  
			}
		else{
			by_left = true; 
			return this; 
			} 
}

void Cell::show(sColor4c color) const
{
	show_line(Vect3f(xl, y, vMap.hZeroPlast), Vect3f(xr, y, vMap.hZeroPlast), color);
//	for(int x = xl; x <= xr; x++)
//		show_vector(Vect3f(x, y, vMap.hZeroPlast), vMap.leveled(vMap.offsetBuf(x, y)) ? color : RED);

//	if(l_cw){
//		Cell& c = *l_cw;
//		if(y != c.y)
//			show_line(Vect3f(xl, y, 0), Vect3f(xl, c.y, 0), YELLOW);
//		else
//			show_line(Vect3f(xl, y - 0.25, 0), Vect3f(c.xr, c.y - 0.25, 0), YELLOW);
//	}
//	if(r_cw){
//		Cell& c = *r_cw;
//		if(y != c.y)
//			show_line(Vect3f(xr, y, 0), Vect3f(xr, c.y, 0), RED);
//		else
//			show_line(Vect3f(xr, y + 0.25, 0), Vect3f(c.xl, c.y + 0.25, 0), RED);
//	}
}

//////////////////////////////////////////////////////////////////////////////////
//			CellLine
//////////////////////////////////////////////////////////////////////////////////
CellLine& CellLine::operator=(const CellLine& line)
{
	int areaInitial = area();

	static_cast<List&>(*this) = static_cast<const List&>(line);
	
	int delta = area() - areaInitial;
	if(delta)
		setChanged(delta);

	return *this;
}

void CellLine::find_interval(const Interval& in, iterator& il, iterator& ir_)
{
	// find [il, ir_) of Intervals wich intersects in
	FOR_EACH(*this, il)
		if(il->xr >= in.xl)
			break;

	for(ir_ = il; ir_ != end(); ++ir_)
		if(ir_->xl > in.xr)
			break;
}

void CellLine::add(const Interval& in, Region* region)
{
	if(!empty()){
		iterator il, ir;
		find_interval(in, il, ir);
		if(il == ir){
			setChanged(in.delta());
			ir = insert(ir, Cell(in, y));
			ir->l_region = ir->r_region = region;
			}
		else{
			--ir;
			int delta = 0;
			int xl = il->xl;
			if(xl > in.xl){
				delta += xl - in.xl;
				xl = in.xl;
			}
			int xr = ir->xr;
			if(xr < in.xr){
				delta += in.xr - xr;
				xr = in.xr;
			}
			while(il != ir){
				int x = il->xr;
				il = erase(il);
				delta += il->xl - x - 1;
			}
			ir->xl = xl;
			ir->xr = xr;
			ir->l_region = ir->r_region = region;
			if(delta){
				xassert(delta > 0);
				setChanged(delta);
			}
		}
	}
	else{
		setChanged(in.delta());
		push_back(Cell(in, y));
		back().l_region = back().r_region = region;
		}
}

void CellLine::sub(const Interval& in, Region* region)
{
	if(!empty()){
		iterator il, ir;
		find_interval(in, il, ir);
		if(il == ir)
			return;
		
		--ir;
		int areaInitial = area();
		//int delta = 0; // negative
		if(il == ir){
			if(il->xl < in.xl){
				//delta += 0;
				il = insert(il, Cell(Interval(il->xl, in.xl - 1), y));
				il->l_region = ir->l_region;
				il->r_region = region;
				}
			} 
		else{
			iterator i = il;
			++i;
			while(i != ir){
				//delta -= i->delta();
				i = erase(i);
			}
			if(il->xl < in.xl){
				//delta += (in.xl - 1) - il->xr;
				il->xr = in.xl - 1;
				il->r_region = region;
				}
			else{
				//delta -= il->delta();
				erase(il);
			}
		}

		if(ir->xr > in.xr){
			//delta += ir->xl - (in.xr + 1);
			ir->xl = in.xr + 1;
			ir->l_region = region;
			}
		else{
			//delta -= ir->delta();
			erase(ir);
		}

		int delta = area() - areaInitial;
		if(delta){
			xassert(delta < 0);
			setChanged(delta);
		}
	}
}

void CellLine::intersect(const CellLine& line)
{
	// C = A - (A - B)
	if(!empty()){
		CellLine delta = *this;
		const_iterator i;
		FOR_EACH(line, i)
			delta.sub(*i);
		FOR_EACH(delta, i)
			sub(*i);
	}
}

void CellLine::intersect(const CellLine& lineA, const CellLine& lineB)
{
	// C = A - (A - B)

	*this = lineA;
	intersect(lineB);

	/*
	CellLine lineA_B = lineA;
	const_iterator i;
	FOR_EACH(lineB, i)
		lineA_B.sub(*i);

	// C -= (A - B)
	FOR_EACH(lineA_B, i)
		sub(*i);

	CellLine lineA_A_B = lineA;
	FOR_EACH(lineA_B, i)
		lineA_A_B.sub(*i);

	// C += A - (A - B)
	FOR_EACH(lineA_A_B, i)
		add(*i);
	*/

	setChanged(0);
}


void CellLine::analyze(CellLine& line1, CellLine& line2, SeedList& seeds)
{
	// Set l_cw for line2
	iterator i1 = line1.begin();
	iterator i2 = line2.begin();
	Cell* target = 0;
	bool target_flag = false;
	while(i2 != line2.end()){
		if(i1 == line1.end()){				   //	 --i1--
			do {							   //	         --i2--  ----
				i2->l_cw = &*i2;			   
				seeds.push_back(&*i2);	// начало положительного региона 			  
			} while(++i2 != line2.end());
			break;
		}
		else if(i1->xr < i2->xl){			  //  --i1-- ----
			++i1;							  //	  	      --i2--
			target = 0;
			target_flag = false;
		}
		else if(i2->xr < i1->xl){ 			  //		 --i1--
			i2->l_cw = &*i2;				  //  --i2--
			seeds.push_back(&*i2);	// начало положительного региона 			  
			++i2;				  
			target = 0;
			target_flag = false;
		}
		else{
			if(!target)
				target = &*i1;
			i2->l_cw = target;
			target = &*i2;
			if(target_flag)
				seeds.push_back(&*i2); // начало отрицательного региона (дырки)	   ---- hole --hanle--
			target_flag = true;

			while(i1 != line1.end() && i1->xr < i2->xr)		 //  --i1-- ---- --x---
				++i1;										 //   ------i2------

			if(i1 != line1.end() && i2->xr < i1->xl){	   //		    --i1--
				target = 0;								   //	--i2--
				target_flag = false;
			}
			++i2;
		}
	}


	// Set r_cw for line1
	reverse_iterator i1r = line1.rbegin();
	reverse_iterator i2r = line2.rbegin();
	target = 0;
	while(i1r != line1.rend()){
		if(i2r == line2.rend()){			   //	 ----  --i1r--
			do								   //	         	    --i2r--
				i1r->r_cw = &*i1r;			   
				while(++i1r != line1.rend());
			break;
		}
		else if(i2r->xl > i1r->xr){			  //    --i1r--             
			++i2r;							  //	         ---- --i2r--  	      
			target = 0;
		}
		else if(i1r->xl > i2r->xr){ 		  //    		 --i1r--
			i1r->r_cw = &*i1r;				  //  	--i2r--		  
			++i1r;
			target = 0;
		}
		else{
			if(!target)
				target = &*i2r;
			i1r->r_cw = target;
			target = &*i1r;

			while(i2r != line2.rend() && i2r->xl > i1r->xl)	 //   ------i1r------
				++i2r;										 //  --x-- ---- --i2r---

			if(i2r != line2.rend() && i1r->xl > i2r->xr)   //			--i1r--
				target = 0;								   //	--i2r--
			++i1r;
		}
	}
}

Region* CellLine::locate(int x) const 
{ 
	const_iterator i; 
	FOR_EACH(*this, i) 
		if(i->xl <= x && i->xr >= x){ 
			if(i->l_region->positive())
				return i->l_region;
			else if(i->r_region->positive())
				return i->r_region;
			else{
				for(++i; i != end(); ++i)
					if(i->r_region->positive())
						return i->r_region;
				xassert(0);
			}
		} 
	return 0; 
}

void CellLine::check()
{
	iterator i0, i1;
	for(i0 = i1 = begin(), ++i1; i1 != end(); i0 = i1, ++i1)
		xassert(i0->xr < i1->xl);

	iterator i;
	FOR_EACH(*this, i)
		xassert((i->l_cw && i->r_cw || !i->l_cw && !i->r_cw) && i->l_region && i->r_region);
}

void CellLine::checkAnalyzing()
{
	iterator i;
	FOR_EACH(*this, i)
		xassert(i->l_cw && i->r_cw);
}

void CellLine::show(sColor4c color) const 
{
	const_iterator i;
	FOR_EACH(*this, i)
		i->show(color);
		//i->show(changed() ? RED : CYAN);
}

//////////////////////////////////////////////////////////////////////////////////
//		Column
//////////////////////////////////////////////////////////////////////////////////
Column::Column(int sy)
: vector<CellLine>(sy)
{
	int y = 0; 
	iterator i; 
	FOR_EACH(*this, i){
		i->y = y++;
		i->column_ = this;
	}
	area_ = 0;
	changeCounter_ = lastChangeCounter_ = 0;
}

void Column::clear()
{
	iterator i; 
	FOR_EACH(*this, i)
		if(!i->empty()){
			i->clear();
			setChanged(0);
		}
	area_ = 0;
	changeCounter_ = lastChangeCounter_ = 0;
}

void Column::add(const Column& column)
{
	xassert(size() == column.size() && "size should be equal");

	const_iterator iSrc = column.begin();
	iterator iDest;
	FOR_EACH(*this, iDest){
		CellLine::const_iterator ci;
		FOR_EACH(*iSrc, ci)
			iDest->add(*ci);
		++iSrc;
	}
}

void Column::sub(const Column& column)
{
	xassert(size() == column.size() && "size should be equal");

	const_iterator iSrc = column.begin();
	iterator iDest;
	FOR_EACH(*this, iDest){
		CellLine::const_iterator ci;
		FOR_EACH(*iSrc, ci)
			iDest->sub(*ci);
		++iSrc;
	}
}

void Column::intersect(const Column& columnA, const Column& columnB)
{
	xassert(size() == columnA.size() && size() == columnB.size() && "size should be equal");

	iterator iDest;
	const_iterator iA = columnA.begin();
	const_iterator iB = columnB.begin();
	FOR_EACH(*this, iDest){
		if(iA->changed() || iB->changed())
			iDest->intersect(*iA, *iB);
		++iA;
		++iB;
	}
}			

void Column::addCircleThenSub(const Vect2i& pos, float radius, const ColumnVect& columns)
{
	Shape shape;
	shape.circle(radius);
	shape.move(pos);

	int yt = 0;
	int y0 = shape.y0;
	if(y0 < 0){
		yt -= y0;
		y0 = 0;
	}
	int y1 = shape.y0 + shape.size();
	if(y1 > size())
		y1 = size();
	
	while(y0 < y1){
		CellLine& line = (*this)[y0];
		line.add(shape[yt++]);

		ColumnVect::const_iterator iSrc;
		FOR_EACH(columns, iSrc){
			CellLine::const_iterator ci;
			FOR_EACH((**iSrc)[y0], ci)
				line.sub(*ci);
		}

		++y0;
	}
}			

void Column::intersect(const Column& column)
{
	xassert(size() == column.size() && "size should be equal");

	iterator iDest = begin();
	const_iterator iSrc;
	FOR_EACH(column, iSrc){
		iDest->intersect(*iSrc);
		++iDest;
	}
			   
//	// C = A - (A - B)
//	Column delta = *this;
//	delta.sub(column);
//	sub(delta);
}			

void Column::operate(const Shape& shape, int add)
{
	start_timer_auto(Region_operate, STATISTICS_GROUP_AI);
	int yt = 0;
	int y0 = shape.y0;
	if(y0 < 0){
		yt -= y0;
		y0 = 0;
	}
	int y1 = shape.y0 + shape.size();
	if(y1 > size())
		y1 = size();
	
	if(add)
		while(y0 < y1)
			(*this)[y0++].add(shape[yt++]);
	else
		while(y0 < y1)
			(*this)[y0++].sub(shape[yt++]);
}

void Column::operateByCircle(const Vect2i& pos, float radius, int add)
{
	Shape circle;
	circle.circle(radius);
	circle.move(pos);
	operate(circle, add);
}

void Column::operateByCycledHermite(CycledHermite& hermite, int add)
{
	Vect2sVect border;
	hermite.getBorder(border);
	
	Vect2sVect::iterator i;
	FOR_EACH(border, i){
		Vect2sVect::iterator i1 = i + 1 != border.end() ? i + 1 : border.begin();
		if(i1->y < i->y){ // Скачек вверх 
			Vect2sVect::iterator k;
			Vect2sVect::iterator k_best = border.end();
			Vect2sVect::iterator k0 = border.end() - 1;
			int d, dist = 0x7fffffff;
			FOR_EACH(border, k){ // Находим ближайший скачек вниз справа
				if(i->y == k->y && k0->y < k->y && (d = k->x - i->x) > 0 && d < dist){
					k_best = k;
					dist = d;
				}
				k0 = k;
			}

			//xassert(k_best != border.end());

			if(k_best == border.end()) // могут теряться 1-интервалы
				continue;
			
			int y = i->y;
			if(y >= 0 && y < size())
				if(add)
					(*this)[y].add(Interval(i->x, k_best->x));
				else
					(*this)[y].sub(Interval(i->x, k_best->x));
		}
	}
}

void Column::check()
{
	iterator i;
	FOR_EACH(*this, i)
		i->check();
}

void Column::show(sColor4c color) const
{
	const_iterator i;
	FOR_EACH(*this, i)
		i->show(color);
}

int Column::intersected(const Shape& shape)
{
	int yt = 0;
	int y0 = shape.y0;
	if(y0 < 0){
		yt -= y0;
		y0 = 0;
		}
	int y1 = shape.y0 + shape.size();
	if(y1 > (int)size())
		y1 = size();

	while(y0 < y1)
		if((*this)[y0++].intersected(shape[yt++]))
			return 1;
	return 0;
}

void Column::expandFilledBound(Vect2i& leftTop, Vect2i& rightBottom) const
{
	const_iterator ci;
	CellLine::const_iterator li;
	FOR_EACH(*this, ci)
		FOR_EACH(*ci, li){
			if(leftTop.x > li->xl)
				leftTop.x = li->xl;
			if(rightBottom.x < li->xr)
				rightBottom.x = li->xr;
			if(leftTop.y > li->y)
				leftTop.y = li->y;
			if(rightBottom.y < li->y)
				rightBottom.y = li->y;
		}
}


//////////////////////////////////////////////////////////////////////////////////
//			Region
//////////////////////////////////////////////////////////////////////////////////
int Region::IDs;
int Region::num_entities;
Region::Region()
{ 
	parent = 0; 
	ID_ = ++IDs;
	num_entities++; 
	numCells_ = 0; 
	positive_ = false; 
	handle_ = 0; 
	handlePosition_ = Vect2i::ZERO;
	y0 = y1 = 0;
	splineInited_ = false;
}

Region::~Region() 
{ 
	num_entities--; 
}

void Region::set(Cell* handle)
{
	handle_ = handle;
	y0 = y1 = handle_->y;
	handlePosition_.set(handle_->xl, handle_->y);

	Cell* cell = handle_;
	bool by_left0 = true;
	bool by_left = by_left0;
	int yPrev = cell->y;
	int area = 0;
	do {
		if(by_left){
//			xassert(!cell->l_region);
			cell->l_region = this;
			}
		else{
//			xassert(!cell->r_region);
			cell->r_region = this;
			}
		numCells_++;

		if(y1 < cell->y)
			y1 = cell->y;

		int x = by_left ? cell->xl : cell->xr;
		if(cell->y - yPrev == 1)
			area += x;
		else if(cell->y - yPrev == -1)
			area -= x;
		yPrev = cell->y;

		cell = cell->cw(by_left);
		} while(cell != handle_ || by_left != by_left0);

	positive_ = area > 0;
}

bool Region::initSpline()
{
	xassert(handle_);

	Cell* cell = handle_;
	bool by_left0 = true;
	bool by_left = by_left0;
	Point p0 = cell->point(by_left);
	float perimeter = 0;
	do {
		cell = cell->cw(by_left);
		Point p1 = cell->point(by_left);
		perimeter += (p1 - p0).norm();
		p0 = p1;
	} while(cell != handle_ || by_left != by_left0);

	float segment_len = region_segment_length;
	int n = round(perimeter/segment_len);
	if(!n)
		return 0;
	segment_len -= (n*segment_len - (perimeter - region_perimeter_epsilon))/n;
	float weight = 1.f/(2.f*segment_len);

	Point p_avr(0, 0);
	p0 = cell->point(by_left);
	float len = 0;
	do {
		cell = cell->cw(by_left);
		Point p1 = cell->point(by_left);
		float l = (p1 - p0).norm();
		if((len += l) < segment_len)
			p_avr += (p1 + p0)*l;
		else{
			Point p0_ = p0;
			float ll = segment_len + l;
			float l0 = ll - len;
			do{
				Point p1_ = p0 + (p1 - p0)*((ll - len)/l);
				p_avr += (p1_ + p0_)*l0;
				spline_.push_back(p_avr*weight); 
				p_avr = Point(0, 0);
				p0_ = p1_;
				l0 = segment_len;
				} while((len -= segment_len) > segment_len);
			p_avr = (p1 + p0_)*len;
			}
		p0 = p1;
		} while(cell != handle_ || by_left != by_left0);

	if(spline_.size() < 4)
		return 0;

	return splineInited_ = spline_.reset();
}

void Region::getBorder(borderCall call,void* data, bool recursive) const
{
	if(handle_){
		//xassert(!const_cast<Region*>(this)->dispatcher()->getEditColumn().changed());
		Cell* cell = handle_;
		bool by_left0 = true;
		bool by_left = by_left0;
		Point p0 = cell->point(by_left);
		do {
			Cell* cell_prev = cell;
			cell = cell->cw(by_left);
			Point p1 = cell->point(by_left);

			call(data,p0);

			if(p0.y < p1.y && p0.x < p1.x || p0.y > p1.y && p0.x > p1.x)
				p0.y = p1.y;
			else if(p0.y == p1.y && cell != cell_prev)
				if(p0.x < p1.x)
					++p0.y;
				else
					--p0.y;
			
			if(p0.x < p1.x){
				while(++p0.x != p1.x)
					call(data,p0);
			}
			else if(p0.x > p1.x){
				while(--p0.x != p1.x)
					call(data,p0);
			}
			p0 = p1;
		} while(cell != handle_ || by_left != by_left0);
	}

	if(recursive){
		const_iterator i;
		FOR_EACH(*this, i)
			(*i)->getBorder(call,data, true);
	}
}

RegionDispatcher* Region::dispatcher() 
{
	Region* r = this;
	while(r->parent)
		r = r->parent;
	xassert(!r->numCells());
	return static_cast<RegionDispatcher*>(r);
}

CellLine& Region::cell_line(int y)
{
	return dispatcher()->rasterize_column[y];
}

void Region::append(Region* multi_region)
{
	push_back(multi_region);
	multi_region->parent = this;
}

void Region::remove(Region* region)
{
	list<ShareHandle<Region> >::remove(region);
	iterator i;
	FOR_EACH(*this, i)
		(*i)->remove(region);
}

Region* Region::find_parent_to_append(Column& column)
{
	Region* rr = handle_->r_region;
	if(rr && rr->positive())
		return rr;

	CellLine& line = column[handle_->y];
	CellLine::iterator i;
	FOR_EACH(line, i)
		if(&*i == handle_)
			break;
	
	for(++i; i != line.end(); ++i)
		if(i->r_region && i->r_region->positive())
			return i->r_region;

//	FOR_EACH(line, i){
//		Cell& c = *i;
//	}
  	
//	xassert(0);
	return 0;
}

void Region::save(XBuffer& buf) const
{
	int including = 0;
	if(parent){
		buf < ID_ < int(positive()) < including < y0 < y1 < spline_.size();
		CycledHermite::const_iterator i;
		FOR_EACH(spline_, i)
			buf < i->x < i->y;
		}

	buf < size();
	const_iterator i;
	FOR_EACH(*this, i)
		(*i)->save(buf);
}

void Region::load(XBuffer& buf)
{
	int including;
	if(parent){
		int ncp, positive;
		buf > ID_ > positive > including > y0 > y1 > ncp;
		positive_ = positive;
		while(ncp--){
			spline_.push_back(Vect2f());
			buf > spline_.back().x > spline_.back().y;
			}
		spline_.reset();
		}

	int ncld;
	buf > ncld;
	while(ncld--){
		Region* region = new Region;
		append(region);
		region->load(buf);
		}
}
	
void Region::write_state(XBuffer& out)
{
	out < "Regions: " <= size() < "\n";
	out < "Control Points: " <= spline().size() < "\n";
	iterator i;
	FOR_EACH(*this, i)
		(*i)->write_state(out);
}

void Region::show(sColor4c colorPositive, sColor4c colorNegative)
{
	CycledHermite::iterator i;
	FOR_EACH(spline(), i)
		show_vector(To3D(*i), positive() ? colorPositive : colorNegative);
				
//	float dt = spline.suggest_dt(1);
//	float t = 0;
//	float t_max = spline.t_max() - dt/2;
//	do 
//	{
//		show_vector(To3D(spline(t)), 1, CYAN);
//	} while((t += dt) < t_max);
//	
	iterator ri;
	FOR_EACH(*this, ri)
		(*ri)->show(colorPositive, colorNegative);
}

void Region::restoreSeeds(Column& column, SeedList& seeds)
{
	if(handle_){
		CellLine& line = column[handlePosition_.y];
		if(!line.changed()){
			Cell* cell = line.find(handlePosition_.x);
			xassert(cell);
			seeds.push_back(cell);
		}
	}
	
	iterator ri;
	FOR_EACH(*this, ri)
		(*ri)->restoreSeeds(column, seeds);
}


//////////////////////////////////////////////////////////////////////////////////
//			RegionDispatcher
//////////////////////////////////////////////////////////////////////////////////
RegionDispatcher::RegionDispatcher(int sy)
: toolzer_pos(0, 0),
toolzer_radius(40),
edit_column(sy),
rasterize_column(sy),
add_operation(1),
size_y(sy),
rasterized_(false)
{
	shapeToolzer();
}

void RegionDispatcher::clear()
{
	Region::clear();
	edit_column.clear();
	rasterize_column.clear();
	segments.clear();
}

void RegionDispatcher::setToolzerRadius(int radius)
{
	toolzer_radius = radius;
	if(toolzer_radius < toolzer_radius_min)
		toolzer_radius = toolzer_radius_min;
	if(toolzer_radius > toolzer_radius_max)
		toolzer_radius = toolzer_radius_max;

	shapeToolzer();
	toolzer.move(toolzer_pos);
}

void RegionDispatcher::changeToolzerRadius(int delta)
{
	toolzer_radius += delta;
	if(toolzer_radius < toolzer_radius_min)
		toolzer_radius = toolzer_radius_min;
	if(toolzer_radius > toolzer_radius_max)
		toolzer_radius = toolzer_radius_max;

	shapeToolzer();
	toolzer.move(toolzer_pos);
}

void RegionDispatcher::shapeToolzer()
{
	toolzer.circle(toolzer_radius);
}

void RegionDispatcher::moveToolzer(const Point& p_)
{
	Point p(round(p_.x), round(p_.y));
	toolzer.move(p - toolzer_pos);
	toolzer_pos = p;
}

void RegionDispatcher::line(const Point& p0, const Point& p1, float radius, int operation)
{
	Shape circle;
	circle.circle(radius);
	Point prev_pos = Vect3f::ZERO;

	Point delta = p1 - p0;
	float len = delta.norm();
	if(len < FLT_EPS)
		return;
	int steps = round(len/(radius*toolzer_lineto_radius_factor));
	if(steps < 1)
		steps = 1;
	delta.normalize(len/(float)steps);

	for(int i = 0; i <= steps; i++)
	{
		Point pos(round(p0.x + delta.x*i), round(p0.y + delta.y*i));
		circle.move(pos - prev_pos);
		prev_pos = pos;
		edit_column.operate(circle, operation);
	}
}

void RegionDispatcher::postOperateAnalyze()
{
	start_timer_auto(Region_analyze, STATISTICS_GROUP_AI);
	vectorize(energy_region_vectorize_minimal_region_size, true);
	rasterize();
}

void RegionDispatcher::vectorize(int minimalRegionSize, bool initSpline)
{
	start_timer_auto(vectorize, STATISTICS_GROUP_AI);

	SeedList seeds;
	restoreSeeds(edit_column, seeds);
	Region::clear();

	int analyze_cnt = 0;
	if(edit_column.front().changed())
		CellLine::analyze(CellLine(), edit_column.front(), seeds);
	Column::iterator i, i_next;
	for(i = i_next = edit_column.begin(), ++i_next; i_next != edit_column.end(); i = i_next, ++i_next)
		if(i->changed() || i_next->changed()){
			CellLine::analyze(*i,*i_next, seeds);
			analyze_cnt++;
		}
	if(edit_column.back().changed())
		CellLine::analyze(edit_column.back(), CellLine(), seeds);

	statistics_add(analyze_cnt, STATISTICS_GROUP_NUMERIC, analyze_cnt);

	typedef vector<Region*> RegionList;
	RegionList regions;
	list<ShareHandle<Region> > bad_regions;
	
	SeedList::iterator si;
	FOR_EACH(seeds, si)
		(*si)->l_region = (*si)->r_region = 0;

	Column::iterator ci;
	FOR_EACH(edit_column, ci){
		CellLine::iterator li;
		FOR_EACH(*ci, li)
			li->l_region = li->r_region = 0;
	}

	for(int pass = 0; pass < 2; pass++){
		FOR_EACH(seeds, si){
			Cell* seed = *si;
			if(!seed->l_region && (pass || seed->l_cw == seed)){ // Positive first
				Region* region = new Region();
				region->parent = this;
				region->set(seed);
				if(region->numCells() >= minimalRegionSize && (!initSpline || region->initSpline())){
					regions.push_back(region);
				}
				else{
					region->positive_ = false; // not to be chosen in find_parent_to_append
					bad_regions.push_back(region);
				}
			}
		}
	}

	RegionList::iterator ri;
	FOR_EACH(regions, ri){
		if((*ri)->positive())
			append(*ri);
		else{
			Region* region = (*ri)->find_parent_to_append(edit_column);
			if(region)
				region->append(*ri);
		}
	}

	bad_regions.clear(); 
}

void RegionDispatcher::rasterize()
{
	rasterize_column.clear();
	Region::rasterize(rasterize_column);
	rasterized_ = true;
}

void RegionDispatcher::save(XBuffer& buf) const
{
	buf < IDs;
	Region::save(buf);
}

void RegionDispatcher::load(XBuffer& buf)
{
	Region::clear();
	buf > IDs;
	Region::load(buf);
	rasterize();
}

void RegionDispatcher::saveEditing(XBuffer& buf) const 
{
	save(buf);
	write_container(buf, edit_column);
	write_container(buf, segments);
}

void RegionDispatcher::loadEditing(XBuffer& buf)
{
	load(buf);
	read_vector(buf, edit_column, READ_CONTAINER_REWRITE);
	read_container(buf, segments, READ_CONTAINER_CLEAR);
	xassert(edit_column.size() == rasterize_column.size());
}

void RegionDispatcher::write_state(XBuffer& out)
{
	if(!enable_write_state)
		return;
//	out < "Edit ";
//	edit_column.write_state(out);
//	out < "Rasterize ";
//	rasterize_column.write_state(out);

	XBuffer buf(2000);
	save(buf);
	out < "Send size: " <= buf.tell() < "\n";

	//Region::write_state(out);
}

void RegionDispatcher::check() 
{ 
	if(enable_check)
		return;

//	Region::check(); 
	edit_column.check();
	rasterize_column.check();
}


////////////////////////////////////////////////
//	CycledHermite Spline
////////////////////////////////////////////////
int CycledHermite::reset()
{
	index = -1;
	t_max_ = size_ = size();
	offset = u = 0;
	suggest_factor = 1;

	if(!empty()){
		Vect2f p0 = back();
		iterator i;
		float perimeter = 0;
		FOR_EACH(*this, i){
			perimeter += (*i - p0).norm();
			p0 = *i;
			}
		if(perimeter){
			suggest_factor = t_max_/perimeter;
			return 1;
			}
		}
	return 0;
}

float CycledHermite::suggest_dt(float len)
{
	float dt = len*suggest_factor;
	int n = round(t_max_/dt);
	xassert(n);
	return dt - (n*dt - t_max_)/n;
}

void CycledHermite::set(float t)
{
	xassert(size() >= 4);
	t = cycle(t + offset);
	int i = round(u = floor(t));
	u = t - u;
	if(index != i){
		index = i;
		P[0] = get(index - 1);
		P[1] = get(index);
		P[2] = get(index + 1);
		P[3] = get(index + 2);
		}
}

Vect2f CycledHermite::operator()(float t)
{	
	set(t);
	float t2 = u*u;
	float t3 = u*t2;
	return P[3]*((-t2+t3)/2.0f) + P[0]*(-u/2.0+t2-t3/2.0) + P[2]*(2.0*t2-3.0/2.0*t3+u/2.0) + P[1]*(1.0-5.0/2.0*t2+3.0/2.0*t3);
}

Vect2f CycledHermite::cw_tangent(float t)
{	
	set(t);
	float u2 = u*u;      
	return P[3]*(-u+1.5f*u2) + P[0]*(-0.5f+2.0f*u-1.5f*u2) + P[2]*(4.0*u-9.0/2.0*u2+1.0/2.0) + P[1]*(-5.0*u+9.0/2.0*u2);
}

Vect2f CycledHermite::inward_normal(float t)
{
	Point p = cw_tangent(t);
	return Vect2f(-p.y, p.x);
}

void CycledHermite::getBorder(vector<Vect2s>& border, float scale)
{
	// Создаем четырехсвязную границу.
	border.clear();

	Vect2s v0 = (*this)(0)*scale;
	border.push_back(v0);

	float dt = suggest_dt(scale);
	for(float t = 0; t < t_max() + 3*dt; t += dt) 
	{
		Vect2f vf = (*this)(t)*scale;
		Vect2s v = vf;
		
		float ddt = dt;
		while((v - v0).norm2() > 2) // скачек, больший 1
		{
			v = vf = (*this)(t -= (ddt /= 2))*scale;
			if(ddt < FLT_EPS)
				break;
		}
		
		if(v == v0)
			continue;

		if((v - v0).norm2() == 1)
		{
			border.push_back(v);
		}
		else
		{
			if(fabs(vf.x - (float)v0.x) > fabs(vf.y - (float)v0.y))
				border.push_back(Vect2s(v.x , v0.y));
			else
				border.push_back(Vect2s(v0.x , v.y));
			border.push_back(v);
		}
		v0 = v;
	}

	// Удаляем добавочные точки, чтобы точно сшить границу.
	while(!border.empty() && !(border.front() == border.back()))
	{
		border.pop_back();
	}

	xassert(!border.empty());
	border.pop_back(); // удаляем совпадающую с началом точку
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
//			FieldDispatcher
////////////////////////////////////////////////////////////////////////////////////////////////////////
void FieldDispatcher::setBorder(FieldCluster* cluster, CycledHermite& hermite)
{
	hermite.getBorder(cluster->border_);
	setBorder(cluster, cluster->border_);
}

static
void FieldDispatcherBorderCall(void* data,Vect2f& p)
{
	FieldCluster* cluster=(FieldCluster*)data;
	cluster->border_.push_back(p);
}

void FieldDispatcher::setBorder(FieldCluster* cluster, const Region& region)
{
	cluster->border_.clear();
	region.getBorder(FieldDispatcherBorderCall,cluster);
	setBorder(cluster, cluster->border_);
}

void FieldDispatcher::setCluster(FieldCluster* cluster, const Column& column)
{
	clearCluster(cluster);
	cluster->clear();

	int y_size = w2m((int)column.size());
	for(int y = 0; y < y_size; y++){
		const CellLine& line = column[m2w(y)];
		CellLine::const_iterator li;
		FOR_EACH(line, li){
			int xl = w2m(li->xl);
			int xr = w2m(li->xr);
			int yu = w2m(y);

			int extended = 0;
			FieldCluster::iterator ci;
			FOR_EACH(*cluster, ci)
				if(y == ci->y && !(xr < ci->xl || xl > ci->xr)){
					ci->xl = min(ci->xl, xl);
					ci->xr = max(ci->xr, xr);
					extended = 1;
					break;
				}
				
			if(!extended)
				cluster->push_back(FieldInterval(xl, xr, y));
		}
	}
}

void FieldDispatcher::showDebugInfo() const
{
	for(int y = 1; y < map_.sizeY() - 1; y++)
		for(int x = 1; x < map_.sizeX() - 1; x++){
			if(height(x, y) == FieldCluster::ZeroGround)
				continue;
			Vect3f p0(m2w(x), m2w(y), height(x, y));
			show_vector(p0, height(x, y) > FieldCluster::ZeroGround ? GREEN : CYAN);
			if(map(x, y).specified()){
				Vect3f p1 = to3D(Vect2f(m2w(x), m2w(y)) + map(x, y).specify_delta, FieldCluster::ZeroGround);
				show_line(p0, p1, RED);
				//show_vector(p1, map(x, y).specify_normal, WHITE);
			}
			//show_vector(p0, normal(x, y), YELLOW);
		}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
//			Spline rasterization
////////////////////////////////////////////////////////////////////////////////////////////////////////
void Region::rasterize(Column& rast_column)
{
	if(!spline().empty()){
		float t_up, y_up, t_down, y_down, x_min, x_max;
		spline().set_offset(0);
		spline().find_x_min_max(t_up, x_min, t_down, x_max);
		spline().find_y_min_max(t_up, y_up, t_down, y_down);

		x0 = clamp(floor(x_min) - 1, 0, INT_INF);
		x1 = ceil(x_max) + 1;

		y0 = floor(y_up);
		y1 = ceil(y_down);

		if(y0 < 0)
			y0 = 0;
		if(y1 >= rast_column.size())
			y1 = rast_column.size() - 1;
		
		vector<vector<int> > column(y1 - y0 + 1);

		#define PUT(x, y) { if(y >= y0 && y <= y1){ vector<int>& line = column[y - y0]; line.insert(find_if(line.begin(), line.end(), bind1st(less<int>(), x)), x); } }
	
		spline().set_offset(t_up);
		float dt = spline().suggest_dt(rasterize_dlen);
		float t_max = spline().t_max() - dt/2;
		float t = 0;
		Point p = spline()(0);
		int xc0 = round(p.x);
		int yc0 = round(p.y);
		int xc = xc0;
		int yc = yc0;
		int dy = 1;
		PUT(xc, yc);
		do {
			Point p = spline()(t);
			int x = round(p.x);
			int y = round(p.y);
			if(dy == 1){
				if(yc < y){
					PUT(xc, yc);
					while(++yc < y){
						PUT((x + xc)/2, yc);
						}
					xc = x;
					}
				else{
					if(y == yc){
						if(xc < x)
							xc = x;
						}
					else{ // yc > y
						dy = -1;
						PUT(xc, yc);
						PUT(x, yc);
						while(--yc > y){
							PUT((x + xc)/2, yc);
							}
						xc = x;
						}
					}
				}
			else // dy == -1
				if(yc > y){
					PUT(xc, yc);
					while(--yc > y){
						PUT((x + xc)/2, yc);
						}
					xc = x;
					}
				else
					if(yc == y){
						if(xc > x)
							xc = x;
						}
					else{ // yc < y
						dy = 1;
						PUT(xc, yc);
						PUT(x, yc);
						while(++yc < y){
							PUT((x + xc)/2, yc);
							}
						xc = x;
						}
			} while((t += dt) < t_max);

		while(yc > yc0){
			PUT((xc0 + xc)/2, yc);
			yc--;
			}

		#undef PUT
		
		vector<vector<int> >::iterator ci;
		int y = 0;
		FOR_EACH(column, ci){
			vector<int>::iterator li;
			//xassert(!(ci->size() & 1));
			FOR_EACH(*ci, li){
				int xl = *li;
				if(++li == ci->end())
					break;
				int xr = *li;
				if(xl != xr)
					if(positive())
						rast_column[y + y0].add(Interval(xl, xr), this);
					else 
						rast_column[y + y0].sub(Interval(xl, xr), this);
				}
			y++;
			}
		}

	iterator ir;
	FOR_EACH(*this, ir)
		(*ir)->rasterize(rast_column);
}

void CycledHermite::find_x_min_max(float& t_min, float& x_min, float& t_max, float& x_max)
{
	x_min = FLT_INF;
	x_max = -FLT_INF;
	for(int i = 0; i < size_; i++){
		float x_1 = get(i - 1).x;
		float x0 = get(i).x;
		float x1 = get(i + 1).x;
		float x2 = get(i + 2).x;

		if(x_min > x0){
			x_min = x0;
			t_min = i;
			}
		if(x_max < x0){
			x_max = x0;
			t_max = i;
			}

		float t18 = -x2+x_1;
		float t17 = -3.0*x1+3.0*x0-t18;
		if(fabs(t17) < FLT_EPS)
			continue;
		t17 = 1.f/t17/3.0;
		float t16 = x2-4.0*x1-2.0*x_1+5.0*x0;
		float t10 = x2*x2+t18*x_1+(10.0*x_1-11.0*x2+25.0*x1)*x1+(-49.0*x1+10.0*x2-11.0*x_1+25.0*x0)*x0;
		if(t10 < 0)
			continue;
		t10 = sqrt(t10);
		
		float u = (-t10+t16)*t17;
		if(u > 0 && u < 1.f){
			float u2 = u*u;
			float u3 = u*u2;
			float x = x2*((-u2+u3)/2.0f) + x_1*(-u/2.0+u2-u3/2.0) + x1*(2.0*u2-3.0/2.0*u3+u/2.0) + x0*(1.0-5.0/2.0*u2+3.0/2.0*u3);
			if(x_min > x){
				x_min = x;
				t_min = u + i;
				}
			if(x_max < x){
				x_max = x;
				t_max = u + i;
				}
			}
			
		u = (t10+t16)*t17;
		if(u > 0 && u < 1.f){
			float u2 = u*u;
			float u3 = u*u2;
			float x = x2*((-u2+u3)/2.0f) + x_1*(-u/2.0+u2-u3/2.0) + x1*(2.0*u2-3.0/2.0*u3+u/2.0) + x0*(1.0-5.0/2.0*u2+3.0/2.0*u3);
			if(x_min > x){
				x_min = x;
				t_min = u + i;
				}
			if(x_max < x){
				x_max = x;
				t_max = u + i;
				}
			}
		}

	xassert(x_min != FLT_INF && x_max != -FLT_INF);
}			

void CycledHermite::find_y_min_max(float& t_min, float& y_min, float& t_max, float& y_max)
{
	y_min = FLT_INF;
	y_max = -FLT_INF;
	for(int i = 0; i < size_; i++){
		float y_1 = get(i - 1).y;
		float y0 = get(i).y;
		float y1 = get(i + 1).y;
		float y2 = get(i + 2).y;

		if(y_min > y0){
			y_min = y0;
			t_min = i;
			}
		if(y_max < y0){
			y_max = y0;
			t_max = i;
			}

		float t18 = -y2+y_1;
		float t17 = -3.0*y1+3.0*y0-t18;
		if(fabs(t17) < FLT_EPS)
			continue;
		t17 = 1.f/t17/3.0;
		float t16 = y2-4.0*y1-2.0*y_1+5.0*y0;
		float t10 = y2*y2+t18*y_1+(10.0*y_1-11.0*y2+25.0*y1)*y1+(-49.0*y1+10.0*y2-11.0*y_1+25.0*y0)*y0;
		if(t10 < 0)
			continue;
		t10 = sqrt(t10);
		
		float u = (-t10+t16)*t17;
		if(u > 0 && u < 1.f){
			float u2 = u*u;
			float u3 = u*u2;
			float y = y2*((-u2+u3)/2.0f) + y_1*(-u/2.0+u2-u3/2.0) + y1*(2.0*u2-3.0/2.0*u3+u/2.0) + y0*(1.0-5.0/2.0*u2+3.0/2.0*u3);
			if(y_min > y){
				y_min = y;
				t_min = u + i;
				}
			if(y_max < y){
				y_max = y;
				t_max = u + i;
				}
			}
			
		u = (t10+t16)*t17;
		if(u > 0 && u < 1.f){
			float u2 = u*u;
			float u3 = u*u2;
			float y = y2*((-u2+u3)/2.0f) + y_1*(-u/2.0+u2-u3/2.0) + y1*(2.0*u2-3.0/2.0*u3+u/2.0) + y0*(1.0-5.0/2.0*u2+3.0/2.0*u3);
			if(y_min > y){
				y_min = y;
				t_min = u + i;
				}
			if(y_max < y){
				y_max = y;
				t_max = u + i;
				}
			}
		}

	xassert(y_min != FLT_INF && y_max != -FLT_INF);
}			

//////////////////////////////////////////////////////////
//			Clipping
//////////////////////////////////////////////////////////
RegionDispatcher::Segment::ClipResult RegionDispatcher::Segment::clip(Segment& border, Segment& rest)
{
	Point delta = border.p1 - border.p0;
	Point normal(-delta.y, delta.x);
	int p0_outside = normal.dot(p0 - border.p0) >= 0;
	int p1_outside = normal.dot(p1 - border.p0) >= 0;
	if(p0_outside && p1_outside)
		return Outside;

	if(!p0_outside && !p1_outside)
		return Inside;

	float t3 = border.p1[0]*p0[1];
	float t4 = border.p0[0]*p0[1];
    float t5 = p0[0]*border.p1[1];
    float t6 = p0[0]*border.p0[1];
    float t11 = p1[0]*border.p0[1];
    float t13 = border.p0[0]*p1[1];
    float t15 = p1[0]*border.p1[1]-t11-t5+t6-border.p1[0]*p1[1]+t3+t13-t4;
	if(fabsf(t15) < FLT_EPS)
	{
		xassert(0);
		return Inside;
	}
	t15 = 1.f/t15;
    float t1 = (t3-t4-t5+t6+border.p1[1]*border.p0[0]-border.p0[1]*border.p1[0])*t15;
    float t2 = -(-p1[0]*p0[1]+t11-t6+p0[0]*p1[1]-t13+t4)*t15;
	if(t1 > 0.f && t1 < 1.f && t2 > 0.f && t2 < 1.f)
	{
		Point p = p0 + (p1 - p0)*t1;
		if(p1_outside)
		{
			rest.p0 = p;
			rest.p1 = p1;
			p1 = p;
		}
		else  // p0_ouside
		{
			rest.p0 = p0;
			rest.p1 = p;
			p0 = p;
		}

		return Clipped;
	}

	return Intermediate;
}

RegionDispatcher::Segment::ClipResult RegionDispatcher::Segment::clip(const Point& p, float radius, Segment& rest)
{
	// p0 + (p1 - p0)*t = p + radius*[cos(alpha), sin(alpha)]
	Point dp = p1 - p0;
	Point dp1 = p0 - p;
	// So, dp1 + dp*t = radius*[cos(alpha), sin(alpha)], 
	// Excluding alpha: (dp1 + dp*t)^2 = radius^2
	float a = dp.norm2();
	float b = 2*dp1.dot(dp);
	float c = dp1.norm2() - radius*radius;
	float D = b*b - 4*a*c;
	if(fabsf(a) < FLT_EPS)
		return Inside;
	if(D <= 0)
		return Outside;
	D = sqrtf(D);
	float t1 = (-b - D)/(2*a);
	float t2 = (-b + D)/(2*a);

	if(t2 <= 0 || t1 >= 1) //  +---+ (  )  or  (  ) +---+
		return Outside;

	if(t1 <= 0)
		if(t2 >= 1)  //  ( +---+ )
			return Inside;
		else  //  ( +-- ) -+
		{
			p0 += dp*t2;
			return Outside;
		}
	else
		if(t2 >= 1)  //  +- ( --+ )
		{
			p1 = p0 + dp*t1;
			return Outside;
		}
		else  //  +- ( - ) -+
		{
			rest.p0 = p0 + dp*t2;
			rest.p1 = p1;
			p1 = p0 + dp*t1;
			return Clipped;
		}
}


void RegionDispatcher::clip_by_segment(const Point& p0_, const Point& p1_, float clip_radius, int saved_segments_number)
{
	Point p0(p0_), p1(p1_);
	Point dir = p1 - p0;
	float len = dir.norm();

	if(segments.size() <= saved_segments_number || len < 0.1)
		return;

	dir /= len;
	Point normal(-dir.y, dir.x);
	p0 -= dir*clip_radius;
	p1 += dir*clip_radius;
	Point delta = normal*clip_radius;
	Point border_points[4] = { p0 + delta, p1 + delta, p1 - delta, p0 - delta };
	Segment borders[4] = { Segment(border_points[0], border_points[1]),	Segment(border_points[1], border_points[2]), 
		Segment(border_points[2], border_points[3]), Segment(border_points[3], border_points[0]) };

	Segment rest;
	SegmentList::iterator end = segments.end();	
	for(int i = 0; i < saved_segments_number; i++)
		--end;
	for(SegmentList::iterator si = segments.begin(); si != end; ++si)
	{
		// Test whether segment's line intersects border rectangle
		Point p0 = si->p0;
		Point delta = si->p1 - p0;
		Point normal(-delta.y, delta.x);
		if(normal.dot(border_points[0] - p0) >= 0)
		{
			for(int i = 1; i < 4; i++)
				if(normal.dot(border_points[i] - p0) < 0)
					goto may_clip;
			goto next_segment;
		}
		else
		{
			for(int i = 1; i < 4; i++)
				if(normal.dot(border_points[i] - p0) >= 0)
					goto may_clip;
			goto next_segment;
		}

may_clip:
		{
		for(int i = 0; i < 4; i++)
			switch(si->clip(borders[i], rest))
			{
			case Segment::Outside:
				goto next_segment;
			case Segment::Clipped:
				segments.insert(si, rest);
				break;
			}
		}
		si = segments.erase(si);
		--si;

next_segment: ;
	}
}

void RegionDispatcher::clip_by_circle(const Point& p, float clip_radius, int saved_segments_number)
{
	if(segments.size() <= saved_segments_number)
		return;

	Segment rest;
	SegmentList::iterator end = segments.end();	
	for(int i = 0; i < saved_segments_number; i++)
		--end;
	for(SegmentList::iterator si = segments.begin(); si != end; ++si)
		switch(si->clip(p, clip_radius, rest))
		{
		case Segment::Clipped:
			segments.insert(si, rest);
			break;
		case Segment::Inside:
			si = segments.erase(si);
			--si;
			break;
		}
}

//////////////////////////////////////////////////////////
//	Контейнер слоев
//////////////////////////////////////////////////////////
RegionMetaDispatcher::RegionMetaDispatcher(int layers, int sy,bool multithreaded_) 
{ 
	MTINIT(lock);

	multithreaded=multithreaded_;
	locked=1;
	regions.reserve(layers); 
	while(layers-- > 0) 
		regions.push_back(new RegionDispatcher(sy)); 
	if(!regions.empty()) 
		setActiveLayer(0); 
	need_to_analyze = false;

	if(multithreaded)
		locked=0;
}

RegionMetaDispatcher::~RegionMetaDispatcher()
{
	MTDONE(lock);
}

void RegionMetaDispatcher::setActiveLayer(int layer) 
{
	checkLocked();
	active_layer = regions.at(layer); 
	temporal_segment_log = 0;
	prev_segment_point.set(0, 0);
}

void RegionMetaDispatcher::postOperateAnalyze() 
{
	checkLocked();
	if(!need_to_analyze)
		return;
	need_to_analyze = false;

	iterator layer;
	FOR_EACH(regions, layer)
		(*layer)->postOperateAnalyze();
}
		
void RegionMetaDispatcher::operate()
{
	checkLocked();
	float radius = active_layer->toolzer_radius;
	Point pos = active_layer->toolzer_pos;
	if(active_layer->add_operation)
	{
		iterator li;
		FOR_EACH(regions, li)
		{
			if(*li != active_layer)
			{
				(*li)->operateByCircle(pos, radius + toolzer_clear_back_layers_radius_delta, 0);
				(*li)->clip_by_circle(pos, radius + toolzer_clear_back_layers_radius_delta + (*li)->toolzer_radius, 0);
			}
			else
				(*li)->operateByCircle(pos, radius, 1);
		}
	}
	else
	{
		active_layer->operateByCircle(pos, radius + toolzer_clear_back_layers_radius_delta, 0);
		active_layer->clip_by_circle(pos, radius + toolzer_clear_back_layers_radius_delta, 0);
	}

	// Show changes
	need_to_analyze = true;
}

void RegionMetaDispatcher::beginLine(const Point& p)
{
	checkLocked();
	prev_segment_point = p;
	temporal_segment_log = 0;
}

void RegionMetaDispatcher::endLine()
{
	checkLocked();
	lineto(prev_segment_point, 0);
}

void RegionMetaDispatcher::lineto(const Point& p, int permanent)
{
	checkLocked();
	// Disable short segments
	if(prev_segment_point.distance(p) < 0.1)
		permanent = 0;

	//active_layer->toolzer_radius = toolzer_radius_fixed; // !!!!!
	float toolzerRadius = toolzer_radius_fixed;

	// Erase active_layer: after clipping it may change
	float erase_radius = toolzerRadius + toolzer_clear_back_layers_radius_delta;
	RegionDispatcher::SegmentList::iterator si;
	FOR_EACH(active_layer->segments, si)
		active_layer->line(si->p0, si->p1, erase_radius, 0);

	// Remove temporal segment
	if(temporal_segment_log)
	{
		active_layer->line(temporal_segment.p0, temporal_segment.p1, erase_radius, 0);
		temporal_segment_log = 0;
	}

	// Store segments for non-permanent clipping
	RegionDispatcher::SegmentList segments_copy;
	if(!permanent)
		segments_copy = active_layer->segments;

	// Clear and clip by new segment
	if(active_layer->add_operation)
	{
		iterator li;
		FOR_EACH(regions, li)
			if(*li != active_layer)
			{
				if(permanent)
				{
					(*li)->line(prev_segment_point, p, erase_radius, 0);
					(*li)->clip_by_segment(prev_segment_point, p, erase_radius, 0);
				}
			}
			else
			{
				active_layer->line(prev_segment_point, p, toolzerRadius, 1);
				active_layer->clip_by_segment(prev_segment_point, p, toolzer_segment_autoclip_radius, toolzer_lineto_saved_segments_number);
			}
	}
	else
	{
		active_layer->line(prev_segment_point, p, erase_radius, 0);
		active_layer->clip_by_segment(prev_segment_point, p, toolzer_segment_autoclip_radius, 0);
	}

	// Push new segments 
	if(permanent && active_layer->add_operation)
		active_layer->segments.push_back(RegionDispatcher::Segment(prev_segment_point, p));

	// Draw active_layer's segments
	FOR_EACH(active_layer->segments, si)
		active_layer->line(si->p0, si->p1, toolzerRadius, 1);

	// Draw temporal segment
	if(!permanent)
	{
		if(prev_segment_point.distance(p) > 0.1)
		{
			temporal_segment.p0 = prev_segment_point;
			temporal_segment.p1 = p;
			temporal_segment_log = 1;
			active_layer->line(prev_segment_point, p, active_layer->add_operation ? toolzerRadius : toolzerRadius + toolzer_clear_back_layers_radius_delta/2, 1);
		}

		active_layer->segments = segments_copy;
	}
	else
		prev_segment_point = p;

	// Show changes
	need_to_analyze = true;
}


Region* RegionMetaDispatcher::locate(const Vect2i& p)
{ 
	checkLocked();
	iterator li;
	Region* t;
	FOR_EACH(regions,li)
	{
		t = (*li)->locateEdit(p);
		if(t)
			return t;
	}
	return NULL;
}

int RegionMetaDispatcher::filled(const Vect2i& p)
{ 
	checkLocked();
	iterator li; 
	FOR_EACH(regions,li){
		if((*li)->filledRasterized(p))
			return 1;
	}
	return 0; 
}

RegionMetaDispatcher& RegionMetaDispatcher::operator=(const RegionMetaDispatcher& metaDispatcher)
{
	MetaRegionLock lock(&metaDispatcher);
	Lock();
	checkLocked();
	XBuffer buffer(5000, 1);
	metaDispatcher.saveEditing(buffer);
	buffer.set(0);
	loadEditing(buffer);
	Unlock();
	return *this;
}

void RegionMetaDispatcher::Lock() const
{
	xassert(multithreaded);
	MTENTER(lock);
	locked++;
}

void RegionMetaDispatcher::Unlock() const
{
	xassert(multithreaded);
	locked--;
	MTLEAVE(lock);
}

