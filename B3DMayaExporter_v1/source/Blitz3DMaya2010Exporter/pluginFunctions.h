

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



int getPosObj(MDagPath path){
				int pos=0;
				
				MStringArray chemin_split;
				MString chemin=path.fullPathName();

		
				chemin.split((char)'|',chemin_split);

				pos=chemin_split.length();


return pos;
}
//ne pas toucher paerfect
void getMatids(MSelectionList selection){
//on checke toute la liste de sélection
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
					//Affich("pas de matériaux associé à cet objet");
			   
			   }break;

			default:{
				//au moins un matériau trouvé
				//chemin du path du matériau
				Affich("*******nom des shaders associés à l'objet**********");
				for(unsigned int j=0;j < shaders.length();++j)
				{

					
					affich = GetShaderName( shaders[j] ).asChar();
					
					//vérifie que le matériau n"est pas dans la liste des matériaux
					int MatExists=0;
					
					for(unsigned int b=0;b<Matid.length();b++){
						if(Matid[b]==affich) MatExists++;
					}
					if(MatExists==0){
						Matid.append(affich);
						Affich(affich);
					}

				}//fin for des matériaux associés à l'objet
					
					}
			}// fin switch case shaders

		}// fin for parcourt liste de selection

}
void ExportBrushes(MSelectionList selection){
	if(Flags.export_textures==0){
		//si pas de textures exportées alors mise à zero du nb de brush utilisées dans les brush
		//afin d'éviter un crash de maya
	//on checke toute la liste de sélection
		
	
		nb_Tex_by_Brush.clear();
		for(unsigned int b=0;b<Matid.length()+1;b++){
			nb_Tex_by_Brush.append(0);

		}//fin a enlever
	}
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
					//Affich("pas de matériaux associé à cet objet");
			   
			   }break;

			default:{
				//au moins un matériau trouvé
				//chemin du path du matériau
				Affich("*******nom des shaders associés à l'objet**********");
				for(unsigned int j=0;j < shaders.length();++j)
				{

					
					affich = GetShaderName( shaders[j] ).asChar();
					
					//vérifie que le matériau n"est pas dans la liste des matériaux
					int MatExists=0;
					
					for(unsigned int b=0;b<Matid.length();b++){
						if(Matid[b]==affich) MatExists++;
					}
					if(MatExists==0){
						Matid.append(affich);
						Affich(affich);

						//écriture des données du shader
												//écriture du nom de la brush
						::output << affich.asChar();
						::output << char(0x00);//fin de chaîne

							// attach a function set to the shading engine
						MFnDependencyNode fn( shaders[j] );
						// get access to the surfaceShader attribute. This will be connected to
						// lambert , phong nodes etc.
						MPlug sshader = fn.findPlug("surfaceShader");
						
						// will hold the connections to the surfaceShader attribute
						MPlugArray materials;

						// get the material connected to the surface shader
						sshader.connectedTo(materials,true,false);


						MFnDependencyNode fnMat(materials[0].node());

						MColor color;
						MPlug p;
						const char* name="Color";

						MString r = name;
						r += "R";
						MString g = name;
						g += "G";
						MString b = name;
						b += "B";
						MString a = name;
						a += "A";

						MFnLambertShader Lambert=materials[0].node();
						MFnPhongShader Phong=materials[0].node();
						MFnBlinnShader Blinn=materials[0].node();
						switch(materials[0].node().apiType()){
			case MFn::kLambert:
								
				Affich("LAMBERT nb_Tex_by_Brush[Matid.length()]");
				affich = nb_Tex_by_Brush[Matid.length()-1];
				Affich(affich);

				if(nb_Tex_by_Brush[Matid.length()-1]==0){
					write_float(Lambert.color().r);
					write_float(Lambert.color().g);
					write_float(Lambert.color().b);
					//write_float(INV(getColorLuminosity(Lambert.transparency().r,Lambert.transparency().g,Lambert.transparency().b)));
					
				}else{
					write_float(1);
					write_float(1);
					write_float(1);
					//write_float(1);
				}

			break;

			case MFn::kPhong:

				Affich("PHONG nb_Tex_by_Brush[Matid.length()]");
				affich = nb_Tex_by_Brush[Matid.length()-1];
				Affich(affich);

				if(nb_Tex_by_Brush[Matid.length()-1]==0){
					write_float(Phong.color().r);
					write_float(Phong.color().g);
					write_float(Phong.color().b);
					//write_float(INV(getColorLuminosity(Phong.transparency().r,Phong.transparency().g,Phong.transparency().b)));

				}else{
					write_float(0);
					write_float(1);
					write_float(1);
					//write_float(1);
				}

			break;

			case MFn::kBlinn :
								Affich("BLINN nb_Tex_by_Brush[Matid.length()]");
				affich = nb_Tex_by_Brush[Matid.length()];
				Affich(affich);

				if(nb_Tex_by_Brush[Matid.length()]==0){
					write_float(Blinn.color().r);
					write_float(Blinn.color().g);
					write_float(Blinn.color().b);
					//write_float(INV(getColorLuminosity(Blinn.transparency().r,Blinn.transparency().g,Blinn.transparency().b)));
				}else{
					write_float(1);
					write_float(1);
					write_float(1);
					//write_float(1);
				}

			break;

			default :
					write_float(1);
					write_float(1);
					write_float(1);
					//write_float(1);

			break;
						
				}//fin switch case materiaux

//écrire la valeur de l'attribut alpha
						MPlug plug = fnMat.findPlug("BrushAlpha");
						float Brushalpha;
						plug.getValue(Brushalpha);
						write_float(Brushalpha);

//default shininess = 0
						plug = fnMat.findPlug("BrushShininess");
						float shininess;
						plug.getValue(shininess);
						write_float(shininess);
						
						

						//default blend
						//blend -
						//1: alpha
						//2: multiply (default)
						//3: add
						plug = fnMat.findPlug("BrushBlending");
						int BrushBlending;
						plug.getValue(BrushBlending);
						write_int(BrushBlending);
						//write_int(1);

						//defaultFX
						//			fx -
						//1: full-bright
						//2: use vertex colors instead of brush color
						//4: flatshaded
						//8: disable fog 

						plug = fnMat.findPlug("FullBright");
						int FullBright;
						plug.getValue(FullBright);
						plug = fnMat.findPlug("TwoSided");
						int TwoSided;
						plug.getValue(TwoSided);
						plug = fnMat.findPlug("FlatShaded");
						int FlatShaded;
						plug.getValue(FlatShaded);
						plug = fnMat.findPlug("DisableFog");
						int DisableFog;
						plug.getValue(DisableFog);
						plug = fnMat.findPlug("VertexColors");
						int VertexColors;
						plug.getValue(VertexColors);
						plug = fnMat.findPlug("VertexAlpha");
						int VertexAlpha;
						plug.getValue(VertexAlpha);


						affich=FullBright;
						Affich("Fullbright-----------------------");
						Affich(affich);

						//VertexColors=VertexColors*2;
						FlatShaded=FlatShaded*4;
						DisableFog=DisableFog*8;
						TwoSided=TwoSided*16;
						




//Sets miscellaneous effects for a brush.
//Flags can be added to combine two or more effects. For example, specifying a flag of 3 (1+2) will result in a full-bright and vertex-coloured brush.
 

						int flag = 0;
						flag = FullBright+(Flags.vertex_colors & VertexColors)*2+FlatShaded+DisableFog+TwoSided;//
						write_int(flag);

						//écriture des textures id
						//for(int b=0;b<nbMaxb3dTextures;b++){
							//A FAIRE ECRITURE LES ID DES TEXTURES
						//		write_int(-1);
						//	}//fin écritures des textures id

						
						Affich("-----------controle nouveau------------");
						
						unsigned int indtext=0;
if(Flags.export_textures){
						int indTex=0;
						for(unsigned int b=0;b<Matid.length();b++){
							

							indTex += nbTexMat[b];
												
						
						}
						indTex -= nbTexMat[Matid.length()-1];
						for(int d=0;d<nbTexMat[Matid.length()-1];d++){
							affich = TexNb[indTex];
							Affich(affich);
							

							for(unsigned int c=0;c<Texid.length();c++){
								if(Texid[c]==TexNb[indTex]){
									indtext++;
									write_int(c);// écriture de l'index de la texture utilisée
									
								}
							}

							indTex++;
						
						}

}else{
								indtext=0;
						}
						if(indtext<nbMaxb3dTextures){
							for(unsigned int i=indtext;i<nbMaxb3dTextures;i++){
								write_int(-1);//remplissage de -1 si texture non utilisée
							}
						
						}

						

					}//fin if matériau exists

				}//fin for des matériaux associés à l'objet
					
					}
			}// fin switch case shaders

		}// fin for parcourt liste de selection




}


// fin de ne pas toucher parfait

// ------------------------------------------écriture des textures
//-------------------------------------------


void addTexture(MObject myNode,MDagPath selected){

		// will hold the txture node name
	MString texname;
					
					nbTexMat[nbTexMat.length()-1]++;
#ifdef TRACE
										Affich("*******");
					Affich((plugs[loop].name().asChar()));
					affich=plugs.length();
					Affich(affich);
					//if(loop>plugs.length()){
					//	Affich("pas de plug suivant");
					//}else{
					//	Affich(plugs[loop+1].name().asChar());
					//}
					Affich("*******");
#endif

  					// bind a function set to it ....
					MFnDependencyNode fnDep(myNode);

					// to get the node name
					texname = fnDep.name();
					

					int TexExists=0;
						for(unsigned int i=0;i<Texid.length();i++)
							{
								if(texname==Texid[i]) TexExists=1;
							}
						if(TexExists==0){
								TexNb.append(texname);
							// écrire la texture car absente
								Texid.append(texname);
								nb_Tex_by_Brush[Matid.length()]++;
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
	brushid_by_tex.append(Matid.length());
	
//************************TO DO*****************************
	MString relativePath = filename.asChar();
	if(Flags.use_relative_path){
	//si utilisation des chemins relatifs
		//création du chemin relatif associé au nom du fichier

		Affich("Using Relative Paths");
		output_base_directory.asChar();

	 //char *test = char *(output_base_directory.asChar());
		//char * relativePath;
		//for (int b = 0; b < sizeof(output_base_directory.asChar()); i++)
   // relativePath[b]  = output_base_directory.asChar()[b];

		relativePath = GetRelativeFilename(output_base_directory.asChar(),filename.asChar());

	Affich("Relative Path");
	Affich(relativePath);


	}
//*************************FIN TO DO***************************

								::output << relativePath;
								
								
								
								::output << char(0x00);
								//write flags, blend default 1, 2
								//données à déterminer par rapport à color ou transparency
								//1: Color
								//2: Alpha
								//4: Masked
								//8: Mipmapped
								//16: Clamp U
								//32: Clamp V
								//64: Spherical reflection map
								//Flags can be added to combine two or more effects, e.g. 3 (1+2) = texture with colour and alpha maps.

						
						ftn = fnDep.findPlug("useColor");
						int useColor;
						ftn.getValue(useColor);
						//useColor**1;

						ftn = fnDep.findPlug("Alpha");
						int FileAlpha;
						ftn.getValue(FileAlpha);
						FileAlpha=FileAlpha*2;

						ftn = fnDep.findPlug("Masked");
						int Masked;
						ftn.getValue(Masked);
						Masked=Masked*4;


						ftn = fnDep.findPlug("Mipmap");
						int MipMap;
						ftn.getValue(MipMap);
						MipMap=MipMap*8;

						ftn = fnDep.findPlug("ClampU");
						int ClampU;
						ftn.getValue(ClampU);
						ClampU=ClampU*16;


						ftn = fnDep.findPlug("ClampV");
						int ClampV;
						ftn.getValue(ClampV);
						ClampV=ClampV*32;

						ftn = fnDep.findPlug("SphereEnv");
						int SphereEnv;
						ftn.getValue(SphereEnv);
						SphereEnv=SphereEnv*64;

						ftn = fnDep.findPlug("CubeMap");
						int CubeMap;
						ftn.getValue(CubeMap);
						CubeMap=CubeMap*128;

						ftn = fnDep.findPlug("StoreVram");
						int StoreVram;
						ftn.getValue(StoreVram);
						StoreVram=StoreVram*256;

						ftn = fnDep.findPlug("HiColor");
						int HiColor;
						ftn.getValue(HiColor);
						HiColor=HiColor*512;

						int flag=useColor+FileAlpha+Masked+MipMap+ClampU+ClampV+SphereEnv+CubeMap+StoreVram+HiColor;

						
						MFnMesh mesh(selected);
						affich = mesh.name().asChar();
						Affich(affich);
						
						Affich("UVsetNames");
						UvSetNames.clear();
						mesh.getUVSetNames(UvSetNames);
						int NoUvSet=0;
						for (unsigned int b=0;b<UvSetNames.length();b++){
							affich = UvSetNames[b].asChar();
							Affich(affich);
							MObjectArray textures;
							mesh.getAssociatedUVSetTextures(UvSetNames[b].asChar(),textures);

							for(unsigned int i=0;i<textures.length();i++){
								MFnDependencyNode fnTex(textures[i]);
								if(texname==fnTex.name().asChar()){
									Affich("texture is in this uvset");
									NoUvSet=b;
								}
								
							}
						
						}
						if(NoUvSet>0){
						flag+=65536;
						}
						
						
						
						write_int(flag);



							/*	if(name=="transparency"){
									write_int(2);
								}else{
									write_int(1);
								}//flags
							*/

//int mipmapped = 0;
//if(

							/*	MStringArray file_extension;
								relativePath.split('.',file_extension);
								*/
								
#ifdef TRACE
								affich = "extension = ";
								affich+=file_extension[file_extension.length()-1].asChar();
								Affich(affich);
#endif
								/*MString file_ext;//= "tga";
								if(file_extension[file_extension.length()-1]== "tga" || file_extension[file_extension.length()-1]== "png" || file_extension[file_extension.length()-1]== "dds"){
									write_int(3);
								}
								else {
									write_int(1);
								}*/
							

							//	write_int(1);
								//blend - blend mode of texture
								//0: disable texture
								//1: alpha
								//2: multiply (default)
								//3: add
								//This is used with multitexturing to control how the texture is combined with other textures.
								//write_int(2);// blend
						ftn = fnDep.findPlug("FileBlend");
						int FileBlend;
						ftn.getValue(FileBlend);
						write_int(FileBlend);



								// write position default O, O
								//données à chercher 
								write_float(0);
								write_float(0);
								//write scale default 1, 1
								//données à chercher 
								write_float(1);
								write_float(1);
								//rotation in radian default= 0
								//données à chercher 
								write_float(0);

								

							
								
							}else {
								TexNb.append(texname);
		nb_Tex_by_Brush[Matid.length()-1]=0;
		//Texids_by_brush.append(0);
				Texids_by_brush.append(0);
			}
					// stop looping
					//TexExists=0;


}
bool Outputtext(MFnDependencyNode& fn,const char* name,MDagPath selected)
{

	Affich("ce Shader est un -----------------------");
	affich=fn.name().asChar();
	Affich(affich);

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


	
	// get plugs connected to colour attribute
	MPlugArray plugs;
	p.connectedTo(plugs,true,false);
	int loop=0;
	// see if any file textures are present
	for(int i=0;i!=plugs.length();++i)
	{
		loop=i;
  			// if file texture found
	Affich("connecté à -----------------------");
	affich=plugs[i].node().apiTypeStr();
	Affich(affich);
	
	
	//TO DO-----------------------------------------------------TO DO
	// DETECTION ET GESTION kLayeredShader

			if(plugs[i].node().apiType() == MFn::kFileTexture)
			{
				addTexture(plugs[i].node(),selected);

			}else{
				if(plugs[i].node().apiType() == MFn::kLayeredTexture){
					
					
					Affich("------------Traitement du Layered Texture------------------");

					
					//à mettre  loop des inputs colors
					//MFnLayeredShader layshad(plugs[i].node());
					MFnDependencyNode fnDep(plugs[i].node());
					//MFnDependencyNode fnDep(layshad);

MPlug plug;

	plug = fnDep.findPlug("inputs.color");
	
	// get plugs connected to colour attribute
	MPlugArray pluggeds;

	fnDep.getConnections(pluggeds);
	

	//plug.connectedTo(pluggeds,true,false);

MPlugArray Plarray;
int maxtex=0;
	// see if any file textures are present
	for(unsigned int i=0;i<pluggeds.length()&&maxtex<8;++i)
	{
		
		Affich(pluggeds[i].name().asChar());

		pluggeds[i].connectedTo(Plarray,true,false);

		for(unsigned int b=0;b<Plarray.length();b++){
			
			affich = Plarray[b].node().apiTypeStr();
			Affich(affich);

								if(Plarray[b].node().apiType() == MFn::kFileTexture)
			{
				
				addTexture(Plarray[b].node(),selected);
				maxtex++;
			}
		
		}

			
	}




					Affich("------------Fin du Layered Shader------------------");

					}//fin if layered texture
			
			
			
			}//fin else if ktexture

			
		}//fin for look into plugs



return true;

}

void getTextures(MObject shadingEngine,MString tag,MDagPath selected)
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
	Outputtext(fnMat,"color",selected);
	//Outputtexture(fnMat,"transparency");

}

void ExportTextures(MSelectionList selection){

	getMatids(selection);
	//on checke toute la liste de sélection
		
		
		nb_Tex_by_Brush.clear();
		for(unsigned int b=0;b<Matid.length()+1;b++){
			nb_Tex_by_Brush.append(0);

		}
		
		Matid.clear();

			for(unsigned int i=0;i<selection.length();i++){
					
				MDagPath selectedPath;
				selection.getDagPath(i,selectedPath);
				MFnMesh meshFn(selectedPath);
				//MFnMesh GlobalMesh(selectedPath);

				unsigned int instancenumbers=0;
				MObjectArray shaders;
				MIntArray indices;

				meshFn.getConnectedShaders(instancenumbers,shaders,indices);
				
				UvSetNames.clear();
				Textures.clear();
				meshFn.getUVSetNames(UvSetNames);


				

		switch(shaders.length()) {

			case 0:{
					//Affich("pas de matériaux associé à cet objet");
			   
			   }break;

			default:{
				//au moins un matériau trouvé
				//chemin du path du matériau
				Affich("*******nom des shaders associés à l'objet**********");
				for(unsigned int j=0;j < shaders.length();++j)
				{

					
					affich = GetShaderName( shaders[j] ).asChar();
					
					//vérifie que le matériau n"est pas dans la liste des matériaux
					int MatExists=0;
					
					for(unsigned int b=0;b<Matid.length();b++){
						if(Matid[b]==affich) MatExists++;
					}
					if(MatExists==0){
						Matid.append(affich);
						Affich(affich);


						nbTexMat.append(0);
						getTextures(shaders[j],"color",selectedPath);

					
					}//fin if matériau exists

				}//fin for des matériaux associés à l'objet
					
					}
			}// fin switch case shaders

		}// fin for parcourt liste de selection




}
/*
void OutputAnimCurve (MObject &obj)
{
  	

MFnAnimCurve fn(obj);

unsigned int iKeyCount = fn.numKeys();

// dont bother if its a pointless curve....
if(iKeyCount == 0) return;


// get all keyframe times & values
for(unsigned int i=0;i<iKeyCount;i++)
{
  	

MTime Time = fnAnimCurve.time(i).value();
float Value = fnAnimCurve.value(i);


// now i'm not too sure about this, so I will
// state something that may be untrue. When
// getting the tangents to the animation curves
// you can pretty much ignore the x tangent
// values because time is generally a linear
// value. (am i right here?) If I am wrong then
// write the x values as well.
//float ix,iy,ox,oy;
//fnAnimCurve.getTangent(i,ix,iy,true);
//fnAnimCurve.getTangent(i,ox,oy,false);

// write keyframe info
//cout << " time " << Time.as(MTime::kSeconds);
//cout << " value " << Value;
////cout << " InTangent " << iy;
//cout << " OutTangent " << oy << endl;

}

}*/

MObject get_joint_root(MObject object){

	
bool isChild=false;
MDagPath pathRoot;

MItDag it(MItDag::kDepthFirst, MFn::kJoint);
for ( ; !it.isDone() && !isChild; it.next() ) {

MFnDagNode jointRoot(it.item());

//cout << jointRoot.fullPathName().asChar()<<endl;

jointRoot.getPath(pathRoot);
isChild=jointRoot.hasChild(object);
if(jointRoot.object()==object){
isChild=1;
}
//cout << isChild <<endl;


} 
return pathRoot.node();
}



void outputTransformData(MObject& obj)
{
  	

// attach the function set to the object
MFnTransform fn(obj);

// If you want the transform data as a matrix
// then you can do something like this. I personally
// am more interested in getting quaternions....
MMatrix mat = fn.transformation().asMatrix();

MQuaternion JointOrient(0,0,0,1);
MQuaternion Rotation(0,0,0,1);
double Scale[3];

// get the transforms local translation
MVector Translation = fn.translation(MSpace::kTransform);

// get the transforms scale
fn.getScale(Scale);

// get the transforms rotation as a quaternion
//fn.getRotation(Rotation);

MQuaternion orientJoint;//fnJoint.rotateOrientation(MSpace::kTransform));

//if the transform is an IK joint then it will contain
//a joint orientation as well as a rotation
if( fn.object().hasFn(MFn::kJoint) )
{
  	MFnIkJoint fnJoint(fn.object());
	Translation=fnJoint.getTranslation(MSpace::kTransform);
	fnJoint.getRotation(Rotation);
	fnJoint.getOrientation(JointOrient);
	orientJoint=fnJoint.rotateOrientation(MSpace::kTransform);

}

/*cout 	<< "translation "
<< Translation.x << " "
<< Translation.y << " "
<< Translation.z << endl;*/
write_float(-float(Translation.x));
write_float(float(Translation.y));
write_float(float(Translation.z));
/*
cout 	<< "scale "
<< Scale[0] << " "
<< Scale[1] << " "
<< Scale[2] << endl;
*/
write_float(float(Scale[0]));
write_float(float(Scale[1]));
write_float(float(Scale[2]));
/*
cout 	<< "rotation "
<< Rotation.x << " "
<< Rotation.y << " "
<< Rotation.z << " "
<< Rotation.w << endl;

cout<<" w Jo "<<JointOrient.w<<"o " <<orientJoint.w<<endl;
cout<<" x Jo "<<JointOrient.x<<"o " <<orientJoint.x<<endl;
cout<<" y Jo "<<JointOrient.y<<"o " <<orientJoint.y<<endl;
cout<<" z Jo "<<JointOrient.z<<"o " <<orientJoint.z<<endl;
*/

/*MEulerRotation rot(0,0,0,MEulerRotation::kXYZ);

rot.x = orientJoint.asEulerRotation().x;
rot.y = orientJoint.asEulerRotation().y;
rot.z = orientJoint.asEulerRotation().z;
rot.x = rot.x;
orientJoint.w = rot.asQuaternion().w;
orientJoint.x = rot.asQuaternion().x;
orientJoint.y = rot.asQuaternion().y;
orientJoint.z = rot.asQuaternion().z;

rot.x = JointOrient.asEulerRotation().x;
rot.y = JointOrient.asEulerRotation().y;
rot.z = JointOrient.asEulerRotation().z;
rot.x = rot.x;
JointOrient.w = rot.asQuaternion().w;
JointOrient.x = rot.asQuaternion().x;
JointOrient.y = rot.asQuaternion().y;
JointOrient.z = rot.asQuaternion().z;


rot.x = Rotation.asEulerRotation().x;
rot.y = Rotation.asEulerRotation().y;
rot.z = Rotation.asEulerRotation().z;
rot.x = -rot.x;
Rotation.w = rot.asQuaternion().w;
Rotation.x = rot.asQuaternion().x;
Rotation.y = rot.asQuaternion().y;
Rotation.z = rot.asQuaternion().z;

*/
orientJoint.x =-orientJoint.x;
JointOrient.x =-JointOrient.x;
Rotation.x =-Rotation.x;

//MQuaternion generalOrient= JointOrient * Rotation  ;
MQuaternion result =  JointOrient * Rotation *  orientJoint;

write_float(float(result.w));
write_float(float(result.x));
write_float(float(result.y));
write_float(float(result.z));

} 

void getKeysCurve (MObject &obj)
{
  	
	cout << "---------------outputing Keys curve------------------------" <<endl;
	cout<< obj.apiTypeStr()<<endl;
	MFnAnimCurve fnAnimCurve(obj);
	
	unsigned int iKeyCount = fnAnimCurve.numKeys();
	//MIntArray Keys;
	// dont bother if its a pointless curve....
	if(iKeyCount == 0) return;// Keys;
	
	cout << "AnimCurve " << fnAnimCurve.name().asChar() << endl;
	cout << "NumKeys " << iKeyCount << endl;
	
	
	
	// get all keyframe times & values
	for(unsigned int i=0;i<iKeyCount;i++)
	{
	  	
		
		MTime Time = fnAnimCurve.time(i).value();
MTime speed = MAnimControl::playbackSpeed();

int key = Time.as(MTime::kSeconds)*(speed.as(MTime::kFilm)*24);

		cout << "key "<<key<<" ";





		
		Keys.append(key);
		
		//float Value = fnAnimCurve.value(i);
	
		//cout << " value " << Value;

		
	}
	//return Keys;
}


void getValuesCurve (MObject &obj)
{
  	//MFloatArray Values;

	MFnAnimCurve fnAnimCurve(obj);
	
	unsigned int iKeyCount = fnAnimCurve.numKeys();
	
	// dont bother if its a pointless curve....
	if(iKeyCount == 0) return;// Values;
	
	cout << "AnimCurve " << fnAnimCurve.name().asChar() << endl;
	cout << "NumKeys " << iKeyCount << endl;
	
	// get all keyframe times & values
	for(unsigned int i=0;i<iKeyCount;i++)
	{
	  	
		
		MTime Time = fnAnimCurve.time(i).value();

		//int key = Time.as(MTime::kSeconds)*24;
		//cout << "key "<<key<<" ";
		
		float Value = fnAnimCurve.value(i);
	
		cout << " value " << Value;
		Values.append(Value);

		
	}
	
	
	//return Values;
}



void output_bone_influences(int b,MFnSkinCluster fn){



		MItGeometry gIter(skinPath);


		cout<< "Skin " << skinPath.partialPathName().asChar() <<endl;
		cout<< "pointcount " << gIter.count() << " numInfluences " << nInfs <<endl; 


for(int c=0;c<gIter.count();c++){
	

	//	cout << " vertex n° " << c <<endl;

			MObject comp = gIter.component();

			MFloatArray wts;
unsigned int infCount;
//regarde les influences b
fn.getWeights(skinPath,comp,b,wts);

//cout <<"b=" <<b << " nb wts trouvées "<< wts.length()<< "weight = " << wts[0] <<endl;
if(wts[0]!=0){
write_int(c);
write_float(wts[0]);
}

gIter.next();
}







}

bool AnimCurveFromAttr(MObject& node,const char* attrname)

{

  	
MStatus stat;
// attach the function set to the object
MFnDependencyNode fn(node);

// get the plug to the requested attribute
MPlug plug = fn.findPlug(attrname,&stat);

if(stat != MStatus::kSuccess) return false;

// use the function set to get the points
MPlugArray Connections;

// get an array of connections to this attribute
plug.connectedTo( Connections, true, false, 0 );

// loop through all connections. We are looking for
// any attached animation curves
	Keys.clear();
	Values.clear();
for(int i=0;i<Connections.length();++i) {
  	

// get a reference to the node
MObject connected = Connections[i].node();

// if it's an animation curve output it
if(connected.hasFn(MFn::kAnimCurve)) {
  	//Keys=getKeysCurve(connected);
	//Values=getValuesCurve(connected);
		cout << "---------------outputing anim curve------------------------" <<endl;
	

	getKeysCurve(connected);
	getValuesCurve(connected);

	//cout << " keys lenght "<< *Keys.length()<<endl;
	cout << "---------------end------------------------" <<endl;

}

}
//return Keys,Values;
} 

void export_joint_hierarchy(MFnDagNode jointRoot,MFnSkinCluster fn){

	int nbchilds = jointRoot.childCount();
	//cout << "nb childs : " << nbchilds << endl;
		//startChunck
		cout << "ouverture "; 
		cout << jointRoot.partialPathName().asChar()<<endl;  





			output << "NODE"; // mise en place du Header node
			StartChunck();
			//MString nom_objet;
			//nom_objet=Path.partialPathName();//chemin_split[chemin_split.length()-1];
			
			output << jointRoot.partialPathName().asChar();//nom du Node
			output << char(0x00);//caractêre de fin de chaîne
			

			outputTransformData(jointRoot.object());

					cout << "outputing weights"<<endl;
					output << "BONE"; // mise en place du Header node
					StartChunck();
			
						for(int b=0;b<infs.length();b++){
						if(jointRoot.fullPathName()==infs[b].fullPathName()){
						//exportation des influences

						//sample
							cout << "found bone influence"<<endl;
							cout << infs[b].fullPathName().asChar()<<endl;
							cout << " b = "<< b <<endl;

							output_bone_influences(b,fn);
					
						//write_int(1); //vertex id
						//write_float(1); // weight
									}
						}

	
		
				EndChunck();
				cout << "outputing weights"<<endl;
					output << "KEYS"; // mise en place du Header node
					//write_int(0);
					StartChunck();
					write_int(1+2+4); //flags 1 = position , 2=scale , 4= rotation;



					//to do
					// get translation
					// get rotation
					// get Scale
					//
					//FramesTranlation - Translations
					//FramesRotation - Rotations
					//FramesScaling - Scalings

MIntArray FramesTranslation;
MIntArray FramesRotation;
MIntArray FramesScaling;

MFloatArray TranslationsX,TranslationsY,TranslationsZ;
MFloatArray RotationsX,RotationsY,RotationsZ,RotationsW;
MFloatArray orientsJointsW,orientsJointsX,orientsJointsY,orientsJointsZ;
MFloatArray JointsOrientsW,JointsOrientsX,JointsOrientsY,JointsOrientsZ;
MFloatArray ScalingsX,ScalingsY,ScalingsZ;

//MIntArray Keys;
//MFloatArray Values;

AnimCurveFromAttr(jointRoot.object(),"translateX");
FramesTranslation=Keys;
TranslationsX=Values;

AnimCurveFromAttr(jointRoot.object(),"translateY");
TranslationsY=Values;

AnimCurveFromAttr(jointRoot.object(),"translateZ");
TranslationsZ=Values;

AnimCurveFromAttr(jointRoot.object(),"scaleX");
ScalingsX=Values;

AnimCurveFromAttr(jointRoot.object(),"scaleY");
ScalingsY=Values;

AnimCurveFromAttr(jointRoot.object(),"scaleZ");
ScalingsZ=Values;


AnimCurveFromAttr(jointRoot.object(),"rotateX");
RotationsX=Values;

AnimCurveFromAttr(jointRoot.object(),"rotateY");
RotationsY=Values;

AnimCurveFromAttr(jointRoot.object(),"rotateZ");
RotationsZ=Values;
/*
AnimCurveFromAttr(jointRoot.object(),"Joint Orient X");
JointsOrientsX=Values;
AnimCurveFromAttr(jointRoot.object(),"Joint Orient Y");
JointsOrientsY=Values;
AnimCurveFromAttr(jointRoot.object(),"Joint Orient Z");
JointsOrientsZ=Values;
*/

for(unsigned int k=0;k<FramesTranslation.length();k++){
write_int(FramesTranslation[k]);
write_float(-TranslationsX[k]);
write_float(TranslationsY[k]);
write_float(TranslationsZ[k]);
write_float(ScalingsX[k]);
write_float(ScalingsY[k]);
write_float(ScalingsZ[k]);


MEulerRotation Rotate(RotationsX[k],RotationsY[k],RotationsZ[k],MEulerRotation::kZYX);


MFnTransform transfn(jointRoot.object());
MQuaternion JointOrient(0,0,0,1);
MQuaternion Rotation(0,0,0,1);
double Scale[3];

MQuaternion orientJoint;

MFnIkJoint fnJoint(transfn.object());
	//Translation=fnJoint.getTranslation(MSpace::kTransform);
	fnJoint.getRotation(Rotation);
	fnJoint.getOrientation(JointOrient);

	orientJoint=fnJoint.rotateOrientation(MSpace::kTransform);
	
	orientJoint.x = -orientJoint.x;
JointOrient.x = -JointOrient.x;
Rotate.x = -Rotate.x;

//MQuaternion generalOrient= JointOrient * Rotate.asQuaternion()  ;
//MQuaternion result =  generalOrient * orientJoint;

MQuaternion result = orientJoint * JointOrient * Rotate.asQuaternion();//JointOrient * orientJoint * 

write_float(float(result.w));
write_float(float(result.x));
write_float(float(result.y));
write_float(float(result.z));






}		

					EndChunck();




				for(unsigned int i=0;i<nbchilds;i++){


		

					export_joint_hierarchy(jointRoot.child(i),fn);
	


					} 
				

	EndChunck();
	cout << "fermeture ";
	cout << jointRoot.partialPathName().asChar()<<endl;
	//endchunck

}