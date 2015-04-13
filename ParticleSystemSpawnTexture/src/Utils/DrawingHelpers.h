//
//  DrawingHelpers.h
//  FirstPersonCamera
//
//  Created by Andreas Müller on 22/09/2014.
//
//

#pragma once

#include "ofMain.h"

class DrawingHelpers
{
	public:
		
		//-----------------------------------------------------------------------------------------
		//
		static void drawFloorGrid( ofTrueTypeFont* _font )
		{
			ofSetColor( ofColor(50) );
			
			ofPushMatrix();
			ofRotate(90, 0, 0, -1);
			ofDrawGridPlane( 10, 1, false );
			ofPopMatrix();
			
			ofDisableDepthTest();
			
			ofSetColor( ofColor(100) );
			
			float tickDistMm = 1000;
			int numTicks = 20;
			ofVec2f start( ((numTicks * -0.5) * tickDistMm), ((numTicks * -0.5) * tickDistMm) );
			ofVec2f end(   ((numTicks *  0.5) * tickDistMm), ((numTicks *  0.5) * tickDistMm) );
			for( int y = 0; y <= numTicks; y++ )
			{
				float tmpY = start.y + (y * tickDistMm);
				
				ofLine( ofVec3f( start.x, 0, tmpY), ofVec3f( end.x, 0, tmpY) );
				
				for( int x = 0; x <= numTicks; x++ )
				{
					float tmpX = start.x + (x * tickDistMm);
					ofLine( ofVec3f( tmpX, 0, start.y), ofVec3f( tmpX, 0, end.y) );
				}
			}
			
			ofSetColor( ofColor(150) );
			
			for( int y = 0; y <= numTicks; y++ )
			{
				float tmpY = start.y + (y * tickDistMm);
				
				ofPushMatrix();
					ofTranslate( ofVec3f(-10,0,tmpY + 10) );
					ofRotateX(-90);
					ofScale(2,2,2);
					//fontLarge.drawString( ofToString(tmpY), 0, 0 );
					if( _font != NULL ) _font->drawString( ofToString(tmpY), 0, 0 );
				ofPopMatrix();
				
				ofPushMatrix();
					ofTranslate( ofVec3f(tmpY + 10,0, -10) );
					ofRotateX(-90);
					ofScale(2,2,2);
					//fontLarge.drawString( ofToString(tmpY), 0, 0 );
					if( _font != NULL ) _font->drawString( ofToString(tmpY), 0, 0 );
				ofPopMatrix();
			}
			
			ofEnableDepthTest();
		}
};
