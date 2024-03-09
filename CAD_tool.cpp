#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <map>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <limits>
#include <iomanip>
using namespace std;

class Netlist_read {
    friend class load_cal;
    friend class delay_power_cal;

    private:
        struct Gate;
        struct Wire;
        struct Gate{
            string name; 
            Wire *ZN = NULL; 
            Wire *A1 = NULL; 
            Wire *A2 = NULL; 
            string gate_type = ""; 
            double load = 0;  
            double output_trans = -1;   
            double delay = -1;
            double total_delay = 0;
            double switchpower = 0;
            double internalpower = 0;
            int toggle_step = 0;

        };


        struct Wire{
            int value = 0; //boundary value      
            string name;        
            Gate *pre = NULL;    
            bool OUT = false;
            vector<Gate *> next;
        };

        unordered_map<string, Wire*> graph; // 存取wire struct
        vector<Wire*> input_wire;
        
    
        vector<double> index1;
        vector<double> index2;
        double NORA1;
        double NORA2;
        double INVXI;
        double NANDA1;
        double NANDA2;    

    public:
        Netlist_read() : NORA1(0), NORA2(0), INVXI(0), NANDA1(0), NANDA2(0) 
        {
        }
        vector<Gate*> totalgate;
        vector<vector<int>>logVal;
        vector<string> wires ;

        unordered_map<string, vector<vector<double>> > dataMap_timing;
        unordered_map<string, vector<vector<double>> > dataMap_power;

        void save_input(string line){
            int pos = 0;
            string substring = "";
            
            pos = line.find("input");
                
            substring = line.substr(pos + 6);
            vector<string> wires ;
            istringstream stream(substring);
            string token;

            while (getline(stream, token, ',')) {
                    
                    // if(token[0]==' '){
                    //     token= token.substr(1);
                    // }
                wires.push_back(token);
            }

            for (int i = 0; i < wires.size(); i++){
                    
                wires[i].erase(remove_if(wires[i].begin(), wires[i].end(), ::isspace), wires[i].end());
                Wire *w = new Wire();
                w->name = wires[i];
                w->pre = NULL;
                graph[wires[i]] = w;
                input_wire.push_back(w);
            }
        }
        void save_output(string line){
            int pos = 0;
            string substring = "";
            pos = line.find("output");
            substring = line.substr(pos + 7);
            vector<string> wires ;
            istringstream stream(substring);
            string token;

            while (getline(stream, token, ',')) {
                    // if(token[0]==' '){
                    //     token= token.substr(1);
                    // }
                wires.push_back(token);
                    
            }
            for (int i = 0; i < wires.size(); i++){
                wires[i].erase(remove_if(wires[i].begin(), wires[i].end(), ::isspace), wires[i].end());
                Wire *w = new Wire();
                w->name = wires[i];
                w->OUT = true;
                graph[wires[i]] = w;
            }
        }
        void save_wire(string line){
            int pos = 0;
            string substring = "";
            pos = line.find("wire");
            substring = line.substr(pos + 5);
            vector<string> wires ;
            istringstream stream(substring);
            string token;

            while (getline(stream, token, ',')) {
                wires.push_back(token);
            }
            for (int i = 0; i < wires.size(); i++){
                wires[i].erase(remove_if(wires[i].begin(), wires[i].end(), ::isspace), wires[i].end());
                Wire *w = new Wire();
                w->name = wires[i];
                graph[wires[i]] = w;
            }
        
        }
        void save_inv(string line){
            Gate *n = new Gate();
            Wire *w;
            int pos = line.find("INVX1");
            string substring = line.substr(pos + 5);
            pos = substring.find("(");
            substring = substring.substr(0, pos);
            substring.erase(remove_if(substring.begin(), substring.end(), ::isspace), substring.end());
            n->name = substring;
            n->gate_type = "INV";

            pos = line.find(".ZN(");
            substring = line.substr(pos + 4);
            pos = substring.find(")");
            substring = substring.substr(0, pos);
            w = graph[substring];
            n->ZN = w; 
            w->pre = n;

            pos = line.find(".I(");
            substring = line.substr(pos + 3);
            pos = substring.find(")");
            substring = substring.substr(0, pos);
            w = graph[substring];
            n->A1 = w; 
            w->next.push_back(n); 
            totalgate.push_back(n);    
        }
        void save_nand(string line){
            Gate *n = new Gate();
            Wire *w;
            int pos = line.find("NANDX1");
            string substring = line.substr(pos + 6);
            pos = substring.find("(");
            substring = substring.substr(0, pos);
            substring.erase(remove_if(substring.begin(), substring.end(), ::isspace), substring.end());
            n->name = substring;
            n->gate_type = "NAND";
            pos = line.find(".ZN(");
            substring = line.substr(pos + 4);
            pos = substring.find(")");
            substring = substring.substr(0, pos);
            w = graph[substring];
            n->ZN = w;
            w->pre = n;
                
                
            pos = line.find(".A1(");
            substring = line.substr(pos + 4);
            pos = substring.find(")");
            substring = substring.substr(0, pos);
            w = graph[substring];
            n->A1 = w;
            w->next.push_back(n);

            pos = line.find(".A2(");
            substring = line.substr(pos + 4);
            pos = substring.find(")");
            substring = substring.substr(0, pos);
            w = graph[substring];
            n->A2 = w;
            w->next.push_back(n);
            totalgate.push_back(n);
        }
        void save_nor(string line){
            Gate *n = new Gate();
            Wire *w;
            int pos = line.find("NOR2X1");
            string substring = line.substr(pos + 6);
            pos = substring.find("(");
            substring = substring.substr(0, pos);
            substring.erase(remove_if(substring.begin(), substring.end(), ::isspace), substring.end());
            n->name = substring;
            n->gate_type = "NOR";

                
            pos = line.find(".ZN(");
            substring = line.substr(pos + 4);
            pos = substring.find(")");
            substring = substring.substr(0, pos);
            w = graph[substring];
            n->ZN = w;
            w->pre = n;
            
                
                
            pos = line.find(".A1(");
            substring = line.substr(pos + 4);
            pos = substring.find(")");
            substring = substring.substr(0, pos);
            w = graph[substring];
            n->A1 = w;
            w->next.push_back(n);


            pos = line.find(".A2(");
            substring = line.substr(pos + 4);
            pos = substring.find(")");
            substring = substring.substr(0, pos);
            w = graph[substring];
            n->A2 = w;
            w->next.push_back(n);    
            totalgate.push_back(n);
        }

        void store_gate(string line){
            if (line.find("INVX1") != string::npos){
                save_inv(line);
            }
            
            else if (line.find("NANDX1") != string::npos){
                save_nand(line);
            }
            
            else if (line.find("NOR2X1") != string::npos){
                save_nor(line);
            }
        }

        void define_gate_wire(ifstream &input){
            int pos = 0;
            string line;
            while (getline(input, line)){
                
                line.erase(remove(line.begin(), line.end(), ';'), line.end());

                if (line == "endmodule"){
                    break;
                }
                if (line.empty()){
                    continue;
                }
                if (line.find("input") != string::npos ){
                    save_input(line);
                }
                else if (line.find("output") != string::npos){
                    save_output(line);
                }
                else if(line.find("wire") != string::npos){
                    save_wire(line);
                }

                if (line.find("INVX1") != string::npos || line.find("NOR2X1") != string::npos || line.find("NANDX1") != string::npos){
                    store_gate(line);
                }
            }
        }
        vector<double> readformat(vector<double> temp_vector,string line){
            size_t start = line.find('"');
            size_t end = line.find('"', start + 1);
            string values = line.substr(start + 1, end - start - 1);
                            
            istringstream iss(values);
            string value;
            while (getline(iss, value, ',')) {
                temp_vector.push_back(stod(value));
                }
            // for(const auto& value : temp_vector){
            //     cout<<value<<" ";
            // }
            // cout<<endl;
            return temp_vector;
        }

        void loadlib(ifstream &lib){
        
        // check whether file is reaching timing() ,power() or not
            bool isReadingTiming = false;
            bool isReadingPower = false;
            bool isReadingNOR = false;
            bool isReadingNAND = false;
            bool isReadingINV = false;
            bool check = false;


            string line;
            while (getline(lib, line)) {
                line.erase(line.find_last_not_of(" \t") + 1);
                line.erase(0, line.find_first_not_of(" \t"));
                if(line.find("index_1")!= string::npos){
                    int pos = line.find("index_1 (\"");
                    
                    
                    string substring = line.substr(pos + 10);
                    substring.erase(substring.length() - 3);
                    
                    istringstream stream(substring);
                    string token;
                    while (getline(stream, token, ',')) {
                        index1.push_back(stod(token));
                    }

                }
                else if(line.find("index_2")!= string::npos){
                    int pos = line.find("index_2 (\"");
                    string substring = line.substr(pos + 10);
                    substring.erase(substring.length() - 3);
                    istringstream stream(substring);
                    string token;
                    while (getline(stream, token, ',')) {
                        index2.push_back(stod(token));
                    }

                }
                
                if(line.find("cell (NOR2X1) {")!= string::npos){
                    isReadingNOR = true;
                }
                else if(line.find("cell (INVX1) {")!= string::npos){
                    isReadingINV = true;
                }
                else if(line.find("cell (NANDX1) {")!= string::npos){
                    isReadingNAND = true;
                }
                
                if(isReadingNOR){
                    
                    if(line.find("capacitance : ")!= string::npos && (!check)&& NORA1 ==0){
                        int pos = line.find("capacitance : ");
                        int endpos = line.find(";");
                        string substring = line.substr(pos + 15, endpos-1);
                        NORA1 = stod(substring);
                        check = true;
                        
                    }
                    else if(line.find("capacitance : ")!= string::npos && check&& NORA2 ==0){
                        int pos = line.find("capacitance : ");
                        int endpos = line.find(";");
                        string substring = line.substr(pos + 15, endpos-1);
                        NORA2 = stod(substring);
                        //cout<<NORA2<<endl;
                        check = false;
                        
                    }
                    else if(line.find("rise_power(table10){")!= string::npos){
                        isReadingPower = true;
                    }
                    else if(line.find("cell_rise(table10){")!= string::npos){
                        isReadingTiming = true;
                    }
                    
                    if(isReadingPower){
                        if (line.find("rise_power") != string::npos) {
                            for(int i = 0; i <7;i++){
                                vector<double> temp_vector;
                                getline(lib, line);
                                temp_vector = readformat(temp_vector,line);
                                dataMap_power["NOR_rise_p"].push_back(temp_vector);
                            }
                        }
                        else if (line.find("fall_power") != string::npos) {
                    
                            for(int i = 0; i <7;i++){
                                vector<double> temp_vector;
                                getline(lib, line);
                                temp_vector = readformat(temp_vector,line);
                                dataMap_power["NOR_fall_p"].push_back(temp_vector);
                            }
                        }
                    else if(isReadingTiming){
                        if (line.find("cell_rise") != std::string::npos) {
                            
                                for(int i = 0; i < 7;i++){
                                    vector<double> temp_vector;
                                    getline(lib, line);
                                    temp_vector = readformat(temp_vector,line);
                                    dataMap_timing["NOR_cell_rise"].push_back(temp_vector);
                                }
                                
                            }
                        else if (line.find("cell_fall") != string::npos) {
                            for(int i = 0; i < 7;i++){
                                    vector<double> temp_vector;
                                    getline(lib, line);
                                    temp_vector = readformat(temp_vector,line);
                                    dataMap_timing["NOR_cell_fall"].push_back(temp_vector);
                                }
                            
                        }
                        else if (line.find("rise_transition") != string::npos) {
                    
                            for(int i = 0; i < 7;i++){
                                    vector<double> temp_vector;
                                    getline(lib, line);
                                    temp_vector = readformat(temp_vector,line);
                                    dataMap_timing["NOR_rise_tr"].push_back(temp_vector);
                                }
                            
                        }
                        else if (line.find("fall_transition") != string::npos) {
                    
                            for(int i = 0; i < 7;i++){
                                    vector<double> temp_vector;
                                    getline(lib, line);
                                    temp_vector = readformat(temp_vector,line);
                                    dataMap_timing["NOR_fall_tr"].push_back(temp_vector);
                                }
                            isReadingNOR = false;
                            isReadingPower = false;
                            isReadingTiming = false;
                            
                        }


                    }
                    }
                
                }
                else if(isReadingNAND){
                    
                    if(line.find("capacitance : ")!= string::npos && !check && NANDA1 ==0){
                        int pos = line.find("capacitance : ");
                        int endpos = line.find(";");
                        string substring = line.substr(pos + 15, endpos-1);
                        NANDA1 = stod(substring);
                        //cout<<NANDA1<<endl;
                        check = true;
                    }
                    else if(line.find("capacitance : ")!= string::npos && (check)&& NANDA2 ==0){
                        int pos = line.find("capacitance : ");
                        int endpos = line.find(";");
                        string substring = line.substr(pos + 15, endpos-1);
                        NANDA2 = stod(substring);
                        //cout<<NANDA2<<endl;
                        check = false;
                    }
                    else if(line.find("rise_power(table10){")!= string::npos){
                        isReadingPower = true;
                    }
                    else if(line.find("cell_rise(table10){")!= string::npos){
                        isReadingTiming = true;
                        
                    }

                    if(isReadingPower){
                        if (line.find("rise_power") != string::npos) {
                            for(int i = 0; i <7;i++){
                                vector<double> temp_vector;
                                getline(lib, line);
                                temp_vector = readformat(temp_vector,line);
                                dataMap_power["NAND_rise_p"].push_back(temp_vector);
                            }
                        }
                        else if (line.find("fall_power") != string::npos) {
                    
                            for(int i = 0; i <7;i++){
                                vector<double> temp_vector;
                                getline(lib, line);
                                temp_vector = readformat(temp_vector,line);
                                dataMap_power["NAND_fall_p"].push_back(temp_vector);
                            }
                            
                        }
                        else if(isReadingTiming){
                        if (line.find("cell_rise") != std::string::npos) {
                            
                                for(int i = 0; i < 7;i++){
                                    vector<double> temp_vector;
                                    getline(lib, line);
                                    temp_vector = readformat(temp_vector,line);
                                    dataMap_timing["NAND_cell_rise"].push_back(temp_vector);
                                }
                                
                            }
                        else if (line.find("cell_fall") != string::npos) {
                            for(int i = 0; i < 7;i++){
                                    vector<double> temp_vector;
                                    getline(lib, line);
                                    temp_vector = readformat(temp_vector,line);
                                    dataMap_timing["NAND_cell_fall"].push_back(temp_vector);
                                }
                            
                        }
                        else if (line.find("rise_transition") != string::npos) {
                    
                            for(int i = 0; i < 7;i++){
                                    vector<double> temp_vector;
                                    getline(lib, line);
                                    temp_vector = readformat(temp_vector,line);
                                    dataMap_timing["NAND_rise_tr"].push_back(temp_vector);
                                }
                            
                        }
                        else if (line.find("fall_transition") != string::npos) {
                    
                            for(int i = 0; i < 7;i++){
                                    vector<double> temp_vector;
                                    getline(lib, line);
                                    temp_vector = readformat(temp_vector,line);
                                    dataMap_timing["NAND_fall_tr"].push_back(temp_vector);
                                }
                            isReadingNAND = false;
                            isReadingPower = false;
                            isReadingTiming = false;
                            
                        }


                    }
                    }

                }
                else if(isReadingINV){
                    if(line.find("capacitance : ")!= string::npos&& INVXI ==0){
                        int pos = line.find("capacitance : ");
                        int endpos = line.find(";");
                        string substring = line.substr(pos + 15, endpos-1);
                        INVXI = stod(substring);
                        //cout<<INVXI<<endl;
                        
                    }
                    else if(line.find("rise_power(table10){")!= string::npos){
                        isReadingPower = true;
                        
                    }
                    else if(line.find("cell_rise(table10){")!= string::npos){
                        isReadingTiming = true;
                        
                        
                    }
                    if(isReadingPower){
                        if (line.find("rise_power") != string::npos) {
                            for(int i = 0; i <7;i++){
                                vector<double> temp_vector;
                                getline(lib, line);
                                temp_vector = readformat(temp_vector,line);
                                dataMap_power["inv_rise_p"].push_back(temp_vector);
                            }
                        }
                        else if (line.find("fall_power") != string::npos) {
                    
                            for(int i = 0; i <7;i++){
                                vector<double> temp_vector;
                                getline(lib, line);
                                temp_vector = readformat(temp_vector,line);
                                dataMap_power["inv_fall_p"].push_back(temp_vector);
                            }
                            
                            
                        }
                        else if(isReadingTiming){
                        if (line.find("cell_rise") != std::string::npos) {
                            //cout<<"dads"<<endl;
                                for(int i = 0; i < 7;i++){
                                    
                                    vector<double> temp_vector;
                                    getline(lib, line);
                                    temp_vector = readformat(temp_vector,line);
                                    dataMap_timing["inv_cell_rise"].push_back(temp_vector);
                                }
                                
                            }
                        else if (line.find("cell_fall") != string::npos) {
                            for(int i = 0; i < 7;i++){
                                    vector<double> temp_vector;
                                    getline(lib, line);
                                    temp_vector = readformat(temp_vector,line);
                                    dataMap_timing["inv_cell_fall"].push_back(temp_vector);
                                }
                            
                        }
                        else if (line.find("rise_transition") != string::npos) {
                    
                            for(int i = 0; i < 7;i++){
                                    vector<double> temp_vector;
                                    getline(lib, line);
                                    temp_vector = readformat(temp_vector,line);
                                    dataMap_timing["inv_rise_tr"].push_back(temp_vector);
                                }
                            
                        }
                        else if (line.find("fall_transition") != string::npos) {
                    
                            for(int i = 0; i < 7;i++){
                                    vector<double> temp_vector;
                                    getline(lib, line);
                                    temp_vector = readformat(temp_vector,line);
                                    dataMap_timing["inv_fall_tr"].push_back(temp_vector);
                                }
                            isReadingINV = false;
                            isReadingPower = false;
                            
                        }


                    }
                    }

                

                    
                }

            }

        }
        static bool compareNAME(Gate *a, Gate *b){
            string aa = a->name.substr(1);
            string bb = b->name.substr(1);
            return  stoi(aa) < stoi(bb);
        }


        void pat_input(string line ){ //將pattern放入input中
        
            int pos = line.find("input");
            line.erase(remove(line.begin(), line.end(), ';'), line.end());
            string substring = line.substr(pos + 6);
            
            istringstream stream(substring);
            string token;

            while (getline(stream, token, ',')) {
                
                wires.push_back(token);
                
            }

        }


        void save_p(int j){
            // cout<<j<<endl;
            for (int i = 0; i < wires.size(); i++){
                wires[i].erase(remove_if(wires[i].begin(), wires[i].end(), ::isspace), wires[i].end());
                // cout<<wires[i]<<endl;
                Wire *w = new Wire();
                w = graph[wires[i]];
                w->value = logVal[j][i];
                
                
            }

        }



        void insert_pattern(ifstream &input){
            int pos = 0;
        
            string line;
            while (getline(input, line)){
            
                line.erase(remove(line.begin(), line.end(), ';'), line.end());

                if (line == ".end"){
                    break;
                }
                else if (line.find("input") != string::npos){
                    pat_input(line);
                }
        
                else if (line.find(".end") == string::npos && line.find("input") == string::npos){
                    // if(line.find("    ")!=string ::npos){
                    //     line = line.substr(4);
                    // }
                    // // else if(line.find("  ")!=string ::npos){
                    // //     line = line.substr(2);
                    // //     cout<<"c"<<endl;
                    // // }
                    // else if(line.find("   ")!=string ::npos){
                    //     //cout<<"a"<<endl;
                    //     line = line.substr(3);
                    // }
                    
                    stringstream ss(line);
                    string token;
                    vector<int> currentData;
                    
                    while (getline(ss, token, ' ')) {
                        //cout<<token<<endl;
                        // if(token == ' '){
                        //     cout<<"fuck"<<endl;
                        // }
                        
                        
                        while(token.size() > 1){
                            token = token.substr(1);
                        }
                        
                        if(token[0] !='0' && token[0] !='1'){
                            continue;
                        }
                        
                        
                        //cout<<token<<endl;
                        currentData.push_back(stoi(token));
                    }
                    logVal.push_back(currentData);
                }
                
            }
            
        }

    };

class load_cal{
    public:
    
    void out_load(Netlist_read& cm){

    for (const auto &g : cm.totalgate)
    {
        if (g->ZN->OUT)
        {
            g->load = 0.03;
        }

        for (const auto &n : g->ZN->next) 
        {
            if (n->gate_type == "INV")
            {
                g->load += cm.INVXI;
            }
            else if (n->gate_type == "NOR")
            {
                if (n->A1 == g->ZN)
                {
                    g->load += cm.NORA1;
                }
                else if (n->A2 == g->ZN)
                {
                    g->load += cm.NORA2;
                }
                else
                {
                    cerr << "ERROR" << endl;
                }
            }
            else if (n->gate_type == "NAND")
            {
                if (n->A1 == g->ZN)
                {
                    g->load +=cm.NANDA1;
                }
                else if (n->A2 == g->ZN)
                {
                    g->load += cm.NANDA2;
                }
                else
                {
                    cerr << "ERROR" << endl;
                }
            }
            else
            {
                cerr << "ERROR" << endl;
            }
        }
    }

    sort(cm.totalgate.begin(), cm.totalgate.end(), Netlist_read::compareNAME);
    }
};

class delay_power_cal{
    public:
        delay_power_cal(){
            toggle = 0;
            total_power = 0;
        }
        ~delay_power_cal(){

        }
        double toggle;
        double total_power;
        double Interpolation(Netlist_read& cm, vector<vector<double>> & grid, double x, double y) {
        
            bool inter_or_extra = false;
            bool x_pos = false;
            bool y_pos = false;
            double interpolatedValue = 0.0;
            double extrapolatedValue = 0.0;
            if (x < cm.index1[0] || x > cm.index1[6] || y < cm.index2[0] || y > cm.index2[6]) {
                inter_or_extra = false;
            }
            else{
                inter_or_extra = true;
            }
            

            int x0 = -1, x1 = -1, y0 = -1, y1 = -1;
            for (int i=0 ;  i < 6; i++) {
                    if (x >= cm.index1[i] && x <= cm.index1[i+1]) {
                        x0 = i;
                        x1 = i+1;
                        x_pos = true;
                        break;
                    }
                }
                // find y pos
            for (int j = 0; j < 6; j++) {
                if (y >= cm.index2[j] && y <= cm.index2[j+1]) {
                    y0 = j;
                    y1 = j+1;
                    y_pos = true;
                    break;
                }
            }
            if(!x_pos){
                if (x < cm.index1[0] ) {
                    x0 = 0;
                    x1 = 1;
                }
                else if (x > cm.index1[6] ) {
                    x0 = 6;
                    x1 = 5;
                }
            }
            if(!y_pos){
                if (y < cm.index2[0] ) {
                    y0 = 0;
                    y1 = 1;
                }
                else if (y > cm.index2[6] ) {
                    y0 = 6;
                    y1 = 5;
                }

            }

            if(inter_or_extra){
                double p0 = grid[y0][x0];
                double p1 = grid[y1][x1];
                double p2 = grid[y1][x0];
                double p3 = grid[y0][x1];
                double A = p0 + (p2-p0)*(y-cm.index2[y0])/(cm.index2[y1]-cm.index2[y0]);
                double B = p3 + (p1-p3)*(y-cm.index2[y0])/(cm.index2[y1]-cm.index2[y0]);
                interpolatedValue = A +(B-A)*(x-cm.index1[x0])/(cm.index1[x1]-cm.index1[x0]);  
                }

            if(!inter_or_extra){
                double A2 = grid[y0][x0]; 
                double B2 = grid[y0][x1];
                double P2 = grid[y1][x0];
                double P1 = grid[y1][x1];
                double P0 = ((cm.index2[y1]-y)*A2-(cm.index2[y0]-y)*P2)/(cm.index2[y1]-cm.index2[y0]);
                double P3 = ((cm.index2[y1]-y)*B2-(cm.index2[y0]-y)*P1)/(cm.index2[y1]-cm.index2[y0]);
                //double A = p0 + (p2-p0)*(y-index2[y0])/(index2[y1]-index2[y0]);
                //double B = p3 + (p1-p3)*(y-index2[y0])/(index2[y1]-index2[y0]);
                extrapolatedValue = P0 +(P3-P0)*(x-cm.index1[x0])/(cm.index1[x1]-cm.index1[x0]);  
            }
            double ans=0.0;
            if(inter_or_extra){
                ans= interpolatedValue;
            }
            if(!inter_or_extra){
                ans= extrapolatedValue;
                if(ans < 0.0){
                    ans = 0;
                }
            }   
            return ans;
        }
        void clear_info(Netlist_read& cm){
            for (auto gatePtr : cm.totalgate) {
                    gatePtr->delay = -1;
                    gatePtr->output_trans=-1;
                    gatePtr->total_delay=0;
                    gatePtr->internalpower = 0;
                    gatePtr->switchpower = 0;
                    total_power = 0;
                }
        }
    
    void delay(Netlist_read& cm, int num){
        // Netlist_read::Wire ;
        // Netlist_read::Gate ;
        queue<Netlist_read::Gate *> q;
        

        for(int i = 0; i < cm.input_wire.size(); i++){
            Netlist_read::Wire *w = cm.input_wire[i];
            for (const auto &g : w->next){
                
                q.push(g);
            }

        }
        
        while (!q.empty()){
            Netlist_read::Gate *current_gate = q.front();
            
            q.pop();
            
            if (current_gate->total_delay != 0)
            {
                continue;
            }
            
            // INVX1
            if (current_gate->gate_type == "INV")
            {
                double transition_time_temp;
                double rise, fall;
                
                //cout<<current_gate->name<<endl;
                
                if(current_gate->A1->pre == NULL){
                    transition_time_temp = 0;
                    current_gate->total_delay = 0;
                }
                else{
                    if(current_gate->A1->pre->delay == -1){
                        continue;
                    }
                    else{
                        transition_time_temp = current_gate->A1->pre->output_trans;
                        current_gate->total_delay = current_gate->A1->pre->total_delay;
                        
                    }
                }
                
                // if(current_gate->A1->pre->ZN->value==0){
                //     a1 = Interpolation(dataMap_timing["inv_cell_rise"], current_gate->load, transition_time_temp);
                // }
                // cout<<current_gate->A1->name<<" "<<current_gate->A1->value <<endl;
                fall = Interpolation(cm, cm.dataMap_timing["inv_cell_fall"], current_gate->load, transition_time_temp);
                rise = Interpolation(cm, cm.dataMap_timing["inv_cell_rise"], current_gate->load, transition_time_temp);

                
                if(current_gate->A1->pre == NULL && current_gate->A1->value == 1){
                current_gate->delay = fall;
                current_gate->output_trans = Interpolation(cm, cm.dataMap_timing["inv_fall_tr"], current_gate->load, transition_time_temp);
                int temp = current_gate->ZN->value;
                current_gate->ZN->value = 0;
                current_gate->internalpower = Interpolation(cm, cm.dataMap_power["inv_fall_p"], current_gate->load, transition_time_temp);
                current_gate->switchpower = 0.5*0.81*current_gate->load;
                if(num==0){
                        total_power += current_gate->internalpower;
                    }
                    else{
                        if(temp != current_gate->ZN->value){
                            if(current_gate->toggle_step < 40){
                                toggle+=1;
                                current_gate->toggle_step+=1;
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                            else{
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                        }
                        else{
                            total_power += current_gate->internalpower;
                        }
                    }
                //cout<<current_gate->output_trans<<endl;
                }
                else if(current_gate->A1->pre == NULL && current_gate->A1->value == 0){
                current_gate->delay = rise;
                current_gate->output_trans = Interpolation(cm, cm.dataMap_timing["inv_rise_tr"], current_gate->load, transition_time_temp);
                int temp = current_gate->ZN->value;
                current_gate->ZN->value = 1;
                current_gate->internalpower = Interpolation(cm, cm.dataMap_power["inv_rise_p"], current_gate->load, transition_time_temp);
                current_gate->switchpower = 0.5*0.81*current_gate->load;
                if(num==0){
                        if(current_gate->toggle_step < 40){
                                toggle+=1;
                                current_gate->toggle_step+=1;
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                            else{
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                    }
                    else{
                        if(temp != current_gate->ZN->value){
                            if(current_gate->toggle_step < 40){
                                toggle+=1;
                                current_gate->toggle_step+=1;
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                            else{
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }

                        }
                        else{
                            total_power += current_gate->internalpower;
                        }
                    }
                }   
                else if(current_gate->A1->pre != NULL){
                    if(current_gate->A1->pre->ZN->value==1){
                        current_gate->delay = fall;
                        current_gate->output_trans = Interpolation(cm, cm.dataMap_timing["inv_fall_tr"], current_gate->load, transition_time_temp);
                        int temp = current_gate->ZN->value;
                        current_gate->ZN->value = 0;
                        current_gate->internalpower = Interpolation(cm, cm.dataMap_power["inv_fall_p"], current_gate->load, transition_time_temp);
                        current_gate->switchpower = 0.5*0.81*current_gate->load;
                        if(num==0){
                            total_power += current_gate->internalpower;
                        }
                        else{
                            if(temp != current_gate->ZN->value){
                                if(current_gate->toggle_step < 40){
                                toggle+=1;
                                current_gate->toggle_step+=1;
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                                }
                                else{
                                    total_power += current_gate->internalpower;
                                    total_power +=current_gate->switchpower;
                                }

                            }
                        else{
                            total_power += current_gate->internalpower;
                        }
                        }
                    }//stoi
                    else{
                        current_gate->delay = rise;
                        current_gate->output_trans = Interpolation(cm, cm.dataMap_timing["inv_rise_tr"], current_gate->load, transition_time_temp);
                        int temp = current_gate->ZN->value;
                        current_gate->ZN->value = 1;
                        current_gate->internalpower = Interpolation(cm, cm.dataMap_power["inv_rise_p"], current_gate->load, transition_time_temp);
                        current_gate->switchpower = 0.5*0.81*current_gate->load;
                        if(num==0){
                            if(current_gate->toggle_step < 40){
                                toggle+=1;
                                current_gate->toggle_step+=1;
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                            else{
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                        }
                        else{
                            if(temp != current_gate->ZN->value){
                               if(current_gate->toggle_step < 40){
                                toggle+=1;
                                current_gate->toggle_step+=1;
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                                }
                                else{
                                    total_power += current_gate->internalpower;
                                    total_power +=current_gate->switchpower;
                                }

                            }
                            else{
                                total_power += current_gate->internalpower;
                            }
                        }
                    }
                }
                
                current_gate->total_delay = current_gate->total_delay + current_gate->delay;
                
                
                for (const auto &g : current_gate->ZN->next){
                    q.push(g);
                }
            }
            
            //NOR
            else if (current_gate->gate_type == "NOR"){
            
            double transition_time_temp;
            double transition_time_temp_con;
            double fall, rise;
            int check = 0;
            if(current_gate->A1->pre == NULL &&  current_gate->A2->pre == NULL){
                transition_time_temp = 0;
                current_gate->total_delay = 0;
                check = 1;
            }
            
            else if(current_gate->A1->pre == NULL && current_gate->A2->pre != NULL){
            
                if(current_gate->A2->pre->delay == -1){
                    continue;
                }
                else{
                check = 2;
                }
                
            }
            
            else if(current_gate->A2->pre == NULL && current_gate->A1->pre != NULL){
                if(current_gate->A1->pre->delay == -1){
                    continue;
                }
                else{

                check =3;
                }
            }
            
            else if(current_gate->A2->pre != NULL && current_gate->A1->pre != NULL){
                
                if(current_gate->A1->pre->delay == -1 || current_gate->A2->pre->delay == -1){
                    continue;
                }
                
                else if (current_gate->A1->pre->delay != -1 && current_gate->A2->pre->delay != -1){
                
                
                
                check = 4;
                }
                
            }
            
            fall = Interpolation(cm, cm.dataMap_timing["NOR_cell_fall"], current_gate->load, transition_time_temp);
            rise = Interpolation(cm, cm.dataMap_timing["NOR_cell_rise"], current_gate->load, transition_time_temp);
            
            if(check == 1 ){ //兩個都是input node
                if(current_gate->A1->value == 1 || current_gate->A2->value == 1){
                    current_gate->delay = fall;
                    current_gate->output_trans = Interpolation(cm, cm.dataMap_timing["NOR_fall_tr"], current_gate->load, 0);
                    int temp = current_gate->ZN->value;
                    current_gate->ZN->value = 0;
                    current_gate->internalpower = Interpolation(cm, cm.dataMap_power["NOR_fall_p"], current_gate->load, 0);
                    current_gate->switchpower = 0.5*0.81*current_gate->load;
                    if(num==0){
                        total_power += current_gate->internalpower;
                    }
                    else{
                        if(temp != current_gate->ZN->value){
                            if(current_gate->toggle_step < 40){
                                toggle+=1;
                                current_gate->toggle_step+=1;
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                            else{
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }

                        }
                        else{
                            total_power += current_gate->internalpower;
                        }
                    }
                }
                else if(current_gate->A1->value == 0 && current_gate->A2->value == 0){
                    current_gate->delay = rise;
                    current_gate->output_trans = Interpolation(cm, cm.dataMap_timing["NOR_rise_tr"], current_gate->load, 0);
                    int temp = current_gate->ZN->value;
                    current_gate->ZN->value = 1;
                    current_gate->internalpower = Interpolation(cm, cm.dataMap_power["NOR_rise_p"], current_gate->load, 0);
                    current_gate->switchpower = 0.5*0.81*current_gate->load;
                    if(num==0){
                        if(current_gate->toggle_step < 40){
                                toggle+=1;
                                current_gate->toggle_step+=1;
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                            else{
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                    }
                    else{
                        if(temp != current_gate->ZN->value){
                            if(current_gate->toggle_step < 40){
                                toggle+=1;
                                current_gate->toggle_step+=1;
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                            else{
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }

                        }
                        else{
                            total_power += current_gate->internalpower;
                        }
                    }
                }
                
            }
            else if(check == 2){//A1 input node
                if(current_gate->A1->value == 1){//A1 是 CONTROL
                    
                    current_gate->total_delay = 0;
                    
                    

                    current_gate->delay = Interpolation(cm, cm.dataMap_timing["NOR_cell_fall"], current_gate->load, 0);
                    current_gate->output_trans = Interpolation(cm, cm.dataMap_timing["NOR_fall_tr"], current_gate->load, 0);
                    int temp = current_gate->ZN->value;
                    current_gate->ZN->value = 0;
                    current_gate->internalpower = Interpolation(cm, cm.dataMap_power["NOR_fall_p"], current_gate->load, 0);
                    current_gate->switchpower = 0.5*0.81*current_gate->load;
                    if(num==0){
                        total_power += current_gate->internalpower;
                    }
                    else{
                        if(temp != current_gate->ZN->value){
                            if(current_gate->toggle_step < 40){
                                toggle+=1;
                                current_gate->toggle_step+=1;
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                            else{
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                        }
                        else{
                            total_power += current_gate->internalpower;
                        }
                    }
                }
                else if(current_gate->A1->value == 0 && current_gate->A2->pre->ZN->value == 1){ //A1非CONTROL
                    transition_time_temp = current_gate->A2->pre->output_trans;
                    current_gate->total_delay = current_gate->A2->pre->total_delay;
                    

                    current_gate->delay = Interpolation(cm, cm.dataMap_timing["NOR_cell_fall"], current_gate->load, transition_time_temp);
                    current_gate->output_trans = Interpolation(cm, cm.dataMap_timing["NOR_fall_tr"], current_gate->load, transition_time_temp);
                    int temp = current_gate->ZN->value;
                    current_gate->ZN->value = 0;
                    current_gate->internalpower = Interpolation(cm, cm.dataMap_power["NOR_fall_p"], current_gate->load, transition_time_temp);
                    current_gate->switchpower = 0.5*0.81*current_gate->load;
                    if(num==0){
                        total_power += current_gate->internalpower;
                    }
                    else{
                        if(temp != current_gate->ZN->value){
                            if(current_gate->toggle_step < 40){
                                toggle+=1;
                                current_gate->toggle_step+=1;
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                            else{
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }

                        }
                        else{
                            total_power += current_gate->internalpower;
                        }
                    }
                }
                else if(current_gate->A1->value == 0 && current_gate->A2->pre->ZN->value == 0){
                    transition_time_temp = current_gate->A2->pre->output_trans;
                    current_gate->total_delay = current_gate->A2->pre->total_delay;
                   

                    current_gate->delay = Interpolation(cm, cm.dataMap_timing["NOR_cell_rise"], current_gate->load, transition_time_temp);
                    current_gate->output_trans = Interpolation(cm, cm.dataMap_timing["NOR_rise_tr"], current_gate->load, transition_time_temp);
                    int temp = current_gate->ZN->value;
                    current_gate->ZN->value = 1;
                    current_gate->internalpower = Interpolation(cm, cm.dataMap_power["NOR_rise_p"], current_gate->load, transition_time_temp);
                    current_gate->switchpower = 0.5*0.81*current_gate->load;
                    if(num==0){
                        if(current_gate->toggle_step < 40){
                                toggle+=1;
                                current_gate->toggle_step+=1;
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                            else{
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                    }
                    else{
                        if(temp != current_gate->ZN->value){
                            if(current_gate->toggle_step < 40){
                                toggle+=1;
                                current_gate->toggle_step+=1;
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                            else{
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }

                        }
                        else{
                            total_power += current_gate->internalpower;
                        }
                    }
                }
            }  
            else if(check == 3){//A2 input node
                if(current_gate->A2->value == 1){//A2 是 CONTROL
                    
                    current_gate->total_delay = 0;
                    

                    current_gate->delay = Interpolation(cm, cm.dataMap_timing["NOR_cell_fall"], current_gate->load, 0);;
                    current_gate->output_trans = Interpolation(cm, cm.dataMap_timing["NOR_fall_tr"], current_gate->load, 0);
                    int temp = current_gate->ZN->value;
                    current_gate->ZN->value = 0;
                    current_gate->internalpower = Interpolation(cm, cm.dataMap_power["NOR_fall_p"], current_gate->load, 0);
                    current_gate->switchpower = 0.5*0.81*current_gate->load;
                    if(num==0){
                        total_power += current_gate->internalpower;
                    }
                    else{
                        if(temp != current_gate->ZN->value){
                            if(current_gate->toggle_step < 40){
                                toggle+=1;
                                current_gate->toggle_step+=1;
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                            else{
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }

                        }
                        else{
                            total_power += current_gate->internalpower;
                        }
                    }
                }
                else if(current_gate->A2->value == 0 && current_gate->A1->pre->ZN->value == 1){ //A2非CONTROL
                    transition_time_temp = current_gate->A1->pre->output_trans;
                    current_gate->total_delay = current_gate->A1->pre->total_delay;
                    

                    current_gate->delay = Interpolation(cm, cm.dataMap_timing["NOR_cell_fall"], current_gate->load, transition_time_temp);
                    current_gate->output_trans = Interpolation(cm, cm.dataMap_timing["NOR_fall_tr"], current_gate->load, transition_time_temp);
                    int temp = current_gate->ZN->value;
                    current_gate->ZN->value = 0;
                    current_gate->internalpower = Interpolation(cm, cm.dataMap_power["NOR_fall_p"], current_gate->load, transition_time_temp);
                    current_gate->switchpower = 0.5*0.81*current_gate->load;
                    if(num==0){
                        total_power += current_gate->internalpower;
                    }
                    else{
                        if(temp != current_gate->ZN->value){
                            if(current_gate->toggle_step < 40){
                                toggle+=1;
                                current_gate->toggle_step+=1;
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                            else{
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }

                        }
                        else{
                            total_power += current_gate->internalpower;
                        }
                    }
                }
                else if(current_gate->A2->value == 0 && current_gate->A1->pre->ZN->value == 0){
                    transition_time_temp = current_gate->A1->pre->output_trans;
                    current_gate->total_delay = current_gate->A1->pre->total_delay;
                    

                    current_gate->delay = Interpolation(cm, cm.dataMap_timing["NOR_cell_rise"], current_gate->load, transition_time_temp);
                    current_gate->output_trans = Interpolation(cm, cm.dataMap_timing["NOR_rise_tr"], current_gate->load, transition_time_temp);
                    int temp = current_gate->ZN->value;
                    current_gate->ZN->value = 1;
                    current_gate->internalpower = Interpolation(cm, cm.dataMap_power["NOR_rise_p"], current_gate->load, transition_time_temp);
                    current_gate->switchpower = 0.5*0.81*current_gate->load;
                    if(num==0){
                        if(current_gate->toggle_step < 40){
                                toggle+=1;
                                current_gate->toggle_step+=1;
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                            else{
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                    }
                    else{
                        if(temp != current_gate->ZN->value){
                            if(current_gate->toggle_step < 40){
                                toggle+=1;
                                current_gate->toggle_step+=1;
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                            else{
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }

                        }
                        else{
                            total_power += current_gate->internalpower;
                        }
                    }
                }
            }
            else if(check == 4){//A1,A2 皆不是input node
                if(current_gate->A1->pre->ZN->value == 0 && current_gate->A2->pre->ZN->value == 0){ //皆非CONTROL
                    //cout<< current_gate->name<<endl;
                    if(current_gate->A1->pre->total_delay > current_gate->A2->pre->total_delay){
                    
                        transition_time_temp = current_gate->A1->pre->output_trans;
                        current_gate->total_delay = current_gate->A1->pre->total_delay;
                        
                    }
                    else{
                        transition_time_temp = current_gate->A2->pre->output_trans;
                        current_gate->total_delay = current_gate->A2->pre->total_delay;
                       
                    }
                    current_gate->delay = Interpolation(cm, cm.dataMap_timing["NOR_cell_rise"], current_gate->load, transition_time_temp);;
                    current_gate->output_trans = Interpolation(cm, cm.dataMap_timing["NOR_rise_tr"], current_gate->load, transition_time_temp);
                    int temp = current_gate->ZN->value;
                    current_gate->ZN->value = 1;
                    current_gate->internalpower = Interpolation(cm, cm.dataMap_power["NOR_rise_p"], current_gate->load, transition_time_temp);
                    current_gate->switchpower = 0.5*0.81*current_gate->load;
                    if(num==0){
                        if(current_gate->toggle_step < 40){
                                toggle+=1;
                                current_gate->toggle_step+=1;
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                            else{
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                    }
                    else{
                        if(temp != current_gate->ZN->value){
                           if(current_gate->toggle_step < 40){
                                toggle+=1;
                                current_gate->toggle_step+=1;
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                            else{
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }

                        }
                        else{
                            total_power += current_gate->internalpower;
                        }
                    }
                }
                else if(current_gate->A1->pre->ZN->value == 1 && current_gate->A2->pre->ZN->value == 0){ //A1CONTROL
                    current_gate->total_delay = current_gate->A1->pre->total_delay;
                    

                    current_gate->delay = Interpolation(cm, cm.dataMap_timing["NOR_cell_fall"], current_gate->load, current_gate->A1->pre->output_trans);
                    current_gate->output_trans = Interpolation(cm, cm.dataMap_timing["NOR_fall_tr"], current_gate->load, current_gate->A1->pre->output_trans);
                    int temp = current_gate->ZN->value;
                    current_gate->ZN->value = 0;
                    current_gate->internalpower = Interpolation(cm, cm.dataMap_power["NOR_fall_p"], current_gate->load, current_gate->A1->pre->output_trans);
                    current_gate->switchpower = 0.5*0.81*current_gate->load;
                    if(num==0){
                        total_power += current_gate->internalpower;
                    }
                    else{
                        if(temp != current_gate->ZN->value){
                            if(current_gate->toggle_step < 40){
                                toggle+=1;
                                current_gate->toggle_step+=1;
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                            else{
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                        }
                        else{
                            total_power += current_gate->internalpower;
                        }
                    }
                }
                else if(current_gate->A1->pre->ZN->value == 0 && current_gate->A2->pre->ZN->value == 1){ //A2CONTROL
                    current_gate->total_delay = current_gate->A2->pre->total_delay;
                    

                    current_gate->delay = Interpolation(cm, cm.dataMap_timing["NOR_cell_fall"], current_gate->load, current_gate->A2->pre->output_trans);;
                    current_gate->output_trans = Interpolation(cm, cm.dataMap_timing["NOR_fall_tr"], current_gate->load, current_gate->A2->pre->output_trans);
                    int temp = current_gate->ZN->value;
                    current_gate->ZN->value = 0;
                    current_gate->internalpower = Interpolation(cm, cm.dataMap_power["NOR_fall_p"], current_gate->load, current_gate->A2->pre->output_trans);
                    current_gate->switchpower = 0.5*0.81*current_gate->load;
                    if(num==0){
                        total_power += current_gate->internalpower;
                    }
                    else{
                        if(temp != current_gate->ZN->value){
                            if(current_gate->toggle_step < 40){
                                toggle+=1;
                                current_gate->toggle_step+=1;
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                            else{
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }

                        }
                        else{
                            total_power += current_gate->internalpower;
                        }
                    }
                }
                else if(current_gate->A1->pre->ZN->value == 1 && current_gate->A2->pre->ZN->value == 1){//皆為control

                    if(current_gate->A1->pre->total_delay < current_gate->A2->pre->total_delay){
                    
                        transition_time_temp_con = current_gate->A1->pre->output_trans;
                        current_gate->total_delay = current_gate->A1->pre->total_delay;
                        
                    }
                    else{
                        transition_time_temp_con = current_gate->A2->pre->output_trans;
                        current_gate->total_delay = current_gate->A2->pre->total_delay;
                        
                        
                    }
                    current_gate->delay = Interpolation(cm, cm.dataMap_timing["NOR_cell_fall"], current_gate->load, transition_time_temp_con);
                    current_gate->output_trans = Interpolation(cm, cm.dataMap_timing["NOR_fall_tr"], current_gate->load, transition_time_temp_con);
                    int temp = current_gate->ZN->value;
                    current_gate->ZN->value = 0;
                    current_gate->internalpower = Interpolation(cm, cm.dataMap_power["NOR_fall_p"], current_gate->load, transition_time_temp_con);
                    current_gate->switchpower = 0.5*0.81*current_gate->load;
                    if(num==0){
                        total_power += current_gate->internalpower;
                    }
                    else{
                        if(temp != current_gate->ZN->value){
                            if(current_gate->toggle_step < 40){
                                toggle+=1;
                                current_gate->toggle_step+=1;
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                            else{
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }

                        }
                        else{
                            total_power += current_gate->internalpower;
                        }
                    }
                }
            }
            current_gate->total_delay = current_gate->total_delay + current_gate->delay;
            
            
            for (const auto &g : current_gate->ZN->next){
                q.push(g);
            }
            
            
            }
            
            // NAND
            
            else if (current_gate->gate_type == "NAND"){
            
            double transition_time_temp;
            double transition_time_temp_con;
            double fall, rise;
            int check = 0;
            if(current_gate->A1->pre == NULL &&  current_gate->A2->pre == NULL){
                transition_time_temp = 0;
                current_gate->total_delay = 0;
                check = 1;
            }
            
            else if(current_gate->A1->pre == NULL && current_gate->A2->pre != NULL){
            
                if(current_gate->A2->pre->delay == -1){
                    continue;
                }
                else{

                check = 2;
                }
                
            }
            
            else if(current_gate->A2->pre == NULL && current_gate->A1->pre != NULL){
                if(current_gate->A1->pre->delay == -1){
                    continue;
                }
                else{
                check =3;
                }
            }
            
            else if(current_gate->A2->pre != NULL && current_gate->A1->pre != NULL){
                
                if(current_gate->A1->pre->delay == -1 || current_gate->A2->pre->delay == -1){
                    continue;
                }
                
                else if (current_gate->A1->pre->delay != -1 && current_gate->A2->pre->delay != -1){
                
                
                check = 4;
                }
                
            }
            
            fall = Interpolation(cm, cm.dataMap_timing["NAND_cell_fall"], current_gate->load, transition_time_temp);
            rise = Interpolation(cm, cm.dataMap_timing["NAND_cell_rise"], current_gate->load, transition_time_temp);
            
            if(check == 1 ){ //兩個都是input node
                if(current_gate->A1->value == 0 || current_gate->A2->value == 0){
                    current_gate->delay = Interpolation(cm, cm.dataMap_timing["NAND_cell_rise"], current_gate->load, 0);;
                    current_gate->output_trans = Interpolation(cm, cm.dataMap_timing["NAND_rise_tr"], current_gate->load, 0);
                    int temp = current_gate->ZN->value;
                    current_gate->ZN->value = 1;
                    current_gate->internalpower = Interpolation(cm, cm.dataMap_power["NAND_rise_p"], current_gate->load, 0);
                    current_gate->switchpower = 0.5*0.81*current_gate->load;
                    if(num==0){
                        if(current_gate->toggle_step < 40){
                                toggle+=1;
                                current_gate->toggle_step+=1;
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                            else{
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                    }
                    else{
                        if(temp != current_gate->ZN->value){
                            if(current_gate->toggle_step < 40){
                                toggle+=1;
                                current_gate->toggle_step+=1;
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                            else{
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }

                        }
                        else{
                            total_power += current_gate->internalpower;
                        }
                    }
                    
                }
                else if(current_gate->A1->value == 1 && current_gate->A2->value == 1){
                    current_gate->delay =  Interpolation(cm, cm.dataMap_timing["NAND_cell_fall"], current_gate->load, 0);
                    current_gate->output_trans = Interpolation(cm, cm.dataMap_timing["NAND_fall_tr"], current_gate->load, 0);
                    int temp = current_gate->ZN->value;
                    current_gate->ZN->value = 0;
                    current_gate->internalpower = Interpolation(cm, cm.dataMap_power["NAND_fall_p"], current_gate->load,0);
                    current_gate->switchpower = 0.5*0.81*current_gate->load;
                    if(num==0){
                        total_power += current_gate->internalpower;
                    }
                    else{
                        if(temp != current_gate->ZN->value){
                            if(current_gate->toggle_step < 40){
                                toggle+=1;
                                current_gate->toggle_step+=1;
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                            else{
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }

                        }
                        else{
                            total_power += current_gate->internalpower;
                        }
                    }
                }
                
            }
            else if(check == 2){//A1 input node
                if(current_gate->A1->value == 0){//A1 是 CONTROL
                    current_gate->total_delay = 0;
                    

                    current_gate->delay = Interpolation(cm, cm.dataMap_timing["NAND_cell_rise"], current_gate->load, 0);;
                    current_gate->output_trans = Interpolation(cm, cm.dataMap_timing["NAND_rise_tr"], current_gate->load, 0);
                    int temp = current_gate->ZN->value;
                    current_gate->ZN->value = 1;
                    current_gate->internalpower = Interpolation(cm, cm.dataMap_power["NAND_rise_p"], current_gate->load, 0);
                    current_gate->switchpower = 0.5*0.81*current_gate->load;
                    if(num==0){
                        if(current_gate->toggle_step < 40){
                                toggle+=1;
                                current_gate->toggle_step+=1;
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                            else{
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                    }
                    else{
                        if(temp != current_gate->ZN->value){
                            if(current_gate->toggle_step < 40){
                                toggle+=1;
                                current_gate->toggle_step+=1;
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                            else{
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }

                        }
                        else{
                            total_power += current_gate->internalpower;
                        }
                    }
                }
                else if(current_gate->A1->value == 1 && current_gate->A2->pre->ZN->value == 0){ //A1非CONTROL
                    transition_time_temp = current_gate->A2->pre->output_trans;
                    current_gate->total_delay = current_gate->A2->pre->total_delay;
                    

                    current_gate->delay = Interpolation(cm, cm.dataMap_timing["NAND_cell_rise"], current_gate->load, transition_time_temp);
                    current_gate->output_trans = Interpolation(cm, cm.dataMap_timing["NAND_rise_tr"], current_gate->load, transition_time_temp);
                    int temp = current_gate->ZN->value;
                    current_gate->ZN->value = 1;
                    current_gate->internalpower = Interpolation(cm, cm.dataMap_power["NAND_rise_p"], current_gate->load, transition_time_temp);
                    current_gate->switchpower = 0.5*0.81*current_gate->load;
                    if(num==0){
                        if(current_gate->toggle_step < 40){
                                toggle+=1;
                                current_gate->toggle_step+=1;
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                            else{
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                    }
                    else{
                        if(temp != current_gate->ZN->value){
                            if(current_gate->toggle_step < 40){
                                toggle+=1;
                                current_gate->toggle_step+=1;
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                            else{
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }

                        }
                        else{
                            total_power += current_gate->internalpower;
                        }
                    }
                }
                else if(current_gate->A1->value == 1 && current_gate->A2->pre->ZN->value == 1){
                    transition_time_temp = current_gate->A2->pre->output_trans;
                    current_gate->total_delay = current_gate->A2->pre->total_delay;
                    

                    current_gate->delay = Interpolation(cm, cm.dataMap_timing["NAND_cell_fall"], current_gate->load, transition_time_temp);;
                    current_gate->output_trans = Interpolation(cm, cm.dataMap_timing["NAND_fall_tr"], current_gate->load, transition_time_temp);
                    int temp = current_gate->ZN->value;
                    current_gate->ZN->value = 0;
                    current_gate->internalpower = Interpolation(cm, cm.dataMap_power["NAND_fall_p"], current_gate->load, transition_time_temp);
                    current_gate->switchpower = 0.5*0.81*current_gate->load;
                    if(num==0){
                        total_power += current_gate->internalpower;
                    }
                    else{
                        if(temp != current_gate->ZN->value){
                            if(current_gate->toggle_step < 40){
                                toggle+=1;
                                current_gate->toggle_step+=1;
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                            else{
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }

                        }
                        else{
                            total_power += current_gate->internalpower;
                        }
                    }
                }
            }  
            else if(check == 3){//A2 input node
                if(current_gate->A2->value == 0){//A2 是 CONTROL
                    
                    current_gate->total_delay = 0;
                    
                

                    current_gate->delay = Interpolation(cm, cm.dataMap_timing["NAND_cell_rise"], current_gate->load, 0);;
                    current_gate->output_trans = Interpolation(cm, cm.dataMap_timing["NAND_rise_tr"], current_gate->load, 0);
                    int temp = current_gate->ZN->value;
                    current_gate->ZN->value = 1;
                    current_gate->internalpower = Interpolation(cm, cm.dataMap_power["NAND_rise_p"], current_gate->load, 0);
                    current_gate->switchpower = 0.5*0.81*current_gate->load;
                    if(num==0){
                        if(current_gate->toggle_step < 40){
                                toggle+=1;
                                current_gate->toggle_step+=1;
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                            else{
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                    }
                    else{
                        if(temp != current_gate->ZN->value){
                            if(current_gate->toggle_step < 40){
                                toggle+=1;
                                current_gate->toggle_step+=1;
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                            else{
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }

                        }
                        else{
                            total_power += current_gate->internalpower;
                        }
                    }
                }
                else if(current_gate->A2->value == 1 && current_gate->A1->pre->ZN->value == 0){ //A2非CONTROL

                    transition_time_temp = current_gate->A1->pre->output_trans;
                    current_gate->total_delay = current_gate->A1->pre->total_delay;
                    

                    current_gate->delay = Interpolation(cm, cm.dataMap_timing["NAND_cell_rise"], current_gate->load, transition_time_temp);
                    current_gate->output_trans = Interpolation(cm, cm.dataMap_timing["NAND_rise_tr"], current_gate->load, transition_time_temp);
                    int temp = current_gate->ZN->value;
                    current_gate->ZN->value = 1;
                    current_gate->internalpower = Interpolation(cm, cm.dataMap_power["NAND_rise_p"], current_gate->load, transition_time_temp);
                    current_gate->switchpower = 0.5*0.81*current_gate->load;
                    if(num==0){
                        if(current_gate->toggle_step < 40){
                                toggle+=1;
                                current_gate->toggle_step+=1;
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                            else{
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                    }
                    else{
                        if(temp != current_gate->ZN->value){
                            if(current_gate->toggle_step < 40){
                                toggle+=1;
                                current_gate->toggle_step+=1;
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                            else{
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }

                        }
                        else{
                            total_power += current_gate->internalpower;
                        }
                    }
                }
                else if(current_gate->A2->value == 1 && current_gate->A1->pre->ZN->value == 1){
                    transition_time_temp = current_gate->A1->pre->output_trans;
                    current_gate->total_delay = current_gate->A1->pre->total_delay;
                    

                    current_gate->delay = Interpolation(cm, cm.dataMap_timing["NAND_cell_fall"], current_gate->load, transition_time_temp);
                    current_gate->output_trans = Interpolation(cm, cm.dataMap_timing["NAND_fall_tr"], current_gate->load, transition_time_temp);
                    int temp = current_gate->ZN->value;
                    current_gate->ZN->value = 0;
                    current_gate->internalpower = Interpolation(cm, cm.dataMap_power["NAND_fall_p"], current_gate->load, transition_time_temp);
                    current_gate->switchpower = 0.5*0.81*current_gate->load;
                    if(num==0){
                        total_power += current_gate->internalpower;
                    }
                    else{
                        if(temp != current_gate->ZN->value){
                            if(current_gate->toggle_step < 40){
                                toggle+=1;
                                current_gate->toggle_step+=1;
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                            else{
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }

                        }
                        else{
                            total_power += current_gate->internalpower;
                        }
                    }
                }
            }
            else if(check == 4){//A1,A2 皆不是input node
                if(current_gate->A1->pre->ZN->value == 1 && current_gate->A2->pre->ZN->value == 1){ //皆非CONTROL
                    if(current_gate->A1->pre->total_delay > current_gate->A2->pre->total_delay){
                
                        transition_time_temp = current_gate->A1->pre->output_trans;
                        current_gate->total_delay = current_gate->A1->pre->total_delay;
                        
                    }
                    else{
                        transition_time_temp = current_gate->A2->pre->output_trans;
                        current_gate->total_delay = current_gate->A2->pre->total_delay;
                        
                    }
                    current_gate->delay = Interpolation(cm, cm.dataMap_timing["NAND_cell_fall"], current_gate->load, transition_time_temp);;
                    current_gate->output_trans = Interpolation(cm, cm.dataMap_timing["NAND_fall_tr"], current_gate->load, transition_time_temp);
                    int temp = current_gate->ZN->value;
                    current_gate->ZN->value = 0;
                    current_gate->internalpower = Interpolation(cm, cm.dataMap_power["NAND_fall_p"], current_gate->load, transition_time_temp);
                    current_gate->switchpower = 0.5*0.81*current_gate->load;
                    if(num==0){
                        total_power += current_gate->internalpower;
                    }
                    else{
                        if(temp != current_gate->ZN->value){
                            if(current_gate->toggle_step < 40){
                                toggle+=1;
                                current_gate->toggle_step+=1;
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                            else{
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }

                        }
                        else{
                            total_power += current_gate->internalpower;
                        }
                    }
                }
                else if(current_gate->A1->pre->ZN->value == 0 && current_gate->A2->pre->ZN->value == 1){ //A1CONTROL
                    current_gate->total_delay = current_gate->A1->pre->total_delay;
                    

                    current_gate->delay = Interpolation(cm, cm.dataMap_timing["NAND_cell_rise"], current_gate->load, current_gate->A1->pre->output_trans);
                    current_gate->output_trans = Interpolation(cm, cm.dataMap_timing["NAND_rise_tr"], current_gate->load, current_gate->A1->pre->output_trans);
                    int temp = current_gate->ZN->value;
                    current_gate->ZN->value = 1;
                    current_gate->internalpower = Interpolation(cm, cm.dataMap_power["NAND_rise_p"], current_gate->load, current_gate->A1->pre->output_trans);
                    current_gate->switchpower = 0.5*0.81*current_gate->load;
                    if(num==0){
                        if(current_gate->toggle_step < 40){
                                toggle+=1;
                                current_gate->toggle_step+=1;
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                            else{
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                    }
                    else{
                        if(temp != current_gate->ZN->value){
                            if(current_gate->toggle_step < 40){
                                toggle+=1;
                                current_gate->toggle_step+=1;
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                            else{
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }

                        }
                        else{
                            total_power += current_gate->internalpower;
                        }
                    }
                }
                else if(current_gate->A1->pre->ZN->value == 1 && current_gate->A2->pre->ZN->value ==0){ //A2CONTROL
                    current_gate->total_delay = current_gate->A2->pre->total_delay;
                    

                    current_gate->delay = Interpolation(cm, cm.dataMap_timing["NAND_cell_rise"], current_gate->load, current_gate->A2->pre->output_trans);
                    current_gate->output_trans = Interpolation(cm, cm.dataMap_timing["NAND_rise_tr"], current_gate->load, current_gate->A2->pre->output_trans);
                    int temp = current_gate->ZN->value;
                    current_gate->ZN->value = 1;
                    current_gate->internalpower = Interpolation(cm, cm.dataMap_power["NAND_rise_p"], current_gate->load, current_gate->A2->pre->output_trans);
                    current_gate->switchpower = 0.5*0.81*current_gate->load;
                    if(num==0){
                        if(current_gate->toggle_step < 40){
                                toggle+=1;
                                current_gate->toggle_step+=1;
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                            else{
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                    }
                    else{
                        if(temp != current_gate->ZN->value){
                           if(current_gate->toggle_step < 40){
                                toggle+=1;
                                current_gate->toggle_step+=1;
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                            else{
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }

                        }
                        else{
                            total_power += current_gate->internalpower;
                        }
                    }
                }
                else if(current_gate->A1->pre->ZN->value == 0 && current_gate->A2->pre->ZN->value == 0){//皆是control
                    if(current_gate->A1->pre->total_delay < current_gate->A2->pre->total_delay){
                
                        transition_time_temp_con = current_gate->A1->pre->output_trans;
                        current_gate->total_delay = current_gate->A1->pre->total_delay;
                        
                    }
                    else{
                        transition_time_temp_con = current_gate->A2->pre->output_trans;
                        current_gate->total_delay = current_gate->A2->pre->total_delay;
                        
                    }
                    current_gate->delay = Interpolation(cm, cm.dataMap_timing["NAND_cell_rise"], current_gate->load, transition_time_temp_con);;
                    current_gate->output_trans = Interpolation(cm, cm.dataMap_timing["NAND_rise_tr"], current_gate->load, transition_time_temp_con);
                    int temp = current_gate->ZN->value;
                    current_gate->ZN->value = 1;
                    current_gate->internalpower = Interpolation(cm, cm.dataMap_power["NAND_rise_p"], current_gate->load,  transition_time_temp_con);
                    current_gate->switchpower = 0.5*0.81*current_gate->load;
                    if(num==0){
                        if(current_gate->toggle_step < 40){
                                toggle+=1;
                                current_gate->toggle_step+=1;
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                            else{
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                    }
                    else{
                        if(temp != current_gate->ZN->value){
                            if(current_gate->toggle_step < 40){
                                toggle+=1;
                                current_gate->toggle_step+=1;
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }
                            else{
                                total_power += current_gate->internalpower;
                                total_power +=current_gate->switchpower;
                            }

                        }
                        else{
                            total_power += current_gate->internalpower;
                        }
                    }
                }
            }
            current_gate->total_delay = current_gate->total_delay + current_gate->delay;
            
            
            for (const auto &g : current_gate->ZN->next){
                q.push(g);
            }
            
            
            }
            
        }
        sort(cm.totalgate.begin(), cm.totalgate.end(), cm.compareNAME);   
    }
};

int main(int argc, char* argv[]){
    
    Netlist_read netlist;
    load_cal load;
    delay_power_cal dpc;
    ifstream filename(argv[1]);
    ifstream pattern(argv[2]);
    ifstream lib(argv[3]);
    
    string file_name = argv[1];
    int pos = file_name.find(".v");
    netlist.define_gate_wire(filename);
    netlist.loadlib(lib);

    file_name.erase(file_name.length() - 2);
    string output_load = "312510191_" + file_name + "_load.txt";
    ofstream fout;
    fout.open(output_load);
    load.out_load(netlist);
    fout << fixed;
    for(int i = 0; i < netlist.totalgate.size(); i++){
        fout << netlist.totalgate[i]->name << " " << fixed << setprecision(6) << netlist.totalgate[i]->load << endl;
    } 
    fout.close();
    
    
        
    
    ofstream outputFile2("312510191_" + file_name + "_gate_info.txt");
    ofstream outputFile3("312510191_" + file_name + "_gate_power.txt");
    ofstream outputFile4("312510191_" + file_name + "_coverage.txt");
    netlist.insert_pattern(pattern);
    for(int j = 0; j < netlist.logVal.size(); j++){
        netlist.save_p(j);
        dpc.delay(netlist, j);
        for(int i = 0 ; i < netlist.totalgate.size(); i++){
            outputFile2 << netlist.totalgate[i]->name << " " << netlist.totalgate[i]->ZN->value << " " << fixed << setprecision(6) << netlist.totalgate[i]->delay << " " << fixed << setprecision(6) << 
            netlist.totalgate[i]->output_trans << endl;
            outputFile3 << netlist.totalgate[i]->name << " " << fixed << setprecision(6) << netlist.totalgate[i]->internalpower << " " << fixed << setprecision(6) << netlist.totalgate[i]->switchpower << " " 
                        << fixed << setprecision(6) <<endl; 
        }
        outputFile4 << j+1 <<" "<<fixed << setprecision(6)<< dpc.total_power <<" "<< fixed << setprecision(2) << 2.5*dpc.toggle/(netlist.totalgate.size()) << "%" <<endl;
        outputFile2<<endl;
        outputFile3<<endl;
        outputFile4<<endl;
        if(j==(netlist.logVal.size()-1)){
            break;
        }
        dpc.clear_info(netlist);
    }
    // for(auto& gate : netlist.totalgate){
    //     cout<<"gate name: "<<gate->name<<" toggle_number: "<<gate->toggle_step<<endl;
    // }

    return 0;
}