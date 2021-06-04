// CurDirSafer.h: interface for the CurDirSafer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CURDIRSAFER_H__513F2DD5_8E2A_4A6E_8886_5484FBD57A58__INCLUDED_)
#define AFX_CURDIRSAFER_H__513F2DD5_8E2A_4A6E_8886_5484FBD57A58__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/// Класс для хранения текущег каталога для какой-нибудь операции с диском
class CurDirSafer  
{
	//! каталог для текущей операции
	CString m_strCurrDirSave;
public:
	CurDirSafer();
	CurDirSafer(const CString &strCurDir);
	~CurDirSafer();
	
	//! сохраняем текущую директорию
	void save_current();
	//! выставляет сохраненную дирректорию 
	void set_current();

	const CString& get_saved(){
		return m_strCurrDirSave;
	}
	static CString GetCurrentDirectory();
};

///  \brief Класс на время своей жизни выставляет каталог, сохраненный в safer_. 
/// При разрушении восстанавливает сохраненный каталог
class CurDirGuard{
	CurDirSafer* safer_;
	//! каталог, который запоминаем, чтобы в любом случае восстановить
	CString m_strRestoreCatalog;
public:
	CurDirGuard(CurDirSafer* ps = NULL){
		safer_ = ps;
		save_and_set();
	}
	CurDirGuard(CurDirSafer* ps, bool bSave){
		safer_ = ps;
		if (bSave)
			save_and_set();
	}
	~CurDirGuard(){
		restore();
	}

	void save(){
		m_strRestoreCatalog = CurDirSafer::GetCurrentDirectory();
	}

	void save_and_set(){
		save();
		if (safer_) 
			safer_->set_current();
	}
	void restore(){
		SetCurrentDirectory(m_strRestoreCatalog);
	}

	const CString& get_saved()const{
		return m_strRestoreCatalog;
	}
};
#endif //!defined(AFX_CURDIRSAFER_H__513F2DD5_8E2A_4A6E_8886_5484FBD57A58__INCLUDED_)
