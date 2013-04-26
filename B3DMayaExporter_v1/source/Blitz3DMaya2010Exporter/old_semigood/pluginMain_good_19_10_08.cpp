//
// Copyright (C) NekoTech 
// 
// File: pluginMain.cpp
//
// Author: Maya Plug-in Wizard 2.0
//
// test pour voir si la révision s'opère



#include <Blitz3Dexporteur.h>
#include <pluginFunctions.h>


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


struct Flags_export{

	int default_brush;
	int vertex_colors;
	int use_vertex_colors;
	int Normals;

};


Flags_export Flags;











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
		//remise à zero des données globales nécessaires
		Matid.clear();
		Texid.clear();
		nb_Tex_by_Brush.clear();
		Texids_by_brush.clear();
		brushid_by_tex.clear();
		

		//liste des objets sélectionnés
		MSelectionList selection;
		MGlobal::getActiveSelectionList( selection );


		

		scan_textures_used(selection);// place les index de tous les materiaux et textures


	
		// ouverture du fichier [BB3D]
		MString output_filename = file.fullName();
		::output.open(output_filename.asChar(),ios::out |ios::binary);
		// ecriture header fichier
		
		::output << "BB3D"; // mise en place du Header fichier B3D
		StartChunck();

	
		
		write_int(1);//écriture de la version BB3D


		Matid.clear();
		//Texid.clear();
		nb_Tex_by_Brush.clear();
		Texids_by_brush.clear();
		brushid_by_tex.clear();



#ifdef EXPORT_TEXTURES
		
		::output << "TEXS";//header Brush
		StartChunck();
		//écriture les textures s'il y en a
		if (Texid.length()>0){
		//Affich("enregistre textures");

		
		// écriture des textures si présentent [TEXS]|detection des textures à enregistrer| + fermeture texs


		Texid.clear();


		//écriture textures ok
		OutputTex(selection);

	

		}
	EndChunck();// fermeture textures
#else
#endif


#ifdef EXPORT_BRUSHES
		// A FAIRE ecritures des BRUSHS
#ifdef OLD_BRUSH

#else
// écriture des textures si présentent [TEXS]|detection des textures à enregistrer| + fermeture texs
	::output << "BRUS";//header Brush
	StartChunck();
	write_int(nbMaxb3dTextures);//écriture du nombre maxi de textures supportées par blitz3D
#endif
		

	

		Matid.clear();
		//Texid.clear();
		//nb_Tex_by_Brush.clear();
		Texids_by_brush.clear();
		//brushid_by_tex.clear();

		getBrushes(selection);


#ifdef OLD_BRUSH

#else
	EndChunck();
#endif
	

#else
#endif

#ifdef SCENE_ROOT
		//début node principal
		::output << "NODE"; // mise en place du Header fichier B3D
		StartChunck();
		

		//*********Scene root***********
			
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

#endif



		//***************fin scene root

		//****************************écriture des nodes de hiérarchie et des triangles

		int pos_objet=0,pos_nouvel_objet;
		MString nom_objet_precedent;


		int position_hierarchie=0;
		int pos=0;
		MItDag it(MItDag::kDepthFirst, MFn::kTransform);

		while(!it.isDone())
		{
				MDagPath path;
				it.getPath(path);
				MFnTransform trans(path);	
				MObject obj=it.item();

				pos=getPosObj(path);

	
			
		



				if(obj.apiType()== MFn::kTransform && path.child(0).apiType()== MFn::kMesh){
						//MGlobal::displayInfo("Transform trouvé avec child Kmesh");
						//écriture du node avec transform
						//incrément de la hierarchie
#ifdef EXPORT_NODES
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

						MStringArray chemin_split;
						MString chemin=path.fullPathName();

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

		

#endif
							
		
			


						path.getPath(path);
						if(selection.hasItem(path)!=MStatus::kSuccess){
														//export MESH
							//---------exportation du mesh
#ifdef EXPORT_MESH
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
//-----A FAIRE        définir le premier matériel rencontré-----------------------*********
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

							/*::output << "VTRS"; // mise en place du Header mesh
							StartChunck();

							EndChunck();//fin VRTS
*/
#ifdef EXPORT_VRTS
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


							MStringArray colorsets;
							MColorArray color;
							//status = meshFn.getColorSetNames(colorsets);
							meshFn.getColorSetNames(colorsets);
							if(colorsets.length()==0){
							Flags.vertex_colors=0;
							
							}else{
								if(Flags.use_vertex_colors!=0){
									Flags.vertex_colors=1;
								
								}
							}



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
									}//fin if
											


								//-----------------------------------------
								//vertex_colors_present=1;
											
								if (flag_normal_colors == 2 || flag_normal_colors==3){
																

										
											   
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
									}//fin if
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
#endif

/*
							::output << "TRIS"; // mise en place du Header mesh
							StartChunck();

							EndChunck();//fin VRTS


*/


#ifdef EXPORT_TRIS

#ifdef OLD_TRIS
						//ecriture TRIS
						::output<<"TRIS";
						StartChunck();
//-----------------A FAIRE   détection des brush---------------------------------------------------
										//brush ID
										write_int(0);//write_int(0);




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
						int instancenumber=path.instanceNumber();
						MObjectArray shaders;
						MIntArray indices;
						meshFn.getConnectedShaders(instancenumber,shaders,indices);

						affich = "shaders length ";
						affich += shaders.length();
						Affich(affich);

						for(unsigned int i=0;i<shaders.length();i++){//navigation dans les shaders connectés

							//*********************************************************************************************

								//___________ouput tris________
	
		affich = "shader ";
		affich += i;
	Affich(affich);

							//ecriture TRIS
						::output<<"TRIS";
						StartChunck();
						


	//trouver le brush id par rapport à matid
	//recup nom shader et compare à matid
	MString nameshader;
	nameshader=GetShaderName(shaders[i]);

	affich = "Matid id lenght";
	affich += Matid.length();
	Affich(affich);

int BrushId=0;
	for (unsigned int b=0;b<Matid.length();b++){
		if (nameshader==Matid[b]){
		BrushId=b;
		}	
	}

							//brush ID
						write_int(BrushId);
						//write_int(-1);//default
			affich= "BrushId ";
		affich += BrushId;
	Affich(affich);

	affich = Matid[BrushId];
	Affich(affich);

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
					
	for (unsigned int i=0;i<indices.length();i++){
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

							//********************************************************************************************
						
						
						}//fin for navigation dans les shaders connectés
						


#endif

#endif


							EndChunck();//fin mesh
		
#endif						
						
						}//fin if
						//fin écriture du contenu du node
						//------------------------------------------------------------
						position_hierarchie=pos;
				}

				//node suivant
				it.next();
			}//fin while boucle principale

			//fermeture dernier node //scene root?
			EndChunck();

		
		
		
		
		
	


		

		
		//****************************fin écriture des nodes et des mesh
#ifdef SCENE_ROOT
		EndChunck();//fin node scene root principal
#endif
		//EndChunck();//  à déterminer
		EndChunck();// écriture fin de fichier


		//fermeture de tous les chuncks restants
		affich = "posfichier restants ";
		affich+= posfichier.length();
		Affich(affich);

		for(int i=posfichier.length();i>0;i--){
			EndChunck();
		
		}

		//fermeture du fichier
		output.close();
		Matid.clear();
		Texid.clear();
		nb_Tex_by_Brush.clear();
		Texids_by_brush.clear();
		brushid_by_tex.clear();
	}
	else		
		//export all polygonal scene objects
	{
		
	
	}
	


	return MS::kSuccess;
}
