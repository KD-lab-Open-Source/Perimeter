#ifndef _ENERGY_USER_H_
#define _ENERGY_USER_H_

class terPlayer;

class EnergyConsumer
{
public:
	EnergyConsumer();
	~EnergyConsumer();

	void attach(terPlayer* player, const ConsumptionData& data);
	void changePlayer(terPlayer* player);
	void detach();
	float requestEnergy(class terEnergyDataType& energyData);
	void setDetached() { player_ = 0; }
	bool finished() const { return accumulative_ && charged(); }
	bool attached() const { return player_; }

	void start(const ConsumptionData& data);
	void requestCharge() { requested_ = true; } // Обязательно запрашивать каждый квант
	bool charged() const { return energy_ < FLT_EPS; } // Для мгновенных процессов первый вызов после старта всегда true
	float progress() const { return energy()/total_; }
	float energy() const { return total_ - energy_; }

	bool discharge(float delta){ energy_ += delta; if(energy_ > total_){ energy_ = total_; return true; } return false; }

	void setProgress(float progress);

	const char* name() const { return name_; }

private:
	terPlayer* player_;
	bool requested_;
	bool accumulative_;
	float total_;
	float energy_;
	float delta_;
	int priority_;
	const char* name_;
};

#endif // _ENERGY_USER_H_
