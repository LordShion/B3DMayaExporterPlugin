//-------------------------------------------------------------------------------------------------
/// \file	Blitz3DShader.cpp
/// \date	6-11-04
/// \author	Rob Bateman
/// \brief	This shader node will eventually turn into a phong shader - for the moment however
///			it just passes through a constant colour as it's output.
///
//-------------------------------------------------------------------------------------------------

#include <Blitz3DShader.h>

const MTypeId Blitz3DShader::m_TypeId( 0x70033 );
const MString Blitz3DShader::m_TypeName( "Blitz3DShader" );


/// The diffuse colour of the material
MObject  Blitz3DShader::m_aDiffuseColor;

/// The output colour of the material
MObject  Blitz3DShader::m_aOutColor;


//
void* Blitz3DShader::creator()
{
    return new Blitz3DShader();
}

//
MStatus Blitz3DShader::initialize()
{
	// we will use a numeric attribute function set to add all of
	// the attributes to the node description.
    MFnNumericAttribute nAttr;

    // create the diffuse colour attribute
	m_aDiffuseColor =  nAttr.createColor( "color", "c");
	nAttr.setStorable(true);
	nAttr.setKeyable(true);
	nAttr.setDefault(0.7f, 0.7f, 0.7f);

    // create the output colour attribute
	m_aOutColor =  nAttr.createColor( "outColor", "oc");
	nAttr.setStorable(false);
	nAttr.setReadable(true);
	nAttr.setWritable(false);




	// add the various attributes to the node
	addAttribute(m_aDiffuseColor);
	addAttribute(m_aOutColor);

	// set the attribute dependencies
	attributeAffects(m_aDiffuseColor,  m_aOutColor);

	// done
    return MS::kSuccess;
}


// computes the output colour. In a software shader this would be used to calculate an output
// colour for a pixel during rendering. There are 2 occasions when this takes place...
//
// 1. When the scene is rendered
// 2. When the sample swatch in the Attribute Editor gets rendered.
//
// In our case then, it will only get called when the swatch gets evaluated. Only software shaders
// can get rendered by the maya renderer...
//
MStatus Blitz3DShader::compute (const MPlug& plug, MDataBlock& data)
{
	// Check that the requested recompute is one of the output values
	//
	if( (plug != m_aOutColor) &&
		(plug.parent() != m_aOutColor) )
		return MS::kUnknownParameter;

	// get the diffuse colour from the data block. We need to first get hold
	// of a handle to the attribute before querying it's actual value
	//
	MDataHandle inputData = data.inputValue(m_aDiffuseColor);

	// use the handle to get the diffuse color value
	const MFloatVector & color = inputData.asFloatVector();


	// again, get hold of a handle to the output attribute... This time
	// however we get an outputHandle.
	//
	MDataHandle outColorHandle = data.outputValue( m_aOutColor );

	// get a reference to the actual data
    MFloatVector & outColor = outColorHandle.asFloatVector();

	// set the output colour to the input colour
	outColor = color;

	// clean the plug, ie flag it as clean so that maya does not re-evaluate this attribute
	outColorHandle.setClean();

	return MS::kSuccess;
}
