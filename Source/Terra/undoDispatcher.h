
extern int UNDO_REDO_BUFFER_SIZE; //Текущий размер буфера в SurMap пикселах

struct sPreChangedArea {
	short x, y;
	short sx, sy;
	unsigned char* geo;
	unsigned char* dam;
	unsigned char* atr;
	unsigned char* sur;
	sPreChangedArea(void){
		geo=NULL;
		dam=NULL;
		atr=NULL;
		sur=NULL;
	};
	void setSize(int _sx, int _sy){
		sx=_sx; sy=_sy;
		int size=sx*sy;
		UNDO_REDO_BUFFER_SIZE+=size;
		geo=new unsigned char[size];
		dam=new unsigned char[size];
		atr=new unsigned char[size];
		sur=new unsigned char[size];
	};
	~sPreChangedArea(void){
		if(sur){
			int size=sx*sy;
			UNDO_REDO_BUFFER_SIZE-=size;
			if(sur) delete [] sur;
			if(atr) delete [] atr;
			if(dam) delete [] dam;
			if(geo) delete [] geo;
		}
	};
};