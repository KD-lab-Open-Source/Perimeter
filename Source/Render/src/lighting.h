#pragma once

struct LightingParameters
{
	float generate_time;
	string texture_name;
	float strip_width_begin;
	float strip_width_time;
	float strip_length;
	float fade_time;
	float lighting_amplitude;
	LightingParameters();
};

class cLighting:public cIUnkClass
{
public:
	cLighting();
	~cLighting();
	void Init(Vect3f pos_begin,vector<Vect3f>& pos_end);
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
	MatXf global_pos;
	Vect3f pos_begin;
	vector<Vect3f> pos_end;
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
		vector<Vect3f> position;
		vector<OneStrip> strip_list;
		float time;
		OneLight();
		~OneLight();

		void Generate(Vect3f pos_begin,Vect3f pos_end,cCamera *pCamera,cLighting* parent);
		void Draw(cCamera *pCamera,cLighting* parent);
		void Animate(float dt);
		void GenerateInterpolate(vector<float>& pos,int size,float amplitude);
		float get(vector<float>& p,float t);

		void BuildStrip(cCamera *pCamera,cLighting* parent);
	};
	friend OneLight;
	list<OneLight*> lights;

	struct PreGenerate
	{
		Vect3f pos_begin;
		Vect3f pos_end;
	};
	vector<PreGenerate> pre_generate;
};
