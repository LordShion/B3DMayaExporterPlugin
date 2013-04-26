//
// Copyright (C) NekoTech 
// 
// File: pluginMain.cpp
//
// Author: Maya Plug-in Wizard 2.0
//
// test pour voir si la révision s'opère

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

//----------Mes variables--------------------
//int TRUE;
//int FALSE;
MString info;
int hierarchie=0;



struct Flags_export{

	int default_brush;
	int vertex_colors;
	int use_vertex_colors;
	int Normals;

};


Flags_export Flags;


//---gestion fichier---------
fstream output;
MIntArray posfichier;

//gestion matériaux
MStringArray Matid;
MStringArray Texid;

MIntArray nb_Tex_by_Brush,Texids_by_brush;




// -------------fin mes variables---------------

//------------Mes Fonctions--------------
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

void StartChunck(){
	int tempo;
	int temp = ::output.tellp();
	posfichier.append(temp);
		
		info = " start position: ";
		tempo = posfichier.length()-1;
		info += tempo;
	
		
		
		info += " rec adress : ";
		info += temp;
		//MGlobal::displayInfo(info);

		write_int(1); // écriture d'une valeur quelconque
}

void EndChunck() // fonction pour écrire une longueur par rapport à position actuelle dans le fichier
{

	


		int temp = posfichier.length()-1;
		info = "end position : ";
		info += temp;
		
		
				info += " address : ";
		info += posfichier[temp];

	

		//EndChunck(posfichier[temp-1]);
			int pos_new = ::output.tellp();//retient la position actuelle
	int size = pos_new - posfichier[temp];// détermine la longueur
	size -= 4 ; // pour les 4 premiers octets
	::output.seekp(posfichier[temp]);// se place à la position indiqué
	write_int(size);// écrit la longueur
	::output.seekp(pos_new);// se repositionne à la position transmise
		
		info += " taille : ";
		info += size;

		//MGlobal::displayInfo(info);
		
		
		posfichier.remove(temp);

}

MString GetShaderName(MObject shadingEngine)
{
	// attach a function set to the shading engine
	MFnDependencyNode fn( shadingEngine );

	// get access to the surfaceShader attribute. This will be connected to
	// lambert , phong nodes etc.
	MPlug sshader = fn.findPlug("surfaceShader");

	// will hold the connections to the surfaceShader attribute
	MPlugArray materials;

	// get the material connected to the surface shader
	sshader.connectedTo(materials,true,false);

	// if we found a material
	if(materials.length())
	{
		MFnDependencyNode fnMat(materials[0].node());
		return fnMat.name();
	}
	return "none";
}




void Affich(MString affich){
	
	MGlobal::displayInfo(affich);

}




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
p = fn.findPlug(r);
p.getValue(color.r);
p = fn.findPlug(g.asChar());
p.getValue(color.g);
p = fn.findPlug(b.asChar());
p.getValue(color.b);
p = fn.findPlug(a.asChar());
p.getValue(color.a);
p = fn.findPlug(name);

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
	color.r=1;
	color.g=1;
	color.b=1;

}
// output the name, color and texture ID
//cout 	<< "\t" << name << " "
//<< color.r << " "
write_float(color.r);
//info=color.r;
//Affich(info);
//info=color.g;
//Affich(info);
//info=color.b;
//Affich(info);
//info=color.a;
//Affich(info);

//<< color.g << " "
write_float(color.g);
//<< color.b << " "
write_float(color.b);
//<< color.a << " tex= "
write_float(color.a);
//<< texname.asChar() << "\n";

return true;

} 

void OutputColors(MObject shadingEngine,MString tag)
{
	// attach a function set to the shading engine
	MFnDependencyNode fn( shadingEngine );

	// get access to the surfaceShader attribute. This will be connected to
	// lambert , phong nodes etc.
	MPlug sshader = fn.findPlug("surfaceShader");

	// will hold the connections to the surfaceShader attribute
	MPlugArray materials;

	// get the material connected to the surface shader
	sshader.connectedTo(materials,true,false);


		MFnDependencyNode fnMat(materials[0].node());
OutputColor(fnMat,"color");
//OutputColor(fnMat,"Transparency");

}
bool Outputtexture(MFnDependencyNode& fn,const char* name)
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
p = fn.findPlug(r);
p.getValue(color.r);
p = fn.findPlug(g.asChar());
p.getValue(color.g);
p = fn.findPlug(b.asChar());
p.getValue(color.b);
p = fn.findPlug(a.asChar());
p.getValue(color.a);
p = fn.findPlug(name);

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
					

					int TexExists=0;
						for(int i=0;i<Texid.length();i++)
							{
								if(texname==Texid[i]) TexExists=1;
							}
							if(TexExists==0){
							// écrire la texture car absente
								Texid.append(texname);
								Affich(texname);
							
								
								//write_int(Matid.length());//id Brush
								//write_int(1);//textures per brush
								//::output << GetShaderName( shaders[j] ).asChar();
								
								
								
								// get the attribute for the full texture path
MPlug ftn = fnDep.findPlug("ftn");

// get the filename from the attribute
MString filename;
ftn.getValue(filename);

// write the file name
								


nb_Tex_by_Brush[Matid.length()-1]++;// une nouvelle texture dans la brush courante
Texids_by_brush.append(Texid.length()-1);
								::output << filename;
								
								
								
								::output << char(0x00);
								//write flags, blend default 1, 2
								write_int(1);
								write_int(2);
								// write position default O, O
								write_float(0);
								write_float(0);
								//write scale default 1, 1
								write_float(1);
								write_float(1);
								//rotation in radian default= 0
								write_float(0);

								//OutputColors(shaders[j],"color");

								//EndChunck();
							}else {
		nb_Tex_by_Brush[Matid.length()-1]=0;
		//Texids_by_brush.append(0);
				Texids_by_brush.append(0);
			}
					// stop looping
					//TexExists=0;

				}

			
		}



return true;

} 


void writeTexture(MObject shadingEngine,MString tag)
{
	// attach a function set to the shading engine
	MFnDependencyNode fn( shadingEngine );

	// get access to the surfaceShader attribute. This will be connected to
	// lambert , phong nodes etc.
	MPlug sshader = fn.findPlug("surfaceShader");

	// will hold the connections to the surfaceShader attribute
	MPlugArray materials;

	// get the material connected to the surface shader
	sshader.connectedTo(materials,true,false);


		MFnDependencyNode fnMat(materials[0].node());
Outputtexture(fnMat,"color");

}

void OutputTextures(MSelectionList selected){
	
	
MString temp;
MString affich;
	

	
int MatExists=0;
	for(int i=0;i<selected.length();i++){
	

	MDagPath path;
	MObject obj;

	selected.getDagPath(i,path);

	
	obj=path.child(0);

	

	MFnMesh fn(path);
	obj=fn.parent(0);
	path.getAPathTo(fn.parent(0));
	MFnMesh fna(path);
	
	
	unsigned int instancenumbers;
	MObjectArray shaders;
	MIntArray indices;

	fna.getConnectedShaders(instancenumbers,shaders,indices);

	

switch(shaders.length()) {

		// if no shader applied to the mesh instance
		case 0:
			{
				//***************Affich("pas de matériaux");
			}
			break;

		// if all faces use the same material
	
		// if more than one material is used, write out the face indices the materials
		// are applied to.
		default:
			{
				//************************Affich("trouvé plusieurs matériaux");

				
				//write_int(shaders.length());

				// now write each material and the face indices that use them
				
				for(int j=0;j < shaders.length();++j)
				{						
					for(int matest=0;matest<Matid.length();matest++){
						//**************************Affich(Matid[matest].asChar());
						//*****************************Affich(GetShaderName( shaders[j] ).asChar());
						if(Matid[matest]== GetShaderName( shaders[j] )){
						MatExists = 1;
						
						}//fin if matid
						
						}// fin for matest
					if(MatExists != 1){
					//*****************************Affich("matériau absent de la liste, enregistrement");
					Matid.append(GetShaderName( shaders[j] ).asChar());
					nb_Tex_by_Brush.append(0);
						writeTexture(shaders[j],"color");

					//Affich(temp);
					}else {
					//*************************************	Affich("matériau existe dans la liste");
					}// fin if matexists
					
					MatExists = 0;
						

				}//fin for j shaders
			}// fin case default
			break;
		}//fin switches

	}//fin for selected
		

	Matid.clear();
}// fin output textures

void OutputMaterials(MSelectionList selected){
	
	//::output << "BRUS";//header Brush
	//StartChunck();
MString temp;
MString affich;
	

	
int MatExists=0;

	for(int i=0;i<selected.length();i++){
Affich("element selectioné");

	MDagPath path;
	MObject obj;
	
	int index;
	selected.getDagPath(i,path);
	index = path.childCount();
	affich ="childs ";affich+=index;
	Affich(affich);

	selected.getDependNode(index,obj);


	
	
	//obj=path.child(0);//.child(0);

	

	//MFnMesh fn(path);
	//obj=fn.parent(0);
	//path.getPath(path);
	MFnMesh fna(path.child(1));
	
	
	unsigned int instancenumbers;
	MObjectArray shaders;
	MIntArray indices;
	
	fna.getConnectedShaders(instancenumbers,shaders,indices);

	affich = "shaders lenght "; 
	affich += shaders.length();
Affich(affich);

switch(shaders.length()) {

		// if no shader applied to the mesh instance
		case 0:
			{
				//***************Affich("pas de matériaux");
			}
			break;

		// if all faces use the same material
	
		// if more than one material is used, write out the face indices the materials
		// are applied to.
		default:
			{
				//************************Affich("trouvé plusieurs matériaux");

				
				//write_int(shaders.length());

				// now write each material and the face indices that use them
				
				for(int j=0;j < shaders.length();++j)
				{						
					for(int matest=0;matest<Matid.length();matest++){
						//**************************Affich(Matid[matest].asChar());
						//*****************************Affich(GetShaderName( shaders[j] ).asChar());
						if(Matid[matest]== GetShaderName( shaders[j] )){
						MatExists = 1;
						
						}//fin if matid
						
						}// fin for matest
					if(MatExists != 1){
					//*****************************Affich("matériau absent de la liste, enregistrement");
					Matid.append(GetShaderName( shaders[j] ).asChar());
					::output << "BRUS";//header Brush
						StartChunck();
							//write_int(Matid.length());//id Brush
						//écrire le nb de textures
						int a=nb_Tex_by_Brush[Matid.length()-1];
						if (a==0) a=1;
						info="nb de textures pour ce brush ";
						info+=nb_Tex_by_Brush[Matid.length()-1];
						Affich(info);

							write_int(a);//nb textures in brush
	
						::output << GetShaderName( shaders[j] ).asChar();
						::output << char(0x00);
						
						
					OutputColors(shaders[j],"color");


						//red
						//write_float(0);
						//green
						//write_float(1);
						//blue
						//write_float(0);
						//alpha
						//write_float(1);
						//shininess
						write_float(0);
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
						int flag=0;
						if (Flags.use_vertex_colors==1 && Flags.vertex_colors==1) flag=3; // par défaut 0
						write_int(flag);// écriture uniquement des vertex
						
						//écrire les id textures
						
						
						if (nb_Tex_by_Brush[Matid.length()-1]==0){
							a=-1;
						}else{
						a=Texids_by_brush[Matid.length()-1];
						}
						info="texid ";
						info+=a;
						Affich(info);
						write_int(a);//int texture_id   -1  no texture for this brush
						
						EndChunck();
					//Affich(temp);
					}else {
					//*************************************	Affich("matériau existe dans la liste");
					}// fin if matexists
					
					MatExists = 0;
						

				}//fin for j shaders
			}// fin case default
			break;
		}//fin switches

	}//fin for selected
		


}// fin output materials

//--------fin mes fonctions--------------

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
				Flags.use_vertex_colors=1;
			else
  				Flags.use_vertex_colors=0;

		}
		
	}

	//----------------------------------fin------------------------

	//export Selected Objects
    if(mode == kExportActiveAccessMode) 
	{
		//liste des objets sélectionnés
		MSelectionList selection;
		MGlobal::getActiveSelectionList( selection );
		
		//MStringArray strings;
		//MDagPath dagPath;

			MObject components;
			MDagPath path;

		//-----

//		int temp;
		//MGlobal::displayInfo("Début exportation des objets sélectionnés");
		// ouverture du fichier [BB3D]
		MString output_filename = file.fullName();
		::output.open(output_filename.asChar(),ios::out |ios::binary);
		// ecriture header fichier
		
		::output << "BB3D"; // mise en place du Header fichier B3D
		StartChunck();

		// sauv garde de la position du début de fichier
		//write_int(1);// mise en place d'une valeur entière quelconque pour mise en place ultérieure de la longeur du fichier
		write_int(1);//écriture de la version BB3D
	

#ifdef OLD_TEXS
		// écriture des textures si présentent [TEXS]|detection des textures à enregistrer| + fermeture texs
::output << "TEXS";//header Brush
								StartChunck();
		OutputTextures(selection);
EndChunck();

		// écriture des matériaux [BRUS]|detection des materiaux à enregistrer| +fermeture brus
	
		
//Matid.clear();
		
OutputMaterials(selection);

#else

		//nouvelles textures

#endif	
		
	//------------------------------------fin materials----------------------------------


#ifdef SCENE_ROOT
		// algo des nodes [NODE] réplication de la hierarchie, 
		::output << "NODE"; // mise en place du Header fichier B3D
		StartChunck();
		

		//write_int(1);// mise en place d'une valeur quelconque
		::output << "Scene Root";//nom du Node
		::output << char(0x00);//fin de chaîne

		//écriture des coordonnées spatiales
		//transaltion
		write_float(0);// translation x
		write_float(0);// translation y
		write_float(0);// translation z
		
		write_float(1);//scale x
		write_float(1);//scale y
		write_float(1);//scale z
		
		write_float(0);// rotation  x
		write_float(0);// rotation  y
		write_float(0);// rotation  z
		write_float(0);// rotation  w
		
#else
#endif
		



		int pos_objet=0,pos_nouvel_objet;
		MString nom_objet_precedent;


 	

		// create an iterator to go through all transforms
		//MItDag it(MItDag::depth, MFn::kTransform); 
		MItDag it(MItDag::kDepthFirst, MFn::kTransform);
		// keep looping until done
		int position_hierarchie=0;
		int pos=0;
		while(!it.isDone())
		{
  				MString temp;
				MDagPath path;
				it.getPath(path);
				MFnTransform trans(path);	
				MObject obj=it.item();

	
			
		

				MStringArray chemin_split;
			MString chemin=path.fullPathName();

		
			chemin.split((char)'|',chemin_split);

			pos=chemin_split.length();
			//temp=pos;

			//MGlobal::displayInfo(temp);
		
			if(obj.apiType()== MFn::kTransform && path.child(0).apiType()== MFn::kMesh){
						//MGlobal::displayInfo("Transform trouvé avec child Kmesh");
						//écriture du node avec transform
						//incrément de la hierarchie
						if(pos<position_hierarchie || pos==position_hierarchie){
							//MGlobal::displayInfo("fermeture des nodes précédents");
							for(int i=position_hierarchie;i>pos-1;i--){
							//temp = i;
							//MGlobal::displayInfo(temp);

							EndChunck();
							}//fin for
						
						}//fin if
							
								//MGlobal::displayInfo("Ouverture node");
								::output << "NODE"; // mise en place du Header node
								StartChunck();
								MString nom_objet;
								nom_objet=chemin_split[chemin_split.length()-1];
								
							
									::output << nom_objet.substring(0,nom_objet.length());//nom du Node
								
									::output << char(0x00);//caractêre de fin de chaîne


									
			

			
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
			write_float(temp);

		


							
		
			


						path.getPath(path);
						if(selection.hasItem(path)!=MStatus::kSuccess){
							//MGlobal::displayInfo("présent dans la liste de selection");
							
							//---------exportation du mesh

						//mais avec des nodes vides, pour les mesh non sélectionnés
					//----------------------------ecriture mesh si objet polygonal présent
						::output << "MESH"; // mise en place du Header mesh
						StartChunck();
										MPointArray vertexArray;// coordonnées des point format double x,y,z;
										MIntArray vertexList;// stockage des indexs des points pour les triangles
										MVector Normal;//stockage d'une normal d'un vertex
		
						//----------------------coordonnées Vertexs  (normal & color si présent et demandés)
						//master brush
						//write_int(0xffffffff);
						write_int(-1);//-1 master brush
						
						MFnMesh meshFn(path.child(0)); // crée une fonction pour le mesh
										
						MItMeshVertex polyperVertex(path, MObject::kNullObj);// crée une fonction pour le mesh , mais avec les fonctions de itmesh
						
						//récupération des coordonnées des points
						//obtient les coordonnées des vertex en mode global
						//meshFn.getPoints(vertexArray,MSpace::kObject);
						meshFn.getPoints(vertexArray,MSpace::kTransform);

						//MFloatArray uArray;
						//MFloatArray vArray;

						//meshFn.getUVs(uArray,vArray);//getUVs( MFloatArray& uArray, MFloatArray& vArray,const MString * uvSet = NULL )
						
						MIntArray uvCounts,uvIds;

						meshFn.getAssignedUVs(uvCounts,uvIds,0);



						//ecriture VRTS
						::output<<"VRTS";
						StartChunck();

										//flags 0=none just vertices coords 1=normal values present, 2=rgba values present
										//The way the flags work, is that you combine them.
										//1 = Vertex Normal
										//2 = Vertex Color
										//3 = Vertex Normal + Color
int flag_normal_colors=0;
//info = "Normals ";
//info += Flags.Normals;
//Affich(info);
//info = "vertex colors ";
//info += Flags.vertex_colors;
//Affich(info);
flag_normal_colors = Flags.Normals+((Flags.use_vertex_colors && Flags.vertex_colors)*2);
//info = flag_normal_colors;
//Affich(info);
						write_int(flag_normal_colors);//présence normale
						
						//int tex_coord_sets          ;texture coords per vertex (eg: 1 for simple U/V) max=8
						write_int(1);//uv simple
						//  int tex_coord_set_size      ;components per set (eg: 2 for simple U/V) max=4
						write_int(2);//2 coordonées textures






										
										




						float x,y,z,normx,normy,normz;//,normx,normy,normz;
						for (unsigned int i=0;i<vertexArray.length();i++){


							x =float(vertexArray[i].x); // -  pour replacer l'axe X dans le sens de celui de blitz
							y =float(vertexArray[i].y);
							z =-float(vertexArray[i].z);// -



							//vertices coords

							write_float(x);
							write_float(y);
							write_float(z);

							//récupère la normale du point
							if(flag_normal_colors==1 || flag_normal_colors==3){
											meshFn.getVertexNormal(i, Normal ,MSpace::kObject);
											normx=float(Normal.x);
											normy=float(Normal.y);
											normz=float(Normal.z);

											
											
											write_float(normx);
											write_float(normy);
											write_float(normz);
							}
											


							//-----------------------------------------
											//vertex_colors_present=1;
											
											if (flag_normal_colors == 2 || flag_normal_colors==3){
											
											
											MStringArray colorsets;
											MColorArray color;
											
											//status = meshFn.getColorSetNames(colorsets);
											meshFn.getColorSetNames(colorsets);
										
												   
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
											

//float tempo;
float u,v;

MFloatArray uArray;
MFloatArray vArray;
MIntArray FaceIds;

polyperVertex.getUVs(uArray,vArray,FaceIds);

//meshFn.getUV(i*2,u,v);

u=uArray[0];
v=vArray[0];



//tempo = uArray[0];
//write_float(tempo);

//tempo = vArray[0];
//write_float(tempo);

write_float(u);
write_float(-v);


polyperVertex.next();













						}//fin for

	

						//-----------------fermeture coordonées Vertex
						EndChunck();
						// ----------------------------------export des triangles


#ifdef OLD_TRIS

							//ecriture TRIS
						::output<<"TRIS";
						StartChunck();

										//brush ID
										write_int(-1);//write_int(0);




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

										EndChunck();

#else

			

	unsigned int instancenumbers;
	MObjectArray shaders;
	MIntArray indices;
	//MFnMesh		Fn(path.instanceNumber);
	meshFn.getConnectedShaders(instancenumbers,shaders,indices);
	MString info="shaders.lenght ";
	info += shaders.length();
	Affich(info);

	for (int i=-1;i<shaders.length();i++){//création de tris en fonction du nombre de brush appliqué
		//___________ouput tris________
	
		info = "shader ";
		info += i;
	Affich(info);

							//ecriture TRIS
						::output<<"TRIS";
						StartChunck();
						


	//trouver le brush id par rapport à matid
	//recup nom shader et compare à matid
	MString nameshader;
	nameshader=GetShaderName(shaders[i]);

	info = "Matid id lenght";
	info += Matid.length();
	Affich(info);

int BrushId=0;
	for (int b=0;b<Matid.length();b++){
		if (nameshader==Matid[b]){
		BrushId=b;
		}	
	}

							//brush ID
						write_int(BrushId);
						//write_int(-1);//default
			info= "BrushId ";
		info += BrushId;
	Affich(info);

	info = Matid[BrushId];
	Affich(info);

	MItMeshPolygon  itPolygon(path.child(0));
	int d=0;
	for ( /* nothing */; !itPolygon.isDone(); itPolygon.next() )
		{

			nameshader=GetShaderName(shaders[indices[d]]);
			if(nameshader==Matid[BrushId]){

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



													
						
						
													write_int(triangleVertices[0]);
													write_int(triangleVertices[2]);
													write_int(triangleVertices[1]);
													
			

												} // fin if	


											};// fin while


			}
		d++;

		}
					
	for (int i=0;i<indices.length();i++){
		//info = " indice ";
		//info += indices[i];
		//Affich(info);

		nameshader=GetShaderName(shaders[indices[i]]);
		if(nameshader==Matid[BrushId]){
		//info=nameshader;
		//Affich(info);

//*********************ecrire triangle*******


		}

	}

	
						EndChunck();

	}
	//for (int i=0;i<Matid.length();i++){
	//	info = Matid[i];
	//	Affich(info);
	//}
//Affich("fin objet");

#endif

						


					//---------------------fermeture mesh
						EndChunck();

							//------------------------------------------------------------
						
						}
						position_hierarchie=pos;
				}
				// move on to next node
				it.next();
			 
			
		}//fin while
		

				//fermeture du node
		//fermeture fichier
		//fermeture de tous les nodes ouverts
			//for (int i=posfichier.length();i>0;i--){

#ifdef SCENE_ROOT
				EndChunck();
#else
#endif

				//}
		//écriture de la longueur du fichier

		output.close();
		Matid.clear();
		Texid.clear();
		nb_Tex_by_Brush.clear();
		Texids_by_brush.clear();
	}
	else		
		//export all polygonal scene objects
	{
	}
	


	return MS::kSuccess;
}


