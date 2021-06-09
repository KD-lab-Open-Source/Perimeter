
#include "StdAfx.h"
#include "DebugUtil.h"			 
#include "Runtime.h"
#include "CameraManager.h"
#include "terra.h"
#include "../HT/ht.h"
#include "GameShell.h"

LogStream fout("lst",XS_OUT);

sColor4c WHITE(255, 255, 255);
sColor4c RED(255, 0, 0);
sColor4c GREEN(0, 255, 0);
sColor4c BLUE(0, 0, 255);
sColor4c YELLOW(255, 255, 0);
sColor4c MAGENTA(255, 0, 255);
sColor4c CYAN(0, 255, 255);
////////////////////////////////////////////////////////////////////////////////////////////
//		Converter
////////////////////////////////////////////////////////////////////////////////////////////
Vect3f To3D(const Vect2f& pos)
{
	Vect3f p;
	p.x=pos.x;
	p.y=pos.y;

	int x = round(pos.x), y = round(pos.y);
	if(x >= 0 && x < vMap.H_SIZE && y >= 0 && y < vMap.V_SIZE)
		p.z=vMap.GVBuf[vMap.offsetGBuf(x>>kmGrid,y>>kmGrid)];
	else
		p.z=0;
	return p;
}

////////////////////////////////////////////////////////////////////////////////////////////
//			Show System
////////////////////////////////////////////////////////////////////////////////////////////
ShowDispatcher show_dispatcher;

Vect3f G2S(const Vect3f &vg)
{
	Vect3f pv,pe;
	terCamera->GetCamera()->ConvertorWorldToViewPort(&vg,&pv,&pe);
	return Vect3f(pe.x, pe.y, pv.z);
}

sColor4c XCOL(sColor4c color, int intensity, int alpha) 
{ 
	return sColor4c(color.r*intensity >>8,
					color.g*intensity >>8,
					color.b*intensity >>8,
					alpha );
}

void clip_line_3D(const Vect3f &v1,const Vect3f &v2,sColor4c color)
{
	terRenderDevice->DrawLine(v1, v2,color);
}

void clip_pixel(int x1,int y1,sColor4c color, int size)
{
	terRenderDevice -> DrawPixel(x1,y1,color);
	if(size){
		terRenderDevice -> DrawPixel(x1 + 1,y1,color);
		terRenderDevice -> DrawPixel(x1,y1 + 1,color);
		terRenderDevice -> DrawPixel(x1 + 1,y1 + 1,color);

		if(size == 2){
			terRenderDevice -> DrawPixel(x1 - 1,y1,color);
			terRenderDevice -> DrawPixel(x1,y1 - 1,color);
			terRenderDevice -> DrawPixel(x1 - 1,y1 - 1,color);
			terRenderDevice -> DrawPixel(x1 - 1,y1 + 1,color);
			terRenderDevice -> DrawPixel(x1 + 1,y1 - 1,color);
			}
		}
}

void clip_circle_3D(const Vect3f& vc, float radius, sColor4c color)
{
	float segment_length = 3;
	int N = round(2*M_PI*radius/segment_length);
	if(N < 10)
		N = 10;
	float dphi = 2*M_PI/N;
	Vect3f v0 = vc + Vect3f(radius,0,0);
	for(float phi = dphi;phi < 2*M_PI + dphi/2; phi += dphi)
	{
		Vect3f v1 = vc + Vect3f(cos(phi), sin(phi),0)*radius;
		terRenderDevice->DrawLine(v0, v1,color);
		v0 = v1;
	}
}


int intensity_by_dist(float z)
{
	int i = round(255.*(1. - (z - show_vector_zmin)/(show_vector_zmax - show_vector_zmin)));
	if(i < 0)
		i = 0;
	if(i > 255)
		i = 255;
	return i;
}

void ShowDispatcher::Shape::show()
{
	switch(type){
		case Point: {
			Vect3f vs = G2S(point);
			if(vs.z > 10)
				clip_pixel(vs.xi(), vs.yi(), color, 1);
			} break;

		case Text: {
			Vect3f vs = G2S(point);
			terRenderDevice->OutText(vs.xi(), vs.yi(), text, sColor4f(color));
			} break;

		case Circle: 
			clip_circle_3D(point, radius, color);
			break;

		case Delta: 
			clip_line_3D(point1, point1 + point2*show_vector_scale, color);
			break;

		case Line: 
			clip_line_3D(point1, point2, color);
			break;

		case Triangle: 
			clip_line_3D(points[0], points[1], color);
			clip_line_3D(points[1], points[2], color);
			clip_line_3D(points[2], points[0], color);
			break;

		case Quadrangle: 
			clip_line_3D(points[0], points[1], color);
			clip_line_3D(points[1], points[2], color);
			clip_line_3D(points[2], points[3], color);
			clip_line_3D(points[3], points[0], color);
			break;

		case ConvexArray:
			//showConvex();
			xassert(0);
			break;
		
		default:
			xassert(0);
		}
}

void ShowDispatcher::draw()
{
	cFont* font = 0;
	if(need_font){
		font = terVisGeneric->CreateDebugFont();
		terRenderDevice->SetFont(font);
		need_font = false;
	}

	List::iterator i;
	FOR_EACH(shapes, i)
		i -> show();

	if(font){
		terRenderDevice->SetFont(0);
		font->Release();
	}
}


////////////////////////////////////////////////////////////////////////////////////////////
//			Watch System
////////////////////////////////////////////////////////////////////////////////////////////

ostrstream& watch_buffer()
{
	const int buffer_size = 10000;
	static ostrstream buffer(new char[buffer_size], buffer_size);
	return buffer;
}

typedef map<string, string> WatchMap;
static WatchMap watch_map;
void add_watch(const char* var, const char* value)
{
	watch_map[var] = value;
}

void show_watch()
{
	ostrstream text;
	WatchMap::iterator i;
	FOR_EACH(watch_map, i)
		text << i -> first.c_str() << ": " << i -> second.c_str() << endl;
	text << '\0';
	ShowCursor(1);
//	show_debug_window(text.str(), debug_window_sx, debug_window_sy);
	RestoreFocus();
}

#ifdef _DO_LOG_
int log_mode = 0;
static int buffer_size = 200000;
XBuffer log_buffer(buffer_size, 1);

void check_determinacy_quant(bool start)
{
	static int inited = 0;
	static XStream recorder_logging;
	static int time_to_exit = 0;
	static bool second_pass = false;
	static MeasurementTimer timer;

	if(!inited && start){
		inited = 1;
		if(second_pass)
			log_mode = 2;
		else if(check_command_line("save_log") || check_command_line("save_and_verify_log"))
			log_mode = 1;
		else if(check_command_line("verify_log"))
			log_mode = 2;

		const char* str = check_command_line("time_to_exit:");
		if(str)
			time_to_exit = 1000*atoi(str);
		log_buffer.init();
		log_buffer.SetDigits(15);
		recorder_logging.close();
		if(log_mode == 2)
			recorder_logging.open("lst_", XS_IN);
		timer.start();
	}

	if(time_to_exit && timer() > time_to_exit){
		if(check_command_line("save_and_verify_log") && log_mode == 1){
			recorder_logging.close();
			log_mode = 0;
			inited = 0;
			second_pass = true;
			timer.stop();
			recorder_logging.open("lst_", XS_IN);
			HTManager::instance()->setMissionToStart(gameShell->CurrentMission);
		}
		else
			ErrH.Exit();
	}

	if(!log_mode)
		return;

	watch(log_mode);
	watch(timer()/1000);
	watch(time_to_exit/1000);
	
	if(log_mode && log_buffer.tell() > buffer_size/4){
		if(log_mode == 1){
			if(!recorder_logging.GetFileName())
				recorder_logging.open("lst_", XS_OUT);
			recorder_logging.write((const char*)log_buffer, log_buffer.tell());
			}
		else{
			static char* buf = (char*)malloc(buffer_size);
			if(buffer_size < log_buffer.tell())
				buf = (char*)realloc(buf, buffer_size = log_buffer.tell());
			int len = recorder_logging.read(buf, log_buffer.tell());
			if(memcmp(buf, (const char*)log_buffer, len)){
				XStream f0("lst0", XS_OUT);
				XStream f1("lst1", XS_OUT);
				f0.write(buf, len);
				f1.write((const char*)log_buffer, len);
				ErrH.Exit();
				}
			if(len < log_buffer.tell()){ // switch to write mode
				if(!check_command_line("append_log"))
					ErrH.Exit();
				log_mode = 1;
				recorder_logging.close();
				recorder_logging.open("lst_", XS_OUT | XS_APPEND | XS_NOREPLACE);
				recorder_logging.write((const char*)log_buffer + len, log_buffer.tell() - len);
				}
			}
		log_buffer.init();
		}
}
#endif		
