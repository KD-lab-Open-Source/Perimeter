
#include "StdAfx.h"

#include "ConnectionDP.h"
#include "EventBufferDP.h"

#include "P2P_interface.h"

#ifndef _FINAL_VERSION_
#define xassertStr2(exp, str) { string s = #exp; s += "\n"; s += str; xxassert(exp,s.c_str()); }
#else
#define xassertStr2(exp, str) 
#endif


LogStream netLog("PerimeterGame.log",XS_OUT);

/***********************************************************************
				Input Event Buffer
***********************************************************************/
/*
InputEventBuffer::InputEventBuffer(unsigned int size)
: XBuffer(size) 
{
	reset();
	event_ID = NETCOM_ID_NONE;
	byte_receive=0;
}

void InputEventBuffer::reset()
{
	next_event_pointer = 0;
	filled_size = 0;
	offset = 0;
}

int InputEventBuffer::receive(XDPConnection& conn, DPNID dpnid)
{
	if(next_event_pointer != tell())
		ErrH.Abort("Incorrect events reading");

//	if(next_event_pointer){
//		if(filled_size != next_event_pointer)
//			memmove(address(),address() + next_event_pointer, filled_size - next_event_pointer);
//		filled_size -= next_event_pointer;
//		offset = next_event_pointer = 0;
//	}
	clearBufferOfTheProcessedCommands();

	int add_size = conn.Receive((char*)address() + filled_size,length() - filled_size, dpnid);
	byte_receive+=add_size;
	filled_size +=add_size;
	next_event();
	return add_size;
}

void InputEventBuffer::clearBufferOfTheProcessedCommands(void)
{
	if(next_event_pointer){
		if(filled_size != next_event_pointer)
			memmove(address(),address() + next_event_pointer, filled_size - next_event_pointer);
		filled_size -= next_event_pointer;
		offset = next_event_pointer = 0;
	}
}

bool InputEventBuffer::putInputBufferInputPacket(unsigned char* buf, unsigned int size)
{
	clearBufferOfTheProcessedCommands();
	if(length()-filled_size < size) {
		xassert("Net input buffer is small.");
		return 0;
	}
	memcpy((char*)address() + filled_size, buf, size);
	byte_receive+=size;
	filled_size +=size;
	next_event();
	return 1;
}

int InputEventBuffer::current_event()
{
	if(event_ID == NETCOM_ID_NONE){
		clearBufferOfTheProcessedCommands();
		return next_event();
	}
	return event_ID; 
} 

terEventID InputEventBuffer::next_event()
{
	event_ID = NETCOM_ID_NONE;

	if(next_event_pointer + sizeof(size_of_event) > filled_size)
		return NETCOM_ID_NONE;

	if(next_event_pointer != tell())
		ErrH.Abort("Incorrect events reading");
	
	read(&size_of_event, sizeof(size_of_event)); //get_short();
	unsigned int new_pointer = next_event_pointer + size_of_event + sizeof(size_of_event);
	if(new_pointer > filled_size){
		set(next_event_pointer);
		return NETCOM_ID_NONE;
		}

	next_event_pointer = new_pointer;
	
	//return (event_ID = get_byte());
	read(&event_ID, sizeof(event_ID));
	return event_ID;
}

void InputEventBuffer::ignore_event()
{
	event_ID = NETCOM_ID_NONE;
	set(next_event_pointer);
}

void InputEventBuffer::back_event()
{
	const unsigned int SIZE_HEAD_PACKET=sizeof(event_ID)+sizeof(size_of_event);
	if(offset>=SIZE_HEAD_PACKET){
		event_ID = NETCOM_ID_NONE;
		offset-=SIZE_HEAD_PACKET;
		next_event_pointer=offset;
	}
	else {
		xassert(0 && "Invalid back event");
	}
}

unsigned long InputEventBuffer::getQuantAmount()
{
	unsigned long cntQuant=0;
	unsigned long i=offset;
	unsigned long sizeCurEvent;
	terEventID curID;
	while(i<filled_size){
		sizeCurEvent=*(unsigned long*)(&buf[i]);
		i+=sizeof(unsigned long);
		curID=*(terEventID*)(&buf[i]);
		if(curID==NETCOM_ID_NEXT_QUANT) cntQuant++;
		i+=sizeCurEvent;

	}
	return cntQuant;
}
*/
/***********************************************************************
				Output Event Buffer
***********************************************************************/
/*
OutputEventBuffer::OutputEventBuffer(unsigned int size)
: XBuffer(size) 
{
	pointer_to_size_of_event = -1;
	byte_sending=0;
}
int OutputEventBuffer::send(XDPConnection& conn, DPNID dpnid)
{
	xassert(pointer_to_size_of_event == -1 && 
		"There wasn't the end of the event");
	unsigned int sent = conn.Send(buf,tell(), dpnid);
	if(sent == tell())
		init();
	else{
		memmove(buf,buf + sent,tell() - sent);
		offset -= sent;
		}
	byte_sending+=sent;
	return sent;
}


void OutputEventBuffer::begin_event(terEventID event_ID)
{
	xassert(pointer_to_size_of_event == -1 && 
		"There wasn't the end of the event");
	pointer_to_size_of_event = offset;
	//*this < short(0) < (unsigned char)event_ID;
	size_of_event=0;
	write(&size_of_event, sizeof(size_of_event));
	write(&event_ID, sizeof(event_ID));

}
void OutputEventBuffer::end_event()
{
	xassert(pointer_to_size_of_event != -1 && 
		"There wasn't the begining of the event");
	int off = tell();
	set(pointer_to_size_of_event);
	///if((off - pointer_to_size_of_event - 2)>=30000) xassert(0 && "overflow packed frame");
	///*this < short(off - pointer_to_size_of_event - 2);
	size_of_event=off-pointer_to_size_of_event - sizeof(size_of_event);
	write(&size_of_event, sizeof(size_of_event));
	set(off);
	pointer_to_size_of_event = -1;
}


void OutputEventBuffer::clear()
{
	offset = 0;
	pointer_to_size_of_event = -1;
}
*/
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//			Universal buffer
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

InOutNetComBuffer::InOutNetComBuffer(unsigned int size, bool autoRealloc)
: XBuffer(size, autoRealloc) 
{
	reset();
	byte_sending=0;
	byte_receive=0;
	event_ID = NETCOM_ID_NONE;
}


void InOutNetComBuffer::reset()
{
	next_event_pointer = 0;
	filled_size = 0;
	offset = 0;
	event_ID = NETCOM_ID_NONE;
}


//Out
int InOutNetComBuffer::send(PNetCenter& conn, DPNID dpnid, bool flag_guaranted)
{
///	Тест 
///	unsigned int size=filled_size;
///	unsigned int msize=size;
///	while(msize>0){
///		int sizeEvent=*((unsigned long*)(&buf[size-msize])) + sizeof(size_of_event);
///		xassert(sizeEvent<=msize);
///		msize-=sizeEvent;
///	}
///	xassert(msize==0);


	unsigned int sent=0; 
	while(sent < filled_size){ // подразумевается ==
		sent+=conn.Send(buf+sent, filled_size-sent, dpnid, flag_guaranted);
	};
	xassert(filled_size==sent);
	init();
	reset();
	byte_sending+=sent;
	return sent;
}

int InOutNetComBuffer::write2File(XStream& fout)
{
	int result=fout.write(buf, filled_size);
	init();
	reset();
	return result;
}

//Out
void InOutNetComBuffer::putNetCommand(const netCommandGeneral* event)
{
	clearBufferOfTheProcessedCommands();
	set(filled_size);
	unsigned int pointer_to_size_of_event;
	//unsigned int size_of_event;
	pointer_to_size_of_event = offset;
	size_of_event=0;
	event_ID=event->EventID;
	write(&size_of_event, sizeof(size_of_event));
	write(&event_ID, sizeof(event_ID));

	event->Write(*this);

	int off = tell();
	set(pointer_to_size_of_event);
	size_of_event=off-pointer_to_size_of_event - sizeof(size_of_event);
	write(&size_of_event, sizeof(size_of_event));
	set(off);
	filled_size=off;
	set(0);
	//для нормального next event
	event_ID = NETCOM_ID_NONE;
	size_of_event=0;
}

//in
void InOutNetComBuffer::clearBufferOfTheProcessedCommands(void)
{
	if(next_event_pointer){
		if(filled_size != next_event_pointer)
			memmove(address(),address() + next_event_pointer, filled_size - next_event_pointer);
		filled_size -= next_event_pointer;
		offset = next_event_pointer = 0;
	}
}

bool InOutNetComBuffer::putBufferPacket(unsigned char* buf, unsigned int size)
{
///	Test
///	unsigned int msize=size;
///	unsigned char* mbuf=buf;
///	while(msize>0){
///		int sizeEvent=*((unsigned long*)(&buf[size-msize])) + sizeof(size_of_event);
///		xassert(sizeEvent<=msize);
///		msize-=sizeEvent;
///	}
///	xassert(msize==0);

	clearBufferOfTheProcessedCommands();
	if(length()-filled_size < size) {
		xassert("Net input buffer is small.");
		return 0;
	}
	memcpy((char*)address() + filled_size, buf, size);
	byte_receive+=size;
	filled_size +=size;
	//nextNetCommand();
	return 1;
}

int InOutNetComBuffer::currentNetCommandID()
{
	if(event_ID == NETCOM_ID_NONE){
		clearBufferOfTheProcessedCommands();
		return nextNetCommand();
	}
	return event_ID;
} 

terEventID InOutNetComBuffer::nextNetCommand()
{
	if(event_ID != NETCOM_ID_NONE){
		if(next_event_pointer /*+ sizeof(size_of_event)*/ > filled_size){
			xassert(0&&"dividual packet ?!");
			init();
			reset();
			return NETCOM_ID_NONE;
		}
		if(next_event_pointer != tell()){
			//ErrH.Abort("Incorrect events reading");
			XBuffer buf;
			buf < "Incorrect events reading" < " eventID=" <= event_ID < " offset=" <= tell() < " nextEventPoint=" <= next_event_pointer;
			xassertStr2(0, buf);
			init();
			reset();
			return NETCOM_ID_NONE;
		}
	}

	event_ID = NETCOM_ID_NONE;

	if(filled_size-tell() > (sizeof(size_of_event) + sizeof(event_ID)) ) {
		read(&size_of_event, sizeof(size_of_event)); //get_short();
		unsigned int new_pointer = next_event_pointer + size_of_event + sizeof(size_of_event);
		if(new_pointer > filled_size){
			xassert(0&&"dividual packet ?!");
			set(next_event_pointer);
			init();
			reset();
			return NETCOM_ID_NONE;
		}

		next_event_pointer = new_pointer;
		
		read(&event_ID, sizeof(event_ID));
	}
	return event_ID;
}

void InOutNetComBuffer::ignoreNetCommand()
{
	event_ID = NETCOM_ID_NONE;
	set(next_event_pointer);
}

void InOutNetComBuffer::backNetCommand()
{
	const unsigned int SIZE_HEAD_PACKET=sizeof(event_ID)+sizeof(size_of_event);
	if(offset>=SIZE_HEAD_PACKET){
		event_ID = NETCOM_ID_NONE;
		offset-=SIZE_HEAD_PACKET;
		next_event_pointer=offset;
	}
	else {
		xassert(0 && "Invalid back event");
	}
}

unsigned long InOutNetComBuffer::getQuantAmount()
{
	unsigned long cntQuant=0;
	unsigned long i=offset;
	unsigned long sizeCurEvent;
	terEventID curID;
	if(event_ID != NETCOM_ID_NONE){
		if(event_ID==NETCOM_ID_NEXT_QUANT) cntQuant++;
		i=next_event_pointer;
	}
	while(i<filled_size){
		sizeCurEvent=*(unsigned long*)(&buf[i]);
		i+=sizeof(unsigned long);
		curID=*(terEventID*)(&buf[i]);
		if(curID==NETCOM_ID_NEXT_QUANT) cntQuant++;
		i+=sizeCurEvent;

	}
	return cntQuant;
}
