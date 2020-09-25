/* UPFparser ver2.0
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cstring>


using namespace std;



struct domains{
	string DomainName = "";
	string SWpresent = "N";
	string DomainElements = "n/a";
	string swName = "n/a";
	string isolationCellname = "n/a";
	string swIn = "n/a";
	string swOut = "n/a";
	string swCtrl = "n/a";
	string swCtrl_port = "n/a";
	string swOut_port = "n/a";
	string swIn_port = "n/a";
	string DomainStatus = "n/a";
	string Iso_present = "N";
	string IsolationElements = "n/a";
	string IsolationClamp = "n/a";
	string involved_Supply_pads = "n/a";  //supply pads interacting with the domain
	string Primary_power_net = "n/a";
	string Primary_ground_net = "n/a";
	string Isolation_signal_status = "n/a"; 
	string Isolation_ctrl_signal = "n/a"; 
	string Isolation_sense = "n/a"; 
	bool more_than_one_iso_flag = false; 
	

	
};

struct supplies{
	string Name = "";
	string Status = "ON";
	
};

extern vector<domains> UPFinfo;
extern vector<supplies> Supply_pads;


//***********************function prototypes*********************
void readUPF(string upfpath, string filepath);
bool flagStatus(string text);
string eraseCharacter(string text);
std::vector<std::string> split_element(std::string s, char delimiter);
void tempo();

//***************************************************************



	void readUPF(string upfpath, string filepath){
	

	string fullpath = upfpath + filepath;
	cout << "\n\nUPF path: "<< fullpath << endl; //debug
     const char * my_name; 
	
	string text = "";
	string newText = "";
	ifstream upffile;
	int domcntr = -999;
	string swName = "";
	string isolationCellname = "";
	string swDomain = "";
	string isoDomain = "";
	string isoCtrlDomain = "";
	bool swFndflag = false; 
	bool isoFndflag = false; 
	bool isoCtrlflag = false; 
	bool pwrDmnFndflag = false;
	bool supply_net_flag = false; 
	bool load_file_flag = false;
	static bool load_scope = false;  
	string tmpDomName = ""; //domain name temporal	
	struct scope{
		string scope_name = "";
		int index = -999;   //1 means scope TOP
	};
	static int scp_cnt = 1; 
	static vector<scope> scope_list;
	static vector<string> tmp_connect;
	static int current_scp = 1;
	string supply_net = "";

	
	upffile.open(fullpath, ios::in);
	
	
	static vector<domains> result;
	static vector<string> new_upf;
	
	
	if(!upffile.is_open()){
		
		cout << "an ERROR ocurred trying to open the file " << fullpath <<endl;
		 
	} else{
		
		upffile >> text;
		while(!upffile.eof()){
			
		
		

			if(pwrDmnFndflag){
				pwrDmnFndflag = flagStatus(text);
			}		
			
			if(pwrDmnFndflag){
				
			
				if(text == "-elements"){
					
					bool elm_flag = true;
					
					while(elm_flag){
										
					upffile >> text; 
					
					if(text[text.length()-1] == '}'){
						
						elm_flag = false; 
						}
						
						
					newText = eraseCharacter(text);
					
				for(int j = 0; j < result.size();j++){
				
					
					
					if(result[j].DomainName == tmpDomName){
						
						if(result[j].DomainElements == "n/a"){
						result[j].DomainElements = newText;
						}else{
							
						result[j].DomainElements += string(",") + newText;	
							
						}
						}
				
					}	
					
					

				}					
					pwrDmnFndflag = false; 
					tmpDomName = "";
					
				}
				
				
			}
			
			if(text == "create_power_domain"){
				
				domains newFoundDomain;
				upffile >> text;
				newFoundDomain.DomainName = text;
				tmpDomName = text; 
				result.push_back(newFoundDomain); 
				pwrDmnFndflag = true; 
				
				if(load_scope == true){
					
					result[result.size()-1].DomainElements = scope_list[scope_list.size()-1].scope_name;
					load_scope = false; 
				}
			}
			
		if(swFndflag){
				swFndflag = flagStatus(text);
				if(!swFndflag){
					
				
					for(int r = 0; r < result.size(); r++){
						
						vector<string> split_sw_name = split_element(result[r].swName, ','); 
						vector<string> split_sw_in = split_element(result[r].swIn,',');
						vector<string> split_sw_out = split_element(result[r].swOut,',');
						vector<string> split_sw_out_port = split_element(result[r].swOut_port, ',');
						
						for(int s = 0; s < split_sw_name.size(); s++){
							
							if(split_sw_name[s] == swName){
								
								for(int t = 0; t < tmp_connect.size(); t++){
									
									 vector<string> split_info_connect = split_element(tmp_connect[t], ',');
									 split_info_connect = split_element(split_info_connect[split_info_connect.size() - 1],'.');
									 if(split_info_connect[split_info_connect.size()-1] == split_sw_in[s]){
										
										 string new_connection = tmp_connect[t] + ",sw."+to_string(current_scp)+string(".")+swName+string("/")+split_sw_out_port[s]+",s." + to_string(current_scp)+string(".")+split_sw_out[s];
										 tmp_connect.push_back(new_connection);
										 
									 }
									
									
								}
								
							
								
								
							}
							
						}
						
						
						
						
					}
					
				}
			}
			
			if(swFndflag){
				
				if(text == "-domain"){
			
				upffile >> 	swDomain; 
				swFndflag = flagStatus(text);
			
				
				for(int j = 0; j < result.size();j++){
				
				
					
					if(result[j].DomainName == swDomain){
						domcntr = j;
						
						result[j].SWpresent = "Y";
						if(result[j].swName == "n/a"){
						result[j].swName = swName; 
						}else{
							result[j].swName += string(",") + swName;
							
						}
						
						}
				
					}
				
				} else {
					
					if(text == "-output_supply_port") {
						upffile >> text;
						swFndflag = flagStatus(text);
						newText = eraseCharacter(text);
						if(result[domcntr].swOut_port == "n/a"){
						result[domcntr].swOut_port = newText;
						upffile >> text;
						swFndflag = flagStatus(text); 
						newText = eraseCharacter(text);
						result[domcntr].swOut = newText;
						}else{
							result[domcntr].swOut_port += string(",") + newText;
							upffile >> text;
							swFndflag = flagStatus(text); 
							newText = eraseCharacter(text);
							result[domcntr].swOut += string(",") + newText;
						}
					}else{
						
				
						
						if(text == "-control_port"){
							upffile >> text;
							swFndflag = flagStatus(text);
							newText = eraseCharacter(text);
							if(result[domcntr].swCtrl_port == "n/a"){
							result[domcntr].swCtrl_port = newText;
							upffile >> text;
							swFndflag = flagStatus(text); 
							newText = eraseCharacter(text);
							result[domcntr].swCtrl = newText;
							}else{
								result[domcntr].swCtrl_port += string(",") + newText;
								upffile >> text;
								swFndflag = flagStatus(text); 
								newText = eraseCharacter(text);
								result[domcntr].swCtrl += string(",") + newText;
							}
						} else {
							
							if(text == "-input_supply_port"){
								
								upffile >> text;
								swFndflag = flagStatus(text);
								newText = eraseCharacter(text);
								if(result[domcntr].swIn_port == "n/a"){
								result[domcntr].swIn_port = newText;
								upffile >> text;
								swFndflag = flagStatus(text); 
								newText = eraseCharacter(text);
								result[domcntr].swIn = newText;
								}else{
									result[domcntr].swIn_port += string(",") + newText;
									upffile >> text;
									swFndflag = flagStatus(text); 
									newText = eraseCharacter(text);
									result[domcntr].swIn += string(",") + newText;
									
								}
							}
							
						}
					}
				}
			
		
			}
			
			if(text == "create_power_switch"){
				
				swFndflag = true; 
				upffile >> swName;
				
			
			}
			
			if(isoFndflag){
				isoFndflag = flagStatus(text);
			}
	
			if(isoFndflag){
								
				if(text == "-domain"){
			
				upffile >> 	isoDomain; 
				isoFndflag = flagStatus(text);
				
				
				for(int j = 0; j < result.size();j++){
				
					
					
					if(result[j].DomainName == isoDomain){
						domcntr = j;
						result[j].Iso_present = "Y";
						if(result[j].isolationCellname == "n/a"){
						result[j].isolationCellname = isolationCellname; 
						} else {
					
								result[j].isolationCellname += string(",") + isolationCellname;
					
							}
						
						}
				
					}	
				}
				if(text == "-isolation_power_net"||(text == "-isolation_ground_net")){
					
					upffile >> text; 
					
				}
				if(text == "-clamp_value"){
					
			
					upffile >> text; 
					if(result[domcntr].IsolationClamp == "n/a"){
					result[domcntr].IsolationClamp = text;
					} else {
						result[domcntr].IsolationClamp += string(",") + text;
					}
							
						
				}
				
				if(text == "-elements"){
					bool elm_flag = true;
					if(result[domcntr].IsolationElements == "n/a"){
					result[domcntr].IsolationElements = "";
					}
					
					
					if(result[domcntr].more_than_one_iso_flag){
						
						result[domcntr].IsolationElements += "|";
					}
					
					while(elm_flag){
						
						upffile >> text; 
						
						
						if((text[0] == '{')||(text[0] == '\\')){
							
							continue;
						}
					
						
						if(text[text.length()-1] == '}'){
						
						elm_flag = false; 
						}
						
						text = eraseCharacter(text);
					
						result[domcntr].IsolationElements += string(" ") + text;
				}
				
				result[domcntr].more_than_one_iso_flag = true; 
					
			}
		}
			
			if(text == "set_isolation"){
				
				isoFndflag = true; 
				upffile >> isolationCellname;
				
			}
			
			if(isoCtrlflag){
				
				isoCtrlflag = flagStatus(text);
			}
			
			if(isoCtrlflag){

				if(text == "-domain"){
			
				upffile >> 	isoCtrlDomain; 
				isoCtrlflag = flagStatus(text);
				
				
				for(int j = 0; j < result.size();j++){
				
					
					
					if(result[j].DomainName == isoCtrlDomain){
						domcntr = j;
					
						
						}
				
					}	
				}	

				if(text == "-isolation_signal"){
				
					string isoCtrlSignal = ""; 
				
					upffile >> isoCtrlSignal; 
				
					if(result[domcntr].Isolation_ctrl_signal == "n/a"){
						result[domcntr].Isolation_ctrl_signal = isoCtrlSignal; 
						result[domcntr].Isolation_signal_status = "OFF";
					}else{
						
						result[domcntr].Isolation_ctrl_signal += string(",") + isoCtrlSignal;
						result[domcntr].Isolation_signal_status += string(",OFF");
					}
				
			

				}	

				if(text == "-isolation_sense"){
				
					string aux5 = "";
				
					upffile >> aux5; 
				
					if(result[domcntr].Isolation_sense == "n/a"){
						result[domcntr].Isolation_sense = aux5;
					}else{
						result[domcntr].Isolation_sense += string(",") + aux5;
					}
				
				
				}
			
			}
			
			if(text == "set_isolation_control"){
				
				isoCtrlflag = true; 
				
				upffile >> text;				
				
				
			
			}

			
			
			if (text == "set_scope"){
			
			scope tmp_scope; 
			
			
			upffile >> tmp_scope.scope_name; 
			tmp_scope.index = scp_cnt; 
			scp_cnt++;
			scope_list.push_back(tmp_scope); 
				
			
			}
			
			if (text == "create_supply_port"){
				
				string port_name = "";
				upffile >> port_name; 
			string  final_port_name = "p."+to_string(current_scp)+string(".")+port_name;
				tmp_connect.push_back(final_port_name);
				
			}
			
			
			if(supply_net_flag){
				supply_net_flag = flagStatus(text);
			}
			
			
			if(supply_net_flag){
				
				
				if(text == "-ports"){
				bool flag = true; 
				int loop_count = 0; 
					do{
					upffile >> text; 
					newText = eraseCharacter(text);
					
					vector<string> aux1 = split_element(newText,'/');
					
					vector<string> aux2; 
					
					if(aux1.size() > 1) {
						
						 int temp_index;
						 int orig_port_index; 
						
				
						
						for(int x = 0; x < scope_list.size(); x++){
							
							
							if(aux1[0] == scope_list[x].scope_name){
								
								temp_index = scope_list[x].index; 
								
							}
						}
						
						string port = "p."+to_string(temp_index)+string(".")+aux1[1]; 
						
						
					
						
						for(int w=0; w < tmp_connect.size(); w++){
							
							vector<string> temp4 = split_element(tmp_connect[w],',');
							
							if(port == temp4[0]){
								
					
								orig_port_index = w; 
							}
							
							
							
						}
						
						
						
						int temp_connect_size=0; 
						string final_connect = ""; 
						int final_index = 0; 
						for(int y= 0; y < tmp_connect.size(); y++){
							
							vector<string> temp2 = split_element(tmp_connect[y],',');
							vector<string> temp3 = split_element(temp2[temp2.size()-1],'.');
							
						
							
							if((temp3[2] == supply_net)&&(temp2.size() >= temp_connect_size)&& (temp3[1] == to_string(current_scp)||(loop_count > 0))){
								
								final_index = y; 
								temp_connect_size = temp2.size();
								
							}
							
						}
						final_connect = tmp_connect[final_index]+string(",")+tmp_connect[orig_port_index];
					
						aux2.push_back(tmp_connect[final_index]);
						tmp_connect[final_index] = final_connect; 
						loop_count++;
						
						
					} else{
					
					
					
					for(int a = 0; a < tmp_connect.size(); a++ ){
						
						vector<string> temp1 = split_element(tmp_connect[a],',');
						temp1 = split_element(temp1[0],'.');
						if((temp1[0] == "p") && (to_string(current_scp) == temp1[1]) &&(temp1[2]==newText)){
							
							
							tmp_connect[a] += string(",s.")+to_string(current_scp)+string(".")+supply_net;
							
						}
						
						
					}
					}
					
					for(int q = 0; q < aux2.size(); q++){
						
						tmp_connect.push_back(aux2[q]);
					}
					
					if('}' == text[text.length()-1]){ 
					flag = false;
					supply_net_flag = flagStatus(text);
					}
				}while(flag);
				
			}
			}
			
			
			if (text == "connect_supply_net"){
				
				
				upffile >> supply_net; 
				supply_net_flag = true; 
				
				
				
			}
			
			
			if(text == "set_domain_supply_net"){
				string tmp_domain_name = ""; 
				int aux_flag = 0; 
				upffile >> tmp_domain_name;
				for(int p = 0; p < result.size(); p++){
				
					if(result[p].DomainName == tmp_domain_name){
						
						do{
							
							upffile >> text; 
							
							if(text == "-primary_power_net"){
								
								upffile >> text; 
								
						
								result[p].Primary_power_net = text;
								aux_flag++;
								
							}
							
							if(text == "-primary_ground_net"){
								
								upffile >> text; 
						
								result[p].Primary_ground_net = text; 
								aux_flag++;
							}
							
							
							
						}while(aux_flag < 2);
						
						
						
					}
					
				}
				
				
				
				
			}
			
			if(load_file_flag){
				
				load_file_flag = flagStatus(text);
				
			}
			
			if(load_file_flag){
				
				if(text == "-scope"){
					
					load_scope = true; 
					scope tmp_scope;
					int org_scp = current_scp;  					
					if(scp_cnt == 1) {scp_cnt++;}
					upffile >> tmp_scope.scope_name;
					tmp_scope.index = scp_cnt;
					scp_cnt++;
					scope_list.push_back(tmp_scope);
					current_scp = scp_cnt - 1;
					
					readUPF(upfpath, new_upf[new_upf.size()-1]);
					current_scp = org_scp;
					
					
				}
				
				
			}
			
			if(text == "load_upf"){
				
			
			
			upffile >> text; 
			
		new_upf.push_back(text);
			
		
			load_file_flag = true; 
			
					
					
			
			}
		
			
			upffile >> text; 
			


			
		}
		
		
		
		
		
		 
	}
	
	
	
	//Logic to identify supply pads starts
	if(current_scp == 1){
	cout << "\n\n*******************************Identifying Supply Pads************************************" << endl; 
 	for(int a = 0; a < result.size(); a++){
		
		cout << "+++++++++++++++++++++++++++++++++++++++++++" << endl; 
		cout << "Domain: " << result[a].DomainName << endl; 
		string supply_pads = ""; 
		for(int b = 0; b < tmp_connect.size(); b++){
			
			vector<string> temp1 = split_element(tmp_connect[b], ','); 
			vector<string> temp2 = split_element(temp1[temp1.size()-1], '.');
			
			if((temp2[temp2.size()-1] == result[a].Primary_power_net)||(temp2[temp2.size()-1]== result[a].Primary_ground_net)){
				
				for(int c = temp1.size()-1; c > -1; c--){
					
					vector<string> temp3 = split_element(temp1[c], '.');
					
					if(((temp3[0] == "p")&&(temp3[1] == "1")) || (temp3[0]=="sw")){
						
												
					
						supply_pads += temp3[2] + string(",");
						supplies temp_spl;
						if( ::Supply_pads.size() == 0){
							temp_spl.Name = temp3[2];
							::Supply_pads.push_back(temp_spl);
							
						} else{
							bool repeat_data = false; 
							for(int d = 0; d < ::Supply_pads.size(); d++){
								
								 if(::Supply_pads[d].Name == temp3[2] ){
									repeat_data = true; 
									
								} 
								
								
							}
							
							if(repeat_data == false){
								
								temp_spl.Name = temp3[2];
								::Supply_pads.push_back(temp_spl);
								
								
							}
							
						}
						
					}
				}
				
			}
			
			
			
		}
		
 		supply_pads.pop_back(); 
		vector<string> aux3 = split_element(supply_pads, ',');
		
		
		vector<string> final_supply_pads;
		bool rep_data = false; 
		
		for(int f=0; f < aux3.size(); f++){
		
			if(final_supply_pads.size() == 0){
			
					final_supply_pads.push_back(aux3[f]);
			} else {
				
				for(int g = 0; g < final_supply_pads.size(); g++){
					
					if(aux3[f] == final_supply_pads[g] ){
						rep_data = true; 
					}
					
					
				}
				
				if(rep_data == false){
					
					final_supply_pads.push_back(aux3[f]);
					
				}
				
			}
		}
		
		string aux4 = ""; 
		for(int s = 0; s < final_supply_pads.size(); s++){
			
			cout << "Supply pad: " << final_supply_pads[s] << endl; 
			aux4+=final_supply_pads[s] + string(",");
		}

	
	
	
	aux4.pop_back(); 
		result[a].involved_Supply_pads = aux4; 
		cout << "Total_Supply_pads: " << aux4 << endl;  
			cout << "+++++++++++++++++++++++++++++++++++++++++++" << endl; 
	}
	 
	
	cout << "*******************************************************************" << endl; 
	
	
	
	
	//Logic to indetify supply pads ends 
	
	
	cout << "\n\n ***************** Temporal Connections ********************************" << endl; 
	for(int o=0; o < tmp_connect.size(); o++){
		
		cout << "----------> position: " << o << " | value: " << tmp_connect[o] << endl; 
		
	}
	
	
	cout << "****************************************************************************" << endl; 
	
	cout << "***************************** Supply pads Vector****************************" << endl; 
	
	for(int h = 0; h < ::Supply_pads.size(); h++){
		
		cout << "pad_name: " << ::Supply_pads[h].Name << " Value: " << ::Supply_pads[h].Status << endl; 
	}
	
	
	cout << "****************************************************************************" << endl;
	
	
	cout << "\n\n*************************** Scopes used in the UPF solution **********************" << endl; 
	
	for(int z = 0; z < scope_list.size(); z++){
		
		cout << "Scope index: " << scope_list[z].index << " , Scope name: " << scope_list[z].scope_name << endl; 
		
	}
	
	cout << "******************************************************************************" << endl;
	}
	upffile.close();
	::UPFinfo = result; 
	
		
}


/*This function enables a flag when the argument is complex, and requeries more than one word, for example: -output_supply_port {swout VDDsw} 
then, flag is true with { and false after }



*/
bool flagStatus(string text){
	
	
	bool status = true; 
	char strgbegin = text[0];
	char strgend = text[text.length()-1];
	static bool aux1 = false;
	
	
	
	if(strgbegin == '{'){
		aux1 = true;
		
	}
	

	if((!(strgend == '}'))&&(aux1 == false)){
	
	if ((((!(strgbegin == '\\'))&&(!(strgbegin == '-'))&&(aux1 == false)))){
		
		status = false;
		
	}
	}
	
	if(strgend == '}'){
		aux1 = false;
		
	}

	return status; 
}



/*
this function removes the unnecesary characters from the beginning ('{') or the end of the string ('}')


*/
string eraseCharacter(string text){

	char aux2 = text[text.length()-1];
	char first_char = text[0];
	string newString = ""; 
	
	if (aux2 == '}'){
		text.erase(text.length()-1);
	}
	if(first_char == '{'){
		text = text.substr(1,text.length());
	}
	
	
	return text;
}


	std::vector<std::string> split_element(std::string s, char delimiter){

		std::vector<std::string> split_string;
		std::string aux1; 
		std::istringstream tokenStream(s);
		while (getline(tokenStream, aux1, delimiter))
		{
			split_string.push_back(aux1);
		}	
		
		return split_string; 
	}
	



void printUPFInfo(){
	
	cout << "\n***************************************************  Information of each found domain  ***********************************************************" << endl;
	
	for(int i=0; i < ::UPFinfo.size() ; i++)
	{
		cout << "Domain " << i << " \n\n\tName: "<< ::UPFinfo[i].DomainName <<" \n\tElements: "<<::UPFinfo[i].DomainElements <<" \n\tSwitch?: "<< ::UPFinfo[i].SWpresent<< \
		" \n\tSW_Name: "<< ::UPFinfo[i].swName<<"\n\tSWsignal_OUT: "<<::UPFinfo[i].swOut<<"\n\tSWsignal_Ctrl: "<< ::UPFinfo[i].swCtrl<< "\n\tSW_out_port: "<< ::UPFinfo[i].swOut_port<<" \n\tIsolation?: " <<::UPFinfo[i].Iso_present<< \
		" \n\tIsolation Elements: " << ::UPFinfo[i].IsolationElements << "\n\tIsolationClamp: " <<::UPFinfo[i].IsolationClamp << " \n\tPrimary power net: "<<::UPFinfo[i].Primary_power_net<<" \n\tPrimary ground net: "<< \
		::UPFinfo[i].Primary_ground_net<<"\n\tInvolved supply pads: " <<::UPFinfo[i].involved_Supply_pads<< "\n\tSWsigmal_IN: " << ::UPFinfo[i].swIn << "\n\tIsolation Control Signal: " << ::UPFinfo[i].Isolation_ctrl_signal << \
		"  \n\tIsolation Sense:" << ::UPFinfo[i].Isolation_sense << " \n\t Isolation Cells name: " << ::UPFinfo[i].isolationCellname << " \n\t IsolationSignalStatus: "<< ::UPFinfo[i].Isolation_signal_status << endl; 
	}

	cout << "***************************************************************************************************************************************************\n"<<endl;
 
    
	
	
	
}

