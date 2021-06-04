#ifndef __CAMERA_MANAGER_H__
#define __CAMERA_MANAGER_H__

class terUnitBase; 
struct SaveCameraData; 
struct SaveCameraSplineData;

//camera movement
class CameraCoordinate
{
public:
	CameraCoordinate(){}
	CameraCoordinate(const Vect2f& position, float psi, float theta, float distance); 

	CameraCoordinate operator+(const CameraCoordinate& coord) const;
	CameraCoordinate operator*(float t) const;

	void check(bool restricted);
	void interpolate(const CameraCoordinate& coord0, const CameraCoordinate& coord1, float t);
	void interpolateHermite(const CameraCoordinate coords[4], float t);

	void uncycle(const CameraCoordinate& coord0); // расцикливает углы по coord0

	void save(SaveCameraData& data) const;
	void load(const SaveCameraData& data);

	const Vect3f& position() const { return position_; }
	float psi() const { return psi_; }
	float theta() const { return theta_; }
	float distance() const { return distance_; }

	Vect3f& position() { return position_; }
	float& psi() { return psi_; }
	float& theta() { return theta_; }
	float& distance() { return distance_; }

	int height();

private:
	Vect3f position_;
	float psi_;
	float theta_;
	float distance_;
};


class terCameraType
{
public:
	terCameraType(cCamera* camera);
	~terCameraType();

	void reset();

	void setFocus(float focus);
	void setCoordinate(const CameraCoordinate& coord) { coordinate_ = coord; update(); }
	
	float focus() const { return focus_; }
	const CameraCoordinate& coordinate() const { return coordinate_; }
	const MatXf& matrix() const { return matrix_; }
	
	void SetFrustumGame();
	void SetFrustumMenu();
	void SetFrustumCutScene();

	void calcRayIntersection(float x,float y,Vect3f& v0,Vect3f& v1);

	cCamera* GetCamera() { return Camera; }

	void setTarget(const CameraCoordinate& coord, int duration);

	bool tilting() const { return tilting_;	}
	terUnitBase* unitFollow() const { return unit_follow; }

	void mouseQuant(const Vect2f& mousePos);
	void tilt(const Vect2f& mouseDelta);
	void shift(const Vect2f& mouseDelta);
	void controlQuant();
	void mouseWheel(int delta);
	void quant(float mouseDeltaX, float mouseDeltaY, float delta_time);

	void SetCameraFollow(terUnitBase* unit, int transitionTime = 0);
	void QuantCameraFollow(float delta_time);
	void SaveCamera(int n);
	void RestoreCamera(int n);

	void destroyLink();

	bool cursorTrace(const Vect2f& pos2,Vect3f& v);

	bool restricted() const { return restricted_; }
	void setRestriction(bool restricted) { restricted_ = restricted; }

	CameraCoordinate& coordinate() { return coordinate_; }

	int pathSize() const { return path_.size(); }
	void addCurrentToPath() { path_.push_back(coordinate()); }
	void removeLastPointFromPath() { if(!path_.empty()) path_.pop_back(); }
	void startReplayPath(int stepDuration, int cycles);
	void stopReplayPath();
	bool isPlayingBack() const { return replayIndex_ != -1; }
	void erasePath();
	void savePath(SaveCameraSplineData& data);
	void loadPath(const SaveCameraSplineData& data, bool addCurrentPosition);

	void startOscillation(int duration, float factor);

private:
	cCamera* Camera;
	MatXf matrix_;
	float focus_;

	bool restricted_;

	CameraCoordinate coordinate_;
	
	int interpolationDuration_;
	int interpolationTimer_;
	CameraCoordinate interpolationPoints_[4];

	bool tilting_;
	bool  cameraMouseZoom;
	
	float  cameraPsiVelocity, cameraPsiForce;
	float  cameraThetaVelocity, cameraThetaForce;
	float  cameraZoomVelocity, cameraZoomForce;
	Vect3f cameraPositionVelocity, cameraPositionForce;
	
	//camera movement		 
	terUnitBase* unit_follow;
	InterpolationTimer unitFollowTimer_;

	CameraCoordinate*    cameraSavePoints[5];

	typedef vector<CameraCoordinate> CoordList;
	CoordList path_;
	int replayIndex_;
	int replayIndexMax_;

	DurationTimer oscillatingTimer_;
	int explodingDuration_;
	float explodingFactor_;

	void update();
	void setPath(int index);
};

extern terCameraType* terCamera;


#endif //__CAMERA_MANAGER_H__