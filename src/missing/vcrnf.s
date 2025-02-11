         MACRO
&LABEL   VCRNF &V1,&V2,&V3,&M4,&M5
         PUSH  PRINT,NOPRINT
         PRINT NOGEN,NOPRINT
.TYPEV1  AIF   (T'&V1 EQ 'N').TYPEV2
         MNOTE 8,'*** V1 must be a number ***'
         AGO   .GETOUT
.TYPEV2  AIF   (T'&V2 EQ 'N').TYPEV3
         MNOTE 8,'*** V2 must be a number ***'
         AGO   .GETOUT
.TYPEV3  AIF   (T'&V3 EQ 'N').TYPEM4
         MNOTE 8,'*** V3 must be a number ***'
         AGO   .GETOUT
.TYPEM4  AIF   (T'&M4 EQ 'N').TYPEM5
         MNOTE 8,'*** M4 must be a number ***'
         AGO   .GETOUT
.TYPEM5  AIF   (T'&M5 EQ 'N').RANGV1
         MNOTE 8,'*** M5 must be a number ***'
         AGO   .GETOUT
.RANGV1  AIF   (&V1 GE 0 AND &V1 LT 32).RANGV2
         MNOTE 8,'*** V1 must be between 0 and 31 ***'
         AGO   .GETOUT
.RANGV2  AIF   (&V2 GE 0 AND &V2 LT 32).RANGV3
         MNOTE 8,'*** V2 must be between 0 and 31 ***'
         AGO   .GETOUT
.RANGV3  AIF   (&V3 GE 0 AND &V3 LT 32).RANGM4
         MNOTE 8,'*** V3 must be between 0 and 31 ***'
         AGO   .GETOUT
.RANGM4  AIF   (&M4 GE 0 AND &M4 LT 16).RANGM5
         MNOTE 8,'*** M4 must be between 0 and 15 ***'
         AGO   .GETOUT
.RANGM5  AIF   (&M5 GE 0 AND &M5 LT 16).SPLITV1
         MNOTE 8,'*** M5 must be between 0 and 15 ***'
         AGO   .GETOUT
.SPLITV1 ANOP
&NV1     SETA  &V1
&NV2     SETA  &V2
&NV3     SETA  &V3
&RXB     SETA  0
         AIF   (&V1 LT 16).SPLITV2
&RXB     SETA  &RXB+8
&NV1     SETA  &NV1-16
.SPLITV2 AIF   (&V2 LT 16).SPLITV3
&RXB     SETA  &RXB+4
&NV2     SETA  &NV2-16
.SPLITV3 AIF   (&V3 LT 16).BUILD
&RXB     SETA  &RXB+2
&NV3     SETA  &NV3-16
.BUILD   ANOP
&VV      SETA  &NV1*16+&NV2
&MR      SETA  &M4*16+&RXB
&NV3     SETA  &NV3*16
         DC    X'E6',AL1(&VV),AL1(&NV3),AL1(&M5),AL1(&MR),X'75'
.GETOUT  ANOP
         POP   PRINT,NOPRINT
         MEND
         MACRO
&LABEL   VCRNF &V1,&V2,&V3,&M4,&M5
         PUSH  PRINT,NOPRINT
         PRINT NOGEN,NOPRINT
.TYPEV1  AIF   (T'&V1 EQ 'N').TYPEV2
         MNOTE 8,'*** V1 must be a number ***'
         AGO   .GETOUT
.TYPEV2  AIF   (T'&V2 EQ 'N').TYPEV3
         MNOTE 8,'*** V2 must be a number ***'
         AGO   .GETOUT
.TYPEV3  AIF   (T'&V3 EQ 'N').TYPEM4
         MNOTE 8,'*** V3 must be a number ***'
         AGO   .GETOUT
.TYPEM4  AIF   (T'&M4 EQ 'N').TYPEM5
         MNOTE 8,'*** M4 must be a number ***'
         AGO   .GETOUT
.TYPEM5  AIF   (T'&M5 EQ 'N').RANGV1
         MNOTE 8,'*** M5 must be a number ***'
         AGO   .GETOUT
.RANGV1  AIF   (&V1 GE 0 AND &V1 LT 32).RANGV2
         MNOTE 8,'*** V1 must be between 0 and 31 ***'
         AGO   .GETOUT
.RANGV2  AIF   (&V2 GE 0 AND &V2 LT 32).RANGV3
         MNOTE 8,'*** V2 must be between 0 and 31 ***'
         AGO   .GETOUT
.RANGV3  AIF   (&V3 GE 0 AND &V3 LT 32).RANGM4
         MNOTE 8,'*** V3 must be between 0 and 31 ***'
         AGO   .GETOUT
.RANGM4  AIF   (&M4 EQ 0).RANGM5
         MNOTE 8,'*** M4 must be 0 ***'
         AGO   .GETOUT
.RANGM5  AIF   (&M5 EQ 2).SPLITV1
         MNOTE 8,'*** M5 must be 2 ***'
         AGO   .GETOUT
.SPLITV1 ANOP
&NV1     SETA  &V1
&NV2     SETA  &V2
&NV3     SETA  &V3
&RXB     SETA  0
         AIF   (&V1 LT 16).SPLITV2
&RXB     SETA  &RXB+8
&NV1     SETA  &NV1-16
.SPLITV2 AIF   (&V2 LT 16).SPLITV3
&RXB     SETA  &RXB+4
&NV2     SETA  &NV2-16
.SPLITV3 AIF   (&V3 LT 16).BUILD
&RXB     SETA  &RXB+2
&NV3     SETA  &NV3-16
.BUILD   ANOP
&VV      SETA  &NV1*16+&NV2
&MR      SETA  &M4*16+&RXB
&NV3     SETA  &NV3*16
         DC    X'E6',AL1(&VV),AL1(&NV3),AL1(&M5),AL1(&MR),X'75'
.GETOUT  ANOP
         POP   PRINT,NOPRINT
         MEND
