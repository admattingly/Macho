         MACRO
&LABEL   VCFN  &V1,&V2,&M3,&M4
         PUSH  PRINT,NOPRINT
         PRINT NOGEN,NOPRINT
.TYPEV1  AIF   (T'&V1 EQ 'N').TYPEV2
         MNOTE 8,'*** V1 must be a number ***'
         AGO   .GETOUT
.TYPEV2  AIF   (T'&V2 EQ 'N').TYPEM3
         MNOTE 8,'*** V2 must be a number ***'
         AGO   .GETOUT
.TYPEM3  AIF   (T'&M3 EQ 'N').TYPEM4
         MNOTE 8,'*** M3 must be a number ***'
         AGO   .GETOUT
.TYPEM4  AIF   (T'&M4 EQ 'N').RANGV1
         MNOTE 8,'*** M4 must be a number ***'
         AGO   .GETOUT
.RANGV1  AIF   (&V1 GE 0 AND &V1 LT 32).RANGV2
         MNOTE 8,'*** V1 must be between 0 and 31 ***'
         AGO   .GETOUT
.RANGV2  AIF   (&V2 GE 0 AND &V2 LT 32).RANGM3
         MNOTE 8,'*** V2 must be between 0 and 31 ***'
         AGO   .GETOUT
.RANGM3  AIF   (&M3 GE 0 AND &M3 LT 16).RANGM4
         MNOTE 8,'*** M3 must be between 0 and 15 ***'
         AGO   .GETOUT
.RANGM4  AIF   (&M4 GE 0 AND &M4 LT 16).SPLITV1
         MNOTE 8,'*** M4 must be between 0 and 15 ***'
         AGO   .GETOUT
.SPLITV1 ANOP
&NV1     SETA  &V1
&NV2     SETA  &V2
&RXB     SETA  0
         AIF   (&V1 LT 16).SPLITV2
&RXB     SETA  &RXB+8
&NV1     SETA  &NV1-16
.SPLITV2 AIF   (&V2 LT 16).BUILD
&RXB     SETA  &RXB+4
&NV2     SETA  &NV2-16
.BUILD   ANOP
&VV      SETA  &NV1*16+&NV2
&MR      SETA  &M3*16+&RXB
         DC    X'E6',AL1(&VV),AL2(&M4),AL1(&MR),X'5D'
.GETOUT  ANOP
         POP   PRINT,NOPRINT
         MEND
