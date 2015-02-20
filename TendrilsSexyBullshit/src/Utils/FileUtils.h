//
//  FileUtils.h
//  ParticleMaker
//
//  Created by Andreas Müller on 04/03/2014.
//
//

#pragma once

class FileUtils
{
	public:
				
		// ---------------------------------------------------------------------------------------------------------------------------------------------------
		//
		static vector<string> getFilePathsInFolder( string _folder, string _extension )
		{
			vector<string> tmpExt;
			tmpExt.push_back( _extension );
			return getFilePathsInFolder( _folder, tmpExt );
		}
		
		// ---------------------------------------------------------------------------------------------------------------------------------------------------
		//
		static vector<string> getFilePathsInFolder( string _folder, vector<string> _extensions )
		{
			vector<string> filePaths;
			
			ofDirectory tmpDir( _folder );
			for( unsigned int i = 0; i < _extensions.size(); i++ )
			{
				tmpDir.allowExt( _extensions.at(i) );
			}
			
			int numFiles = tmpDir.listDir( _folder );
			for( int i = 0; i < numFiles; i++ )
			{
				filePaths.push_back( tmpDir.getPath(i) );
			}
			
			return filePaths;
		}

		// ---------------------------------------------------------------------------------------------------------------------------------------------------
		//
		static string getFirstFileOfTypeInFolder( string _folder, string _extension )
		{
			vector<string> tmpFiles = getFilePathsInFolder( _folder, _extension );
			if( tmpFiles.size() > 0 )
			{
				return tmpFiles.at(0);
			}
			return "";
		}
	
		// ---------------------------------------------------------------------------------------------------------------------------------------------------
		//
		static vector<string> getImageFilePathsInFolder( string _folder )
		{
			vector<string> tmpExt;
			tmpExt.push_back( "png" );
			tmpExt.push_back( "bmp" );
			tmpExt.push_back( "jpg" );
			tmpExt.push_back( "tga" );
			
			return getFilePathsInFolder( _folder, tmpExt );
		}
	
		// ---------------------------------------------------------------------------------------------------------------------------------------------------
		//
		static void loadImagesInFolder( string _folder, string _extension, vector<ofImage*>& _images )
		{
			vector<string> tmpExt;
			tmpExt.push_back( _extension );
			return loadImagesInFolder( _folder, tmpExt, _images );
		}
		
		// ---------------------------------------------------------------------------------------------------------------------------------------------------
		//
		static void loadImagesInFolder( string _folder, vector<string> _extensions, vector<ofImage*>& _images )
		{
			vector<string> textureFilePaths = FileUtils::getFilePathsInFolder( _folder, _extensions );
			
			for( unsigned int i = 0; i < textureFilePaths.size(); i++ )
			{
				ofImage* tmpIm = new ofImage();
				tmpIm->loadImage( textureFilePaths.at(i) );
				_images.push_back( tmpIm );
			}
		}
	
};
