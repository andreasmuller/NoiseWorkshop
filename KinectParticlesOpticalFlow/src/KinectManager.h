
#include "ofMain.h"

#include "ofxGui.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxKinect.h"

#include "Math/MathUtils.h"
#include "Utils/OrientedBoundingBox.h"
//#include "Utils/ofEasyCamExt.h"

#define KINECT_RES_WIDTH  (640)
#define KINECT_RES_HEIGHT (480)

#define KINECT_SENSOR_NEAR_LIMIT (400.0f)
#define KINECT_SENSOR_FAR_LIMIT  (4000.0f)

using namespace ofxCv;
using namespace cv;

// ----------------------------------------------------------------
//
class KinectManager : public ofThread
{
	public:

		KinectManager();

		void init();
		void start();
		void stop();
	    void threadedFunction();

		void getCurrentPointCloud( vector<ofVec3f>& _points );
		void getCurrentPointCloud( vector<ofVec3f>& _points, vector<ofFloatColor>& _colors );
		void getCurrentPointCloudColors( vector<ofFloatColor>& _colors );
	
		bool haveNewInteractionPoints() { return doesHaveNewInteractionPoints; }
		vector<ofVec2f> getInteractionPoints( ofRectangle _targetRect );
	
		ofVec2f getAverageFlow();
		ofVec2f getAverageFlowSmoothed();

	//denseFlow.getAverageFlow();
	
		void updateTracking();
		void updateOpticalFlow();
		void updateInteractionAreaSetup();

		void updateValidPixelsPointCloud();
	
		void updateKinectClippingFromInteractionBox();
	
		void updateFlowParams();
	
		void drawPointCloud();
		void debugDraw2D();
		void drawFlow( ofRectangle _rect, bool _showAverageFlow = false );

		ofxCv::FlowFarneback& getFlowObject() { return denseFlow; }
	
		void drawGui();
	
		void trackerFloatParamChanged(float & _newParam );
		void trackerIntParamChanged(int & _newParam );
		void trackerVec3ParamChanged( ofVec3f & _newParam );
		void trackerBoolParamChanged( bool & _newParam );
	
		void flowFloatParamChanged(float & _newParam );
		void flowIntParamChanged(int & _newParam );
		void flowBoolParamChanged( bool & _newParam );
	
		float getNumFramesProcessedPerSecond() { return kinectFramesProcessedPerSecond; }
	
		ofxKinect* getKinectDevice();
	
		ofEasyCam				camera;
	
		ofVec2f					testDepthMapPos;
	
	protected:

		bool					haveOpenedKinect;
		ofxKinect				kinect;

		ofPixels				colorPixels;
		Mat						colorPixelsMat;
	
		ofPixels				depthPixelsChar;
		Mat						depthPixelsCharMat;

		ofImage					depthImageMask;
		Mat						depthImageMaskMat;
	
		ofPixels				maskedDepthPixels;
		Mat						maskedDepthPixelsMat;
	
		ofPixels				opticalFlowScratch;
		Mat						opticalFlowScratchMat;

		ofPixels				opticalFlowPixelsSmall;
		Mat						opticalFlowPixelsSmallMat;
	
		OrientedBoundingBox		interactionAreaBoundingBox;

		bool					imagesNeedUpdating;
		ofImage					colorImage;
		ofImage					depthImageForContours;
		ofImage					maskedColorImage;
	
		ofxCv::ContourFinder	contourFinder;
		ofxCv::FlowFarneback	denseFlow;
		ofVec2f					averageFlow;
		ofVec2f					averageFlowSmoothed;
	
		bool					doesHaveNewInteractionPoints;
		vector<ofVec3f>			trackedObjectPos;
		vector<ofColor>			trackedObjectColor;

		ofMesh					activeWorldPointsMesh;


		ofxPanel				gui;
	
		ofParameter<int>		pointCloudStep;

		//ofParameter<float>	kinectNearThreshold;
		ofParameter<float>		kinectFarThreshold;

		ofParameter<bool>		blobTrackingEnabled;
	
		ofParameter<float>		trackerMinAreaRadius;
		ofParameter<float>		trackerMaxAreaRadius;
		//ofParameter<float>	trackerThreshold;

		ofParameter<int>		trackerPersistence;
		ofParameter<float>		trackerMaximumDistance;

		ofParameter<bool>		isSettingUpInteractionArea;
		ofParameter<bool>		mask2DIsEnabled;

		ofParameter<ofVec3f>	interactionAreaBoxPos;
		ofParameter<ofVec3f>	interactionAreaBoxSize;

		ofParameter<bool>		doOpticalFlow;
	
		ofxPanel				guiFlow;
	
		ofParameter<float>		flowPyrScale;
		ofParameter<int>		flowLevels;
		ofParameter<int>		flowWinSize;
		ofParameter<int>		flowIterations;
		ofParameter<int>		flowPolyN;
		ofParameter<float>		flowPolySigma;
		ofParameter<bool>		flowUseGaussian;

		float					kinectFramesProcessedPerSecond;
		int						kinectFramesProcessed;
		float					lastTimeCheckedKinectFramesProcessed;
	
	private:

};