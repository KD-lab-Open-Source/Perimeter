#pragma once

struct LightingParameters
{
	float generate_time;
	std::string texture_name;
	float strip_width_begin;
	float strip_width_time;
	float strip_length;
	float fade_time;
    float min_amplitude;
    float max_amplitude;
	LightingParameters();
};

class cLighting:public cIUnkClass
{
public:
	cLighting();
	~cLighting();
	void Init(Vect3f pos_begin, std::vector<Vect3f>& pos_end,float scaler);
	void SetParameters(LightingParameters& param_);

	virtual void PreDraw(cCamera *UCamera);
	virtual void Draw(cCamera *UCamera);
	virtual void Animate(float dt);

	const MatXf& GetPosition(void) const
	{ 
		return global_pos;
	}
protected:
	void Generate(Vect3f pos_begin,Vect3f pos_end,cCamera *pCamera);
	float time;
    float scaler;
	MatXf global_pos;
	Vect3f pos_begin;
	std::vector<Vect3f> pos_end;
	LightingParameters param;

	cTexture* pTexture;
	struct OneStrip
	{
		Vect3f pos;
		Vect3f n;
		float u;
	};

	struct OneLight
	{
		Vect3f pos_begin,pos_end;
		std::vector<Vect3f> position;
		std::vector<OneStrip> strip_list;
		float time;
		OneLight();
		~OneLight();

		void Generate(Vect3f pos_begin,Vect3f pos_end,cCamera *pCamera,cLighting* parent);
		void Draw(cCamera *pCamera,cLighting* parent);
		void Animate(float dt);
		void GenerateInterpolate(std::vector<float>& pos,int size,float min_amp, float max_amp);
		float get(std::vector<float>& p,float t);

		void BuildStrip(cCamera *pCamera,cLighting* parent);
	};
	friend OneLight;
	std::list<OneLight*> lights;

	struct PreGenerate
	{
		Vect3f pos_begin;
		Vect3f pos_end;
	};
	std::vector<PreGenerate> pre_generate;
};
