#ifndef __MEMORY_POOL_H__
#define __MEMORY_POOL_H__

////////////////////////////////////////////
//	Пул памяти фиксированного размера
////////////////////////////////////////////
template <class T>
class MemoryPoolTemplate {
public:
	~MemoryPoolTemplate() { clear(); }
	void clear() { while(!heap.empty()){ delete heap.back(); heap.pop_back(); } }

	void* alloc(size_t size) { 
		xassert(size == sizeof(T) && "Не определен пул памяти");
		if(heap.empty()) 
			return new char[size]; 
		void* p = heap.back();
		heap.pop_back();
		return p;
	}
	void free(void* p) { heap.push_back(p); }

	// Debug
	size_t size() const { heap.size()*sizeof(T); }
	size_t blocks() const { return heap.size(); }

private:
	vector<void*> heap;
};

// В декларацию каждого класса
#define MEMORY_POOL_DECLARATION(Type) \
	static MemoryPoolTemplate<Type> memory_pool; \
	void* operator new(size_t size) { return memory_pool.alloc(size); } \
	void operator delete(void* ptr) { memory_pool.free(ptr); } 

// В описание каждого класса
#define MEMORY_POOL_DEFINITION(Type) \
	MemoryPoolTemplate<Type> Type::memory_pool;


////////////////////////////////////////////
//	    Пул памяти переменного размера
// Используя перегруженные new/delete, 
// перехватывается и кэшируется выделение памяти.
// Дифференциация производится по размеру
// выделяемых кусков (точное совпадение, хотя 
// несложно переделать на >=). 
// В начале каждого куска небольшая служебная
// информация.
// Использование:
//
// class Base {
//		static MemoryPool memory_pool;
// public:
// 		void* operator new(size_t size) { return memory_pool.alloc(size); }
// 		void operator delete(void* ptr) { memory_pool.free(ptr); }
// };
//
// MemoryPool Base::memory_pool;
//
////////////////////////////////////////////

class MemoryPool {
public:
	void clear(); // Очищает накопившийся пул
	void* alloc(size_t size);
	void free(void* ptr);

	// Debug
	size_t size() const; // Без заголовков, т.е. то, что было запрошено в new
	size_t blocks() const;

private:
	typedef multimap<int, void*> MemoryMap;
	MemoryMap memory_map;

	// В начале каждого блока памяти находится заголовок с размером и верификатором.
#ifndef _FINAL_VERSION_
	struct Header {
		enum { Verificator = 0x83838383 };
		size_t verificator;
		size_t size;
		Header(size_t sz) : verificator(Verificator), size(sz) {}
	};
#else // _FINAL_VERSION_
	struct Header {
		size_t size;
		Header(size_t sz) : size(sz) {}
	};
#endif // _FINAL_VERSION_
};


///////////////////////////////////////////
//		Inline definitions
///////////////////////////////////////////
inline void* MemoryPool::alloc(size_t size)
{
	MemoryMap::iterator mi = memory_map.find(size);
	if(mi == memory_map.end()) {
		void* p = new char[size + sizeof(Header)];
		*((Header*)p - 1) = Header(size);
		return p;
	}
	void* p = mi->second;
	memory_map.erase(mi);
	return p;
}

inline void MemoryPool::free(void* ptr)
{
	Header* header = ((Header*)ptr - 1);
	xassert(header->verificator == Header::Verificator);
	memory_map.insert(MemoryMap::value_type(header->size, ptr));
}

inline void MemoryPool::clear()
{
	MemoryMap::iterator mi;
	FOR_EACH(memory_map, mi) {
		void* p = (Header*)mi->second - 1;
		delete p;
	}
	memory_map.clear();
}

inline size_t MemoryPool::size() const
{
	size_t sz = 0;
	MemoryMap::const_iterator mi;
	FOR_EACH(memory_map, mi) 
		sz += mi->first;
	return sz;
}

inline size_t MemoryPool::blocks() const
{
	return memory_map.size();
}



#endif //__MEMORY_POOL_H__