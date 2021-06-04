#include "stdafxTr.h"

#include <ctype.h>
#include <stdio.h>

#include "sur_scr.h"

s_sur_scr sur_scr;

int get_lex(char*& buf, char* world)
{
	int counter=0;
	while( ((isspace(*buf) ) || (*buf=='=')) && (*buf) ) buf++;
	while( (!(isspace(*buf)) && (*buf!='=')) && (*buf) ){
		*world=*buf; world++, buf++, counter ++;
	}
	*world=0;
	return counter;
}
int get_str(char*& buf, char* world)
{
	int counter=0;
	while( (*buf !='\"') && (*buf) ) buf++;
	buf++;
	while( (*buf !='\"') && (*buf) ){
		*world=*buf; world++, buf++, counter ++;
	}
	*world=0;
	return counter;

}
int get_line(XStream& f, char* buf, int maxL){
	char b;
	if (maxL==0) return 0;
	int len=0;
	f.read(&b,1);// Пропуск служебных символов в начале строки
	while(isspace(b)) {f.read(&b,1);}
	do {
		*buf=b;
		buf++;
		maxL--;
		len++;
		f.read(&b,1);
	}
	while( (!f.eof()) && (b!='\n') && (maxL));
	*buf=0;
	return len;
}

void s_sur_scr::lex2oper(char* world, char*& buf)
{
	if(strcmp(world,"last")==0){
		row[numbers_rows].flag_last_value=1;
	}
	if(strcmp(world,"default")==0){
		row[numbers_rows].flag_last_value=0;
	}
	if(strcmp(world,"int")==0){
		row[numbers_rows].type=INT_VARIABLE;
		if(get_lex(buf, world)) strcpy(row[numbers_rows].name,world);
		if(get_lex(buf, world)) row[numbers_rows].d_int=atoi(world);
		numbers_rows++;
	}
	if(strcmp(world,"double")==0){
		row[numbers_rows].type=DOUBLE_VARIABLE;
		if(get_lex(buf, world)) strcpy(row[numbers_rows].name,world);
		if(get_lex(buf, world)) row[numbers_rows].d_double=atof(world);
		numbers_rows++;
	}
	if(strcmp(world,"string")==0){
		row[numbers_rows].type=CHAR_VARIABLE;
		if(get_lex(buf, world)) row[numbers_rows].line_width=atoi(world);
		if(get_lex(buf, world)) strcpy(row[numbers_rows].name,world);
		if(get_str(buf, world)) strcpy(row[numbers_rows].d_char,world);
		numbers_rows++;
	}

}

int s_sur_scr::load_scr(char* fname)
{
	strcpy(name_scr,fname);
	XStream f(0);
	if(!f.open(fname,XS_IN)) return 0;
	char* buf,*buft,*world;
	buft=buf=new char[256]; world=new char[256];
	char b;
	//ОПОЗНАНИЕ скрипта и версии
	f.read(buf,7); buf[7]=0;
	if(strcmp(buf,"SUR_SCR")!=0) { delete world; delete buft; f.close(); return 0;}
	f >=version;
	//Определение типа ячейки (обычная или направленная)
	get_line(f,buf=buft,255);
	type=CELL_NOT_CLEAR;
	if(get_lex(buf,world))
		if(strcmp(world,"cell")==0)
			if(get_lex(buf,world)){
				if(strcmp(world,"usual")==0){
					type=CELL_USUAL;
					numbers_rows=2;
					numbers_cell=0;
					row[0].type=INT_VARIABLE; row[1].type=INT_VARIABLE;
					strcpy(row[0].name,"X"); strcpy(row[1].name,"Y");
				}
				if(strcmp(world,"directed")==0){
					type=CELL_DIRECTED;
					numbers_rows=3;
					numbers_cell=0;
					row[0].type=INT_VARIABLE; row[1].type=INT_VARIABLE; row[2].type=INT_VARIABLE;
					strcpy(row[0].name,"X"); strcpy(row[1].name,"Y"); strcpy(row[2].name,"alpha");
				}
			}
	if(type==CELL_NOT_CLEAR) return 0;

	// Поиск скрипта { script }
	f.seek(7, XS_BEG);
	struct { int begin,end;
	} script={0,0};
	while( !f.eof()){ //Поиск начала скрипта
		f.read(&b,1); if(b=='{') { 
			script.begin=f.tell();
			break;
		}
	}
	while( !f.eof()){ //Поиск конца скрипта
		f.read(&b,1); if(b=='}') { 
			script.end=f.tell();
			break;
		}
	}
	if(script.end <= script.begin) {delete world; delete buft; f.close(); return 0;} //Выход если нет скрипта
	f.seek(script.begin+1, XS_BEG);
	while(f.tell() < script.end){
		//f.getline(buf,255);
		get_line(f,buf=buft,255);
		if(!get_lex(buf,world)) continue;
		lex2oper(world, buf);
		if(!get_lex(buf,world)) continue;
		lex2oper(world, buf);

/*				b>=b;
		if( (b==" ") || (!b) ){

		}
		f.read(b,1)
		f.getline(buf,255);
		for(int i=0; i<)*/

	}
	strcpy(name_scr,fname);
	delete world; delete buft;
	f.close();
	return 1;
}

void s_sur_scr::load_data()
{
	XStream f(0);
	if(!f.open(name_scr,XS_IN)) return ;
	char* world;
	world=new char[256];
	char b;
	int t_int;
	double t_double;

	while( !f.eof()) { f.read(&b,1); if(b=='}') break; }//Поиск конца скрипта

	int x,y,a,i;
	while( !f.eof()){
		i=0;
		f>=x ; 
		if(f.eof()) break;
		f>=y; i+=2;
		if(type==CELL_DIRECTED) { f>=a; i++;}
		if(type==CELL_DIRECTED){addIset_default(x,y,a);}
		else{addIset_default(x,y);}
		for(i; i<numbers_rows; i++){
			if( row[i].type==INT_VARIABLE){
				f>=t_int;
				row[i].set_el(current_cell,t_int);
			}
			if( row[i].type==DOUBLE_VARIABLE){
				f>=t_double;
				row[i].set_el(current_cell,t_double);
			}
			if( row[i].type==CHAR_VARIABLE){
				if(get_line(f,world,255)==0) break;
				row[i].set_el(current_cell,world);
			}
		}
	}
	delete world;
	f.close();
}

void s_sur_scr::save_data_release()
{
	XBuffer t;
	t < name_scr;t-=3;t < "old";
	if (rename(name_scr, t)!=0) return;
	XStream fi(0),fo(0);
	if(!fi.open(t, XS_IN)) return ;
	if(!fo.open(name_scr, XS_OUT)) return ;
//	char* world=new char[256];
	char b;
	int i,j,t_int;
	double t_double;
	char* t_char;

	while( !fi.eof() ) { fi.read(&b,1); fo.write(&b,1); if(b=='}') break; } //Поиск конца скрипта
	for( j=0; j<numbers_cell; j++){
		for(i=0; i<numbers_rows; i++){
			if( row[i].type==INT_VARIABLE){
				row[i].get_el(j,t_int);
				fo< "\n" <=t_int;
			}
			if( row[i].type==DOUBLE_VARIABLE){
				row[i].get_el(j,t_double);
				fo< "\n" <=t_double;
			}
			if( row[i].type==CHAR_VARIABLE){
				row[i].get_el(j,t_char);
				fo < "\n" < t_char;
			}
		}
		fo < "\n";
	}
//	delete world;
	fo < "\n";
	fi.close();
	fo.close();
	remove(t);
	delete_data();

}
void s_sur_scr::delete_data()
{
	//delete data
	for(int j=numbers_cell-1; j>=0; j--){
		for(int i=0; i<numbers_rows; i++){
			row[i].del_el(j);
		}
	}
	numbers_rows=2;
	numbers_cell=0;
	if(type==CELL_USUAL){
		numbers_rows=2;
		numbers_cell=0;
	}
	if(type==CELL_DIRECTED){
		numbers_rows=3;
		numbers_cell=0;
	}

}
void s_sur_scr::lex2convert(char* world, char*& buf, XStream &fout)
{
	if(strcmp(world,"convert")==0){ // conver AK last double AKM = 11
		get_lex(buf, world);

		int temp=numbers_rows; //Для того что-бы поиск шел толко по старым строкам
		numbers_rows=beg_num_rows;
		int num_old_row=findRow(world);
		numbers_rows=temp;

		fout < buf <"\n";

		if(num_old_row==-1) ErrH.Abort("Не существующее имя переменной старого скрипта",XERR_USER,0,world);
		get_lex(buf, world);
		if(strcmp(world,"last")==0){ row[numbers_rows].flag_last_value=1; }
		if(strcmp(world,"default")==0){ row[numbers_rows].flag_last_value=0; }
		get_lex(buf, world);
		if(strcmp(world,"int")==0){
			row[numbers_rows].type=INT_VARIABLE;
			if(get_lex(buf, world)) strcpy(row[numbers_rows].name,world);
			if(get_lex(buf, world)) row[numbers_rows].d_int=atoi(world);
		}
		else if(strcmp(world,"double")==0){
			row[numbers_rows].type=DOUBLE_VARIABLE;
			if(get_lex(buf, world)) strcpy(row[numbers_rows].name,world);
			if(get_lex(buf, world)) row[numbers_rows].d_double=atof(world);
		}
		else if(strcmp(world,"string")==0){
			row[numbers_rows].type=CHAR_VARIABLE;
			if(get_lex(buf, world)) row[numbers_rows].line_width=atoi(world);
			if(get_lex(buf, world)) strcpy(row[numbers_rows].name,world);
			if(get_str(buf, world)) strcpy(row[numbers_rows].d_char,world);
		}
		else return;
		int t_int;
		double t_double;
		char* t_char;
		char buffer[20];
		int i;
		for(i=0; i<numbers_cell; i++){
			row[numbers_rows].add_el_d();
			if(row[num_old_row].type==INT_VARIABLE){ //Если старая ячейка - Int
				row[num_old_row].get_el(i,t_int);
				if(row[numbers_rows].type==INT_VARIABLE) row[numbers_rows].set_el(i,t_int);
				if(row[numbers_rows].type==DOUBLE_VARIABLE){ t_double=t_int; row[numbers_rows].set_el(i,t_double); }
				if(row[numbers_rows].type==CHAR_VARIABLE){
					itoa(t_int,buffer,10);
					row[numbers_rows].set_el(i,buffer);					
				}
			}
			if(row[num_old_row].type==DOUBLE_VARIABLE){
				row[num_old_row].get_el(i,t_double);
				if(row[numbers_rows].type==INT_VARIABLE){ t_int=round(t_double); row[numbers_rows].set_el(i,t_int);}
				if(row[numbers_rows].type==DOUBLE_VARIABLE) row[numbers_rows].set_el(i,t_double);
				if(row[numbers_rows].type==CHAR_VARIABLE){
					XBuffer bbb; bbb <= t_double;
					row[numbers_rows].set_el(i,bbb.address());					
				}
			}
			if(row[num_old_row].type==CHAR_VARIABLE){
				row[num_old_row].get_el(i,t_char);
				if(row[numbers_rows].type==INT_VARIABLE){ t_int=atoi(t_char); row[numbers_rows].set_el(i,t_int);}
				if(row[numbers_rows].type==DOUBLE_VARIABLE){ t_double=atof(t_char); row[numbers_rows].set_el(i,t_double);}
				if(row[numbers_rows].type==CHAR_VARIABLE) row[numbers_rows].set_el(i,t_char);
			}
			//
		}

		numbers_rows++;
	}	
	else if(strcmp(world,"new")==0){

		fout < buf <"\n";

		get_lex(buf, world);

		if(strcmp(world,"last")==0){ row[numbers_rows].flag_last_value=1; }
		if(strcmp(world,"default")==0){ row[numbers_rows].flag_last_value=0; }
		get_lex(buf, world);
		if(strcmp(world,"int")==0){
			row[numbers_rows].type=INT_VARIABLE;
			if(get_lex(buf, world)) strcpy(row[numbers_rows].name,world);
			if(get_lex(buf, world)) row[numbers_rows].d_int=atoi(world);
		}
		else if(strcmp(world,"double")==0){
			row[numbers_rows].type=DOUBLE_VARIABLE;
			if(get_lex(buf, world)) strcpy(row[numbers_rows].name,world);
			if(get_lex(buf, world)) row[numbers_rows].d_double=atof(world);
		}
		else if(strcmp(world,"string")==0){
			row[numbers_rows].type=CHAR_VARIABLE;
			if(get_lex(buf, world)) row[numbers_rows].line_width=atoi(world);
			if(get_lex(buf, world)) strcpy(row[numbers_rows].name,world);
			if(get_str(buf, world)) strcpy(row[numbers_rows].d_char,world);
		}
		else return;
		int i;
		for(i=0; i<numbers_cell; i++){
			row[numbers_rows].add_el_d();
		}
		numbers_rows++;
	}

}

int s_sur_scr::convert(char* name_scr, char* name_update_scr)
{
	if(load_scr(name_scr)==0) return 0;
	load_data();

// Расшифровка Updata скрипта
	XStream f(0);
	if(!f.open(name_update_scr,XS_IN)) return 0;
	char* buf,*buft,*world;
	buft=buf=new char[256]; world=new char[256];
	char b;
	//ОПОЗНАНИЕ скрипта и версии
	f.read(buf,10); buf[10]=0;
	if(strcmp(buf,"SUR_SCR_UP")!=0) { delete world; delete buft; f.close(); return 0;}
	int ver_up;
	f >=ver_up;
	if(ver_up <= version ) ErrH.Abort("Старая версия Updata скрипта");

	XStream fout(0);
	if(!fout.open(name_scr,XS_OUT)) return 0;
	fout < "SUR_SCR " <=ver_up < "\n";

	//Определение типа ячейки (обычная или направленная)
	get_line(f,buf=buft,255);
	int newtype;
	newtype=CELL_NOT_CLEAR;
	if(get_lex(buf,world))
		if(strcmp(world,"cell")==0)
			if(get_lex(buf,world)){
				if(strcmp(world,"usual")==0){
					newtype=CELL_USUAL;
					//numbers_rows=2;
					//numbers_cell=0;
					//row[0].type=INT_VARIABLE; row[1].type=INT_VARIABLE;
					//strcpy(row[0].name,"X"); strcpy(row[1].name,"Y");
					fout < "cell usual\n";
				}
				if(strcmp(world,"directed")==0){
					newtype=CELL_DIRECTED;
					//numbers_rows=3;
					//numbers_cell=0;
					//row[0].type=INT_VARIABLE; row[1].type=INT_VARIABLE; row[2].type=INT_VARIABLE;
					//strcpy(row[0].name,"X"); strcpy(row[1].name,"Y"); strcpy(row[2].name,"alpha");
					fout < "cell directed\n";
				}
			}
	if(newtype==CELL_NOT_CLEAR) return 0;

	// Поиск скрипта { script }
	f.seek(10, XS_BEG);
	struct { int begin,end;
	} script={0,0};
	while( !f.eof()){ //Поиск начала скрипта
		f.read(&b,1); if(b=='{') { 
			script.begin=f.tell();
			break;
		}
	}
	while( !f.eof()){ //Поиск конца скрипта
		f.read(&b,1); if(b=='}') { 
			script.end=f.tell();
			break;
		}
	}
	if(script.end <= script.begin) {delete world; delete buft; f.close(); return 0;} //Выход если нет скрипта
	f.seek(script.begin+1, XS_BEG);
	beg_num_rows=numbers_rows;
	fout < "{\n";
	while(f.tell() < script.end){
		get_line(f,buf=buft,255);
		if(!get_lex(buf,world)) continue;
		lex2convert(world, buf,fout);
	}
	fout < "}\n";

// Запись данных в файл
	int j,i,t_int;
	double t_double;
	char* t_char;
	for( j=0; j<numbers_cell; j++){
		for(i=0; i<2; i++){
			if( row[i].type==INT_VARIABLE){
				row[i].get_el(j,t_int);
				fout< "\n" <=t_int;
			}
			else ErrH.Abort("Фигня заместо X,Y");
		}
		if(newtype==CELL_DIRECTED){
			if(type==CELL_DIRECTED)	row[i].get_el(j,t_int);
			else t_int=0;
			fout< "\n" <=t_int;
		}

		for(i=beg_num_rows; i<numbers_rows; i++){
			if( row[i].type==INT_VARIABLE){
				row[i].get_el(j,t_int);
				fout< "\n" <=t_int;
			}
			if( row[i].type==DOUBLE_VARIABLE){
				row[i].get_el(j,t_double);
				fout< "\n" <=t_double;
			}
			if( row[i].type==CHAR_VARIABLE){
				row[i].get_el(j,t_char);
				fout < "\n" < t_char;
			}
		}
		fout < "\n";
	}
//	delete world;
	fout < "\n";
	fout.close();
// Конец записи в файл
//	strcpy(name_scr,fname);
	delete world; delete buft;
	f.close();
	return 1;

//
	return 0;//Ошибка
	return 1;
}

