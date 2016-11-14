#include "ApplyHLSigmaCut.h"
#include "PHCompositeNode.h"
#include "getClass.h"
#include "Fun4AllReturnCodes.h"
#include "Fun4AllServer.h"
#include "stdlib.h"
#include "TMpcExHitContainer.h"
#include "TMpcExCalibContainer.h"
#include "TMpcExCalib.h"
#include <fstream>
#include <math.h>

using namespace std;
using namespace findNode;

Double_t fitf(Double_t* x,Double_t* par){
  double result=0;
  result += par[0]+par[1]*x[0];
  return result;
}


ApplyHLSigmaCut::ApplyHLSigmaCut(const char* name) :
  SubsysReco(name)
{
  _mpcex_hits_container=NULL;
  _mpcex_calib_container=NULL;
  for(int i=0;i<128*24*16;i++){
    slope[i] = -9999;
    intercept[i] = -9999;
    sigma[i] = -9999;
  }
}

int ApplyHLSigmaCut::End(PHCompositeNode* topNode){
  return EVENT_OK;
}
 
ApplyHLSigmaCut::~ApplyHLSigmaCut(){
}
 
int ApplyHLSigmaCut::Init(PHCompositeNode* topNode){
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
 
int ApplyHLSigmaCut::InitRun(PHCompositeNode* topNode){
  set_interface_ptrs(topNode);
  return EVENT_OK;
}

void ApplyHLSigmaCut::set_interface_ptrs(PHCompositeNode* topNode){
   _mpcex_hits_container = getClass<TMpcExHitContainer>(topNode,"TMpcExHitContainer");
  if(!_mpcex_hits_container){
    cout << PHWHERE <<":: No TMpcExHitContainer!!!"<<endl;
    exit(1);
  }
 
  _mpcex_calib_container = getClass<TMpcExCalibContainer>(topNode,"TMpcExCalibContainer");
  if(!_mpcex_calib_container){
    cout << PHWHERE <<":: No TMpcExCalibContainer !!!"<<endl;
    exit(1);
  }
}

int ApplyHLSigmaCut::process_event(PHCompositeNode* topNode){
  int size=_mpcex_hits_container->size();
  vector<TMpcExHit*> bad_hits;
  for(int i=0;i<size;i++){
    TMpcExHit* hit = _mpcex_hits_container->getHit(i);
    double high = hit->high();
    double low = hit->low();
    int key= hit->key();
    if(slope[key]<0) continue;
    TMpcExCalib *calib = _mpcex_calib_container->get(key);
    double sigma_db = calib->get_high_low_sigma();
    if(fabs(sigma_db-sigma[key])/sigma_db > 0.2){
      cout<<key<<" "<<sigma_db<<" "<<sigma[key]<<endl;
    }
    double mip=calib->get_mip_in_sensor();
    mip=mip*calib->get_minipad_mip_correction();
    double high_adc=high*mip/147.5;
    double low_adc=low*mip/147.5*slope[key];
    double eval_low = intercept[key]+slope[key]*high_adc;
    double dist = (eval_low-low_adc)/sqrt(slope[key]*slope[key]+1);
    if(fabs(dist)>5*sigma[key]){
      if(high_adc+calib->high_pedestal()<245) bad_hits.push_back(hit);
      else if(low_adc<eval_low) bad_hits.push_back(hit);
    }
  }

  //erase bad hits
  int nbad = bad_hits.size();
  for(int i=0;i<nbad;i++){
    _mpcex_hits_container->erase(bad_hits[i]);
  }
  return EVENT_OK;  
}

