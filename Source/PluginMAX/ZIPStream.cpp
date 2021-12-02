
#include "tweaks.h"

#include "ZIPStream.h"
#include "files/files.h"

#ifndef _NOZIP
#include "zip_resource.h"
static XZIP_Resource* pzip=NULL;
#endif //_NOZIP

bool ZIPIsOpen()
{
#ifndef _NOZIP
	return pzip!=NULL;
#else
	return false;
#endif //_NOZIP
}

void ZIPOpen(const char* zip_filename)
{
#ifndef _NOZIP
	ZIPClose();
	pzip = new XZIP_Resource(
        convert_path_content(zip_filename).c_str(),
        XZIP_ENABLE_ZIP_HEADERS
    );
#endif //_NOZIP
}

void ZIPClose()
{
#ifndef _NOZIP
	delete pzip;
	pzip=NULL;
#endif //_NOZIP
}

ZIPStream::ZIPStream() : XStream(0) {
}

int	ZIPStream::open(const char* name, unsigned f) {
    if (!name) return 0;
    std::string path = convert_path_content(name);
    if (path.empty()) path = convert_path_native(name);
    bool b_open = XStream::open(path, f) != 0;
#ifndef _NOZIP
	if(pzip && !b_open) {
        std::string zippath = convert_path_posix(name);
        _strlwr(zippath.data());
        b_open = pzip->open(zippath, *this) != 0;
	}
#endif //_NOZIP
    
    return b_open ? 1 : 0;
}
