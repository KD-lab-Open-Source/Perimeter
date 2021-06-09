#pragma once

#ifndef _HISTORYSCENECAMERA_H
#define _HISTORYSCENECAMERA_H

#include "SCodeInterpreter/Controller.hpp"

class CameraPosition {
	public:
		CameraPosition() {
		}
		CameraPosition(
			const Vect3f& pivotPosition,
			float psi,
			float theta,
			float distanceToPivot ); 

		CameraPosition operator+(const CameraPosition& pos) const;
		CameraPosition operator*(float t) const;

		static void interpolate(
			const CameraPosition& pos0,
			const CameraPosition& pos1,
			float t,
			CameraPosition* res);
		
		static void interpolateHermite(
			const CameraPosition positions[4],
			float t,
			CameraPosition* res);

		static void correctToShortestAngle(float leadingAngle, float* followingAngle) {
			float d = fmod( *followingAngle - leadingAngle, M_PI * 2.0f );
			float ad = (float) fabs(d);
			*followingAngle = leadingAngle + d;
			if(ad > M_PI) {
				*followingAngle += (d < 0) ? (2.0f * M_PI) : (-2.0f * M_PI); 
			}
		}

		static void correctToShortestPath(
				const CameraPosition& leadingPos,
				CameraPosition* followingPos ) {

			correctToShortestAngle(leadingPos.psi, &(followingPos->psi));
			correctToShortestAngle(leadingPos.theta, &(followingPos->theta));
		}

		static void correctToShortestPath( CameraPosition positions[4] ) {
			correctToShortestPath(positions[0], &positions[1]);
			correctToShortestPath(positions[1], &positions[2]);
			correctToShortestPath(positions[2], &positions[3]);
		}


		Vect3f pivotPosition;
		float psi;
		float theta;
		float distanceToPivot;
};

class HistorySceneCamera {
	public:
		HistorySceneCamera(Controller* controller);
		~HistorySceneCamera();

		void quant(const Vect2f& mousePos, float dt);
		void mouseWheel(int delta);

		void reset();

		void setCamera(cCamera* camera);
		cCamera* getCamera() const {
			return camera;
		}

		void setPosition(const CameraPosition& pos) {
			position = pos;
			update();
		}
		const CameraPosition& getPosition() const {
			return position;
		}
		static void setMatrixToCamera(cCamera* camera, const MatXf& matrix);

		void addToPath(const CameraPosition& pos);
		void clearPath();
		void startPlaying();
		void stopPlaying();

		void setPositionToBegin();
		void setPositionToEnd();
		void setPositionToNomad();

		void setStepDuration(float duration) {
			flyStepDuration = duration;
		}
		void setLoopedPlayingMode(bool isLooped) {
			isLoopedPlaying = isLooped;
		}
		bool isLoopedPlayingMode() {
			return isLoopedPlaying;
		}

		void onResolutionChanged();

		void addLineToLog(const string& line);
		string log;
		static string posToString(const CameraPosition& pos);

		void calcRayIntersection(float x, float y, Vect3f& v0, Vect3f& v1);

		const Vect2f& getMousePos() const {
			return lastMousePos;
		}

		bool isFollowNomadMode() const {
			return followNomadMode;
		}
		void setFollowNomadMode(bool follow);
		void setFollowNomadModeSettings(float deltaPsi, float radius, float theta);

	protected:
		void setup();
		void update();
		void advance();
		void setupInterpolationPoints();
		void setFree(bool free);

		void setupNextFollowPos(const Vect3f& nomadPosition);

		Controller* controller;
		
		cCamera* camera;
		CameraPosition position;

		float flyStepDuration;
		float flyStepTimeElapsed;

		int currentStep;
		CameraPosition interpolationPoints[4];
		bool isPlaying;
		bool isFree;
		bool isLoopedPlaying;
		bool isFirstPointFromPath;
		bool shouldClearAtEnd;
		deque<CameraPosition> path;
		deque<CameraPosition> waitingList;

		bool followNomadMode;
		CameraPosition currentFollowPos;
		CameraPosition nextFollowPos;
		CameraPosition originFollowPos;
		float dPsi;
		bool tracking;

		Vect2f lastMousePos;
};


#endif //_HISTORYSCENECAMERA_H
