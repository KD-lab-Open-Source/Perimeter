#ifndef __TRIGGER_EXPORT_H__
#define __TRIGGER_EXPORT_H__

#include <atltypes.h>
#include "Serialization.h"

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

enum AIPlayerType;
enum CompareOperator;

//-----------------------------
template <class T>
class TriggerAllocator {
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
  TriggerAllocator(const allocator<T>&) {}
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
struct Condition : ShareHandleBase // Не выполняется никогда (для выключения триггеров)
{
	Condition() {
		state_ = false; 
		internalColor_ = 0;
	}
	virtual ~Condition() {
	}

	bool checkDebug(AIPlayer& aiPlayer) { 
		return state_ = check(aiPlayer); 
	}
	virtual_ void checkEvent(AIPlayer& aiPlayer, const class Event& event) {}
	virtual_ void clear() {}
	virtual_ void writeInfo(XBuffer& buffer, string offset) const {}

	bool state() const {
		return state_;
	}
	int color() const {
		return internalColor_;
	}
	
	bool inversed() const {
		return false;
	}
    
	template<class Archive>	
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(state_, "_Текущее состояние");
		ar & WRAP_OBJECT(internalColor_);
	}

	static bool checkPlayer(const class terPlayer* palyer1, const terPlayer* player2, AIPlayerType playerType);
	static AIPlayer* getPlayer(AIPlayer& aiPlayer, AIPlayerType playerType);
	static bool compare(int op1, int op2, CompareOperator op);
	static bool compare(float op1, float op2, CompareOperator op);

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

	void writeInfo(XBuffer& buffer, string offset) const;
	bool check(AIPlayer& aiPlayer) { return condition ? (type == NORMAL ? condition->checkDebug(aiPlayer) : !condition->checkDebug(aiPlayer)) : false; }

	template<class Archive>	
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(type, "&Тип");
		ar & TRANSLATE_OBJECT(condition, "&");
	}
};

struct ConditionSwitcher : Condition // И/ИЛИ
{
	enum Type {
		AND, // И
		OR // ИЛИ
	};
	EnumWrapper<Type> type;
	vector<ConditionNode, TriggerAllocator<ConditionNode> > conditions; 

	ConditionSwitcher() {
		type = AND; 
	}

	bool check(AIPlayer& aiPlayer);
	void checkEvent(AIPlayer& aiPlayer, const Event& event);
	void clear();
	void writeInfo(XBuffer& buffer, string offset) const;

	template<class Archive>	
	void serialize(Archive& ar) {
		Condition::serialize(ar);
		ar & TRANSLATE_OBJECT(type, "&Тип");
		ar & TRANSLATE_OBJECT(conditions, "Условия");
	}
};

//-----------------------------
struct Action : ShareHandleBase // Пустое действие, вставлять не надо!
{		
	int internalColor_;

	Action() {
		internalColor_ = 0;
	}
	virtual ~Action() {}

	virtual_ bool automaticCondition(AIPlayer& aiPlayer) const { return true; }
	virtual_ void activate(AIPlayer& aiPlayer) {}
	virtual_ bool workedOut(AIPlayer& aiPlayer) { return true; }
	virtual_ bool onlyIfAI() const { return false; }

	const char* name() const;

	template<class Archive>	
	void serialize(Archive& ar) {
		ar & WRAP_OBJECT(internalColor_);
	}
};


//-----------------------------
struct CPointSerialized : CPoint
{
	CPointSerialized() {
		x = y = INT_MIN;
	}

	bool valid() const {
 		return x != INT_MIN || y != INT_MIN;
	}

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(x, "x");
		ar & TRANSLATE_OBJECT(y, "y");
	}
};

struct CRectSerialized : CRect
{
	CRectSerialized() {
		left = top = right = bottom = INT_MIN;
	}

	bool valid() const {
		return left != INT_MIN || right != INT_MIN || bottom != INT_MIN || top != INT_MIN;
	}

	template<class Archive>
	void serialize(Archive& ar) {
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
	void setParentOffset(const CPoint& offset) {
		static_cast<CPoint&>(parentOffset_) = offset;
	}
    
	const CPointSerialized& childOffset() const {
		return childOffset_;
	}
	void setChildOffset(const CPoint& offset) {
		static_cast<CPoint&>(childOffset_) = offset;
	}

	template<class Archive>	
	void serialize(Archive& ar) {
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

typedef vector<TriggerLink, TriggerAllocator<TriggerLink> > OutcomingLinksList;
typedef vector<TriggerLink*, TriggerAllocator<TriggerLink*> > IncomingLinksList;

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
	void checkEvent(AIPlayer& aiPlayer, const Event& event);
	void activate(AIPlayer& aiPlayer, TriggerChain& triggerChain);
	bool removeLinkByChild(Trigger* child);

	const char* displayText() const;
	const char* debugDisplayText() const;
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
		cellIndex_.SetPoint(x, y);
	}
	void setCellIndex(const CPoint& cellIndex) {
		static_cast<CPoint&>(cellIndex_) = cellIndex;
	}

	const CRectSerialized& boundingRect() const {
		return boundingRect_;
	}
	void setBoundingRect(const CRect& boundingRect) {
		static_cast<CRect&>(boundingRect_) = boundingRect;
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

	template<class Archive>	
	void serialize(Archive& ar) {
		ar & TRANSLATE_NAME(name_, "name", "&Имя");	
		ar & TRANSLATE_OBJECT(condition, "");
		ar & TRANSLATE_OBJECT(action, "Действие");
		ar & TRANSLATE_NAME(outcomingLinks_, "outcomingLinks", "_Исходящие связи");
		ar & TRANSLATE_OBJECT(state_, "_Текущее состояние");
		ar & TRANSLATE_OBJECT(executionCounter_, "_Запускался");
		ar & WRAP_OBJECT(internalColor_);
		
		ar & WRAP_NAME(cellIndex_, "cellIndex");
		ar & WRAP_NAME(boundingRect_, "boundingRect");
		
		initialize();
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

	template<class Archive>	
	void serialize(Archive& ar) {
		ar & WRAP_OBJECT(event);
		ar & WRAP_OBJECT(index);
		ar & WRAP_OBJECT(state);
	}	
};

typedef vector<Trigger, TriggerAllocator<Trigger> > TriggerList;

//-----------------------------
class TriggerChain // Стратегия игрока
{
public:
	typedef vector<TriggerEvent, TriggerAllocator<TriggerEvent> > TriggerEventList;

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
	void checkEvent(AIPlayer& aiPlayer, const Event& event);
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

	void initializeCameraTrigger(int playerIndex);
	void initializeTriggersAndLinks();

	void activateTrigger(Trigger* trigger);
	void deactivateTrigger(Trigger* trigger);

	const CRectSerialized& boundingRect() const {
		return boundingRect_;
	}
	void setBoundingRect(const RECT& boundingRect) {
		static_cast<RECT&>(boundingRect_) = boundingRect;
	}

	const CRectSerialized& viewRect() const {
		return viewRect_;
	}
	void setViewRect(const RECT& viewRect) {
		static_cast<RECT&>(viewRect_) = viewRect;
	}

	template<class Archive>	
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(name, "Имя");
		ar & TRANSLATE_OBJECT(triggers, "Триггера");

		ar & WRAP_NAME(boundingRect_, "boundingRect");
		ar & WRAP_NAME(viewRect_, "viewRect");

		initialize();
	}

private:
	CRectSerialized boundingRect_;
	CRectSerialized viewRect_;

	TriggerEventList triggerEvents_;

	void initTriggersState();

	typedef vector<Trigger*, TriggerAllocator<Trigger*> > ActiveTriggers;
	ActiveTriggers activeTriggers_;
};

//-----------------------------
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

	virtual bool editCondition(Trigger& trigger, HWND hwnd) = 0;
	virtual bool editTrigger(Trigger& trigger, HWND hwnd) = 0;

	virtual void* malloc(size_t n) = 0;
	virtual void free(void* p) = 0;
};

TriggerInterface& triggerInterface();

//-----------------------------

#endif //__TRIGGER_EXPORT_H__