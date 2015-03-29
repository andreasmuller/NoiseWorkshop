
#pragma once

#include "ofMain.h"

class BoundingBox
{
	public:

		// --------------------------------------------
		BoundingBox()
		{
			transformHasChanged = true;
		}

		// --------------------------------------------
		void setPosition( ofVec3f _pos )
		{
			pos = _pos; 
			transformHasChanged = true;
		}

		// --------------------------------------------
		ofVec3f getPosition()
		{
			return pos;
		}

		// --------------------------------------------
		void setSize( ofVec3f _size )
		{
			size = _size;
			sizeHalf = size * 0.5f;
			transformHasChanged = true;
		}

		// --------------------------------------------
		ofVec3f getSize()
		{
			return size;
		}

		// --------------------------------------------
		bool pointIsInside( ofVec3f _point )
		{
			ofVec3f pointBoxSpace = _point - pos;
			return localSpacePointIsInWithinBoxSize( pointBoxSpace );
		}

		// --------------------------------------------
		void calculateAABoundingBox( vector<ofVec3f>& _points )
		{
			ofVec3f min(  99999999999999999,  99999999999999999,  99999999999999999 );
			ofVec3f max( -99999999999999999, -99999999999999999, -99999999999999999 );

			for( unsigned int i = 0; i < _points.size(); i++ )
			{
				ofVec3f p = _points.at(i);

				min.x = MIN( min.x, p.x );
				min.y = MIN( min.y, p.y );
				min.z = MIN( min.z, p.z );

				max.x = MAX( max.x, p.x );
				max.y = MAX( max.y, p.y );
				max.z = MAX( max.z, p.z );
			}

			setSize( max - min );
			setPosition( min.getInterpolated( max, 0.5f ) );
		}

		// --------------------------------------------
		void draw()
		{
			ofNoFill();
				ofPushMatrix();
				ofTranslate( pos );
					ofDrawBox( ofVec3f(0,0,0), size.x, size.y, size.z );
				ofPopMatrix();
			ofFill();
		}

	protected:

		// --------------------------------------------
		bool localSpacePointIsInWithinBoxSize(  ofVec3f _point )
		{
			if( abs(_point.x) < sizeHalf.x &&
				abs(_point.y) < sizeHalf.y && 
				abs(_point.z) < sizeHalf.z )
			{
				return true;
			}

			return false;	
		}

		ofVec3f pos;
		ofVec3f size;
		ofVec3f sizeHalf;

		bool transformHasChanged;
};