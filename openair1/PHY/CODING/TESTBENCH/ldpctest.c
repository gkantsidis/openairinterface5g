/*
 * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The OpenAirInterface Software Alliance licenses this file to You under
 * the OAI Public License, Version 1.0  (the "License"); you may not use this file
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

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "SIMULATION/TOOLS/defs.h"



// 4-bit quantizer
char quantize4bit(double D,double x)
{
  double qxd;
  qxd = floor(x/D);
  //  printf("x=%f,qxd=%f\n",x,qxd);

  if (qxd <= -8)
    qxd = -8;
  else if (qxd > 7)
    qxd = 7;

  return((char)qxd);
}

char quantize(double D,double x,unsigned char B)
{
  double qxd;
  char maxlev;
  qxd = floor(x/D);
  //    printf("x=%f,qxd=%f\n",x,qxd);
  maxlev = 1<<(B-1);

  if (qxd <= -maxlev)
    qxd = -maxlev;
  else if (qxd >= maxlev)
    qxd = maxlev-1;

  return((char)qxd);
}

#define MAX_BLOCK_LENGTH 8448

int test_ldpc(short No_iteration,
              int nom_rate,
              int denom_rate,
              double SNR,
              unsigned char qbits,
              short block_length,
              unsigned int ntrials,
              unsigned int *errors,
              unsigned int *crc_misses)
{
  //clock initiate
  time_stats_t time,time_optim,tinput,tprep,tparity,toutput;
  opp_enabled=1;
  cpu_freq_GHz = get_cpu_freq_GHz();
  //short test_input[block_length];
  unsigned char *test_input;
  //short *c; //padded codeword
  short *esimated_output;
  unsigned char *channel_input;
  unsigned char *channel_input_optim;
  double *channel_output;
  double *modulated_input;
  short *channel_output_fixed;
  unsigned int i,trial=0;
  short BG,Zc,Kb,nrows,ncols;
  int no_punctured_columns,removed_bit;
  int i1;
  //Table of possible lifting sizes
  short lift_size[51]= {2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,18,20,22,24,26,28,30,32,36,40,44,48,52,56,60,64,72,80,88,96,104,112,120,128,144,160,176,192,208,224,240,256,288,320,352,384};
  *errors=0;
  *crc_misses=0;
  // generate input block
  test_input=(unsigned char *)malloc(sizeof(unsigned char) * block_length/8);
  channel_input = (unsigned char *)malloc(sizeof(unsigned char) * 68*384);
  channel_input_optim = (unsigned char *)malloc(sizeof(unsigned char) * 68*384);
  modulated_input = (double *)malloc(sizeof(double) * 68*384);
  channel_output  = (double *)malloc(sizeof(double) * 68*384);
  reset_meas(&time);
  reset_meas(&time_optim);
  reset_meas(&tinput);
  reset_meas(&tprep);
  reset_meas(&tparity);
  reset_meas(&toutput);
  for (i=0; i<block_length/8; i++)
  {
    //test_input[i]=(unsigned char) rand();
    test_input[i]=217;
  }

  //determine number of bits in codeword
  //if (block_length>3840)
  {
    BG=1;
    Kb = 22;
    nrows=46; //parity check bits
    ncols=22; //info bits
  }
  /*else if (block_length<=3840)
  {
    BG=2;
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
      }*/

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

  printf("ldpc_test: BG %d, Zc %d, Kb %d\n",BG, Zc, Kb);
  no_punctured_columns=(int)((nrows-2)*Zc+block_length-block_length*(1/((float)nom_rate/(float)denom_rate)))/Zc;
  //  printf("puncture:%d\n",no_punctured_columns);
  removed_bit=(nrows-no_punctured_columns-2) * Zc+block_length-(int)(block_length/((float)nom_rate/(float)denom_rate));
  if (ntrials==0)
    ldpc_encoder_orig(test_input,channel_input, block_length, nom_rate, denom_rate, 1);
  
  for (trial=0; trial < ntrials; trial++)
  {

    //// encoder
    start_meas(&time);

    //if (BG==1) 
    //ldpc_encoder(test_input, channel_input,block_length,nom_rate,denom_rate);
    //else
    ldpc_encoder_orig(test_input, channel_input,block_length,nom_rate,denom_rate,0);
    
    stop_meas(&time);
    start_meas(&time_optim);
    ldpc_encoder_optim(test_input,channel_input_optim,block_length,nom_rate,denom_rate,&tinput,&tprep,&tparity,&toutput);
    stop_meas(&time_optim);
    
    if (ntrials==1)    
      for (i = 0; i < block_length+(nrows-no_punctured_columns) * Zc - removed_bit; i++)
	if (channel_input[i]!=channel_input_optim[i]) printf("differ in pos %d (%d,%d)\n",i,
							     channel_input[i],
							     channel_input_optim[i]);
    //print_meas_now(&time, "", stdout);

   // for (i=0;i<6400;i++)
    //printf("channel_input[%d]=%d\n",i,channel_input[i]);
    //printf("%d ",channel_input[i]);

    if ((BG==2) && (Zc==128||Zc==256))
    {
      channel_output_fixed  = (short *)malloc( (Kb+nrows) * Zc*sizeof(short));
      memset(channel_output_fixed,0,(Kb+nrows) * Zc*sizeof(short));
      removed_bit=(nrows-no_punctured_columns-2) * Zc+block_length-(int)(block_length/((float)nom_rate/(float)denom_rate));
      //printf("removed_bit:%d\n",removed_bit);

      for (i = 2*Zc; i < (Kb+nrows-no_punctured_columns) * Zc-removed_bit; i++)
      {
#ifdef DEBUG_CODER

        if ((i&0xf)==0)
          printf("\ne %d..%d:    ",i,i+15);

#endif

        if (channel_input[i-2*Zc]==0)
          modulated_input[i]=1/sqrt(2);  //QPSK
        else
          modulated_input[i]=-1/sqrt(2);

        channel_output[i] = modulated_input[i] + gaussdouble(0.0,1.0) * 1/sqrt(2*SNR);
        channel_output_fixed[i] = (short) ((channel_output[i]*128)<0?(channel_output[i]*128-0.5):(channel_output[i]*128+0.5)); //fixed point 9-7
      }

      //for (i=(Kb+nrows) * Zc-5;i<(Kb+nrows) * Zc;i++)
      //{
      //  printf("channel_input[%d]=%d\n",i,channel_input[i]);
      //printf("%lf %d\n",channel_output[i], channel_output_fixed[i]);
      //printf("v[%d]=%lf\n",i,modulated_input[i]);}
#ifdef DEBUG_CODER
      printf("\n");
      exit(-1);
#endif
      // decode the sequence
      // decoder supports BG2, Z=128 & 256
      esimated_output=ldpc_decoder(channel_output_fixed, block_length, No_iteration, (double)((float)nom_rate/(float)denom_rate));

      //for (i=(Kb+nrows) * Zc-5;i<(Kb+nrows) * Zc;i++)
      //  printf("esimated_output[%d]=%d\n",i,esimated_output[i]);

      //count errors
      for (i=2*Zc; i<(Kb+nrows-no_punctured_columns) * Zc-removed_bit; i++)
      {
        if (esimated_output[i] != channel_input[i-2*Zc])
        {
          *errors = (*errors) + 1;
          break;
        }
      }

      free(channel_output_fixed);
    }
    else if (trial==0)
      printf("decoder is not supported\n");
  }

  print_meas(&time,"ldpc_encoder",NULL,NULL);
  print_meas(&time_optim,"ldpc_encoder_optim",NULL,NULL);
  print_meas(&tinput,"ldpc_encoder_optim(input)",NULL,NULL);
  print_meas(&tprep,"ldpc_encoder_optim(prep)",NULL,NULL);
  print_meas(&tparity,"ldpc_encoder_optim(parity)",NULL,NULL);
  print_meas(&toutput,"ldpc_encoder_optim(output)",NULL,NULL);
  return *errors;
}

int main(int argc, char *argv[])
{
  unsigned int errors,crc_misses;
  short block_length=576; // decoder supports length: 1201 -> 1280, 2401 -> 2560
  short No_iteration=25;
  //double rate=0.333;
  int nom_rate=1;
  int denom_rate=3;
  double SNR,SNR_lin;
  unsigned char qbits=4;
  unsigned int decoded_errors[100]; // initiate the size of matrix equivalent to size of SNR
  int c,i=0;

  int n_trials = 1;

  randominit(0);

  while ((c = getopt (argc, argv, "q:r:s:l:n:")) != -1)
    switch (c)
    {
      case 'q':
        qbits = atoi(optarg);
        break;

      case 'r':
        nom_rate = atoi(optarg);
        break;

      case 's':
        denom_rate = atoi(optarg);
        break;

      case 'l':
        block_length = atoi(optarg);
        break;

      case 'n':
        n_trials = atoi(optarg);
        break;

      default:
        abort ();
    }

  printf("the decoder supports BG2, Kb=10, Z=128 & 256\n");
  printf(" range of blocklength: 1201 -> 1280, 2401 -> 2560\n");
  printf("block length %d: \n", block_length);
  printf("rate: %d/%d\n",nom_rate,denom_rate);

  for (SNR=-2.1; SNR<-2; SNR+=.1)
  {
    SNR_lin = pow(10,SNR/10);
    decoded_errors[i]=test_ldpc(No_iteration,
                                nom_rate,
                                denom_rate,
                                SNR_lin,   // noise standard deviation
                                qbits,
                                block_length,   // block length bytes
                                n_trials,
                                &errors,
                                &crc_misses);
    printf("SNR %f, BLER %f (%d/%d)\n",SNR,(float)decoded_errors[i]/(float)n_trials,decoded_errors[i],n_trials);
    i=i+1;
  }

  return(0);
}


