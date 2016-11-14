#include "mMpcExHLShower.h"
#include "PHCompositeNode.h"
#include "getClass.h"
#include "Fun4AllReturnCodes.h"
#include "Fun4AllServer.h"
#include "Fun4AllHistoManager.h"
#include "stdlib.h"
#include "TMpcExHitContainer.h"
#include "TMpcExShower.h"
#include "MpcExRawHit.h"
#include "TMpcExCalibContainer.h"
#include "TMpcExShowerContainer.h"
#include "TH2D.h"
#include "TriggerHelper.h"
#include <fstream>
#include <BbcOut.h>
#include <PHGlobal.h>

using namespace std;
using namespace findNode;
 
mMpcExHLShower::mMpcExHLShower(const char* name) :
  SubsysReco(name)
{
  _mpcex_hits_container=NULL;
  _mpcex_calib_container=NULL;
  _mpcex_raw_hits=NULL;
  _mpcex_shower_container=NULL;
  for(int iarm=0;iarm<2;iarm++){
    hlayer_e[iarm]=NULL;
    hhigh_low[iarm]=NULL;
    hhigh_low_adc[iarm]=NULL;
    hhigh_low_adc_org[iarm]=NULL;
    hhigh_low_org[iarm]=NULL;
    hhsxy_diff_good[iarm]=NULL;
    hhsxy_diff_bad[iarm]=NULL;
    hhsxy_diff_good_even[iarm]=NULL;
    hhsxy_diff_good_odd[iarm]=NULL;
    hhsxy_diff_bad_even[iarm]=NULL;
    hhsxy_diff_bad_odd[iarm]=NULL;
    for(int i=0;i<7;i++){
      hlayers_e[iarm][i]=NULL;
    }
  }
  hhigh_low_exam=NULL;
  hhigh_low_exam_org=NULL;
  _vertex=-9999.9;
  _total_hits=0;
  _nevent=0;

  for(int i=0;i<128*24*16;i++){
    slope[i] = -9999;
    intercept[i] = -9999;
    sigma[i] = -9999;
  }
}

int mMpcExHLShower::End(PHCompositeNode* topNode){
  return EVENT_OK;
}
 
mMpcExHLShower::~mMpcExHLShower(){
}
 
int mMpcExHLShower::Init(PHCompositeNode* topNode){
  Fun4AllServer* se = Fun4AllServer::instance();
  Fun4AllHistoManager* hm = se->getHistoManager("HLShower");
  if(!hm){
    hm = new Fun4AllHistoManager("HLShower");
    se->registerHistoManager(hm);
  }

  char hname[500];
  for(int iarm=0;iarm<2;iarm++){
    sprintf(hname,"hlayer_e_arm%d",iarm);
    hlayer_e[iarm] = new TH2D(hname,hname,8,-0.5,7.5,800,0,0.01);
    hlayer_e[iarm]->GetXaxis()->SetTitle("Layer");
    hlayer_e[iarm]->GetYaxis()->SetTitle("E/GeV");
    hm->registerHisto(hlayer_e[iarm]);

    sprintf(hname,"hhigh_low_arm%d",iarm);
    hhigh_low[iarm] = new TH2D(hname,hname,500,0,4000,500,0,4000);
    hhigh_low[iarm]->GetXaxis()->SetTitle("Low/KeV");
    hhigh_low[iarm]->GetYaxis()->SetTitle("High/KeV");
    hm->registerHisto(hhigh_low[iarm]);

    sprintf(hname,"hhigh_low_adc_arm%d",iarm);
    hhigh_low_adc[iarm] = new TH2D(hname,hname,250,-0.5,250-0.5,250,-0.5,250-0.5);
    hhigh_low_adc[iarm]->GetXaxis()->SetTitle("Low ADC");
    hhigh_low_adc[iarm]->GetYaxis()->SetTitle("High ADC");
    hm->registerHisto(hhigh_low_adc[iarm]);
  
    sprintf(hname,"hhigh_low_org_arm%d",iarm);
    hhigh_low_org[iarm] = new TH2D(hname,hname,500,0,4000,500,0,4000);
    hhigh_low_org[iarm]->GetXaxis()->SetTitle("Low/KeV");
    hhigh_low_org[iarm]->GetYaxis()->SetTitle("High/KeV");
    hm->registerHisto(hhigh_low_org[iarm]);

    sprintf(hname,"hhigh_low_adc_org_arm%d",iarm);
    hhigh_low_adc_org[iarm] = new TH2D(hname,hname,250,-0.5,250-0.5,250,-0.5,250-0.5);
    hhigh_low_adc_org[iarm]->GetXaxis()->SetTitle("Low ADC");
    hhigh_low_adc_org[iarm]->GetYaxis()->SetTitle("High ADC");
    hm->registerHisto(hhigh_low_adc_org[iarm]);
  
    sprintf(hname,"hhsxy_diff_good_arm%d",iarm);
    hhsxy_diff_good[iarm] = new TH2D(hname,hname,600,-0.1,0.1,600,-0.1,0.1);
    hhsxy_diff_good[iarm]->GetXaxis()->SetTitle("X diff");
    hhsxy_diff_good[iarm]->GetYaxis()->SetTitle("Y diff");
    hm->registerHisto(hhsxy_diff_good[iarm]);

    sprintf(hname,"hhsxy_diff_bad_arm%d",iarm);
    hhsxy_diff_bad[iarm] = new TH2D(hname,hname,600,-0.1,0.1,600,-0.1,0.1);
    hhsxy_diff_bad[iarm]->GetXaxis()->SetTitle("X diff");
    hhsxy_diff_bad[iarm]->GetYaxis()->SetTitle("Y diff");
    hm->registerHisto(hhsxy_diff_bad[iarm]);

    sprintf(hname,"hhsxy_diff_good_even_arm%d",iarm);
    hhsxy_diff_good_even[iarm] = new TH2D(hname,hname,600,-0.1,0.1,600,-0.1,0.1);
    hhsxy_diff_good_even[iarm]->GetXaxis()->SetTitle("X diff");
    hhsxy_diff_good_even[iarm]->GetYaxis()->SetTitle("Y diff");
    hm->registerHisto(hhsxy_diff_good_even[iarm]);

    sprintf(hname,"hhsxy_diff_good_odd_arm%d",iarm);
    hhsxy_diff_good_odd[iarm] = new TH2D(hname,hname,600,-0.1,0.1,600,-0.1,0.1);
    hhsxy_diff_good_odd[iarm]->GetXaxis()->SetTitle("X diff");
    hhsxy_diff_good_odd[iarm]->GetYaxis()->SetTitle("Y diff");
    hm->registerHisto(hhsxy_diff_good_odd[iarm]);

    sprintf(hname,"hhsxy_diff_bad_even_arm%d",iarm);
    hhsxy_diff_bad_even[iarm] = new TH2D(hname,hname,600,-0.1,0.1,600,-0.1,0.1);
    hhsxy_diff_bad_even[iarm]->GetXaxis()->SetTitle("X diff");
    hhsxy_diff_bad_even[iarm]->GetYaxis()->SetTitle("Y diff");
    hm->registerHisto(hhsxy_diff_bad_even[iarm]);

    sprintf(hname,"hhsxy_diff_bad_odd_arm%d",iarm);
    hhsxy_diff_bad_odd[iarm] = new TH2D(hname,hname,600,-0.1,0.1,600,-0.1,0.1);
    hhsxy_diff_bad_odd[iarm]->GetXaxis()->SetTitle("X diff");
    hhsxy_diff_bad_odd[iarm]->GetYaxis()->SetTitle("Y diff");
    hm->registerHisto(hhsxy_diff_bad_odd[iarm]);
  
    for(int i=0;i<7;i++){
      sprintf(hname,"hlayers_e_arm%d_%d",iarm,i);
      hlayers_e[iarm][i] = new TH2D(hname,hname,500,0,0.1,500,0,0.1);
      hlayers_e[iarm][i]->GetXaxis()->SetTitle("Previous layer e");
      hlayers_e[iarm][i]->GetYaxis()->SetTitle("layer e");
      hm->registerHisto(hlayers_e[iarm][i]);
    }
  
  }

  hhigh_low_exam = new TH2D("hhigh_low_exam","High Low Example",500,0,4000,500,0,4000);
  hhigh_low_exam->GetXaxis()->SetTitle("Low/KeV");
  hhigh_low_exam->GetYaxis()->SetTitle("High/KeV");
  hm->registerHisto(hhigh_low_exam);
  
  hhigh_low_exam_org = new TH2D("hhigh_low_exam_org","High Low Example",500,0,4000,500,0,4000);
  hhigh_low_exam_org->GetXaxis()->SetTitle("Low/KeV");
  hhigh_low_exam_org->GetYaxis()->SetTitle("High/KeV");
  hm->registerHisto(hhigh_low_exam_org);


  ifstream in_txt(_ifile);
  if(!in_txt){
    cout<<"NO txt file found !"<<endl;
  }
  else{
    int key;
    double par0;
    double error0;
    double par1;
    double error1;
    double par2;
    double error2;

    while(in_txt>>key>>par0>>error0>>par1>>error1>>par2>>error2){
//      cout<<key<<" "<<par0<<" "<<error0<<" "
//                    <<par1<<" "<<error1<<" "
//		    <<par2<<" "<<error2<<" "
//		    <<endl;
      slope[key]=par0;
      intercept[key]=par1;
      sigma[key]=par2;
    }
  }

  return EVENT_OK;
}
 
int mMpcExHLShower::InitRun(PHCompositeNode* topNode){
  set_interface_ptrs(topNode);
  return EVENT_OK;
}


void mMpcExHLShower::set_interface_ptrs(PHCompositeNode* topNode){
  _mpcex_hits_container = getClass<TMpcExHitContainer>(topNode,"TMpcExHitContainer");
  if(!_mpcex_hits_container){
    cout << PHWHERE <<":: No TMpcExHitContainer!!!"<<endl;
    exit(1);
  }

  _mpcex_raw_hits = getClass<MpcExRawHit>(topNode,"MpcExRawHit");
  if(!_mpcex_raw_hits){
    cout << PHWHERE <<":: NO MpcExRawHit !!!"<<endl;
    exit(1);
  }

  _mpcex_calib_container = getClass<TMpcExCalibContainer>(topNode,"TMpcExCalibContainer");
  if(!_mpcex_calib_container){
    cout << PHWHERE <<":: No TMpcExCalibContainer !!!"<<endl;
    exit(1);
  }

  _mpcex_shower_container = getClass<TMpcExShowerContainer>(topNode,"TMpcExShowerContainer");
  if(!_mpcex_shower_container){
    cout << PHWHERE <<":: No TMpcExShowerContainer !!!"<<endl;
//    exit(1);
  }

  _phglobal = getClass<PHGlobal>(topNode, "PHGlobal");
  _bbcout = getClass<BbcOut>(topNode, "BbcOut");

}


int mMpcExHLShower::process_event(PHCompositeNode* topNode){
//  TriggerHelper* myTH = new TriggerHelper(topNode);
//  int fire_minbias0 = myTH->trigScaled("BBCLL1(>0 tubes)");
//  int fire_minbias1 = myTH->trigScaled("BBCLL1(>0 tubes) novertex");
//  int fire_minbias2 = myTH->trigScaled("BBCLL1(>0 tubes) narrowvtx");
//  if(!(fire_minbias0||fire_minbias1||fire_minbias2)) return EVENT_OK;

  if(!_bbcout && !_phglobal){
    _vertex=0;
  }
  else{
    _vertex=(_phglobal==0) ? _phglobal->getBbcZVertex() : _bbcout->get_VertexPoint();
  }

  
  int Nshowers = _mpcex_shower_container->size();
  for(int i = 0;i < Nshowers;i++){
    TMpcExShower* shower = _mpcex_shower_container->getShower(i);
    int arm=shower->get_arm();
    int nfired=shower->get_nlayers();
    int Nhits = shower->sizeHits();
    double swr_hsx=shower->get_hsx();
    double swr_hsy=shower->get_hsy();
    double swr_hsx_rms=shower->get_rms_hsx();
    double swr_hsy_rms=shower->get_rms_hsy();
    if(nfired!=8 || Nhits!=8 ) continue;
    int fst_fired=shower->get_first_layer();
//    bool want_shower=false;
//    double thresh[8]={0.02,0.03,0.035,0.04,0.04,0.04,0.04,0.04};
    for(int j=fst_fired;j<nfired;j++){
      double e=shower->get_e_layer(j);
      hlayer_e[arm]->Fill(j,e);
//      if(e>thresh[j]) want_shower=true;
      if(j>0){
        hlayers_e[arm][j-1]->Fill(shower->get_e_layer(j-1),e);
      }
    }

//
//
//    if(want_shower){
//      for(int j=fst_fired;j<nfired;j++){
//        double e=shower->get_e_layer(j);
//        hlayer_e[arm]->Fill(j,e);
//      }
//    }
//
   
    for(int ihit=0;ihit<Nhits;ihit++){
      int key = shower->getHit(ihit);
      TMpcExHit* hit = _mpcex_hits_container->get_hit_by_key(key);
      if(!hit) cout<<"bad hit !!!"<<endl;
      double high = hit->high();
      double low = hit->low();
      double hsx = hit->hsx(_vertex);
      double hsy = hit->hsy(_vertex);
      int layer=hit->layer();
//      if(fabs(hsx-swr_hsx)>1*swr_hsx_rms || fabs(hsy-swr_hsy)>1*swr_hsy_rms)continue;

//      cout<<key<<" "<<high<<" "<<low<<endl;
      hhigh_low[arm]->Fill(low,high);
      if(key==25204){
        hhigh_low_exam->Fill(low,high);        
      }
    
      //adc hit part
      TMpcExCalib *calib = _mpcex_calib_container->get(key);
      double mip=calib->get_mip_in_sensor();
      mip=mip*calib->get_minipad_mip_correction();
      double high_adc=high*mip/147.5;
      double low_adc=low*mip/147.5;
      if(slope[key]<0)continue;
      low_adc=low_adc-intercept[key]/slope[key];
      hhigh_low_adc[arm]->Fill(low_adc,high_adc);

      //dist
      low_adc=low_adc*slope[key];
      double eval_low = intercept[key]+slope[key]*high_adc;
      double dist = (eval_low-low_adc)/sqrt(slope[key]*slope[key]+1);
      if((fabs(dist)>5*sigma[key])&&(high_adc+calib->high_pedestal()<250)){
        hhsxy_diff_bad[arm]->Fill(hsx-swr_hsx,hsy-swr_hsy);
	if(layer%2==0)hhsxy_diff_bad_even[arm]->Fill(hsx-swr_hsx,hsy-swr_hsy);
	else hhsxy_diff_bad_odd[arm]->Fill(hsx-swr_hsx,hsy-swr_hsy);
      }
      else{
        hhsxy_diff_good[arm]->Fill(hsx-swr_hsx,hsy-swr_hsy);
        if(layer%2==0)hhsxy_diff_good_even[arm]->Fill(hsx-swr_hsx,hsy-swr_hsy);
	else hhsxy_diff_good_odd[arm]->Fill(hsx-swr_hsx,hsy-swr_hsy);

      }
    }   
  }
  

  int size=_mpcex_hits_container->size();
  _total_hits+=size;
  _nevent++;
  for(int i=0;i<size;i++){
    TMpcExHit* hit = _mpcex_hits_container->getHit(i);
    int arm = hit->arm();
    if(arm==0) _south_hits++;
    else _north_hits++;
  }
//  cout<<"number of events: "<<_nevent<<" total hits: "<<_total_hits<<endl;

  for(int i=0;i<size;i++){
    TMpcExHit* hit = _mpcex_hits_container->getHit(i);
    double high = hit->high();
    double low = hit->low();
    int arm = hit->arm();
    hhigh_low_org[arm]->Fill(low,high);
    int key= hit->key();
    if(key==25204){
      hhigh_low_exam_org->Fill(low,high);
    }
      
    //adc hit part
    TMpcExCalib *calib = _mpcex_calib_container->get(key);
    double mip=calib->get_mip_in_sensor();
    mip=mip*calib->get_minipad_mip_correction();
    double high_adc=high*mip/147.5;
    double low_adc=low*mip/147.5;
    if(slope[key]<0) continue;
    low_adc=low_adc-intercept[key]/slope[key];
    hhigh_low_adc_org[arm]->Fill(low_adc,high_adc);
  }

  return EVENT_OK;
}

void mMpcExHLShower::print_info(){
  cout<<"Number of Events: "<<_nevent<<endl;
  cout<<"Number of hits: "<<_total_hits<<endl;
  cout<<"South hits: "<<_south_hits<<" North hits: "<<_north_hits<<endl;
}
