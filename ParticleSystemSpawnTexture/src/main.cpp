#include "ofMain.h"
#include "ofApp.h"
#include "ofAppGLFWWindow.h"

//========================================================================
int main()
{
	//ofSetCurrentRenderer( ofGLProgrammableRenderer::TYPE );
	//ofSetOpenGLVersion( 3, 2 );

	ofAppGLFWWindow window;
	window.setMultiDisplayFullscreen(true);
	//window.setNumSamples(4);

	ofSetupOpenGL( &window, 1360,768, OF_WINDOW);
	//ofSetupOpenGL(1280,620, OF_WINDOW);
	ofRunApp( new ofApp());

}
