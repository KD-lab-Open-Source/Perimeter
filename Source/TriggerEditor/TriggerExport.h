#ifndef __TRIGGER_EXPORT_H__
#define __TRIGGER_EXPORT_H__

#ifndef _FINAL_VERSION_
#ifdef _WIN32
#include <atltypes.h> //MFC stuff
#endif
#endif

#include "Serialization.h"
#define PERIMETER_SERIALIZATION_ARCHIVE_NEED
#include "SerializationVirtual.h"

//-----------------------------
//This was originally on Conditions.h, had to be moved since C++ now forbids forward declaring enums
enum AIPlayerType
{
    AI_PLAYER_TYPE_ME, // Я
    AI_PLAYER_TYPE_ENEMY, // Другой
    AI_PLAYER_TYPE_WORLD, // Мир
    AI_PLAYER_TYPE_ANY // Любой
};

DECLARE_ENUM_DESCRIPTOR(AIPlayerType)

enum CompareOperator
{
    COMPARE_LESS,	// Меньше
    COMPARE_LESS_EQ, // Меньше либо равно
    COMPARE_EQ, // Равно
    COMPARE_NOT_EQ, // Не равно
    COMPARE_GREATER, // Больше
    COMPARE_GREATER_EQ // Больше либо равно		 
};

DECLARE_ENUM_DESCRIPTOR(CompareOperator)

//-----------------------------
// Для отделения контента
#ifndef _SURMAP_
#define virtual_ virtual
#else
#define virtual_ 
#endif

//-----------------------------
class AIPlayer;
class terUnitBase;
class TreeNode;
class TriggerChain;
class Trigger;

//-----------------------------
struct Action;
struct Condition;

typedef ShareHandle<Action> ActionPtr;
typedef ShareHandle<Condition> ConditionPtr;

//-----------------------------
class TriggerInterface
{
public:
    virtual const char* actionComboList() = 0;  // |-separated
    virtual const char* conditionComboList() = 0; // |-separated
    virtual ActionPtr createAction(int typeIndex) = 0; // index in actionComboList
    virtual ConditionPtr createCondition(int typeIndex) = 0; // index in conditionComboList

    virtual const char* actionName(const Action& action) = 0;
    virtual const char* conditionName(const Condition& condition) = 0;

    virtual bool editCondition(Trigger& trigger, void* hwnd) = 0;
    virtual bool editTrigger(Trigger& trigger, void* hwnd) = 0;

    virtual void* malloc(size_t n) = 0;
    virtual void free(void* p) = 0;
};
TriggerInterface& triggerInterface();

//-----------------------------
template <class T>
class TriggerAllocator : public std::allocator<T> {
public:

  typedef T        value_type;
  typedef value_type *       pointer;
  typedef const T* const_pointer;
  typedef T&       reference;
  typedef const T& const_reference;
  typedef size_t     size_type;
  typedef ptrdiff_t  difference_type;

  template <class _Tp1> struct rebind {
    typedef TriggerAllocator<_Tp1> other;
  };
  TriggerAllocator() {}
  TriggerAllocator(const std::allocator<T>&) {}
  ~TriggerAllocator() {}
  
  pointer address(reference x) const { return &x; }
  const_pointer address(const_reference x) const { return &x; }
  
  T* allocate(size_type n, const void* = 0) { 
    return n != 0 ? reinterpret_cast<value_type*>(triggerInterface().malloc(n * sizeof(value_type))) : 0;
  }
  
  void deallocate(pointer p, size_type n) {
	  //xassert( (p == 0) == (n == 0) );
      if (p != 0) 
		  triggerInterface().free(p);
  }

  size_type max_size() const // _STLP_NOTHROW 
    { return size_t(-1) / sizeof(T); }
};

//-----------------------------
struct Condition : ShareHandleBaseSerializeVirtual // Не выполняется никогда (для выключения триггеров)
{
	Condition() {
		state_ = false; 
		internalColor_ = 0;
	}
	virtual ~Condition() {
	}

    const char* name() const;

	bool checkDebug(AIPlayer& aiPlayer) { 
		return state_ = check(aiPlayer); 
	}
	virtual_ void checkEvent(AIPlayer& aiPlayer, const class Event* event) {}
	virtual_ void clear() {}
	virtual_ void writeInfo(XBuffer& buffer, std::string offset) const {}

	bool state() const {
		return state_;
	}
	int color() const {
		return internalColor_;
	}
	
	bool inversed() const {
		return false;
	}

    static bool checkPlayer(const class terPlayer* palyer1, const terPlayer* player2, AIPlayerType playerType);
    static AIPlayer* getPlayer(AIPlayer& aiPlayer, AIPlayerType playerType);
    static bool compare(int op1, int op2, CompareOperator op);
    static bool compare(float op1, float op2, CompareOperator op);

    VIRTUAL_SERIALIZE(ar) {
		ar & TRANSLATE_OBJECT(state_, "_Текущее состояние");
		ar & WRAP_OBJECT(internalColor_);
	}

protected:
	bool state_; 
	int internalColor_;

	virtual_ bool check(AIPlayer& aiPlayer) { return false; }
};

struct ConditionNode //
{
	enum Type {
		NORMAL, // да
		INVERTED // НЕ
	};
	EnumWrapper<Type> type; 
	ShareHandle<Condition> condition; 

    ConditionNode() {
		type = NORMAL; 
	}

	void writeInfo(XBuffer& buffer, std::string offset) const;
	bool check(AIPlayer& aiPlayer) { return condition ? (type == NORMAL ? condition->checkDebug(aiPlayer) : !condition->checkDebug(aiPlayer)) : false; }

	SERIALIZE(ar) {
		ar & TRANSLATE_OBJECT(type, "&Тип");
		ar & TRANSLATE_OBJECT(condition, "&");
	}
};

DECLARE_ENUM_DESCRIPTOR_ENCLOSED(ConditionNode, Type);

struct ConditionSwitcher : Condition // И/ИЛИ
{
	enum Type {
		AND, // И
		OR // ИЛИ
	};
	EnumWrapper<Type> type;
	std::vector<ConditionNode, TriggerAllocator<ConditionNode> > conditions;

	ConditionSwitcher() {
		type = AND; 
	}

	bool check(AIPlayer& aiPlayer);
	void checkEvent(AIPlayer& aiPlayer, const Event* event);
	void clear();
	void writeInfo(XBuffer& buffer, std::string offset) const;

    VIRTUAL_SERIALIZE(ar) {
		Condition::serialize_template(ar);
		ar & TRANSLATE_OBJECT(type, "&Тип");
		ar & TRANSLATE_OBJECT(conditions, "Условия");
	}
};

DECLARE_ENUM_DESCRIPTOR_ENCLOSED(ConditionSwitcher, Type);

//-----------------------------
struct Action : ShareHandleBaseSerializeVirtual // Пустое действие, вставлять не надо!
{		
	int internalColor_;

	Action() {
		internalColor_ = 0;
	}

	virtual_ bool automaticCondition(AIPlayer& aiPlayer) const { return true; }
	virtual_ void activate(AIPlayer& aiPlayer) {}
	virtual_ bool workedOut(AIPlayer& aiPlayer) { return true; }
	virtual_ bool onlyIfAI() const { return false; }

	const char* name() const;

	VIRTUAL_SERIALIZE(ar) {
		ar & WRAP_OBJECT(internalColor_);
	}
};


//-----------------------------
struct CPointSerialized : sPoint
{
	CPointSerialized() {
		x = y = INT_MIN;
	}

	bool valid() const {
 		return x != INT_MIN || y != INT_MIN;
	}

	SERIALIZE(ar) {
		ar & TRANSLATE_OBJECT(x, "x");
		ar & TRANSLATE_OBJECT(y, "y");
	}
};

struct CRectSerialized : sRect
{
	CRectSerialized() {
		left = top = right = bottom = INT_MIN;
	}

	bool valid() const {
		return left != INT_MIN || right != INT_MIN || bottom != INT_MIN || top != INT_MIN;
	}

	SERIALIZE(ar) {
		ar & TRANSLATE_OBJECT(left, "left");
		ar & TRANSLATE_OBJECT(top, "top");
		ar & TRANSLATE_OBJECT(right, "right");
		ar & TRANSLATE_OBJECT(bottom, "bottom");
	}
};

enum ColorType
{
	STRATEGY_RED, // Красный
	STRATEGY_GREEN, // Зеленый
	STRATEGY_BLUE, // Синий
	STRATEGY_YELLOW, // Желтый
	STRATEGY_COLOR_0,
	STRATEGY_COLOR_1,
	STRATEGY_COLOR_2,
	STRATEGY_COLOR_3,
	STRATEGY_COLOR_4,
	STRATEGY_COLOR_5,
	STRATEGY_COLOR_MAX,
	LINK_TYPES_COUNT = STRATEGY_COLOR_MAX
};

DECLARE_ENUM_DESCRIPTOR(ColorType)

struct TriggerLink // Связь
{
	enum Type {
		THIN, // Тонкая
		THICK // Толстая
	};

	TriggerLink();

	void activate(TriggerChain& triggerChain);
	void deactivate(TriggerChain& triggerChain);

	const char* triggerName() const {
		return triggerName_;
	}
	void setTriggerName(const char* triggerName) {
		triggerName_ = triggerName;
	}
	
	bool active() const {
		return active_;
	}

	bool autoRestarted() const { 
		return autoRestarted_ == THICK; 
	}
	void setAutoRestarted(bool autoRestarted) {
		autoRestarted_ = autoRestarted ? THICK : THIN;
	}

	int getType() const {
		return colorType_;
	}
	void setType(int type)
	{
		colorType_ = static_cast<ColorType>(type);
	}

	const CPointSerialized& parentOffset() const {
		return parentOffset_;
	}
	void setParentOffset(const sPoint& offset) {
		static_cast<sPoint&>(parentOffset_) = offset;
	}
    
	const CPointSerialized& childOffset() const {
		return childOffset_;
	}
	void setChildOffset(const sPoint& offset) {
		static_cast<sPoint&>(childOffset_) = offset;
	}

	SERIALIZE(ar) {
		ar & WRAP_NAME(triggerName_, "triggerName");
		ar & WRAP_NAME(colorType_, "color");
		ar & WRAP_NAME(autoRestarted_, "type");
		ar & WRAP_OBJECT(active_);
		
		ar & WRAP_NAME(parentOffset_, "parentOffset");
		ar & WRAP_NAME(childOffset_, "childOffset");
    }

	Trigger* parent;
	Trigger* child;

private:
	PrmString triggerName_;
	bool active_; 
	EnumWrapper<ColorType> colorType_; 
	EnumWrapper<Type> autoRestarted_;

	CPointSerialized parentOffset_;
	CPointSerialized childOffset_;

	friend class TriggerChain;
};

DECLARE_ENUM_DESCRIPTOR_ENCLOSED(TriggerLink, Type)

typedef std::vector<TriggerLink, TriggerAllocator<TriggerLink> > OutcomingLinksList;
typedef std::vector<TriggerLink*, TriggerAllocator<TriggerLink*> > IncomingLinksList;

class Trigger // Триггер
{
public:
	enum State {
		SLEEPING, // Проверяет входные связи
		CHECKING, // Проверяет условия
		WORKING, // Выполняется
		DONE // Выполнен
	};

	Trigger();

	void initialize();

	void quant(AIPlayer& aiPlayer, TriggerChain& triggerChain);
	void checkEvent(AIPlayer& aiPlayer, const Event* event);
	void activate(AIPlayer& aiPlayer, TriggerChain& triggerChain);
	bool removeLinkByChild(Trigger* child);

	const char* displayText() const;
	const char* debugDisplayText() const;
    const char* logText() const;
	bool active() const;

	bool isChild(const Trigger& child) const;

	const char* name() const {
		return name_;
	}
	void setName(const char* name);

	State state() const {
		return state_;
	}
	void setState(State state) {
		state_ = state;
	}
	
	int color() const {
		return internalColor_;
	}
	void setColor(int color) {
		internalColor_ = color;
	}

	bool selected() const {
		return selected_;
	}
	void setSelected(bool selected) {
		selected_ = selected;
	}

	const CPointSerialized& cellIndex() const {
		return cellIndex_;
	}
	void setCellIndex(int x, int y) {
		cellIndex_.x = x;
        cellIndex_.y = y;
	}
	void setCellIndex(const sPoint& cellIndex) {
		static_cast<sPoint&>(cellIndex_) = cellIndex;
	}

	const CRectSerialized& boundingRect() const {
		return boundingRect_;
	}
	void setBoundingRect(const sRect& boundingRect) {
		static_cast<sRect&>(boundingRect_) = boundingRect;
	}

	const OutcomingLinksList& outcomingLinks() const {
		return outcomingLinks_;
	}
	OutcomingLinksList& outcomingLinks() {
		return outcomingLinks_;
	}

	const IncomingLinksList& incomingLinks() const {
		return incomingLinks_;
	}
	IncomingLinksList& incomingLinks() {
		return incomingLinks_;
	}

	SERIALIZE(ar) {
		ar & TRANSLATE_NAME(name_, "name", "&Имя");	
		ar & TRANSLATE_OBJECT(condition, "");
		ar & TRANSLATE_OBJECT(action, "Действие");
		ar & TRANSLATE_NAME(outcomingLinks_, "outcomingLinks", "_Исходящие связи");
		ar & TRANSLATE_OBJECT(state_, "_Текущее состояние");
		ar & TRANSLATE_OBJECT(executionCounter_, "_Запускался");
		ar & WRAP_OBJECT(internalColor_);
		
		ar & WRAP_NAME(cellIndex_, "cellIndex");
		ar & WRAP_NAME(boundingRect_, "boundingRect");

        if(ar.isInput()) {
            initialize();
        }
	}

	ShareHandle<Condition> condition; 
	ShareHandle<Action> action; 

private:
	PrmString name_; 
	OutcomingLinksList outcomingLinks_; 
	IncomingLinksList incomingLinks_; 
	EnumWrapper<State> state_; 
	int executionCounter_;
	int internalColor_;
	
	CPointSerialized cellIndex_;
	CRectSerialized boundingRect_;
	bool selected_;

	friend TriggerChain;
};

DECLARE_ENUM_DESCRIPTOR_ENCLOSED(Trigger, State)

//-----------------------------
struct TriggerEvent
{
	PrmString event;
	int index;
	EnumWrapper<Trigger::State> state;

	TriggerEvent() {
		index = 0;
		state = Trigger::SLEEPING;
	}

	SERIALIZE(ar) {
		ar & WRAP_OBJECT(event);
		ar & WRAP_OBJECT(index);
		ar & WRAP_OBJECT(state);
	}	
};

typedef std::vector<Trigger, TriggerAllocator<Trigger> > TriggerList;

//-----------------------------
class TriggerChain // Стратегия игрока
{
public:
	typedef std::vector<TriggerEvent, TriggerAllocator<TriggerEvent> > TriggerEventList;

	PrmString name;
	TriggerList triggers; // 0-й trigger - стартовый

	TriggerChain();
	
	void load(const char* name);
	void save() const;
    void initialize();

	TriggerChain& operator=(const TriggerChain& rhs);
	bool operator==(const TriggerChain& rhs) const;

	void buildLinks();
	void quant(AIPlayer& aiPlayer);
	void checkEvent(AIPlayer& aiPlayer, const Event* event);
	Trigger* find(const char* name);

	bool removeLink(int parentIndex, int childIndex);
	TriggerLink* addLink(int parentIndex, int childIndex, int type);

	Trigger* addTrigger(Trigger const& trigger);
	Trigger* insertTrigger(int pos, Trigger const& triger);
	void removeTrigger(int triggerIndex);
	int triggerIndex(const Trigger& trigger) const;

	void setLogData(const TriggerEventList& data);
	const TriggerEventList& logData() const { return triggerEvents_; }
	bool isLogValid() const;

	int numLogRecords() const { return triggerEvents_.size(); }
	const char* logRecordText(int index) const { return triggerEvents_[index].event; }
	void setLogRecord(int index);
	void addLogRecord(const Trigger& trigger, const char* event);

	void initializeTriggersAndLinks();

	void activateTrigger(Trigger* trigger);
	void deactivateTrigger(Trigger* trigger);

	const CRectSerialized& boundingRect() const {
		return boundingRect_;
	}
	void setBoundingRect(const sRect& boundingRect) {
		static_cast<sRect&>(boundingRect_) = boundingRect;
	}

	const CRectSerialized& viewRect() const {
		return viewRect_;
	}
	void setViewRect(const sRect& viewRect) {
		static_cast<sRect&>(viewRect_) = viewRect;
	}

	SERIALIZE(ar) {
		ar & TRANSLATE_OBJECT(name, "Имя");
		ar & TRANSLATE_OBJECT(triggers, "Триггера");

		ar & WRAP_NAME(boundingRect_, "boundingRect");
		ar & WRAP_NAME(viewRect_, "viewRect");

        if(ar.isInput()) {
            initialize();
        }
	}

private:
	CRectSerialized boundingRect_;
	CRectSerialized viewRect_;

	TriggerEventList triggerEvents_;

	void initTriggersState();

	typedef std::vector<Trigger*, TriggerAllocator<Trigger*> > ActiveTriggers;
	ActiveTriggers activeTriggers_;
};

//-----------------------------

#endif //__TRIGGER_EXPORT_H__
