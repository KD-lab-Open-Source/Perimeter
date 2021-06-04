#ifndef _CSLOTMENU_H
#define _CSLOTMENU_H

class CSlotMenu : public CMenu {

	public:
		CSlotMenu();
		virtual ~CSlotMenu();
		void init();

		virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMIS);
		virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);

	protected:
		CMenu parent;

};

#endif //_CSLOTMENU_H
