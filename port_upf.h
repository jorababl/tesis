
#include <string>
#include <vector>
#include <sstream>
#include <mutex>






template<typename T>
class sc_in_upf : public sc_channel
{
	
	private: 
	 bool sw_input; 
	 bool iso_ctrl_input; 
	 int domain_index; 
	 int sw_index;
	 string sw_name;
	 string sw_out;
	 string sw_out_port; 
	 vector<string> domain_supply_pads;
	 vector<string> temp_sw_names;
	 vector<string> temp_iso_signal_names; 
	char Isolated_in = 'N'; 
	string module_name = ""; 
	string port_name = ""; 
	string clamp_value = "";
	std::vector<string> ISO_elements;
	std::vector<string> dom_elements;
	int pos_in_iso_vector;
	int iso_element_group;
	string isolation_sense = ""; 
	std::vector<string> isolation_signal_sts;
	std::vector<string> iso_elements_vector;
	std::string isolation_status = "";
	bool upf_flag = false; 
	 
	std::string split(std::string s, char delimiter, int position){   //returns the name of the module, using (*this.name as input)

		std::vector<std::string> split_string;
		std::string aux1; 
		std::istringstream tokenStream(s);
		while (getline(tokenStream, aux1, delimiter))
		{
			split_string.push_back(aux1);
		}	
		
		if(position == -1){
			
			position = split_string.size() - 1; 
		}else{
			
			if(position == -2){
				
				position = split_string.size() - 2; 
				
			}
		}
		
		return split_string[position]; 
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
	
	public:
	
	sc_in<T> input; 
	sc_signal<T> signal; 
	
	SC_HAS_PROCESS(sc_in_upf);
	sc_in_upf(const sc_module_name& nmod) : sc_channel(nmod){
	
	
	sw_input = false;
	for(int i=0; i < ::UPFinfo.size(); i++ ){
		temp_sw_names = split_element(::UPFinfo[i].swCtrl,',');
		for(int j=0; j < temp_sw_names.size(); j++){
		if(temp_sw_names[j] == split((*this).name(),'.',-1)){
			
			sw_input = true;
			domain_index = i; 
			sw_index = j; 
			sw_name = split(::UPFinfo[i].swName,',',j);
			sw_out = split(::UPFinfo[i].swOut_port,',',j);
			
			 if(split((*this).name(),'.',-2) != ::UPFinfo[i].DomainElements){
				
				cout << "Warning: the SW " << temp_sw_names[j]<< " was not declared in the same power domain/element used in UPF file " << endl; 
				
			} 
		}
	}
	}
	
	iso_ctrl_input = false; 
	for(int i=0; i < ::UPFinfo.size(); i++ ){
		temp_iso_signal_names = split_element(::UPFinfo[i].Isolation_ctrl_signal,',');
		for(int j=0; j < temp_iso_signal_names.size(); j++){
			if(temp_iso_signal_names[j] == split((*this).name(),'.',-1)){
				
				
				iso_ctrl_input = true; 
				domain_index = i; 
				pos_in_iso_vector = j;
				isolation_sense = split(::UPFinfo[domain_index].Isolation_sense,',',j);
				
			}
			
		}
	}
	 
	if(iso_ctrl_input == true){
		SC_METHOD(iso_ctrl_signal_status);
		sensitive << input;
		
	} else{
		if(sw_input == false){
			SC_METHOD(write_input);
			sensitive << upf_event << input; 
			
		} else{
			SC_METHOD(ctrl_signal_status);
			sensitive << input;
			
		}
	}
	
	
	module_name =  split((*this).name(),'.',-2);
	port_name = split((*this).name(),'.',-1);
	
		for(int i=0; i < ::UPFinfo.size(); i++ ){
			
 			dom_elements=split_element(::UPFinfo[i].DomainElements,',');
			
			for(int j = 0; j < dom_elements.size(); j++){
				
				if(module_name == dom_elements[j]){
					
					iso_elements_vector = split_element(::UPFinfo[i].IsolationElements,'|');
					
				
					
					for(int l = 0; l < iso_elements_vector.size(); l++){
				
				ISO_elements = split_element(iso_elements_vector[l],' ');
				
								
				for(int k = 0; k < ISO_elements.size(); k++){
					
	
					if(ISO_elements[k] == port_name){
						
						Isolated_in = 'Y'; 
						clamp_value = split(::UPFinfo[i].IsolationClamp,',',l);
						iso_element_group = l;
						 
					}
					
				}
				
				}
												
				}
			}
							
		}
	
	
     info();
	
	}
	
	~sc_in_upf(){}
	
	
	void write_input(){

		std::vector<std::string> elements;
		std::string mod_name = split((*this).name(),'.',-2);
		std::string domain_state = "ON";
		upf_flag = false; 
		
		for(int i=0; i < ::UPFinfo.size(); i++ ){
			
 			elements=split_element(::UPFinfo[i].DomainElements,',');
			
			for(int j = 0; j < elements.size(); j++){
				
				if(mod_name == elements[j]){
					
					domain_index = i; 
					
					
				domain_supply_pads = split_element(::UPFinfo[i].involved_Supply_pads,',');
						for(int k=0; k < domain_supply_pads.size(); k++){
			
			
			
							for(int l=0; l < ::Supply_pads.size(); l++){
				
				
				
								if((domain_supply_pads[k] == ::Supply_pads[l].Name)&&(::Supply_pads[l].Status == "OFF")){
					
								domain_state = "OFF";
								upf_flag = true; 
												
								}
				
							}
			
			
						}
												
				}
			}
							
		}
		
		isolation_status = split(::UPFinfo[domain_index].Isolation_signal_status,',',iso_element_group); 
		
		if((domain_state == "OFF")&&(::UPFenable == 1)){
			
			if(Isolated_in == 'N'){
			
				signal = 0; 
			
			} else {
				
				if((clamp_value == "1")&&(isolation_status == "ON")){
				
					signal = 1; 
				
				} else {
					
					if((clamp_value == "0")&&(isolation_status == "ON")){
					
						signal = 0; 
					
					} else{
						
						signal = 0;
						
					}
					
				}
			}
			 
		}else{
			
			if((::UPFenable == 1)&&(isolation_status == "ON") && Isolated_in == 'Y'){
				
				if(clamp_value == "1"){
					
					signal = 1;
					
				}else{
					
					signal = 0; 
					
				}
				
			}else{
			signal = input;
		}
		
		
		
	}
	
	}
		
	T read(){
		
		if(upf_flag == true){
			
		return signal.read(); 
		} else {
			
			return input.read();
		}
		
	}

	void info(){
	//cout << "name: "<<(*this).name() << " kind: " << (*this).kind() << endl; 
	}
	const char* kind() const
        {
			
			const char* kind; 
			if(iso_ctrl_input == true){
				
				kind = "upf_Iso_control_signal";
				
			} else{
				if(sw_input == true){
					
					kind = "upf_switch_control" ;
					
				} else{
					kind = "sc_in_upf";
;				}
			}
			
			return kind;
			
			
			}
		

	void ctrl_signal_status(){
		
		if(input.read() == 0){
			
		
		
		sw_out_port = sw_name+"/"+sw_out;
		
	
		for(int i=0; i < ::Supply_pads.size() ; i++){
			
			if(::Supply_pads[i].Name == sw_out_port){
				
				::Supply_pads[i].Status = "OFF";
			
			}
			
		}
		
		::upf_event.notify();
		
		
			
		}else{
			if(input.read() == 1){
			
		
		
		sw_out_port = sw_name+"/"+sw_out;
		for(int i=0; i < ::Supply_pads.size() ; i++){
			
			if(::Supply_pads[i].Name == sw_out_port){
				
				::Supply_pads[i].Status = "ON";
			
			}
			
		}		
		
		::upf_event.notify();
			
		}
	
	}
	
	}
	
	void iso_ctrl_signal_status() {
		
		std:lock_guard<mutex> lock(mutex2);
		
		string final_iso_status = "";
		
		isolation_signal_sts = split_element(::UPFinfo[domain_index].Isolation_signal_status,',');
			
		if((input.read() == 1 )&&(isolation_sense == "high")){
			
			isolation_signal_sts[pos_in_iso_vector] = "ON";
			
			for(int i = 0; i < isolation_signal_sts.size(); i++){
				
				if(i > 0){
					final_iso_status += string(",");
				}
				
				final_iso_status += isolation_signal_sts[i];
				
			}
		
			::UPFinfo[domain_index].Isolation_signal_status = final_iso_status;
			
		
		} else {
			if((input.read() == 0 )&&(isolation_sense == "low")){
				
				isolation_signal_sts[pos_in_iso_vector] = "ON";
				
				for(int i = 0; i < isolation_signal_sts.size(); i++){
				
				if(i > 0){
					final_iso_status += string(",");
				}
				
				final_iso_status += isolation_signal_sts[i];
				
			}
		
			::UPFinfo[domain_index].Isolation_signal_status = final_iso_status;
				
				
				
			}
			
			
		else{
			isolation_signal_sts[pos_in_iso_vector] = "OFF";
			for(int i = 0; i < isolation_signal_sts.size(); i++){
				
				if(i > 0){
					final_iso_status += string(",");
				}
				
				final_iso_status += isolation_signal_sts[i];
				
			}
		
			::UPFinfo[domain_index].Isolation_signal_status = final_iso_status;
			
			
		}
	}
	
	
	::upf_event.notify();
	}
};


template<>
class sc_in_upf <sc_dt::sc_logic> : public sc_channel
{
	private: 
	 bool sw_input; 
	 int domain_index; 
	 string sw_out_port; 
	 vector<string> domain_supply_pads;
	 char Isolated_in = 'N'; 
	string module_name = ""; 
	string port_name = ""; 
	string clamp_value = "";
	std::vector<string> ISO_elements;
	std::vector<string> dom_elements;
	int iso_element_group;
	std::vector<string> isolation_signal_sts;
	std::vector<string> iso_elements_vector;
	std::string isolation_status = "";
	bool upf_flag = false; 
	 
	std::string split(std::string s, char delimiter, int position){   //returns the name of the module, using (*this.name as input)

		std::vector<std::string> split_string;
		std::string aux1; 
		std::istringstream tokenStream(s);
		while (getline(tokenStream, aux1, delimiter))
		{
			split_string.push_back(aux1);
		}	
		
				if(position == -1){
			
			position = split_string.size() - 1; 
		}else{
			
			if(position == -2){
				
				position = split_string.size() - 2; 
				
			}
		}
		
		return split_string[position]; 
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
	
	public:
	
	sc_in<sc_logic> input; 
	sc_signal<sc_logic> signal; 
	
	SC_HAS_PROCESS(sc_in_upf);
	sc_in_upf(const sc_module_name& nmod) : sc_channel(nmod){
	
	
	sw_input = false;
	for(int i=0; i < ::UPFinfo.size(); i++ ){
		if(::UPFinfo[i].swCtrl == split((*this).name(),'.',-1)){
			
			sw_input = true;
			domain_index = i; 
			
		}
	}
	 
	if(sw_input == false){
		SC_METHOD(write_input);
		sensitive << upf_event << input; 
	
	} else{
		SC_METHOD(ctrl_signal_status);
		sensitive << input;
		
	}
	
		module_name =  split((*this).name(),'.',-2);
		port_name = split((*this).name(),'.',-1);
	
		for(int i=0; i < ::UPFinfo.size(); i++ ){
			
 			dom_elements=split_element(::UPFinfo[i].DomainElements,',');
			
			for(int j = 0; j < dom_elements.size(); j++){
				
				if(module_name == dom_elements[j]){
					
				iso_elements_vector = split_element(::UPFinfo[i].IsolationElements,'|');
					
					
					
					for(int l = 0; l < iso_elements_vector.size(); l++){	
				
				ISO_elements = split_element(iso_elements_vector[l],' ');
				
								
				for(int k = 0; k < ISO_elements.size(); k++){
					
	
					if(ISO_elements[k] == port_name){
						
						Isolated_in = 'Y'; 
						clamp_value = split(::UPFinfo[i].IsolationClamp,',',l);
						iso_element_group = l;
						
						domain_index = i; 
						
					}
					
				}
				
				}
												
				}
			}
							
		}
	
     info();
	
	}
	
	~sc_in_upf(){}
	
	
	void write_input(){

		std::vector<std::string> elements;
		std::string mod_name = split((*this).name(),'.',-2);
		std::string domain_state = "ON";
		upf_flag = false; 
		
		for(int i=0; i < ::UPFinfo.size(); i++ ){
			
 			elements=split_element(::UPFinfo[i].DomainElements,',');
			
			for(int j = 0; j < elements.size(); j++){
				
				if(mod_name == elements[j]){
				
				
				domain_supply_pads = split_element(::UPFinfo[i].involved_Supply_pads,',');
						for(int k=0; k < domain_supply_pads.size(); k++){
			
			
			
							for(int l=0; l < ::Supply_pads.size(); l++){
				
				
				
								if((domain_supply_pads[k] == ::Supply_pads[l].Name)&&(::Supply_pads[l].Status == "OFF")){
					
								domain_state = "OFF";
								upf_flag = true; 
								
					
								}
				
							}
			
			
						}
												
				}
			}
							
		}
		
		isolation_status = split(::UPFinfo[domain_index].Isolation_signal_status,',',iso_element_group); 
		
		if((domain_state == "OFF")&&(::UPFenable == 1)){
			
			if(Isolated_in == 'N'){
			
			signal = SC_LOGIC_X; 
			} else {
				if((clamp_value == "1")&&(isolation_status == "ON")){
				
					signal = SC_LOGIC_1; 
				
				} else {
					
					if((clamp_value == "0")&&(isolation_status == "ON")){
					
						signal = SC_LOGIC_0; 
					
					} else{
						
						signal = SC_LOGIC_X;
						
					}
					
				}
				
				
			}
			 
		}else{
			
			if((::UPFenable == 1)&&(isolation_status == "ON") && Isolated_in == 'Y'){
				
				if(clamp_value == "1"){
					
					signal = SC_LOGIC_1;
					
				}else{
					
					signal = SC_LOGIC_0; 
					
				}
				
			}else{
			signal = input;
		}
			
		}
		
		
		
	}
		
	sc_logic read(){
		
		if(upf_flag == true){
		return signal.read(); 
		}else{
			return input.read();
		}
		
	}

	void info(){
	//cout << "name: "<<(*this).name() << " kind: " << (*this).kind() << endl; 
	}
	const char* kind() const
        {return (sw_input == true)? "upf_switch_control" : "sc_in_upf"; }
		

	void ctrl_signal_status(){
		
		if(input.read() == SC_LOGIC_0){
			
		
		
		sw_out_port = ::UPFinfo[domain_index].swName+"\\"+::UPFinfo[domain_index].swOut_port;
		
		for(int i=0; i < ::Supply_pads.size() ; i++){
			
			if(::Supply_pads[i].Name == sw_out_port){
				
				::Supply_pads[i].Status = "OFF";
			
			}
			
		}
		
		::upf_event.notify();
		
		
			
		}else{
			if(input.read() == SC_LOGIC_1){
			
		
		
		sw_out_port = ::UPFinfo[domain_index].swName+"\\"+::UPFinfo[domain_index].swOut_port;
		for(int i=0; i < ::Supply_pads.size() ; i++){
			
			if(::Supply_pads[i].Name == sw_out_port){
				
				::Supply_pads[i].Status = "ON";
			
			}
			
		}		
		
		::upf_event.notify();
			
		}
	
	}
	
	}

		

	
};

template<int U>

class sc_in_upf < sc_dt::sc_lv<U>> : public sc_channel
{
	private: 
	
	 int domain_index; 
	 string sw_out_port; 
	 vector<string> domain_supply_pads;
	 char Isolated_in = 'N'; 
	string module_name = ""; 
	string port_name = ""; 
	string clamp_value = "";
	std::vector<string> ISO_elements;
	std::vector<string> dom_elements;
	int iso_element_group;
	std::vector<string> isolation_signal_sts;
	std::vector<string> iso_elements_vector;
	std::string isolation_status = "";
	bool upf_flag = false; 
	
	std::string split(std::string s, char delimiter, int position){   //returns the name of the module, using (*this.name as input)

		std::vector<std::string> split_string;
		std::string aux1; 
		std::istringstream tokenStream(s);
		while (getline(tokenStream, aux1, delimiter))
		{
			split_string.push_back(aux1);
		}	
		
		if(position == -1){
			
			position = split_string.size() - 1; 
		}else{
			
			if(position == -2){
				
				position = split_string.size() - 2; 
				
			}
		}
		
		return split_string[position]; 
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
	
	public:
	
	sc_in<sc_lv<U>> input; 
	sc_signal<sc_lv<U>> signal; 
	
	SC_HAS_PROCESS(sc_in_upf);
	sc_in_upf(const sc_module_name& nmod) : sc_channel(nmod){
	
	
 	for(int i=0; i < ::UPFinfo.size(); i++ ){
		if(::UPFinfo[i].swCtrl == split((*this).name(),'.',-1)){
			
			domain_index = i; 
			
		}
	} 
	 
	
		SC_METHOD(write_input);
		sensitive << upf_event << input; 
	
	
 		module_name =  split((*this).name(),'.',-2);
		port_name = split((*this).name(),'.',-1);
	
		for(int i=0; i < ::UPFinfo.size(); i++ ){
			
 			dom_elements=split_element(::UPFinfo[i].DomainElements,',');
			
			for(int j = 0; j < dom_elements.size(); j++){
				
				if(module_name == dom_elements[j]){
					iso_elements_vector = split_element(::UPFinfo[i].IsolationElements,'|');
					
					
					
					for(int l = 0; l < iso_elements_vector.size(); l++){
				
				ISO_elements = split_element(iso_elements_vector[l],' ');
				
								
				for(int k = 0; k < ISO_elements.size(); k++){
					
	
					if(ISO_elements[k] == port_name){
						
						Isolated_in = 'Y'; 
						clamp_value = split(::UPFinfo[i].IsolationClamp,',',l);
						iso_element_group = l;
						
						domain_index = i; 
						 
					}
					
				}
				
				}
												
				}
			}
							
		} 
	
     info();
	
	}
	
	~sc_in_upf(){}
	
	
	void write_input(){
 
		std::vector<std::string> elements;
		std::string mod_name = split((*this).name(),'.',-2);
		std::string domain_state = "ON";
		upf_flag = false; 
		
		for(int i=0; i < ::UPFinfo.size(); i++ ){
			
 			elements=split_element(::UPFinfo[i].DomainElements,',');
			
			for(int j = 0; j < elements.size(); j++){
				
				if(mod_name == elements[j]){
				
				
				domain_supply_pads = split_element(::UPFinfo[i].involved_Supply_pads,',');
						for(int k=0; k < domain_supply_pads.size(); k++){
			
			
			
							for(int l=0; l < ::Supply_pads.size(); l++){
				
				
				
								if((domain_supply_pads[k] == ::Supply_pads[l].Name)&&(::Supply_pads[l].Status == "OFF")){
					
								domain_state = "OFF";
								upf_flag = true; 
								
					
								}
				
							}
			
			
						}
												
				}
			}
							
		}
		
		
		isolation_status = split(::UPFinfo[domain_index].Isolation_signal_status,',',iso_element_group); 
		
		if((domain_state == "OFF")&&(::UPFenable == 1)){
			
			if(Isolated_in == 'N'){
			
			signal = sc_lv<U>(sc_logic('x')); 
			} else {
				if((clamp_value == "1")&&(isolation_status == "ON")){
				
					signal = sc_lv<U>(sc_logic('1')); 
				
				} else {
					
					if((clamp_value == "0")&&(isolation_status == "ON")){
					
						signal = sc_lv<U>(sc_logic('0')); 
					
					} else{
						
						signal = sc_lv<U>(sc_logic('X'));
						
					}
					
				}
				
				
			}
			 
		}else{
			
			if((::UPFenable == 1)&&(isolation_status == "ON") && Isolated_in == 'Y'){
				
				if(clamp_value == "1"){
					
					signal = sc_lv<U>(sc_logic('1'));
					
				}else{
					
					signal = sc_lv<U>(sc_logic('0')); 
					
				}
				
			}else{
			signal = input;
		}
			
		}
		
		
		
	}
		
	sc_lv<U> read(){
		
		if(upf_flag == true){
		
		return signal.read();
		}else{
		return input.read(); 
		}			
		
	}

	 void info(){
	//cout << "name: "<<(*this).name() << " kind: " << (*this).kind() << endl; 
	}
	const char* kind() const
        {return  "sc_in_upf"; } 
		



	
};

template<typename T>

class sc_out_upf : public sc_channel
{
	
	private: 
	
	char Isolated_out = 'N'; 
	string module_name = ""; 
	string port_name = ""; 
	string clamp_value = "";
	vector<string> domain_supply_pads;
	
	std::vector<string> ISO_elements;
	std::vector<string> dom_elements;
	int domain_index; 
	int iso_element_group;
	string isolation_sense = ""; 
	std::vector<string> isolation_signal_sts;
	std::vector<string> iso_elements_vector;
	std::string isolation_status = "";
	
	std::string split(std::string s, char delimiter, int position){

		std::vector<std::string> split_string;
		std::string aux1; 
		std::istringstream tokenStream(s);
		while (getline(tokenStream, aux1, delimiter))
		{
			split_string.push_back(aux1);
		}	
		
		if(position == -1){
			
			position = split_string.size() - 1; 
		}else{
			
			if(position == -2){
				
				position = split_string.size() - 2; 
				
			}
		}
		
		
		return split_string[position]; 
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
		
	public:
	
	sc_out<T> out; 
	
	
	SC_HAS_PROCESS(sc_out_upf);
	sc_out_upf(const sc_module_name& nmod ) : sc_channel(nmod){
	info();	
	SC_METHOD(writeUPFevent);
	sensitive << upf_event; 
	
	
	
	module_name =  split((*this).name(),'.',-2);
	port_name = split((*this).name(),'.',-1);
	
			for(int i=0; i < ::UPFinfo.size(); i++ ){
			
 			dom_elements=split_element(::UPFinfo[i].DomainElements,',');
			
			for(int j = 0; j < dom_elements.size(); j++){
				
				if(module_name == dom_elements[j]){
					
					iso_elements_vector = split_element(::UPFinfo[i].IsolationElements,'|');
					
				
					
					for(int l = 0; l < iso_elements_vector.size(); l++){
				
				ISO_elements = split_element(iso_elements_vector[l],' ');
				
								
				for(int k = 0; k < ISO_elements.size(); k++){
					
	
					if(ISO_elements[k] == port_name){
						
						Isolated_out = 'Y'; 
						clamp_value = split(::UPFinfo[i].IsolationClamp,',',l);
						iso_element_group = l;
						
						domain_index = i; 
						
					}
					
				}
				
				}
												
				}
			}
							
		}
	
		
	}
	
	void write(T value){
		
		std::vector<string> elements;
		std::string mod_name = split((*this).name(),'.',-2);
		std::string domain_state = "ON";
		
		for(int i=0; i < ::UPFinfo.size(); i++ ){
			
 			elements=split_element(::UPFinfo[i].DomainElements,',');
			
			for(int j = 0; j < elements.size(); j++){
				
				if(mod_name == elements[j]){
				
				
				domain_supply_pads = split_element(::UPFinfo[i].involved_Supply_pads,',');
						for(int k=0; k < domain_supply_pads.size(); k++){
			
			
			
							for(int l=0; l < ::Supply_pads.size(); l++){
				
				
				
								if((domain_supply_pads[k] == ::Supply_pads[l].Name)&&(::Supply_pads[l].Status == "OFF")){
					
								domain_state = "OFF";
								
					
								}
				
							}
			
			
						}
												
				}
			}
							
		}
		
		isolation_status = split(::UPFinfo[domain_index].Isolation_signal_status,',',iso_element_group); 
		
		if((domain_state == "OFF")&&(::UPFenable == 1)){
			
			if(Isolated_out == 'N'){
			
			out.write(0);
			} else {
				
				if((clamp_value == "1")&&(isolation_status == "ON")){
				
					out.write(1);
				
				} else {
					
					if((clamp_value == "0")&&(isolation_status == "ON")){
					
						out.write(0);
					
					} else{
						
						out.write(0);
						
					}
					
				}
				
				
				
			}
			
		}else{
			
			if((::UPFenable == 1)&&(isolation_status == "ON") && Isolated_out == 'Y'){
				
				if(clamp_value == "1"){
					
					out.write(1);
					
				}else{
					
					out.write(0); 
					
				}
				
			}else{
			out.write(value);
		}
			
				
		}
		
	}
	
		void writeUPFevent(){
	
		std::vector<string> elements;
		std::string mod_name = split((*this).name(),'.',-2);
		std::string domain_state = "";
		
		for(int i=0; i < ::UPFinfo.size(); i++ ){
			
 			elements=split_element(::UPFinfo[i].DomainElements,',');
			
			for(int j = 0; j < elements.size(); j++){
				
				if(mod_name == elements[j]){
				
				
				domain_supply_pads = split_element(::UPFinfo[i].involved_Supply_pads,',');
						for(int k=0; k < domain_supply_pads.size(); k++){
			
			
			
							for(int l=0; l < ::Supply_pads.size(); l++){
				
				
				
								if((domain_supply_pads[k] == ::Supply_pads[l].Name)&&(::Supply_pads[l].Status == "OFF")){
					
								domain_state = "OFF";
								
					
								}
				
							}
			
			
						}
												
				}
			}
							
		}
		
		isolation_status = split(::UPFinfo[domain_index].Isolation_signal_status,',',iso_element_group);
		
		if((domain_state == "OFF")&&(::UPFenable == 1)){
			
			if(Isolated_out == 'N'){
		
			out.write(0);
			}else{
				
				if((clamp_value == "1")&&(isolation_status == "ON")){
				
					out.write(1);
				
				} else {
					
					if((clamp_value == "0")&&(isolation_status == "ON")){
					
						out.write(0);
					
					} else{
						
						out.write(0);
						
					}
					
				}
				
								
			
			}
		} else {
			
				if((::UPFenable == 1)&&(isolation_status == "ON") && Isolated_out == 'Y'){
				
				if(clamp_value == "1"){
					
					out.write(1);
					
				}else{
					
					out.write(0); 
					
				}
				
			}
			
		}
		
	}
	

	
	T read(){
		return out.read();
	}
	
	void info(){
	//cout << "name: "<<(*this).name() << " kind: " << (*this).kind() << endl; 
	}
	
	const char* kind() const
        { return "sc_out_upf"; }
		
	
	

};

template<>

class sc_out_upf <sc_dt::sc_logic> : public sc_channel 
{

	private: 
	
	char Isolated_out = 'N'; 
	string module_name = ""; 
	string port_name = ""; 
	string clamp_value = "";
	vector<string> domain_supply_pads;
	
	std::vector<string> ISO_elements;
	std::vector<string> dom_elements;
	int domain_index; 
	int iso_element_group;
	string isolation_sense = ""; 
	std::vector<string> isolation_signal_sts;
	std::vector<string> iso_elements_vector;
	std::string isolation_status = "";
	
	std::string split(std::string s, char delimiter, int position){

		std::vector<std::string> split_string;
		std::string aux1; 
		std::istringstream tokenStream(s);
		while (getline(tokenStream, aux1, delimiter))
		{
			split_string.push_back(aux1);
		}	
		
		if(position == -1){
			
			position = split_string.size() - 1; 
		}else{
			
			if(position == -2){
				
				position = split_string.size() - 2; 
				
			}
		}
		
		
		return split_string[position]; 
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
		
	public:
	
	sc_out<sc_logic> out; 
	
	
	SC_HAS_PROCESS(sc_out_upf);
	sc_out_upf(const sc_module_name& nmod ) : sc_channel(nmod){
	info();	
	SC_METHOD(writeUPFevent);
	sensitive << upf_event; 
	
	
	
	module_name =  split((*this).name(),'.',-2);
	port_name = split((*this).name(),'.',-1);
	
			for(int i=0; i < ::UPFinfo.size(); i++ ){
			
 			dom_elements=split_element(::UPFinfo[i].DomainElements,',');
			
			for(int j = 0; j < dom_elements.size(); j++){
				
				if(module_name == dom_elements[j]){
					
					iso_elements_vector = split_element(::UPFinfo[i].IsolationElements,'|');
					
				
					
					for(int l = 0; l < iso_elements_vector.size(); l++){
				
				ISO_elements = split_element(iso_elements_vector[l],' ');
				
								
				for(int k = 0; k < ISO_elements.size(); k++){
					
	
					if(ISO_elements[k] == port_name){
						
						Isolated_out = 'Y'; 
						clamp_value = split(::UPFinfo[i].IsolationClamp,',',l);
						iso_element_group = l;
						
						domain_index = i; 
						 
					}
					
				}
				
				}
												
				}
			}
							
		}
	
		
	}
	
	void write(sc_logic value){
		
		std::vector<string> elements;
		std::string mod_name = split((*this).name(),'.',-2);
		std::string domain_state = "ON";
		
		for(int i=0; i < ::UPFinfo.size(); i++ ){
			
 			elements=split_element(::UPFinfo[i].DomainElements,',');
			
			for(int j = 0; j < elements.size(); j++){
				
				if(mod_name == elements[j]){
				
				
				domain_supply_pads = split_element(::UPFinfo[i].involved_Supply_pads,',');
						for(int k=0; k < domain_supply_pads.size(); k++){
			
			
			
							for(int l=0; l < ::Supply_pads.size(); l++){
				
				
				
								if((domain_supply_pads[k] == ::Supply_pads[l].Name)&&(::Supply_pads[l].Status == "OFF")){
					
								domain_state = "OFF";
								
					
								}
				
							}
			
			
						}
												
				}
			}
							
		}
		
		isolation_status = split(::UPFinfo[domain_index].Isolation_signal_status,',',iso_element_group); 
		
		if((domain_state == "OFF")&&(::UPFenable == 1)){
			
			if(Isolated_out == 'N'){
			
			out = SC_LOGIC_X;
			} else {
				
				if((clamp_value == "1")&&(isolation_status == "ON")){
				
					out = SC_LOGIC_1;
				
				} else {
					
					if((clamp_value == "0")&&(isolation_status == "ON")){
					
						out = SC_LOGIC_0;
					
					} else{
						
						out = SC_LOGIC_X;
						
					}
					
				}
				
			
			}
			
		}else{
			
				if((::UPFenable == 1)&&(isolation_status == "ON") && Isolated_out == 'Y'){
				
				if(clamp_value == "1"){
					
					out = SC_LOGIC_1;
					
				}else{
					
					out = SC_LOGIC_0;
					
				}
				
			}else{
			out.write(value);
		}
			
					
		}
		
	}
	
		void writeUPFevent(){
	
		std::vector<string> elements;
		std::string mod_name = split((*this).name(),'.',-2);
		std::string domain_state = "";
		
		for(int i=0; i < ::UPFinfo.size(); i++ ){
			
 			elements=split_element(::UPFinfo[i].DomainElements,',');
			
			for(int j = 0; j < elements.size(); j++){
				
				if(mod_name == elements[j]){
				
				
				domain_supply_pads = split_element(::UPFinfo[i].involved_Supply_pads,',');
						for(int k=0; k < domain_supply_pads.size(); k++){
			
			
			
							for(int l=0; l < ::Supply_pads.size(); l++){
				
				
				
								if((domain_supply_pads[k] == ::Supply_pads[l].Name)&&(::Supply_pads[l].Status == "OFF")){
					
								domain_state = "OFF";
								
					
								}
				
							}
			
			
						}
												
				}
			}
							
		}
		
		isolation_status = split(::UPFinfo[domain_index].Isolation_signal_status,',',iso_element_group);
		
		if((domain_state == "OFF")&&(::UPFenable == 1)){
			
			if(Isolated_out == 'N'){
		
			out = SC_LOGIC_X;
			}else{
				
				if((clamp_value == "1")&&(isolation_status == "ON")){
				
					out = SC_LOGIC_1;
				
				} else {
					
					if((clamp_value == "0")&&(isolation_status == "ON")){
					
						out = SC_LOGIC_0;
					
					} else{
						
						out = SC_LOGIC_X;
						
					}
					
				}
								
			
			}
		} else{
			
			if((::UPFenable == 1)&&(isolation_status == "ON") && Isolated_out == 'Y'){
				
				if(clamp_value == "1"){
					
					out = SC_LOGIC_1;
					
				}else{
					
					out = SC_LOGIC_0;
					
				}
				
			}
			
		}
		
	}
	

	
	sc_logic read(){
		return out.read();
	}
	
	void info(){
	//cout << "name: "<<(*this).name() << " kind: " << (*this).kind() << endl; 
	}
	
	const char* kind() const
        { return "sc_out_upf"; }
		
	
	
	
	      void operator = ( const sc_logic& a )
        { 
				std::vector<string> elements;
		std::string mod_name = split((*this).name(),'.',-2);
		std::string domain_state = "";
				for(int i=0; i < ::UPFinfo.size(); i++ ){
			
 			elements=split_element(::UPFinfo[i].DomainElements,',');
			
			for(int j = 0; j < elements.size(); j++){
				
				if(mod_name == elements[j]){
				
				
				
				domain_supply_pads = split_element(::UPFinfo[i].involved_Supply_pads,',');
						for(int k=0; k < domain_supply_pads.size(); k++){
			
			
			
							for(int l=0; l < ::Supply_pads.size(); l++){
				
				
				
								if((domain_supply_pads[k] == ::Supply_pads[l].Name)&&(::Supply_pads[l].Status == "OFF")){
					
								domain_state = "OFF";
								
					
								}
				
							}
			
			
						}
												
				}
			}
							
		}
		
		isolation_status = split(::UPFinfo[domain_index].Isolation_signal_status,',',iso_element_group);
		
		if((domain_state == "OFF")&&(::UPFenable == 1)){
			
			if(Isolated_out == 'N'){
			
			out = SC_LOGIC_X;
			} else {
				
				if((clamp_value == "1")&&(isolation_status == "ON")){
				
					out = SC_LOGIC_1;
				
				} else {
					
					if((clamp_value == "0")&&(isolation_status == "ON")){
					
						out = SC_LOGIC_0;
					
					} else{
						
						out = SC_LOGIC_X;
						
					}
					
				}
				
			
			}
			
		}else{
			
				if((::UPFenable == 1)&&(isolation_status == "ON") && Isolated_out == 'Y'){
				
				if(clamp_value == "1"){
					
					out = SC_LOGIC_1;
					
				}else{
					
					out = SC_LOGIC_0;
					
				}
				
			}else{
			out = a; 
		}
			
					
		}
			
		
		}


};


template<int U>

class sc_out_upf < sc_dt::sc_lv<U>>: public sc_channel
{
	
	private: 
	
	char Isolated_out = 'N'; 
	string module_name = ""; 
	string port_name = ""; 
	string clamp_value = "";
	vector<string> domain_supply_pads;
	
	std::vector<string> ISO_elements;
	std::vector<string> dom_elements;
	int domain_index; 
	int iso_element_group;
	string isolation_sense = ""; 
	std::vector<string> isolation_signal_sts;
	std::vector<string> iso_elements_vector;
	std::string isolation_status = "";
	
	
	std::string split(std::string s, char delimiter, int position){

		std::vector<std::string> split_string;
		std::string aux1; 
		std::istringstream tokenStream(s);
		while (getline(tokenStream, aux1, delimiter))
		{
			split_string.push_back(aux1);
		}	
		
		if(position == -1){
			
			position = split_string.size() - 1; 
		}else{
			
			if(position == -2){
				
				position = split_string.size() - 2; 
				
			}
		}
		
		
		return split_string[position]; 
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
		
	public:
	
	sc_out<sc_lv<U>> out; 
	
	
	SC_HAS_PROCESS(sc_out_upf);
	sc_out_upf(const sc_module_name& nmod ) : sc_channel(nmod){
	info();	
	SC_METHOD(writeUPFevent);
	sensitive << upf_event; 
	
	
	
	module_name =  split((*this).name(),'.',-2);
	port_name = split((*this).name(),'.',-1);
	
			for(int i=0; i < ::UPFinfo.size(); i++ ){
			
 			dom_elements=split_element(::UPFinfo[i].DomainElements,',');
			
			for(int j = 0; j < dom_elements.size(); j++){
				
				if(module_name == dom_elements[j]){
					
					iso_elements_vector = split_element(::UPFinfo[i].IsolationElements,'|');
					
				
					
					for(int l = 0; l < iso_elements_vector.size(); l++){
				
				ISO_elements = split_element(iso_elements_vector[l],' ');
				
								
				for(int k = 0; k < ISO_elements.size(); k++){
					
	
					if(ISO_elements[k] == port_name){
						
						Isolated_out = 'Y'; 
						clamp_value = split(::UPFinfo[i].IsolationClamp,',',l);
						iso_element_group = l;
						
						domain_index = i; 
						
					}
					
				}
				
				}
												
				}
			}
							
		}
	
		
	}
	
	
		void writeUPFevent(){
	
		std::vector<string> elements;
		std::string mod_name = split((*this).name(),'.',-2);
		std::string domain_state = "";
		
		for(int i=0; i < ::UPFinfo.size(); i++ ){
			
 			elements=split_element(::UPFinfo[i].DomainElements,',');
			
			for(int j = 0; j < elements.size(); j++){
				
				if(mod_name == elements[j]){
				
				
				domain_supply_pads = split_element(::UPFinfo[i].involved_Supply_pads,',');
						for(int k=0; k < domain_supply_pads.size(); k++){
			
			
			
							for(int l=0; l < ::Supply_pads.size(); l++){
				
				
				
								if((domain_supply_pads[k] == ::Supply_pads[l].Name)&&(::Supply_pads[l].Status == "OFF")){
					
								domain_state = "OFF";
								
					
								}
				
							}
			
			
						}
												
				}
			}
							
		}
		
		isolation_status = split(::UPFinfo[domain_index].Isolation_signal_status,',',iso_element_group); 
		
		if((domain_state == "OFF")&&(::UPFenable == 1)){
			
			if(Isolated_out == 'N'){
		
			out = sc_lv<U>(sc_logic('X'));
			}else{
				
				if((clamp_value == "1")&&(isolation_status == "ON")){
				
					out = sc_lv<U>(sc_logic('1'));
				
				} else {
					
					if((clamp_value == "0")&&(isolation_status == "ON")){
					
						out = sc_lv<U>(sc_logic('0'));
					
					} else{
						
						out = sc_lv<U>(sc_logic('X'));
						
					}
					
				}
				
								
			
			}
		} else {
			
				if((::UPFenable == 1)&&(isolation_status == "ON") && Isolated_out == 'Y'){
				
				if(clamp_value == "1"){
					
					out = sc_lv<U>(sc_logic('1'));
					
				}else{
					
					out = sc_lv<U>(sc_logic('0'));
					
				}
				
			}
			
		}
		
	}
	

	
	sc_lv<U> read(){
		return out.read();
	}
	
	void info(){
	//cout << "name: "<<(*this).name() << " kind: " << (*this).kind() << endl; 
	}
	
	const char* kind() const
        { return "sc_out_upf"; }
		
	


	 void operator = ( const char * a )
        { 
				std::vector<string> elements;
		std::string mod_name = split((*this).name(),'.',-2);
		std::string domain_state = "";
				for(int i=0; i < ::UPFinfo.size(); i++ ){
			
 			elements=split_element(::UPFinfo[i].DomainElements,',');
			
			for(int j = 0; j < elements.size(); j++){
				
				if(mod_name == elements[j]){
				
				
				
				domain_supply_pads = split_element(::UPFinfo[i].involved_Supply_pads,',');
						for(int k=0; k < domain_supply_pads.size(); k++){
			
			
			
							for(int l=0; l < ::Supply_pads.size(); l++){
				
				
				
								if((domain_supply_pads[k] == ::Supply_pads[l].Name)&&(::Supply_pads[l].Status == "OFF")){
					
								domain_state = "OFF";
								
					
								}
				
							}
			
			
						}
												
				}
			}
							
		}
		
		isolation_status = split(::UPFinfo[domain_index].Isolation_signal_status,',',iso_element_group); 
		
		if((domain_state == "OFF")&&(::UPFenable == 1)){
			
			if(Isolated_out == 'N'){
			
			out = sc_lv<U>(sc_logic('X'));
			} else {
				
				if((clamp_value == "1")&&(isolation_status == "ON")){
				
					out = sc_lv<U>(sc_logic('1'));
				
				} else {
					
					if((clamp_value == "0")&&(isolation_status == "ON")){
					
						out = sc_lv<U>(sc_logic('0'));
					
					} else{
						
						out = sc_lv<U>(sc_logic('X'));
						
					}
					
				}
				
			
			}
			
		}else{
			
				if((::UPFenable == 1)&&(isolation_status == "ON") && Isolated_out == 'Y'){
				
				if(clamp_value == "1"){
					
					out = sc_lv<U>(sc_logic('1'));
					
				}else{
					
					out = sc_lv<U>(sc_logic('0'));
					
				}
				
			}else{
			out = a; 
		}
			
					
		}
			
	
		}
		
		void operator = ( int a )
        { 
				std::vector<string> elements;
		std::string mod_name = split((*this).name(),'.',-2);
		std::string domain_state = "";
				for(int i=0; i < ::UPFinfo.size(); i++ ){
			
 			elements=split_element(::UPFinfo[i].DomainElements,',');
			
			for(int j = 0; j < elements.size(); j++){
				
				if(mod_name == elements[j]){
				
				
				
				domain_supply_pads = split_element(::UPFinfo[i].involved_Supply_pads,',');
						for(int k=0; k < domain_supply_pads.size(); k++){
			
			
			
							for(int l=0; l < ::Supply_pads.size(); l++){
				
				
				
								if((domain_supply_pads[k] == ::Supply_pads[l].Name)&&(::Supply_pads[l].Status == "OFF")){
					
								domain_state = "OFF";
								
					
								}
				
							}
			
			
						}
												
				}
			}
							
		}
		
		isolation_status = split(::UPFinfo[domain_index].Isolation_signal_status,',',iso_element_group); 
		
		if((domain_state == "OFF")&&(::UPFenable == 1)){
			
			if(Isolated_out == 'N'){
			
			out = sc_lv<U>(sc_logic('X'));
			} else {
				
				if((clamp_value == "1")&&(isolation_status == "ON")){
				
					out = sc_lv<U>(sc_logic('1'));
				
				} else {
					
					if((clamp_value == "0")&&(isolation_status == "ON")){
					
						out = sc_lv<U>(sc_logic('0'));
					
					} else{
						
						out = sc_lv<U>(sc_logic('X'));
						
					}
					
				}
				
			
			}
			
		}else{
			
				if((::UPFenable == 1)&&(isolation_status == "ON") && Isolated_out == 'Y'){
				
				if(clamp_value == "1"){
					
					out = sc_lv<U>(sc_logic('1'));
					
				}else{
					
					out = sc_lv<U>(sc_logic('0'));
					
				}
				
			}else{
			out = a; 
		}
			
					
		}
			
	
		}
		int X;
		




};



