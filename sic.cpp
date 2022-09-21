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
    int table = 0;
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

class LexicalAnalysis{
    public:
    int line = 0, j = 0,count =0 ; //line ,loc, count第一二行loc一樣
    int CX = 0;
    string  sloc = "", LOC ="";
    bool END = false;
    bool TYPEX = false;
    fstream outFile;
    vector <Tabledata> tData1, tData2, tData3, tData4, fData ;
    vector <output> labelTable;
    vector <output> outputData;
    Tabledata tData5[100],tData6[100],tData7[100]; // symbol,integer,string
    vector<string> cmtC;
    string standard;
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
    bool syntaxCheck(vector <Tabledata>, int );
};
// ==================== MAIN() ====================
int main(void) {
    LexicalAnalysis LAItem;
    int command = 0;
    do {
        cout << endl << "************     HW1     ************";
        cout << endl << "* 0. QUIT                           *";
        cout << endl << "* 1. LexicalAnalysis                *";
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
    cmtC.clear();
    line = 0;
    j = 0;
    count =0 ; //line ,loc, count第一二行loc一樣
    CX = 0;
    END = false;
    TYPEX = false;
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
    Tabledata temp;
    do {
        cout << endl << "Input a file name ( input...( [0]:quit )): ";
        cin >> fileName;
        if (!fileName.compare("0")) // No file to open, so quit
            return false;
        File.open(("SIC_" + fileName + ".txt").c_str(), fstream::in); // open a txt file
        if (File.is_open())
            break; // successfully open a file, so leave the loop
        else {
            cout << endl << "******* SIC_" + fileName + ".txt  IS NOT EXIST!! *******" << endl ;
        } // else
    } while (true); //end while1

    if (File.is_open()) {
        outFile.open("SIC_output.txt", fstream::out);
        outFile << "Line    Loc    Source statement        Object code" << endl;
        cout << "Line    Loc    Source statement        Object code" << endl;
        while ( true ) {
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
                if(ch == ' ' || ch == '\n' || ch == '\t' || ch == '\r') {
                    inTable = FindTable(ins);
                    if(inTable == false) BuildTable(ins);
                    ins = "";
                    if(ch == '\n'  ){ //換行及寫入資料
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
            if(File.eof()) break;
        } //end while
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

void LexicalAnalysis::assembler( string dataline ){
    count ++;
    output tempO; //暫存每一行的結果 之後pushback到outputData
    bool start = false, first = false, Hasins = false;
    output labelTemp;
    vector <Tabledata> temp;
    string tmpLoc = "", strtmp= "", sloctemp = "", preLoc = "";
    int stmp= 0;
    if(dataline != "" && !END ){
        while( j < fData.size() ){
            if(!start){
                for(int k = 0 ; k <fData[j].instruction.length() ;k++){ //小寫轉大寫
                    if( fData[j].instruction[k] >= 'a' && fData[j].instruction[k] <= 'z' ) fData[j].instruction[k] = fData[j].instruction[k] - 32;
                }
                temp.push_back(fData[j]); // 如果是start後面數字不存到temp
            }
            if(fData[j].instruction == "START"){
                sloc = fData[j+1].instruction; //設定起始位置
                LOC = fData[j+1].instruction; //設定起始位置 不更動！
                start = true;
            }
            j++ ;
            first = true;
        } //while
        if(count >3){
            tempO.line = line;
            preLoc = sloc;
            if(TYPEX) {
                sloc = Hex(preLoc,1,preLoc.length()-1);
                TYPEX = false;
            }
            else {
                sloc = Hex(sloc,3,sloc.length()-1);
            }
            tempO.loc = sloc;
            
        }
        else{
            tempO.line = line; // line & loc 一起處理 其他變數下面處理
            tempO.loc = LOC;
        }
        
        for (int i = 0; i < temp.size() ; i++) {
            if((temp[i].table == 1 || temp[i].table == 2) && (temp[0].table != 5 ) && !(temp[0].instruction == "." && temp[3].table == 5)){
                tempO.sourceStatement.push_back("   "); //非label
            }
            tempO.sourceStatement.push_back(temp[i].instruction);
            if( temp[i].table == 2 ){ // table2
                if(temp[i].instruction == "BYTE"){
                    string SStemp = "" ;
                    for(i = i+1 ; i < temp.size(); i++){
                        if(temp[i+1].CorX == 1)SStemp ="C";
                        else if (temp[i+1].CorX == 2)SStemp ="X" ;
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
                    if(strtmp.length()<4)strtmp = "0"+strtmp; //0FFD
                    for(int k = strtmp.length()-1 ; k >=0; k--){
                        stmp = T016(strtmp[k]); //stmp (int)
                        sloc = Hex(sloc, stmp, k);
                    }
                    tempO.finished = true;
                }
                else if(temp[i].instruction == "END"){
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
                } // 新增
                string m =opcode(temp, i);
                if(m != "ERROR" )tempO.machineCode += m ;
                else {
                    tempO.machineCode = "ERROR";
                    tempO.finished = true;
                }
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
            else if( temp[i].table == 5 && i != 0 ){ //要去找有無是否定義的symbol
                for (int c =0 ; c < labelTable.size() ; c++){
                    if(temp[i].instruction == labelTable[c].sourceStatement[0] && !tempO.finished ){ // 已定義
                        standard = labelTable[c].loc;
                        tempO.machineCode += labelTable[c].loc;
                        tempO.finished = true;
                    }
                    else{ //未定義label
                        tempO.undefined = temp[i].instruction;
                    }
                }
            }
            else if( temp[i].instruction == "." ){ //註解
                tempO.sourceStatement.push_back(cmtC[0]); // 註解後的內容
                //tempO.sourceStatement += cmtC[0];
                cmtC.erase(cmtC.begin()); //清掉第一個註解 因為已經存完
                //tempO.finished = true;
                if(!Hasins)tmpLoc =tempO.loc;
                tempO.loc = "" ;
                if(Hasins) {
                    line += 5;
                    tempO.line = line ;
                }
                outputData.push_back(tempO);
                tempO.sourceStatement.clear() ;
                Hasins = true;
            }
            if(END) break;
        }

        outputData.push_back(tempO);
        tempO.machineCode = "";
        temp.clear();
        line += 5;
    }
    
}
string LexicalAnalysis::Ito16(int integer){
    int  remainder = 0 ;
    char c;
    string ans , finalans;
    if(integer == 0 ) finalans = "0"; // 新增！
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
    for(int i = hex.length()-1 ; i >= 0; i-- ){
        if( i == w ){
            temp = T016(hex[i]);
            hextemp = temp +p ;
        }
        else{
            hextemp = T016(hex[i]);
            //hextemp = hex[i] - '0';
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
    
    for(int i = 3 ; i >=0 ; i -- ){
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
    for (int i =0 ; i < outputData.size() ; i++){
        if(!outputData[i].finished){ //未完成machine code
            for (int c =0 ; c < labelTable.size() ; c++){
                if(outputData[i].undefined == labelTable[c].sourceStatement[0]  && outputData[i].machineCode !="ERROR" ){
                    outputData[i].machineCode += labelTable[c].loc;
                    outputData[i].finished = true;
                }
            }
        }
        //cout<<outputData[i].sourceStatement.size() <<" %%%%%%%"<<endl;
        if(outputData[i].line >= 5 ){
            //int k =outputData[outputData.size()-1].line;
            cout << setw(3) << outputData[i].line<< "  ";
            outFile << setw(3) << outputData[i].line<< "  ";
        }
        cout <<  setw(4) <<outputData[i].loc << "  ";
        outFile <<  setw(4) <<outputData[i].loc<< "  ";
        for(int k =0 ; k < outputData[i].sourceStatement.size() ; k ++ ){
            if(outputData[i].sourceStatement[k+1] == "," || outputData[i].sourceStatement[k] == ","){
                outFile << setiosflags(ios::left) <<outputData[i].sourceStatement[k] ;
                cout << setiosflags(ios::left)<< setiosflags(ios::left) << outputData[i].sourceStatement[k];
            }
            else{
                outFile << setiosflags(ios::left) <<setw(6) <<outputData[i].sourceStatement[k] <<"    ";
                cout << setiosflags(ios::left)<< setiosflags(ios::left) << setw(6)<<outputData[i].sourceStatement[k] <<"    ";
            }
        }
        cout <<outputData[i].machineCode <<endl;
        outFile <<outputData[i].machineCode <<endl;
    }
}

string LexicalAnalysis::opcode(vector <Tabledata> temp, int i){ // 順便做sytax analysis
    bool isERROR = false;
    for(int c = 0 ; c < temp[i].instruction.length();c++){ //大寫轉小寫
       if( temp[i].instruction[c] >= 'A' && temp[i].instruction[c] <= 'Z' ) temp[i].instruction[c] = temp[i].instruction[c] + 32;
    }
    string op = "";
    if(temp[i].instruction == "add"){
        isERROR = syntaxCheck(temp, i);
        op = "18";
    }
//    else if(temp[i].instruction == "addf"){
//        op = "58";
//    }
//    if(temp[i].instruction == "addr"){
//        op = "90";
//    }
    else if(temp[i].instruction == "and"){
        isERROR = syntaxCheck(temp, i);
        op = "40";
    }
//    if(temp[i].instruction == "clear"){
//        op = "B4";
//    }
    else if(temp[i].instruction == "comp"){
        isERROR = syntaxCheck(temp, i);
        op = "28";
    }
//    if(temp[i].instruction == "compf"){
//        op = "88";
//    }
//    if(temp[i].instruction == "compr"){
//        op = "A0";
//    }
    else if(temp[i].instruction == "div"){
        isERROR = syntaxCheck(temp, i);
        op = "24";
    }
//    else if(temp[i].instruction == "divf"){
//        op = "64";
//    }
//    if(temp[i].instruction == "divr"){
//        op = "9C";
//    }
//    if(temp[i].instruction == "fix"){
//        op = "C4";
//    }
//    if(temp[i].instruction == "float"){
//        op = "C0";
//    }
//    if(temp[i].instruction == "hio"){
//        op = "F4";
//    }
    else if(temp[i].instruction == "j"){
        isERROR = syntaxCheck(temp, i);
        op = "3C";
    }
    else if(temp[i].instruction == "jeq"){
        isERROR = syntaxCheck(temp, i);
        op = "30";
    }
    else if(temp[i].instruction == "jet"){
        isERROR = syntaxCheck(temp, i);
        op = "34";
    }
    else if(temp[i].instruction == "jlt"){
        isERROR = syntaxCheck(temp, i);
        op = "38";
    }
    else if(temp[i].instruction == "jsub"){
        isERROR = syntaxCheck(temp, i);
        op = "48";
    }
    else if(temp[i].instruction == "lda"){
        isERROR = syntaxCheck(temp, i);
        op = "00";
    }
//    if(temp[i].instruction == "ldb"){
//        op = "68";
//    }
    else if(temp[i].instruction == "ldch"){
        isERROR = syntaxCheck(temp, i);
        op = "50";
    }
//    if(temp[i].instruction == "ldf"){
//        op = "70";
//    }
    else if(temp[i].instruction == "ldl"){
        isERROR = syntaxCheck(temp, i);
        op = "08";
    }
//    else if(temp[i].instruction == "lds"){
//        isERROR = syntaxCheck(temp, i);
//        op = "6C";
//    }
//    if(temp[i].instruction == "ldt"){
//        op = "74";
//    }
    else if(temp[i].instruction == "ldx"){
        isERROR = syntaxCheck(temp, i);
        op = "04";
    }
//    if(temp[i].instruction == "lps"){
//        op = "D0";
//    }
    else if(temp[i].instruction == "mul"){
        isERROR = syntaxCheck(temp, i);
        op = "20";
    }
//    if(temp[i].instruction == "mulf"){
//        op = "60";
//    }
//    if(temp[i].instruction == "mulr"){
//        op = "98";
//    }
//    if(temp[i].instruction == "norm"){
//        op = "C8";
//    }
    else if(temp[i].instruction == "or"){
        isERROR = syntaxCheck(temp, i);
        op = "44";
    }
    else if(temp[i].instruction == "rd"){
        isERROR = syntaxCheck(temp, i);
        op = "D8";
    }
//    if(temp[i].instruction == "rmo"){
//        op = "AC";
//    }
    if(temp[i].instruction == "rsub"){
        op = "4C";
    }
//    if(temp[i].instruction == "shiftl"){
//        op = "A4";
//    }
//    if(temp[i].instruction == "shiftr"){
//        op = "A8";
//    }
//    if(temp[i].instruction == "sio"){
//        op = "F0";
//    }
//    if(temp[i].instruction == "ssk"){
//        op = "EC";
    //}
    else if(temp[i].instruction == "sta"){
        isERROR = syntaxCheck(temp, i);
        op = "0C";
    }
//    if(temp[i].instruction == "stb"){
//        op = "78";
//    }
    else if(temp[i].instruction == "stch"){
        isERROR = syntaxCheck(temp, i);
        op = "54";
    }
//    if(temp[i].instruction == "stf"){
//        op = "80";
//    }
//    if(temp[i].instruction == "sti"){
//        op = "D4";
//    }
    else if(temp[i].instruction == "stl"){
        isERROR = syntaxCheck(temp, i);
        op = "14";
    }
//    else if(temp[i].instruction == "sts"){
//        op = "7C";
//    }
    else if(temp[i].instruction == "stsw"){
        isERROR = syntaxCheck(temp, i);
        op = "E8";
    }
//    if(temp[i].instruction == "stt"){
//        op = "84";
//    }
    else if(temp[i].instruction == "stx"){
        isERROR = syntaxCheck(temp, i);
        op = "10";
    }
    else if(temp[i].instruction == "sub"){
        isERROR = syntaxCheck(temp, i);
        op = "1C";
    }
//    if(temp[i].instruction == "subf"){
//        op = "5C";
//    }
//    if(temp[i].instruction == "subfr"){
//        op = "94";
//    }
//    if(temp[i].instruction == "svc"){
//        op = "B0";
//    }
    else if(temp[i].instruction == "td"){
        isERROR = syntaxCheck(temp, i);
        op = "E0";
    }
//    if(temp[i].instruction == "tio"){
//        op = "F8";
//    }
    else if(temp[i].instruction == "tix"){
        isERROR = syntaxCheck(temp, i);
        op = "2C";
    }
//    if(temp[i].instruction == "tixr"){
//        op = "B8";
//    }
    else if(temp[i].instruction == "wd"){
        isERROR = syntaxCheck(temp, i);
        op = "DC";
    }
    //else isERROR = true;
    if( isERROR ) op = "ERROR";
    return op ;
}
bool LexicalAnalysis::syntaxCheck( vector <Tabledata> temp, int i){
    bool isERROR = false;
    int m =temp.size()- i;
    if(m == 1 ){
        if(temp[i].instruction != "rsub" ) {
            isERROR = true;
        }
        return isERROR ;
    }
    else if(m == 2) { // <symbol>
        if(temp[i+1].table != 5 ) {
            isERROR = true;
        }
        return isERROR ;
        
    }
    else if(m == 4) { //BUFFER X
        if(temp[i+1].table != 5 || temp[i+2].instruction != "," || temp[i+3].instruction != "X") {
            cout <<temp[i+3].instruction <<endl;
            isERROR = true;
        }
        return isERROR ;
    }
    else return true;
        
    return isERROR ;
}

