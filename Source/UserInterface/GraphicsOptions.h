#pragma once

#ifndef _GRAPHICSOPTIONS_H
#define _GRAPHICSOPTIONS_H

#include "SourceUIResolution.h"

class Options {
	public:
		virtual void load(const char* sectionName, const char* iniFileName) = 0;
		virtual void save(const char* iniFileName) = 0;
		virtual void apply() = 0;
};

class CustomGraphOptions : public Options {
	public:
		void load(const char* sectionName, const char* iniFileName);
		void save(const char* iniFileName);
		void apply();

		int landscapeDetails;

		bool mapReflections;
		bool objReflections;


		bool enableOcclusion;
		bool enablePointLight;

		int shadowQuality;
		int shadowType;
		int shadowSamples;

		bool bumpMapping;
		bool bumpChaos;
		float particleRate;

		bool compressedTextures;

		bool operator == (const CustomGraphOptions &cgo) const {
			return (
						landscapeDetails == cgo.landscapeDetails
					&&	mapReflections == cgo.mapReflections
					&&	objReflections == cgo.objReflections
					&&	enableOcclusion == cgo.enableOcclusion
					&&	enablePointLight == cgo.enablePointLight
					&&	shadowQuality == cgo.shadowQuality
					&&	shadowType == cgo.shadowType
					&&	shadowSamples == cgo.shadowSamples
					&&	bumpMapping == cgo.bumpMapping
					&&	bumpChaos == cgo.bumpChaos
					&&	particleRate == cgo.particleRate
					&&	compressedTextures == cgo.compressedTextures
				);
		};
		bool operator != (const CustomGraphOptions &cgo) const {
			return !(*this == cgo);
		};
};

class GraphOptions : public Options {
	public:
		void load(const char* sectionName, const char* iniFileName);
		void save(const char* iniFileName);
		void apply();

		CustomGraphOptions customOptions;

		std::vector<Vect2i> resolutions;
        Vect2i resolution;
		int colorDepth;

		bool operator == (const GraphOptions &cgo) const {
			return (
						customOptions == cgo.customOptions
					&&	resolution == cgo.resolution
					&&	colorDepth == cgo.colorDepth
				);
		}
		bool operator != (const GraphOptions &cgo) const {
			return !(*this == cgo);
		}
};

class GraphOptionsManager {
	public:
		enum SettingsType {
			FAST,
			PERFORMANCE,
			MAX,
//			EXTREME,
			CUSTOM
		};

		~GraphOptionsManager() {
			if (instance) {
				delete instance;
				instance = 0;
			}
		}

		bool hasChanges();
		bool hasConfirmChanges();
		void apply();
		void reject();
		void approveChanges();
		void reset();

		SettingsType getType();
		void setType(SettingsType newType);

		void save();
		void load();

		GraphOptions& getGraphicsOptions() {
			return newOptions;
		};

		static GraphOptionsManager& getInstance() {
			if (instance == 0) {
				instance = new GraphOptionsManager();
			}
			return *instance;
		}

	protected:

		GraphOptionsManager() {
		}

		static GraphOptionsManager* instance;

		GraphOptions currentOptions;
		GraphOptions newOptions;

		CustomGraphOptions preSettings[MAX - FAST + 1];
};

#endif //_GRAPHICSOPTIONS_H
