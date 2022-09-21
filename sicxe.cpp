//  main.cpp
//  hw1
//  Created by Ruby on 2021/10/20.
#include <iostream> // cout, endl
#include <fstream>    // open, is_open, close, write, ignore
#include <string>    // string, find_last_of, substr, clear
#include <cstdlib>    // atoi, system
#include <iomanip>    // setw
#include <cstring>    // strcpy
#include <vector>     // vector, push_back
#include <time.h>
#include <stdio.h>
#include<cstdio>
//#include <array>
using namespace std;
typedef struct Tabledata{
    string instruction ;
    int loc ;
    int table ;
    int CorX = 0; // C =1, X =2;
} Tabledata;

typedef struct output{
    //string sourceStatement ;
    string machineCode;
    string undefined; // 未定義label
    int line ;
    string loc ;
    bool finished = false;
    vector <string> sourceStatement;
} output;

typedef struct Base{
    bool isBASE = false;
    string symbol;
    string baseLoc;
    bool defined = false;
} Base;


class LexicalAnalysis{
    public:
    int line = 0, j = 0,count =0 ; //line ,loc, count第一二行loc一樣
    int CX = 0;
    string  sloc = "", LOC ="";
    bool END = false;
    bool TYPEX = false;
    bool start = false;
    int insformat = 0;
    int lastplus = 3;
    fstream outFile;
    vector <Tabledata> tData1, tData2, tData3, tData4, fData ;
    vector <Tabledata> undefinded;
    vector <output> labelTable;
    vector <output> outputData;
    Tabledata tData5[100],tData6[100],tData7[100]; // symbol,integer,string
    vector<string> cmtC;
    string standard;
    Base BASE;
    bool isStr, integer;
    bool ReadFile();
    void ReadTable();
    bool FindTable(string);
    void BuildTable(string);
    bool IsDig(string);
    int Hash(string);
    void initial();
    bool checkTable4(char, string);
    //--------2--------
    void assembler(string);
    string opcode(vector <Tabledata>, int);
    void assembler2();
    string Hex(string, int, int); //int 加多少, int 第幾個位置加
    int T016(char);
    string Ito16(int);
    string hex2();
    bool syntaxCheck(vector <Tabledata>, int, int );
    string machineCode(vector <Tabledata>, int, int, string );
    string intTo4Bit(int);
    string FbitsToString(string);
    int Getformat(vector <Tabledata>);
};
// ==================== MAIN() ====================
int main(void) {
    LexicalAnalysis LAItem;
    int command = 0;
    do {
        cout << endl << "************     HW2     ************";
        cout << endl << "* 0. QUIT                           *";
        cout << endl << "* 1. assembler sicxe                *";
        cout << endl << "*************************************";
        cout << endl << "Input a choice(0, 1): ";

        cin >> command; // get a command;
        switch (command) {
        case 1:
                LAItem.initial();
                LAItem.ReadTable();
                LAItem.ReadFile();
                LAItem.assembler2();
                LAItem.outFile.close();
                break;
        case 0: cout << "Quit the program !" << endl; break;
        default: cout << endl << "Command does not exist!" << endl;
        } // end switch( command )
    } while (command != 0);
    return 0;
} // end main()

// ====================⇡MAIN()⇡====================
void LexicalAnalysis::initial(){
    isStr = false;
    integer = false;
    tData1.clear();
    tData2.clear();
    tData3.clear();
    tData4.clear();
    fData.clear();
    labelTable.clear();
    outputData.clear();
    undefinded.clear();
    cmtC.clear();
    line = 5;
    j = 0;
    count =0 ; //line ,loc, count第一二行loc一樣
    CX = 0;
    END = false;
    TYPEX = false;
    insformat = 0;
    lastplus = 3;
    sloc = "";
    LOC ="";
    start = false;
        //standard = "";;
        //isStr ="" ;
        //integer ="" ;
    for(int i = 0; i < 100 ; i++){
        tData5[i].instruction = "";
        tData6[i].instruction = "";
        tData7[i].instruction = "";
    }
}

bool LexicalAnalysis::ReadFile(){
    LexicalAnalysis LAItem;
    fstream File; // input file handle
    string fileName;
    string ins = "";
    bool inTable = false, table4 = false, isTable4 = false;
    char ch;
    bool typeC = false, typeX  = false , first = true;
    string dataLine = "", comment="" ;
    bool finish = false;
    Tabledata temp;
    do {
        cout << endl << "Input a file name ( input...( [0]:quit )): ";
        cin >> fileName;
        if (!fileName.compare("0")) // No file to open, so quit
            return false;
        File.open(("SICXE_" + fileName + ".txt").c_str(), fstream::in); // open a txt file
        if (File.is_open())
            break; // successfully open a file, so leave the loop
        else {
            cout << endl << "******* SIC_" + fileName + ".txt  IS NOT EXIST!! *******" << endl ;
        } // else
    } while (true); //end while1

    if (File.is_open()) {
        outFile.open("SIC_output.txt", fstream::out);
        outFile << "Line    Loc    Source statement        Object code\n ----  --------    -----------              -----------" << endl;
        cout << "Line    Loc    Source statement        Object code\n ----  --------    -----------              -----------" << endl;
        
        do{
            finish =false;
            File >> noskipws >> ch ;
            dataLine = dataLine + ch;
            if(ch == '.'){ // 是註解
                std::getline(File, comment); //讀掉整行註解
                comment[comment.length()-1] = '\0';
                cmtC.push_back(comment);
                ins.insert(0, 1, ch);
                isTable4 = FindTable(ins);
                dataLine = "";
                table4 = true;
            }
            if(ch == '\n' && !File.eof() && !first) { //結尾就印換行
                first = false;
                //outFile << "\r\n" ;
            }
            if(ch == 'C'|| ch == 'X' || ch == '\''){
                if( ch == 'C' || ch == '\'' ) {
                    typeC = true;
                    CX = 1;
                }
                else if(ch == 'X') {
                    typeX = true;
                    CX = 2;
                }
                ins = ins + ch;
                if(ch != '\'') {
                    File >> noskipws >> ch ;
                    dataLine = dataLine + ch;
                }
                if(ch == '\'' ){
                    table4 = true;
                    ins ="";
                    ins.insert(0, 1, ch);
                    inTable = FindTable(ins); //存‘
                    File >> noskipws >> ch ;
                    dataLine = dataLine + ch;
                    ins = "";
                    if(ch == '\''){
                        ins.insert(0, 1, ch);
                        inTable = FindTable(ins); //存‘
                        ins = "";
                    }
                    else{
                        while( ch != '\'' ){ // 處理EOF
                            ins = ins+ch;
                            File >> noskipws >> ch ;
                            dataLine = dataLine + ch;
                            if(ch == '\n' || ch == '\r') {
                                break;
                            }
                        }
                        if(typeC) isStr = true;
                        else integer = true;
                        BuildTable(ins);
                        CX = 0 ;
                        ins ="";
                        if(ch != '\n' || ch != '\r'){
                            ins.insert(0, 1, ch); // char to string 把‘存回ins遇到空白再處理
                            inTable = FindTable(ins);
                            ins ="";
                        }
                        if(ch == '\n' ) {
                            first = false ;
                            dataLine = "" ;
                        }
                    }
                }
            }
            if(!table4) isTable4 =  checkTable4(ch, ins); // 判斷是否為delimiter
            if(isTable4) ins = "" ;
            if(!table4 && !isTable4){
                if(ch == ' ' || ch == '\n' || ch == '\t' || ch == '\r' || File.eof()) {
                    finish =true;
                    inTable = FindTable(ins);
                    if(inTable == false) BuildTable(ins);
                    ins = "";
                    if(ch == '\n'  ||   File.eof()){ //換行及寫入資料
                        first = false ;
                        assembler(dataLine);
                        dataLine = "" ;
                    }
                }
                else ins = ins + ch;
            }
            typeC = false;
            typeX = false;
            table4 = false;
            if(File.eof()) {
                break;
            }
        }while (true);
        File.close(); // close input file
        
        return true;
    } // end if
    else
        return false;
} // end readfile

void LexicalAnalysis::ReadTable(){
    fstream TableFile; // input file handle
    string tableinput;
    Tabledata temp;
    int location = 1 ;
    TableFile.open("Table1.table", fstream::in); // open a table file
    if(TableFile.is_open()) {
        while ( true ) {
            TableFile >> tableinput ;
            temp.instruction = tableinput;
            temp.loc =location;
            location ++ ;
            temp.table = 1;
            tData1.push_back(temp);
            if(TableFile.eof()) break;
        } //end while
        TableFile.close(); // close input file
    } // end if
    TableFile.open("Table2.table", fstream::in); // open a table file
    location = 1;
    if(TableFile.is_open()) {
        while ( true ) {
            TableFile >> tableinput ;
            temp.instruction = tableinput;
            temp.loc =location;
            location ++ ;
            temp.table = 2;
            tData2.push_back(temp);
            if(TableFile.eof()) break;
        } //end while
        TableFile.close(); // close input file
    } // end if
    TableFile.open("Table3.table", fstream::in); // open a table file
    location = 1;
    if(TableFile.is_open()) {
        while ( true ) {
            TableFile >> tableinput ;
            temp.instruction = tableinput;
            temp.loc =location;
            location ++ ;
            temp.table = 3;
            tData3.push_back(temp);
            if(TableFile.eof()) break;
        } //end while
        TableFile.close(); // close input file
    } // end if
    TableFile.open("Table4.table", fstream::in); // open a table file
    location = 1;
    if(TableFile.is_open()) {
        while ( true ) {
            TableFile >> tableinput ;
            temp.instruction = tableinput;
            temp.loc =location;
            location ++ ;
            temp.table = 4;
            tData4.push_back(temp);
            if(TableFile.eof()) break;
        } //end while
        TableFile.close(); // close input file
    } // end if
}

bool LexicalAnalysis::FindTable(string ins){
    Tabledata temp;
    string instemp  = ins ;
    string instemp2  = ins ;
    if(ins != ""){
        for(int j = 0 ; j <ins.length() ;j++){ //小寫轉大寫
            if( ins[j] >= 'a' && ins[j] <= 'z' ) instemp[j] = ins[j] - 32;
        }
        for(int j = 0 ; j < ins.length();j++){ //大寫轉小寫
           if( ins[j] >= 'A' && ins[j] <= 'Z' ) instemp2[j] = ins[j] + 32;
        }
    }
    
    if(ins != ""){
        for(int i = 0 ;i < tData1.size(); i++ ){
            if(instemp2 == tData1[i].instruction || instemp == tData1[i].instruction ){
                temp.instruction = tData1[i].instruction;
                temp.loc = tData1[i].loc;
                temp.table = tData1[i].table;
                fData.push_back(temp);
                return true;
            }
        }
        for(int i = 0 ;i < tData2.size(); i++ ){
            if(instemp2 == tData2[i].instruction || instemp == tData2[i].instruction){
                temp.instruction = tData2[i].instruction;
                temp.loc = tData2[i].loc;
                temp.table = tData2[i].table;
                fData.push_back(temp);
                return true;
            }
        }
        for(int i = 0 ;i < tData3.size(); i++ ){
            if(instemp2 == tData3[i].instruction || instemp == tData3[i].instruction ){
                temp.instruction = tData3[i].instruction;
                temp.loc = tData3[i].loc;
                temp.table = tData3[i].table;
                fData.push_back(temp);
                return true;
            }
        }
        for(int i = 0 ;i < tData4.size(); i++ ){
            if(ins == tData4[i].instruction ){
                temp.instruction = tData4[i].instruction;
                temp.loc = tData4[i].loc;
                temp.table = tData4[i].table;
                fData.push_back(temp);
                return true;
            }
        }
        return false;
    }
    return true;
}

void LexicalAnalysis::BuildTable(string ins){
    Tabledata temp;
    int hashLoc = 0;
    hashLoc = Hash(ins) ;
    if (IsDig(ins) || integer ){ // integer
        while(tData6[hashLoc].instruction != "" && tData6[hashLoc].instruction != ins){
            hashLoc ++;
        }
        tData6[hashLoc].instruction = ins;
        tData6[hashLoc].loc = hashLoc;
        tData6[hashLoc].table = 6;
        tData6[hashLoc].CorX = CX;
        fData.push_back(tData6[hashLoc]);
        integer = false;
    }
    else if(isStr){ // String
        while(tData7[hashLoc].instruction != "" && tData7[hashLoc].instruction != ins){
            hashLoc ++;
        }
        tData7[hashLoc].instruction = ins;
        tData7[hashLoc].loc = hashLoc;
        tData7[hashLoc].table = 7;
        tData7[hashLoc].CorX = CX;
        fData.push_back(tData7[hashLoc]);
        isStr = false;
    }
    else { //5
        while(tData5[hashLoc].instruction != "" && tData5[hashLoc].instruction != ins){
            hashLoc ++;
        }
        tData5[hashLoc].instruction = ins;
        tData5[hashLoc].loc = hashLoc;
        tData5[hashLoc].table = 5;
        fData.push_back(tData5[hashLoc]);
    }
}

bool LexicalAnalysis::IsDig(string ins){
    bool dig = false;
    for(int i = 0; i < ins.length() ; i++ ){
        if((ins[i] <= '9' && ins[i] >= '0') ||  ins[0] == 'X' ) dig = true;
        else return false;
    }
    if(dig) return true;
    else return false;
}

int LexicalAnalysis::Hash(string ins) {
    int loc = 0, i = 0, sum = 0 ;
    while(i < ins.length()){
        sum = sum + (int)ins[i];
        i++;
    }
    loc = sum % 100;
    return loc ;
}

bool LexicalAnalysis::checkTable4(char ch, string ins){
    bool inTable = false;
    Tabledata temp;
    for(int j = 0; j < tData4.size(); j++){
        string str = tData4[j].instruction;
        if( ch == str[0]){ //delimiter
            if(ins != ""){
                inTable = FindTable(ins);
                if(inTable == false) BuildTable(ins);
            }
            temp.instruction = tData4[j].instruction; //'
            temp.loc = tData4[j].loc;
            temp.table = tData4[j].table;
            fData.push_back(temp);
            return true;
        }
    }
    return false;
}
int LexicalAnalysis::Getformat(vector<Tabledata> temp ){
    for (int i = 0; i < temp.size() ; i++) {
        if(temp[i].instruction == "FIX" || temp[i].instruction == "FLOAT" || temp[i].instruction == "HIO" ||temp[i].instruction == "NORM"  || temp[i].instruction == "SIO" || temp[i].instruction == "TIO"){
            insformat =1;
            return insformat;
        }
        else if(temp[i].instruction == "ADDR" || temp[i].instruction == "CLEAR" ||
                temp[i].instruction == "COMPR" || temp[i].instruction == "DIVR" ||
                temp[i].instruction == "MULR" || temp[i].instruction == "RMO" ||
                temp[i].instruction == "SHIFTL" || temp[i].instruction == "SHIFTR" ||
                temp[i].instruction == "SUBR" || temp[i].instruction == "SVC" ||
                temp[i].instruction == "TIXR" ) { // format2
            insformat =2;
            return insformat;
        }
        else {
            if(temp[i].instruction == "+" ) {
                insformat = 4;
                return insformat;
            }
            else {
                insformat = 3;
                return insformat;
            }
        }
    }
    return 0 ;
}

void LexicalAnalysis::assembler( string dataline){
    //count ++;
    output tempO; //暫存每一行的結果 之後pushback到outputData
    bool first = false, Hasins = false;
    output labelTemp;
    vector <Tabledata> temp;
    string tmpLoc = "", strtmp= "", sloctemp = "", preLoc = "";
    int stmp= 0;
    if(dataline != "" && !END ){
        while( j < fData.size() ){
            //if(!start){
                for(int k = 0 ; k <fData[j].instruction.length() ;k++){ //小寫轉大寫
                    if( fData[j].instruction[k] >= 'a' && fData[j].instruction[k] <= 'z' ) fData[j].instruction[k] = fData[j].instruction[k] - 32;
                }
                temp.push_back(fData[j]); // 如果是start後面數字不存到temp
            //}
            if(fData[j].instruction == "START"){
                sloc = fData[j+1].instruction; //設定起始位置
                sloc = Hex(sloc,0,sloc.length()-1); //將讀入的位置(10進位）-> 16進位
                LOC = sloc; //設定起始位置 不更動！
                start = true;
            }
            j++ ;
            first = true;
        } //while
        if(count > 1){
            tempO.line = line;
            preLoc = sloc;
            if(TYPEX) {
                sloc = Hex(preLoc,1,preLoc.length()-1);
                TYPEX = false;
            }
            else {
                sloc = Hex(sloc,lastplus,sloc.length()-1);
                insformat = Getformat(temp);
                if(insformat == 1)lastplus=1;
                else if(insformat == 2)lastplus=2;
                else if(insformat == 3)lastplus=3;
                else if(insformat == 4)lastplus=4;
                else lastplus = 3;
                //sloc = Hex(sloc,1,sloc.length()-1);
            }
            tempO.loc = sloc;
        }
        else{
            count ++ ;
            tempO.line = line; // line & loc 一起處理 其他變數下面處理
            tempO.loc = LOC;
            start = false;
        }
        
        for (int i = 0; i < temp.size() ; i++) {
            if(((temp[i].table == 1 || temp[i].table == 2 ) && (temp[0].table != 5 ) && !(temp[0].instruction == "." && temp[3].table == 5)) &&  !(temp[i].table == 1 && temp[i-1].instruction == "+")){
                tempO.sourceStatement.push_back("   "); //非label
            }
            
            tempO.sourceStatement.push_back(temp[i].instruction);
//            if( temp[i].table == 4 || temp[i-1].table == 4 ){
//                tempO.sourceStatement = tempO.sourceStatement + temp[i].instruction;
//            }
//            else tempO.sourceStatement = tempO.sourceStatement +"    "+ temp[i].instruction;

            
            if(temp[i].instruction == "BASE") {
                BASE.isBASE = true;
                BASE.symbol = temp[i+1].instruction;
            }
            else if(temp[i].instruction == "BASE"){
                BASE.isBASE = false;
            }
            else if(temp[i].instruction == BASE.symbol && i == 0){
                BASE.baseLoc = sloc;
                BASE.defined = true;
            }
            else if(temp[i].instruction == "EQU" ) {
                //tempO.machineCode =strtmp;
                tempO.finished = true;
            }
            else if( temp[i].table == 2 ){ // table2
                if(temp[i].instruction == "BYTE"){
                    string SStemp = "" ;
                    for(i = i+1 ; i < temp.size(); i++){
                        if(temp[i+1].CorX == 1)SStemp ="C";
                        else if (temp[i+1].CorX == 2)SStemp ="X" ;
                        //tempO.sourceStatement = tempO.sourceStatement + temp[i].instruction;
                        SStemp +=temp[i].instruction;
                        if(i == temp.size()-1)tempO.sourceStatement.push_back(SStemp);
                        if(temp[i].instruction != "\'"){
                            for(int k = 0 ; k <temp[i].instruction.length(); k++){
                                if(temp[i].CorX == 1) { // typeC
                                    stmp  =  int(temp[i].instruction[k]) ;
                                    strtmp += Ito16(stmp); // ascii 10進位轉16進位
                                }// typeC
                                else { //typeX
                                    TYPEX = true;
                                    strtmp = temp[i].instruction;
                                }// typeC
                            }
                        }
                    }
                    tempO.machineCode =strtmp;
                    tempO.finished = true;
                }
                else if(temp[i].instruction == "WORD"){
                    string smp;
                    i++;
                    //tempO.sourceStatement.push_back("   ");
                    tempO.sourceStatement.push_back(temp[i].instruction);
                    //tempO.sourceStatement = tempO.sourceStatement +"    "+ temp[i].instruction;
                    stmp= stoi(temp[i].instruction);
                    smp += Ito16(stmp);
                    stmp = 6-smp.length();
                    for(int k = 0 ; k <stmp; k++){
                        strtmp +=  "0";
                    }
                    strtmp += smp;
                    tempO.machineCode = strtmp;
                    tempO.finished = true;
                }
                else if(temp[i].instruction == "RESB"){
                    i++;
                    tempO.sourceStatement.push_back(temp[i].instruction);
                    stmp= stoi(temp[i].instruction) ; //ex: 4096
                    stmp -= 3;
                    strtmp += Ito16(stmp); // strtmp FFD ,4093轉16進制 strtmp(strng)
                    if(strtmp.length()<4){
                        int length =4 -strtmp.size();
                        for(int k = 0; k < length ; k++){
                            strtmp = "0"+strtmp; //0009
                        }
                    }
                    for(int k = strtmp.length()-1 ; k >=0; k--){
                        stmp = T016(strtmp[k]); //stmp (int)
                        sloc = Hex(sloc, stmp, k);
                    }
                    tempO.finished = true;
                }
                else if(temp[i].instruction == "END"){
                    //tempO.sourceStatement = tempO.sourceStatement + "   " +temp[i+1].instruction;
                    //tempO.sourceStatement.push_back("   ");
                    tempO.sourceStatement.push_back(temp[i+1].instruction);
                    tempO.machineCode = "";
                    tempO.finished=true;
                    tempO.loc = "";
                    //outputData.push_back(tempO);
                    END = true;
                    break;
                }
                else{
                    tempO.machineCode = "";
                    tempO.finished = true;
                }
            }
            else if( temp[i].table == 5 &&  (i == 0 || i == 3) ){ // 為label
                if(Hasins) {
                    line += 5;
                    cout <<line <<  " LLLhasins"<<endl;
                    tempO.line = line;
                    tempO.loc = tmpLoc;
                    Hasins = false;
                }
                labelTemp.sourceStatement.push_back(temp[i].instruction);
                labelTemp.loc = tempO.loc; // 存label位置
                labelTable.push_back(labelTemp); // label vector
            }
            else if( temp[i].table == 1 ){ //instruction
                if(Hasins) {
                    line += 5;
                    tempO.line = line;
                    tempO.loc = tmpLoc;
                    Hasins = false;
                }
                
                //else{
                string m =opcode(temp, i);
                if(m != "ERROR" && m != "UNDEFINED" ) {
                    tempO.machineCode += m ;
                    tempO.finished = true;
                }
                else if(m == "UNDEFINED") {
                    for(int k=0; k<temp.size();k++){
                        if(temp[0].instruction != "COMP") undefinded.push_back(temp[k]);
                    }
                    Tabledata t;
                    t.instruction = "!";
                    undefinded.push_back(t);
                    tempO.machineCode = "UNDEFINED";
                    tempO.finished = false;
                }
                else {
                    tempO.machineCode = "ERROR";
                    tempO.finished = true;
                }
                //}
                
                if(temp[i].instruction == "RSUB" && i == temp.size()-1 ){
                    tempO.sourceStatement.push_back("");
                    tempO.machineCode += "0000";
                    tempO.finished = true;
                }
            }
            else if(temp[i].instruction == "X"){ //standard type
                int s = 0;
                char c = standard[0];
                int a = c-'0';
                a += 8;
                standard[0] = a + '0';
                for(int k = 2; k < tempO.machineCode.length() ; k ++ ){
                    tempO.machineCode[k] =standard[s];
                    s++;
                }
            }
            else if( temp[i].instruction == "." ){ //註解
                tempO.sourceStatement.push_back(cmtC[0]); // 註解後的內容
                cmtC.erase(cmtC.begin()); //清掉第一個註解 因為已經存完
                if(!Hasins)tmpLoc =tempO.loc;
                tempO.loc = "" ;
                if(Hasins) {
                    line += 5;
                    tempO.line = line ;
                }
                tempO.finished =true;
                outputData.push_back(tempO);
                tempO.sourceStatement.clear() ;
                //tempO.sourceStatement[0] = "";
                Hasins = true;
            }
            if(END) break;
        }
        outputData.push_back(tempO);
        tempO.machineCode = "";
        for(int k = 0 ; k < tempO.sourceStatement.size(); k ++ ){
        }
        temp.clear();
        line += 5;
    }
    
}
string LexicalAnalysis::Ito16(int integer){
    int  remainder = 0 ;
    char c;
    string ans , finalans;
    if(integer == 0 ) finalans = "0";
    while(integer > 0 ){
        remainder = integer%16;
        integer/=16;
        if(remainder < 10 ){
            c = remainder + '0'; // int to char
            ans += c;
        }
        else{
            if(remainder == 10){
                ans += 'A';
            }
            else if(remainder == 11){
                ans += 'B';
            }
            else if(remainder == 12){
                ans += 'C';
            }
            else if(remainder == 13){
                ans += 'D';
            }
            else if(remainder == 14){
                ans += 'E';
            }
            else if(remainder == 15){
                ans += 'F';
            }
        }
    }
    for(int i = ans.length()-1 ; i>=0 ; i -- ){
        finalans += ans[i];
    }
    return finalans;
    
    
}

string LexicalAnalysis::Hex(string hex, int p, int w ) { // p: plus value, w = where plus
    string ans ="", finalans = "";
    char c ;
    int carry = -1,hextemp = 0,temp =0;
    int length = 0;
    string zero = "";
    if(hex.size() != 4 ) {
        length = 4-hex.size();
        for(int k = 0 ; k <length ;k++ ){
            zero += "0";
        }
        hex =zero + hex;
    }
    for(int i = hex.length()-1 ; i >= 0; i-- ){
        if( i == w ){
            temp = T016(hex[i]);
            hextemp = temp +p ;
        }
        else{
            hextemp = T016(hex[i]);
        }
        if(carry > 0 ) { // 進位
            hextemp ++;
            carry = -1;
        }
        if(hextemp < 10 ){
            c = hextemp + '0'; // int to char
            ans += c;
        }
        else{
            if(hextemp == 10){
                ans += 'A';
            }
            else if(hextemp == 11){
                ans += 'B';
            }
            else if(hextemp == 12){
                ans += 'C';
            }
            else if(hextemp == 13){
                ans += 'D';
            }
            else if(hextemp == 14){
                ans += 'E';
            }
            else if(hextemp == 15){
                ans += 'F';
            }
            else { // 16
                carry = hextemp - 15 ;
                ans += carry-1 + '0';
            }
        }
    }
    
    for(int i = hex.length()-1 ; i >=0 ; i -- ){
        finalans += ans[i];
    }
    return finalans;
}

int LexicalAnalysis::T016(char c ){
    int temp= 0;
    if(c == 'A'){
        temp = 10;
    }
    else if(c == 'B'){
        temp = 11;
    }
    else if(c == 'C'){
        temp = 12;
    }
    else if(c == 'D'){
        temp = 13;
    }
    else if(c == 'E'){
        temp = 14;
    }
    else if(c == 'F'){
        temp = 15;
    }
    else{
        temp = c - '0';
    }
    return temp;
}

void LexicalAnalysis::assembler2(){
    vector <Tabledata> temp;
    int n = 0;
    int k = 0;
    for (int i =0 ; i < outputData.size() ; i++){
        if(outputData[i].sourceStatement[1] == "COMP"){
            string op = "28", strtemp,opstr,f3 = "",finalM  = ""  ;
            int optemp =0 ;
            for(int k = 0; k < op.length(); k++){ //8bit
                optemp = T016(op[k]);
                //optemp = op[k] - '0';
                opstr = intTo4Bit(optemp);
                f3 += opstr;
            }
            optemp = T016(outputData[i+1].sourceStatement[0][0]);
            opstr = intTo4Bit(optemp);
            for(int k = 0; k < 16-opstr.length() ; k ++){
                strtemp +="0";
            }
            f3 = f3+  strtemp +opstr;
            strtemp = "";
            f3[6] = '1'; // sicxe n
            f3[7] = '1'; //sicxe i
            strtemp ="" ;
            int cnt  = 0;
            for(int k = 0 ; k < f3.size(); k ++ ){
                cnt ++ ;
                strtemp += f3[k];
                if(cnt %4 == 0 ){
                    finalM += FbitsToString(strtemp);
                    strtemp = "" ;
                }
            } // for
            outputData[i].machineCode = finalM ;
            outputData[i].finished = true ;
        }
        else if( !outputData[i].finished || outputData[i].machineCode == "UNDEFINED" ){ //未完成machine code
            if(outputData[i].machineCode !="ERROR" ){
                for(int k = k ; k < undefinded.size(); k++){
                    if(undefinded[k].instruction!="!"){
                        if(undefinded[k].table ==1 ) n=k;
                        temp.push_back(undefinded[k]);
                    }
                    if(undefinded[k].instruction=="!"){
                        string m =opcode(temp,n);
                        if(m != "ERROR" && m != "UNDEFINED") {
                            outputData[i].machineCode ="" ;
                            outputData[i].machineCode +=m ;
                            break;
                        }
                        else {
                            outputData[i].machineCode = "ERROR";
                            outputData[i].finished = true;
                            break;
                        }
                    }
                }
            }
            
        }
        
        if(outputData[i].line >= 5 ){
            //int k =outputData[outputData.size()-1].line;
            cout << setw(3) << outputData[i].line<< "  ";
            outFile << setw(3) << outputData[i].line<< "  ";
        }
        cout <<  setw(4) <<outputData[i].loc << "  ";
        outFile <<  setw(4) <<outputData[i].loc<< "  ";
        bool first = false;
        for(int k =0 ; k < outputData[i].sourceStatement.size() ; k ++ ){
            //cout <<outputData[i].sourceStatement[k];
            if(outputData[i].sourceStatement[0] == "+" && !first ){
                cout <<  setw(12)<< "  ";;
                outFile <<setw(12) << "  ";
                first = true;
            }
            if(outputData[i].sourceStatement[k+1] == "," || outputData[i].sourceStatement[k] == "," ||
               outputData[i].sourceStatement[k] == "+"  ||  outputData[i].sourceStatement[k] == "." ){
                //cout <<outputData[i].sourceStatement[k];
                outFile <<setiosflags(ios::left) <<outputData[i].sourceStatement[k] ;
                cout <<setiosflags(ios::left)<< setiosflags(ios::left) << outputData[i].sourceStatement[k];
            }
            else if(outputData[i].sourceStatement[k] == "FIX" || outputData[i].sourceStatement[k] == "FLOAT"||
                    outputData[i].sourceStatement[k] == "HIO" || outputData[i].sourceStatement[k] == "NORM"||outputData[i].sourceStatement[k] == "SIO" || outputData[i].sourceStatement[k] == "TIX"){
                outFile << setw(20)<<setiosflags(ios::left) <<outputData[i].sourceStatement[k] << "    ";
                cout << setw(22)<<setiosflags(ios::left)<< setiosflags(ios::left) << outputData[i].sourceStatement[k]<< "  ";
            }
            else if(outputData[i].sourceStatement[k-1] == "+") { // format2
                outFile << setw(7)<<setiosflags(ios::left) <<outputData[i].sourceStatement[k] << "    ";
                cout << setw(9)<<setiosflags(ios::left)<< setiosflags(ios::left) << outputData[i].sourceStatement[k]<< "  ";
            }
            else{
                //cout <<outputData[i].sourceStatement[k];
                outFile << setw(6)<<setiosflags(ios::left) <<outputData[i].sourceStatement[k] <<"      ";
                cout <<setw(6)<< setiosflags(ios::left)<< setiosflags(ios::left) <<outputData[i].sourceStatement[k] <<"      ";
            }
        }
        cout << setw(10)<<setiosflags(ios::left)<<outputData[i].machineCode <<endl;
        outFile <<setw(10)<< setiosflags(ios::left)<< setiosflags(ios::left)<<outputData[i].machineCode <<endl;
    }
}

string LexicalAnalysis::opcode(vector <Tabledata> temp, int i){ // 順便做sytax analysis
    bool isERROR = false;
    string machineC ;
    for(int c = 0 ; c < temp[i].instruction.length();c++){ //大寫轉小寫
       if( temp[i].instruction[c] >= 'A' && temp[i].instruction[c] <= 'Z' ) temp[i].instruction[c] = temp[i].instruction[c] + 32;
    }
    string op = "";
    if(temp[i].instruction == "add"){ //3,4 m
        isERROR = syntaxCheck(temp, i, 3);
        op = "18";
        if(!isERROR) machineC = machineCode(temp, i, 3, op ) ;//3,4統稱進去再決定是3or4
    }
    else if(temp[i].instruction == "addf"){ // 3,4 m
        isERROR = syntaxCheck(temp, i, 3);
        op = "58";
        if(!isERROR) machineC = machineCode(temp, i, 3, op ) ;//3,4統稱進去再決定是3or4
    }
    if(temp[i].instruction == "addr"){
        isERROR = syntaxCheck(temp, i, 2);
        op = "90";
        if(!isERROR) machineC = machineCode(temp, i, 2, op ) ;
    }
    else if(temp[i].instruction == "and"){
        isERROR = syntaxCheck(temp, i, 3);
        op = "40";
        if(!isERROR) machineC = machineCode(temp, i, 3, op ) ;
        
    }
    if(temp[i].instruction == "clear"){
        isERROR = syntaxCheck(temp, i, 2);
        op = "B4";
        if(!isERROR) machineC = machineCode(temp, i, 2, op ) ;
    }
    else if(temp[i].instruction == "comp"){
        isERROR = syntaxCheck(temp, i, 3);
        op = "28";
        if(!isERROR) machineC = machineCode(temp, i, 3, op ) ;
        
    }
    if(temp[i].instruction == "compf"){
        isERROR = syntaxCheck(temp, i, 3);
        op = "88";
        if(!isERROR) machineC = machineCode(temp, i, 3, op ) ;
    }
    if(temp[i].instruction == "compr"){
        isERROR = syntaxCheck(temp, i, 2);
        op = "A0";
        if(!isERROR) machineC = machineCode(temp, i, 2, op ) ;
    }
    else if(temp[i].instruction == "div"){
        isERROR = syntaxCheck(temp, i,3);
        op = "24";
        if(!isERROR) machineC = machineCode(temp, i, 3, op ) ;
    }
    else if(temp[i].instruction == "divf"){
        isERROR = syntaxCheck(temp, i, 3);
        op = "64";
        if(!isERROR) machineC = machineCode(temp, i, 3, op ) ;
    }
    if(temp[i].instruction == "divr"){
        isERROR = syntaxCheck(temp, i, 2);
        op = "9C";
        if(!isERROR) machineC = machineCode(temp, i, 2, op ) ;
    }
    if(temp[i].instruction == "fix"){
        isERROR = syntaxCheck(temp, i, 1);
        op = "C4";
        if(!isERROR) machineC = machineCode(temp, i, 1, op ) ;
    }
    if(temp[i].instruction == "float"){
        isERROR = syntaxCheck(temp, i, 1);
        op = "C0";
        if(!isERROR) machineC = machineCode(temp, i, 1, op ) ;
    }
    if(temp[i].instruction == "hio"){
        isERROR = syntaxCheck(temp, i, 1);
        op = "F4";
        if(!isERROR) machineC = machineCode(temp, i, 1, op ) ;
    }
    else if(temp[i].instruction == "j"){
        isERROR = syntaxCheck(temp, i,3);
        op = "3C";
        if(!isERROR) machineC = machineCode(temp, i, 3, op ) ;
    }
    else if(temp[i].instruction == "jeq"){
        isERROR = syntaxCheck(temp, i,3);
        op = "30";
        if(!isERROR) machineC = machineCode(temp, i, 3, op ) ;
    }
    else if(temp[i].instruction == "jet"){
        isERROR = syntaxCheck(temp, i, 3);
        op = "34";
        if(!isERROR) machineC = machineCode(temp, i, 3, op ) ;
    }
    else if(temp[i].instruction == "jlt"){
        isERROR = syntaxCheck(temp, i ,3);
        op = "38";
        if(!isERROR) machineC = machineCode(temp, i, 3, op ) ;
    }
    else if(temp[i].instruction == "jsub"){
        isERROR = syntaxCheck(temp, i ,3);
        op = "48";
        if(!isERROR) machineC = machineCode(temp, i, 3, op ) ;
    }
    else if(temp[i].instruction == "lda"){
        isERROR = syntaxCheck(temp, i, 3);
        op = "00";
        if(!isERROR) machineC = machineCode(temp, i, 3, op ) ;
    }
    if(temp[i].instruction == "ldb"){
        isERROR = syntaxCheck(temp, i, 3);
        op = "68";
        if(!isERROR) machineC = machineCode(temp, i, 3, op ) ;
    }
    else if(temp[i].instruction == "ldch"){
        isERROR = syntaxCheck(temp, i, 3);
        op = "50";
        if(!isERROR) machineC = machineCode(temp, i, 3, op ) ;
    }
    if(temp[i].instruction == "ldf"){
        isERROR = syntaxCheck(temp, i, 3);
        op = "70";
        if(!isERROR) machineC = machineCode(temp, i, 3, op ) ;
    }
    else if(temp[i].instruction == "ldl"){
        isERROR = syntaxCheck(temp, i,3 );
        op = "08";
        if(!isERROR) machineC = machineCode(temp, i, 3, op ) ;
    }
    else if(temp[i].instruction == "lds"){
        isERROR = syntaxCheck(temp, i,3);
        op = "6C";
        if(!isERROR) machineC = machineCode(temp, i, 3, op ) ;
    }
    if(temp[i].instruction == "ldt"){
        isERROR = syntaxCheck(temp, i,3);
        op = "74";
        if(!isERROR) machineC = machineCode(temp, i, 3, op ) ;
    }
    else if(temp[i].instruction == "ldx"){
        isERROR = syntaxCheck(temp, i,3);
        op = "04";
        if(!isERROR) machineC = machineCode(temp, i, 3, op ) ;
    }
    if(temp[i].instruction == "lps"){
        isERROR = syntaxCheck(temp, i,3);
        op = "D0";
        if(!isERROR) machineC = machineCode(temp, i, 3, op ) ;
    }
    else if(temp[i].instruction == "mul"){
        isERROR = syntaxCheck(temp, i,3 );
        op = "20";
        if(!isERROR) machineC = machineCode(temp, i, 3, op ) ;
    }
    if(temp[i].instruction == "mulf"){
        isERROR = syntaxCheck(temp, i,3 );
        op = "60";
        if(!isERROR) machineC = machineCode(temp, i, 3, op ) ;
    }
    if(temp[i].instruction == "mulr"){
        isERROR = syntaxCheck(temp, i,2 );
        op = "98";
        if(!isERROR) machineC = machineCode(temp, i, 2, op ) ;
    }
    if(temp[i].instruction == "norm"){
        isERROR = syntaxCheck(temp, i,1 );
        op = "C8";
        if(!isERROR) machineC = machineCode(temp, i, 1, op ) ;
    }
    else if(temp[i].instruction == "or"){
        isERROR = syntaxCheck(temp, i, 3);
        op = "44";
        if(!isERROR) machineC = machineCode(temp, i, 3, op ) ;
    }
    else if(temp[i].instruction == "rd"){
        isERROR = syntaxCheck(temp, i,3);
        op = "D8";
        if(!isERROR) machineC = machineCode(temp, i, 3, op ) ;
    }
    if(temp[i].instruction == "rmo"){
        isERROR = syntaxCheck(temp, i,2);
        op = "AC";
        if(!isERROR) machineC = machineCode(temp, i, 2, op ) ;
    }
    if(temp[i].instruction == "rsub"){
        isERROR = syntaxCheck(temp, i,3);
        op = "4C";
        if(!isERROR) machineC = machineCode(temp, i, 3, op ) ;
    }
    if(temp[i].instruction == "shiftl"){
        isERROR = syntaxCheck(temp, i,3);
        op = "A4";
        if(!isERROR) machineC = machineCode(temp, i, 2, op ) ;
    }
    if(temp[i].instruction == "shiftr"){
        isERROR = syntaxCheck(temp, i,2);
        op = "A8";
        if(!isERROR) machineC = machineCode(temp, i, 2, op ) ;
    }
    if(temp[i].instruction == "sio"){
        isERROR = syntaxCheck(temp, i,1);
        op = "F0";
        if(!isERROR) machineC = machineCode(temp, i, 1, op ) ;
    }
    if(temp[i].instruction == "ssk"){
        isERROR = syntaxCheck(temp, i,3);
        op = "EC";
        if(!isERROR) machineC = machineCode(temp, i, 3, op ) ;
    }
    else if(temp[i].instruction == "sta"){
        isERROR = syntaxCheck(temp, i, 3);
        op = "0C";
        if(!isERROR) machineC = machineCode(temp, i, 3, op ) ;
    }
    if(temp[i].instruction == "stb"){
        isERROR = syntaxCheck(temp, i, 3);
        op = "78";
        if(!isERROR) machineC = machineCode(temp, i, 3, op ) ;
    }
    else if(temp[i].instruction == "stch"){
        isERROR = syntaxCheck(temp, i,3);
        op = "54";
        if(!isERROR) machineC = machineCode(temp, i, 3, op ) ;
    }
    if(temp[i].instruction == "stf"){
        isERROR = syntaxCheck(temp, i,3);
        op = "80";
        if(!isERROR) machineC = machineCode(temp, i, 3, op ) ;
    }
    if(temp[i].instruction == "sti"){
        isERROR = syntaxCheck(temp, i,3);
        op = "D4";
        if(!isERROR) machineC = machineCode(temp, i, 3, op ) ;
    }
    else if(temp[i].instruction == "stl"){
        isERROR = syntaxCheck(temp, i,3 );
        op = "14";
        if(!isERROR) machineC = machineCode(temp, i, 3, op ) ;
    }
    else if(temp[i].instruction == "sts"){
        isERROR = syntaxCheck(temp, i,3);
        op = "7C";
        if(!isERROR) machineC = machineCode(temp, i, 3, op ) ;
    }
    else if(temp[i].instruction == "stsw"){
        isERROR = syntaxCheck(temp, i,3);
        op = "E8";
        if(!isERROR) machineC = machineCode(temp, i, 3, op ) ;
    }
    if(temp[i].instruction == "stt"){
        isERROR = syntaxCheck(temp, i,3);
        op = "84";
        if(!isERROR) machineC = machineCode(temp, i, 3, op ) ;
    }
    else if(temp[i].instruction == "stx"){
        isERROR = syntaxCheck(temp, i,3);
        op = "10";
        if(!isERROR) machineC = machineCode(temp, i, 3, op ) ;
    }
    else if(temp[i].instruction == "sub"){
        isERROR = syntaxCheck(temp, i,3);
        op = "1C";
        if(!isERROR) machineC = machineCode(temp, i, 3, op ) ;
    }
    if(temp[i].instruction == "subf"){
        isERROR = syntaxCheck(temp, i,3);
        op = "5C";
        if(!isERROR) machineC = machineCode(temp, i, 3, op ) ;
    }
    if(temp[i].instruction == "subr"){
        isERROR = syntaxCheck(temp, i,2);
        op = "94";
        if(!isERROR) machineC = machineCode(temp, i, 2, op ) ;
    }
    if(temp[i].instruction == "svc"){
        isERROR = syntaxCheck(temp, i,2);
        op = "B0";
        if(!isERROR) machineC = machineCode(temp, i, 2, op ) ;
    }
    else if(temp[i].instruction == "td"){
        isERROR = syntaxCheck(temp, i,3);
        op = "E0";
        if(!isERROR) machineC = machineCode(temp, i, 3, op ) ;
    }
    if(temp[i].instruction == "tio"){
        isERROR = syntaxCheck(temp, i,3);
        op = "F8";
        if(!isERROR) machineC = machineCode(temp, i, 3, op ) ;
    }
    else if(temp[i].instruction == "tix"){
        isERROR = syntaxCheck(temp, i,3);
        op = "2C";
        if(!isERROR) machineC = machineCode(temp, i, 3, op ) ;
    }
    if(temp[i].instruction == "tixr"){
        isERROR = syntaxCheck(temp, i,2);
        op = "B8";
        if(!isERROR) machineC = machineCode(temp, i, 2, op ) ;
    }
    else if(temp[i].instruction == "wd"){
        isERROR = syntaxCheck(temp, i,3);
        op = "DC";
        if(!isERROR) machineC = machineCode(temp, i, 3, op ) ;
    }
    //else isERROR = true;
    if( isERROR ) machineC = "ERROR";
    return machineC ;
}

bool LexicalAnalysis::syntaxCheck( vector <Tabledata> temp, int i , int format){
    bool isERROR = false;
    if(format == 1) {
        if(temp.size() !=  1) isERROR= true;
        return isERROR;
    }
    else if(format == 2){
        if((temp[1].table != 3 || temp[2].instruction != "+" ||temp[3].table != 3 ) && temp[0].instruction!="SHIFTL" && temp[0].instruction!="SHIFTR" && temp[0].instruction!="SVC") isERROR= true;
        if(temp[0].instruction!="SHIFTL" && temp[0].instruction!="SHIFTR" ){
            if(temp[1].table == 3 &&  temp[2].instruction == "," )isERROR= false;
        }
        if(temp[0].instruction!="CLEAR") {
            if(temp[0].table == 1 &&  temp[1].table == 3 )isERROR= false;
        }
        return isERROR;
    }
    else if(format == 3){
        if(temp[0].instruction == "+"){
            if(temp.size() != 3|| temp[2].table !=5 ) isERROR= true;
            if(temp[0].instruction == "RUSB") isERROR= false;
            if(temp[0].instruction == ".") isERROR= false;
            return isERROR;
        }
        if(temp.size() != 2 ) isERROR= true;
        if(temp[0].instruction == "RUSB") isERROR= false;
        if(temp[0].instruction == ".") isERROR= false;
        return isERROR;
    }
    return isERROR ;
}

string LexicalAnalysis::machineCode(vector<Tabledata> temp, int i, int format, string op){
    insformat = 0;
    bool format4 = false;
    bool firstOne = false;
    bool finds = false;
    string f3 = "" , f4 = "" ;
    string TAstr,PCstr,Bstr;
    int optemp , o = 0;
    int TA = 0, PC = 0 , B =0;;
    int tmpPC = 0 , tmpTA = 0 ,lenght =0, tmpB =0 ;
    string opstr="", T ="", finalM = "",strtemp = "", tatemp = "" ,bremp="" ;
    //if(temp[i].instruction == "COMP") finds = true;
    for(int k = 0; k< temp.size() ; k++){
        for(int f = 0 ; f <temp[k].instruction.length() ;f++){ //小寫轉大寫
            if( temp[k].instruction[f] >= 'a' && temp[k].instruction[f] <= 'z' ) temp[k].instruction[f] = temp[k].instruction[f] - 32;
        }
    }
    //if(temp[i].instruction == "COMP") finds = true;
    if(format == 3 ){
        
        for(int k = 0; k< temp.size() ; k++){
            if(temp[k].instruction == "+"){
                format4 = true;
            }
        }
        for(int k = 0; k < op.length(); k++){ //8bit
            optemp = T016(op[k]);
            //optemp = op[k] - '0';
            opstr = intTo4Bit(optemp);
            f3 += opstr;
        }
        for(int k = 0 ; k < labelTable.size() ; k ++ ){ // 找到label的LOC
            if(temp[i+1].instruction == labelTable[k].sourceStatement[0] ){
                finds = true;
                T = labelTable[k].loc;
                //TA = stoi(T);
                for(int k = 0; k < sloc.length(); k++){ //8bit
                    optemp = T016(sloc[k]);
                    if(k == 0 ) tmpTA*=1;
                    else if(k == 1 ) tmpTA*=16;
                    else if(k == 2 ) tmpTA*=256;
                    else if(k == 2 ) tmpTA*=4096;
                    TA+=tmpTA;
                }
            }
        }
        //if(temp[i].instruction == "COMP") finds = true;
        if(!finds) return "UNDEFINED";
        if(temp[i].instruction != "COMP"){
            for(int k =0; k < T.size() ; k ++ ){ //ta 0000
                tatemp="";
                tmpTA = T016(T[k]);
                //tmpTA = T[k]-'0';
                tatemp = intTo4Bit( tmpTA );
                TAstr +=tatemp;
            } // for
        }
        
        if(!format4){ //format3 ,18
            insformat =3;
            for(int k = 0; k < sloc.length(); k++){ //8bit
                optemp = T016(sloc[k]);
                if(k == 0 ) tmpTA*=1;
                else if(k == 1 ) tmpPC*=16;
                else if(k == 2 ) tmpPC*=256;
                else if(k == 2 ) tmpPC*=4096;
                PC+=tmpPC;
            }
            string pc = Hex(sloc, 3,sloc.length()-1 );
            if(temp[0].instruction == "COMP" && temp[1].instruction >= "0" && temp[1].instruction <= "0"){
                finds = true;
                optemp = T016(temp[1].instruction[0]);
                opstr = intTo4Bit(optemp);
                for(int k = 0; k < 16-opstr.length() ; k ++){
                    strtemp +="0";
                }
                f3 = f3+  strtemp +opstr;
                strtemp = "";
            }
            
            else if(!BASE.isBASE || TA-PC >= 0  ){ // 沒有basereg 所以用pc //else
                
                for(int k =0; k <pc.size() ; k ++ ){ // pc 0012 002A
                    strtemp="";
                    tmpPC = T016(pc[k]);
                    //tmpPC = sloc[k]-'0';
                    strtemp = intTo4Bit( tmpPC );
                    PCstr+=strtemp;
                } // for
                //pC做2's complement
                for(int k = PCstr.length()-1 ; k >= 0  ; k-- ){
                    if(!firstOne) PCstr[k] = PCstr[k];
                    if(firstOne){
                        if(PCstr[k] == '0') PCstr[k] = '1';
                        else PCstr[k] = '0';
                    }
                    if(PCstr[k] == '1' )firstOne =true;
                    
                } //for
                // ta + pc's 2complement
                for(int k = TAstr.length()-1 ; k >= 0  ;k --){
                    o = TAstr[k] -'0';
                    PCstr = Hex(PCstr, o, k);
                } // for
                string ppppc = "";
                if(PCstr.length()!=16){
                    for(int k = 1; k < PCstr.length(); k++){
                        ppppc +=PCstr[k];
                    }
                    PCstr = ppppc;
                }
                f3 += PCstr;
                f3[8] = '0'; //sicxe x
                f3[9] = '0'; //sicxe b
                f3[10] = '1'; //sicxe p
                f3[11] = '0'; //sicxe p
            }
        } // format4
        else if(BASE.isBASE && TA-PC < 0){
            if(!BASE.defined){ //
                for(int k = 0 ; k < labelTable.size() ; k ++ ){ // 找到label的LOC
                    if(BASE.symbol == labelTable[k].sourceStatement[0] ){
                        BASE.baseLoc =labelTable[k].loc;
                        BASE.defined = true;
                    }
                }
                if(!BASE.defined){ //
                    finalM = "UNDEFINED";
                }
                else{ // 已經defined
                    for(int k =0; k < BASE.baseLoc.length() ; k ++ ){ // pc 0012 002A
                        strtemp="";
                        tmpB = T016(BASE.baseLoc[k]);
                        //tmpPC = sloc[k]-'0';
                        strtemp = intTo4Bit( tmpB );
                        Bstr+=strtemp;
                    } // for
                    for(int k = BASE.baseLoc.length()-1 ; k >= 0  ; k-- ){ // B do 2's complement
                        if(!firstOne) Bstr[k] = Bstr[k];
                        if(firstOne){
                            if(Bstr[k] == '0') Bstr[k] = '1';
                            else Bstr[k] = '0';
                        }
                        if(Bstr[k] == '1' )firstOne =true;
                    } //for
                    // ta + B's 2complement
                    for(int k = TAstr.length()-1 ; k >= 0  ;k --){
                        o = TAstr[k] -'0';
                        Bstr = Hex(Bstr, o, k);
                    } // for
                    string ppppc = "";
                    if(Bstr.length()!=16){
                        for(int k = 1; k < Bstr.length(); k++){
                            ppppc +=Bstr[k];
                        }
                        Bstr = ppppc;
                    }
                    f3 += Bstr;
                    f3[8] = '0'; //sicxe x
                    f3[9] = '1'; //sicxe b
                    f3[10] = '0'; //sicxe p
                    f3[11] = '0'; //sicxe p
                }
            }
        }
        else if(format4){
            insformat = 4;
            if(T.length() < 5){
                lenght = 5 -T.length();
                for(int k = 0 ; k < lenght ; k++){
                    strtemp += "0" ;
                }
                T = strtemp+T;
            }
            f3 = f3 + "0001" ;
        }
        f3[6] = '1'; // sicxe n
        f3[7] = '1'; //sicxe i
        strtemp ="" ;
        int cnt  = 0;
        for(int k = 0 ; k < f3.size(); k ++ ){
            cnt ++ ;
            strtemp += f3[k];
            if(cnt %4 == 0 ){
                finalM += FbitsToString(strtemp);
                strtemp = "" ;
            }
        } // for
        if(format4) finalM+=T;
        
        
    } // if
    else if(format == 2){
        finds = true;
        insformat = 2;
        string regnum = "";
        for(int k = 0; k < op.length(); k++){ //8bit
            optemp = T016(op[k]);
            //optemp = op[k] - '0';
            opstr = intTo4Bit(optemp);
            finalM += FbitsToString(opstr);
        }
        for(int k = 0 ; k < temp.size() ; k++ ){
            if(temp[k].instruction == "A")regnum += "0";
            else if(temp[k].instruction == "X")regnum += "1";
            else if(temp[k].instruction == "L")regnum += "2";
            else if(temp[k].instruction == "B")regnum += "3";
            else if(temp[k].instruction == "S")regnum += "4";
            else if(temp[k].instruction == "T")regnum += "5";
            else if(temp[k].instruction == "F")regnum += "6";
            else regnum = "";
            finalM +=regnum;
        }
        if(temp[0].instruction == "CLEAR") finalM += "0";
        
    }
    else if(format == 1){
        finds =true;
        finalM = op;
    }
    if(finds)return finalM;
    else {
        finalM = "UNDEFINED";
        return finalM;
    }
}

string LexicalAnalysis::intTo4Bit( int integer ){
    string fourBit ;
    int num = 8;
    if(integer == 0 ) fourBit ="0000";
    else{
        for(int k = 0; k< 4 ; k ++ ){
            if( integer - num >= 0 ){
                integer -= num ;
                fourBit+= "1";
            }
            else if(integer - num < 0 ){
                fourBit+= "0";
            }
            num/=2;
        }
    } // format 3.4
    return fourBit;
}

string LexicalAnalysis::FbitsToString( string FourBits ){
    string hex = "";
    int num = 0;
    for(int k = 0 ; k < FourBits.length() ; k ++){
        if(k == 0 && FourBits[k] == '1' ) num += 8;
        else if(k == 1 && FourBits[k] == '1' ) num += 4;
        else if(k == 2 && FourBits[k] == '1' ) num += 2;
        else if(k == 3 && FourBits[k] == '1' ) num += 1;
        //else num += 0;
    }
    hex = Ito16(num);
    return hex;
}
