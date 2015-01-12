#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{

	resolution.addListener(this, &ofApp::resolutionChanged );
	
	gui.setup( "Main", "Main.xml" );

	gui.add( resolution.set( "Resolution", 512, 2, 1024 ) );
	gui.add( noiseFrequency.set( "Noise Frequency", 80.0f, 0.00001f, 4096.0f ) );
	gui.add( animate.set( "Animate", false ) );

	gui.add( doThreshold.set( "Threshold", false ) );
	gui.add( thresholdLow.set( "Treshold Low", 0, 0, 255 ) );
	gui.add( thresholdHigh.set( "Treshold High", 127, 0, 255 ) );
	gui.add( invert.set( "Invert Threshold", false ) );
	
	noiseStartX = 0;
	noiseStartY = 0;
	
	
	drawGui = true;
	
	noiseImage.allocate( resolution, resolution, OF_IMAGE_GRAYSCALE );
}

//--------------------------------------------------------------
void ofApp::update()
{
	if( ofGetKeyPressed(OF_KEY_LEFT)  )  { noiseStartX -= 5; }
	if( ofGetKeyPressed(OF_KEY_RIGHT ) ) { noiseStartX += 5; }
	if( ofGetKeyPressed(OF_KEY_UP)  )    { noiseStartY -= 5; }
	if( ofGetKeyPressed(OF_KEY_DOWN ) )  { noiseStartY += 5; }
	
	float time = 0.0f;
	if( animate ) time = ofGetElapsedTimef();
	
	ofPixelsRef pixels = noiseImage.getPixelsRef();
	
	int tmpIndex = 0;
	for( int y = 0; y < noiseImage.getHeight(); y++ )
	{
		for( int x = 0; x < noiseImage.getWidth(); x++ )
		{
			// We're grabbing a single reading from the noise function here, but we can also add together more
			// readings at different frequencies to make more complex patterns
			
			float tmpNoise = ofNoise( (noiseStartX + x) / noiseFrequency, (noiseStartY + y) / noiseFrequency, time );
			
			pixels[tmpIndex] = tmpNoise * 255.0f;
			tmpIndex++;
		}
	}
	
	if( doThreshold)
	{
		threshold( false );
	}
	
	noiseImage.update(); // uploads the new pixels to the gfx card
}

//--------------------------------------------------------------
void ofApp::threshold( bool _upload )
{
	ofPixelsRef pixels = noiseImage.getPixelsRef();
	
	int tmpIndex = 0;
	for( int y = 0; y < noiseImage.getHeight(); y++ )
	{
		for( int x = 0; x < noiseImage.getWidth(); x++ )
		{
			if( pixels[tmpIndex] >= thresholdLow && pixels[tmpIndex] <= thresholdHigh )
			{
				pixels[tmpIndex] = 255;
			}
			else
			{
				pixels[tmpIndex] = 0;
			}
			   
			if( invert )
			{
				pixels[tmpIndex] = 255 - pixels[tmpIndex];
			}
			
			tmpIndex++;
		}
	}

	
	if( _upload )
	{
		noiseImage.update();
	}
}


//--------------------------------------------------------------
void ofApp::draw()
{
	ofRectangle imageRect(0,0,noiseImage.getWidth(), noiseImage.getHeight() );
	imageRect.scaleTo( ofRectangle(0,0,ofGetWidth(),ofGetHeight()) );
	noiseImage.draw( imageRect );
	
	if( drawGui )
	{
		gui.draw();
	}
	
}

//--------------------------------------------------------------
void ofApp::resolutionChanged( int& _res )
{
	noiseImage.allocate( _res, _res, OF_IMAGE_GRAYSCALE );
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}