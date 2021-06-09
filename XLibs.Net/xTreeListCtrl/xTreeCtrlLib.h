#ifndef __XTREE_CTRL_LIB_H__
#define __XTREE_CTRL_LIB_H__
#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef _TREE_LIST_NO_AUTOMATIC_LINK_

#ifdef _MT
#define _MT_SUFFIX "_MT"
#else //_MT
#define _MT_SUFFIX 
#endif//_MT

#ifdef _DEBUG
#define _DEBUG_SUFFIX "_D"
#else //_DEBUG
#define _DEBUG_SUFFIX 
#endif //_DEBUG

#ifdef _AFXDLL
#define _CRT_MFC_TYPE "_SH"
#else //_AFXDLL
#define _CRT_MFC_TYPE "_ST"
#endif //_AFXDLL

#if (_MSC_VER == 1200)

#define _VC_SUFFIX "VC6"

#elif (_MSC_VER == 1300)

#define _VC_SUFFIX "VC7"
#ifndef _MT
#error No single thread version  for VC7
#endif//_MT

#elif (_MSC_VER > 1300)

#define _VC_SUFFIX "VC71"

#ifndef _MT
#error No single thread version  for VC7.1
#endif//_MT

#endif //(_MSC_VER >= 1300)

#define _LIB_NAME "xTreeListCtrl"_VC_SUFFIX _CRT_MFC_TYPE _MT_SUFFIX _DEBUG_SUFFIX".lib"
#pragma message("Automatically linking with " _LIB_NAME ) 
#pragma comment(lib,_LIB_NAME) 

#undef _VC_SUFFIX
#undef _DEBUG_SUFFIX
#undef _MT_SUFFIX
#undef _LIB_NAME

#endif //_TREE_LIST_NO_AUTOMATIC_LINK_
#include "h/EditItemCtrlBase.h"
#include "h/TreeListColumnInfo.h"
#include "h/TreeListItem.h"
#include "h/TreeListHeaderCtrl.h"
#include "h/TreeListTipCtrl.h"
#include "h/TreeListStaticCtrl.h"
#include "h/TreeListEditCtrl.h"
#include "h/TreeListComboCtrl.h"
#include "h/TreeListCtrl.h"
#include "h/ICustomDrawNotifyListener.h"
#include "h/NullCustomDrawNotifyListener.h"

#endif//__XTREE_CTRL_LIB_H__