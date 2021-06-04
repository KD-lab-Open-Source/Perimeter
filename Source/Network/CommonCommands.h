#ifndef _PERIMETER_COMMON_COMMANDS_
#define _PERIMETER_COMMON_COMMANDS_

enum terUnitAttributeID;
enum CommandID;

const int PERIMETER_REGION_DATA_SIZE = 64;

//-------------------------------

class terUnitID
{
public:
	terUnitID(unsigned int unit_id = 0, unsigned int player_id = 0)
	{
		unitID_ = unit_id;
		playerID_ = player_id;
	}

	unsigned int playerID() const { return playerID_; }
	unsigned int unitID() const { return unitID_; }

	bool operator == (const terUnitID &id) const { return (unitID_ == id.unitID_ && playerID_ == id.playerID_); }

	friend XBuffer& operator < (XBuffer& in, const terUnitID& id) { in < id.unitID_ < id.playerID_; return in; }
	friend XBuffer& operator > (XBuffer& out, terUnitID& id) { out > id.unitID_ > id.playerID_; return out; }

private:
	unsigned int unitID_;
	unsigned int playerID_;
};

enum CommandSelectionMode
{
	COMMAND_SELECTED_MODE_SINGLE = 0,
	COMMAND_SELECTED_MODE_NEGATIVE
};


//-------------------------------
class UnitCommand
{
public:
	UnitCommand(){}
	UnitCommand(CommandID commandID, int commandData, CommandSelectionMode selectionMode = COMMAND_SELECTED_MODE_NEGATIVE);
	UnitCommand(CommandID commandID, const Vect3f& position, int commandData, CommandSelectionMode selectionMode = COMMAND_SELECTED_MODE_NEGATIVE);
	UnitCommand(CommandID commandID, class terUnitBase* unit, int commandData, CommandSelectionMode selectionMode = COMMAND_SELECTED_MODE_NEGATIVE);
	
	CommandSelectionMode selectionMode() const { return selectionMode_; }
	CommandID commandID() const { return commandID_; }
	
	const Vect3f& position() const { return position_; }
	terUnitBase* unit() const { return unit_; }
	
	int commandData() const { return commandData_; }
	
	bool operator==(const UnitCommand& command) const { return selectionMode() == command.selectionMode() 
		&& commandID() == command.commandID()
		&& commandData() == command.commandData() && position() == command.position()
		&& unitID_ == command.unitID_;
	}

	void prepareToSend();
	void prepareAfterSend();
	void Write(XBuffer& out) const{
		out.write(&selectionMode_, sizeof(selectionMode_));
		out.write(&commandID_, sizeof(commandID_));
		out.write(&commandData_, sizeof(commandData_));
		out.write(&position_, sizeof(position_));
		out.write(&unitID_, sizeof(unitID_));
	}
	void Read(XBuffer& in) {
		in.read(&selectionMode_, sizeof(selectionMode_));
		in.read(&commandID_, sizeof(commandID_));
		in.read(&commandData_, sizeof(commandData_));
		in.read(&position_, sizeof(position_));
		in.read(&unitID_, sizeof(unitID_));
	}
	UnitCommand& operator =(const UnitCommand& donor){
		selectionMode_=donor.selectionMode_;
		commandID_=donor.commandID_;
		commandData_=donor.commandData_;
		position_=donor.position_;
		unitID_=donor.unitID_;
		unit_=donor.unit_; // ! отличие от Read Write
		return *this;
	}

	
private:
	CommandSelectionMode selectionMode_;
	CommandID commandID_;
	
	Vect3f position_;
	terUnitBase* unit_;
	terUnitID unitID_;
	unsigned int commandData_;
};
#endif

