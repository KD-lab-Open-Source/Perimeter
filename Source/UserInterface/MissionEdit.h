#ifndef __MISSION_EDITOR_H__
#define __MISSION_EDITOR_H__


class MissionEditor
{
public:
	MissionEditor();
	~MissionEditor();

	bool keyPressed(const sKey& Key);
	bool keyUnpressed(const sKey& Key);
	bool mouseMove(const Vect2f& pos);
	bool mouseLeftPressed(const Vect2f& pos);
	bool mouseRightPressed(const Vect2f& pos);

	void quant();
	void show();
	const char* info();

	bool hardnessChanged() const { return hardnessChanged_; }
	void clearHardnessChanged() { hardnessChanged_ = false; }

private:
	std::string info_;

	terUnitAttributeID copiedID_;
	struct SaveUnitData* copiedData_;

	bool showHardness_;
	bool editingHardness_;
	Vect3fVect hardnessPolygon_;
	bool hardnessChanged_;

	void createUnit();
	void copyUnit();
	void pasteUnit();

	void setPlayer(int playerID);

	void applyHardness();
	void clearHardness();
};

#endif //__MISSION_EDITOR_H__