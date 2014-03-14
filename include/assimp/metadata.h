/*
---------------------------------------------------------------------------
Open Asset Import Library (assimp)
---------------------------------------------------------------------------

Copyright (c) 2006-2012, assimp team

All rights reserved.

Redistribution and use of this software in source and binary forms, 
with or without modification, are permitted provided that the following 
conditions are met:

* Redistributions of source code must retain the above
  copyright notice, this list of conditions and the
  following disclaimer.

* Redistributions in binary form must reproduce the above
  copyright notice, this list of conditions and the
  following disclaimer in the documentation and/or other
  materials provided with the distribution.

* Neither the name of the assimp team, nor the names of its
  contributors may be used to endorse or promote products
  derived from this software without specific prior
  written permission of the assimp team.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
---------------------------------------------------------------------------
*/

/** @file metadata.h
 *  @brief Defines the data structures for holding node meta information.
 */
#ifndef __AI_METADATA_H_INC__
#define __AI_METADATA_H_INC__

#include <stdint.h>



// -------------------------------------------------------------------------------
/**
  * Enum used to distinguish data types
  */
 // -------------------------------------------------------------------------------
enum aiType
{
	AI_BOOL = 0, 
	AI_INT = 1, 
	AI_UINT64 = 2, 
	AI_FLOAT = 3, 
	AI_AISTRING = 4,
	AI_AIVECTOR3D = 5,

	FORCE_32BIT = INT_MAX
};



// -------------------------------------------------------------------------------
/**
  * Metadata entry
  *
  * The type field uniquely identifies the underlying type of the data field
  */
 // -------------------------------------------------------------------------------
struct aiMetaDataEntry
{
	aiType type;
	void* data;
};



#ifdef __cplusplus

// -------------------------------------------------------------------------------
/**
  * Helper functions to get the aiType enum entry for a type
  */
 // -------------------------------------------------------------------------------
inline aiType GetAiType( bool ) { return AI_BOOL; }
inline aiType GetAiType( int ) { return AI_INT; }
inline aiType GetAiType( uint64_t ) { return AI_UINT64; }
inline aiType GetAiType( float ) { return AI_FLOAT; }
inline aiType GetAiType( aiString ) { return AI_AISTRING; }
inline aiType GetAiType( aiVector3D ) { return AI_AIVECTOR3D; }



// -------------------------------------------------------------------------------
/**
  * Transform
  *
  * Applies the callable, c, to the given data of the given type.
  * The callable, c, is expected to have the following interface
  *
  *		c( T* data )
  *
  * where T can be any type with a corresponding entry in the aiType enum.
  */
 // -------------------------------------------------------------------------------
template<typename callable>
inline void transform( aiType type, void* data, callable c )
{
	switch (type) 
	{
	case AI_BOOL:
		callable(static_cast<bool*>(data));
		break;
	case AI_INT:
		callable(static_cast<int*>(data));
		break;
	case AI_UINT64:
		callable(static_cast<uint64_t*>(data));
		break;
	case AI_FLOAT:
		callable(static_cast<float*>(data));
		break;
	case AI_AISTRING:
		callable(static_cast<aiString*>(data));
		break;
	case AI_AIVECTOR3D:
		callable(static_cast<aiVector3D*>(data));
		break;
	default:
		assert(false);
		break;
	}
}

// -------------------------------------------------------------------------------
/**
  * Transform. 
  * 
  * This is a convenience overload for aiMetaDataEntry's.
  */
 // -------------------------------------------------------------------------------
template<typename callable>
inline void transform( aiMetaDataEntry entry, callable c )
{ transform(entry.type, entry.data, c); }

#endif



// -------------------------------------------------------------------------------
/**
  * Container for holding metadata.
  *
  * Metadata is a key-value store using string keys and values.
  */
 // -------------------------------------------------------------------------------
struct aiMetadata 
{
	/** Length of the mKeys and mValues arrays, respectively */
	unsigned int mNumProperties;

	/** Arrays of keys, may not be NULL. Entries in this array may not be NULL as well. */
	C_STRUCT aiString* mKeys;

	/** Arrays of values, may not be NULL. Entries in this array may be NULL if the
	  * corresponding property key has no assigned value. */
	C_STRUCT aiMetaDataEntry* mValues;

#ifdef __cplusplus

	/** Constructor */
	aiMetadata()
		// set all members to zero by default
		: mKeys(NULL)
		, mValues(NULL)
		, mNumProperties(0)
	{}


	/** Destructor */
	~aiMetadata()
	{
		if (mKeys)
			delete [] mKeys;
		if (mValues)
		{
			// Delete each metadata entry
			for (unsigned i=0; i<mNumProperties; ++i)
				transform(mValues[i], (void (*)(void*))(operator delete));
			// Delete the metadata array
			delete [] mValues;
		}
		
	}



	template<typename T>
	inline void Set( unsigned index, const std::string& key, const T& value )
	{
		// In range assertion
		assert(index < mNumProperties);

		// Set metadata key
		mKeys[index] = key;

		// Set metadata type
		mValues[index].type = GetAiType(value);
		// Copy the given value to the dynamic storage
		mValues[index].data = new T(value);
	}

	template<typename T>
	inline bool Get( unsigned index, T& value )
	{
		// Return false if the output data type does 
		// not match the found value's data type
		if (GetAiType(value) != mValues[index].type)
			return false;

		// Otherwise, output the found value and 
		// return true
		value = *static_cast<T*>(mValues[index].data);
		return true;
	}

	template<typename T>
	inline bool Get( const aiString& key, T& value )
	{
		// Search for the given key
		for (unsigned i=0; i<mNumProperties; ++i)
			if (mKeys[i]==key)
				return Get(i, value);
		return false;
	}

#endif // __cplusplus
};

#endif // __AI_METADATA_H_INC__


