//
//  ofMaterialExt.h
//  GeometryShaderTubes
//
//  Created by Andreas Müller on 06/12/2014.
//
//

#pragma once 

#include "ofMaterial.h"

// ------------------------------------------------------------------------
//
class ofMaterialExt : public ofMaterial
{
	public:
	
		// --------------------------------------------
		void setParams( ofShader* _shader )
		{
			_shader->setUniform4fv("materialDiffuse", getDiffuseColor().v );
			_shader->setUniform4fv("materialSpecular", getSpecularColor().v );
			_shader->setUniform1f("materialShininess", getShininess() );
		}
};
