#ifndef __EVENT_BUFFER_DP
#define __EVENT_BUFFER_DP

#include "ConnectionDP.h"
#include "CommonEvents.h"
/*
class OutputEventBuffer : public XBuffer 
{
	unsigned int pointer_to_size_of_event;

	unsigned int size_of_event;

	unsigned long byte_sending;

public:
	OutputEventBuffer(unsigned int size);

	int send(XDPConnection& conn, DPNID dpnid);
	void begin_event(terEventID event_ID);
	void end_event();
	void clear();
	unsigned long getByteSending(){
		unsigned long result=byte_sending;
		byte_sending=0;
		return result;
	}
	void putNetCommand(const netCommandGeneral* event);
};

class InputEventBuffer : public XBuffer 
{
	//int event_ID;
	//int size_of_event;
	terEventID event_ID;
	unsigned long size_of_event;

	unsigned int next_event_pointer;
	unsigned int filled_size;

	unsigned long byte_receive;

public:
	InputEventBuffer(unsigned int size);

	void reset();

	int receive(XDPConnection& conn, DPNID dpnid); 
	void clearBufferOfTheProcessedCommands(void);
	bool putInputBufferInputPacket(unsigned char* buf, unsigned int size);


	int current_event();
	int event_size(){ return size_of_event; }
	terEventID next_event(); 
	void ignore_event();
	void back_event();

	int get_byte(){ unsigned char t; *this > t; return (int)t; }
	int get_short(){ short t; *this > t; return (int)t; }
	int get_word(){ unsigned short t; *this > t; return (int)t; }
	int get_int(){ int t; *this > t; return (int)t; }
	unsigned int get_dword(){ unsigned int t; *this > t; return (unsigned int )t; }
	double get_float(){ float t; *this > t; return (double)t; }
	unsigned long getQuantAmount();

	unsigned long getByteReceive(){
		unsigned long result=byte_receive;
		byte_receive=0;
		return result;
	}

};
*/
///////////////////////////////////////////////////////////////////////////////////

class PNetCenter;
class InOutNetComBuffer : public XBuffer 
{
	unsigned long byte_receive;//in
	unsigned long byte_sending;//out
public:
	terEventID event_ID;//in

	unsigned long size_of_event;//in
	unsigned int next_event_pointer;//in
public:
	unsigned int filled_size;//in
	InOutNetComBuffer(unsigned int size, bool autoRealloc);

	void clearBufferOfTheProcessedCommands(void);//out
	int send(PNetCenter& conn, DPNID dpnid, bool flag_guaranted=1);//out
	unsigned long getByteSending(){//out
		unsigned long result=byte_sending;
		byte_sending=0;
		return result;
	}
	unsigned long getByteReceive(){
		unsigned long result=byte_receive;
		byte_receive=0;
		return result;
	}
	void reset();//?
	int write2File(XStream& fout);
	void putNetCommand(const netCommandGeneral* event);//out
	bool putBufferPacket(unsigned char* buf, unsigned int size);//in
	int currentNetCommandID();//in
	terEventID nextNetCommand();//in
	void ignoreNetCommand();//in
	void backNetCommand();//in
	unsigned long getQuantAmount();//in
	bool isEmpty(void){
		return filled_size <= 0; // подразумевается ==
	}

};
#endif //__EVENT_BUFFER_DP




