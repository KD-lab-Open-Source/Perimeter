// BasePage.h: interface for the BasePage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASEPAGE_H__1AE33142_90A6_4140_9E93_A95B9C0ACBAC__INCLUDED_)
#define AFX_BASEPAGE_H__1AE33142_90A6_4140_9E93_A95B9C0ACBAC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>
#include "Settings/ISetting.h"

class BasePage : public CPropertyPage  
{
protected:
	typedef std::list<ISetting*> SettingsList;
	SettingsList& getSettingsList();
public:
	BasePage(UINT nIDTemplate, UINT nIDCaption = 0);
	virtual ~BasePage();

public:
	virtual BOOL OnApply();
	virtual void reloadLanguage() = 0;
	virtual LPCTSTR getLocaleKeyPostfix() const = 0;

	virtual void initSettings();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();


	void onChangeInCombo();
	void onChangeInRadioGroup();
	void onClickInCheck();
	CString makeComboItemKey(LPCTSTR item);
	CString makeControlKey(LPCTSTR item);

	void reloadCotrolCaption(UINT ctrlId, LPCTSTR ctrlPostfix);
	void initControls();
private:
	SettingsList settingsList_;
};

#endif // !defined(AFX_BASEPAGE_H__1AE33142_90A6_4140_9E93_A95B9C0ACBAC__INCLUDED_)
