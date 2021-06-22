#include <fstream>
#include <iostream>
#include "xstream.h"
#include "xerrhand.h"

static char *seekMSG	= "BAD SEEK";
static char *flushMSG	= "FLUSH FILE BUFFERS ERROR";
static char *sizeMSG	= "FILE SIZE CALCULATION ERROR";

long XStream::seek(long offset, int dir)
{
    /*
	long ret;

	if(extSize != -1){
		switch(dir){
			case XS_BEG:
				ret = SetFilePointer(handler,extPos + offset,0,dir) - extPos;
				break;
			case XS_END:
				ret = SetFilePointer(handler,extPos + extSize - offset - 1,0,XS_BEG) - extPos;
				break;
			case XS_CUR:
				ret = SetFilePointer(handler,extPos + pos + offset,0,XS_BEG) - extPos;
				break;
		}
	}
	else
		ret = SetFilePointer(handler,offset,0,dir);

	if (ret == -1L) {
        if (ErrHUsed) ErrH.Abort(seekMSG, XERR_USER, GetLastError(), fname);
        else return -1L;
    }
	if (ret >= size() - 1) eofFlag = 1;  else eofFlag = 0;
	return pos = ret;
     */
    
    /* Full stream debug*/
    /*std::fstream debug("openfile.txt", std::ios::out|std::ios::app);
    if (debug.is_open())
        debug<<"SEEK START "<<fname<<" tellg:"<<handler->tellg()
        <<" addr:"<<handler<<" curo:"<<offset<<std::endl;
    debug.close();*/

    long ret=0;
    if(extSize != -1){
        switch (dir) {
            case XS_BEG:
                //ret = SetFilePointer(handler,extPos + offset,0,dir) - extPos;
                if (handler->flags() & std::ios::out) {
                    ret = handler->tellp() - (std::streamoff)extPos;
                    handler->seekp(extPos + offset, std::ios_base::beg);
                }
                else {
                    ret = handler->tellg() - (std::streamoff)extPos;
                    handler->seekg(extPos + offset, std::ios_base::beg);
                }
                break;
            case XS_END:
                //ret = SetFilePointer(handler,extPos + extSize - offset - 1,0,XS_BEG) - extPos;
                if (handler->flags() & std::ios::out) {
                    ret = handler->tellp() - (std::streamoff)extPos;
                    handler->seekp(extPos + extSize - offset - 1, std::ios_base::beg);
                } else {
                    ret = handler->tellg() - (std::streamoff)extPos;
                    handler->seekg(extPos + extSize - offset - 1, std::ios_base::beg);
                }
                break;
            case XS_CUR:
                //ret = SetFilePointer(handler,extPos + pos + offset,0,XS_BEG) - extPos;
                //handler->clear();
                if (handler->flags() & std::ios::out) {
                    ret = handler->tellp() - (std::streamoff)extPos;
                    handler->seekp(extPos + pos + offset, std::ios_base::beg);
                } else {
                    ret = handler->tellg() - (std::streamoff)extPos;
                    handler->seekg(extPos + pos + offset, std::ios_base::beg);
                }
                break;
            default:
                break;
        }
    }
    else
    {
        //ret = SetFilePointer(handler,offset,0,dir);
        //std::cout<<"SEEK:"<<fname<<std::endl;
        switch (dir) {
            case XS_BEG:
                if (handler->flags() & std::ios::out)
                    handler->seekp(offset, std::ios_base::beg);
                else
                    handler->seekg(offset, std::ios_base::beg);
                break;
            case XS_END:
                if (handler->flags() & std::ios::out)
                    handler->seekp(offset, std::ios_base::end);
                else
                    handler->seekg(offset, std::ios_base::end);
                break;
            case XS_CUR:
                if (handler->flags() & std::ios::out)
                    handler->seekp(offset, std::ios_base::cur);
                else
                    handler->seekg(offset, std::ios_base::cur);
                break;
            default:
                break;
        }
        if (handler->flags() & std::ios::out)
            ret = handler->tellp();
        else
            ret = handler->tellg();

    }
    /* Full stream debug*/
    /*debug.open("openfile.txt", std::ios::out|std::ios::app);
    if (debug.is_open())
        debug<<"SEEK "<<fname<<" offset:"<<offset<<" dir:"<<dir
        <<" extSize:"<<extSize<<" pos:"<<pos<<" extPos:"<<extPos
        <<" ret:"<<ret<<std::endl;
    debug.close();*/
    if (handler->fail()) {
        fprintf(stderr, "Warning: Bad seek in file.");
        if (ErrHUsed) ErrH.Abort(seekMSG, XERR_USER, ret, fname.c_str());
        else return -1L;
    }
	if (ret >= size() - 1) eofFlag = 1;  else eofFlag = 0;
	return pos = ret;
}

void XStream::flush()
{
    /* Full stream debug
    std::fstream debug("openfile.txt", std::ios::out|std::ios::app);
    if (debug.is_open())
        debug<<"FLUSH "<<fname<<std::endl;
    debug.close();
    */
    //if (!FlushFileBuffers(handler) && ErrHUsed)
    //	ErrH.Abort(flushMSG,XERR_USER,GetLastError(),fname);
    handler->flush();
}

long XStream::size() const
{
    /*
	long tmp = extSize;
	
	if(tmp == -1){
		tmp=GetFileSize(handler,0);
		if (tmp == -1L) {
            if (ErrHUsed) ErrH.Abort(sizeMSG, XERR_USER, GetLastError(), fname);
            else return -1;
        }
	}
     */
    
    //std::cout<<"XStream::size()"<<std::endl;
    long tmp = extSize;
    long int tmp2;
    if(tmp == -1){
        //tmp=GetFileSize(handler,0);
        if (handler->flags() & std::ios::in) {
            tmp2 = handler->tellg();
            handler->seekp(0, std::ios_base::end);
            tmp = handler->tellg();
            handler->seekp(tmp2);
        } else {
            tmp2 = handler->tellp();
            handler->seekg(0, std::ios_base::end);
            tmp = handler->tellp();
            handler->seekg(tmp2);
        }
        if (tmp == -1L) {
            if (ErrHUsed)
                ErrH.Abort(sizeMSG,XERR_USER,0,fname.c_str());
            else
                return -1;
        }
    }
    /* Full stream debug
    std::fstream debug("openfile.txt", std::ios::out|std::ios::app);
    if (debug.is_open())
        debug<<"SIZE "<<fname<<" size:"<<tmp<<std::endl;
    debug.close();
    */
    return tmp;
}

