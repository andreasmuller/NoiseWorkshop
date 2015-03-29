
#pragma once

#include "BoundingBox.h"

class OrientedBoundingBox : public BoundingBox
{
	public:


		// --------------------------------------------
		void setOrientation( ofQuaternion _orientation )
		{
			orientation = _orientation;
			transformHasChanged = true;
		}

		// --------------------------------------------
		bool pointIsInside( ofVec3f _point )
		{
			if( transformHasChanged )
			{
				worldToBox = getWorldToBoxmatrix();
				transformHasChanged = false;
			}

			ofVec3f pointBoxSpace = _point * worldToBox;
			return localSpacePointIsInWithinBoxSize( pointBoxSpace );
		}


		// --------------------------------------------
		void draw()
		{
			ofNoFill();
				ofPushMatrix();
				ofTranslate( pos );
					ofMatrix4x4 rotMat;
					orientation.get( rotMat );
					ofMultMatrix( rotMat );
					ofDrawBox( ofVec3f(0,0,0), size.x, size.y, size.z );
				ofPopMatrix();
			ofFill();
		}

	private:

		// --------------------------------------------
		ofMatrix4x4 getWorldToBoxmatrix()
		{
			ofMatrix4x4 tmpWorldToBoxMat;
			orientation.get( tmpWorldToBoxMat );
			tmpWorldToBoxMat = tmpWorldToBoxMat * ofMatrix4x4::newTranslationMatrix( pos );
			tmpWorldToBoxMat = tmpWorldToBoxMat.getInverse();
			return tmpWorldToBoxMat;
		}

		ofMatrix4x4 worldToBox;
		ofQuaternion orientation;
};