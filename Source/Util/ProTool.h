#ifndef __PRO_TOOL_H__
#define __PRO_TOOL_H__

template<class C, class S> 
void write_container(S& stream, const C& container) 
{ 
	stream < container.size(); 
	for(C::const_iterator i = container.begin(); i != container.end(); ++i) 
		stream < *i; 
}

enum ReadContainerMode {
	READ_CONTAINER_CLEAR, // стирает содержимое контейнера
	READ_CONTAINER_APPEND, // дописывает в конец
	READ_CONTAINER_REWRITE // пишет поверху и далее добавляет
};

template<class C, class S> 
void read_container(S& stream, C& container, ReadContainerMode mode = READ_CONTAINER_CLEAR) 
{ 
	int size; 
	stream > size; 
	if(mode == READ_CONTAINER_CLEAR)
		container.clear(); 
	if(mode == READ_CONTAINER_REWRITE)
		for(C::iterator i = container.begin(); i != container.end() && size-- > 0; ++i) 
			stream > *i;
	while(size-- > 0){ 
		container.push_back(C::value_type()); 
		stream > container.back(); 
		} 
}

template<class T, class S> 
void read_vector(S& stream, vector<T>& container, ReadContainerMode mode = READ_CONTAINER_CLEAR) 
{ 
	int size; 
	stream > size; 
	if(mode == READ_CONTAINER_CLEAR)
		container.clear(); 
	if(mode == READ_CONTAINER_REWRITE)
		for(vector<T>::iterator i = container.begin(); i != container.end() && size-- > 0; ++i) 
			stream > *i;
	if(size > 0){
		container.reserve(container.size() + size);
		while(size-- > 0){ 
			container.push_back(T()); 
			stream > container.back(); 
			} 
		}
}


inline void average(Vect3f& v_avr, const Vect3f& v, float tau){ average(v_avr.x, v.x, tau); average(v_avr.y, v.y, tau); average(v_avr.z, v.z, tau); }
inline void average(Vect2f& v_avr, const Vect2f& v, float tau){ average(v_avr.x, v.x, tau); average(v_avr.y, v.y, tau); }

#endif // __PRO_TOOL_H__
