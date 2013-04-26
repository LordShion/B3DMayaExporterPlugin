//-------------------------------------------------------------------------------------------------
/// \file	Blitz3DShader.h
/// \date	6-11-04
/// \author	Rob Bateman
/// \brief	This shader node will eventually turn into a phong shader - for the moment however
///			it just passes through a constant colour as it's output.
///
//-------------------------------------------------------------------------------------------------

#ifndef __BASIC_SW_SHADER__H__
#define __BASIC_SW_SHADER__H__

//	#ifdef WIN32
//		#define NT_PLUGIN
//		#pragma once
//		#define WIN32_LEAN_AND_MEAN
//		#include <windows.h>
//	#endif

	#include <maya/MIOStream.h>
	#include <maya/MString.h>
	#include <maya/MPlug.h>
	#include <maya/MDataBlock.h>
	#include <maya/MDataHandle.h>
	#include <maya/MFnNumericAttribute.h>
	#include <maya/MFloatVector.h>
	#include <maya/MPxNode.h>


	//---------------------------------------------------------------------------------------------
	/// \brief	This shader is the first stage towards creating a Software Shader.
	///
	///
	///
	class Blitz3DShader
		: public MPxNode
	{
	public:

		/// \brief	The compute function is called to calculate the value of a specific
		/// 		output attribute. This usually only happens when an input value changes
		/// 		on which the output was dependant on; or when the node is forced to be
		/// 		recomputed because it has been flagged as dirty.
		/// \param	plug	-	the output attribute to calculate
		/// \param	data	-	the data from which you MUST read the attributes back from.
		///	\return	Did it succeed or fail?
		///
		/// \note	YOU MUST ONLY EVER READ THE NODE'S ATTRIBUTES BACK FROM THE DATA BLOCK
		/// 		GIVEN TO YOU. YOU MUST NOT ATTEMPT TO USE ANY FUNCTION SETS, AND YOU MUST
		/// 		NOT ACCESS ANY OTHER NODES WHILST IN THE COMPUTE FUNCTION.
		///
		/// 		THE COMPUTE FUNCTION IS THE ONLY FUNCTION ON OUR NODE, THAT IS LIKELY TO
		/// 		BE USED DURING RENDERING. BY ACCESSING ONLY THE ATTRIBUTES VIA THE DATA
		/// 		BLOCK, MAYA ENSURES THAT RENDERING REMAINS THREAD SAFE. DUPLICATE
		/// 		DATA BLOCKS CAN BE PASSED TO TWO OR MORE PROCESSORS. THIS MEANS THAT NO
		/// 		SHARED DATA WILL BE ACCESSED BY DIFFERENT THREADS. ie, EACH THREAD RECIEVES
		/// 		IT'S OWN COPY OF THE DATA.
		///
		/// 		IF YOU DO START ACCESSING GLOBAL DATA, OR DATA FROM OTHER NODES WITHIN THE
		/// 		COMPUTE FUNCTION - IT WILL PROBABLY WORK WHILST YOU'RE WORKING INSIDE MAYA,
		/// 		HOWEVER - IT IS LIABLE TO CRASH WHEN RENDERING, ie WHEN MULTIPLE PROCESSORS
		/// 		ARE USED.
		///
		virtual MStatus compute( const MPlug&, MDataBlock& );

	public:

		/// \brief	This function returns a new instance of our shader
		/// \return	A new shader instance
		///
		static  void *  creator();

		/// \brief	This function sets up the attributes on the shader node
		/// \return	ok or fail
		///
		static  MStatus initialize();

	public:

		// 	------------ The Type Info ------------

		/// The specific ID for this node type
		const static  MTypeId m_TypeId;

		/// The type name for this new node type
		const static  MString m_TypeName;

	private:

		// 	------------ The attributes ------------


		/// The diffuse colour of the material
		static MObject m_aDiffuseColor;

		/// The output colour for the material
		static MObject m_aOutColor;


	};


#endif
