#include "MichelTriggerDecision.h"

int MichelTriggerDecision(vector<vector<int>> candidates){
  prev_channel = -9000;
  prev_time = -9000;
  int nChannels = 2560;
  int last_APA = -9000;
  vector<int> Michel_candidates; 
  for (unsigned int i_candidate = 0; i_candidate< candidates.size(); i++){
    //check if the last channel in the trigger candidates is not at the end of the APA and previous trigger
    //candidates is also in the same APA
    if (candidates.at(i_candidate).at(6) < nChannels and last_APA == candidates.at(i_candidate).at(8)){
      Michel_candidates.push_back(i_candidate);
    
    }//check end channel of trigger candidates
    
   // else if (candidates.at(i_candidate).at(6) < nChannels and last_APA == canda){
    
    //}
    
    prev_channel = candidates.at(i_candidate).at(6);
    prev_time = candidates.at(i_candidate).at(7);
  
  }//finish looping through the trigger candidates
  return 0;
}


//some standard C++ includes
#include <iostream>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <fstream>

//some ROOT includes
#include "TInterpreter.h"
#include "TROOT.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3D.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TString.h"
#include "TStopwatch.h"

struct TriggerCandidate{
    int adjacency;
    int adcpeak;
    int adcsum;
    int tspan;
    int first_ch;
    int last_ch;
    double tstart;
    double first_time;
    double last_time;
    int apanum;
};

struct TP{
  unsigned int channel;
  unsigned int tstart;
  unsigned int tspan;
  unsigned int adcsum;
  unsigned int adcpeak;
  unsigned int flags;
};

void newMichelTrigger(){
    //using namespace std;
    
    //get the actual TCs from the txt
    TString inputname1 = "/Applications/textFiles/MichelTriggering/tcsorted_20adjacency.txt";
    std::ifstream inputFile1;
    inputFile1.open(inputname1);
    int adjacency;
    int adcpeak;
    int adcsum;
    int tspan;
    int first_ch;
    int last_ch;
    double tstart;
    double first_time;
    double last_time;
    int apanum;
    std::vector<TriggerCandidate> candidates;
    while (inputFile1 >> adjacency >> adcpeak >> adcsum >> tspan >> first_ch>> last_ch>> tstart >> first_time >> last_time >> apanum){
        TriggerCandidate tc;
        tc.adjacency = adjacency;
        tc.adcpeak = adcpeak;
        tc.adcsum = adcsum;
        tc.tspan = tspan;
        tc.first_ch = first_ch;
        tc.last_ch = last_ch;
        tc.tstart = tstart;
        tc.first_time = first_time;
        tc.last_time = last_time;
        tc.apanum = apanum;
        candidates.push_back(tc);
    }
    inputFile1.close();
    

  int prev_channel = -9000;
  int prev_time = -9000;
  int nChannels = 480;
  int last_APA = -9000;
  std::vector< std::pair<double,double> > Michel_candidates;
  std::pair<double,double> m_candidate;
  int michel_candidates_num = 0;
  for ( unsigned int i_candidate =0; i_candidate<candidates.size();i_candidate++){
  
    //check if the last channel in the trigger candidates is not at the end of the APA and previous trigger
    //candidates is also in the same APA
   
      if (last_APA == candidates.at(i_candidate).apanum){
      //if (last_APA == candidates.at(i_candidate).at(8)){
        if(fabs(candidates.at(i_candidate).first_ch - prev_channel) <=1 ){
        //if(fabs(candidates.at(i_candidate).at(4)- prev_channel) <=1  ){
            michel_candidates_num++;
            //std::cout << " grouping possible trigger" <<std::endl;
            //std::cout << fabs(candidates.at(i_candidate).first_ch- prev_channel) << " Channel difference" << std::endl;
        }
        else{
            //std::cout <<candidates.at(i_candidate).at(4)- prev_channel<< " Too big of a channel gap" << std::endl;
            //if the next TC candidates is not withing a wire, check if the previous ones were, and try to trigger
            
            if (michel_candidates_num >0){
            //save first_time of the first TC and last_time for the last TC
                m_candidate.first = candidates.at(i_candidate-michel_candidates_num-1).first_time;
                m_candidate.second = candidates.at(i_candidate-1).last_time;
                Michel_candidates.push_back(m_candidate);
                //std::cout << "Added a several candidate trigger " << i_candidate-michel_candidates_num << ","<< i_candidate<<std::endl;
                //std::cout << fabs(candidates.at(i_candidate).first_ch- prev_channel) << " Channel difference" << std::endl;
                michel_candidates_num = 0;
                
            }

            michel_candidates_num = 0;
        }
      
      
    }//check end channel of trigger candidates
      
      else{
          if (michel_candidates_num >0){
          //save first_time of the first TC and last_time for the last TC
		            m_candidate.first = candidates.at(i_candidate-michel_candidates_num-1).first_time;
                m_candidate.second = candidates.at(i_candidate-1).last_time;
                Michel_candidates.push_back(m_candidate);
                //std::cout << "Added a several candidate trigger " << i_candidate-michel_candidates_num << ","<< i_candidate<<std::endl;
                //std::cout << fabs(candidates.at(i_candidate).first_ch- prev_channel) << " Channel difference" << std::endl;
                michel_candidates_num = 0;
                         
                }
          michel_candidates_num = 0;
          
          
      }

      //gets the last channel and APA for the first entry
    if (fabs(candidates.at(i_candidate).last_ch-candidates.at(i_candidate).first_ch )< nChannels){
      prev_channel = candidates.at(i_candidate).last_ch;
      prev_time = candidates.at(i_candidate).last_time;
      last_APA = candidates.at(i_candidate).apanum;
    }
  }//finish looping through the trigger candidates

  //for(unsigned int i_mic
    //now look search through the TPs given their tstarts
    const auto tmp1 = Michel_candidates.at(0).first;
    const auto tmp2 = Michel_candidates.at(0).second;
    
    //is this part doable for tpsets?
    auto first_tp = std::find_if(tpset.tps.begin(),tpset.tps.end(),[tmp1](const TP& tp_1){return (double)tp_1.tstart == tmp1 ;});
    auto last_tp = std::find_if(tpset.tps.begin(),tpset.tps.end(),[tmp2](const TP& tp_2){return (double)tp_2.tstart == tmp2 ;});

    //std::cout << first_tp -tpset.tps.begin() <<std::endl;
    //std::cout << last_tp - tpset.tps.begin() << std::endl;
    
    //now we get the index of the first TP and the last TP from our possible decaying muon
    int tp1_index = (int)(first_tp -tpset.tps.begin());
    int tp2_index = (int)(last_tp - tpset.tps.begin());
    //std::cout << tp1_index <<std::endl;
    //std::cout << tp2_index << std::endl;
    
    std::vector<TriggerCandidate> trigger_hits;
    trigger_hits.reserve(tp2_index - tp1_index +1 );
    for (int i_tp = tp1_index; i_tp < tp2_index +1; i_tp++){
        trigger_hits.emplace_back(tpset.tps[i_tp]);
        std::cout <<trigger_hits.at(trigger_hits.size()-1).first_time << " " << trigger_hits.size()<< std::endl;
    }
    //now we proceed to calculate linearity and dqds..
    //i will add this 
    
  //return 0;
}
# ifndef __CINT__
int main(){
    newMichelTrigger();
    hello();
    return 0;
}

#endif

