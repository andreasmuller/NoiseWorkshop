#pragma once

#include "ofMain.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp
{
	public:
		void setup();
		void update();
		void draw();
	
		void resolutionChanged( int& _res );
	
		void threshold( bool _upload );
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
	
		ofParameter<int> resolution;
		ofParameter<float> noiseFrequency;
		ofParameter<bool> animate;
	
		ofParameter<bool> doThreshold;
		ofParameter<int> thresholdLow;
		ofParameter<int> thresholdHigh;
		ofParameter<bool> invert;
	
		int noiseStartX;
		int noiseStartY;
	
		ofImage noiseImage;
	
		ofxPanel gui;
		bool drawGui;
};
