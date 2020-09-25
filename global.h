
#ifndef UPFGLB_H
#define UPFGLB_H

#include "systemc.h"
#include <mutex>

// ***********************************  global variables   ****************************************************

vector<domains> UPFinfo;

bool UPFenable = 1; 

std::mutex mutex1;  //used to turn on/off PS
std::mutex mutex2;  //used in isolation logic
sc_event upf_event;



vector<supplies> Supply_pads;



void supply_off(char const * supplypad, double volt = 1.0){
	
	std:lock_guard<mutex> lock(mutex1);
	
	for(int i = 0; i < ::Supply_pads.size(); i++){
		
		if(supplypad == ::Supply_pads[i].Name){
			
			::Supply_pads[i].Status = "OFF"; 
			
			::upf_event.notify();
		}
	}
	
	
	
}


void supply_on(char const * supplypad, double volt = 1.0){
	
		std:lock_guard<mutex> lock(mutex1);
	
	for(int i = 0; i < ::Supply_pads.size(); i++){
		
		if(supplypad == ::Supply_pads[i].Name){
			
			::Supply_pads[i].Status = "ON"; 
			::upf_event.notify();
		}
	}
	
	
	
}



//*************************************************************************************************************


#endif