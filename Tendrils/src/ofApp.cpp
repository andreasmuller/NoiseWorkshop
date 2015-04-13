#include "ofApp.h"

//-----------------------------------------------------------------------------------------
//
void ofApp::setup()
{
	//ofSetLogLevel( OF_LOG_VERBOSE );
	
	ofSetFrameRate( 60 );
	
	fontSmall.loadFont("Fonts/DIN.otf", 8 );
	
	ofxGuiSetFont( "Fonts/DIN.otf", 8 );
	ofxGuiSetDefaultWidth( 300 );
	
	string xmlSettingsPath = "Settings/Grass.xml";
	gui.setup( "Tendrils", xmlSettingsPath );
	
	gui.add( placementResolution.set("Resolution",  60,  3,  400) );
	gui.add( placementSize.set("Placement Size",  2,  0.1,  10) );
	gui.add( placementNoiseSpaceFrequency.set("Placement Space Freq",   1.0f,  0.0001f,  3.0f) );
	gui.add( placementBottomThreshold.set("Placement Bottom Threshold",  0,  0,  1) );
	
	
	gui.add( stalkRadius.set("Tendril Radius",  0.1f,  0.0001f,  1.0f) );
	gui.add( stalkHeight.set("Tendril Height",  1.0f,  0.0001f,  10.0f) );
	gui.add( swayingMaxAngle.set("Swaying Max Angle",  20.0,  0.0f,  180.0f) );
	gui.add( swayingTimeScale.set("Swaying Time Scale",  1.0f,  0.0001f,  3.0f) );
	gui.add( swayingNoiseSpaceFrequency.set("Swaying Noise Space Freq",  0.1f,  0.0001f,  5.0f) );
	gui.add( swayingTimeMaxDifference.set("Swaying Time Max Diff",  1.0f,  0.0001f,  5.0f) );
	
	gui.loadFromFile( xmlSettingsPath );
	
	// Register callbacks so that we regenerate the mesh when needed
	placementResolution.addListener( this, &ofApp::intParamChanged );
	placementSize.addListener( this, &ofApp::floatParamChanged );
	placementNoiseSpaceFrequency.addListener( this, &ofApp::floatParamChanged );
	placementBottomThreshold.addListener( this, &ofApp::floatParamChanged );
	
	// Give us a starting point for the camera, we're changing the scale a bit here to keep things more intuitive
	camera.setNearClip(0.01f);
	camera.setPosition( 0, 0.5, 10.0 );
	camera.setMovementMaxSpeed( 0.1f );
	
	// https://www.opengl.org/wiki/Geometry_Shader
	// https://www.opengl.org/discussion_boards/showthread.php/175003-Geometry-Shader-Max-output-vertices
	// https://www.opengl.org/discussion_boards/showthread.php/176987-Geometry-Shader-Output-Limits
	
	int maxGeometryOutputVertices = -1;
	int maxGeometryTotalOutputComponents = -1;
	int maxGeometryOutputComponents = -1;
	
	//int maxVerticesOut;
	
	glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES, &maxGeometryOutputVertices);
	glGetIntegerv(GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS, &maxGeometryTotalOutputComponents);
	glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_COMPONENTS, &maxGeometryOutputComponents);
	
	
	ofLogNotice() << "maxGeometryOutputVertices: " << maxGeometryOutputVertices;
	ofLogNotice() << "maxGeometryTotalOutputComponents: " << maxGeometryTotalOutputComponents;
	ofLogNotice() << "maxGeometryOutputComponents: " << maxGeometryOutputComponents;
	
	//max_vertices_out = MIN(maxGeometryOutputVertices, maxGeometryTotalOutputComponents / components);
	

	// We need to set a few extra params for the geometry shader, in this order.
	grassShader.setGeometryInputType(GL_LINES);
	grassShader.setGeometryOutputType( GL_TRIANGLES );
	grassShader.setGeometryOutputCount( 3 * 6 * 6  ); // GTX 970 has a limit of 128 vertices output per shader limiting us to 3 * 6 * 6, investigate..
	grassShader.load("Shaders/Tendrils/GL2/Tendrils.vert", "Shaders/Tendrils/GL2/Tendrils.frag", "Shaders/Tendrils/GL2/Tendrils.geom");
	
	computeMesh();
	
	drawGui = false;
}

//--------------------------------------------------------------
void ofApp::intParamChanged(int& _param )
{
	computeMesh();
}

//--------------------------------------------------------------
void ofApp::floatParamChanged(float& _param )
{
	computeMesh();
}


//-----------------------------------------------------------------------------------------
//
void ofApp::update()
{

}

//-----------------------------------------------------------------------------------------
//
void ofApp::computeMesh()
{
	
	ofMesh srcMesh = ofMesh::sphere( placementSize, placementResolution, OF_PRIMITIVE_TRIANGLES );
	// Todo: swap in other meshes
	
	grassMesh.clear();
	grassMesh.setMode( OF_PRIMITIVE_LINES );
	
	if( srcMesh.getNumVertices() != srcMesh.getNumNormals() )
	{
		ofLogError() << "void ofApp::computeMesh() Hey, this is not going to work if we don't have a normal for each vertex.";
	}
	else
	{
		vector<ofVec3f>& vertices = srcMesh.getVertices();
		vector<ofVec3f>& normals = srcMesh.getNormals();
		for( int i = 0; i < vertices.size(); i++ )
		{
			ofVec3f vertexPos = vertices.at(i);
			ofVec3f perlinPos = vertexPos * placementNoiseSpaceFrequency; //ofMap( mx, 0, 1,  0.01, 5);
			
			float placementPerlinVal = ofNoise( perlinPos.x, perlinPos.y, perlinPos.z );
			
			// Remap the number back to 0..1 taking our bottom threshold into account and clamping
			placementPerlinVal= ofMap( placementPerlinVal, placementBottomThreshold, 1,	0, 1, true );

			if( placementPerlinVal > 0 )
			{
				grassMesh.addVertex( vertexPos );
				grassMesh.addVertex( vertexPos + ( normals[i] * placementPerlinVal) ); // the normal has a length we later use to module tendril height
			}
		}
	}
}

//-----------------------------------------------------------------------------------------
//
void ofApp::draw()
{
	ofBackgroundGradient( ofColor(40,40,40), ofColor(0,0,0), OF_GRADIENT_CIRCULAR);
	
	ofEnableDepthTest();
	
	ofDisableLighting();
	
	camera.begin();
	
		drawTendrils( &camera );
	
	camera.end();
	
	ofDisableDepthTest();
	ofEnableBlendMode( OF_BLENDMODE_ALPHA );
	ofSetColor( ofColor::white );
	
	if( drawGui )
	{
		gui.draw();
	}
	
	fontSmall.drawStringShadowed(ofToString(ofGetFrameRate(),2), ofGetWidth()-35, ofGetHeight() - 6, ofColor::whiteSmoke, ofColor::black );
}

//-----------------------------------------------------------------------------------------
//
void ofApp::drawTendrils( ofCamera* _camera )
{
	float mx = ofMap( ofGetMouseX(), 0, ofGetWidth(),   0, 1 );
	float my = ofMap( ofGetMouseY(), 0, ofGetHeight(),  0, 1 );
	
	grassShader.begin();

		grassShader.setUniform1f("timeSecs", ofGetElapsedTimef() );

		grassShader.setUniform3fv( "cameraWorldPos", _camera->getGlobalPosition().getPtr() );

		grassShader.setUniform1f("stalkRadius", stalkRadius );
		grassShader.setUniform1f("stalkHeight", stalkHeight );

		grassShader.setUniform1f("animationTimeMaxDifference", swayingTimeMaxDifference );

		grassShader.setUniform1f("stalkSwayingMaxAngle", ofDegToRad(swayingMaxAngle) * 0.5f );
	
		grassShader.setUniform1f("grassSwayingTimeScale", swayingTimeScale );
		grassShader.setUniform1f("grassSwayingNoiseFrequency", swayingNoiseSpaceFrequency );
	
		ofSetColor( ofColor::white );
	
		grassMesh.draw();
	
	grassShader.end();
}

//-----------------------------------------------------------------------------------------
//
void ofApp::keyPressed(int key)
{
	if( key == OF_KEY_TAB )
	{
		drawGui = !drawGui;
	}
	else if( key == 'f' )
	{
		ofToggleFullscreen();
	}
	else if( key == OF_KEY_LEFT  )
	{
	}
	else if( key == OF_KEY_RIGHT  )
	{
	}
}
