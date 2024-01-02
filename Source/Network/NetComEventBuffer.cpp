
#include "NetIncludes.h"

#include "NetComEventBuffer.h"
#include "P2P_interface.h"


#ifdef PERIMETER_DEBUG_ASSERT
#define xassertStr2(exp, str) { std::string s = #exp; s += "\n"; s += str; xxassert(exp,s.c_str()); }
#else
#define xassertStr2(exp, str) 
#endif


LogStream netLog("PerimeterGame.log",XS_OUT);

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

InOutNetComBuffer::InOutNetComBuffer(void* p, size_t sz)
: XBuffer(p, sz)
{
    reset();
    byte_sending=0;
    byte_receive=size;
    event_ID = NETCOM_ID_NONE;
    filled_size=size;
}

void InOutNetComBuffer::reset()
{
	next_event_pointer = 0;
	filled_size = 0;
	offset = 0;
	event_ID = NETCOM_ID_NONE;
}

//Out
void InOutNetComBuffer::putNetCommand(const netCommandGeneral* event)
{
	clearBufferOfTheProcessedCommands();
	set(filled_size);
    write(&NETCOM_BUFFER_PACKET_ID, sizeof(NETCOM_BUFFER_PACKET_ID));
    unsigned int pointer_to_size_of_event = offset;
    event_size_t size_of_event = 0;
	write(&size_of_event, sizeof(size_of_event));
    event_ID=event->EventID;
	write(&event_ID, sizeof(event_ID));

	event->Write(*this);

	size_t off = tell();
	set(pointer_to_size_of_event);
	size_of_event=off-pointer_to_size_of_event - sizeof(size_of_event);
	write(&size_of_event, sizeof(size_of_event));
	set(off);
	filled_size=off;
	set(0);
	//для нормального next event
	event_ID = NETCOM_ID_NONE;
}

//in
void InOutNetComBuffer::clearBufferOfTheProcessedCommands() {
	if(next_event_pointer){
		if(filled_size != next_event_pointer)
			memmove(address(),address() + next_event_pointer, filled_size - next_event_pointer);
		filled_size -= next_event_pointer;
		offset = next_event_pointer = 0;
	}
}

bool InOutNetComBuffer::putBufferPacket(char* buf, unsigned int size)
{
    if (size < SIZE_NETCOM_PACKET_HEAD) {
        fprintf(stderr, "Buffer packet is too small\n");
        xassert(0);
        return false;
    }
    uint32_t packet_id = *(reinterpret_cast<uint32_t*>(buf));
    if (packet_id != NETCOM_BUFFER_PACKET_ID) {
        fprintf(stderr, "Buffer packet is not a netcom buffer\n");
        xassert(0);
        return false;
    }
	clearBufferOfTheProcessedCommands();
	if(length()-filled_size < size) {
        fprintf(stderr, "Net input buffer is small\n");
        xassert(0);
		return false;
	}
	memcpy(address() + filled_size, buf, size);
	byte_receive+=size;
	filled_size +=size;
	//nextNetCommand();
	return true;
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
	if (event_ID != NETCOM_ID_NONE) {
		if(next_event_pointer > filled_size){
			xassert(0&&"Incomplete packet ?!");
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

	if (filled_size-tell() > SIZE_NETCOM_PACKET_HEAD) {
        uint32_t packet_id = 0;
        read(&packet_id, sizeof(NETCOM_BUFFER_PACKET_ID));
        if (packet_id != NETCOM_BUFFER_PACKET_ID) {
            ErrH.Abort("Couldn't match packet header ID, wrong data in input buffer?");
        }
        event_size_t size_of_event = 0;
		read(&size_of_event, sizeof(size_of_event));
		unsigned int new_pointer = next_event_pointer + size_of_event + sizeof(size_of_event) + sizeof(packet_id);
		if(new_pointer > filled_size){
			xassert(0&&"Incomplete packet ?!");
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
	if(offset >= SIZE_NETCOM_PACKET_HEAD){
		event_ID = NETCOM_ID_NONE;
		offset-=SIZE_NETCOM_PACKET_HEAD;
		next_event_pointer=offset;
	}
	else {
		xassert(0 && "Invalid back event");
	}
}

size_t InOutNetComBuffer::getQuantAmount()
{
	size_t cntQuant=0;
	event_size_t i=offset;
	event_size_t sizeCurEvent;
	terEventID curID;
	if(event_ID != NETCOM_ID_NONE){
		if(event_ID==NETCOM_ID_NEXT_QUANT) cntQuant++;
		i=next_event_pointer;
	}
	while(i<filled_size){
		sizeCurEvent=*(event_size_t*)(&buf[i]);
		i+=sizeof(event_size_t);
		curID=*(terEventID*)(&buf[i]);
		if(curID==NETCOM_ID_NEXT_QUANT) cntQuant++;
		i+=sizeCurEvent;

	}
	return cntQuant;
}
