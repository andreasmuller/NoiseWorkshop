#pragma once

#include "ofMain.h"

class ofTrueTypeFontExt : public ofTrueTypeFont
{
	public:
	
	// -----------------------------------------------------------------
	void drawStringShadowed( string _s, ofVec2f _pos,
							ofColor _frontColor = ofColor(255,255,255), ofColor _backColor = ofColor(0,0,0) )
	{
		drawStringShadowed( _s, _pos.x, _pos.y, _frontColor, _backColor );
	}
	
		// -----------------------------------------------------------------
		void drawStringShadowed( string _s, float _x, float _y,
								 ofColor _frontColor = ofColor(255,255,255), ofColor _backColor = ofColor(0,0,0) )
		{
			ofSetColor( _backColor );
			drawString( _s, _x + 1, _y + 1 );
			
			ofSetColor( _frontColor );
			drawString( _s, _x, _y );
		}

		// -----------------------------------------------------------------
		void drawTextureAtlas( float _x, float _y, float _w, float _h )
		{

			if( _w == 0.0f || _h == 0.0f )
			{
				_w = texAtlas.getWidth();
				_h = texAtlas.getHeight();					
			}

			texAtlas.draw( _x, _y, _w, _h );
		}
		
};
