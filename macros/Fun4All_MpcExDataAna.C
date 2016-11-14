void Fun4All_MpcExDataAna(char* input_file_mpc ="",char* input_file_eve="",int runnumber = 0,int segment = 0){
  gSystem->Load("libfun4all");
  gSystem->Load("libuspin.so");
  gSystem->Load("libMyMpcEx.so");
  gSystem->Load("libfun4allfuncs.so");
  gSystem->Load("libbbc");
  gSystem->Load("libt0");
  gSystem->Load("libmpc.so");
  gSystem->Setenv("ODBCINI","/opt/phenix/etc/odbc.ini.mirror");

  TStopwatch t;
  t.Start();

  recoConsts* rc = recoConsts::instance();
  Fun4AllServer* se = Fun4AllServer::instance();

  rc->set_IntFlag("MPC_RECO_MODE",0x16);
  rc->set_IntFlag("MPCEXKILLBADHITS",1);
  rc->set_IntFlag("MPCEX_SIGMA_CUT",0);
//  rc->set_IntFlag("MPCEX_HL_CONSISTENCY_CUT",1);

  rc->set_IntFlag("MPCEXCALIBMODE",5);//0:only ped subtract,1: + CMN sub,5: compelete
  rc->set_IntFlag("MPCEXCALIBAPPLYSTACK",1);//single buffer
//  rc->set_IntFlag("MPCEX_CMN_SUBTRACT",1);
  runnumber = 454789;
  rc->set_IntFlag("RUNNUMBER",runnumber);
  
  SubsysReco *mpcreco = new MpcReco("MPCRECO");
  se->registerSubsystem(mpcreco);
  
  SubsysReco* createNodeTree = new mMpcExCreateNodeTree();
  se->registerSubsystem(createNodeTree);

  SubsysReco* mpcexLoadCal = new mMpcExLoadCalibrations();
  se->registerSubsystem(mpcexLoadCal);

  SubsysReco* mpcexApplyCal = new mMpcExApplyCalibrations();
  se->registerSubsystem(mpcexApplyCal);

//  ApplyHLSigmaCut* apcut = new ApplyHLSigmaCut();
//  apcut->set_file("Low_High_ratio_new.txt");
//  se->registerSubsystem(apcut);


  mMpcExShower* mpcexShower = new mMpcExShower();
  mpcexShower->DisableSouthReco();
  se->registerSubsystem(mpcexShower);

  mMpcExHLShower* HLShower = new mMpcExHLShower();
  HLShower->set_file("Low_High_ratio_new.txt");
  se->registerSubsystem(HLShower);

  Fun4AllInputManager* mpcex_dst_eve = new Fun4AllDstInputManager("DST_EVE","DST","TOP");
  se->registerInputManager(mpcex_dst_eve);

  Fun4AllDstInputManager* mpcex_dst_mpc = new Fun4AllDstInputManager("DST_MPC","DST","TOP");
  se->registerInputManager(mpcex_dst_mpc);
  
  Fun4AllInputManager* mpcex_dst_mpcex = new Fun4AllDstInputManager("DST_MPCEX","DST","TOP");
  se->registerInputManager(mpcex_dst_mpcex);

  
  rc->set_IntFlag("RUNNUMBER",runnumber);
  cout << "run number "<<runnumber<<endl;
 
//  MpcMap* map = MpcMap::instance();
//  map->Print();
  for(int i = 0;i < 1;i++){
    
      char path_mpcex[200];
      sprintf(path_mpcex,"/gpfs/mnt/gpfs02/phenix/mpcex/online_production/run16_mpcex_mpcex/run_0000454000_0000455000/DST_MPCEX/DST_MPCEX_run16_mpcex_mpcex-0000%d-00%02d.root",runnumber,i);

    cout <<"open "<<path_mpcex<<endl;
    char path_mpc[200];
    sprintf(path_mpc,"/gpfs/mnt/gpfs02/phenix/mpcex/online_production/run16_mpcex_mpcex/run_0000454000_0000455000/DST_MPC/DST_MPC_run16_mpcex_mpcex-0000%d-00%02d.root",runnumber,i);
    cout <<"open "<<path_mpc<<endl;
    char path_eve[200];
    sprintf(path_eve,"/gpfs/mnt/gpfs02/phenix/mpcex/online_production/run16_mpcex_mpcex/run_0000454000_0000455000/DST_EVE/DST_EVE_run16_mpcex_mpcex-0000%d-00%02d.root",runnumber,i);
    cout <<"open "<<path_eve<<endl;
    se->fileopen(mpcex_dst_eve->Name(),path_eve);
    se->fileopen(mpcex_dst_mpc->Name(),path_mpc);
    se->fileopen(mpcex_dst_mpcex->Name(),path_mpcex);

    se->run(20000);
  }
  
  HLShower->print_info();
  t.Stop();
  t.Print();
  
  se->End();
  char output[100];
  sprintf(output,"HLShower_new-%d.root",runnumber);
  Fun4AllHistoManager* hm = se->getHistoManager("HLShower");
  if(hm) hm->dumpHistos(output);

  delete se;

  cout << "Completed reconstruction." <<endl;
}
