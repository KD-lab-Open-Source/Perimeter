#ifndef __REGION_H__
#define __REGION_H__

#include "..\util\ProTool.h"
#include "Handle.h"

typedef Vect2f Point;
typedef vector<Vect2s> Vect2sVect;

typedef vector<Column*> ColumnVect;

struct Interval
{
	short xl, xr;
	Interval(){}
	Interval(short xl_, short xr_) : xl(xl_), xr(xr_) {}
	int delta() const { return xr - xl + 1; }
};

class Shape : public vector<Interval>
{
public:
	int y0;

	void circle(int radius);
	void square(int sx, int sy);
	void move(const Vect2i& delta);
	void show(sColor4c color);
};

class Cell : public Interval
{
public:
	int y;
	Cell *l_cw, *r_cw;
	class Region *l_region, *r_region;

	Cell() { l_region = r_region = 0; l_cw = r_cw = 0; }
	Cell(const Interval& in, int y_) : Interval(in), y(y_) { l_region = r_region = 0; l_cw = r_cw = 0; }
	Cell* cw(bool& by_left);
	Point point(bool by_left) const { return Point(by_left ? xl : xr, y); }
	void show(sColor4c color) const;
	friend XBuffer& operator< (XBuffer& buf, const Cell& cell){ buf < cell.y < cell.xl < cell.xr; return buf; }
	friend XBuffer& operator> (XBuffer& buf, Cell& cell){ buf > cell.y > cell.xl > cell.xr; return buf; }
};

typedef vector<Cell*> SeedList;
class Column;

class CellLine : public list<Cell>
{
	typedef list<Cell> List;

public:
	CellLine() { y = 0; column_ = 0; changeCounter_ = 0; }
	CellLine(const CellLine& line) { y = 0; column_ = 0; changeCounter_ = 0; *this = line; }
	CellLine& operator=(const CellLine& line);

	void add(const Interval& in, Region* region = 0);
	void sub(const Interval& in, Region* region = 0);
	void intersect(const CellLine& line);
	void intersect(const CellLine& lineA, const CellLine& lineB);

	bool changed() const;
	bool changedPrev() const;

	bool filled(int x) const { const_iterator i; FOR_EACH(*this, i) if(i->xl <= x && x <= i->xr) return true; return false; }
	Region* locate(int x) const;
	int intersected(const Interval& in) const { const_iterator i; FOR_EACH(*this, i) if(!(i->xr < in.xl || in.xr < i->xl)) return 1; return 0; }
	int area() const { int a = 0; const_iterator i; FOR_EACH(*this, i) a += i->delta(); return a; }
	
	void find_interval(const Interval& in, iterator& il, iterator& ir_);
	Cell* find(int x) { iterator i; FOR_EACH(*this, i) if(i->xl <= x && x <= i->xr) return &*i; return 0; }
	void check();
	void checkAnalyzing();
	void show(sColor4c color) const;
	
	static void analyze(CellLine& line1, CellLine& line2, SeedList& seeds);

	friend XBuffer& operator< (XBuffer& buf, const CellLine& line){ buf < line.y; write_container(buf, line); return buf; }
	friend XBuffer& operator> (XBuffer& buf, CellLine& line){ buf > line.y; read_container(buf, line); return buf; }

private:
	int y;
	Column* column_;
	int changeCounter_;

	void setChanged(int deltaArea);

	friend Column;
};

class Column : public vector<CellLine>
{
public:
	Column(int sy);
	
	void clear();
	
	void add(const Column& column);
	void sub(const Column& column);
	void intersect(const Column& column);
	void intersect(const Column& columnA, const Column& columnB);
	void addCircleThenSub(const Vect2i& pos, float radius, const ColumnVect& columns);

	void operate(const Shape& shape, int add);
	void operateByCircle(const Vect2i& pos, float radius, int add);
	void operateByCycledHermite(class CycledHermite& hermite, int add);
	
	bool filled(int x, int y) const { return y >= 0 && y < size() ? (*this)[y].filled(x) : false; }
	int intersected(const Shape& shape);
	void expandFilledBound(Vect2i& leftTop, Vect2i& rightBottom) const;

	void setChanged(int deltaArea) { lastChangeCounter_ = changeCounter(); area_ += deltaArea; }
	void setUnchanged() { ++changeCounter_; }
	int changeCounter() const { return changeCounter_; }
	bool changed() const { return lastChangeCounter_ == changeCounter(); }
	bool changedPrev() const { return lastChangeCounter_ == changeCounter() || lastChangeCounter_ + 1 == changeCounter(); }

	int area() const { return area_; }
	int areaCalculated() const { int a = 0; const_iterator i; FOR_EACH(*this, i) a += i->area(); return a; }

	void check();
	void show(sColor4c color) const;

private:
	int area_;
	int changeCounter_;
	int lastChangeCounter_;
};

////////////////////////////////////////////////
//	Hermite Spline
////////////////////////////////////////////////
class CycledHermite : public vector<Vect2f> 
{
	Vect2f P[4];
	float t_max_, u, offset;
	float suggest_factor;
	int index, size_;
	const Vect2f& get(int i){ return (*this)[(i + size_) % size_]; }
	void set(float t);

public:
	CycledHermite(){ reset(); }
	int reset();
	float t_max() const { return t_max_; }
	float cycle(float t) const { t = fmodf(fmodf(t, t_max_) + t_max_, t_max_); return t; }

	float suggest_dt(float len);
	void set_offset(float t){ offset = cycle(t); }
	float get_offset() const { return offset; }

	Vect2f operator()(float t);   // function
	Vect2f cw_tangent(float t); // касательная по часовой
	Vect2f inward_normal(float t); // нормаль внутрь

	void find_x_min_max(float& t_min, float& x_min, float& t_max, float& x_max);
	void find_y_min_max(float& t_min, float& y_min, float& t_max, float& y_max);

	void getBorder(Vect2sVect& border, float scale = 1.f);
};

////////////////////////////////////////////////
//		Регион
////////////////////////////////////////////////
class Region : public list<ShareHandle<Region> >, public ShareHandleBase
{
public:
	Region();
	~Region();

	bool positive() const { return positive_; }
	int ID() const { return ID_; };
	int numCells() const { return numCells_; }

	CycledHermite& spline() { return spline_; }

	void show(sColor4c colorPositive, sColor4c colorNegative);

	template<class BorderOp, class InternalOp> 
	void scanRecursive(BorderOp& border_op, InternalOp& internal_op, int space) 
	{
		if(positive())
			scan(border_op, internal_op, space);

		iterator i;
		FOR_EACH(*this, i)
			(*i)->scanRecursive(border_op, internal_op, space);
	}

	typedef void (*borderCall)(void* data,Vect2f& p);

	void getBorder(borderCall call,void* data, bool recursive = false) const; // square distance between points == 1 or 2, doesn't clear the border

protected:
	int ID_;
	int numCells_;
	Cell* handle_;
	Vect2i handlePosition_;
	bool positive_;
	int x0, x1;
	int y0, y1;

	CycledHermite spline_;
	bool splineInited_;

	Region* parent;

	static int IDs;
	static int num_entities;

	class RegionDispatcher* dispatcher();
	
	void set(Cell* cell);
	bool initSpline();
	CellLine& cell_line(int y);
	void append(Region* region);
	void remove(Region* region);
	void restoreSeeds(Column& column, SeedList& seeds);
	void rasterize(Column& column);
	Region* find_parent_to_append(Column& column);
	void save(XBuffer& buf) const;
	void load(XBuffer& buf);
	void write_state(XBuffer& out);

	template <class BorderOp, class InternalOp> 
	void scan(BorderOp& border_op, InternalOp& internal_op, int space) 
	{
		start_timer_auto(Region_scan, STATISTICS_GROUP_AI);
		assert(positive_);

		int x_off = x0 - x0 % space;
		int x_end = x1 + (x1 % space ? space - x1 % space : 0);
		int x_size = (x_end - x_off)/space;
		int x_size_world = x_size*space;
		int xc0 = x_off + space/2;
		vector<int> counters_line(x_size + 1);

		int y = Region::y0 - Region::y0 % space;
		int y_end = Region::y1 + (Region::y1 % space ? space - Region::y1 % space : 0);
		int y_size = (y_end - y)/space;
		int yc = y + space/2;
		int space2 = space*space;
		xassert(y + y_size*space <= dispatcher()->size_y);

		while(y_size-- > 0){
			fill(counters_line.begin(), counters_line.end(), 0);
			for(int j = 0; j < space; j++){
				CellLine& line = cell_line(y++);
				CellLine::iterator i;
				FOR_EACH(line, i){
					//if(i->l_region == this || i->r_region == this || i->l_region->parent == this || i->r_region->parent == this)
					int xl = clamp(i->xl - x_off, 0, x_size_world);
					int xr = clamp(i->xr - x_off, 0, x_size_world);
					int xl_ = xl + (space - xl % space) % space;
					int xr_ = xr - xr % space;
					counters_line[xl/space] += xl_ - xl; 
					int i1;
					counters_line[i1 = xr/space] += xr - xr_; 
					int i0 = xl_/space;
					while(i0 < i1)
						counters_line[i0++] += space; 
				}
			}

			int xc = xc0;
			vector<int>::iterator ci;
			FOR_EACH(counters_line, ci){
				if(*ci)
					if(*ci == space2)
						internal_op(xc, yc);
					else
						border_op(xc, yc);
				xc += space;
				}
			yc += space;
			}				
	}

	static void set_IDs(int id){ IDs = id; };
	static int get_IDs(){ return IDs; };

	friend class RegionDispatcher;
};

//////////////////////////////////////////////////////////
//	Контейнер регионов
//////////////////////////////////////////////////////////
class RegionDispatcher : public Region
{
public:
	RegionDispatcher(int sy);
	void clear();

	void setToolzerRadius(int radius);
	void changeToolzerRadius(int delta);
	int getToolzerRadius() const { return toolzer_radius; }
	void moveToolzer(const Point& coords);
	void setOperation(int add_operation_) { add_operation = add_operation_; }

	void save(XBuffer& buf) const;
	void load(XBuffer& buf); 

	void saveEditing(XBuffer& buf) const;
	void loadEditing(XBuffer& buf); 
	
	Region* locateEdit(const Vect2i& p) { return rasterize_column[at_y(p.y)].locate(p.x); }
	int filledEdit(const Vect2i& p) const { return rasterize_column[at_y(p.y)].filled(p.x); }

	Region* locateRasterized(const Vect2i& p) { return rasterize_column[at_y(p.y)].locate(p.x); } // xassert(rasterized_); 
	int filledRasterized(const Vect2i& p) const { return rasterize_column[at_y(p.y)].filled(p.x); } // xassert(rasterized_); 
	
	Column& getEditColumn() { return edit_column; }
	Column& getRasterizeColumn() { return rasterize_column; } // xassert(rasterized_); 
	const Column& getEditColumn() const { return edit_column; }
	const Column& getRasterizeColumn() const { xassert(rasterized_); return rasterize_column; }

	// Ручная работа с регионами
	void operateByCircle(const Point& pos, float radius, int add) { edit_column.operateByCircle(pos, radius, add); }
	void vectorize(int minimalRegionSize, bool initSpline);
	void postOperateAnalyze(); // Перестраивает регионы и растр для быстрого доступа (rasterize_column)

	// Debug
	void write_state(XBuffer& out);
	void check();

	struct Segment
	{
		Point p0, p1;
		Segment(){}
		Segment(const Point& p0_, const Point& p1_) : p0(p0_), p1(p1_) {}
		enum ClipResult { Outside, Clipped, Inside, Intermediate };
		ClipResult clip(Segment& border, Segment& rest);
		ClipResult clip(const Point& p, float radius, Segment& rest); // 1 - completely inside, therefore delete
		friend XBuffer& operator< (XBuffer& buf, const Segment& segment){ buf.write(&segment, sizeof(segment)); return buf; }
		friend XBuffer& operator> (XBuffer& buf, Segment& segment){ buf.read(&segment, sizeof(segment)); return buf; }
	};

private:
	int size_y;
	float toolzer_radius;
	Point toolzer_pos;
	Shape toolzer;
	int add_operation;
	
	Column edit_column;
	Column rasterize_column;

	bool rasterized_;
	
	typedef list<Segment> SegmentList;
	SegmentList segments;
	
	void shapeToolzer();
	void rasterize();
	void line(const Point& p0, const Point& p1, float radius, int operation);
	void clip_by_segment(const Point& p0, const Point& p1, float clip_radius, int saved_segments_number);
	void clip_by_circle(const Point& p, float clip_radius, int saved_segments_number);
	int at_y(int y) const { return y >= 0 ? (y < size_y ? y : size_y - 1) : 0; }
	
	friend Region;
	friend class RegionMetaDispatcher;
};

//////////////////////////////////////////////////////////
//	Контейнер слоев
//////////////////////////////////////////////////////////

class MetaLockRegionDispatcher
{
	RegionMetaDispatcher* meta;
	RegionDispatcher* region;
public:
	MetaLockRegionDispatcher()
	{
		meta=NULL;
		region=NULL;
	}

	MetaLockRegionDispatcher(RegionMetaDispatcher* meta_,RegionDispatcher* region_)
	{
		meta=meta_;
		region=region_;
	}

	inline RegionDispatcher* MetaLockRegionDispatcher::operator->();
	inline RegionDispatcher* data();
};

class RegionMetaDispatcher
{
	RegionDispatcher* active_layer;

	Point prev_segment_point;
	RegionDispatcher::Segment temporal_segment;
	int temporal_segment_log;
	bool need_to_analyze;

	vector<ShareHandle<RegionDispatcher> > regions;
	typedef vector<ShareHandle<RegionDispatcher> >::iterator iterator;
	typedef vector<ShareHandle<RegionDispatcher> >::const_iterator const_iterator;
	
	bool multithreaded;
	mutable int locked;//для assert
	mutable MTDECLARE(lock);
public:
	RegionMetaDispatcher(int layers, int sy,bool multithreaded=true);
	~RegionMetaDispatcher();
	void clear() {	checkLocked(); iterator i; FOR_EACH(regions, i) (*i)->clear(); }

	//Lock,Unlock вызываются при любом обращении к данным 
	//желательно вызывать как можно реже.
	void Lock() const;
	void Unlock() const;
	void checkLocked()const{xassert(locked);}
	MetaLockRegionDispatcher operator[](int i)
	{
		return MetaLockRegionDispatcher(this,&*regions.at(i));
	}

	RegionMetaDispatcher& operator=(const RegionMetaDispatcher& metaDispatcher);

	void setActiveLayer(int layer);
	RegionDispatcher* activeLayer() { checkLocked(); return active_layer; }
	
	void setOperation(int operation){ checkLocked(); active_layer->setOperation(operation); }
	int getToolzerRadius() const { checkLocked(); return active_layer->getToolzerRadius(); }
	void changeToolzerRadius(int delta) { checkLocked(); active_layer->changeToolzerRadius(delta); }

	void operate(); 
	void beginLine(const Point& p);
	void lineto(const Point& p, int permanent); // Рисует линию текущей операцией от текущей позиции
	void endLine(); // Убирает временную линию
	void postOperateAnalyze();

	Region* locate(const Vect2i& p);
	int filled(const Vect2i& p);

	void save(XBuffer& buf) { checkLocked(); iterator i; FOR_EACH(regions, i) (*i)->save(buf); }
	void load(XBuffer& buf) { checkLocked(); iterator i; FOR_EACH(regions, i) (*i)->load(buf); }

	bool changed() const { checkLocked(); const_iterator i; FOR_EACH(regions, i) if(!(*i)->empty()) return true; return false; }
	void saveEditing(XBuffer& buf) const { checkLocked(); const_iterator i; FOR_EACH(regions, i) (*i)->saveEditing(buf); }
	void loadEditing(XBuffer& buf) { checkLocked(); iterator i; FOR_EACH(regions, i) (*i)->loadEditing(buf); }
};

class MetaRegionLock
{
	const RegionMetaDispatcher* region;
public:
	MetaRegionLock(const RegionMetaDispatcher* region_)
	{
		region=region_;
		region->Lock();
	}

	~MetaRegionLock()
	{
		region->Unlock();
	}
};

inline RegionDispatcher* MetaLockRegionDispatcher::operator->()
{
	meta->checkLocked();
	return region;
}

inline RegionDispatcher* MetaLockRegionDispatcher::data()
{
	meta->checkLocked();
	return region;
}
//--------------------------------------------------

inline bool CellLine::changed() const
{
	return changeCounter_ == column_->changeCounter();
}

inline bool CellLine::changedPrev() const
{
	return changeCounter_ == column_->changeCounter() || changeCounter_ + 1 == column_->changeCounter();
}

inline void CellLine::setChanged(int deltaArea)
{
	if(column_){
		changeCounter_ = column_->changeCounter();
		column_->setChanged(deltaArea);
	}
}


#endif // __REGION_H__
