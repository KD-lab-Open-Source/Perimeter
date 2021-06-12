#ifndef _SLOT_MANAGER_H
#define _SLOT_MANAGER_H

#include <list>
#include <Compound3D.hpp>
#include "Slot.h"

using namespace tx3d;

class SlotManager {

	public :

		SlotManager();
		virtual ~SlotManager();

		Slot* addNewPainter(cScene* scene);
		Slot* addNewMixer(cScene* scene);

		void addPainter(Slot* painter);
		void addMixer(Slot* mixer);

		void removePainter(Slot* painter);
		void removeMixer(Slot* mixer);

		void moveToPainters(Slot* mixer);
		void moveToMixers(Slot* painter);

		int getPainterCount();
		int getMixerCount();

		Slot* getPainter(int index);
		Slot* getMixer(int index);

		Texture3D* assembleTexture();
		bool parseTexture(Texture3D* texture, cScene* scene);

		static bool isTextureSimple(Texture3D* texture);


	protected:

		void addPainters( list<Slot*>& slotList );
		void addMixers( list<Slot*>& slotList );

		static void clearSlotList( list<Slot*>& slotList );
		static void slotListToCompound3D( list<Slot*>& slotList, Compound3D* compound );
		bool compound3DToSlotList( Compound3D* compound, list<Slot*>& slotList, cScene* scene );
		bool texture3DToSlotList( Texture3D* texture, list<Slot*>& slotList, cScene* scene );

		list<Slot*> painters;
		list<Slot*> mixers;
		Texture3D* currentAssembledTexture;

};

#endif //_SLOT_MANAGER_H

