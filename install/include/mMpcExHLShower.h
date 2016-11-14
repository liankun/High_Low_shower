#ifndef __MMPCEXHLSHOWER_HH__
#define __MMPCEXHLSHOWER_HH__
 
#ifndef __CINT__
#include <SubsysReco.h>
#endif

class SubsysReco;
class PHCompositeNode;
class TMpcExHitContainer;
class MpcExRawHit;
class TMpcExCalibContainer;
class TMpcExShowerContainer;
class TH2D;
class PHGlobal;
class BbcOut;
 
class mMpcExHLShower:public SubsysReco{
  public:
    mMpcExHLShower(const char* name = "MMPCEXHLSHOWER");
    virtual int Init(PHCompositeNode*);
    virtual int InitRun(PHCompositeNode*);
    virtual int process_event(PHCompositeNode*);
    virtual ~mMpcExHLShower();
    virtual int End(PHCompositeNode*);
    void set_file(const char* ifile){_ifile = ifile;}
    void print_info();
  private:
    void set_interface_ptrs(PHCompositeNode*);
    TMpcExHitContainer* _mpcex_hits_container;
    TMpcExCalibContainer* _mpcex_calib_container;
    MpcExRawHit* _mpcex_raw_hits;
    TMpcExShowerContainer* _mpcex_shower_container;
    PHGlobal* _phglobal;
    BbcOut* _bbcout;

    TH2D* hlayer_e[2];
    TH2D* hhigh_low[2];
    TH2D* hhigh_low_adc[2];
    TH2D* hhigh_low_org[2];
    TH2D* hhigh_low_adc_org[2];
    TH2D* hhigh_low_exam;
    TH2D* hhigh_low_exam_org;
    TH2D* hhsxy_diff_good[2];
    TH2D* hhsxy_diff_bad[2];
    TH2D* hhsxy_diff_good_odd[2];
    TH2D* hhsxy_diff_bad_odd[2];
    TH2D* hhsxy_diff_good_even[2];
    TH2D* hhsxy_diff_bad_even[2];

    //layer e between layers
    TH2D* hlayers_e[2][7];


    const char* _ifile;

    double slope[24*128*16];
    double intercept[24*128*16];
    double sigma[24*128*16];
    double _vertex;
    int _total_hits;
    int _north_hits;
    int _south_hits;
    int _nevent;

  protected:
};
#endif /*__MMPCEXHLSHOWER_HH__*/
