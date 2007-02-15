;//--------------------------------------------------------------------
;// QuaternionToMatrix - Converts a quaternion rotation to a matrix
;// Thanks to Colin Hugues (SCEE) for that one
;//
;// Note: ACC and I registers are modified
;//--------------------------------------------------------------------
.macro   QuaternionToMatrix   matresult,quaternion

   mula.xyz       ACC,  \quaternion, \quaternion            ; xx yy zz

   loi            1.414213562
   muli           vclsmlftemp, \quaternion, I               ; x sqrt2 y sqrt2 z sqrt2 w sqrt2

   mr32.w         \matresult[0], vf00                       ; Set rhs matrix line 0 to 0
   mr32.w         \matresult[1], vf00                       ;
   mr32.w         \matresult[2], vf00                       ; Set rhs matrix
   move           \matresult[3], vf00                       ; Set bottom line to 0 0 0 1

   madd.xyz       vcl_2qq, \quaternion, \quaternion         ; 2xx       2yy       2zz
   addw.xyz       Vector111, vf00, vf00                     ; 1         1         1         -

   opmula.xyz     ACC,  vclsmlftemp, vclsmlftemp            ; 2yz       2xz       2xy       -
   msubw.xyz      vclsmlftemp2, vclsmlftemp, vclsmlftemp    ; 2yz-2xw   2xz-2yz   2xy-2zw   -
   maddw.xyz      vclsmlftemp3, vclsmlftemp, vclsmlftemp    ; 2yz+2xw   2xz+2yz   2xy+2zw   -
   addaw.xyz      ACC,  vf00, vf00                          ; 1         1         1         -
   msubax.yz      ACC,  Vector111, vcl_2qq                  ; 1         1-2xx     1-2xx

   msuby.z        \matresult[2], Vector111, vcl_2qq         ; -         -         1-2xx-2yy -
   msubay.x       ACC, Vector111, vcl_2qq                   ; 1-2yy     1-2xx     1-2xx-2yy -
   msubz.y        \matresult[1], Vector111, vcl_2qq         ; -         1-2xx-2zz -         -
   mr32.y         \matresult[0], vclsmlftemp2
   msubz.x        \matresult[0], Vector111, vcl_2qq         ; 1-2yy-2zz -         -         -
   mr32.x         \matresult[2], vclsmlftemp2
   addy.z         \matresult[0], vf00, vclsmlftemp3
   mr32.w         vclsmlftemp, vclsmlftemp2
   mr32.z         \matresult[1], vclsmlftemp
   addx.y         \matresult[2], vf00, vclsmlftemp3
   mr32.y         vclsmlftemp3, vclsmlftemp3
   mr32.x         \matresult[1], vclsmlftemp3

.endm

;//--------------------------------------------------------------------
;// QuaternionToTransposeMatrix - Converts a quaternion rotation to a 
;// that is the transpose of the one created by the function without
;// transpose in its name :)..
;// Thanks to Colin Hugues (SCEE) for that one
;//
;// Note: ACC and I registers are modified
;//--------------------------------------------------------------------
.macro   QuaternionToTransposeMatrix   matresult,quaternion

   mula.xyz       ACC,  \quaternion, \quaternion            ; xx yy zz

   loi            1.414213562
   muli           vclsmlftemp, \quaternion, I               ; x sqrt2 y sqrt2 z sqrt2 w sqrt2

   mr32.w         \matresult[0], vf00                       ; Set rhs matrix line 0 to 0
   mr32.w         \matresult[1], vf00                       ;
   mr32.w         \matresult[2], vf00                       ; Set rhs matrix
   move           \matresult[3], vf00                       ; Set bottom line to 0 0 0 1

   madd.xyz       vcl_2qq, \quaternion, \quaternion         ; 2xx       2yy       2zz
   addw.xyz       Vector111, vf00, vf00                     ; 1         1         1         -

   opmula.xyz     ACC,  vclsmlftemp, vclsmlftemp            ; 2yz       2xz       2xy       -
   msubw.xyz      vclsmlftemp3, vclsmlftemp, vclsmlftemp    ; 2yz-2xw   2xz-2yz   2xy-2zw   -
   maddw.xyz      vclsmlftemp2, vclsmlftemp, vclsmlftemp    ; 2yz+2xw   2xz+2yz   2xy+2zw   -
   addaw.xyz      ACC,  vf00, vf00                          ; 1         1         1         -
   msubax.yz      ACC,  Vector111, vcl_2qq                  ; 1         1-2xx     1-2xx

   msuby.z        \matresult[2], Vector111, vcl_2qq         ; -         -         1-2xx-2yy -
   msubay.x       ACC, Vector111, vcl_2qq                   ; 1-2yy     1-2xx     1-2xx-2yy -
   msubz.y        \matresult[1], Vector111, vcl_2qq         ; -         1-2xx-2zz -         -
   mr32.y         \matresult[0], vclsmlftemp2
   msubz.x        \matresult[0], Vector111, vcl_2qq         ; 1-2yy-2zz -         -         -
   mr32.x         \matresult[2], vclsmlftemp2
   addy.z         \matresult[0], vf00, vclsmlftemp3
   mr32.w         vclsmlftemp, vclsmlftemp2
   mr32.z         \matresult[1], vclsmlftemp
   addx.y         \matresult[2], vf00, vclsmlftemp3
   mr32.y         vclsmlftemp3, vclsmlftemp3
   mr32.x         \matresult[1], vclsmlftemp3

.endm

;//--------------------------------------------------------------------
;// QuaternionMultiply - Multiplies "quaternion1" and "quaternion2",
;// and puts the result in "quatresult".
;// Thanks to Colin Hugues (SCEE) for that one
;//
;// Note: ACC register is modified
;//--------------------------------------------------------------------
.macro   QuaternionMultiply   quatresult,quaternion1,quaternion2
   mul            vclsmlftemp, \quaternion1, \quaternion2   ; xx yy zz ww

   opmula.xyz     ACC,         \quaternion1, \quaternion2   ; Start Outerproduct
   madd.xyz       ACC,         \quaternion1, \quaternion2[w]; Add w2.xyz1
   madd.xyz       ACC,         \quaternion2, \quaternion1[w]; Add w1.xyz2
   opmsub.xyz     \quatresult, \quaternion2, \quaternion1   ; Finish Outerproduct

   sub.w          ACC,         vclsmlftemp,  vclsmlftemp[z] ; ww - zz
   msub.w         ACC,         vf00,         vclsmlftemp[y] ; ww - zz - yy
   msub.w         \quatresult, vf00,         vclsmlftemp[x] ; ww - zz - yy - xx
.endm
