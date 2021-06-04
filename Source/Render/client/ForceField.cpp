#include "StdAfxRD.h"
#include "ForceField.h"
//#include "..\Util\ProTool.h"
#include "..\src\Scene.h"

//#include "PrmEdit.h"
#include "Scripts\ForceField.hi"
#include "Scripts\ForceField.cppi"

FieldDispatcher* field_dispatcher = NULL;

void FieldDispatcher::Cell::clear()
{
	field = 0;
	height_initial = FieldCluster::ZeroGround;
	velocity = 0;
	cluster = 0;
	
	specify_delta.set(0, 0);
	specify_error = max_error;
}

void FieldDispatcher::Cell::specify(const Vect2i& delta, const Vect2i& tangenta, const FieldCluster* cluster_)
{
	if(cluster != cluster_)
		return;
	int d = delta.norm2();
	if(specify_error > d){
	   specify_error = d;
	   specify_delta = delta;
	   specify_normal.set(tangenta.y, -tangenta.x, 0);
	   specify_normal.Normalize();
	}
}

void FieldDispatcher::Cell::integrate(float& height)
{
	//  Vz *= damping
	//   z += Vz
	float k = force_field_damping - sqr(velocity)*force_field_damping2;
	if(k < force_field_damping_min)
		k = force_field_damping_min;
	velocity *= k;
	height += velocity*force_field_time_step;
}


//--------------------------------------

FieldDispatcher::FieldDispatcher(int xmax, int ymax, int zeroLayerHeight) 
: cIUnkObj(KIND_FORCEFIELD),
map_(xmax, ymax, Cell(zeroLayerHeight)),
map_height(xmax, ymax, zeroLayerHeight), 
tile_map_(xmax, ymax, 0)
{
	FieldCluster::ZeroGround = zeroLayerHeight;
	inv_scale_shl=1.0f/float(1<<scale);

	tile_global = 0;

	pDrawData = 0;
	int i;
	for(i = 0; i < 2; i++)
		Texture[i] = 0;
	Frame = new cFrame;

	gb_RenderDevice->CreateFFDData(this);
	prev_real_time = -1;

	hmap_index=2;
	for(i=0;i<3;i++)
	{
		hmap[i]=new HMap(xmax,ymax,float(zeroLayerHeight));
	}
	hmapRotate();
	MTINIT(hmap_lock);
	interpolation_factor=0;
}

FieldDispatcher::~FieldDispatcher()
{
	MTDONE(hmap_lock);
	gb_RenderDevice->DeleteFFDData(this);

	int i;
	for(i=0;i<2;i++)
		if(Texture[i]){ 
			Texture[i]->Release(); 
			Texture[i]=0; 
		}

	for(i=0;i<3;i++)
	{
		delete hmap[i];
	}

	if(Frame)
		delete Frame; 
}

void FieldDispatcher::hmapRotate()
{
	interpolation_factor=0;
	hmap_index=(hmap_index+1)%3;
	hmap_cur=hmap[hmap_index];
	hmap_prev=hmap[(hmap_index+1)%3];
	hmap_logic=hmap[(hmap_index+2)%3];
}


void FieldDispatcher::logicQuant()
{
	MTL();
	ClusterList::iterator ki;
	FOR_EACH(clusters, ki)
		ki->logicQuant();

	evolveField();

	const float* in=map_height.map();
	float* out=hmap_logic->map();
	for(int y=0;y<map_.sizeY();y++)
	{
		int index=y*map_.sizeX();
		int index_max=index+map_.sizeX();
		for(;index<index_max;index++)
		{
			out[index]=in[index];
		}
	}

	MTEnter lock(hmap_lock);
	hmapRotate();
}

void FieldDispatcher::SetTexture(int n,cTexture *pTexture)
{
	RELEASE(Texture[n]);
	Texture[n]=pTexture;
}

void FieldDispatcher::PreDraw(cCamera *DrawNode)
{
	if(GetAttribute(ATTRUNKOBJ_IGNORE)) return;
	DrawNode->Attach(SCENENODE_OBJECT,this);
	DrawNode->Attach(SCENENODE_OBJECTSPECIAL2,this);
	
}

void FieldDispatcher::Draw(cCamera *DrawNode)
{
	DrawNode->GetRenderDevice()->Draw(this);
	MTEnter lock(hmap_lock);
	static double prev_time=0;
	double time=clockf();
	double dt=time-prev_time;
	prev_time=time;

	interpolation_factor+=/*IParent->GetDTime()*/ dt*gb_VisGeneric->GetLogicTimePeriodInv();
	xassert(interpolation_factor>=0);
	if(interpolation_factor>1)
		interpolation_factor=1;
}

void FieldDispatcher::Animate(float dt)
{
	GetFrame()->AddPhase(dt);

//	double t = clockf();
//	if(prev_real_time >= 0)
//		GetFrame()->AddPhase(t-prev_real_time);
//	prev_real_time = t;

	ClusterList::iterator it;
	FOR_EACH(clusters,it)
		(*it).Animate(dt);
}

void FieldDispatcher::UpdateTile()
{
	tile_global = 0;
	for(int yt = 0; yt < tileMapSizeY(); yt++)
		for(int xt = 0; xt < tileMapSizeX(); xt++){
			BYTE& cur_tile = tile_map_(xt, yt);
			cur_tile = 0;
			int tile_size = t2m(1);
			int x_begin = t2m(xt);
			int y_begin = t2m(yt);
			for(int y = 0; y <= tile_size; y++)
				for(int x = 0; x <= tile_size; x++){
					int xm = clamp(x + x_begin, 1, mapSizeX() - 2);
					int ym = clamp(y + y_begin, 1, mapSizeY() - 2);
					if(height_initial(xm, ym) > FieldCluster::ZeroGround){
						FieldCluster* p = getCluster(xm, ym);
						if(p)
							cur_tile |= p->GetTT();
					}
					
					tile_global |= cur_tile;
				}
		}
}

void FieldDispatcher::setBorder(FieldCluster* cluster, const Vect2sVect& border)
{
	clearCluster(cluster);
	cluster->clear();

	vector<FieldInterval> intervals;
	Vect2sVect::const_iterator i;
	FOR_EACH(border, i)
	{
		Vect2sVect::const_iterator i1 = i + 1 != border.end() ? i + 1 : border.begin();
		if(i1->y < i->y)
		{ // Скачек вверх 
			Vect2sVect::const_iterator k;
			Vect2sVect::const_iterator k_best = border.end();
			Vect2sVect::const_iterator k0 = border.end() - 1;
			int d, dist = 0x7fffffff;
			FOR_EACH(border, k)
			{ // Находим ближайший скачек вниз справа
				if(i->y == k->y && k0->y < k->y && (d = k->x - i->x) > 0 && d < dist)
				{
					k_best = k;
					dist = d;
				}
				k0 = k;
			}

			//xassert(k_best != border.end());

			if(k_best == border.end()) // могут теряться 1-интервалы
				continue;

			intervals.push_back(FieldInterval(i->x, k_best->x, i->y));
		}
	}

	FieldCluster::iterator ii;
	FOR_EACH(intervals, ii)
	{
		int xl = clamp(w2m(ii->xl), 1, map_.sizeX() - 2);
		int xr = clamp(w2m(ii->xr), 1, map_.sizeX() - 2);
		int y = clamp(w2m(ii->y), 1, map_.sizeY() - 2);

		int extended = 0;
		FieldCluster::iterator ik;
		FOR_EACH(*cluster, ik)
		if(y == ik->y && !(xr < ik->xl || xl > ik->xr)){
			ik->xl = min(ik->xl, xl);
			ik->xr = max(ik->xr, xr);
			extended = 1;
			break;
		}

		if(!extended)
			cluster->push_back(FieldInterval(xl, xr, y));
	}

	calcHeight(cluster);

	Vect2sVect::const_iterator pi;
	FOR_EACH(border, pi){
		Vect2i vm(w2m(pi->x), w2m(pi->y));
		Vect2i delta = Vect2i(*pi) - Vect2i(m2w(vm.x), m2w(vm.y));
		int i = pi - border.begin();
		Vect2i tangenta = border[(i + force_field_tangeta_length) % border.size()] - border[(i - force_field_tangeta_length + border.size()) % border.size()];
		if(delta.x < half){
			if(delta.y < half)
				map(vm.x, vm.y).specify(delta, tangenta, cluster);
			else
				map(vm.x, vm.y + 1).specify(Vect2i(delta.x, delta.y - half), tangenta, cluster);
		}
		else
			if(delta.y < half)
				map(vm.x + 1, vm.y).specify(Vect2i(delta.x - half, delta.y), tangenta, cluster);
			else
				map(vm.x + 1, vm.y + 1).specify(Vect2i(delta.x - half, delta.y - half), tangenta, cluster);
	}
}

void FieldDispatcher::calcHeight(FieldCluster* cluster)
{
	FieldCluster::iterator i;
	FOR_EACH(*cluster, i)
		for(int x = i->xl; x <= i->xr; x++)
			map(x, i->y).field = POTENTIAL;

	for(int j = 0; j < height_field_iterations; j++){
		FOR_EACH(*cluster, i){
			int y = i->y;
			for(int x = i->xl; x <= i->xr; x++)
				map(x, y).field = (map(x - 1, y).field + map(x + 1, y).field + map(x, y - 1).field + map(x, y + 1).field)*0.25f;
		}
	}

	FOR_EACH(*cluster, i){
		int y = i->y;
		for(int x = i->xl; x <= i->xr; x++){
			Cell& c = map(x, y);
			c.height_initial = FieldCluster::ZeroGround + force_field_height*c.field;
			map_height(x,y) = FieldCluster::ZeroGround - 1;
			c.velocity = 0;
			c.cluster = cluster;
		}
	}

	UpdateTile();
}

void FieldDispatcher::clearCluster(FieldCluster* cluster)
{
	FieldCluster::iterator i;
	FOR_EACH(*cluster, i){
		int y = i->y;
		for(int x = i->xl; x <= i->xr; x++)
			map(x, y).clear();
	}
	
	UpdateTile();
}

void FieldDispatcher::removeCluster(FieldCluster* cluster) 
{ 
	ClusterList::iterator i; 
	FOR_EACH(clusters, i) 
	if(&*i == cluster)
	{
		clearCluster(cluster);
		clusters.erase(i);
		break;
	}
}

void FieldDispatcher::evolveField()
{
//	start_timer_auto(evolveField, 1);

	for(int i = 0; i < evolve_field_iterations; i++){
		ClusterList::iterator ki;
		FOR_EACH(clusters, ki){
			if(!ki->started_logic())
				continue;
			//  Heights elastic evolution
			//  Vz -= k_elasticity*(z - z_avr)
			float dz_factor = force_field_stiffness*force_field_time_step;
			FieldCluster::iterator i;
			FOR_EACH(*ki, i){
				int y = i->y;
				for(int x = i->xl; x <= i->xr; x++){
					float dz = dz_factor*(height(x, y) - height_initial(x, y));
					map(x, y).velocity -= dz;
					dz *= force_field_spawn_factor;
					map(x - 1, y).velocity += dz;
					map(x + 1, y).velocity += dz;
					map(x, y - 1).velocity += dz;
					map(x, y + 1).velocity += dz;
				}
			}

			//  Vz *= damping
			//   z += Vz
			FOR_EACH(*ki, i){
				int y = i->y;
				for(int x = i->xl; x <= i->xr; x++)
					map(x, y).integrate(map_height(x,y));
			}
		}
	}
}

int FieldDispatcher::getIncludingCluster(const Vect3f& r)
{ 
/*
	float x = r.x/(1 << scale);
	float y = r.y/(1 << scale);

	float xf = floorf(x);
	float yf = floorf(y);
	
	int xi = clamp(round(xf), 1, mapSizeX() - 2);
	int yi = clamp(round(yf), 1, mapSizeY() - 2);
	
	float dx = x - xf;
	float dy = y - yf;
	float t5 = 1.0f - dy;
	float z = (height_initial(xi + 1, yi)*t5 + height_initial(xi + 1, yi + 1)*dy)*dx + (height_initial(xi, yi)*t5 + height_initial(xi, yi + 1)*dy)*(1.0f - dx);
	if(z > r.z) 
		return max(max(attribute(xi + 1, yi),attribute(xi + 1, yi + 1)), max(attribute(xi, yi), attribute(xi, yi + 1)));
	return 0;
/*/
  	float x = r.x*inv_scale_shl;
	float y = r.y*inv_scale_shl;

	int xi=round(x-0.5f);
	int yi=round(y-0.5f);
	float dx = x - xi;
	float dy = y - yi;
	
	xi = clamp(xi, 1, mapSizeX() - 2);
	yi = clamp(yi, 1, mapSizeY() - 2);
	
	float t5 = 1.0f - dy;
	float z = (height(xi + 1, yi)*t5 + height(xi + 1, yi + 1)*dy)*dx + (height(xi, yi)*t5 + height_initial(xi, yi + 1)*dy)*(1.0f - dx);
	if(z > r.z) 
		return max(max(attribute(xi + 1, yi),attribute(xi + 1, yi + 1)), max(attribute(xi, yi), attribute(xi, yi + 1)));
	return 0;
/**/
}

float FieldDispatcher::getGraphZ(float r_x,float r_y)
{
	float t=gb_VisGeneric->GetInterpolationFactor();
	float t_=1-t;
	float x = r_x*inv_scale_shl;
	float y = r_y*inv_scale_shl;

	int xi=round(x-0.5f);
	int yi=round(y-0.5f);
	float dx = x - xi;
	float dy = y - yi;
	
	xi = clamp(xi, 1, mapSizeX() - 2);
	yi = clamp(yi, 1, mapSizeY() - 2);
	
	float t5 = 1.0f - dy;
	float z = (interpolateHeight(xi + 1, yi,t,t_)*t5 + interpolateHeight(xi + 1, yi + 1,t,t_)*dy)*dx + 
			  (interpolateHeight(xi, yi,t,t_)*t5 + interpolateHeight(xi, yi + 1,t,t_)*dy)*(1.0f - dx);
	return z;
}


bool FieldDispatcher::checkPlace(const Vect2f& pos, const Vect2f& delta)
{
	Vect2i p = w2m(pos);
	Vect2i d = w2m(delta);

	int attr = attribute(p.x, p.y);								
	for(int y = p.y - d.y; y <= p.y + d.y; y++)
		for(int x = p.x - d.x; x <= p.x + d.x; x++)
			if(attribute(x, y) != attr || // другой атрибут
				attr && map(x, y).height_initial < FieldCluster::ZeroGround + force_field_check_place_height) // слишком низко под полем
					return false;
	return true;
}

Vect2f FieldDispatcher::findPlace(const Vect2f& pos0, const Vect2f& delta)
{
	if(checkPlace(pos0, delta))
		return pos0;

	Vect2f pos = pos0;
	Vect2f dx = Vect2f(delta.x, 0);
	Vect2f dy = Vect2f(0, delta.y);
	Vect2f dd;
	int i;
	for(i = 1; ;i++)
	{
		if(checkPlace(pos + dx*i, delta))
		{
			dd = dx;
			break;
		}

		if(checkPlace(pos - dx*i, delta))
		{
			dd = -dx;
			break;
		}

		if(checkPlace(pos + dy*i, delta))
		{
			dd = dy;
			break;
		}

		if(checkPlace(pos - dy*i, delta))
		{
			dd = -dy;
			break;
		}

		xassert(i < 10);
	}

	pos += dd*i;
	for(i = 0; i < 4; i++)
	{
		dd /= 2;
		if(checkPlace(pos - dd, delta))
			pos -= dd;
	}

	return pos;
}

float ray_triangle_intersection(const Vect3f points[3], const Vect3f& origin, const Vect3f& direction)
{
	const float eps = 0.01f;

	Vect3f d1 = points[1] - points[0];
	Vect3f d2 = points[2] - points[0];
	Vect3f b = origin - points[0];

	float t41 = d1[1];
	float t40 = d1[2];
	float t38 = d2[1];
	float t37 = d2[2];
	float t51 = -t40*t38+t37*t41;
	float t42 = d1[0];
	float t39 = d2[0];
	float t50 = -t39*t41+t42*t38;
	float t49 = t39*t40-t42*t37;
	float t48 = direction[0];
	float t47 = direction[1];
	float t46 = direction[2];
	float t45 = b[0];
	float t44 = b[1];
	float t43 = b[2];
	float t36 = t51*t48+t50*t46+t49*t47;
	if(fabs(t36) < FLT_EPS)
		return FLT_INF;
	t36 = 1/t36;
	float t2 = -((t40*t44-t43*t41)*t48+(t45*t41-t42*t44)*t46+(t42*t43-t45*t40)*t47)*t36;
	float t1 = ((-t38*t43+t44*t37)*t48+(-t44*t39+t38*t45)*t46+(-t37*t45+t43*t39)*t47)*t36;
	float t = -(t50*t43+t49*t44+t51*t45)*t36;
	if(t < 0 || t1 < -eps || t1 > 1 + eps || t2 < -eps || t2 > 1 + eps)
		return FLT_INF;
	return t;
}

int FieldDispatcher::ray_cell_intersection(int x, int y, float t_zero, const Vect3f& origin, const Vect3f& direction, Vect3f& intersection, Vect3f& normal_)
{
	Vect3f points[3] = { Vect3f(m2w(x), m2w(y + 1), height_initial(x, y + 1)), Vect3f(m2w(x + 1), m2w(y), height_initial(x + 1, y)), Vect3f(m2w(x), m2w(y), height_initial(x, y)) };
	float t = min(ray_triangle_intersection(points, origin, direction), t_zero);
	points[2] = Vect3f(m2w(x + 1), m2w(y + 1), height_initial(x + 1, y + 1));
	t = min(ray_triangle_intersection(points, origin, direction), t_zero);
	if(t < 1){
		intersection.scaleAdd(origin, direction, t);
		normal_ = t != t_zero ? normal(x, y) : Vect3f::K;
		return 1;
		}
	return 0;
}

int FieldDispatcher::castRay(const Vect3f& origin, const Vect3f& direction_or_point, Vect3f& intersection, Vect3f& normal)
{
	//xassert(getIncludingCluster(origin) && origin.z > FieldCluster::ZeroGround);
	if(!getIncludingCluster(origin) || origin.z < FieldCluster::ZeroGround)
		return 0;

	Vect3f direction = direction_or_point;
	float t_zero = FLT_INF;
	if(direction.norm() < 2){
		if(!getIncludingCluster(origin) || origin.z < FieldCluster::ZeroGround)
			return 0;
		direction.scale(1000.);
		t_zero = direction.z < -FLT_EPS ? (FieldCluster::ZeroGround - origin.z)/direction.z : FLT_INF;
		}
	else {
		xassert(0 && "передавай единичное направление");
		direction -= origin;
		}

	int x1 = w2m(origin.xi());
	int y1 = w2m(origin.yi());

	Vect3f final = origin + direction;
	
	int x2 = w2m(final.xi());
	int y2 = w2m(final.yi());

	if(x1 == x2 && y1 == y2)
		return ray_cell_intersection(x1, y1, t_zero, origin, direction, intersection, normal);

	const int F_PREC = 16;
	if(abs(x2 - x1) > abs(y2 - y1)){
		int a = x2 - x1;
		int b = y2 - y1;
		int x = x1;
		int y = (y1 << F_PREC) + (1 << F_PREC - 1);
		int incr = 1;
		int k = (b << F_PREC)/a;
		if(x1 > x2){
			incr = -1;
			k = -k;
			a = -a;
			}
		do{
			if(ray_cell_intersection(x, y >> F_PREC, t_zero, origin, direction, intersection, normal))
				return 1;
			if(ray_cell_intersection(x, (y >> F_PREC) + 1, t_zero, origin, direction, intersection, normal))
				return 1;
			if(ray_cell_intersection(x, (y >> F_PREC) - 1, t_zero, origin, direction, intersection, normal))
				return 1;
			x += incr;
			y += k;
			} while(--a >= 0);
		}
	else{
		int a = x2 - x1;
		int b = y2 - y1;
		int x = (x1 << F_PREC) + (1 << F_PREC - 1);
		int y = y1;
		int incr = 1;
		int k = (a << F_PREC)/b;
		if(y1 > y2){
			incr = -1;
			k = -k;
			b = -b;
			}
		do{
			if(ray_cell_intersection(x >> F_PREC, y, t_zero, origin, direction, intersection, normal))
				return 1;
			if(ray_cell_intersection((x >> F_PREC) + 1, y, t_zero, origin, direction, intersection, normal))
				return 1;
			if(ray_cell_intersection((x >> F_PREC) - 1, y, t_zero, origin, direction, intersection, normal))
				return 1;
			y += incr;
			x += k;
			} while(--b >= 0);
		}

	return 0;
}

int FieldDispatcher::calcPenalty(const Vect3f& center, float radius, float feedback_factor, Vect3f& r, Vect3f&f)
{
	int xc = w2m(center.xi());
	int yc = w2m(center.yi());
	int D = (round(radius) >> scale) + 1;
	float d_best = FLT_INF;
	int x_best, y_best;
	const Vect3f* n_best;
	for(int y = yc - D; y <= yc + D; y++)
		for(int x = xc - D; x <= xc + D; x++){
			const Vect3f& n = normal(x, y);
			float d = n.x*(center.x - m2w(x)) + n.y*(center.y - m2w(y)) + n.z*(center.z - height(x, y));
			if(fabs(d_best) > fabs(d)){
				d_best = d;
				x_best = x;
				y_best = y;
				n_best = &n;
				}
			}
	if(fabs(d_best) < radius){
		f = *n_best*(d_best*force_field_penalty_stiffness);
		r.set(m2w(x_best), m2w(y_best), height(x_best, y_best));
		return 1;
		}
	return 0;
}

void FieldDispatcher::switchON(FieldCluster* cluster)
{
	cluster->switchON();
	//calcHeight(cluster);
}

void FieldDispatcher::switchOFF(FieldCluster* cluster)
{
	cluster->switchOFF();
	//clearCluster(cluster);//temp
}

void FieldDispatcher::switchDELETE(FieldCluster* cluster)
{
	clearCluster(cluster);
}


///////////////////////////////////////////////////////////////////////////
//		FieldCluster
///////////////////////////////////////////////////////////////////////////
float FieldCluster::ZeroGround = 0;
bool FieldCluster::enable_transparency_ = true;

FieldCluster::FieldCluster() 
: player_id(0), cluster_id(0)
{
	is_transparent = true;
	SetColor(sColor4c(255,255,255,255));
	fade_timer = 0;
	fade_timer_logic = 1000;
	ttype = TT_TRANSPARENT_ADD;
	animate_type = FIELD_STOPPED;
	animate_type_logic = FIELD_STOPPED;
}

void FieldCluster::clear() 
{ 
	vector<FieldInterval>::clear(); 
}

int FieldCluster::inside(int x, int y)
{
	iterator i;
	FOR_EACH(*this, i)
		if(y == i->y && x >= i->xl && x <= i->xr)
			return 1;
	return 0;
}


void FieldCluster::switchON()
{
	MTL();
	animate_type_logic=FIELD_STARTED;
	fade_timer_logic = 0;
	if(animate_type == FIELD_STOPPED || animate_type == FIELD_STOPPING){
		fade_timer = 0;
		animate_type = FIELD_STARTING;
	}
}

void FieldCluster::switchOFF()
{
	MTL();
	animate_type_logic=FIELD_STOPPED;
	fade_timer_logic = 0;
	if(animate_type == FIELD_STARTING || animate_type == FIELD_STARTED){
		fade_timer = 0;
		animate_type = FIELD_STOPPING;
	}
}

void FieldCluster::Animate(float dt)
{
	fade_timer += dt;

	float phase = clamp(fade_timer/fade_duration, 0, 1);


	switch(animate_type){
	case FIELD_STARTING:
		CurrentDiffuse.a = round(Diffuse.a*phase);
		if(phase >= 1)
			animate_type = FIELD_STARTED;
		break;
	case FIELD_STARTED:
		CurrentDiffuse = Diffuse;
		break;
	case FIELD_STOPPING:
		CurrentDiffuse.a = round(Diffuse.a*(1 - phase));
		if(phase >= 1)
			animate_type = FIELD_STOPPED;
		break;
	}

	if(isTransparent())
		ttype = TT_TRANSPARENT_ADD;
	else{
		if(animate_type != FIELD_STOPPED)
			ttype = TT_TRANSPARENT_MOD;
		else
			ttype = TT_OPAQUE;
	}
}

void FieldCluster::logicQuant()
{
	if(animate_type_logic==FIELD_STOPPED)
	{
		float fade_logic_duration=fade_duration*1e-2;
		if(fade_timer_logic<fade_logic_duration && fade_timer_logic+1>=fade_logic_duration)
		{
			field_dispatcher->clearCluster(this);
		}

		fade_timer_logic+=1;
	}
}
