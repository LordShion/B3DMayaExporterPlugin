//
// Copyright (C) NekoTech 
// 
// File: pluginMain.cpp
//
// Author: Maya Plug-in Wizard 2.0
//
// test pour voir si la r�vision s'op�re



#include <Blitz3Dexporteur.h>
#include <Blitz3DShader.h>
#include <pluginFunctions.h>

// required to link the libraries under VisualC++/Win32
/*
#ifdef WIN32
	#pragma comment(lib,"Foundation.lib")
	#pragma comment(lib,"OpenMaya.lib")
	#pragma comment(lib,"OpenMayaFx.lib")
	#pragma comment(lib,"OpenMayaUi.lib")
	#pragma comment(lib,"Image.lib")
	#pragma comment(lib,"OpenMayaAnim.lib")
	#pragma comment(lib,"OpenMayaRender.lib")
#endif
*/
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




void getMatids(MSelectionList selection){
//on checke toute la liste de s�lection
		Matid.clear();
			for(unsigned int i=0;i<selection.length();i++){
					
				MDagPath selectedPath;
				selection.getDagPath(i,selectedPath);
				MFnMesh meshFn(selectedPath);

				unsigned int instancenumbers=0;
				MObjectArray shaders;
				MIntArray indices;

				meshFn.getConnectedShaders(instancenumbers,shaders,indices);

		switch(shaders.length()) {

			case 0:{
					//Affich("pas de mat�riaux associ� � cet objet");
			   
			   }break;

			default:{
				//au moins un mat�riau trouv�
				//chemin du path du mat�riau
				Affich("*******nom des shaders associ�s � l'objet**********");
				for(unsigned int j=0;j < shaders.length();++j)
				{

					
					affich = GetShaderName( shaders[j] ).asChar();
					
					//v�rifie que le mat�riau n"est pas dans la liste des mat�riaux
					int MatExists=0;
					
					for(int b=0;b<Matid.length();b++){
						if(Matid[b]==affich) MatExists++;
					}
					if(MatExists==0){
						Matid.append(affich);
						Affich(affich);
					}

				}//fin for des mat�riaux associ�s � l'objet
					
					}
			}// fin switch case shaders

		}// fin for parcourt liste de selection

}




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

		// a classification for where the node will appear on the create menus
	// of the multilister and hypershade.
	const MString UserClassify( "shader/surface" );

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
  			status.perror("Error - initializePlugin File Translator");

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
  			status.perror("Error - uninitializePlugin Blitz3D File exporter");

		}


	return status;
}

MStatus MayaFileTranslator::writer( 	const MFileObject& file,
const MString& options,
FileAccessMode mode){

	//-------------------d�tection des options transmises par le script MEL---------------
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
				Flags.default_brush=0;
			else
				Flags.default_brush=1;

		}



				// if we recognised our second option
		if( Option == "exportbrushes" ) {
  		// check for true or false
			if(optionList[++i]=="0")
				Flags.export_brushes=0;
			else
				Flags.export_brushes=1;

		}

						// if we recognised our second option
		if( Option == "exporttextures" ) {
  		// check for true or false
			if(optionList[++i]=="0")
				Flags.export_textures=0;
			else
				Flags.export_textures=1;

		}

								// if we recognised our second option
		if( Option == "exportHierarchy" ) {
  		// check for true or false
			if(optionList[++i]=="0")
				Flags.export_hierarchy=0;
			else
				Flags.export_hierarchy=1;

		}

										// if we recognised our second option
		if( Option == "UseRelativePath" ) {
  		// check for true or false
			if(optionList[++i]=="0")
				Flags.use_relative_path=0;
			else
				Flags.use_relative_path=1;

		}
		
		
		
	}

	//----------------------------------fin------------------------

		//export Selected Objects
    if(mode == kExportActiveAccessMode) 
	{








				//remise � zero des donn�es globales n�cessaires
		
		Texid.clear();
		nb_Tex_by_Brush.clear();
		Texids_by_brush.clear();
		brushid_by_tex.clear();




		// ouverture du fichier [BB3D]
		MString output_filename = file.fullName();
		output_base_directory = file.path();
#ifdef TRACE
		Affich("start file");
		Affich(output_base_directory.asChar());
#endif
		output.open(output_filename.asChar(),ios::out |ios::binary);
		// ecriture header fichier
		output << "BB3D"; // mise en place du Header fichier B3D
		StartChunck();

	
		
		write_int(1);//�criture de la version BB3D

				//liste des objets s�lectionn�s
		MSelectionList selection;
		MGlobal::getActiveSelectionList( selection );


#ifdef EXPORT_NEW_TEXTURES
		//nouvel export des textures

		getMatids(selection);//stockage des nom des shaders dans Matid

	

#endif

#ifdef EXPORT_NEW_BRUSHES
		//nouvel export de brush



#endif

		//nouveau code pour la d�tection des textures et des brush
		nb_Tex_by_Brush.clear();
		//initialisation du nombre de textures par brush
		for(int b=0;b<Matid.length();b++){
			nb_Tex_by_Brush.append(0);
		}


				scan_textures_used(selection);// place les index de tous les materiaux et textures


						//Matid.clear();
		//Texid.clear();
		//nb_Tex_by_Brush.clear();
		//Texids_by_brush.clear();
		//brushid_by_tex.clear();
		

#ifdef EXPORT_TEXTURES
		if(Flags.export_textures){
		::output << "TEXS";//header Brush
		StartChunck();
		//�criture les textures s'il y en a
		if (Texid.length()>0){
		//Affich("enregistre textures");

		
		// �criture des textures si pr�sentent [TEXS]|detection des textures � enregistrer| + fermeture texs


		


		//�criture textures ok
		OutputTex(selection);

	

		}
	EndChunck();// fermeture textures

		}//fin if si export textures
#endif

#ifdef EXPORT_BRUSHES
		if(Flags.export_brushes){
		// A FAIRE ecritures des BRUSHS
#ifdef OLD_BRUSH

#else
// �criture des textures si pr�sentent [TEXS]|detection des textures � enregistrer| + fermeture texs
	::output << "BRUS";//header Brush
	StartChunck();
	write_int(nbMaxb3dTextures);//�criture du nombre maxi de textures support�es par blitz3D
#endif
		

	

			//Matid.clear();
		//Texid.clear();
		//nb_Tex_by_Brush.clear();
			//Texids_by_brush.clear();
		//brushid_by_tex.clear();

		getBrushes(selection);


#ifdef OLD_BRUSH

#else
	EndChunck();
#endif
	
		}
#endif



	//-----------export nodes-----------------

		//r�cup�ration de la liste de s�lection
		MSelectionList Selection;
		MGlobal::getActiveSelectionList(Selection);


		int pos_old=0;
		int pos_objet=0;
		// cr�ation du chemin vers un node
		MItDag it(MItDag::kDepthFirst, MFn::kTransform);
		while(!it.isDone()){
			MDagPath Path;
			it.getPath(Path);
			if(Path.hasFn(MFn::kMesh)){
				// si le node poss�de un Mesh  ou autre � rajouter
				//mettre d'autres conditions si de nouveaux types de nodes sont d�sir�s
				//ex : MFn::kCamera    MFn::kLight  MFn::kLocator  si suppport�es par le moteur
				// attention si objet rencontr� non g�r� impact sur la hi�rarchie d�calage d'un cran vers le haut de la hi�rarchie
				//pouvant provoquer un comportement non d�sir�

			pos_objet = Hierar_pos(Path);

#ifdef TRACE
			affich = it.fullPathName().asChar();
			Affich(affich);
			affich = " path count :";
			affich += pos_objet;
			Affich(affich);
#endif
			if(pos_objet==pos_old || pos_objet<pos_old){
				//si node identique au pr�c�dent ou inf�rieur � l'actuel
				//fermeture des nodes pr�c�dents jusqu'au node actuel
#ifdef TRACE
			Affich("fermeture des nodes jusqu'au node actuel");
#endif
			for(int i=pos_objet;i<pos_old+1;i++){
#ifdef TRACE
				Affich("------------------close node");
#endif
				EndChunck();
				
			
			}

			}

				//ouverture du node
			MString nom_objet;
			nom_objet=Path.partialPathName();//chemin_split[chemin_split.length()-1];
#ifdef TRACE
			Affich("---------------------------ouverture node");
			affich = "---------------------------nom du node :: ";
			affich +=nom_objet;
			Affich(affich);
#endif

			//rescan des textures et mat�riaux utilis�s
			scan_textures_used(selection);// place les index de tous les materiaux et textures

//---------------------------�criture du node--------------------------------------------

			output << "NODE"; // mise en place du Header node
			StartChunck();
			//MString nom_objet;
			//nom_objet=Path.partialPathName();//chemin_split[chemin_split.length()-1];
			
			output << nom_objet.asChar();//nom du Node
			output << char(0x00);//caract�re de fin de cha�ne
			MVector Translation;
			// get the transforms local translation
			MFnTransform trans(Path);
			Translation = trans.getTranslation(MSpace::kTransform);
			float temp=(float)Translation.x;
			//�criture des coordonn�es spaciales
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

			MObject obj=it.item();
			//d�tection d'une pr�sence de mesh, ou un autre �l�ment
			if(obj.apiType()== MFn::kTransform && Path.child(0).apiType()== MFn::kMesh){
				//exportation Mesh

#ifdef TRACE
				Affich("exportation du mesh associ� � ce node");

#endif

#ifdef EXPORT_MESH
							//�criture du header mesh
							//mais avec des nodes vides, pour les mesh non s�lectionn�s
							//----------------------------ecriture mesh si objet polygonal pr�sent
							output << "MESH"; // mise en place du Header mesh
							StartChunck();
							
														MPointArray vertexArray;// coordonn�es des point format double x,y,z;
							MIntArray vertexList;// stockage des indexs des points pour les triangles
							MVector Normal;//stockage d'une normal d'un vertex
		
							//----------------------coordonn�es Vertexs  (normal & color si pr�sent et demand�s)
							//master brush
							//write_int(0xffffffff);
		//-----A FAIRE        d�finir le premier mat�riel rencontr�-----------------------*********
							write_int(-1);//-1 master brush
							//fin header mesh


#ifdef EXPORT_VRTS

							MFnMesh meshFn(Path.child(0)); // cr�e une fonction pour le mesh
							MItMeshVertex polyperVertex(Path, MObject::kNullObj);// cr�e une fonction pour le mesh , mais avec les fonctions de itmesh
						
							//r�cup�ration des coordonn�es des points
							//obtient les coordonn�es des vertex en mode global
							//meshFn.getPoints(vertexArray,MSpace::kObject);
							meshFn.getPoints(vertexArray,MSpace::kTransform);
							
							MIntArray uvCounts,uvIds;

							meshFn.getAssignedUVs(uvCounts,uvIds,0);

							//�criture header des VRTS
							//ecriture VRTS
							output<<"VRTS";
							StartChunck();

							//fin header VRTS

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
							int flag;
							if(colorsets.length()==0){
							flag=0;
							
							}
							else{
								flag=Flags.vertex_colors;
						
							}
							
							//flag_normal_colors = Flags.Normals+((Flags.use_vertex_colors && Flags.vertex_colors)*2);
							flag_normal_colors = Flags.Normals+((flag)*2);
							
							//else{
							//	if(Flags.use_vertex_colors!=0){
							//		Flags.vertex_colors=1;
							//	
							//	}
							//}



							
							//info = flag_normal_colors;
							//Affich(info);
							write_int(flag_normal_colors);//pr�sence normale
							
							//int tex_coord_sets          ;texture coords per vertex (eg: 1 for simple U/V) max=8
							write_int(1);//uv simple
							//  int tex_coord_set_size      ;components per set (eg: 2 for simple U/V) max=4
							write_int(2);//2 coordon�es textures






										
										




							float x,y,z,normx,normy,normz;//,normx,normy,normz;
							for (unsigned int i=0;i<vertexArray.length();i++){


								x =float(vertexArray[i].x); // -  pour replacer l'axe X dans le sens de celui de blitz
								y =float(vertexArray[i].y);
								z =-float(vertexArray[i].z);// -



								//vertices coords

								write_float(x);
								write_float(y);
								write_float(z);
	
								//r�cup�re la normale du point
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
									//r�cup�re la couleur moyenne des faces connect�s au point
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




							//fin VRTS
							EndChunck();

#endif

//----------------------------exportation des Triangles--------------------------------

	#ifdef OLD_TRIS
						//ecriture TRIS
						::output<<"TRIS";
						StartChunck();
//-----------------A FAIRE   d�tection des brush---------------------------------------------------
										//brush ID
										write_int(-1);//write_int(0);




										//MItMeshPolygon  itPolygon( path, MObject::kNullObj );
										MItMeshPolygon  itPolygon(Path.child(0));

	

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

										EndChunck();//fermeture du chunck tris

#else
						int instancenumber=Path.instanceNumber();
						MObjectArray shaders;
						MIntArray indices;
						meshFn.getConnectedShaders(instancenumber,shaders,indices);

						affich = "shaders length ";
						affich += shaders.length();
						Affich(affich);

						for(unsigned int i=0;i<shaders.length();i++){//navigation dans les shaders connect�s

							//*********************************************************************************************

								//___________ouput tris________
	
		affich = "shader ";
		affich += i;
	Affich(affich);

							//ecriture TRIS
						::output<<"TRIS";
						StartChunck();
						


	//trouver le brush id par rapport � matid
	//recup nom shader et compare � matid
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
	if(Flags.export_brushes){
						write_int(BrushId);
	}else{
		write_int(-1);
	}
						//write_int(-1);//default
			affich= "BrushId ";
		affich += BrushId;
	Affich(affich);

	affich = Matid[BrushId];
	Affich(affich);

	MItMeshPolygon  itPolygon(Path.child(0));
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
						
						
						}//fin for navigation dans les shaders connect�s
						


#endif


//--------------------------fin exportation des Triangles------------------------------


							//fermeture header mesh
							EndChunck();


#endif 


				//EndChunck();
				//fin exportation mesh

			}//fin d�tection mesh
			
			
//---------------------fin node  (endchunck �crit en d�but de loop------------------------
			pos_old=pos_objet;
			}//fin if node poss�de un mesh ou autre � rajouter
			it.next();
		}
		
#ifdef TRACE
			Affich("fermeture de tous les nodes ouverts");
#endif
			for(int i=1;i<pos_old+1;i++){
				
#ifdef TRACE
				Affich("------------------close node2");
#endif
				EndChunck();
			
			}
		

		//cr�ation du handler pour le mesh
	//----------------------------------------
	//------------fin export nodes

#ifdef TRACE
			//fermeture de tous les chuncks restants
		affich = "posfichier restants ";
		affich+= posfichier.length();
		Affich(affich);
#endif
		//fermeture de tous les chuncks rest�es ouverts
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
