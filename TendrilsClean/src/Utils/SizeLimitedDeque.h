//
//  SizeLimitedDeque.h
//  LostSignalTrackingNew
//
//  Created by Andreas Müller on 26/08/2014.
//
//

#pragma once 

#include <string>
using namespace std;

template <class T>
class SizeLimitedDeque
{
	public:

		// -----------------------------------------
		SizeLimitedDeque()
		{
			maxSize = -1;
		}
	
		// -----------------------------------------
		void pushFront (const T& _val)
		{
			dat.push_front( _val );
			limitSizeFromBack();
		}

		// -----------------------------------------
		void pushBack (const T& _val)
		{
			dat.push_back( _val );
			limitSizeFromFront();
		}
	
		// -----------------------------------------
		T at( int _index )
		{
			return dat.at(_index);
		}

		// -----------------------------------------
		size_t size()
		{
			return dat.size();
		}
	
		// -----------------------------------------
		void setMaxSize( int _size )
		{
			maxSize = _size;
		}
	
	
		// -----------------------------------------
		int getMaxSize()
		{
			return maxSize;
		}
	
		deque<T> dat;
	
	protected:

		// -----------------------------------------
		void limitSizeFromBack()
		{
			if( maxSize > -1 )
			{
				if( dat.size() > maxSize )
				{
					while( dat.size() > maxSize )
					{
						dat.pop_back();
					}
				}
			}
		}
	
		// -----------------------------------------
		void limitSizeFromFront()
		{
			if( maxSize > -1 )
			{
				if( dat.size() > maxSize )
				{
					while( dat.size() > maxSize )
					{
						dat.pop_front();
					}
				}
			}
		}
	
		int maxSize;
};