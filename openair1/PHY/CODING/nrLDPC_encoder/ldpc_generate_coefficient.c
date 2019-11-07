/*
 * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The OpenAirInterface Software Alliance licenses this file to You under
 * the OAI Public License, Version 1.1  (the "License"); you may not use this file
 * except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.openairinterface.org/?page_id=698
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *-------------------------------------------------------------------------------
 * For more information about the OpenAirInterface (OAI) Software Alliance:
 *      contact@openairinterface.org
 */

/*!\file ldpc_generate_coefficient.c
 * \brief Generates the optimized LDPC encoder
 * \author Florian Kaltenberger, Raymond Knopp, Kien le Trung (Eurecom)
 * \email openair_tech@eurecom.fr
 * \date 27-03-2018
 * \version 1.0
 * \note
 * \warning
 */

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "Gen_shift_value.h"
#include "assertions.h"
#include "defs.h"

short *choose_generator_matrix(short BG,short Zc)
{
  short *Gen_shift_values = NULL;

  if (BG==1)
  {
    switch (Zc)
    {
      case 2: Gen_shift_values=(short *) Gen_shift_values_BG1_Z_2;
        break;

      case 3: Gen_shift_values=(short *) Gen_shift_values_BG1_Z_3;
        break;

      case 4: Gen_shift_values=(short *) Gen_shift_values_BG1_Z_4;
        break;

      case 5: Gen_shift_values=(short *) Gen_shift_values_BG1_Z_5;
        break;

      case 6: Gen_shift_values=(short *) Gen_shift_values_BG1_Z_6;
        break;

      case 7: Gen_shift_values=(short *) Gen_shift_values_BG1_Z_7;
        break;

      case 8: Gen_shift_values=(short *) Gen_shift_values_BG1_Z_8;
        break;

      case 9: Gen_shift_values=(short *) Gen_shift_values_BG1_Z_9;
        break;

      case 10: Gen_shift_values=(short *) Gen_shift_values_BG1_Z_10;
        break;

      case 11: Gen_shift_values=(short *) Gen_shift_values_BG1_Z_11;
        break;

      case 12: Gen_shift_values=(short *) Gen_shift_values_BG1_Z_12;
        break;

      case 13: Gen_shift_values=(short *) Gen_shift_values_BG1_Z_13;
        break;

      case 14: Gen_shift_values=(short *) Gen_shift_values_BG1_Z_14;
        break;

      case 15: Gen_shift_values=(short *) Gen_shift_values_BG1_Z_15;
        break;

      case 16: Gen_shift_values=(short *) Gen_shift_values_BG1_Z_16;
        break;

      case 18: Gen_shift_values=(short *) Gen_shift_values_BG1_Z_18;
        break;

      case 20: Gen_shift_values=(short *) Gen_shift_values_BG1_Z_20;
        break;

      case 22: Gen_shift_values=(short *) Gen_shift_values_BG1_Z_22;
        break;

      case 24: Gen_shift_values=(short *) Gen_shift_values_BG1_Z_24;
        break;

      case 26: Gen_shift_values=(short *) Gen_shift_values_BG1_Z_26;
        break;

      case 28: Gen_shift_values=(short *) Gen_shift_values_BG1_Z_28;
        break;

      case 30: Gen_shift_values=(short *) Gen_shift_values_BG1_Z_30;
        break;

      case 32: Gen_shift_values=(short *) Gen_shift_values_BG1_Z_32;
        break;

      case 36: Gen_shift_values=(short *) Gen_shift_values_BG1_Z_36;
        break;

      case 40: Gen_shift_values=(short *) Gen_shift_values_BG1_Z_40;
        break;

      case 44: Gen_shift_values=(short *) Gen_shift_values_BG1_Z_44;
        break;

      case 48: Gen_shift_values=(short *) Gen_shift_values_BG1_Z_48;
        break;

      case 52: Gen_shift_values=(short *) Gen_shift_values_BG1_Z_52;
        break;

      case 56: Gen_shift_values=(short *) Gen_shift_values_BG1_Z_56;
        break;

      case 60: Gen_shift_values=(short *) Gen_shift_values_BG1_Z_60;
        break;

      case 64: Gen_shift_values=(short *) Gen_shift_values_BG1_Z_64;
        break;

      case 72: Gen_shift_values=(short *) Gen_shift_values_BG1_Z_72;
        break;

      case 80: Gen_shift_values=(short *) Gen_shift_values_BG1_Z_80;
        break;

      case 88: Gen_shift_values=(short *) Gen_shift_values_BG1_Z_88;
        break;

      case 96: Gen_shift_values=(short *) Gen_shift_values_BG1_Z_96;
        break;

      case 104: Gen_shift_values=(short *) Gen_shift_values_BG1_Z_104;
        break;

      case 112: Gen_shift_values=(short *) Gen_shift_values_BG1_Z_112;
        break;

      case 120: Gen_shift_values=(short *) Gen_shift_values_BG1_Z_120;
        break;

      case 128: Gen_shift_values=(short *) Gen_shift_values_BG1_Z_128;
        break;

      case 144: Gen_shift_values=(short *) Gen_shift_values_BG1_Z_144;
        break;

      case 160: Gen_shift_values=(short *) Gen_shift_values_BG1_Z_160;
        break;

      case 176: Gen_shift_values=(short *) Gen_shift_values_BG1_Z_176;
        break;

      case 192: Gen_shift_values=(short *) Gen_shift_values_BG1_Z_192;
        break;

      case 208: Gen_shift_values=(short *) Gen_shift_values_BG1_Z_208;
        break;

      case 224: Gen_shift_values=(short *) Gen_shift_values_BG1_Z_224;
        break;

      case 240: Gen_shift_values=(short *) Gen_shift_values_BG1_Z_240;
        break;

      case 256: Gen_shift_values=(short *) Gen_shift_values_BG1_Z_256;
        break;

      case 288: Gen_shift_values=(short *) Gen_shift_values_BG1_Z_288;
        break;

      case 320: Gen_shift_values=(short *) Gen_shift_values_BG1_Z_320;
        break;

      case 352: Gen_shift_values=(short *) Gen_shift_values_BG1_Z_352;
        break;

      case 384: Gen_shift_values=(short *) Gen_shift_values_BG1_Z_384;
        break;
    }
  }
  else if (BG==2)
  {
    switch (Zc)
    {
      case 2: Gen_shift_values=(short *) Gen_shift_values_BG2_Z_2;
        break;

      case 3: Gen_shift_values=(short *) Gen_shift_values_BG2_Z_3;
        break;

      case 4: Gen_shift_values=(short *) Gen_shift_values_BG2_Z_4;
        break;

      case 5: Gen_shift_values=(short *) Gen_shift_values_BG2_Z_5;
        break;

      case 6: Gen_shift_values=(short *) Gen_shift_values_BG2_Z_6;
        break;

      case 7: Gen_shift_values=(short *) Gen_shift_values_BG2_Z_7;
        break;

      case 8: Gen_shift_values=(short *) Gen_shift_values_BG2_Z_8;
        break;

      case 9: Gen_shift_values=(short *) Gen_shift_values_BG2_Z_9;
        break;

      case 10: Gen_shift_values=(short *) Gen_shift_values_BG2_Z_10;
        break;

      case 11: Gen_shift_values=(short *) Gen_shift_values_BG2_Z_11;
        break;

      case 12: Gen_shift_values=(short *) Gen_shift_values_BG2_Z_12;
        break;

      case 13: Gen_shift_values=(short *) Gen_shift_values_BG2_Z_13;
        break;

      case 14: Gen_shift_values=(short *) Gen_shift_values_BG2_Z_14;
        break;

      case 15: Gen_shift_values=(short *) Gen_shift_values_BG2_Z_15;
        break;

      case 16: Gen_shift_values=(short *) Gen_shift_values_BG2_Z_16;
        break;

      case 18: Gen_shift_values=(short *) Gen_shift_values_BG2_Z_18;
        break;

      case 20: Gen_shift_values=(short *) Gen_shift_values_BG2_Z_20;
        break;

      case 22: Gen_shift_values=(short *) Gen_shift_values_BG2_Z_22;
        break;

      case 24: Gen_shift_values=(short *) Gen_shift_values_BG2_Z_24;
        break;

      case 26: Gen_shift_values=(short *) Gen_shift_values_BG2_Z_26;
        break;

      case 28: Gen_shift_values=(short *) Gen_shift_values_BG2_Z_28;
        break;

      case 30: Gen_shift_values=(short *) Gen_shift_values_BG2_Z_30;
        break;

      case 32: Gen_shift_values=(short *) Gen_shift_values_BG2_Z_32;
        break;

      case 36: Gen_shift_values=(short *) Gen_shift_values_BG2_Z_36;
        break;

      case 40: Gen_shift_values=(short *) Gen_shift_values_BG2_Z_40;
        break;

      case 44: Gen_shift_values=(short *) Gen_shift_values_BG2_Z_44;
        break;

      case 48: Gen_shift_values=(short *) Gen_shift_values_BG2_Z_48;
        break;

      case 52: Gen_shift_values=(short *) Gen_shift_values_BG2_Z_52;
        break;

      case 56: Gen_shift_values=(short *) Gen_shift_values_BG2_Z_56;
        break;

      case 60: Gen_shift_values=(short *) Gen_shift_values_BG2_Z_60;
        break;

      case 64: Gen_shift_values=(short *) Gen_shift_values_BG2_Z_64;
        break;

      case 72: Gen_shift_values=(short *) Gen_shift_values_BG2_Z_72;
        break;

      case 80: Gen_shift_values=(short *) Gen_shift_values_BG2_Z_80;
        break;

      case 88: Gen_shift_values=(short *) Gen_shift_values_BG2_Z_88;
        break;

      case 96: Gen_shift_values=(short *) Gen_shift_values_BG2_Z_96;
        break;

      case 104: Gen_shift_values=(short *) Gen_shift_values_BG2_Z_104;
        break;

      case 112: Gen_shift_values=(short *) Gen_shift_values_BG2_Z_112;
        break;

      case 120: Gen_shift_values=(short *) Gen_shift_values_BG2_Z_120;
        break;

      case 128: Gen_shift_values=(short *) Gen_shift_values_BG2_Z_128;
        break;

      case 144: Gen_shift_values=(short *) Gen_shift_values_BG2_Z_144;
        break;

      case 160: Gen_shift_values=(short *) Gen_shift_values_BG2_Z_160;
        break;

      case 176: Gen_shift_values=(short *) Gen_shift_values_BG2_Z_176;
        break;

      case 192: Gen_shift_values=(short *) Gen_shift_values_BG2_Z_192;
        break;

      case 208: Gen_shift_values=(short *) Gen_shift_values_BG2_Z_208;
        break;

      case 224: Gen_shift_values=(short *) Gen_shift_values_BG2_Z_224;
        break;

      case 240: Gen_shift_values=(short *) Gen_shift_values_BG2_Z_240;
        break;

      case 256: Gen_shift_values=(short *) Gen_shift_values_BG2_Z_256;
        break;

      case 288: Gen_shift_values=(short *) Gen_shift_values_BG2_Z_288;
        break;

      case 320: Gen_shift_values=(short *) Gen_shift_values_BG2_Z_320;
        break;

      case 352: Gen_shift_values=(short *) Gen_shift_values_BG2_Z_352;
        break;

      case 384: Gen_shift_values=(short *) Gen_shift_values_BG2_Z_384;
        break;
    }
  }

  return Gen_shift_values;
}

int ldpc_encoder_orig(unsigned char *test_input,unsigned char *channel_input,short block_length, short BG,unsigned char gen_code)
{
    int result = 0;

#ifndef _WINDOWS
  unsigned char c[22*384]; //padded input, unpacked, max size
  unsigned char d[68*384]; //coded output, unpacked, max size
#else
    unsigned char * c = (unsigned char *) malloc(22 * 384 * sizeof(unsigned char)); //padded input, unpacked, max size
    unsigned char * d = (unsigned char *) malloc(68 * 384 * sizeof(unsigned char)); //coded output, unpacked, max size

    if (c == NULL || d == NULL)
    {
        result = -2;
        goto cleanup_and_return;
    }
#endif

  unsigned char channel_temp,temp;
  short *Gen_shift_values, *no_shift_values, *pointer_shift_values;
  short Zc;
  //initialize for BG == 1
  short Kb = 22;
  short nrows = 46;//parity check bits
  short ncols = 22;//info bits


  int i,i1,i2,i3,i4,i5,temp_prime,var;
  int no_punctured_columns,removed_bit;
  //Table of possible lifting sizes
  short lift_size[51]= {2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,18,20,22,24,26,28,30,32,36,40,44,48,52,56,60,64,72,80,88,96,104,112,120,128,144,160,176,192,208,224,240,256,288,320,352,384};

  int nind=0;
  int indlist[1000];
  int indlist2[1000];

  //determine number of bits in codeword
  //if (block_length>3840)
     if (BG==1)
       {
         //BG=1;
         Kb = 22;
         nrows=46; //parity check bits
         ncols=22; //info bits
       }
       //else if (block_length<=3840)
      else if	(BG==2)
       {
         //BG=2;
         nrows=42; //parity check bits
         ncols=10; // info bits

         if (block_length>640)
   	Kb = 10;
         else if (block_length>560)
   	Kb = 9;
         else if (block_length>192)
         Kb = 8;
       else
         Kb = 6;
         }

  //find minimum value in all sets of lifting size
  Zc=0;
  for (i1=0; i1 < 51; i1++)
  {
    if (lift_size[i1] >= (double) block_length/Kb)
    {
      Zc = lift_size[i1];
      //printf("%d\n",Zc);
      break;
    }
  }
  if (Zc==0) {
    printf("ldpc_encoder_orig: could not determine lifting size\n");
    result = -1;
    goto cleanup_and_return;
  }

  Gen_shift_values=choose_generator_matrix(BG,Zc);
  if (Gen_shift_values==NULL) {
    printf("ldpc_encoder_orig: could not find generator matrix\n");
    result = -1;
    goto cleanup_and_return;
  }

  //printf("ldpc_encoder_orig: BG %d, Zc %d, Kb %d\n",BG, Zc, Kb);

  // load base graph of generator matrix
  if (BG==1)
  {
    no_shift_values=(short *) no_shift_values_BG1;
    pointer_shift_values=(short *) pointer_shift_values_BG1;
  }
  else if (BG==2)
  {
    no_shift_values=(short *) no_shift_values_BG2;
    pointer_shift_values=(short *) pointer_shift_values_BG2;
  }
  else {
	  AssertFatal(0,"BG %d is not supported yet\n",BG);
  }

  no_punctured_columns=(int)((nrows-2)*Zc+block_length-block_length*3)/Zc;
  removed_bit=(nrows-no_punctured_columns-2) * Zc+block_length-(block_length*3);
  //printf("%d\n",no_punctured_columns);
  //printf("%d\n",removed_bit);
  // unpack input
  memset(c,0,sizeof(unsigned char) * ncols * Zc);
  memset(d,0,sizeof(unsigned char) * nrows * Zc);

  for (i=0; i<block_length; i++)
  {
    //c[i] = test_input[i/8]<<(i%8);
    //c[i]=c[i]>>7&1;
    c[i]=(test_input[i/8]&(1<<(i&7)))>>(i&7);
  }

  // parity check part

  if (gen_code==1)
  {
    char fname[100];
    sprintf(fname,"ldpc_BG%d_Zc%d_byte.c",BG,Zc);
    FILE *fd=fopen(fname,"w");
    AssertFatal(fd!=NULL,"cannot open %s\n",fname);
    sprintf(fname,"ldpc_BG%d_Zc%d_16bit.c",BG,Zc);
    FILE *fd2=fopen(fname,"w");
    AssertFatal(fd2!=NULL,"cannot open %s\n",fname);

    int shift;
    char data_type[100];
    char xor_command[100];
    int mask;




    fprintf(fd,"#include \"PHY/sse_intrin.h\"\n");
    fprintf(fd2,"#include \"PHY/sse_intrin.h\"\n");

    if ((Zc&31)==0) {
      shift=5; // AVX2 - 256-bit SIMD
      mask=31;
      strcpy(data_type,"__m256i");
      strcpy(xor_command,"_mm256_xor_si256");
    }
    else if ((Zc&15)==0) {
      shift=4; // SSE4 - 128-bit SIMD
      mask=15;
      strcpy(data_type,"__m128i");
      strcpy(xor_command,"_mm_xor_si128");

    }
    else if ((Zc&7)==0) {
      shift=3; // MMX  - 64-bit SIMD
      mask=7;
      strcpy(data_type,"__m64");
      strcpy(xor_command,"_mm_xor_si64");
    }
    else {
      shift=0;                 // no SIMD
      mask=0;
      strcpy(data_type,"uint8_t");
      strcpy(xor_command,"scalar_xor");
      fprintf(fd,"#define scalar_xor(a,b) ((a)^(b))\n");
      fprintf(fd2,"#define scalar_xor(a,b) ((a)^(b))\n");
    }
    fprintf(fd,"// generated code for Zc=%d, byte encoding\n",Zc);
    fprintf(fd2,"// generated code for Zc=%d, 16bit encoding\n",Zc);
    fprintf(fd,"static inline void ldpc_BG%d_Zc%d_byte(uint8_t *c,uint8_t *d) {\n",BG,Zc);
    fprintf(fd2,"static inline void ldpc_BG%d_Zc%d_16bit(uint16_t *c,uint16_t *d) {\n",BG,Zc);
    fprintf(fd,"  %s *csimd=(%s *)c,*dsimd=(%s *)d;\n\n",data_type,data_type,data_type);
    fprintf(fd2,"  %s *csimd=(%s *)c,*dsimd=(%s *)d;\n\n",data_type,data_type,data_type);
    fprintf(fd,"  %s *c2,*d2;\n\n",data_type);
    fprintf(fd2,"  %s *c2,*d2;\n\n",data_type);
    fprintf(fd,"  int i2;\n");
    fprintf(fd2,"  int i2;\n");
    fprintf(fd,"  for (i2=0; i2<%d; i2++) {\n",Zc>>shift);
    fprintf(fd2,"  for (i2=0; i2<%d; i2++) {\n",Zc>>(shift-1));
    for (i2=0; i2 < 1; i2++)
    {
      //t=Kb*Zc+i2;

      // calculate each row in base graph


      fprintf(fd,"     c2=&csimd[i2];\n");
      fprintf(fd,"     d2=&dsimd[i2];\n");
      fprintf(fd2,"     c2=&csimd[i2];\n");
      fprintf(fd2,"     d2=&dsimd[i2];\n");

      for (i1=0; i1 < nrows; i1++)

      {
        channel_temp=0;
        fprintf(fd,"\n//row: %d\n",i1);
        fprintf(fd2,"\n//row: %d\n",i1);
	fprintf(fd,"     d2[%d]=",(Zc*i1)>>shift);
	fprintf(fd2,"     d2[%d]=",(Zc*i1)>>(shift-1));

        nind=0;

        for (i3=0; i3 < ncols; i3++)
        {
          temp_prime=i1 * ncols + i3;


	  for (i4=0; i4 < no_shift_values[temp_prime]; i4++)
	    {

	      var=(int)((i3*Zc + (Gen_shift_values[ pointer_shift_values[temp_prime]+i4 ]+1)%Zc)/Zc);
	      int index =var*2*Zc + (i3*Zc + (Gen_shift_values[ pointer_shift_values[temp_prime]+i4 ]+1)%Zc) % Zc;

	      indlist[nind] = ((index&mask)*((2*Zc)>>shift)*Kb)+(index>>shift);
	      indlist2[nind++] = ((index&(mask>>1))*((2*Zc)>>(shift-1))*Kb)+(index>>(shift-1));

	    }


        }
	for (i4=0;i4<nind-1;i4++) {
	  fprintf(fd,"%s(c2[%d],",xor_command,indlist[i4]);
	  fprintf(fd2,"%s(c2[%d],",xor_command,indlist2[i4]);
	}
	fprintf(fd,"c2[%d]",indlist[i4]);
	fprintf(fd2,"c2[%d]",indlist2[i4]);
	for (i4=0;i4<nind-1;i4++) { fprintf(fd,")"); fprintf(fd2,")"); }
	fprintf(fd,";\n");
	fprintf(fd2,";\n");

      }
      fprintf(fd,"  }\n}\n");
      fprintf(fd2,"  }\n}\n");
    }
    fclose(fd);
    fclose(fd2);
  }
  else if(gen_code==0)
  {
    for (i2=0; i2 < Zc; i2++)
    {
      //t=Kb*Zc+i2;

      //rotate matrix here
      for (i5=0; i5 < Kb; i5++)
      {
        temp = c[i5*Zc];
        memmove(&c[i5*Zc], &c[i5*Zc+1], (Zc-1)*sizeof(unsigned char));
        c[i5*Zc+Zc-1] = temp;
      }

      // calculate each row in base graph
      for (i1=0; i1 < nrows-no_punctured_columns; i1++)
      {
        channel_temp=0;

        for (i3=0; i3 < Kb; i3++)
        {
          temp_prime=i1 * ncols + i3;

          for (i4=0; i4 < no_shift_values[temp_prime]; i4++)
          {
            channel_temp = channel_temp ^ c[ i3*Zc + Gen_shift_values[ pointer_shift_values[temp_prime]+i4 ] ];
          }
        }

        d[i2+i1*Zc]=channel_temp;
        //channel_input[t+i1*Zc]=channel_temp;
      }
    }
  }

  // information part and puncture columns
  memcpy(&channel_input[0], &c[2*Zc], (block_length-2*Zc)*sizeof(unsigned char));
  memcpy(&channel_input[block_length-2*Zc], &d[0], ((nrows-no_punctured_columns) * Zc-removed_bit)*sizeof(unsigned char));
  //memcpy(channel_input,c,Kb*Zc*sizeof(unsigned char));

cleanup_and_return:
#ifdef _WINDOWS
  if (c != NULL) free(c);
  if (d != NULL) free(d);
#endif

  return result;
}

#ifdef _WINDOWS
EncoderInfo ldpc_encoder_orig_full(unsigned char* test_input, unsigned char* channel_input, short block_length, short BG)
{
    EncoderInfo result = { .Start = -1, .Count = -1, .Length = 0 };

    static __declspec(thread) unsigned char* c = NULL;
    static __declspec(thread) unsigned char* d = NULL;

    if (c == NULL) c = (unsigned char*)malloc(22 * 384 * sizeof(unsigned char)); //padded input, unpacked, max size
    if (d == NULL) d = (unsigned char*)malloc(68 * 384 * sizeof(unsigned char)); //coded output, unpacked, max size

    if (c == NULL || d == NULL)
    {
        goto cleanup_and_return;
    }

    unsigned char channel_temp, temp;
    short* Gen_shift_values, * no_shift_values, * pointer_shift_values;
    short Zc;
    //initialize for BG == 1
    short Kb = 22;
    short nrows = 46;//parity check bits
    short ncols = 22;//info bits


    int i, i1, i2, i3, i4, i5, temp_prime, var;
    int no_punctured_columns, removed_bit;
    //Table of possible lifting sizes
    short lift_size[51] = { 2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,18,20,22,24,26,28,30,32,36,40,44,48,52,56,60,64,72,80,88,96,104,112,120,128,144,160,176,192,208,224,240,256,288,320,352,384 };

    int nind = 0;
    int indlist[1000];
    int indlist2[1000];

    //determine number of bits in codeword
    //if (block_length>3840)
    if (BG == 1)
    {
        //BG=1;
        Kb = 22;
        nrows = 46; //parity check bits
        ncols = 22; //info bits
    }
    //else if (block_length<=3840)
    else if (BG == 2)
    {
        //BG=2;
        nrows = 42; //parity check bits
        ncols = 10; // info bits

        if (block_length > 640)
            Kb = 10;
        else if (block_length > 560)
            Kb = 9;
        else if (block_length > 192)
            Kb = 8;
        else
            Kb = 6;
    }

    //find minimum value in all sets of lifting size
    Zc = 0;
    for (i1 = 0; i1 < 51; i1++)
    {
        if (lift_size[i1] >= (double)block_length / Kb)
        {
            Zc = lift_size[i1];
            //printf("%d\n",Zc);
            break;
        }
    }
    if (Zc == 0) {
        printf("ldpc_encoder_orig: could not determine lifting size\n");
        goto cleanup_and_return;
    }

    Gen_shift_values = choose_generator_matrix(BG, Zc);
    if (Gen_shift_values == NULL) {
        printf("ldpc_encoder_orig: could not find generator matrix\n");
        goto cleanup_and_return;
    }

    //printf("ldpc_encoder_orig: BG %d, Zc %d, Kb %d\n",BG, Zc, Kb);

    // load base graph of generator matrix
    if (BG == 1)
    {
        no_shift_values = (short*)no_shift_values_BG1;
        pointer_shift_values = (short*)pointer_shift_values_BG1;
    }
    else if (BG == 2)
    {
        no_shift_values = (short*)no_shift_values_BG2;
        pointer_shift_values = (short*)pointer_shift_values_BG2;
    }
    else {
        AssertFatal(0, "BG %d is not supported yet\n", BG);
    }

    no_punctured_columns = (int)((nrows - 2) * Zc + block_length - block_length * 3) / Zc;
    removed_bit = (nrows - no_punctured_columns - 2) * Zc + block_length - (block_length * 3);
    //printf("%d\n",no_punctured_columns);
    //printf("%d\n",removed_bit);
    // unpack input
    memset(c, 0, sizeof(unsigned char) * ncols * Zc);
    memset(d, 0, sizeof(unsigned char) * nrows * Zc);

    for (i = 0; i < block_length; i++)
    {
        //c[i] = test_input[i/8]<<(i%8);
        //c[i]=c[i]>>7&1;
        c[i] = (test_input[i / 8] & (1 << (i & 7))) >> (i & 7);
    }

    // parity check part

    int max_d_index = 0;
    for (i2 = 0; i2 < Zc; i2++)
    {
        //t=Kb*Zc+i2;

        //rotate matrix here
        for (i5 = 0; i5 < Kb; i5++)
        {
            temp = c[i5 * Zc];
            memmove(&c[i5 * Zc], &c[i5 * Zc + 1], (Zc - 1) * sizeof(unsigned char));
            c[i5 * Zc + Zc - 1] = temp;
        }

        // calculate each row in base graph
        for (i1 = 0; i1 < nrows - no_punctured_columns; i1++)
        {
            channel_temp = 0;

            for (i3 = 0; i3 < Kb; i3++)
            {
                temp_prime = i1 * ncols + i3;

                for (i4 = 0; i4 < no_shift_values[temp_prime]; i4++)
                {
                    channel_temp = channel_temp ^ c[i3 * Zc + Gen_shift_values[pointer_shift_values[temp_prime] + i4]];
                }
            }

            max_d_index = i2 + i1 * Zc;
            d[max_d_index] = channel_temp;
            //channel_input[t+i1*Zc]=channel_temp;
        }
    }

    // information part and puncture columns
    memcpy(&channel_input[0], c, block_length * sizeof(unsigned char));
    memcpy(&channel_input[block_length], d, (1 + max_d_index) * sizeof(unsigned char));

    result.Start = 2 * Zc;
    result.Count = 
        (block_length - 2 * Zc) * sizeof(unsigned char) +
        ((nrows - no_punctured_columns) * Zc - removed_bit) * sizeof(unsigned char);
    result.Length = (block_length + max_d_index + 1) * sizeof(unsigned char);

cleanup_and_return:
    return result;
}
#endif
