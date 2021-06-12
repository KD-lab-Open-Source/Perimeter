#ifndef _POSITION_GENERATOR_H_
#define _POSITION_GENERATOR_H_

////////////////////////////////////////////////////////////////////////////////
//	Генератор позиций.
////////////////////////////////////////////////////////////////////////////////
class PositionGenerator
{
public:
	enum Mode { Square, Column, Rank };

	PositionGenerator(bool up_) { up = up_; mode = Square; curvature_radius = 0; clear(); }
	void setMode(Mode m, float curvature_radius_ = 0){ if(mode != m || curvature_radius != curvature_radius_) clear(); mode = m; curvature_radius = curvature_radius_; }
	void clear() { max_side = 0; lines.clear(); m_counter = 0; radius_ = 0; }
	Vect2f get(float radius)
	{
		radius_ = radius;
		++m_counter;
		float diameter = 2*radius_;
		float x_acceptable = mode != Rank ? max_side : FLT_INF; // В режиме шеренги - расширяться только в стороны
		float sign_diameter = up ? -diameter : diameter;
		float x, x_min = FLT_INF;
		LineList::iterator i, i_min = lines.end();
		FOR_EACH(lines, i){
			if(x_min > (x = min(i->left, i->right)) && x < x_acceptable){
				x_min = x;
				i_min = i;
			}
		}

		Vect2f pos;
		if(i_min != lines.end()){
			if(i_min->left < i_min->right)
				pos.set(-(i_min->left += diameter), (i_min - lines.begin())*sign_diameter);
			else
				pos.set(i_min->right += diameter, (i_min - lines.begin())*sign_diameter);
		}	
		else{
			lines.push_back(Line(0));
			max_side = max(max_side, (mode != Column ? lines.size() : 0)*diameter); // В режиме колонны не давать расширять в стороны
			pos.set(0, (lines.size() - 1)*sign_diameter);
		}

		return curvate(pos);
	}

	int counter() const { return m_counter; }
	int numLines() const { return lines.size();	}

	Vect2f invert(const Vect2f& pc) const 
	{ 
		Vect2f p = uncurvate(pc);
		float x = fabs(p.x) - radius_*0.2;
		float y_max = -2*radius_;
		LineList::const_iterator i;
		FOR_EACH(lines, i){
			if(x > (p.x < 0 ? i->left : i->right)) 
				break;
			y_max += 2*radius_;
		}
		if(up)
			y_max = -y_max;
		return curvate(Vect2f(-p.x, y_max - p.y));
	}

	void inversion()
	{
		LineList::iterator i;
		FOR_EACH(lines, i)
			swap(i->left,i->right);
	}

	Vect2f curvate(const Vect2f& p) const
	{
		if(curvature_radius)
		{
			float r = curvature_radius + p.y;
			if(r > FLT_EPS)
			{
				float phi = p.x/r;
				//xassert(fabsf(phi) < M_PI && "PositionGenerator: cycling.");
				return Vect2f(r*sinf(phi), r*cos(phi) - curvature_radius);
			}
			//else
			//	xassert(0&&"Position generator: curvature radius_ is too small.");
		}
		return p;
	}

	Vect2f uncurvate(const Vect2f& p) const
	{
		if(curvature_radius)
		{
			float y = p.y + curvature_radius;
			float r = sqrt(sqr(p.x) + sqr(y));
			float phi = atan2(p.x, y);
			return Vect2f(r*phi, r - curvature_radius);
		}
		return p;
	}

	float curvatureRadius() const { return curvature_radius; }

private:
	bool up;
	Mode mode;
	struct Line
	{
		float left, right;
		Line(float border = 0) { left = right = border; }
	};
	typedef vector<Line> LineList;
	LineList lines;
	float max_side;
	int m_counter;
	float radius_;
	float curvature_radius;
};

#endif // _POSITION_GENERATOR_H_
