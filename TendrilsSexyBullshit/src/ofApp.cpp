#include "ofApp.h"

//-----------------------------------------------------------------------------------------
//
void ofApp::setup()
{

	ofSetLogLevel( OF_LOG_VERBOSE );
	
	ofSetFrameRate( 60 );
	
	fontSmall.loadFont("Fonts/DIN.otf", 8 );
	fontMedium.loadFont("Fonts/DIN.otf", 16 );
	fontLarge.loadFont("Fonts/DIN.otf", 36 );
	
	int numLights = 1;
	for( int i = 0; i < numLights; i++ )
	{
		ofSeedRandom(i << 24);
		
		lights.push_back( new ShadowMapLight() );
		
		lights.back()->setGlobalPosition( ofRandom(-4, 4), ofRandom(4, 6), ofRandom(-4, 4) );
		lights.back()->setDiffuseColor( ofFloatColor::fromHsb( i / (float)numLights, 1.0, 1.0) );
		lights.back()->setSpecularColor( ofFloatColor::white );
		lights.back()->setAmbientColor( ofFloatColor(0,0,0) );
		
		lights.back()->setRadius( 1.0 );
		
		lights.back()->enable();
	}
	ofSeedRandom();
	

	lights.at(0)->setGlobalPosition( -3, 14, 0 );
	lights.at(0)->lookAt( ofVec3f( 0, 0, 0) );
//	lights.at(1).setGlobalPosition(  4, 6, 1 );
//	lights.at(2).setGlobalPosition( -2, 6, -4 );
//	lights.at(3).setGlobalPosition(  4, 6, 4 );

	//lights.front().enable();
	
	lights.at(0)->setup( 256, 70,  3, 25 );
	

	material.setAmbientColor( ofFloatColor::black );
	material.setDiffuseColor( ofFloatColor::grey );
	material.setSpecularColor( ofFloatColor::white );
	material.setShininess( 50 );
	
	
	ofxGuiSetDefaultWidth( 300 );

	
	// Main UI
	string xmlSettingsPath = "Settings/Main.xml";
	gui.setup( "Main", xmlSettingsPath );
	
	gui.add( sceneAmbient.set( "Scene Ambient", ofColor::black, ofColor(0,0,0,0), ofColor::white) );

	gui.add( lightDiffuse1.set( "Light 1 Diffuse", lights.at(0)->getDiffuseColor(), ofColor(0,0,0,0), ofColor::white) );
	gui.add( lightRadius1.set( "Light 1 Radius",   lights.at(0)->getRadius(), 0, 40) );
	
/*
	gui.add( lightDiffuse2.set( "Light 2 Diffuse", lights.at(1)->getDiffuseColor(), ofColor(0,0,0,0), ofColor::white) );
	gui.add( lightRadius2.set( "Light 2 Radius",   lights.at(1)->getRadius(), 0, 40) );
	
	gui.add( lightDiffuse3.set( "Light 3 Diffuse", lights.at(2)->getDiffuseColor(), ofColor(0,0,0,0), ofColor::white) );
	gui.add( lightRadius3.set( "Light 3 Radius",   lights.at(2)->getRadius(), 0, 40) );
	
	gui.add( lightDiffuse4.set( "Light 4 Diffuse", lights.at(3)->getDiffuseColor(), ofColor(0,0,0,0), ofColor::white) );
	gui.add( lightRadius4.set( "Light 4 Radius",   lights.at(3)->getRadius(), 0, 40) );
*/
	gui.loadFromFile( xmlSettingsPath );


	
	string xmlSettingsShapePath = "Settings/Tendrils.xml";
	guiShape.setup( "Tendrils", xmlSettingsShapePath );
	
	guiShape.add( placementResolution.set("Resolution",  60,  3,  400) );
	guiShape.add( placementSize.set("Placement Size",  2,  0.1,  10) );
	guiShape.add( placementNoiseSpaceFrequency.set("Placement Space Freq",   1.0f,  0.0001f,  3.0f) );
	guiShape.add( placementBottomThreshold.set("Placement Bottom Threshold",  0,  0,  1) );
	
	
	guiShape.add( tendrilRadius.set("Tendril Radius",  0.1f,  0.0001f,  1.0f) );
	guiShape.add( tendrilHeight.set("Tendril Height",  1.0f,  0.0001f,  10.0f) );
	guiShape.add( swayingMaxAngle.set("Swaying Max Angle",  20.0,  0.0f,  180.0f) );
	guiShape.add( swayingTimeScale.set("Swaying Time Scale",  1.0f,  0.0001f,  3.0f) );
	guiShape.add( swayingNoiseSpaceFrequency.set("Swaying Noise Space Freq",  0.1f,  0.0001f,  5.0f) );
	guiShape.add( swayingTimeMaxDifference.set("Swaying Time Max Diff",  1.0f,  0.0001f,  5.0f) );
	
	guiShape.loadFromFile( xmlSettingsShapePath );
	guiShape.setPosition( gui.getPosition().x, gui.getPosition().y + gui.getHeight() + 5 );
	
	// Register callbacks so that we regenerate the mesh when needed
	placementResolution.addListener( this, &ofApp::intParamChanged );
	placementSize.addListener( this, &ofApp::floatParamChanged );
	placementNoiseSpaceFrequency.addListener( this, &ofApp::floatParamChanged );
	placementBottomThreshold.addListener( this, &ofApp::floatParamChanged );
	
	
	
	
	// Material UI
	string xmlSettingsMaterialPath = "Settings/Material.xml";
	guiMaterial.setup( "Material", xmlSettingsMaterialPath );
	
	guiMaterial.add( materialDiffuse.set( "Material Diffuse", ofColor::grey, ofColor(0,0,0,0), ofColor::white) );
	guiMaterial.add( materialSpecular.set( "Material Specular", ofColor::white, ofColor(0,0,0,0), ofColor::white) );
	guiMaterial.add( materialShininess.set( "Material Shininess", 30, 0, 127) );
	
	guiMaterial.loadFromFile( xmlSettingsMaterialPath );
	guiMaterial.setPosition( ofGetWidth() - guiMaterial.getWidth() - 5, 5 );

	// Shadow map UI
	string xmlSettingsShadowPath = "Settings/ShadowMap.xml";

	guiShadowMap.setup( "Shadow Map", xmlSettingsShadowPath );

	guiShadowMap.add( shadowMapLightFov.set( "Fov", 60, 1, 180) );
	guiShadowMap.add( shadowMapLightNear.set( "Near", 5, 0.1, 100) );
	guiShadowMap.add( shadowMapLightFar.set( "Far",  25, 0.1, 100) );
	
	guiShadowMap.add( shadowMapLightBlurFactor.set( "Blur Factor", 4.0, 1.0, 20) );
	guiShadowMap.add( shadowMapLightBlurNumPasses.set( "Blur Num Passes", 2, 1, 20) );
	guiShadowMap.add( shadowCoeffecient.set( "Shadow Coeffecient", 1.0, 0.01, 20) );

	guiShadowMap.loadFromFile( xmlSettingsShadowPath );
	guiShadowMap.setPosition( guiMaterial.getPosition().x, guiMaterial.getPosition().y + guiMaterial.getHeight() + 5 );
	
	// Load the lighting shader
	lightingShader.load( "Shaders/BlinnPhongRadiusShadowMap/GL2/BlinnPhongRadiusShadowMap" );
	
	// We need to set a few extra params for the geometry shader, in this order.
	tendrilShaderNoLight.setGeometryInputType(GL_LINES);
	tendrilShaderNoLight.setGeometryOutputType( GL_TRIANGLES );
	tendrilShaderNoLight.setGeometryOutputCount( 7 * 8 * 6  );
	tendrilShaderNoLight.load("Shaders/TendrilsLightShadowMap/GL2/Tendrils.vert", "Shaders/TendrilsLightShadowMap/GL2/Tendrils.frag", "Shaders/TendrilsLightShadowMap/GL2/Tendrils.geom");
	
	
	// We need to set a few extra params for the geometry shader, in this order.
	tendrilShader.setGeometryInputType(GL_LINES);
	tendrilShader.setGeometryOutputType( GL_TRIANGLES );
	tendrilShader.setGeometryOutputCount( 7 * 8 * 6  );
	tendrilShader.load("Shaders/TendrilsNoLight/GL2/Tendrils.vert", "Shaders/TendrilsNoLight/GL2/Tendrils.frag", "Shaders/TendrilsNoLight/GL2/Tendrils.geom");
	
	
	
	camera.setNearClip(0.01f);
	camera.orbit( 170, -14, 10.0 );
	camera.setMovementMaxSpeed( 0.3f );

	

	
	drawGui = false;
}


//-----------------------------------------------------------------------------------------
//
void ofApp::update()
{
	ofSetGlobalAmbientColor( sceneAmbient.get() );
	
	lights.at(0)->setDiffuseColor( lightDiffuse1.get() );
	lights.at(0)->setRadius( lightRadius1 );
	
	/*
	lights.at(1)->setDiffuseColor( lightDiffuse2.get() );
	lights.at(1)->setRadius( lightRadius2 );
	
	lights.at(2)->setDiffuseColor( lightDiffuse3.get() );
	lights.at(2)->setRadius( lightRadius3 );
	
	lights.at(3)->setDiffuseColor( lightDiffuse4.get() );
	lights.at(3)->setRadius( lightRadius4 );
	*/
	
	material.setDiffuseColor( materialDiffuse.get() );
	material.setSpecularColor( materialSpecular.get() );
	material.setShininess( materialShininess );

	lights.at(0)->setFov( shadowMapLightFov );
	lights.at(0)->setNearFar( shadowMapLightNear, shadowMapLightFar );
	lights.at(0)->setBlurParams( shadowMapLightBlurFactor, shadowMapLightBlurNumPasses );
	
	//cout << shadowMapLightFov << " " << shadowMapLightNear << "  " << shadowMapLightFar << endl;
	
	//float t = ofGetElapsedTimef();
	//lights.at(0).setGlobalPosition( sinf(t) * -2.0, 3, cosf(t) * -2.0 );
	
	//lights.at(0).setGlobalPosition( 1.2, 3, -0.3 );
	//cout << "Setting the light pos to: 1.2, 3, -0.3" << endl;


}

//-----------------------------------------------------------------------------------------
//
void ofApp::draw()
{
	float time = ofGetElapsedTimef();
	
	ofBackgroundGradient( ofColor(40,40,40), ofColor(0,0,0), OF_GRADIENT_CIRCULAR);	

	ofDisableAlphaBlending();
	//ofEnableAlphaBlending();
	ofEnableDepthTest();
	
	// Draw to the shadow map
	lights.at(0)->beginShadowMap();
		ofSetColor( ofColor::white );
		drawScene( time );
	lights.at(0)->endShadowMap();

	ofEnableAlphaBlending();
	
	camera.begin();
	
		ofSetColor( ofColor::white );
	
		lights.at(0)->enable(); // make sure we upload the light settings to the built in GLSL uniforms
		lightingShader.begin();
	
			ofLightExt::setParams( &lightingShader, (vector<ofLightExt*>&)lights, &camera );
			material.setParams( &lightingShader );

			int shadowMapTexUnit = 0;
			//lights.at(0)->bindShadowMapTexture( shadowMapTexUnit );

			lightingShader.setUniformTexture( "shadowMap", lights.at(0)->getShadowMapTexture(), shadowMapTexUnit );
			//lightingShader.setUniform1i("shadowMap",  shadowMapTexUnit );
			lightingShader.setUniformMatrix4f("toShadowSpaceMatrix",  lights.at(0)->getShadowMatrix( camera ) );
			lightingShader.setUniform1f( "shadowLinearDepthConstant", lights.at(0)->getLinearDepthScalar() );

			lightingShader.setUniform1f( "shadowCoeffecient", shadowCoeffecient );
	
	
			material.begin();
	
				drawScene( time );
	
			material.end();

			//lights.at(0)->unbindShadowMapTexture();
	
		lightingShader.end();
	
	
	
	
		ofDisableLighting();
		for( unsigned int i = 0; i < lights.size(); i++ )
		{
			lights.at(i)->draw( 0.2 );
		}
	
		lights.at(0)->debugDrawViewingVolume();
	
	camera.end();
	

	
	if( drawGui )
	{
		ofDisableDepthTest();
		ofDisableAlphaBlending();
		ofSetColor( ofColor::white );
		float tmpSize = 256;
		lights.at(0)->debugDrawShadowMap( ofVec2f(ofGetWidth(), ofGetHeight())-tmpSize, tmpSize, tmpSize );
		ofEnableAlphaBlending();
		
		gui.draw();
		guiShape.draw();
		guiMaterial.draw();
		guiShadowMap.draw();
	}
	
	ofDisableDepthTest();
	fontSmall.drawStringShadowed(ofToString(ofGetFrameRate(),2), ofGetWidth()-33, ofGetHeight()-6, ofColor::whiteSmoke, ofColor::black );
}

//-----------------------------------------------------------------------------------------
//
void ofApp::drawScene( float _time, bool _forShadowMap )
{
	
	ofMesh mesh;
	ofMesh sphereMesh = ofSpherePrimitive(0.6, 30).getMesh();
	
	for( unsigned int i = 0; i < 10; i++ )
	{
		ofSeedRandom( i );
		
		ofVec3f noisePos( ofRandom(-10, 10), ofRandom(-10, 10), ofRandom(-10, 10) );
		noisePos += ofVec3f(1,0,0) * _time * 0.1;
		
		ofVec3f drawPos;
		drawPos.x = ofSignedNoise(noisePos.x, noisePos.y, noisePos.z ) * 8.0;
		drawPos.y = ofNoise(noisePos.y, noisePos.z, noisePos.y ) * 5.0;
		drawPos.z = ofSignedNoise(noisePos.z, noisePos.x, noisePos.y ) * 8.0;
		
		ofMesh tmpSphereMesh = sphereMesh;
		for( unsigned int i = 0; i < tmpSphereMesh.getNumVertices(); i++ )
		{
			tmpSphereMesh.getVertices()[i] += drawPos;
		}
		
		//ofDrawSphere( drawPos, 0.6 );
		mesh.append( tmpSphereMesh );
	}
	
	
	ofSeedRandom();
	
	
	// Draw the vertices already in world space for now
	float size = 10;
	ofMesh floorMesh;
	floorMesh.setMode( OF_PRIMITIVE_TRIANGLES );
	floorMesh.addVertex( ofVec3f( -size, 0, -size ) );
	floorMesh.addVertex( ofVec3f(  size, 0, -size ) );
	floorMesh.addVertex( ofVec3f(  size, 0,  size ) );

	floorMesh.addVertex( ofVec3f( -size, 0, -size ) );
	floorMesh.addVertex( ofVec3f(  size, 0,  size ) );
	floorMesh.addVertex( ofVec3f( -size, 0,  size ) );
	for( unsigned int i = 0; i < floorMesh.getNumVertices(); i++ ) { floorMesh.addNormal( ofVec3f(0,1,0) ); }
	
	floorMesh.draw();
	
	mesh.draw();
	
	ofPushMatrix();
	
	
	ofPopMatrix();
	
}

//-----------------------------------------------------------------------------------------
//
void ofApp::drawTendrils( float _time, ofShader* _shader, ofCamera* _camera )
{
	_shader->begin();

		_shader->setUniform1f("timeSecs", _time );

		_shader->setUniform3fv( "cameraWorldPos", _camera->getGlobalPosition().getPtr() );

		_shader->setUniform1f("stalkRadius", tendrilRadius );
		_shader->setUniform1f("stalkHeight", tendrilHeight );

		_shader->setUniform1f("animationTimeMaxDifference", swayingTimeMaxDifference );

		_shader->setUniform1f("stalkSwayingMaxAngle", ofDegToRad(swayingMaxAngle) * 0.5f );
	
		_shader->setUniform1f("grassSwayingTimeScale", swayingTimeScale );
		_shader->setUniform1f("grassSwayingNoiseFrequency", swayingNoiseSpaceFrequency );
	
		ofSetColor( ofColor::white );
	
		tendrilMesh.draw();
	
	_shader->end();
}

//-----------------------------------------------------------------------------------------
//
void ofApp::computeMesh()
{
	
	ofMesh srcMesh = ofMesh::sphere( placementSize, placementResolution, OF_PRIMITIVE_TRIANGLES );
	// Todo: swap in other meshes
	
	tendrilMesh.clear();
	tendrilMesh.setMode( OF_PRIMITIVE_LINES );
	
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
				tendrilMesh.addVertex( vertexPos );
				tendrilMesh.addVertex( vertexPos + ( normals[i] * placementPerlinVal) ); // the normal has a length we later use to module tendril height
			}
		}
	}
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
