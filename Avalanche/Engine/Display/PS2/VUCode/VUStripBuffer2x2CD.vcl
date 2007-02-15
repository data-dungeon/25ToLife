
;;; give vcl all the vu registers
.init_vf_all
.init_vi_all

;;; this is the set of macros that are used to define microcodes.		  
.include "VUStripBufferKernel.i"		  

;;; the name of the group of microcodes defined in this file
.vu
;;; currently valid keywords:
;;;
;;; Outer most 
;;;  BeginMicrocode "name"		  * start of a microcode named "name"
;;;  EndMicrocode					  * end of a microcode named "name"
;;;  BeginLoop						  * start of vertex loop for microcode named "name"
;;;  EndLoop						  * end of vertex loop for microcode named "name"
;;; 
;;; Initialization * Between BeginMicrocode and BeginLoop:
;;;  InitKernel passes			  * must be there.  passes is the # of draw passes per vertex
;;;  LoadTransformMatrix		  * load the transform matrix into registers (can be moved to loop if registers is a problem)
;;;  LoadCullMatrix				  * load the culling matrix into registers (can be moved to loop if registers is a problem)
;;;  LoadTexMatrices				  * load all the texture transforms into registers (if registers is a problem, you can put some LoadTexMatrix lines into the loop)
;;;  LoadTexMatrix pass			  * load a texture transform for a particular pass (can be put in the loop if registers is a problem (This is a subset of LoadTexMatrices))
;;;
;;; Body * Between BeginLoop & EndLoop (most of these have to go in the order the data occurs in the input stream)
;;;  BaseTransformVertex vtx	  * transform the input vertex by the transform matrix (must be loaded) and store in vtx
;;;  PerspCorrectVertex vtx	  * perform the divide by w on the vertex vtx (div & mulq)
;;;  CullVertex dest				  * transform the input vertex by the cull matrix (must be loaded) & set ADC bit of W in dest
;;;  AlwaysDrawVertex dest		  * clear the ADC bit of W in dest
;;;  SaveVertex dest, vtx		  * coverts vtx to fixed point (into dest) and stores to output stream
;;;  LoadNormal dest				  * load the input normal into dest
;;;  LoadColorCompr color		  * load a compressed 8888 color into color[x]
;;;  SaveColorCompr color,qsrc  * saves (color[x], qsrc[y], 0x1) to output stream (1 == GS reg RGBAQ)
;;;  SaveTexCoordsForColorCompr vtx		 * stores [xy] of tex coords from vtx[pass] into the output stream
;;;  PerspCorrectTex dest, src  * perpective corrects tex coords in src[pass] into dest[pass]
;;;  SaveTexCoords vtx			  * stores [xyz] of tex coords from vtx[pass] into the output stream
;;;  LoadTexCoords vtx, pas, dep			 * decompresses the input texture coords into vtx for pass # pas as a set of dep floats
;;;  TransformTexCoords vtx, pas, dep	 * decompress the input texture coords into vtx while multiplying my tex matrix for pass # pas as a set of dep floats
;;;
;;; Body * Low level macros (you should probably NEVER need to use these :)
;;;  TransformVertex vtx, mtx	  * transform the input vertex by mtx (must be loaded) and store in vtx
;;;  NextField						  * move the kernels internal pointers to the next input field
;;;  BOnFlagClear flags, val, lab, tmp= * if(tmp=(!(flags & #val))) goto lab
;;;  BOnFlagSet flags, val, lab, tmp=	 * if(tmp=(!(flags & #val))) goto lab
;;;  MtxLoadInc mtx, vumem		  * load mtx from (vumem) and increment vumem
;;;  VectorLoadInc vec, vumem	  * load vector from (vumem) and increment vumem
;;;  VectorSaveInc vec, vumem	  * save vector to (vumem) and increment vumem
;;;  IntVecLoad offset, vumem, [dx], [dy], [dz], [dw]	  * load vector and store integers in registers dx,dy,dz,dw respectively
;;;
;;; VCL variables defined by the kernel that you can use
;;;  ones					* VF register filled with ones.
;;;  zeros					* VF register filled with zeros.
;;;  flagsA					* integer with flags from engine
;;;  flagsB					* integer with flags from engine
;;;  flagsC					* integer with flags from engine
;;;  loopCount				* current loop counter
;;;  transmtx[]			* transformation matrix
;;;  cullmtx[]				* cull matrix
;;;  texmtx[pass][]		* the texture transform matrices
;;;  vtxLines				* # of lines per vertex (from engine)
;;;  vtxcull				* post-cull transform vertex
;;;  culltemp				* integer 0x7fff or 0x8000 based on culling
;;;  fogParams[z]			* fog shift value
;;;  fogParams[w]			* fog scale value
;;; 
;;; VCL variables defined by the Kernel (you shouldnt use them in microcodes)
;;;  bufferBase
;;;  input
;;;  inittemp
;;;  output[]
;;;  texMatrixSrc
;;;  line
;;;  srcAddr
;;; 
;;; GASP variables used by the kernel (you shouldnt change them in microcodes because you may conflict)
;;;  KernelLine			* current input line #
;;;  KernelField			* current input field letter
;;;  KernelPasses			* number of passes passed to InitKernel
;;;  MAX_LINES				* max # of input lines supported
		  
 BeginMicrocode "VUStripBuffer2x2CD"
		  InitKernel		2
		  LoadCullMatrix
		  LoadTexMatrices
 BeginBuffer
 BeginLoop 
		  LoadTransformMatrix
		  CullVertex

		  TransformTexCoords		  finalTex,			0, 2
		  TransformTexCoords		  finalTex,			1, 2

		  LoadColorCompr			  finalColor
 EndLoop 
 EndBuffer
 EndMicrocode 


		  
;; tell gasp that it has reached the end.. :)  
.end
		  
