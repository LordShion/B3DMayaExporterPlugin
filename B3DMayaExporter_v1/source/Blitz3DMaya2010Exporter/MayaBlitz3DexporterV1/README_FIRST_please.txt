copy files in the same folders of your maya 2008 directory.

before overwrite the AEfileTemplate.mel and the AElambertCommon.mel files 
please do a backup of the original ones.



here are some advices for modeling & animation:

 try to put cutted UVs where there's less animation deformation, and in discret places of your mesh.
 be sure to use "detach components" were UVs are cutted otherwise you'll have messy UVs


try to have transform of your root joint at 0,0,0 and your mesh to 0,0,0 also 


smooth bind only is supported.

if you have to detach components, do it after the skinning process (after painting weights).



try to have keys on every channel (translation, rotation, scaling)

my get keys function is based on the translateX animCurve, so if there'no keys on this curve, 
no animation will be saved.

before exporting an animation, please set the framerate to a value.

the end and the start of animation are set by the time slider start & stop


I have provided to you a sample folder, with a full maya example.


Feel free to contact me for any questions or suggestions at



filipe@asianalternative.com



I hope you will enjoy working with maya & blitz3D.


