#include <maya/MPxFileTranslator.h>
#include <maya/MStringArray.h>
#include <maya/MFnPlugin.h>
#include <maya/MPxCommand.h>
#include <maya/MItDag.h>
#include <maya/MObject.h>
#include <maya/MGlobal.h>
#include <maya/MStatus.h>
#include <maya/MDagPath.h>

#include <maya/MFnMesh.h>
#include <maya/MFnSet.h>
#include <maya/MPointArray.h>
#include <maya/MArgList.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MItMeshVertex.h>
#include <maya/MSelectionList.h>
#include <maya/MItSelectionList.h>
#include<maya/MQuaternion.h>
#include<maya/MEulerRotation.h>

#include<maya/MFnTransform.h>


#include<maya/MFnLambertShader.h>
#include<maya/MFnBlinnShader.h>
#include<maya/MFnPhongShader.h>
#include<maya/MItDependencyNodes.h>
#include<maya/MItGeometry.h>

#include<maya/MPlugArray.h>
#include<maya/MFnDependencyNode.h>
 	

#include<maya/MAnimControl.h>
#include <maya/MFnAnimCurve.h>
#include <maya/MFnSkinCluster.h>


#include<maya/MDagPathArray.h>

#include<maya/MMatrix.h>
#include<maya/MFnIkJoint.h>


#include <fstream>
#include <iostream>
#include "getrelativepath.h"

//#define TRACE
#define EXPORT_NEW_TEXTURES
#define EXPORT_NEW_BRUSHES
#define EXPORT_TEXTURES
#define EXPORT_BRUSHES
//#define OLD_BRUSH
//#define SCENE_ROOT
#define EXPORT_NODES
#define EXPORT_MESH
#define EXPORT_VRTS

//#define OLD_TRIS

#define INV(valeur) (1-valeur)


using namespace std;

//mes variables globales
unsigned int nbMaxb3dTextures = 8;
MString affich;
MStringArray Matid;
MStringArray Texid;
MIntArray nb_Tex_by_Brush,Texids_by_brush,brushid_by_tex;


		MIntArray posMat;
		MIntArray nbTexMat;
		MStringArray TexNb;

		MStringArray UvSetNames;
		MObjectArray Textures;

		//MFnMesh GlobalMesh;

//---gestion fichier---------
fstream output;
MIntArray posfichier;
MString output_base_directory;

MDagPathArray infs;
unsigned int nInfs;
unsigned int nGeoms;
MDagPath skinPath;

MIntArray Keys;
MFloatArray Values;

MAnimControl animctrl; 


struct TEXSChunck{
char * filename;
int alpha;
};
struct B3Dfile{
	char * filename;
	TEXSChunck TEXS;


};
struct Flags_export{

	int default_brush;
	int vertex_colors;
	int use_vertex_colors;
	int Normals;
	int export_hierarchy;
	int export_brushes;
	int export_textures;
	int use_relative_path;
	int UseRoot;
	int exportAnim;


};

Flags_export Flags;


void Affich(MString affich){
	
	MGlobal::displayInfo(affich);

}
int Hierar_pos(MDagPath path){
	MStringArray chemin;
	path.fullPathName().split((char)'|',chemin);

	return chemin.length();
}

void write_int(int entier)
{
	output.write((char *)&entier,sizeof(entier));
}

void write_float(float flottant)
{
	output.write((char *)&flottant,sizeof(flottant));
}

void write_double(double donnee)
{
	output.write((char *)&donnee,sizeof(donnee));
}

void StartChunck(){
	int tempo;
	int tata = ::output.tellp();
	posfichier.append(tata);
/*		
		affich = " start position: ";
		tempo = posfichier.length()-1;
		affich += tempo;
	
		
		
		affich += " rec adress : ";
		affich += tata;
		Affich(affich);
*/
		write_int(1); // écriture d'une valeur quelconque
}

void EndChunck() // fonction pour écrire une longueur par rapport à position actuelle dans le fichier
{

	


		int toto = posfichier.length()-1;
	/*	affich = "end position : ";
		affich += toto;
		
		
		affich += " address : ";
		affich += posfichier[toto];
*/
	

		//EndChunck(posfichier[temp-1]);
			int pos_new = output.tellp();//retient la position actuelle
	int size = pos_new - posfichier[toto];// détermine la longueur
	size -= 4 ; // pour les 4 premiers octets
	output.seekp(posfichier[toto]);// se place à la position indiqué
	write_int(size);// écrit la longueur
	output.seekp(pos_new);// se repositionne à la position transmise
		
	/*	affich += " taille : ";
		affich += size;

		Affich(affich);
		
		*/
		posfichier.remove(toto);

}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : rgb_to_hsl
@INPUT      : r
               g
               b
 @OUTPUT     : h
               s
               l
 @RETURNS    : 
 @DESCRIPTION: Converts an RGB colour to a HSL (Hue-Saturation-Light) colour.
               This code was taken from Graphics Gems.
 @METHOD     : 
 @GLOBALS    : 
 @CALLS      : 
 @CREATED    : 1993            David MacDonald
 @MODIFIED   : 
 ---------------------------------------------------------------------------- */
 
   float  getColorLuminosity(float r,float g,float b)
   {
	   //,
     //Real    *h,
     //Real    *s,
     //Real    *l )
 //{
     float  v, m, vm,l;// r2, g2, b2,h,l,s;
 
    // check_initialize_colours();
 
    // v = MAX3( r, g, b );
	 v = max( r, max(g, b) );
 
     //m = MIN3( r, g, b );
	 m = min( r, min(g, b) );
 
     l = (m + v) / float(2.0);
 
   /*  if( l > float(0.0) )
	 {
         vm = v - m;
         s = vm;
 
         if( s > float(0.0) )
         {
             if( l <= float(0.5) )
                 s /= v + m;
             else
                 s /= float(2.0) - v - m;
 
             r2 = (v - r) / vm;
             g2 = (v - g) / vm;
             b2 = (v - b) / vm;
 
             if( r == v )
             {
                 if( g == m )
                     h = float(5.0) + b2;
                 else
                     h = float(1.0) - g2;
             }
             else if( g == v )
             {
                 if( b == m )
                     h = float(1.0) + r2;
                 else
                     h = float(3.0) - b2;
             }
             else
             {
                 if( r == m )
                     h = float(3.0) + g2;
                 else
                     h = float(5.0) - r2;
             }
 
             h /= float(6.0);
         }
     }*/
	 return l;
   }


	// our plugin file translator class. This must inherit
	// from the base class MPxFileTranslator
class MayaFileTranslator : public MPxFileTranslator	{
		public:
  	

		MayaFileTranslator() : MPxFileTranslator() {};
		~MayaFileTranslator() {};
	// we overload this method to perform any
	// file export operations needed
		MStatus writer( 	const MFileObject& file,
		const MString& optionsString,
		FileAccessMode mode);


	// returns true if this class can export files
		bool haveWriteMethod() const {
  		return true;

		}
	// returns the default extension of the file supported
	// by this FileTranslator.
		MString defaultExtension() const {
  			return "b3d";

		}
	// Used by Maya to create a new instance of our
	// custom file translator
		static void* creator(){
  			return new MayaFileTranslator;

		}


};

//--------------------------------------------------------------------------------------
class addBlitz3DImageFileAttributes : public MPxCommand {
 public:

	/// \brief	executes our function
	/// \param	args	-	the argument list specified
	/// \return	MS::kSuccess or MS::kFailure
	///
	virtual MStatus doIt( const MArgList& args ) ;
	
	/// \brief	This function is used by maya to create a new instance of the function
	/// 		object, each time the command is called. This only really makes sense 
	/// 		later when dealing with undo and redo. The function object will create a
	/// 		list of things to undo so maya can store this command in it's history.
	/// \return	a new instance of this function
	/// 
	static void* creator() {
		return new addBlitz3DImageFileAttributes;
	}
 };




class addBlitz3DShaderAttributes : public MPxCommand {
 public:

	/// \brief	executes our function
	/// \param	args	-	the argument list specified
	/// \return	MS::kSuccess or MS::kFailure
	///
	virtual MStatus doIt( const MArgList& args ) ;
	
	/// \brief	This function is used by maya to create a new instance of the function
	/// 		object, each time the command is called. This only really makes sense 
	/// 		later when dealing with undo and redo. The function object will create a
	/// 		list of things to undo so maya can store this command in it's history.
	/// \return	a new instance of this function
	/// 
	static void* creator() {
		return new addBlitz3DShaderAttributes;
	}
 };

