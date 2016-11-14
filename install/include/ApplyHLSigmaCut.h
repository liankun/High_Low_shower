#ifndef __APPLYHLSIGMACUT_HH__
#define __APPLYHLSIGMACUT_HH__
 
#ifndef __CINT__
#include <SubsysReco.h>
#endif
class SubsysReco; 
class PHCompositeNode;
class TMpcExHitContainer;
class TMpcExCalibContainer;
 
class ApplyHLSigmaCut:public SubsysReco{
  public:
    ApplyHLSigmaCut(const char* name = "APPLYHLSIGMACUT");
    virtual int Init(PHCompositeNode*);
    virtual int InitRun(PHCompositeNode*);
    virtual int process_event(PHCompositeNode*);
    virtual ~ApplyHLSigmaCut();
    virtual int End(PHCompositeNode*);
    void set_file(const char* ifile){_ifile = ifile;}

  private:
    void set_interface_ptrs(PHCompositeNode*);
     
    TMpcExHitContainer* _mpcex_hits_container;
    TMpcExCalibContainer* _mpcex_calib_container;
    const char* _ifile;
    double slope[24*128*16];
    double intercept[24*128*16];
    double sigma[24*128*16];


  protected:
};
#endif /*__APPLYHLSIGMACUT_HH__*/
