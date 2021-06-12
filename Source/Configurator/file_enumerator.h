#ifndef __DFN_ITERATOR__
#define __DFN_ITERATOR__
/********************************************************************
created:	2002/07/29
created:	29:7:2002   16:44
filename: 	D:\Projects\Quest\QuestEditor\file_enumerator.h
file path:	D:\Projects\Quest\QuestEditor
file base:	dfn_iterator
file ext:	h
author:		Илюха

  purpose:	Итератор, который позволяет продвигаться
  по каталогу с целью получения информации о файлах
*********************************************************************/
//#include <iterator>
#include <io.h>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/iterator/iterator_adaptor.hpp>

class file_find_data : public _tfinddata_t
{
public:
#ifdef _AFX
	operator CString() const{
		return CString(name());
	};
#endif //_AFX

	operator std::basic_string<TCHAR> () const{
		return std::basic_string<TCHAR>(name());
	}

	bool operator <(const file_find_data& ffd) const{
		return (_tcsicmp(name(), ffd.name())<0);
	}

	bool has_attribs(UINT attrib) const{
		return ((this->attrib&attrib) != 0);
	}
	bool is_directory() const{
		return has_attribs(_A_SUBDIR);
	}
	bool is_readonly() const{
		return has_attribs(_A_RDONLY);
	}
	bool is_archive() const{
		return has_attribs(_A_ARCH);
	}
	bool is_system() const{
		return has_attribs(_A_SYSTEM);
	}
	bool is_hidden() const{
		return has_attribs(_A_HIDDEN);
	}
	bool is_normal() const{
		return has_attribs(_A_NORMAL);
	}
	time_t time_create() const{
		return this->_tfinddata_t::time_create;
	}
	time_t time_access() const{
		return this->_tfinddata_t::time_access;
	}
	time_t time_write() const{
		return this->_tfinddata_t::time_write;
	}
	time_t size() const{
		return this->_tfinddata_t::size;
	}
	LPCTSTR name() const{
		return this->_tfinddata_t::name;
	}
	bool is_parent_dir() const{
		return (!_tcscmp(name(), _T("..")));
	}
	bool is_current_dir() const{
		return (!_tcscmp(name(), _T(".")));
	}
	bool is_skipable_dir() const{
		return (is_parent_dir()||is_current_dir());
	}
};


class file_enumerator
{
	typedef file_enumerator _Self;
public:

	file_enumerator(LPCTSTR lpszExt)
	{
#if (_MSC_VER > 1200)
		hFile = _tfindfirst(lpszExt, &fData_);
#else
		hFile = _tfindfirst(const_cast<TCHAR*>(lpszExt), &fData_);
#endif
		m_bOk = (hFile == -1L) ? false : true;
	};

	~file_enumerator()
	{
		if (hFile != -1L) {
			_findclose(hFile);
		}
	}
	
	class iterator;
	friend class iterator;

	class iterator : public boost::iterator_facade<
								iterator,
								file_find_data const,
								boost::single_pass_traversal_tag
								>
	{
	public:
		iterator ():m_pEnum(NULL), m_bOk(false){}
		explicit iterator (file_enumerator*penum):m_pEnum(penum){
			m_bOk = (m_pEnum&&m_pEnum->good()? true: false);
		}
	private:

	friend class boost::iterator_core_access;

    void increment() { 
		if (m_bOk) {
			m_bOk = m_pEnum->find_next();
		}
	}

    bool equal(iterator const& other) const
    {
		return ((m_bOk == other.m_bOk)&&(!m_bOk||m_pEnum == other.m_pEnum));
    }

    reference dereference() const { 
		return m_pEnum->fData_; 
	}

	private:
		file_enumerator * m_pEnum;
		bool m_bOk;
	};

	iterator begin(){
		if (good()) {
			return iterator(this);
		}
		return iterator(NULL);
	}
	iterator end(){
		return iterator(NULL);
	}
	
	bool good() const {return m_bOk;}
	bool operator !() const {return !good();}

private:
	file_find_data fData_;
	LONG hFile;
	bool m_bOk;

	bool find_next(){
		return (m_bOk = (_tfindnext(hFile, &fData_) == 0));
	}
	file_enumerator(const file_enumerator&);
	void operator = (const file_enumerator&);

};


#endif//__DFN_ITERATOR__