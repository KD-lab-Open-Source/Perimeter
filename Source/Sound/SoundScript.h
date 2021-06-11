#ifndef __SOUND_SCRIPT_H__
#define __SOUND_SCRIPT_H__

#include "Handle.h"
#include "Serialization.h"

struct SoundDeltaPrm 
{
	bool enable;
	bool random;	// �������� ������� ����� �������� ��� ��� �������
			
	float up; // �� ������� ��� ������� ����� ����� ������������� (0..2)
	float down; //�� ������� ��� ������� ����� ����� ����������� (0..20)

	SoundDeltaPrm() {
		enable = false;
		random = false;	
		up = 1.5; 
		down = 0.5; 
	}

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(enable, "&��������");
		ar & TRANSLATE_OBJECT(random, "�������� ������� ����� �������� ��� ��� �������");
		ar & TRANSLATE_OBJECT(up, "�� ������� ��� ������� ����� ����� ������������� (0..2)");
		ar & TRANSLATE_OBJECT(down, "�� ������� ��� ������� ����� ����� ����������� (0..20)");
	}
};

struct SoundSetupPrm
{
	PrmString name;
	float volume; // ��������� ����� 0..1 (�� ���������: 1)
	float volmin; // ����������� ��������� ����� 0..1 (�� ���������: 0)
	
	bool language_dependency; // 1 ���� ���� ������ ��� ������ ������
	bool belligerent_dependency; // 1 ���� ���� ������ ��� ������ ������� ������
	
	PrmString soundName; //�������� ����� 
	vector<PrmString> additionalNames; //�������� ����� (����� ����� ����� ���� ���������, ����� ���� ���������� �� rnd)
	float radius; //�������� ������� (������ � 3D) � ����� ������� ��������� ����� �������� ������� (�� ���������: 50)
	float max_radius; //������������ ���������� �� �������, ����� �������� ���� �������� ���� �������� (�� ���������: oo)

	//������������ ���������� ������������ �������� ������ (�� ���������: 5)
	//� ������ 2D ��� ������� ��������� ������, ��� max_num_sound ������ - ���� �� �����������.
	//� ������ 3D:
	//    ���� max_num_sound == 1, ����������� ������ 1 ����, ����������� �� �����������.
	//    ���� max_num_sound>1, ����������� ��� �����, �� ������ ������ ���������.
	int max_num_sound;

	SoundDeltaPrm delta;

	SoundSetupPrm() {
		volume = 1.0; 
		volmin = 0.0; 
	
		language_dependency = false; 
		belligerent_dependency = false; 
	
		radius = 100; 
		max_radius = -1; 

		max_num_sound = 25;
	}

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(name, "&name");
		ar & TRANSLATE_OBJECT(volume, "��������� ����� 0..1 (�� ���������: 1)");
		ar & TRANSLATE_OBJECT(volmin, "����������� ��������� ����� 0..1 (�� ���������: 0)");
	
		ar & TRANSLATE_OBJECT(language_dependency, "���� ������ ��� ������ ������");
		ar & TRANSLATE_OBJECT(belligerent_dependency, "���� ������ ��� ������ ������� ������");

		ar & TRANSLATE_OBJECT(soundName, "�������� �����");
		ar & TRANSLATE_OBJECT(additionalNames, "�������������� ����� ��� RND");

		ar & TRANSLATE_OBJECT(radius, "�������� ������� (������ � 3D) � ����� ������� ��������� ����� �������� ������� (�� ���������: 50)");
		ar & TRANSLATE_OBJECT(max_radius, "������������ ���������� �� �������, ����� �������� ���� �������� ���� �������� (�� ���������: oo)");

		ar & TRANSLATE_OBJECT(max_num_sound, "������������ ���������� ������������ �������� ������ (�� ���������: 5)");

		ar & TRANSLATE_OBJECT(delta, "������");
	}
};

struct SoundScriptPrm
{
	bool is3D; // 1 ���� ����� ���������
	PrmString name;
	vector<SoundSetupPrm> data;

	SoundScriptPrm() {
		is3D = false; 
	}

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(name, "&name");
		ar & TRANSLATE_OBJECT(is3D, "����� ���������");
		ar & TRANSLATE_OBJECT(data, "�����");
	}
};

struct SoundScriptTable
{
	// ������� ����� ����� ��� ������, ������� ������ ��� ������ ������� ������.
	vector<PrmString> belligerentPrefix;
	vector<SoundScriptPrm> table;

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(table, "table");
		ar & TRANSLATE_OBJECT(belligerentPrefix, "������� ����� ����� ��� ������, ������� ������ ��� ������ ������� ������");
	}
};

extern SingletonPrm<SoundScriptTable> soundScriptTable;

DEFINE_SINGLETON_PRM(SoundScriptTable)

#endif //__SOUND_SCRIPT_H__