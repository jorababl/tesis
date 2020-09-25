# tesis

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Author Information  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

Author: Engineer Jorge Abarca Blanco
email: jorge.abarca.blanco@gmail.com
year: 2020
Open Source Licence: MIT


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Introduction %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

Currently the HDL SystemC (v2.3) does not support the UPF power intent, which represents a constraint when it is required to perform a functional verification including the
power intent behavior at RTL level for low power designs. According to a previous research, contributions related to this topic have been few,
and they are limited to academic or industrial private solutions, therefore they are not available to the scientific community.
This thesis proposes a partial support of the UPF language over SystemC at RTL level, by means of an open source library, which makes use of techniques based on inheritance,
whereby a solution is achieved that complements SystemC without modifying its original code. 
It parses the power intent file in UPF language v1.0, and gets the required information to enable the use of concepts such as: power domains, power gating, isolation cells,
etc. In addition, details of the implemented code are shown, in order to allow a continues improvement process, besides that, detailed examples of use and their respective analysis
of results are included, and in this way the reader could easily add the solution in their projects if it is desired.



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% estructure of project  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


the project has 3 files, which are used to complement SystemC with UPF behavior: 

		global.h ---> contains global structures used by the project. 
		UPFparser.h ---> function in charge of reading and parsing the upf file. it extracts all the relevant information. 
		port_upf.h  ---> contains classes to provide new input / output ports with UPF behavior (sc_in_upf  / sc_out_upf ). 
		
		
		
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% supported UPF concepts %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

power domains
supply ports
supply nets 
supply pads
switches
isolation cells
plain / hierarchical UPF files
