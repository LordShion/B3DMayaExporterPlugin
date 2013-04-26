//
// Copyright (C) NekoTech 
// 
// File: pluginMain.cpp
//
// Author: Maya Plug-in Wizard 2.0
//


#include <maya/MFnPlugin.h>
#include <maya/MPxCommand.h>
#include <maya/MItDag.h>
#include <maya/MObject.h>
#include <maya/MGlobal.h>
#include <maya/MStatus.h>
#include <maya/MDagPath.h>
#include <maya/MFnMesh.h>
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

#include<maya/MPlugArray.h>
#include<maya/MFnDependencyNode.h>




#include <fstream>
#include <iostream>

#include <Blitz3Dexporteur.h>


// under WIN32 we need to export two functions
// under linux compiling with -shared does the job...
#ifdef WIN32
#define MLL_EXPORT __declspec(dllexport)
#else
#define MLL_EXPORT
#endif

using namespace std;

// specifies a script for the Export Options UI
char* g_OptionScript = "MayaBlitzFileExportScript";

// the default option string
char * g_DefaultExportOptions = "";

MString info;

fstream output;
// stockage position fichier
int pos_file,pos_first_node,pos_mesh,pos_vrts,pos_tri,pos_final_nodes;

int hierarchie=0;
int childs=0;
MObject old_object,new_object;

struct Flags_export{

	int default_brush;
	int vertex_colors;
	int use_vertex_colors;
	int Normals;

};

Flags_export Flags;



void write_int(int entier)
{
	::output.write((char *)&entier,sizeof(entier));
}

void write_float(float flottant)
{
	::output.write((char *)&flottant,sizeof(flottant));
}

void write_double(double donnee)
{
	::output.write((char *)&donnee,sizeof(donnee));
}

void EndChunck(int pos_old) // fonction pour écrire une longueur par rapport à position actuelle dans le fichier
{
	int pos_new = ::output.tellp();//retient la position actuelle
	int size = pos_new - pos_old;// détermine la longueur
	size -= 4 ; // pour les 4 premiers octets
	::output.seekp(pos_old);// se place à la position indiqué
	write_int(size);// écrit la longueur
	::output.seekp(pos_new);// se repositionne à la position transmise
	
}
	// fin donnees et fonctions persos






//*********************exportation des materiaux



 #include<maya/MPlugArray.h>
#include<maya/MFnDependencyNode.h>

bool OutputColor(MFnDependencyNode& fn,const char* name)
{
  	MPlug p;

MString r = name;
r += "R";
MString g = name;
g += "G";
MString b = name;
b += "B";
MString a = name;
a += "A";

// get the color value
MColor color;

// get a plug to the attribute
//p = fn.findPlug(r.c_str());
p = fn.findPlug(r.asChar());
p.getValue(color.r);
//p = fn.findPlug(g.c_str());
p = fn.findPlug(g.asChar());
p.getValue(color.g);
//p = fn.findPlug(b.c_str());
p = fn.findPlug(b.asChar());
p.getValue(color.b);
//p = fn.findPlug(a.c_str());
p = fn.findPlug(a.asChar());
p.getValue(color.a);
//p = fn.findPlug(name.c_str());
//p = fn.findPlug(name.name.asChar());
// will hold the txture node name
MString texname;

// get plugs connected to colour attribute
MPlugArray plugs;
p.connectedTo(plugs,true,false);

// see if any file textures are present
for(int i=0;i!=plugs.length();++i)
{
  	// if file texture found
if(plugs[i].node().apiType() == MFn::kFileTexture)
{
  	// bind a function set to it ....
MFnDependencyNode fnDep(plugs[i].node());

// to get the node name
texname = fnDep.name();

// stop looping
break;

}

}
if( 	name == "color" &&
color.r <0.01 &&
color.g < 0.01 &&
color.b < 0.01)

{
  	color.r=color.g=color.b=0.6f;

}
// output the name, color and texture ID
//cout 	<< "\t" << name << " "
//<< color.r << " "
//<< color.g << " "
//<< color.b << " "
//<< color.a << " tex= "
//<< texname.asChar() << "\n";

return true;

} 


void OutputMaterials()
{
  	// iterate through the mesh nodes in the scene
MItDependencyNodes itDep(MFn::kLambert);

// we have to keep iterating until we get through
// all of the nodes in the scene
//
while (!itDep.isDone())
{
  	switch(itDep.item().apiType()) {
  	// if found phong shader
case MFn::kPhong:
{
MFnPhongShader fn( itDep.item() );
//cout<<"Phong "<<fn.name().asChar()<<"\n";
OutputColor(fn,"ambientColor");
OutputColor(fn,"color");
OutputColor(fn,"specularColor");
OutputColor(fn,"incandescence");
OutputColor(fn,"transparency");
//cout<<"\tcos "<<fn.cosPower()<< endl;
//OutputBumpMap(itDep.item());
//OutputEnvMap(itDep.item());
}
break;
// if found lambert shader
case MFn::kLambert:
{
MFnLambertShader fn( itDep.item() );
//cout<<"Lambert "<<fn.name().asChar()<<"\n";
OutputColor(fn,"ambientColor");
OutputColor(fn,"color");
OutputColor(fn,"incandescence");
OutputColor(fn,"transparency");
//OutputBumpMap(itDep.item());
//OutputEnvMap(itDep.item());
}
break;
// if found blinn shader
case MFn::kBlinn:
{
MFnBlinnShader fn( itDep.item() );
//cout<<"Blinn "<<fn.name().asChar()<<"\n";
OutputColor(fn,"ambientColor");
OutputColor(fn,"color");
OutputColor(fn,"specularColor");
OutputColor(fn,"incandescence");
OutputColor(fn,"transparency");
//cout 	<<"\teccentricity "
//<<fn.eccentricity()<< endl;
//cout 	<< "\tspecularRollOff "
//<< fn.specularRollOff()<< endl;
//OutputBumpMap(itDep.item());
//OutputEnvMap(itDep.item());
}
break;
default:
break;

}
itDep.next();
}

} 
//**********************************



MStatus initializePlugin( MObject obj )
//
//	Description:
//		this method is called when the plug-in is loaded into Maya.  It 
//		registers all of the services that this plug-in provides with 
//		Maya.
//
//	Arguments:
//		obj - a handle to the plug-in object (use MFnPlugin to access it)
//
{ 
	MStatus   status;
	MFnPlugin plugin( obj, "NekoTech", "1.0", "Any");

		// Add plug-in feature registration here
		//
		// an error code

		// register the file translator with Maya
	status = plugin.registerFileTranslator( "Blitz3D",
  	"none",
	MayaFileTranslator::creator,
	(char*)g_OptionScript,
	(char*)g_DefaultExportOptions );

	if (status != MS::kSuccess) {
  			status.perror("Error - initializePlugin");

		}

	return status;
}

MStatus uninitializePlugin( MObject obj )
	//
	//	Description:
	//		this method is called when the plug-in is unloaded from Maya. It 
	//		deregisters all of the services that it was providing.
	//
	//	Arguments:
	//		obj - a handle to the plug-in object (use MFnPlugin to access it)
	//
{
	MStatus   status;
	MFnPlugin plugin( obj );

	// Add plug-in feature deregistration here
	//
	// an error code

	// de-register the file translator with Maya
	status = plugin.deregisterFileTranslator("Blitz3D");

	if (status != MS::kSuccess) {
  			status.perror("Error - uninitializePlugin");

		}


	return status;
}









MStatus MayaFileTranslator::writer( 	const MFileObject& file,
const MString& options,
FileAccessMode mode){




	//-------------------détection des options transmises par le script MEL---------------


	// this will store our option strings
	MStringArray optionList;

	// seperate the option string
	options.split(' ', optionList);

	
	// check all of the options
	int len = optionList.length();

	for( int i = 0; i < len; ++i ){
  		MString Option = optionList[i];

	

		// if we recognised option 1
		if( Option == "vertexcolorFlag" ) {
  			// check for true or false
			if(optionList[++i]=="0")
				Flags.vertex_colors=0;
			else
  				Flags.vertex_colors=1;

		}

		// if we recognised our second option
		if( Option == "vertexnormalFlag" ) {
  		// check for true or false
			if(optionList[++i]=="0")
				Flags.Normals=0;
			else
  				Flags.Normals=1;

		}



		// if we recognised our third option
		if( Option == "brushFX" ) {
  			// check for true or false
			if(optionList[++i]=="0")
				Flags.use_vertex_colors=2;
			else
  				Flags.use_vertex_colors=0;

		}
		
	}

	//----------------------------------fin------------------------


		//------------------ouverture du fichier------------------------
	// ouverture en écriture du fichier d'export
	// Get the output filename
	MString output_filename = file.fullName();

	::output.open(output_filename.asChar(),ios::out |ios::binary);

	//---------------------------------------------------------------


	::output << "BB3D"; // mise en place du Header fichier B3D
	pos_file=::output.tellp();// sauv garde de la position du début de fichier
	write_int(1);// mise en place d'une valeur entière quelconque pour mise en place ultérieure de la longeur du fichier
	
	write_int(1);// écriture de la version du format B3D


//********************exportation des matériaux***********
OutputMaterials();

// textures début
::output << "TEXS";
int Temp_positionFile = ::output.tellp();
//ecrire_entier(B3DVersion);;//mise en place valeur entière quelconque pour placer la taille du fichier plus tard.
write_int(1);

//nom fichier texture
::output << "./texture512.jpg";



::output << char(0x00);// pour fin de chaîne de caractère
//MFnDependencyNode fnDependNode( material );
// par défaut une seule texture

//flags (optional) - texture flag:
//1: Color
//2: Alpha
//4: Masked
//8: Mipmapped
//16: Clamp U
//32: Clamp V
//64: Spherical reflection map

//blend - blend mode of texture
//0: disable texture
//1: alpha
//2: multiply (default)
//3: add
int tex_flag=1,tex_blend=2;
float tex_x=0,tex_y=0,tex_xscale=1,tex_yscale=1,tex_rotation=0;

write_int(tex_flag);
write_int(tex_blend);
write_float(tex_x);
write_float(tex_y);
write_float(tex_xscale);
write_float(tex_yscale);
write_float(tex_rotation);


EndChunck(Temp_positionFile);

////textures fin


::output << "BRUS";
Temp_positionFile = ::output.tellp();
write_int(1);//mise en place valeur entière quelconque pour placer la taille du fichier plus tard.



write_int(1);

::output << "material01";
::output << char(0x00);

//red
write_float(1);

//green
write_float(1);

//blue
write_float(1);

//alpha
write_float(1);

//shininess
write_float(0);





//int data=0;
//::output.write((char *)&data,sizeof(data));
//write_int(1);

// Brush Blend
//brush - brush handle
//blend -
//1: alpha
//2: multiply (default)
//3: add
write_int(0);


//blend FX
//brush - brush handle
//fx -
//1: full-bright
//2: use vertex colors instead of brush color +shininess
//3:just vertex color no shininess
//4: flatshaded
//8: disable fog  
int flag=2;
//if (Flags.use_vertex_colors==2) flag=2; else flag=1;// par défaut 0
		


										write_int(flag);// écriture uniquement des vertex

write_int(0);
										//int texture_id
										//write_int(0);
										EndChunck(Temp_positionFile);

//*********************************************

	//**************************création du ROOT NODE


	::output << "NODE";  //
	pos_first_node=::output.tellp();// retient la position pour fermeture ultérieure
	write_int(1);// mise en place d'une valeur quelconque
	::output << "Scene Root";//nom du Node

	//écriture des coordonnées spaciales
	//transaltion
	write_float(0);// translation x
	write_float(0);// translation y
	write_float(0);// translation z
	/*
	write_float(1);//scale x
	write_float(1);//scale y
	write_float(1);//scale z
	write_float(0);// rotation  x
	write_float(0);// rotation  y
	write_float(0);// rotation  z
	write_float(0);// rotation  w
/**/

	// scale X 
::output << char(0x00);//
::output << char(0x00);//-------
::output << char(0x00);//   SX
::output << char(0x80);//
// scale Y
::output << char(0x3f);//
::output << char(0x00);//------
::output << char(0x00);//SY
::output << char(0x80);//
// scale Z
::output << char(0x3f);//
::output << char(0x00);//-------
::output << char(0x00);//SZ
::output << char(0x80);//
//rot 1
::output << char(0x3f);//
::output << char(0x00);//-------
::output << char(0x00);//R1
::output << char(0x80);//
//rot 2
::output << char(0x3f);//
::output << char(0x00);//-------
::output << char(0x00);//R2
::output << char(0x00);//
//rot 3
::output << char(0x00);//
::output << char(0x00);//-------
::output << char(0x00);//R3
::output << char(0x00);//
////rot 4
::output << char(0x00);//
::output << char(0x00);//-------
::output << char(0x00);//R4
::output << char(0x00);//
::output << char(0x00);//
/**/

	// export selected objects only
	if(mode == kExportActiveAccessMode) {
/*
		//création de la liste de sélection
		MSelectionList selList;

		// get a list of the selected items
		MGlobal::getActiveSelectionList( selList );
	
		// we will use an iterator this time to walk over the selection list.
		MItSelectionList NodeTransform( selList, MFn::kTransform ); // sélection de tous les nodes contenant une information de position dans l'espace
  	

		//itération à travers tous les transforms sélectionnés
		while( !NodeTransform.isDone() ) {

			MGlobal::displayInfo("kTransform Trouvé");
			

			NodeTransform.findMObject

			NodeTransform.next();
		}
*/

		MSelectionList selection;
		MGlobal::getActiveSelectionList( selection );
		MStringArray strings;
		MDagPath dagPath;

		int start_node[100];
		int pos_objet=0,pos_nouvel_objet;

		for ( unsigned i=0;i!=selection.length(); i++)
		{
			//MGlobal::displayInfo("kTransform Trouvé");
			



			MObject components;
			MDagPath path;
			
			//selection.getDagPath(i,object,components);
			selection.getDagPath(i,path,components);
			

			
			


			MStringArray chemin_split;
			MString chemin=path.fullPathName();
			//MGlobal::displayInfo(chemin);
			chemin.split((char)'|',chemin_split);
			pos_nouvel_objet=chemin_split.length();
			
			//chemin = chemin_split[pos_nouvel_objet-1];
			//MGlobal::displayInfo(chemin);

			if (pos_objet==pos_nouvel_objet || pos_nouvel_objet<pos_objet)
			{
				
				

				if(pos_objet==pos_nouvel_objet)
						{
							MGlobal::displayInfo("fermeture des nodes précédents  2==");
							EndChunck(start_node[pos_objet]);
							//EndChunck(start_node[hierarchie]);
							//if (hierarchie!=0)hierarchie--;
				}else{

				for (int i=pos_objet;i>pos_nouvel_objet;i--)
				{
					
		
						if (start_node[i]>0)EndChunck(start_node[i]);MGlobal::displayInfo("fermeture des nodes précédents 1>");
					//EndChunck(start_node[hierarchie]);
					//if (hierarchie!=0)
					//	hierarchie--;
			
				}
				
					if (start_node[pos_nouvel_objet]>0)EndChunck(start_node[pos_nouvel_objet]);MGlobal::displayInfo("fermeture des nodes précédents 3");
					//EndChunck(start_node[hierarchie]);
					//hierarchie--;
				}


			}
		



			//MFnTransform trans(path);
			MFnTransform trans(path);	
		



			//MGlobal::displayInfo("Creation NODE");
			
			hierarchie++;

			::output<<"NODE";
			start_node[pos_nouvel_objet]=::output.tellp();
			//start_node[hierarchie]=::output.tellp();
			
			write_int(1);// mise en place d'une valeur quelconque
			
			::output << chemin_split[pos_nouvel_objet-1];
			
			//"Scene Root";//nom du Node
			

::output << char(0x00);
				// attach the function set to the object


MVector Translation;
				// get the transforms local translation
		
			Translation = trans.getTranslation(MSpace::kTransform);
		
			
			float temp=(float)Translation.x;
			//écriture des coordonnées spaciales
			//transaltion
			write_float(temp);// translation x
			temp=(float)Translation.y;
			write_float(temp);// translation y
			temp=-(float)Translation.z;
			write_float(temp);// translation z
			
				
			double scale[3];
			trans.getScale(scale);
			temp=(float)scale[0];
			write_float(temp);
			temp=(float)scale[1];
			write_float(temp);
			temp=(float)scale[2];
			write_float(temp);

			
			MQuaternion Rotation;
			
			trans.getRotation(Rotation,MSpace::kTransform);



			temp=(float)Rotation.w;
			write_float(temp);
			temp=(float)Rotation.x;
			write_float(temp);
			temp=(float)Rotation.y;
			write_float(temp);
			temp=(float)Rotation.z; 
//			temp=1;
			write_float(temp);



			
        
		  if (!path.child(0).isNull()) 
		  {

			    switch (path.child(0).apiType()) 
				{
					case MFn::kMesh: {
										MPointArray vertexArray;// coordonnées des point format double x,y,z;
										MIntArray vertexList;// stockage des indexs des points pour les triangles
										MVector Normal;//stockage d'une normal d'un vertex

										//MGlobal::displayInfo("kMesh Trouvé");
										//ecriture MESH
										::output<<"MESH";
										pos_mesh=::output.tellp();
										write_int(1);
										//master brush
										write_int(0xffffffff);
										//::output << char(0xff);// brush id


										MFnMesh meshFn(path.child(0)); // crée une fonction pour le mesh
										
										MItMeshVertex polyperVertex(path, MObject::kNullObj);// crée une fonction pour le mesh , mais avec les fonctions de itmesh

										//récupération des coordonnées des points
										//obtient les coordonnées des vertex en mode global
										//meshFn.getPoints(vertexArray,MSpace::kObject);
										meshFn.getPoints(vertexArray,MSpace::kTransform);






										//ecriture VRTS
										::output<<"VRTS";
										pos_vrts=::output.tellp();
										write_int(1);

										//flags 0=none just vertices coords 1=normal values present, 2=rgba values present
										//The way the flags work, is that you combine them.
										//1 = Vertex Normal
										//2 = Vertex Color
										//3 = Vertex Normal + Color
										MColorArray color;
										MStatus status;
										status = meshFn.getVertexColors(color,0);
										int vertex_colors_present=1;
										//if (status != MS::kSuccess) {
										//	MGlobal::displayInfo("vertex color found");
										//	vertex_colors_present=1;
										//}

										int flag=0; // par défaut 0
		
										if (Flags.Normals == 1){
											flag=1;//  si demande export normales alors osition du flag à 1
											if (Flags.use_vertex_colors == 1 && vertex_colors_present == 1){
												flag=3;
												MGlobal::displayInfo("flag 3");
											}
										}else{
											flag=0;
											if (Flags.use_vertex_colors == 1 && vertex_colors_present == 1){
												flag=2;MGlobal::displayInfo("flag 2");
											}
										}
										//flag=0;
										write_int(flag);// écriture uniquement des vertex
										Flags.Normals=flag;
										//write_int(0);


										//int tex_coord_sets          ;texture coords per vertex (eg: 1 for simple U/V) max=8
										//  int tex_coord_set_size      ;components per set (eg: 2 for simple U/V) max=4


										//tex_coord_sets
										int tex_coords_sets=1;
										write_int(tex_coords_sets);


										//tex_coord_set_size
										int tex_coord_set_size=2;
										write_int(tex_coord_set_size);




										float x,y,z,normx,normy,normz;
										for (unsigned int i=0;i<vertexArray.length();i++){


											x =float(vertexArray[i].x); // -  pour replacer l'axe X dans le sens de celui de blitz
											y =float(vertexArray[i].y);
											z =-float(vertexArray[i].z);// -



											//vertices coords

											write_float(x);
											write_float(y);
											write_float(z);
											

											

											//récupère la normale du point
											meshFn.getVertexNormal(i, Normal ,MSpace::kObject);
											normx=float(Normal.x);
											normy=float(Normal.y);
											normz=float(Normal.z);

											if (Flags.Normals==1 || Flags.Normals == 3){
											//if (flag==1 || flag == 3){
											write_float(normx);
											write_float(normy);
											write_float(normz);
											}
											
						
											//-----------------------------------------
											//vertex_colors_present=1;
											
											if (Flags.Normals == 2 || Flags.Normals == 3){
											
											
											MStringArray colorsets;
											
											status = meshFn.getColorSetNames(colorsets);
										
												   
												MColor couleur;
												MString colorset = colorsets[0];
												//récupère la couleur moyenne des faces connectés au point
												meshFn.getVertexColors(color,&colorset);

												
												
												
												//polyperVertex.getColor
												//int a;
												//meshFn.getColor(a,couleur);
												//meshFn.getColors(color);
												couleur=color[i];
												float col=float(couleur.r);
												//R
												::output.write((char*)&couleur.r,sizeof(couleur.r));
												//write_float(col);
												col=float(couleur.g);
												//G
												::output.write((char*)&couleur.g,sizeof(couleur.g));
												//write_float(col);
												col=float(couleur.b);
												//B
												::output.write((char*)&couleur.b,sizeof(couleur.b));
												//write_float(col);
												col=float(couleur.a);
												//Alpha
												::output.write((char*)&couleur.a,sizeof(couleur.a));
												//write_float(col);
											
											
											
											}
											//-----------------------------------------

											// coordonées de textures UV

float2 uvpoint;

//MFloatArray UsPoint;
//MFloatArray VsPoint;
//MIntArray faceIds;

//polyperVertex.getUV(uvpoint);

											MStringArray uvsetnames;
											meshFn.getUVSetNames(uvsetnames);								
MPoint monpoint;
											meshFn.getPoint(i,monpoint,MSpace::kObject);
											meshFn.getUVAtPoint(monpoint,uvpoint,MSpace::kObject);




//polyperVertex.getUVs(UsPoint,VsPoint,faceIds);




float tempo;

tempo = uvpoint[0];
write_float(tempo);
//::output.write((char *)&tempo,sizeof(tempo));
tempo = -uvpoint[1];
write_float(tempo);
//::output.write((char *)&tempo,sizeof(tempo));






								




										}

										EndChunck(pos_vrts);

										//ecriture TRIS
										::output<<"TRIS";
										pos_tri=::output.tellp();
										write_int(1);

										//brush ID
										write_int(0);




										//MItMeshPolygon  itPolygon( path, MObject::kNullObj );
										MItMeshPolygon  itPolygon(path.child(0));

	

										for ( /* nothing */; !itPolygon.isDone(); itPolygon.next() )
										{		
					
	

											// Get triangulation of this poly.
											int  numTriangles; 
											itPolygon.numTriangles(numTriangles);

											while ( numTriangles-- )
											{
												//MGlobal::displayInfo("  triangle");
												MStatus status;

												MIntArray                           polygonVertices;

												itPolygon.getVertices( polygonVertices );

												MPointArray                     nonTweaked;
												// object-relative vertex indices for each triangle
												MIntArray                       triangleVertices;
												// face-relative vertex indices for each triangle
												MIntArray                       localIndex;

												status = itPolygon.getTriangle( numTriangles,
												nonTweaked,
												triangleVertices,
												MSpace::kObject );
		
												if ( status == MS::kSuccess )
												{



													//traitement du triangle
			
													// Get face-relative vertex indices for this triangle

			
													//int temp=triangleVertices[0];
						
						
													write_int(triangleVertices[0]);
													write_int(triangleVertices[2]);
													write_int(triangleVertices[1]);
													//::output.write((char*)&triangleVertices[0],sizeof(triangleVertices[0]));
													//::output.write((char*)&triangleVertices[2],sizeof(triangleVertices[2]));
													//::output.write((char*)&triangleVertices[1],sizeof(triangleVertices[1]));
			
			

												} // fin if	


											};// fin while



									}; //fin for

										EndChunck(pos_tri);

										EndChunck(pos_mesh);


									 }
					break;
				default: break;
				}
		  }
pos_final_nodes=pos_objet;	
pos_objet=pos_nouvel_objet;
		}








				//for (int i=hierarchie; i>0;i--){
					for (int i=pos_nouvel_objet; i>0;i--){
						if (start_node[i]>0)	{
						MGlobal::displayInfo("fermeture finale des nodes précédents");
						info = "obj :";
						info += i;
						info += " pos : ";
						info += start_node[i];
						MGlobal::displayInfo(info);

					EndChunck(start_node[i]);
						//EndChunck(start_node[hierarchie]);
						//hierarchie--;
					}
			
				}
				/*if (start_node[0]>0)	{
						info = "obj :";
						info += 0;
						info += " pos : ";
						info += start_node[0];
						MGlobal::displayInfo(info);
					
						EndChunck(start_node[0]);
					}*/

	}
	else
	//export all scene objects
	{
	
	
	}
	
hierarchie=0;
EndChunck(pos_first_node);//fermeture de la longueur du premier node
EndChunck(pos_file);//écriture de la longueur du fichier
output.close();

	return MS::kSuccess;
}
