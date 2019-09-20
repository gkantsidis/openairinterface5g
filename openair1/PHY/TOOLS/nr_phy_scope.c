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

/* Form definition file generated by fdesign */

#include <stdlib.h>
#include "nr_phy_scope.h"

#define TPUT_WINDOW_LENGTH 100
int otg_enabled;

FL_COLOR rx_antenna_colors[4] = {FL_RED,FL_BLUE,FL_GREEN,FL_YELLOW};

float tput_time_enb[NUMBER_OF_UE_MAX][TPUT_WINDOW_LENGTH] = {{0}};
float tput_enb[NUMBER_OF_UE_MAX][TPUT_WINDOW_LENGTH] = {{0}};
float tput_time_ue[NUMBER_OF_UE_MAX][TPUT_WINDOW_LENGTH] = {{0}};
float tput_ue[NUMBER_OF_UE_MAX][TPUT_WINDOW_LENGTH] = {{0}};
float tput_ue_max[NUMBER_OF_UE_MAX] = {0};


static void ia_receiver_on_off( FL_OBJECT *button, long arg)
{

  if (fl_get_button(button)) {
    fl_set_object_label(button, "IA Receiver ON");
    //    PHY_vars_UE_g[0][0]->use_ia_receiver = 1;
    fl_set_object_color(button, FL_GREEN, FL_GREEN);
  } else {
    fl_set_object_label(button, "IA Receiver OFF");
    //    PHY_vars_UE_g[0][0]->use_ia_receiver = 0;
    fl_set_object_color(button, FL_RED, FL_RED);
  }
}

static void dl_traffic_on_off( FL_OBJECT *button, long arg)
{

  if (fl_get_button(button)) {
    fl_set_object_label(button, "DL Traffic ON");
    otg_enabled = 1;
    fl_set_object_color(button, FL_GREEN, FL_GREEN);
  } else {
    fl_set_object_label(button, "DL Traffic OFF");
    otg_enabled = 0;
    fl_set_object_color(button, FL_RED, FL_RED);
  }
}

FD_phy_scope_gnb *create_phy_scope_gnb( void )
{

  FL_OBJECT *obj;
  FD_phy_scope_gnb *fdui = fl_malloc( sizeof *fdui );

  // Define form
  fdui->phy_scope_gnb = fl_bgn_form( FL_NO_BOX, 800, 800 );

  // This the whole UI box
  obj = fl_add_box( FL_BORDER_BOX, 0, 0, 800, 800, "" );
  fl_set_object_color( obj, FL_BLACK, FL_BLACK );

  // Received signal
  fdui->rxsig_t = fl_add_xyplot( FL_NORMAL_XYPLOT, 20, 20, 370, 100, "Received Signal (Time-Domain, dB)" );
  fl_set_object_boxtype( fdui->rxsig_t, FL_EMBOSSED_BOX );
  fl_set_object_color( fdui->rxsig_t, FL_BLACK, FL_RED );
  fl_set_object_lcolor( fdui->rxsig_t, FL_WHITE ); // Label color
  fl_set_xyplot_ybounds(fdui->rxsig_t,10,70);

  // Time-domain channel response
  fdui->chest_t = fl_add_xyplot( FL_NORMAL_XYPLOT, 410, 20, 370, 100, "SRS Frequency Response (samples, abs)" );
  fl_set_object_boxtype( fdui->chest_t, FL_EMBOSSED_BOX );
  fl_set_object_color( fdui->chest_t, FL_BLACK, FL_RED );
  fl_set_object_lcolor( fdui->chest_t, FL_WHITE ); // Label color

  // Frequency-domain channel response
  fdui->chest_f = fl_add_xyplot( FL_IMPULSE_XYPLOT, 20, 140, 760, 100, "Channel Frequency  Response (RE, dB)" );
  fl_set_object_boxtype( fdui->chest_f, FL_EMBOSSED_BOX );
  fl_set_object_color( fdui->chest_f, FL_BLACK, FL_RED );
  fl_set_object_lcolor( fdui->chest_f, FL_WHITE ); // Label color
  fl_set_xyplot_ybounds( fdui->chest_f,30,70);

  // LLR of PUSCH
  fdui->pusch_llr = fl_add_xyplot( FL_POINTS_XYPLOT, 20, 260, 500, 200, "PUSCH Log-Likelihood Ratios (LLR, mag)" );
  fl_set_object_boxtype( fdui->pusch_llr, FL_EMBOSSED_BOX );
  fl_set_object_color( fdui->pusch_llr, FL_BLACK, FL_YELLOW );
  fl_set_object_lcolor( fdui->pusch_llr, FL_WHITE ); // Label color
  fl_set_xyplot_symbolsize( fdui->pusch_llr,2);

  // I/Q PUSCH comp
  fdui->pusch_comp = fl_add_xyplot( FL_POINTS_XYPLOT, 540, 260, 240, 200, "PUSCH I/Q of MF Output" );
  fl_set_object_boxtype( fdui->pusch_comp, FL_EMBOSSED_BOX );
  fl_set_object_color( fdui->pusch_comp, FL_BLACK, FL_YELLOW );
  fl_set_object_lcolor( fdui->pusch_comp, FL_WHITE ); // Label color
  fl_set_xyplot_symbolsize( fdui->pusch_comp,2);
  fl_set_xyplot_xgrid( fdui->pusch_llr,FL_GRID_MAJOR);

  // I/Q PUCCH comp (format 1)
  fdui->pucch_comp1 = fl_add_xyplot( FL_POINTS_XYPLOT, 540, 480, 240, 100, "PUCCH1 Energy (SR)" );
  fl_set_object_boxtype( fdui->pucch_comp1, FL_EMBOSSED_BOX );
  fl_set_object_color( fdui->pucch_comp1, FL_BLACK, FL_YELLOW );
  fl_set_object_lcolor( fdui->pucch_comp1, FL_WHITE ); // Label color
  fl_set_xyplot_symbolsize( fdui->pucch_comp1,2);
  //  fl_set_xyplot_xgrid( fdui->pusch_llr,FL_GRID_MAJOR);

  // I/Q PUCCH comp (fromat 1a/b)
  fdui->pucch_comp = fl_add_xyplot( FL_POINTS_XYPLOT, 540, 600, 240, 100, "PUCCH I/Q of MF Output" );
  fl_set_object_boxtype( fdui->pucch_comp, FL_EMBOSSED_BOX );
  fl_set_object_color( fdui->pucch_comp, FL_BLACK, FL_YELLOW );
  fl_set_object_lcolor( fdui->pucch_comp, FL_WHITE ); // Label color
  fl_set_xyplot_symbolsize( fdui->pucch_comp,2);
  //  fl_set_xyplot_xgrid( fdui->pusch_llr,FL_GRID_MAJOR);

  // Throughput on PUSCH
  fdui->pusch_tput = fl_add_xyplot( FL_NORMAL_XYPLOT, 20, 480, 500, 100, "PUSCH Throughput [frame]/[kbit/s]" );
  fl_set_object_boxtype( fdui->pusch_tput, FL_EMBOSSED_BOX );
  fl_set_object_color( fdui->pusch_tput, FL_BLACK, FL_WHITE );
  fl_set_object_lcolor( fdui->pusch_tput, FL_WHITE ); // Label color

  // Generic eNB Button
  fdui->button_0 = fl_add_button( FL_PUSH_BUTTON, 20, 600, 240, 40, "" );
  fl_set_object_lalign(fdui->button_0, FL_ALIGN_CENTER );
  fl_set_button(fdui->button_0,0);
  otg_enabled = 0;
  fl_set_object_label(fdui->button_0, "DL Traffic OFF");
  fl_set_object_color(fdui->button_0, FL_RED, FL_RED);
  fl_set_object_callback(fdui->button_0, dl_traffic_on_off, 0 );

  fl_end_form( );
  fdui->phy_scope_gnb->fdui = fdui;

  return fdui;
}

void phy_scope_gNB(FD_phy_scope_gnb *form,
                   PHY_VARS_gNB *phy_vars_gnb,
                   int UE_id)
{
  int i,i2,arx,atx,ind,k;
  NR_DL_FRAME_PARMS *frame_parms = &phy_vars_gnb->frame_parms;
  int nsymb_ce = 12*frame_parms->N_RB_UL*frame_parms->symbols_per_tti;
  uint8_t nb_antennas_rx = frame_parms->nb_antennas_rx;
  uint8_t nb_antennas_tx = 1; // frame_parms->nb_antennas_tx; // in LTE Rel. 8 and 9 only a single transmit antenna is assumed at the UE
  int16_t **rxsig_t;
  int16_t **chest_t=NULL;
  int16_t **chest_f=NULL;
  int16_t *pusch_llr=NULL;
  int32_t *pusch_comp=NULL;
  int32_t *pucch1_comp=NULL;
  int32_t *pucch1_thres=NULL;
  int32_t *pucch1ab_comp=NULL;
  float Re,Im,ymax;
  float *llr, *bit;
  float I[nsymb_ce*2], Q[nsymb_ce*2];
  float I_pucch[10240],Q_pucch[10240],A_pucch[10240],B_pucch[10240],C_pucch[10240];
  float rxsig_t_dB[nb_antennas_rx][FRAME_LENGTH_COMPLEX_SAMPLES];
  float chest_t_abs[nb_antennas_rx][frame_parms->ofdm_symbol_size];
  float *chest_f_abs;
  float time[FRAME_LENGTH_COMPLEX_SAMPLES];
  float time2[2048];
  float freq[nsymb_ce*nb_antennas_rx*nb_antennas_tx];
  uint32_t total_dlsch_bitrate = phy_vars_gnb->total_dlsch_bitrate;
  int coded_bits_per_codeword = 0;
  uint8_t harq_pid; // in TDD config 3 it is sf-2, i.e., can be 0,1,2
  int Qm = 2;

  if (!RC.nrmac[0]->UE_list.active[UE_id])
    return;
  
  // choose max MCS to compute coded_bits_per_codeword
  if (phy_vars_gnb->ulsch[UE_id][0]!=NULL) {
    for (harq_pid=0; harq_pid<3; harq_pid++) {
      //Qm = cmax(phy_vars_gnb->ulsch[UE_id][0]->harq_processes->Qm,Qm);
    }
  }

  coded_bits_per_codeword = frame_parms->N_RB_UL*12*Qm*frame_parms->symbols_per_tti;

  chest_f_abs = (float*) calloc(nsymb_ce*nb_antennas_rx*nb_antennas_tx,sizeof(float));
  llr = (float*) calloc(coded_bits_per_codeword,sizeof(float)); // init to zero
  bit = malloc(coded_bits_per_codeword*sizeof(float));

  rxsig_t = (int16_t**) phy_vars_gnb->common_vars.rxdata;
  //chest_t = (int16_t**) phy_vars_gnb->pusch_vars[UE_id]->drs_ch_estimates_time[eNB_id];
  /*  chest_t = (int16_t**) phy_vars_gnb->srs_vars[UE_id].srs_ch_estimates;
  chest_f = (int16_t**) phy_vars_gnb->pusch_vars[UE_id]->drs_ch_estimates;
  pusch_llr = (int16_t*) phy_vars_gnb->pusch_vars[UE_id]->llr;
  pusch_comp = (int32_t*) phy_vars_gnb->pusch_vars[UE_id]->rxdataF_comp;
  pucch1_comp = (int32_t*) phy_vars_gnb->pucch1_stats[UE_id];
  pucch1_thres = (int32_t*) phy_vars_gnb->pucch1_stats_thres[UE_id];
  pucch1ab_comp = (int32_t*) phy_vars_gnb->pucch1ab_stats[UE_id];
  */

  // Received signal in time domain of receive antenna 0
  if (rxsig_t != NULL) {
    if (rxsig_t[0] != NULL) {
      for (i=0; i<FRAME_LENGTH_COMPLEX_SAMPLES; i++) {
        rxsig_t_dB[0][i] = 10*log10(1.0+(float) ((rxsig_t[0][2*i])*(rxsig_t[0][2*i])+(rxsig_t[0][2*i+1])*(rxsig_t[0][2*i+1])));
        time[i] = (float) i;
      }

      fl_set_xyplot_data(form->rxsig_t,time,rxsig_t_dB[0],FRAME_LENGTH_COMPLEX_SAMPLES,"","","");
    }

    for (arx=1; arx<nb_antennas_rx; arx++) {
      if (rxsig_t[arx] != NULL) {
        for (i=0; i<FRAME_LENGTH_COMPLEX_SAMPLES; i++) {
          rxsig_t_dB[arx][i] = 10*log10(1.0+(float) ((rxsig_t[arx][2*i])*(rxsig_t[arx][2*i])+(rxsig_t[arx][2*i+1])*(rxsig_t[arx][2*i+1])));
        }

        fl_add_xyplot_overlay(form->rxsig_t,arx,time,rxsig_t_dB[arx],FRAME_LENGTH_COMPLEX_SAMPLES,rx_antenna_colors[arx]);
      }
    }
  }

  // Channel Impulse Response
  if (chest_t != NULL) {
    ymax = 0;

    if (chest_t[0] !=NULL) {
      for (i=0; i<(frame_parms->ofdm_symbol_size); i++) {
        //i2 = (i+(frame_parms->ofdm_symbol_size>>1))%frame_parms->ofdm_symbol_size;
	i2=i;
        //time2[i] = (float)(i-(frame_parms->ofdm_symbol_size>>1));
        time2[i] = (float)i;
        chest_t_abs[0][i] = 10*log10((float) (1+chest_t[0][2*i2]*chest_t[0][2*i2]+chest_t[0][2*i2+1]*chest_t[0][2*i2+1]));

        if (chest_t_abs[0][i] > ymax)
          ymax = chest_t_abs[0][i];
      }

      fl_set_xyplot_data(form->chest_t,time2,chest_t_abs[0],(frame_parms->ofdm_symbol_size),"","","");
    }

    for (arx=1; arx<nb_antennas_rx; arx++) {
      if (chest_t[arx] !=NULL) {
        for (i=0; i<(frame_parms->ofdm_symbol_size>>3); i++) {
          chest_t_abs[arx][i] = 10*log10((float) (1+chest_t[arx][2*i]*chest_t[arx][2*i]+chest_t[arx][2*i+1]*chest_t[arx][2*i+1]));

          if (chest_t_abs[arx][i] > ymax)
            ymax = chest_t_abs[arx][i];
        }

        fl_add_xyplot_overlay(form->chest_t,arx,time,chest_t_abs[arx],(frame_parms->ofdm_symbol_size>>3),rx_antenna_colors[arx]);
        fl_set_xyplot_overlay_type(form->chest_t,arx,FL_DASHED_XYPLOT);
      }
    }

    // Avoid flickering effect
    //        fl_get_xyplot_ybounds(form->chest_t,&ymin,&ymax);
    fl_set_xyplot_ybounds(form->chest_t,0,ymax);
  }

  // Channel Frequency Response
  if (chest_f != NULL) {
    ind = 0;

    for (atx=0; atx<nb_antennas_tx; atx++) {
      for (arx=0; arx<nb_antennas_rx; arx++) {
        if (chest_f[(atx<<1)+arx] != NULL) {
          for (k=0; k<nsymb_ce; k++) {
            freq[ind] = (float)ind;
            Re = (float)(chest_f[(atx<<1)+arx][(2*k)]);
            Im = (float)(chest_f[(atx<<1)+arx][(2*k)+1]);

            chest_f_abs[ind] = (short)10*log10(1.0+((double)Re*Re + (double)Im*Im));
            ind++;
          }
        }
      }
    }

    // tx antenna 0
    fl_set_xyplot_xbounds(form->chest_f,0,nb_antennas_rx*nb_antennas_tx*nsymb_ce);
    fl_set_xyplot_xtics(form->chest_f,nb_antennas_rx*nb_antennas_tx*frame_parms->symbols_per_tti,3);
    fl_set_xyplot_xgrid(form->chest_f,FL_GRID_MAJOR);
    fl_set_xyplot_data(form->chest_f,freq,chest_f_abs,nsymb_ce,"","","");

    for (arx=1; arx<nb_antennas_rx; arx++) {
      fl_add_xyplot_overlay(form->chest_f,1,&freq[arx*nsymb_ce],&chest_f_abs[arx*nsymb_ce],nsymb_ce,rx_antenna_colors[arx]);
    }

    // other tx antennas
    if (nb_antennas_tx > 1) {
      if (nb_antennas_rx > 1) {
        for (atx=1; atx<nb_antennas_tx; atx++) {
          for (arx=0; arx<nb_antennas_rx; arx++) {
            fl_add_xyplot_overlay(form->chest_f,(atx<<1)+arx,&freq[((atx<<1)+arx)*nsymb_ce],&chest_f_abs[((atx<<1)+arx)*nsymb_ce],nsymb_ce,rx_antenna_colors[arx]);
          }
        }
      } else { // 1 rx antenna
        atx=1;
        arx=0;
        fl_add_xyplot_overlay(form->chest_f,atx,&freq[atx*nsymb_ce],&chest_f_abs[atx*nsymb_ce],nsymb_ce,rx_antenna_colors[arx]);
      }
    }
  }

  // PUSCH LLRs
  if (pusch_llr != NULL) {
    for (i=0; i<coded_bits_per_codeword; i++) {
      llr[i] = (float) pusch_llr[i];
      bit[i] = (float) i;
    }

    fl_set_xyplot_data(form->pusch_llr,bit,llr,coded_bits_per_codeword,"","","");
  }

  // PUSCH I/Q of MF Output
  if (pusch_comp!=NULL) {
    ind=0;

    for (k=0; k<frame_parms->symbols_per_tti; k++) {
      for (i=0; i<12*frame_parms->N_RB_UL; i++) {
        I[ind] = pusch_comp[(2*frame_parms->N_RB_UL*12*k)+2*i];
        Q[ind] = pusch_comp[(2*frame_parms->N_RB_UL*12*k)+2*i+1];
        ind++;
      }
    }

    fl_set_xyplot_data(form->pusch_comp,I,Q,ind,"","","");
  }

  // PUSCH I/Q of MF Output
  if (pucch1ab_comp!=NULL) {
    for (ind=0; ind<10240; ind++) {

      I_pucch[ind] = (float)pucch1ab_comp[2*(ind)];
      Q_pucch[ind] = (float)pucch1ab_comp[2*(ind)+1];
      A_pucch[ind] = 10*log10(pucch1_comp[ind]);
      B_pucch[ind] = ind;
      C_pucch[ind] = (float)pucch1_thres[ind];
    }
    fl_set_xyplot_data(form->pucch_comp,I_pucch,Q_pucch,10240,"","","");
    fl_set_xyplot_data(form->pucch_comp1,B_pucch,A_pucch,1024,"","","");
    fl_add_xyplot_overlay(form->pucch_comp1,1,B_pucch,C_pucch,1024,FL_RED);
    fl_set_xyplot_ybounds(form->pucch_comp,-5000,5000);
    fl_set_xyplot_xbounds(form->pucch_comp,-5000,5000);

    fl_set_xyplot_ybounds(form->pucch_comp1,0,80);
  }


  // PUSCH Throughput
  memmove( tput_time_enb[UE_id], &tput_time_enb[UE_id][1], (TPUT_WINDOW_LENGTH-1)*sizeof(float) );
  memmove( tput_enb[UE_id], &tput_enb[UE_id][1], (TPUT_WINDOW_LENGTH-1)*sizeof(float) );

  tput_time_enb[UE_id][TPUT_WINDOW_LENGTH-1]  = (float) 0;
  tput_enb[UE_id][TPUT_WINDOW_LENGTH-1] = ((float) total_dlsch_bitrate)/1000.0;

  fl_set_xyplot_data(form->pusch_tput,tput_time_enb[UE_id],tput_enb[UE_id],TPUT_WINDOW_LENGTH,"","","");

  //    fl_get_xyplot_ybounds(form->pusch_tput,&ymin,&ymax);
  //    fl_set_xyplot_ybounds(form->pusch_tput,0,ymax);

  fl_check_forms();

  free(llr);
  free(bit);
  free(chest_f_abs);
}

FD_phy_scope_nrue *create_phy_scope_nrue( void )
{

  FL_OBJECT *obj;
  FD_phy_scope_nrue *fdui = fl_malloc( sizeof *fdui );

  // Define form
  fdui->phy_scope_nrue = fl_bgn_form( FL_NO_BOX, 800, 900 );

  // This the whole UI box
  obj = fl_add_box( FL_BORDER_BOX, 0, 0, 800, 900, "" );
  fl_set_object_color( obj, FL_BLACK, FL_BLACK );

  // Received signal
  fdui->rxsig_t = fl_add_xyplot( FL_NORMAL_XYPLOT, 20, 20, 370, 100, "Received Signal (Time-Domain, dB)" );
  fl_set_object_boxtype( fdui->rxsig_t, FL_EMBOSSED_BOX );
  fl_set_object_color( fdui->rxsig_t, FL_BLACK, FL_RED );
  fl_set_object_lcolor( fdui->rxsig_t, FL_WHITE ); // Label color
  fl_set_xyplot_ybounds(fdui->rxsig_t,10,70);

  // Time-domain channel response
  fdui->chest_t = fl_add_xyplot( FL_NORMAL_XYPLOT, 410, 20, 370, 100, "Channel Impulse Response (samples, abs)" );
  fl_set_object_boxtype( fdui->chest_t, FL_EMBOSSED_BOX );
  fl_set_object_color( fdui->chest_t, FL_BLACK, FL_RED );
  fl_set_object_lcolor( fdui->chest_t, FL_WHITE ); // Label color

  // Frequency-domain channel response
  fdui->chest_f = fl_add_xyplot( FL_IMPULSE_XYPLOT, 20, 140, 760, 100, "Channel Frequency Response (RE, dB)" );
  fl_set_object_boxtype( fdui->chest_f, FL_EMBOSSED_BOX );
  fl_set_object_color( fdui->chest_f, FL_BLACK, FL_RED );
  fl_set_object_lcolor( fdui->chest_f, FL_WHITE ); // Label color
  fl_set_xyplot_ybounds( fdui->chest_f,30,70);

  // LLR of PBCH
  fdui->pbch_llr = fl_add_xyplot( FL_POINTS_XYPLOT, 20, 260, 500, 100, "PBCH Log-Likelihood Ratios (LLR, mag)" );
  fl_set_object_boxtype( fdui->pbch_llr, FL_EMBOSSED_BOX );
  fl_set_object_color( fdui->pbch_llr, FL_BLACK, FL_GREEN );
  fl_set_object_lcolor( fdui->pbch_llr, FL_WHITE ); // Label color
  fl_set_xyplot_symbolsize( fdui->pbch_llr,2);
  fl_set_xyplot_xgrid( fdui->pbch_llr,FL_GRID_MAJOR);
  //fl_set_xyplot_xbounds( fdui->pbch_llr,0,1920);

  // I/Q PBCH comp
  fdui->pbch_comp = fl_add_xyplot( FL_POINTS_XYPLOT, 540, 260, 240, 100, "PBCH I/Q of MF Output" );
  fl_set_object_boxtype( fdui->pbch_comp, FL_EMBOSSED_BOX );
  fl_set_object_color( fdui->pbch_comp, FL_BLACK, FL_GREEN );
  fl_set_object_lcolor( fdui->pbch_comp, FL_WHITE ); // Label color
  fl_set_xyplot_symbolsize( fdui->pbch_comp,2);
  //  fl_set_xyplot_xbounds( fdui->pbch_comp,-100,100);
  //  fl_set_xyplot_ybounds( fdui->pbch_comp,-100,100);

  // LLR of PDCCH
  fdui->pdcch_llr = fl_add_xyplot( FL_POINTS_XYPLOT, 20, 380, 500, 100, "PDCCH Log-Likelihood Ratios (LLR, mag)" );
  fl_set_object_boxtype( fdui->pdcch_llr, FL_EMBOSSED_BOX );
  fl_set_object_color( fdui->pdcch_llr, FL_BLACK, FL_CYAN );
  fl_set_object_lcolor( fdui->pdcch_llr, FL_WHITE ); // Label color
  fl_set_xyplot_symbolsize( fdui->pdcch_llr,2);

  // I/Q PDCCH comp
  fdui->pdcch_comp = fl_add_xyplot( FL_POINTS_XYPLOT, 540, 380, 240, 100, "PDCCH I/Q of MF Output" );
  fl_set_object_boxtype( fdui->pdcch_comp, FL_EMBOSSED_BOX );
  fl_set_object_color( fdui->pdcch_comp, FL_BLACK, FL_CYAN );
  fl_set_object_lcolor( fdui->pdcch_comp, FL_WHITE ); // Label color
  fl_set_xyplot_symbolsize( fdui->pdcch_comp,2);
  fl_set_xyplot_xgrid( fdui->pdcch_llr,FL_GRID_MAJOR);

  // LLR of PDSCH
  fdui->pdsch_llr = fl_add_xyplot( FL_POINTS_XYPLOT, 20, 500, 500, 200, "PDSCH Log-Likelihood Ratios (LLR, mag)" );
  fl_set_object_boxtype( fdui->pdsch_llr, FL_EMBOSSED_BOX );
  fl_set_object_color( fdui->pdsch_llr, FL_BLACK, FL_YELLOW );
  fl_set_object_lcolor( fdui->pdsch_llr, FL_WHITE ); // Label color
  fl_set_xyplot_symbolsize( fdui->pdsch_llr,2);
  fl_set_xyplot_xgrid( fdui->pdsch_llr,FL_GRID_MAJOR);

  // I/Q PDSCH comp
  fdui->pdsch_comp = fl_add_xyplot( FL_POINTS_XYPLOT, 540, 500, 240, 200, "PDSCH I/Q of MF Output" );
  fl_set_object_boxtype( fdui->pdsch_comp, FL_EMBOSSED_BOX );
  fl_set_object_color( fdui->pdsch_comp, FL_BLACK, FL_YELLOW );
  fl_set_object_lcolor( fdui->pdsch_comp, FL_WHITE ); // Label color
  fl_set_xyplot_symbolsize( fdui->pdsch_comp,2);

  // Throughput on PDSCH
  fdui->pdsch_tput = fl_add_xyplot( FL_NORMAL_XYPLOT, 20, 720, 500, 100, "PDSCH Throughput [frame]/[kbit/s]" );
  fl_set_object_boxtype( fdui->pdsch_tput, FL_EMBOSSED_BOX );
  fl_set_object_color( fdui->pdsch_tput, FL_BLACK, FL_WHITE );
  fl_set_object_lcolor( fdui->pdsch_tput, FL_WHITE ); // Label color

  // Generic UE Button
  fdui->button_0 = fl_add_button( FL_PUSH_BUTTON, 540, 720, 240, 40, "" );
  fl_set_object_lalign(fdui->button_0, FL_ALIGN_CENTER );
  //openair_daq_vars.use_ia_receiver = 0;
  fl_set_button(fdui->button_0,0);
  fl_set_object_label(fdui->button_0, "IA Receiver OFF");
  fl_set_object_color(fdui->button_0, FL_RED, FL_RED);
  fl_set_object_callback(fdui->button_0, ia_receiver_on_off, 0 );
  fl_hide_object(fdui->button_0);

  fl_end_form( );
  fdui->phy_scope_nrue->fdui = fdui;

  return fdui;
}

void phy_scope_nrUE(FD_phy_scope_nrue *form,
		            PHY_VARS_NR_UE *phy_vars_ue,
					int eNB_id,
					int UE_id,
					uint8_t subframe)
{
  int i,arx,atx,ind,k;
  NR_DL_FRAME_PARMS *frame_parms = &phy_vars_ue->frame_parms;
  //int nsymb_ce = frame_parms->ofdm_symbol_size;//*frame_parms->symbols_per_tti;
  int samples_per_frame = frame_parms->samples_per_frame;
  uint8_t nb_antennas_rx = frame_parms->nb_antennas_rx;
  uint8_t nb_antennas_tx = frame_parms->nb_antenna_ports_eNB;
  int16_t **rxsig_t;
  float **rxsig_t_dB;
  float *time;
  float *corr;
  int16_t **chest_t;
  int16_t **chest_f;
  int16_t *pdsch_llr;
  int16_t *pdsch_comp;
  //int16_t *pdsch_mag;
  int8_t  *pdcch_llr;
  int16_t *pdcch_comp;
  int16_t *pbch_llr;
  int16_t *pbch_comp;
  float llr_pbch[1920], bit_pbch[1920];
  float *llr, *bit; 
  float *llr_pdcch, *bit_pdcch;
  float *I, *Q;
  int num_pdcch_symbols=2;
  int num_re = 4500;
  int Qm = 2;
  int coded_bits_per_codeword = num_re*Qm;
  int symbol, first_symbol=2,nb_re;
  int nb_rb_pdsch=50,nb_symb_sch=9;
  float ymax=1;
  float **chest_t_abs;
  float Re,Im;
  float *chest_f_abs;
  float *freq;
  static int overlay = 0;
  /*
  int frame = phy_vars_ue->proc.proc_rxtx[0].frame_rx;
  int mcs = 0;
  unsigned char harq_pid = 0;
  */

  /*
  if (phy_vars_ue->dlsch[phy_vars_ue->current_thread_id[subframe]][eNB_id][0]!=NULL) {
    harq_pid = phy_vars_ue->dlsch[phy_vars_ue->current_thread_id[subframe]][eNB_id][0]->current_harq_pid;

    if (harq_pid>=8)
      return;

    mcs = phy_vars_ue->dlsch[phy_vars_ue->current_thread_id[subframe]][eNB_id][0]->harq_processes[harq_pid]->mcs;

    // Button 0
    if(!phy_vars_ue->dlsch[phy_vars_ue->current_thread_id[subframe]][eNB_id][0]->harq_processes[harq_pid]->dl_power_off) {
      // we are in TM5
      fl_show_object(form->button_0);
    }
  }

  if (phy_vars_ue->pdcch_vars[phy_vars_ue->current_thread_id[subframe]][eNB_id]!=NULL) {
    num_pdcch_symbols = phy_vars_ue->pdcch_vars[phy_vars_ue->current_thread_id[subframe]][eNB_id]->num_pdcch_symbols;
  }

  //    coded_bits_per_codeword = frame_parms->N_RB_DL*12*get_Qm(mcs)*(frame_parms->symbols_per_tti);
  if (phy_vars_ue->dlsch[phy_vars_ue->current_thread_id[subframe]][eNB_id][0]!=NULL) {
    coded_bits_per_codeword = get_G(frame_parms,
                                    phy_vars_ue->dlsch[phy_vars_ue->current_thread_id[subframe]][eNB_id][0]->harq_processes[harq_pid]->nb_rb,
                                    phy_vars_ue->dlsch[phy_vars_ue->current_thread_id[subframe]][eNB_id][0]->harq_processes[harq_pid]->rb_alloc_even,
                                    get_Qm(mcs),
                                    phy_vars_ue->dlsch[phy_vars_ue->current_thread_id[subframe]][eNB_id][0]->harq_processes[harq_pid]->Nl,
                                    num_pdcch_symbols,
                                    frame,
                                    subframe,
                                    beamforming_mode);
  } else {
    coded_bits_per_codeword = 0; //frame_parms->N_RB_DL*12*get_Qm(mcs)*(frame_parms->symbols_per_tti);
  }
  */
  I = (float*) calloc(frame_parms->ofdm_symbol_size*frame_parms->symbols_per_slot*2,sizeof(float));
  Q = (float*) calloc(frame_parms->ofdm_symbol_size*frame_parms->symbols_per_slot*2,sizeof(float));

  chest_t_abs = (float**) malloc(nb_antennas_rx*sizeof(float*));

  for (arx=0; arx<nb_antennas_rx; arx++) {
    chest_t_abs[arx] = (float*) calloc(frame_parms->ofdm_symbol_size,sizeof(float));
  }

  chest_f_abs = (float*) calloc(frame_parms->ofdm_symbol_size,sizeof(float));
  freq = (float*) calloc(frame_parms->ofdm_symbol_size,sizeof(float));

  llr = (float*) calloc(coded_bits_per_codeword,sizeof(float)); // init to zero
  bit = malloc(coded_bits_per_codeword*sizeof(float));

  llr_pdcch = (float*) calloc(12*frame_parms->N_RB_DL*num_pdcch_symbols*2,sizeof(float)); // init to zero
  bit_pdcch = (float*) calloc(12*frame_parms->N_RB_DL*num_pdcch_symbols*2,sizeof(float));

  rxsig_t = (int16_t**) phy_vars_ue->common_vars.rxdata;
  rxsig_t_dB = calloc(nb_antennas_rx,sizeof(float*));
  for (arx=0; arx<nb_antennas_rx; arx++) {
    rxsig_t_dB[arx] = (float*) calloc(samples_per_frame,sizeof(float));
  }
  time = calloc(samples_per_frame,sizeof(float));
  corr = calloc(samples_per_frame,sizeof(float));

  chest_t = (int16_t**) phy_vars_ue->pdcch_vars[phy_vars_ue->current_thread_id[subframe]][eNB_id]->dl_ch_estimates_time;
  chest_f = (int16_t**) phy_vars_ue->pdcch_vars[phy_vars_ue->current_thread_id[subframe]][eNB_id]->dl_ch_estimates;

  pbch_llr = (int16_t*) phy_vars_ue->pbch_vars[eNB_id]->llr;
  pbch_comp = (int16_t*) phy_vars_ue->pbch_vars[eNB_id]->rxdataF_comp[0];

  pdcch_llr = (int8_t*) phy_vars_ue->pdcch_vars[phy_vars_ue->current_thread_id[subframe]][eNB_id]->llr;
  pdcch_comp = (int16_t*) phy_vars_ue->pdcch_vars[phy_vars_ue->current_thread_id[subframe]][eNB_id]->rxdataF_comp[0];
   pdsch_llr = (int16_t*) phy_vars_ue->pdsch_vars[phy_vars_ue->current_thread_id[subframe]][eNB_id]->llr[0]; // stream 0
  //    pdsch_llr = (int16_t*) phy_vars_ue->lte_ue_pdsch_vars_SI[eNB_id]->llr[0]; // stream 0
  pdsch_comp = (int16_t*) phy_vars_ue->pdsch_vars[phy_vars_ue->current_thread_id[subframe]][eNB_id]->rxdataF_comp0[0];
  //pdsch_mag = (int16_t*) phy_vars_ue->pdsch_vars[phy_vars_ue->current_thread_id[subframe]][eNB_id]->dl_ch_mag0[0];

  // Received signal in time domain of receive antenna 0
  if (rxsig_t != NULL) {
    if (rxsig_t[0] != NULL) {
      for (i=0; i<samples_per_frame; i++) {
        rxsig_t_dB[0][i] = 10*log10(1.0+(float) ((rxsig_t[0][2*i])*(rxsig_t[0][2*i])+(rxsig_t[0][2*i+1])*(rxsig_t[0][2*i+1])));
        time[i] = (float) i;
      }

      fl_set_xyplot_data(form->rxsig_t,time,rxsig_t_dB[0],samples_per_frame,"","","");
    }

    /*
    for (arx=1; arx<nb_antennas_rx; arx++) {
      if (rxsig_t[arx] != NULL) {
        for (i=0; i<FRAME_LENGTH_COMPLEX_SAMPLES; i++) {
          rxsig_t_dB[arx][i] = 10*log10(1.0+(float) ((rxsig_t[arx][2*i])*(rxsig_t[arx][2*i])+(rxsig_t[arx][2*i+1])*(rxsig_t[arx][2*i+1])));
        }

        fl_add_xyplot_overlay(form->rxsig_t,arx,time,rxsig_t_dB[arx],FRAME_LENGTH_COMPLEX_SAMPLES,rx_antenna_colors[arx]);
      }
    }
    */
  }

  if (phy_vars_ue->is_synchronized==0) {
    for (ind=0;ind<3;ind++) {
      /*
      if (pss_corr_ue[ind]) {
	for (i=0; i<samples_per_frame; i++) {
	  corr[i] = (float) pss_corr_ue[ind][i];
	  time[i] = (float) i;
	}
	
	if (ind==0)
	  fl_set_xyplot_data(form->chest_t,time,corr,samples_per_frame,"","","");
	else
	  fl_add_xyplot_overlay(form->chest_t,ind,time,corr,samples_per_frame,rx_antenna_colors[ind]);

	overlay = 1;
      }
      */
    }
  } 
  else {
	
  if (overlay) { //there was a previous overlay
    fl_clear_xyplot(form->chest_t);
    overlay = 0;
  }

  // Channel Impulse Response
  if (chest_t != NULL) {
    ymax = 0;

    if (chest_t[0] !=NULL) {
      for (i=0; i<(frame_parms->ofdm_symbol_size>>3); i++) {
        chest_t_abs[0][i] = (float) (chest_t[0][2*i]*chest_t[0][2*i]+chest_t[0][2*i+1]*chest_t[0][2*i+1]);
	time[i] = (float) i;

        if (chest_t_abs[0][i] > ymax)
          ymax = chest_t_abs[0][i];
      }

      fl_set_xyplot_data(form->chest_t,time,chest_t_abs[0],(frame_parms->ofdm_symbol_size>>3),"","","");
    }
    /*
    for (arx=1; arx<nb_antennas_rx; arx++) {
      if (chest_t[arx] !=NULL) {
        for (i=0; i<(frame_parms->ofdm_symbol_size>>3); i++) {
          chest_t_abs[arx][i] = (float) (chest_t[arx][4*i]*chest_t[arx][4*i]+chest_t[arx][4*i+1]*chest_t[arx][4*i+1]);

          if (chest_t_abs[arx][i] > ymax)
            ymax = chest_t_abs[arx][i];
        }

        fl_add_xyplot_overlay(form->chest_t,arx,time,chest_t_abs[arx],(frame_parms->ofdm_symbol_size>>3),rx_antenna_colors[arx]);
        fl_set_xyplot_overlay_type(form->chest_t,arx,FL_DASHED_XYPLOT);
      }
    }
    */
    // Avoid flickering effect
    //        fl_get_xyplot_ybounds(form->chest_t,&ymin,&ymax); // Does not always work...
    fl_set_xyplot_ybounds(form->chest_t,0,(double) ymax);
  }
  }

  // Channel Frequency Response (includes 5 complex sample for filter)
  if (chest_f != NULL) {
    ind = 0;

    for (atx=0; atx<nb_antennas_tx; atx++) {
      for (arx=0; arx<nb_antennas_rx; arx++) {
        if (chest_f[(atx<<1)+arx] != NULL) {
          for (k=0; k<frame_parms->ofdm_symbol_size; k++) {
            freq[ind] = (float)ind;
            Re = (float)(chest_f[(atx<<1)+arx][(2*k)]);
            Im = (float)(chest_f[(atx<<1)+arx][(2*k)+1]);

            chest_f_abs[ind] = (short)10*log10(1.0+((double)Re*Re + (double)Im*Im));
            ind++;
          }
        }
      }
    }

    // tx antenna 0
    //fl_set_xyplot_xbounds(form->chest_f,0,nb_antennas_rx*nb_antennas_tx*nsymb_ce);
    //fl_set_xyplot_xtics(form->chest_f,nb_antennas_rx*nb_antennas_tx*frame_parms->symbols_per_tti,2);
    //        fl_set_xyplot_xtics(form->chest_f,nb_antennas_rx*nb_antennas_tx*2,2);
    //fl_set_xyplot_xgrid(form->chest_f,FL_GRID_MAJOR);
    fl_set_xyplot_data(form->chest_f,freq,chest_f_abs,frame_parms->ofdm_symbol_size,"","","");

    /*
    for (arx=1; arx<nb_antennas_rx; arx++) {
      fl_add_xyplot_overlay(form->chest_f,1,&freq[arx*nsymb_ce],&chest_f_abs[arx*nsymb_ce],nsymb_ce,rx_antenna_colors[arx]);
    }

    // other tx antennas
    if (nb_antennas_tx > 1) {
      if (nb_antennas_rx > 1) {
        for (atx=1; atx<nb_antennas_tx; atx++) {
          for (arx=0; arx<nb_antennas_rx; arx++) {
            fl_add_xyplot_overlay(form->chest_f,(atx<<1)+arx,&freq[((atx<<1)+arx)*nsymb_ce],&chest_f_abs[((atx<<1)+arx)*nsymb_ce],nsymb_ce,rx_antenna_colors[arx]);
          }
        }
      } else { // 1 rx antenna
        atx=1;
        arx=0;
        fl_add_xyplot_overlay(form->chest_f,atx,&freq[atx*nsymb_ce],&chest_f_abs[atx*nsymb_ce],nsymb_ce,rx_antenna_colors[arx]);
      }
    }
    */
  }
  
  // PBCH LLRs
  if (pbch_llr != NULL) {
    for (i=0; i<864; i++) {
      llr_pbch[i] = (float) pbch_llr[i];
      bit_pbch[i] = (float) i;
    }

    fl_set_xyplot_data(form->pbch_llr,bit_pbch,llr_pbch,864,"","","");
  }

  first_symbol=1;

  // PBCH I/Q of MF Output
  if (pbch_comp!=NULL) {
    for (symbol=first_symbol; symbol<(first_symbol+3); symbol++) {
      if (symbol == 2 || symbol == 6) 
	nb_re = 72;
      else
	nb_re = 180;
      for (i=0; i<nb_re; i++) {
	I[i] = pbch_comp[2*symbol*20*12+2*i];
	Q[i] = pbch_comp[2*symbol*20*12+2*i+1];
      }
    }
    fl_set_xyplot_data(form->pbch_comp,I,Q,432,"","","");
  }

  // PDCCH LLRs
  if (pdcch_llr != NULL) {
    for (i=0; i<100; i++) { //12*frame_parms->N_RB_DL*2*num_pdcch_symbols
      llr_pdcch[i] = (float) pdcch_llr[2*24*9 +i];
      bit_pdcch[i] = (float) i;
    }

    fl_set_xyplot_data(form->pdcch_llr,bit_pdcch,llr_pdcch,12*frame_parms->N_RB_DL*num_pdcch_symbols,"","","");
  }

  // PDCCH I/Q of MF Output
  if (pdcch_comp!=NULL) {
    for (i=0; i<100; i++) {
      I[i] = pdcch_comp[2*50*12+2*i];
      Q[i] = pdcch_comp[2*50*12+2*i+1];
    }
    fl_set_xyplot_data(form->pdcch_comp,I,Q,12*frame_parms->N_RB_DL*num_pdcch_symbols,"","","");
  }

  // PDSCH LLRs
  if (pdsch_llr != NULL) {
    for (i=0; i<coded_bits_per_codeword; i++) {
      llr[i] = (float) pdsch_llr[i];
      bit[i] = (float) i;
    }

    //fl_set_xyplot_xbounds(form->pdsch_llr,0,coded_bits_per_codeword);
    fl_set_xyplot_data(form->pdsch_llr,bit,llr,coded_bits_per_codeword,"","","");
  }

  first_symbol = 2;
  ind = 0;
  // PDSCH I/Q of MF Output
  if (pdsch_comp!=NULL) {
    for (symbol=0;symbol<nb_symb_sch;symbol++) {
      for (i=0; i<nb_rb_pdsch*12; i++) {
	I[ind] = pdsch_comp[2*((first_symbol+symbol)*frame_parms->N_RB_DL*12+i)  ];
	Q[ind] = pdsch_comp[2*((first_symbol+symbol)*frame_parms->N_RB_DL*12+i)+1];
	ind++;
      }
    }
    
    fl_set_xyplot_data(form->pdsch_comp,I,Q,nb_symb_sch*nb_rb_pdsch*12,"","","");
  }
  /*

  // PDSCH Throughput
  memmove( tput_time_ue[UE_id], &tput_time_ue[UE_id][1], (TPUT_WINDOW_LENGTH-1)*sizeof(float) );
  memmove( tput_ue[UE_id],      &tput_ue[UE_id][1],      (TPUT_WINDOW_LENGTH-1)*sizeof(float) );

  tput_time_ue[UE_id][TPUT_WINDOW_LENGTH-1]  = (float) frame;
  tput_ue[UE_id][TPUT_WINDOW_LENGTH-1] = ((float) total_dlsch_bitrate)/1000.0;

  if (tput_ue[UE_id][TPUT_WINDOW_LENGTH-1] > tput_ue_max[UE_id]) {
    tput_ue_max[UE_id] = tput_ue[UE_id][TPUT_WINDOW_LENGTH-1];
  }

  fl_set_xyplot_data(form->pdsch_tput,tput_time_ue[UE_id],tput_ue[UE_id],TPUT_WINDOW_LENGTH,"","","");

  fl_set_xyplot_ybounds(form->pdsch_tput,0,tput_ue_max[UE_id]);
  */

  fl_check_forms();

  free(time);
  free(corr);
  for (arx=0; arx<nb_antennas_rx; arx++) {
    free(rxsig_t_dB[arx]);
  }
  free(rxsig_t_dB);

  free(I);
  free(Q);
  free(llr);
  free(bit);
  free(bit_pdcch);
  free(llr_pdcch);

  /*
  free(chest_f_abs);
  for (arx=0; arx<nb_antennas_rx; arx++) {
    free(chest_t_abs[arx]);
  }
  free(chest_t_abs);
  */
}


typedef struct {
  FL_FORM    *stats_form;
  void       *vdata;
  char       *cdata;
  long        ldata;
  FL_OBJECT *stats_text;
  FL_OBJECT *stats_button;
} FD_stats_form;


// current status is that every UE has a DL scope for a SINGLE eNB (gnb_id=0)
// at eNB 0, an UL scope for every UE
//FD_phy_scope_gnb             *form_gnb[MAX_NUM_CCs][NUMBER_OF_UE_MAX];
static FD_phy_scope_gnb        *form_gnb[NUMBER_OF_UE_MAX];
//FD_stats_form                  *form_stats=NULL,*form_stats_l2=NULL;
//char                            title[255];
unsigned char                   scope_enb_num_ue = 2;
//static pthread_t                forms_thread; //xforms


void reset_stats_gNB(FL_OBJECT *button,
		             long arg)
{
  int i,k;
  //PHY_VARS_gNB *phy_vars_gNB = RC.gNB[0][0];

  for (i=0; i<NUMBER_OF_UE_MAX; i++) {
    for (k=0; k<8; k++) { //harq_processes
      /*      for (j=0; j<phy_vars_gNB->dlsch[i][0]->Mlimit; j++) {
        phy_vars_gNB->UE_stats[i].dlsch_NAK[k][j]=0;
        phy_vars_gNB->UE_stats[i].dlsch_ACK[k][j]=0;
        phy_vars_gNB->UE_stats[i].dlsch_trials[k][j]=0;
            }
      phy_vars_gNB->UE_stats[i].dlsch_l2_errors[k]=0;
      phy_vars_gNB->UE_stats[i].ulsch_errors[k]=0;
      phy_vars_gNB->UE_stats[i].ulsch_consecutive_errors=0;
      phy_vars_gNB->UE_stats[i].dlsch_sliding_cnt=0;
      phy_vars_gNB->UE_stats[i].dlsch_NAK_round0=0;
      phy_vars_gNB->UE_stats[i].dlsch_mcs_offset=0;*/
    }
  }
}


static void *scope_thread_gNB(void *arg) {
  int UE_id;
  int ue_cnt=0;
# ifdef ENABLE_XFORMS_WRITE_STATS
  FILE *gNB_stats = fopen("gNB_stats.txt", "w");
#endif

  while (!oai_exit) {
    ue_cnt=0;
    
    for(UE_id=0; UE_id<NUMBER_OF_UE_MAX; UE_id++) {
        if ((ue_cnt<scope_enb_num_ue)) {
          //this function needs to be written
          phy_scope_gNB(form_gnb[ue_cnt], RC.gNB[0], UE_id);
          ue_cnt++;
        }
    }
    sleep(1);
  }

//  printf("%s",stats_buffer);
/*#ifdef ENABLE_XFORMS_WRITE_STATS

  if (eNB_stats) {
    rewind (gNB_stats);
    fwrite (stats_buffer, 1, len, gNB_stats);
    fclose (gNB_stats);
  }

#endif
  pthread_exit((void *)arg);
}*/

  return NULL;
}

FD_stats_form * create_form_stats_form( void ) {
  FL_OBJECT *obj;
  FD_stats_form *fdui = fl_malloc( sizeof *fdui );
  fdui->vdata = fdui->cdata = NULL;
  fdui->ldata = 0;
  fdui->stats_form = fl_bgn_form( FL_NO_BOX, 1115, 900 );
  obj = fl_add_box( FL_UP_BOX, 0, 0, 1115, 900, "" );
  //fdui->stats_text = obj = fl_add_text( FL_NORMAL_TEXT, 60, 50, 1000, 810, "test" );
  //fl_set_object_lsize( obj, FL_TINY_SIZE );
  fdui->stats_text = obj = fl_add_browser( FL_NORMAL_BROWSER, 60, 50, 1000, 810, "test" );
  fl_set_browser_fontsize(obj,FL_TINY_SIZE);
  fdui->stats_button = obj = fl_add_button( FL_PUSH_BUTTON, 60, 10, 130, 30, "Reset Stats" );
  fl_set_object_lalign( obj, FL_ALIGN_CENTER );
  fl_set_object_color( obj, FL_GREEN, FL_GREEN);
  fl_end_form( );
  fdui->stats_form->fdui = fdui;
  return fdui;
}

void startScope(scopeParms_t * p) {
  FD_stats_form *form_stats=NULL,*form_stats_l2=NULL;
  char title[255];
  fl_initialize (p->argc, p->argv, NULL, 0, 0);
  form_stats_l2 = create_form_stats_form();
  fl_show_form (form_stats_l2->stats_form, FL_PLACE_HOTSPOT, FL_FULLBORDER, "l2 stats");
  form_stats = create_form_stats_form();
  fl_show_form (form_stats->stats_form, FL_PLACE_HOTSPOT, FL_FULLBORDER, "stats");

  for(int UE_id=0; UE_id<scope_enb_num_ue; UE_id++) {
    form_gnb[UE_id] = create_phy_scope_gnb();
    sprintf (title, "LTE UL SCOPE eNB for UE %d",UE_id);
    fl_show_form (form_gnb[UE_id]->phy_scope_gnb, FL_PLACE_HOTSPOT, FL_FULLBORDER, title);
  } // UE_id

  pthread_t forms_thread;
  threadCreate(&forms_thread, scope_thread_gNB, NULL, "scope", -1, OAI_PRIORITY_RT_LOW);
}
