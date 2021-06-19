
struct sAPoint{
	int x, y;
};

struct sArea{
	std::list<sAPoint> points;
	std::list<sAPoint>::iterator curPoint;
	unsigned char fl_editing;
	sArea() {
		fl_editing=0;
	};
	~sArea() {
		points.erase(points.begin(), points.end());
	};
	void insPoint(int x, int y){
		points.push_back(sAPoint());
		curPoint=points.end(); curPoint--;
		curPoint->x=x;
		curPoint->y=y;
	};
	void curPoint2Begin(void){
		curPoint=points.begin();
	};
	sAPoint * getPointAndPP(){
		if(curPoint!=points.end()){
			sAPoint * tmp=&(*curPoint);
			curPoint++;
			return tmp;
		}
		else return NULL;
	}

};

struct sAreas{
	std::list<sArea> areas;
	std::list<sArea>::iterator curArea;
	sAreas() {
	};
	~sAreas() {
		areas.erase(areas.begin(), areas.end());
	};
	void newArea(void){
		areas.erase(areas.begin(), areas.end());
		areas.push_back(sArea());
		curArea=areas.end(); curArea--;
	};
};

extern sAreas HFAreas;