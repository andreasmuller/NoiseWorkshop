#pragma once

#include "ofMain.h"
#include "Math/MathUtils.h"

// Can't use an enum if we want to increment and decrement
#define STEP						(0)
#define LINEARSTEP					(1)
#define SMOOTHSTEP					(2)
#define LINEARSTEP_INOUT			(3)
#define SMOOTHSTEP_INOUT			(4)
#define INTERPOLATION_TYPE_AMOUNT	(5)

class ofApp : public ofBaseApp
{
	public:
	
	
		// ----------------------------------------------------
		void setup()
		{
			fontSmall.loadFont("Fonts/DIN.otf", 8 );
			fontLarge.loadFont("Fonts/DIN.otf", 24 );
			
			draggedEdgeIndex = -1;
			
			for( int i = 0; i < 4; i++ )
			{
				edges.push_back( i / 3.0 );
			}
			
			interpolationType = SMOOTHSTEP_INOUT;
			interpolationTypechanged();
		}

		// ----------------------------------------------------
		void update()
		{
			canvasRect.set( ofGetWidth() * 0.1, ofGetHeight() * 0.25, ofGetWidth() * 0.8, ofGetHeight() * 0.5 );
	
			if( ofGetMousePressed() && draggedEdgeIndex != -1 )
			{
				float mx = ofGetMouseX();
				edges[draggedEdgeIndex] = ofClamp( (mx - canvasRect.x) / canvasRect.width, 0, 1);
			}
			
			ofSort( edges );
		}
	
		// ----------------------------------------------------
		void draw()
		{
			ofBackgroundGradient( ofColor(40,40,40), ofColor(0,0,0), OF_GRADIENT_CIRCULAR);
			
			ofEnableBlendMode( OF_BLENDMODE_ALPHA );
			
			// draw frame
			ofSetColor( ofColor(200), 60 );
			ofNoFill();
			ofRect( canvasRect );
			ofFill();
			
			// draw title
			string title = getTitle( interpolationType );
			float strW = fontLarge.stringWidth( title );
			float strH = fontLarge.stringWidth( "H" );
			ofSetColor( ofColor::white );
			fontLarge.drawString( title, canvasRect.x + ((canvasRect.width*0.5)-strW*0.5), canvasRect.y - ((canvasRect.y*0.5)-(strH*0.5)) );
			
			// draw the edges
			ofSetColor( ofColor(150) );
			for( int i = 0; i < edges.size(); i++ )
			{
				if( ofContains( edgesToUse, i ) )
				{
					ofSetColor( ofColor(150, 60) );
					if( i == getEdgeMouseIsOver() ) ofSetColor( ofColor::white );
					ofLine( ofVec2f(getEdgeX(i),canvasRect.y), ofVec2f(getEdgeX(i),canvasRect.y + canvasRect.height) );
					ofSetColor( ofColor::white );
					fontSmall.drawString(ofToString(edges.at(i),2), getEdgeX(i), canvasRect.y - 8 );
				}
			}

			// draw the curve
			ofMesh mesh;
			mesh.setMode( OF_PRIMITIVE_LINE_STRIP );
			int resolution = canvasRect.width;
			for( int i = 0; i < resolution; i++ )
			{
				float frac = i / (float)(resolution-1);
				ofVec2f p = canvasRect.position;
				p.x += canvasRect.width * frac;
				
				// You don't have to feed in normalized coordinates of course, we could just as easily feed
				// in pixel coordinates everywhere to these interpolation functions
				float val = 0.0;
				
				if( interpolationType == STEP )						{ val = MathUtils::step( edges.at(2), frac ); }
				else if( interpolationType == LINEARSTEP )			{ val = MathUtils::linearStep(  edges.at(1), edges.at(2), frac ); }
				else if( interpolationType == SMOOTHSTEP )			{ val = MathUtils::smoothStep(  edges.at(1), edges.at(2), frac );  }
				else if( interpolationType == LINEARSTEP_INOUT )	{ val = MathUtils::linearStepInOut( edges.at(0), edges.at(1), edges.at(2), edges.at(3), frac ); }
				else if( interpolationType == SMOOTHSTEP_INOUT )	{ val = MathUtils::smoothStepInOut( edges.at(0), edges.at(1), edges.at(2), edges.at(3), frac ); }
				
				p.y = val * canvasRect.height;
				p.y = (canvasRect.y + canvasRect.height) - p.y; // let's flip it upside down
				
				mesh.addVertex(p);
			}
			ofSetColor( ofColor::white );
			mesh.draw();
			
		}

		// ----------------------------------------------------
		int getEdgeMouseIsOver( int _tolerance = 5 )
		{
			float mx = ofGetMouseX();
			for( unsigned int i = 0; i < edges.size(); i++ )
			{
				if( abs(getEdgeX(i) - mx) < _tolerance )
				{
					return i;
				}
			}
			return -1;
		}

		// ----------------------------------------------------
		float getEdgeX( int _index )
		{
			return canvasRect.x + (edges.at(_index) * canvasRect.width);
		}

		// ----------------------------------------------------
		string getTitle( int _type )
		{
			if( _type == STEP ) { return "step( " + getEdgeString(2) + ", t )"; ; }
			else if( _type == LINEARSTEP ) { return "linearStep( " + getEdgeString(1) + ", " + getEdgeString(2) + ", t )";  }
			else if( _type == SMOOTHSTEP ) { return "smoothStep( " + getEdgeString(1) + ", " + getEdgeString(2) + ", t )";  }
			else if( _type == LINEARSTEP_INOUT ) { return "linearStepInOut( " + getEdgeString(0) + ", " + getEdgeString(1) + ", " + getEdgeString(2) + ", " + getEdgeString(3) + ", t )"; }
			else if( _type == SMOOTHSTEP_INOUT ) { return "smoothStepInOut( " + getEdgeString(0) + ", " + getEdgeString(1) + ", " + getEdgeString(2) + ", " + getEdgeString(3) + ", t )"; }
			return "INVALID";
		}

		// ----------------------------------------------------
		string getEdgeString( int _index, int _precision = 2 )
		{
			return ofToString( edges.at(_index), _precision );
		}

		// ----------------------------------------------------
		void mousePressed(int x, int y, int button)
		{
			draggedEdgeIndex = getEdgeMouseIsOver();
		}

		// ----------------------------------------------------
		void mouseReleased(int x, int y, int button)
		{
			draggedEdgeIndex = -1;
		}
	
		// ----------------------------------------------------
		void interpolationTypechanged()
		{
			edgesToUse.clear();
			bool addAll = false;
			if( interpolationType == STEP ) { edgesToUse.push_back(2); }
			else if( interpolationType == LINEARSTEP ) { edgesToUse.push_back(1); edgesToUse.push_back(2);  }
			else if( interpolationType == SMOOTHSTEP ) { edgesToUse.push_back(1); edgesToUse.push_back(2);  }
			else if( interpolationType == LINEARSTEP_INOUT ) { addAll = true; }
			else if( interpolationType == SMOOTHSTEP_INOUT ) { addAll = true; }
			if( addAll ) { for(int i = 0; i < edges.size(); i++) { edgesToUse.push_back(i); } }
		}
	
		// ----------------------------------------------------
		void keyPressed(int key)
		{
			if( key == OF_KEY_LEFT )
			{
				interpolationType--;
				if( interpolationType < STEP ) { interpolationType = INTERPOLATION_TYPE_AMOUNT-1; }
				interpolationTypechanged();
			}
			else if( key == OF_KEY_RIGHT )
			{
				interpolationType++;
				if( interpolationType >= INTERPOLATION_TYPE_AMOUNT ) { interpolationType = STEP; }
				interpolationTypechanged();
			}
			else if ( key == 'f' )
			{
				ofToggleFullscreen();
			}
		}

		ofRectangle canvasRect;
		vector<float> edges;
		vector<int> edgesToUse;
		int draggedEdgeIndex;
		int interpolationType;
		ofTrueTypeFont fontSmall;
		ofTrueTypeFont fontLarge;
};
