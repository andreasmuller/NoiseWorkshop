//
//  DebugDrawOpticalFlow.h
//  KinectParticlesOpticalFlow
//
//  Created by Andreas Müller on 29/03/2015.
//
//

#pragma once

#include "ofMain.h"
#include "ofxGui.h"

class OpticalFlowDebugDrawer
{
	public:

		// -------------------------------------------
		OpticalFlowDebugDrawer()
		{
			
		}

		// -------------------------------------------
		void init()
		{
			string xmlSettingsPath = "Settings/OpticalFlowDebug.xml";
			gui.setup( "Optical Flow Debug", xmlSettingsPath );
			
			gui.add( enabled.set( "Enabled", true ) );
			gui.add( res.set( "Res", 20, 4, 100 ) );
			gui.add( pos.set( "Pos", ofVec3f(0,50,0), ofVec3f(-200,0,-200), ofVec3f(200,400,200) ) );
			gui.add( size.set( "Size", ofVec2f(100,100), ofVec2f(-600,-600), ofVec2f(600,600) ) );
			
			gui.loadFromFile( xmlSettingsPath );
			gui.minimizeAll();
			
			// We need to set a few extra params for the geometry shader, in this order.
			debugOpticalFlowShader.setGeometryInputType(GL_LINES);
			debugOpticalFlowShader.setGeometryOutputType(GL_LINE_STRIP );
			debugOpticalFlowShader.setGeometryOutputCount(12);
			debugOpticalFlowShader.load( "Shaders/DebugOpticalFlow/GL2/DebugOpticalFlow.vert",
										 "Shaders/DebugOpticalFlow/GL2/DebugOpticalFlow.frag",
										 "Shaders/DebugOpticalFlow/GL2/DebugOpticalFlow.geom");
		}
	
		// -------------------------------------------
		void draw( ofTexture& _opticalFlowTexture, ofMatrix4x4 _worldToKinect, float _maxMag )
		{
			ofTextureData tmpTexDat = _opticalFlowTexture.getTextureData();
			
			ofMesh tmpMesh;
			tmpMesh.setMode( OF_PRIMITIVE_LINES ); // Expanding points in the geom shader isn't working, work around by using lines
			for( int y = 0; y < res; y++ )
			{
				for( int x = 0; x < res; x++ )
				{
					ofVec3f p = pos.get() +  ofVec3f( ofMap( x, 0, res-1, size.get().x * -0.5, size.get().x * 0.5),
													  ofMap( y, 0, res-1, size.get().y * -0.5, size.get().y * 0.5),
													  0 );
					
					ofVec2f flowOffset = (ofVec2f(x,y) - ofVec2f(res/2,res/2)).getNormalized() * 1.0;
					
					tmpMesh.addVertex( p );
					tmpMesh.addVertex( p + flowOffset );
				}
			}
			
			
			debugOpticalFlowShader.begin();
			
			debugOpticalFlowShader.setUniformTexture( "u_opticalFlowMap", _opticalFlowTexture, 3 );
			
			debugOpticalFlowShader.setUniformMatrix4f("u_worldToKinect",  _worldToKinect );
			
			double HFOV = 1.01447;	// Grabbed these by querying with OpenNI, so will probably only work with the Kinect V1
			double VFOV = 0.789809;
			
			debugOpticalFlowShader.setUniform1f("u_fXToZ", tan(HFOV/2)*2 );
			debugOpticalFlowShader.setUniform1f("u_fYToZ", tan(VFOV/2)*2 );

			debugOpticalFlowShader.setUniform2f("u_pixelDepthmap", 1.0 / tmpTexDat.tex_w, 1.0 / tmpTexDat.tex_h );

			debugOpticalFlowShader.setUniform1f("u_maxFlowMag", _maxMag);
			
			
				ofSetColor( ofColor::white);
				tmpMesh.draw();
			
			debugOpticalFlowShader.end();
	
		}

		// -------------------------------------------
		ofxPanel* getGui()
		{
			return &gui;
		}
	
		bool isEnabled()
		{
			return enabled;
		}
	
	
	protected:
	
		ofxAutoReloadedShader	debugOpticalFlowShader;
	
		ofxPanel gui;
	
		ofParameter<bool>		enabled;
		ofParameter<ofVec3f>	pos;
		ofParameter<ofVec2f>	size;
		ofParameter<int>		res;
};