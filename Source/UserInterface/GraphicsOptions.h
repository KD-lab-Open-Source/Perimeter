#pragma once

#ifndef _GRAPHICSOPTIONS_H
#define _GRAPHICSOPTIONS_H

#include "SourceUIResolution.h"

class DisplayMode {
public:
    bool fullscreen;
    int display;
    int x, y;
    int refresh;

    FORCEINLINE DisplayMode() {
        this->set(false, -1, 0, 0, 0);
    }

    FORCEINLINE DisplayMode(bool windowed_, int display_, int x_, int y_, int refresh_) {
        this->set(windowed_, display_, x_, y_, refresh_);
    }

    FORCEINLINE void set(bool fullscreen_, int display_, int x_, int y_, int refresh_) {
        this->fullscreen = fullscreen_;
        this->display = display_;
        this->x = x_;
        this->y = y_;
        this->refresh = refresh_;
    }
    
    FORCEINLINE int operator == (const DisplayMode& v) const {
        return x == v.x && y == v.y && display == v.display
            && refresh == v.refresh && fullscreen == v.fullscreen;
    }
    
    FORCEINLINE int operator != (const DisplayMode& v) const {
        return !(*this == v);
    }

    FORCEINLINE bool operator <(const DisplayMode& v) const {
        return std::tie(y, x, fullscreen, refresh, display) < std::tie(v.y, v.x, v.fullscreen, v.refresh, v.display);
    }
    
    FORCEINLINE std::string text() const {
        std::string text = std::to_string(x) + "x" + std::to_string(y);
        if (fullscreen) {
            text += " " + std::to_string(refresh) + "hz Display: " + std::to_string(display);
        } else {
            text += " window";
        }
        return text;
    }
};

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

		std::vector<DisplayMode> resolutions;
        DisplayMode resolution;
		int colorDepth;
        int uiAnchor;
        bool grabInput;
        bool fogEnable;

		bool operator == (const GraphOptions &cgo) const {
			return (
						customOptions == cgo.customOptions
					&&	resolution == cgo.resolution
                    &&	colorDepth == cgo.colorDepth
                    &&	uiAnchor == cgo.uiAnchor
                    &&	grabInput == cgo.grabInput
                    &&	fogEnable == cgo.fogEnable
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
