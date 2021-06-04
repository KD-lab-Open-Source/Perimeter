/********************************************************************
	created:	2003/09/25
	created:	25:9:2003   18:07
	filename: 	F:\PERIMETER\TriggerEditor\gdi_resource.h
	file path:	F:\PERIMETER\TriggerEditor
	file base:	gdi_resource
	file ext:	h
	author:		Илюха
	
	purpose:	Класс, ухаживающий за gdi ресурcами
*********************************************************************/
#ifndef GDI_RESOURCE_HDR
#define GDI_RESOURCE_HDR

typedef int (WINAPI *GDIDeleter)(HGDIOBJ);
template<class GDI_RESOURCE, class Deleter = GDIDeleter>
class gdi_resource_guard
{
public:
	gdi_resource_guard(GDI_RESOURCE hr, Deleter del = DeleteObject):
				gdi_resource_(hr)
			  , deleter_(del){}
	~gdi_resource_guard(){
		deleter_(gdi_resource_);
	}
	GDI_RESOURCE get() const{
		return gdi_resource_;
	}
	GDI_RESOURCE release(){
		GDI_RESOURCE gr = gdi_resource_;
		return gdi_resource_;
	}
	void reset(GDI_RESOURCE gr){
		deleter_(gdi_resource_);
		gdi_resource_ = gr;
	}
	typedef GDI_RESOURCE gdi_resource_guard::*unspecified_bool;
	operator unspecified_bool() const{
		return gdi_resource_? &gdi_resource_guard::gdi_resource_: NULL;
	}
	GDI_RESOURCE operator*() const{
		return gdi_resource_;
	}
private:
	GDI_RESOURCE gdi_resource_;
	Deleter deleter_;
};

#endif