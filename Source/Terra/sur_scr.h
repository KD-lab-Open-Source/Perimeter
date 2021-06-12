#ifndef __SUR_SCR_H__
#define __SUR_SCR_H__

#include "auxInlineFuction.h"

#define MAX_ROWS 40
#define MAX_LINE_WIDTH 100
#define MAX_SCR_NAME 100
const int NOT_VARIABLE=0;
const int INT_VARIABLE=1;
const int DOUBLE_VARIABLE=2;
const int CHAR_VARIABLE=3;

const int CELL_NOT_CLEAR=0;
const int CELL_USUAL=1;
const int CELL_DIRECTED=2;

struct s_el {
	union {
		int v_int;
		double v_double;
		char* v_char;
	};
	s_el(void) {;}
	s_el(int v){ v_int = v; };
	s_el(double v){ v_double = v; };
	s_el(char* v){ v_char = v; };
	/*void set(int& v) { v_int=v;}
	void set(double& v) { v_double=v;}
	void set(char*& v) { v_char=v;}
	void get(int& v) { v=v_int;}
	void get(double& v) { v=v_double;}
	void get(char*& v) { v=v_char;}*/
};

struct s_row { //: s_el 
	vector<s_el> el;
	int type;
	int line_width;//Ширина текстовой строки
	char name[40];
	int d_int; //default
	double d_double;
	char* d_char;
	char flag_last_value;

	s_row(){ type=0;line_width=0; d_char=new char [MAX_LINE_WIDTH+1]; flag_last_value=0;};
	~s_row() {delete d_char;}
	void add_el(int v){ el.push_back(v);};
	void add_el(double v){ el.push_back(v);};
	void add_el(char* v){ 
		char* k=new char[line_width+1];
		strcpy(k,v);
		el.push_back(k);};
	void add_el_d(){ //добавитсь элемент со значением = default
		if(type==INT_VARIABLE) add_el(d_int);
		if(type==DOUBLE_VARIABLE) add_el(d_double);
		if(type==CHAR_VARIABLE) add_el(d_char);
	};


	void set_el(int number, int v){ el[number].v_int=v; if(flag_last_value)d_int=v;};
	void set_el(int number, double v){ el[number].v_double=v; if(flag_last_value)d_double=v;};
	void set_el(int number, char* v){ 
		strcpy(el[number].v_char, v);
		if(flag_last_value) strcpy(d_char,v);};

	void set_el_d(int number, int v){ el[number].v_int=v; d_int=v;}; //установить значение элемента и default-а
	void set_el_d(int number, double v){ el[number].v_double=v; d_double=v;};
	void set_el_d(int number, char* v){ el[number].v_char=v; d_char=v;};

	void get_el(int number, int& v){ v=el[number].v_int;};
	void get_el(int number, double& v){ v=el[number].v_double;};
	void get_el(int number, char*& v){ v=el[number].v_char;};

	void del_el(int number){ 
		if(type==CHAR_VARIABLE) delete el[number].v_char; 
		el.erase(el.begin()+number);};

//Вставка в конкретную позицию
	void ins_el(int poz, int v){
		el.insert(el.begin()+poz,1);
		set_el(poz, v);
	};
	void ins_el(int poz, double v){ 
		el.insert(el.begin()+poz,1);
		set_el(poz, v);
	};
	void ins_el(int poz, char* v){ 
		el.insert(el.begin()+poz,1);
		char* k=new char[line_width+1]; //Выделяем память под строку 
		el[poz].v_char=k; //Помещаем указатель под выделенную память в массив. Эти сложности необходимы т.к. В массиве хранятся указатели а не строки
		set_el(poz, v);
	};
	void ins_el_d(int poz){ //добавитсь элемент со значением = default
		if(type==INT_VARIABLE) ins_el(poz, d_int);
		if(type==DOUBLE_VARIABLE) ins_el(poz, d_double);
		if(type==CHAR_VARIABLE) ins_el(poz, d_char);
	};


};

struct s_sur_scr  { //: s_row
	s_row row[MAX_ROWS];
	int numbers_rows;
	int numbers_cell;
	int current_cell;
	int version;
	int type;
	char name_scr[MAX_SCR_NAME];
	int beg_num_rows; //Для конвертации скрипта необходимо запомнить кол-во старых строк

	s_sur_scr() {
		numbers_rows=0;
		numbers_cell=0;
		current_cell=-1;//Начальное значение т.к. сразу пребавляется 1 для вставки в заданную позицию
#ifdef _SURMAP2_
		draw_seq=0;
#endif
	};
	void delCell(int n){
		if(numbers_cell){
			for(int i=0; i<numbers_rows; i++) row[i].del_el(n);
			numbers_cell--;
			if(current_cell >= numbers_cell) current_cell=(numbers_cell-1); //??
		}
	};
	int load_scr(char* fname);
	void lex2oper(char* world, char*& buf);
	void load_data();
	void save_data_release();
	void new_row() {};
	void new_col() {};
	void addIset_default(int x, int y){ //Добавление CELL и заполнение ее defaul-значениями
		for(int i=0; i<numbers_rows; i++) row[i].add_el_d();//row X и Y заполняются передаваемыми X и Y
		row[0].set_el(numbers_cell, x);
		row[1].set_el(numbers_cell, y);
		current_cell=numbers_cell;
		numbers_cell++;
	};
	void addIset_default(int x, int y, int alpha){ //Добавление CELL и заполнение ее defaul-значениями
		for(int i=0; i<numbers_rows; i++) row[i].add_el_d();//row X и Y заполняются передаваемыми X и Y
		row[0].set_el(numbers_cell, x);
		row[1].set_el(numbers_cell, y);
		row[2].set_el(numbers_cell, alpha);
		current_cell=numbers_cell;
		numbers_cell++;
	};

	void insIset_default(int num_ins, int x, int y){ //Добавление CELL и заполнение ее defaul-значениями
		if(num_ins > numbers_cell) ErrH.Abort("Error in SurScr(insert element)");
		for(int i=0; i<numbers_rows; i++) row[i].ins_el_d(num_ins);//row X и Y заполняются передаваемыми X и Y
		row[0].set_el(num_ins, x);
		row[1].set_el(num_ins, y);
		current_cell=num_ins;
		numbers_cell++;
	};
	void insIset_default(int num_ins, int x, int y, int alpha){ //Добавление CELL и заполнение ее defaul-значениями
		if(num_ins > numbers_cell) ErrH.Abort("Error in SurScr(insert element)");
		for(int i=0; i<numbers_rows; i++) row[i].ins_el_d(num_ins);//row X и Y заполняются передаваемыми X и Y
		row[0].set_el(num_ins, x);
		row[1].set_el(num_ins, y);
		row[2].set_el(num_ins, alpha);
		current_cell=num_ins;
		numbers_cell++;
	};

	int findCell(int x, int y, int delta){
		int xc,yc;
		for(int i=0; i<numbers_cell; i++){
			row[0].get_el(i,xc);
			if(abs(calcDX(x,xc))<=delta){
				row[1].get_el(i,yc);
				if(abs(calcDY(y,yc))<=delta) return i;
			}
		}
		return -1;
	};
#ifdef _SURMAP2_
	char draw_seq; //Показ последовательности поинтов
	void drawCell(){
		int xc,yc,alpha,old_xc,old_yc;
		for(int i=0; i<numbers_cell; i++){
			row[0].get_el(i,xc);
			row[1].get_el(i,yc);
			GSetPixel16X(xc,yc,255);
			if(i==current_cell){
				GSetPixel16X(vMap.XCYCL(xc+1),vMap.YCYCL(yc+1),255);
				GSetPixel16X(vMap.XCYCL(xc-1),vMap.YCYCL(yc-1),255);
				GSetPixel16X(vMap.XCYCL(xc+1),vMap.YCYCL(yc-1),255);
				GSetPixel16X(vMap.XCYCL(xc-1),vMap.YCYCL(yc+1),255);
			}
			if(type==CELL_DIRECTED){
				row[2].get_el(i,alpha);
				const int v_long=20;
				GLine16(xc,yc,vMap.XCYCL(xc+round(v_long*cos((float)alpha*3.1415926535/180))),vMap.YCYCL(yc-round(v_long*sin((float)alpha*3.1415926535/180))),255);
			}
			if(draw_seq){
					extern sqFont sysfont;
					XBuffer status;
					status <= i;
					sysfont.draw(Gx2x(vMap.XCYCL(xc+3)),Gy2y(vMap.YCYCL(yc + 3)),(unsigned char*)((char*)status),255,-1);
					if(i>0)GLine16(xc,yc,old_xc,old_yc,255);
					old_xc=xc;old_yc=yc;


			}
		}
	};
#endif
	void lex2convert(char* world, char*& buf, XStream &fout);
	int findRow(char * name_row){
		int i;
		for(i=0; i< numbers_rows; i++){
			if(strcmp(name_row,row[i].name)==0) return i;
		}
		return -1;
	};
	int convert(char* name_scr, char* name_update_scr);
	void delete_data();

};
extern s_sur_scr sur_scr;
extern int get_lex(char*& buf, char* world);
#endif //__SUR_SCR_H__
