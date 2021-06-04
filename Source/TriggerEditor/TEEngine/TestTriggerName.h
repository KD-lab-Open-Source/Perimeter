#ifndef __TEST_TRIGGER_NAME_H__
#define __TEST_TRIGGER_NAME_H__

class TestTriggerName 
{
public:
	
	TestTriggerName(CString const& name) : name_(name) {
	}
	
	bool operator()(Trigger const& trigger) const {
		return !strcmp(trigger.name(), name_);
	}

	bool operator()(TriggerLink const& link) const {
		return (!_tcscmp(link.triggerName(), name_));
	}

private:
	CString const& name_;
};

#endif //__TEST_TRIGGER_NAME_H__