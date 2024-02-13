#pragma once
//Позволяет оповещать объекты о смене состояния.
class Observer;
class ObserverLink
{
public:
	inline ObserverLink()
	{
		observer=NULL;
	}

	inline ~ObserverLink();
	virtual void Update(){};
protected:
	Observer* observer;
	friend class Observer;

	inline void SetLink(Observer* o)
	{
		VISASSERT(observer==NULL);
		observer=o;
	}

	inline void ClearLink(Observer* o)
	{
		VISASSERT(o==observer);
		observer=NULL;
	}
};

class Observer
{
	friend class ObserverLink;
	std::vector<ObserverLink*> links;
public:
	Observer()
	{
	}

	~Observer()
	{
		std::vector<ObserverLink*>::iterator it;
		FOR_EACH(links,it)
		{
			(*it)->ClearLink(this);
		}
	}

	void AddLink(ObserverLink* link)
	{
#ifdef PERIMETER_DEBUG_ASSERT
		std::vector<ObserverLink*>::iterator it = std::find(links.begin(),links.end(),link);
		if(it!=links.end())
			VISASSERT(0);
		else
			links.push_back(link);
#else
		links.push_back(link);
#endif
		link->SetLink(this);
	}

	void BreakLink(ObserverLink* link)
	{
		std::vector<ObserverLink*>::iterator it = std::find(links.begin(),links.end(),link);
		if(it!=links.end())
		{
			links.erase(it);
		}else
			VISASSERT(0);
	}

	void UpdateLink()
	{
		std::vector<ObserverLink*>::iterator it;
		FOR_EACH(links,it)
		{
			(*it)->Update();
		}
	}

	bool empty()
	{
		return links.empty();
	}
};


/////////////
ObserverLink::~ObserverLink()
{
	if(observer)
		observer->BreakLink(this);
}
