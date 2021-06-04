
#ifndef __PHYSICS_UTIL_H__
#define __PHYSICS_UTIL_H__

#include <strstream>
#include "statistics.h"
#include "..\Render\Inc\umath.h"

/////////////////////////////////////////////////////////////////////////////////
//		Statistic's Groups
/////////////////////////////////////////////////////////////////////////////////
enum terStatisticsGroupType
{
	STATISTICS_GROUP_OVERALL = -1,
	STATISTICS_GROUP_AI,
	STATISTICS_GROUP_TOTAL,
	STATISTICS_GROUP_LOGIC,
	STATISTICS_GROUP_PHYSICS,
	STATISTICS_GROUP_UNITS,
	STATISTICS_GROUP_NUMERIC
};

/////////////////////////////////////////////////////////////////////////////////
//		Отладочный вывод 3D с кэшированием
/////////////////////////////////////////////////////////////////////////////////
class ShowDispatcher
{
	class Shape {
		enum Type { Point, Text, Circle, Delta, Line, Triangle, Quadrangle, ConvexArray };
		Type type;
		sColor4c color;
		union {
			struct { Vect3f point; float radius; };
			struct { Vect3f pointX; const char* text; };
			struct { Vect3f point1, point2; };
			struct { int n_points; Vect3f* points; };
			};
		static bool isArray(Type type) { return type == Triangle || type == Quadrangle || type == ConvexArray; }
	public:	
		Shape(const Vect3f& v, sColor4c color_) { type = Point; point = v; color = color_; }
		Shape(const Vect3f& v, const char* text_, sColor4c color_) { type = Text; point = v; text = strdup(text_); color = color_; }
		Shape(const Vect3f& v, float radius_, sColor4c color_) { type = Circle; point = v; radius = radius_; color = color_; }
		Shape(const Vect3f& v0, const Vect3f& v1, sColor4c color_, int line) { type = line ? Line : Delta; point1 = v0; point2 = v1; color = color_; }
		Shape(const Vect3f& v0, const Vect3f& v1, const Vect3f& v2, sColor4c color_) { type = Triangle; points = new Vect3f[n_points = 3]; points[0] = v0; points[1] = v1; points[2] = v2; color = color_; }
		Shape(const Vect3f& v0, const Vect3f& v1, const Vect3f& v2, const Vect3f& v3, sColor4c color_) { type = Quadrangle; points = new Vect3f[n_points = 4]; points[0] = v0; points[1] = v1; points[2] = v2; points[3] = v3; color = color_; }
		Shape(int n_points_, const Vect3f* points_, sColor4c color_) { type = ConvexArray; points = new Vect3f[n_points = n_points_]; memcpy(points, points_, sizeof(Vect3f)*n_points); color = color_; }
		Shape(const Shape& shape) 
		{ 
			*this = shape; 
			if(isArray(type)){ 
				points = new Vect3f[n_points = shape.n_points]; 
				memcpy(points, shape.points, sizeof(Vect3f)*n_points); 
			} 
			else if(type == Text)
				text = strdup(shape.text);
		}
		~Shape() { 
			if(isArray(type)) 
				delete points; 
			else if(type == Text) 
				free((void*)text); 
		}
		void show();
		void showConvex();
		};
	typedef vector<Shape> List;
	List shapes;
	bool need_font;

public:
	void draw();
	void clear() { shapes.clear(); need_font = false; }

	void point(const Vect3f& v, sColor4c color) { shapes.push_back(Shape(v, color)); }
	void text(const Vect3f& v, const char* text, sColor4c color) { shapes.push_back(Shape(v, text, color)); need_font = true; }
	void circle(const Vect3f& v, float radius, sColor4c color) { shapes.push_back(Shape(v, radius, color)); }
	void line(const Vect3f &v0, const Vect3f &v1, sColor4c color) { shapes.push_back(Shape(v0, v1, color, 1)); }
	void delta(const Vect3f& v, const Vect3f& dv, sColor4c color) { shapes.push_back(Shape(v, dv, color, 0)); }
	void triangle(const Vect3f &v0, const Vect3f &v1, const Vect3f &v2, sColor4c color) { shapes.push_back(Shape(v0, v1, v2, color)); }
	void quadrangle(const Vect3f &v0, const Vect3f &v1, const Vect3f &v2, const Vect3f &v3, sColor4c color) { shapes.push_back(Shape(v0, v1, v2, v3, color)); }
	void convex(int n_points, const Vect3f* points, sColor4c color) { shapes.push_back(Shape(n_points, points, color));  }
};

extern ShowDispatcher show_dispatcher;

inline void show_vector(const Vect3f& vg, sColor4c color){ show_dispatcher.point(vg, color); }
inline void show_vector(const Vect3f& vg, float radius, sColor4c color){ show_dispatcher.circle(vg, radius, color); }
inline void show_vector(const Vect3f& vg, const Vect3f& delta, sColor4c color){ show_dispatcher.delta(vg, delta, color); }
inline void show_vector(const Vect3f &vg0, const Vect3f &vg1, const Vect3f &vg2, sColor4c color){ show_dispatcher.triangle(vg0, vg1, vg2, color); }
inline void show_vector(const Vect3f &vg0, const Vect3f &vg1, const Vect3f &vg2, const Vect3f &vg3, sColor4c color){ show_dispatcher.quadrangle(vg0, vg1, vg2, vg3, color); }
inline void show_convex(int n_points, const Vect3f* points, sColor4c color){ show_dispatcher.convex(n_points, points, color); }
inline void show_line(const Vect3f &vg0, const Vect3f &vg1, sColor4c color){ show_dispatcher.line(vg0, vg1, color); }
inline void show_text(const Vect3f& vg, const char* text, sColor4c color){ show_dispatcher.text(vg, text, color); }

extern sColor4c 
	 WHITE,
	 RED,
	 GREEN,
	 BLUE,
	 YELLOW,
	 MAGENTA,
	 CYAN;
sColor4c XCOL(sColor4c color, int intensity = 255, int alpha = 255);

/////////////////////////////////////////////////////////////////////////////////
//		Watch system
/////////////////////////////////////////////////////////////////////////////////
void add_watch(const char* var, const char* value);

ostrstream& watch_buffer();
#define watch(var) { watch_buffer().seekp(0); watch_buffer() << var; watch_buffer() << '\0'; add_watch(#var, watch_buffer().str()); }
#define watch_i(var, index) { watch_buffer().seekp(0); watch_buffer() << #var << "[" << index << "]"; watch_buffer() << '\0'; string name(watch_buffer().str()); watch_buffer().seekp(0); watch_buffer() << var; watch_buffer() << '\0'; add_watch(name.c_str(), watch_buffer().str()); }
#define watch_gi(var, index, group) { watch_buffer()().seekp(0); watch_buffer()() << #group << "." << #var << "[" << index << "]"; watch_buffer()() << '\0'; string name(watch_buffer()().str()); watch_buffer()().seekp(0); watch_buffer()() << var; watch_buffer()() << '\0'; add_watch(name.c_str(), watch_buffer()().str()); }
void show_watch();

/////////////////////////////////////////////////////////////////////////////////
//		Determinacy Log
//
//  Log usage:
//
//		log_var(var);  
//		
//	or:	
//
//		#ifndef _FINAL_VERSION_
//		if(log_mode)
//			log_buffer <= var;
//		#endif
//
//	Command line arguments:
//
//	save_log [time_to_exit:seconds]
//	verify_log [append_log] [time_to_exit:seconds]
//
//	_FORCE_NET_LOG_ - to force network log under _FINAL_VERSION_
/////////////////////////////////////////////////////////////////////////////////

//#define _FORCE_NET_LOG_

#if (!defined(_FINAL_VERSION_) || defined(_FORCE_NET_LOG_)) && !defined(_GEOTOOL_)
#define _DO_LOG_
#endif

#ifdef _DO_LOG_
extern int log_mode;
extern XBuffer log_buffer;
extern bool net_log_mode;
extern XBuffer net_log_buffer;
void check_determinacy_quant(bool start);
///#define log_var(var) { if(log_mode){ watch_buffer().seekp(0); watch_buffer() << var; watch_buffer() << '\0'; log_buffer < #var < ": " < watch_buffer().str() < "\n"; } if(net_log_mode){ watch_buffer().seekp(0); watch_buffer() << var; watch_buffer() << '\0'; net_log_buffer < #var < ": " < watch_buffer().str() < "\n"; } }
#define log_var(var) { if(log_mode || net_log_mode) { watch_buffer().seekp(0); watch_buffer() << var; watch_buffer() << '\0'; if(log_mode) { log_buffer < #var < ": " < watch_buffer().str() < "\n"; } if(net_log_mode){ net_log_buffer < #var < ": " < watch_buffer().str() < "\n"; } } }
#define log_var_crc(address, size)	log_var(crc32((const unsigned char*)address, size, startCRC32))
#else
inline void check_determinacy_quant(bool start){}
#define log_var(var)
#define log_var_crc(address, size)	
#endif

/////////////////////////////////////////////////////////////////////////////////
//		Smart Log
/////////////////////////////////////////////////////////////////////////////////
#ifdef _FINAL_VERSION_
	class VoidStream
	{
	public:
		VoidStream(const char* name, unsigned flags){}
		void open(const char* name, unsigned flags){}
		void close(){}
		template<class T> 
		VoidStream& operator< (const T&) { return *this; }
		template<class T> 
		VoidStream& operator<= (const T&) { return *this; }
		template<class T> 
		VoidStream& operator<< (const T&) { return *this; }
	};

	typedef VoidStream LogStream;
#else
	typedef XStream LogStream;
#endif

extern LogStream fout;


/////////////////////////////////////////////////////////////////////////////////
//		Utils
/////////////////////////////////////////////////////////////////////////////////
template<class T> // Для установки параметров
void check_command_line_parameter(const char* switch_str, T& parameter) { const char* s = check_command_line(switch_str); if(s) parameter = atoi(s); }

// Vect2f  -> Vect3f convertions
Vect3f To3D(const Vect2f& pos);
inline Vect3f To3Dzero(const Vect2f& pos) { return Vect3f(pos.x, pos.y, 0); }
inline Vect3f to3D(const Vect2f& pos, float z) { return Vect3f(pos.x, pos.y, z); }


#undef xassert_s
#ifndef _FINAL_VERSION_
#define xassert_s(exp, str) { string s = #exp; s += "\n"; s += str; xxassert(exp,s.c_str()); }
#else
#define xassert_s(exp, str) 
#endif

//--------------------------------------
extern RandomGenerator logicRND;
extern RandomGenerator terEffectRND;

inline int logicRNDi(int x, const char* file, int line)
{
	//log_var(file);
	//log_var(line);
	//log_var(logicRND.get());
	return logicRND(x);
}
inline float logicRNDf(const char* file, int line)
{
	//log_var(file);
	//log_var(line);
	//log_var(logicRND.get());
	return logicRND.frnd();
}
inline float logicRNDfa(const char* file, int line)
{
	//log_var(file);
	//log_var(line);
	//log_var(logicRND.get());
	return logicRND.frand();
}

#define terLogicRND(x) logicRNDi(x, __FILE__, __LINE__)

//-1..+1
#define terLogicRNDfrnd() logicRNDf(__FILE__, __LINE__)

//0..+1
#define terLogicRNDfrand() logicRNDfa(__FILE__, __LINE__)

//--------------------------------------

enum terUnitAttributeID;

#include "..\Util\DebugPrm.h"

#endif // __PHYSICS_UTIL_H__
