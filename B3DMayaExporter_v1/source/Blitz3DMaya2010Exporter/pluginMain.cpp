//
// Copyright (C) NekoTech 
// 
// File: pluginMain.cpp
//
// Author: Maya Plug-in Wizard 2.0
//
// test pour voir si la révision s'opère



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

		// register the mel command with the plugin function set.
	// Do this for each mel command your plugin is going to add into Maya
	//
	status = plugin.registerCommand( "addBlitz3DImageFileAttributes", addBlitz3DImageFileAttributes::creator ); 
	if (!status) 
	{
		status.perror("Failed to register \"addBlitz3DImageFileAttributes\"\n"); 
		return status; 
	}

		status = plugin.registerCommand( "addBlitz3DShaderAttributes", addBlitz3DShaderAttributes::creator ); 
	if (!status) 
	{
		status.perror("Failed to register \"addBlitz3DShaderAttributes\"\n"); 
		return status; 
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

		status = plugin.deregisterCommand( "addBlitz3DImageFileAttributes" ); 
	if (!status) { 
		status.perror("failed to deregister \"addBlitz3DImageFileAttributes\"\n"); 
		return status; 
	} 

		status = plugin.deregisterCommand( "addBlitz3DShaderAttributes" ); 
	if (!status) { 
		status.perror("failed to deregister \"addBlitz3DShaderAttributes\"\n"); 
		return status; 
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

		// if we recognised our second option
		if( Option == "UseRoot" ) {
  		// check for true or false
			if(optionList[++i]=="0")
				Flags.UseRoot=0;
			else
				Flags.UseRoot=1;

		}

		// if we recognised our second option
		if( Option == "exportAnim" ) {
  		// check for true or false
			if(optionList[++i]=="0")
				Flags.exportAnim=0;
			else
				Flags.exportAnim=1;

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

	
		
		write_int(1);//écriture de la version BB3D

				//liste des objets sélectionnés
		MSelectionList selection;
		MGlobal::getActiveSelectionList( selection );










		getMatids(selection);//stockage des nom des shaders dans Matid



		//posMat.clear();
		nbTexMat.clear();
		TexNb.clear();

#ifdef EXPORT_TEXTURES
		if(Flags.export_textures){

		::output << "TEXS";//header Brush
		StartChunck();
		ExportTextures(selection);
		EndChunck();
		}//fin if si export textures


#endif

#ifdef EXPORT_BRUSHES
		if(Flags.export_brushes){
		// A FAIRE ecritures des BRUSHS

		// écriture des textures si présentent [TEXS]|detection des textures à enregistrer| + fermeture texs
		::output << "BRUS";//header Brush
		StartChunck();
		write_int(nbMaxb3dTextures);//écriture du nombre maxi de textures supportées par blitz3D

		
       ExportBrushes(selection);
	



		EndChunck();

	
		}//fin if brush is to be exported
#endif



	//-----------export nodes-----------------

		//récupération de la liste de sélection
		MSelectionList Selection;
		MGlobal::getActiveSelectionList(Selection);



		//------------------écriture du Scenne Root si demandé----------------------
		if(Flags.UseRoot==1){


			output << "NODE"; // mise en place du Header node
			StartChunck();
			//MString nom_objet;
			//nom_objet=Path.partialPathName();//chemin_split[chemin_split.length()-1];
			
			output << "Scene Root";//nom du Node
			output << char(0x00);//caractêre de fin de chaîne


			//transaltion
			write_float(0);// translation x
			write_float(0);// translation y
			write_float(0);// translation z

			//scale			
			write_float(1);	//x		
			write_float(1);	//y		
			write_float(1);//z

			//rotatoin
			write_float(1);//x???
			write_float(0);//y
			write_float(0);//z
			write_float(0);//w

		
		
		}
		//-----------------fin écriture Scene Root-------------------------
		int pos_old=0;
		int pos_objet=0;
		// création du chemin vers un node
		MItDag it(MItDag::kDepthFirst, MFn::kTransform);
		while(!it.isDone()){
			MDagPath Path;
			it.getPath(Path);
			if(Path.hasFn(MFn::kMesh)){
				// si le node possêde un Mesh  ou autre à rajouter
				//mettre d'autres conditions si de nouveaux types de nodes sont désirés
				//ex : MFn::kCamera    MFn::kLight  MFn::kLocator  si suppportées par le moteur
				// attention si objet rencontré non géré impact sur la hiérarchie décalage d'un cran vers le haut de la hiérarchie
				//pouvant provoquer un comportement non désiré

			pos_objet = Hierar_pos(Path);

#ifdef TRACE
			affich = it.fullPathName().asChar();
			Affich(affich);
			affich = " path count :";
			affich += pos_objet;
			Affich(affich);
#endif
			if(pos_objet==pos_old || pos_objet<pos_old){
				//si node identique au précédent ou inférieur à l'actuel
				//fermeture des nodes précédents jusqu'au node actuel
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

			//rescan des textures et matériaux utilisés

			//scan_textures_used(selection);// place les index de tous les materiaux et textures

//---------------------------écriture du node--------------------------------------------

			output << "NODE"; // mise en place du Header node
			StartChunck();
			//MString nom_objet;
			//nom_objet=Path.partialPathName();//chemin_split[chemin_split.length()-1];
			
			output << nom_objet.asChar();//nom du Node
			output << char(0x00);//caractêre de fin de chaîne
			MVector Translation;
			// get the transforms local translation
			MFnTransform trans(Path);
			Translation = trans.getTranslation(MSpace::kTransform);

			float temp=-(float)Translation.x;
			//écriture des coordonnées spaciales
			//transaltion
			write_float(temp);// translation x
			temp=(float)Translation.y;
			write_float(temp);// translation y
			temp=(float)Translation.z;
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
			temp=-(float)Rotation.x;
			write_float(temp);
			temp=(float)Rotation.y;
			write_float(temp);
			temp=(float)Rotation.z; 
			write_float(temp);

			MObject obj=it.item();
			//détection d'une présence de mesh, ou un autre élément
			if(obj.apiType()== MFn::kTransform && Path.child(0).apiType()== MFn::kMesh){
				//exportation Mesh

#ifdef TRACE
				Affich("exportation du mesh associé à ce node");

#endif

#ifdef EXPORT_MESH
							//écriture du header mesh
							//mais avec des nodes vides, pour les mesh non sélectionnés
							//----------------------------ecriture mesh si objet polygonal présent
							output << "MESH"; // mise en place du Header mesh
							StartChunck();
							
														MPointArray vertexArray;// coordonnées des point format double x,y,z;
							MIntArray vertexList;// stockage des indexs des points pour les triangles
							MVector Normal;//stockage d'une normal d'un vertex
		
							//----------------------coordonnées Vertexs  (normal & color si présent et demandés)
							//master brush
							//write_int(0xffffffff);
		//-----A FAIRE        définir le premier matériel rencontré-----------------------*********
							write_int(-1);//-1 master brush
							//fin header mesh


#ifdef EXPORT_VRTS


							//-------------------TO DO---------------------------------------------
							//ya-t-il un kSkinCluster Filter
							//Si oui alors prise en compte des données vertex du bind pose
							//sinon utiliser le premier meshFn Trouvé
							//
							//----------------------------------------------------------------------

							//-----------------------------------------------------------------------
							MFnMesh meshFn(Path.child(0)); // crée une fonction pour le mesh
							MItMeshVertex polyperVertex(Path, MObject::kNullObj);// crée une fonction pour le mesh , mais avec les fonctions de itmesh
						
							//récupération des coordonnées des points
							//obtient les coordonnées des vertex en mode global
							//meshFn.getPoints(vertexArray,MSpace::kObject);
							meshFn.getPoints(vertexArray,MSpace::kTransform);

							//MIntArray trianglecounts;
							//MIntArray trianglevertices;
							//meshFn.getTriangles(trianglecounts,trianglevertices);
							
							MStringArray UvSetames;
							meshFn.getUVSetNames(UvSetNames);
							
							MIntArray uvCounts,uvIds;

							meshFn.getAssignedUVs(uvCounts,uvIds,0);

							//écriture header des VRTS
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
							write_int(flag_normal_colors);//présence normale
							
							//int tex_coord_sets          ;texture coords per vertex (eg: 1 for simple U/V) max=8
							if(UvSetNames.length()>1) {write_int(2);
							}else{
								write_int(1);//uv simple
							}
							//  int tex_coord_set_size      ;components per set (eg: 2 for simple U/V) max=4
							write_int(2);//2 coordonées textures

				




							float x,y,z,normx,normy,normz;//,normx,normy,normz;
							for (unsigned int i=0;i<vertexArray.length();i++){


								x =-float(vertexArray[i].x); // -  pour replacer l'axe X dans le sens de celui de blitz
								y =float(vertexArray[i].y);
								z =float(vertexArray[i].z);// -



								//vertices coords

								write_float(x);
								write_float(y);
								write_float(z);
	
								//récupère la normale du point
								if(flag_normal_colors==1 || flag_normal_colors==3){
									meshFn.getVertexNormal(i, Normal ,MSpace::kTransform);
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
								
								float2 uvPoint;
								const MString uvname(UvSetNames[0].asChar());
								polyperVertex.getUV(uvPoint,&uvname);

								
									u=uvPoint[0];
									v=uvPoint[1];
								//u=uArray[0];
								//v=vArray[0];



								write_float(u);
								write_float(-v);

							if(UvSetNames.length()>1) {
								//uvname=UvSetNames[1].asChar();
								const MString uvname(UvSetNames[1].asChar());
								
								
								polyperVertex.getUV(uvPoint,&uvname);
								
									u=uvPoint[0];
									v=uvPoint[1];

								//u=uArray[1];
								//v=vArray[1];



								write_float(u);
								write_float(-v);
							}


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
//-----------------A FAIRE   détection des brush---------------------------------------------------
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
						
						
						}//fin for navigation dans les shaders connectés
						


#endif


//--------------------------fin exportation des Triangles------------------------------


							//fermeture header mesh
							EndChunck();

							//exportation données anim
							
							if(Flags.exportAnim==1){
							output << "ANIM"; // mise en place du Header mesh
							StartChunck();

							

							// flags unused , default = 0
							write_int(0);

							//nombre de frames au total
							//write_int(24);
							MTime Start=MAnimControl::animationStartTime();
							MTime End=MAnimControl::animationEndTime();

							write_int(End.as(MTime::kFilm)-Start.as(MTime::kFilm));
							
							cout << "---------------------------------------- total frames " << End.as(MTime::kFilm)-Start.as(MTime::kFilm)<< endl;

							

							// nombre de frame par seconde fps
							//write_float(24);

							MTime speed = MAnimControl::playbackSpeed();
							write_float(speed.as(MTime::kFilm)*24);

							cout << "------------------------------------------- playbackSpeed = " << speed.as(MTime::kFilm)*24 << endl;
							


							EndChunck();
							Affich("------------------Export Bones-------------------------------");
							
						MObject shape(meshFn.object());
MFnDependencyNode fn(shape);
cout << "objet a scanner -----------------------" << endl;
MStatus stat;
MPlug plug = fn.findPlug("inMesh",&stat);
if(stat == MS::kSuccess){
cout << "inMesh attribute found" <<endl;
}else{
cout << "inMesh attribute Not found" <<endl;
}
//print for control purpose
cout << plug.name().asChar() << endl;
MPlugArray Connect;
plug.connectedTo( Connect, true, false,&stat );
if(stat != MS::kSuccess){
cout << "nothing is connected to the plug" << endl;
}else{
cout << "something is connected" << endl;
}
//print for control purpose
cout << Connect.length() <<endl;
for(unsigned int i=0;i<Connect.length();++i) {
// get  the node
MObject connected = Connect[i].node();
cout << connected.apiTypeStr() << endl;

if(connected.apiType()==MFn::kSkinClusterFilter){
//j'ai trouvé un kSkinClusterFilter
	cout << "j'ai trouvé un kSkinClusterFilter" <<endl;
	//extraction des données
	cout << "extraction des données du kSkinClusterFilter" <<endl;

	//------------------------------------------------------
MFnSkinCluster fn(connected);
//MDagPathArray infs;

nInfs = fn.influenceObjects(infs,&stat);

//MItDependencyNodes it( MFn::kJoint );
cout<<"first influenced joint found  "<<infs[0].fullPathName().asChar()<<endl;

nGeoms = fn.numOutputConnections();
cout << "num nGeoms "<<nGeoms<<endl;

MFnDagNode jointRoot(get_joint_root(infs[0].node()));


cout << jointRoot.fullPathName().asChar() << endl;
cout << "Start export joints nodes"<<endl;


 	


unsigned int index;
index = fn.indexForOutputConnection(0); 
fn.getPathAtIndex(index,skinPath);

export_joint_hierarchy(jointRoot,fn);






	cout << "End export joints nodes"<<endl;
// le premier root joint est stocké dans  jointRoot





//}
	//------------------------------------------------------

	


}
} 


							Affich("-------------------fin export Bones------------------------");

cout << "------------------------------bones------------------" << endl;

							




							
							}


#endif 


				//EndChunck();
				//fin exportation mesh

			}//fin détection mesh
			
			
//---------------------fin node  (endchunck écrit en début de loop------------------------
			pos_old=pos_objet;
			}//fin if node possêde un mesh ou autre à rajouter
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
		

		//création du handler pour le mesh
	//----------------------------------------
	//------------fin export nodes

#ifdef TRACE
			//fermeture de tous les chuncks restants
		affich = "posfichier restants ";
		affich+= posfichier.length();
		Affich(affich);
#endif
		//fermeture de tous les chuncks restées ouverts
		for(int i=posfichier.length();i>0;i--){
			EndChunck();
		
		}
//---------------------fermeture du Scene Root-------------------
//		if(Flags.UseRoot==1){
//			EndChunck();
//		}


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

 MStatus addBlitz3DImageFileAttributes::doIt( const MArgList& args ) {
	// iterate through all nodes in the scene

	MItDependencyNodes it;
	while( !it.isDone() )
	{
		// attach a dependency node function set to the object
		MFnDependencyNode fn(it.item());
		
		// print the name
		MGlobal::displayInfo( fn.name().asChar() );
		
		// next item please!
		it.next();
	}
	return MS::kSuccess;
 }

 MStatus addBlitz3DShaderAttributes::doIt( const MArgList& args ) {
	// iterate through all nodes in the scene
	 
	 MSelectionList selection;
	 MGlobal::getActiveSelectionList(selection);

	 for(unsigned int i=0;i<selection.length();i++){
	 Affich("2");
	 
	 }


	return MS::kSuccess;
 }

