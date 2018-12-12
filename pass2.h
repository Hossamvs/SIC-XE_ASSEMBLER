#ifndef PASS2_H
#define PASS2_H

#include <fstream>
#include <vector>
#include <sstream>
#include <map>
#include <iostream>
#include <iomanip>
#include <string>
#include "optable.h"
#include "utilities.h"

void generateOpcode(std::vector<std::vector<std::string>>code, std::vector<int>&objectCodeInt, std::map<std::string,std::string>opTable, int lines){

    for(int i=1;i<lines;i++){

        if(code[i].size()==1){

            if(code[i][0]=="LTORG"){
                objectCodeInt.push_back(-1);
            }else{//RSUB

                if(code[i][0][0]=='+'){
                    //format 4 handle (opcode , flags)
                    objectCodeInt.push_back(handleFormat4(code[i], code[i][0] ,opTable));

                }else{
                    //handle format 3 (opcode , flags)
                    objectCodeInt.push_back(handleFormat3(code[i], code[i][0] ,opTable));
                }
            }

        }else if(code[i].size()==2){

                    if(code[i][0][0]=='+'){

                        //format 4 handle (opcode , flags)
                        objectCodeInt.push_back(handleFormat4(code[i], code[i][0] ,opTable));

                    }else if(code[i][0]=="FIX" || code[i][0]=="FLOAT" || code[i][0]=="HIO" || code[i][0]=="NORM" || code[i][0]=="SIO" || code[i][0]=="TIO" || code[i][0]=="FIX" ){
                        //handle format 1  (opcode)
                        objectCodeInt.push_back(handleFormat1(code[i], code[i][0] ,opTable));

                    }else if(code[i][0]=="ADDR" || code[i][0]=="CLEAR" || code[i][0]=="COMPR" || code[i][0]=="DIVR" || code[i][0]=="MULR" || code[i][0]=="RMO" || code[i][0]=="SHIFTL" || code[i][0]=="SHIFTR" || code[i][0]=="SUBR" || code[i][0]=="SVC" || code[i][0]=="TIXR"){
                         //handle format 2 (opcode , registers)
                         objectCodeInt.push_back(handleFormat2(code[i], code[i][0] ,opTable));

                    }else if(code[i][0]=="BASE"){
                        objectCodeInt.push_back(-1);

                    }else{
                         //handle format 3 (opcode , flags)
                         objectCodeInt.push_back(handleFormat3(code[i], code[i][0] ,opTable));

                    }

        }else if(code[i].size()==3){

                    if(code[i][1][0]=='+'){

                        //format 4 handle (opcode , flags)
                        objectCodeInt.push_back(handleFormat4(code[i], code[i][1] ,opTable));

                    }else if(code[i][1]=="FIX" || code[i][1]=="FLOAT" || code[i][1]=="HIO" || code[i][1]=="NORM" || code[i][1]=="SIO" || code[i][1]=="TIO" || code[i][1]=="FIX" ){
                        //handle format 1  (opcode)
                        objectCodeInt.push_back(handleFormat1(code[i], code[i][1] ,opTable));

                    }else if(code[i][1]=="ADDR" || code[i][1]=="CLEAR" || code[i][1]=="COMPR" || code[i][1]=="DIVR" || code[i][1]=="MULR" || code[i][1]=="RMO" || code[i][1]=="SHIFTL" || code[i][1]=="SHIFTR" || code[i][1]=="SUBR" || code[i][1]=="SVC" || code[i][1]=="TIXR"){
                         //handle format 2 (opcode , registers)
                         objectCodeInt.push_back(handleFormat2(code[i], code[i][1] ,opTable));

                    }else if(code[i][1]=="EQU"){//EQU

                        objectCodeInt.push_back(-1);

                    }else if(code[i][1]=="RESW" || code[i][1]=="RESB"){

                            objectCodeInt.push_back(-1);

                    }else if((code[i][2][0]=='X' || code[i][2][0]=='x')&& code[i][2].size()>=3){
                        std::string temp, declaredValue;

                        int stringLength = code[i][2].substr(2,code[i][2].size()-3).size();
                        declaredValue = code[i][2].substr(2,code[i][2].size()-3);

                        bool oddflag=false;

                        if(stringLength%2){

                            stringLength--;
                            oddflag=true;
                        }


                        for(int i=0;i<stringLength;i+=2){

                            objectCodeInt.push_back(hexStringToDec(declaredValue.substr(i,2)));
                        }

                        if(oddflag){

                            objectCodeInt.push_back(hexStringToDec(declaredValue.substr(declaredValue.size()-1,1)));
                        }

                    }else if((code[i][2][0]=='C' || code[i][2][0]=='c')&& code[i][2].size()>=3){

                        std::stringstream stream;
                        std::string temp, declaredValue;

                        int stringLength = code[i][2].substr(2,code[i][2].size()-3).size();
                        declaredValue = code[i][2].substr(2,code[i][2].size()-3);

                        bool oddflag=false;
                        int offset=0;

                        if(stringLength%3){

                            offset = stringLength%3;
                            oddflag=true;
                        }


                        for(int i=0;i<stringLength - offset;i+=3){

                            stream<< (int)declaredValue.substr(i,3)[0]<<(int)declaredValue.substr(i,3)[1]<<(int)declaredValue.substr(i,3)[2];

                            int temp;
                            stream >> temp;
                            stream.str(std::string());

                            objectCodeInt.push_back(temp);
                        }

                        if(oddflag){
                            for(int i=stringLength-offset;i<stringLength;i++)
                                stream<< (int)declaredValue[i];

                            int temp;
                            stream >> temp;
                            stream.str(std::string());

                            objectCodeInt.push_back(temp);
                        }

                    }else{
                         //handle format 3 (opcode , flags)
                         objectCodeInt.push_back(handleFormat3(code[i], code[i][1] ,opTable));

                    }

        }
    }
}

void generateAddresses(std::vector<std::vector<std::string>>code, std::vector<std::string>&objectCode,std::vector<int>objectCodeInt, std::map<std::string,string> symbolTable,std::vector<std::vector<std::string>>literalTable,std::vector<int> location, int lines){


    for(int i=0;i<objectCodeInt.size();i++){

        if(code[i].size()==1){

            if(code[i][0]=="LTORG"){
                objectCode.push_back("-");
            }else{//RSUB
                if(code[i][0][0]=='+'){
                    objectCode.push_back(intToHexString(objectCodeInt[i]<<20));
                }else{
                    objectCode.push_back(intToHexString(objectCodeInt[i]<<12));
                }
            }

        }else if(code[i].size()==2){

                    if(code[i][0][0]=='+'){
                        if(code[i][1][0]=='='){
                            int temp = objectCodeInt[i];
                            temp = temp <<20;
                            temp |= findInLiteralTable(code[i][1],literalTable);
                            objectCode.push_back(intToHexString(temp));
                        }else{
                            int temp = objectCodeInt[i];
                            temp = temp<<20;
                            std::string temp3 = (symbolTable.find(code[i][1])->second);
                            int temp2 = hexStringToDec(temp3);
                            temp = temp |temp2;
                            objectCode.push_back(intToHexString(temp));
                        }

                    }else if(code[i][0]=="FIX" || code[i][0]=="FLOAT" || code[i][0]=="HIO" || code[i][0]=="NORM" || code[i][0]=="SIO" || code[i][0]=="TIO" || code[i][0]=="FIX" ){
                        //handle format 1  (opcode)
                        objectCode.push_back(intToHexString(objectCodeInt[i]));

                    }else if(code[i][0]=="ADDR" || code[i][0]=="CLEAR" || code[i][0]=="COMPR" || code[i][0]=="DIVR" || code[i][0]=="MULR" || code[i][0]=="RMO" || code[i][0]=="SHIFTL" || code[i][0]=="SHIFTR" || code[i][0]=="SUBR" || code[i][0]=="SVC" || code[i][0]=="TIXR"){
                         //handle format 2 (opcode , registers)
                         objectCode.push_back(intToHexString(objectCodeInt[i]));

                    }else if(code[i][0]=="BASE"){
                        objectCode.push_back("-");
                    }else{
                         //handle format 3 (opcode , flags)
                        if(code[i][1][0]=='='){
                            int temp = objectCodeInt[i];
                            temp = temp <<12;
                            temp |=((findInLiteralTable(code[i][1],literalTable))-location[i+1]);
                            objectCode.push_back(intToHexString(temp));
                        }else{
                            int temp = objectCodeInt[i];
                            temp = temp<<12;
                            temp |= hexStringToDec(symbolTable.find(code[i][1])->second)-location[i+1];
                            objectCode.push_back(intToHexString(temp));
                        }
                    }

        }else if(code[i].size()==3){

                    if(code[i][1][0]=='+'){
                        if(code[i][2][0]=='='){
                            int temp = objectCodeInt[i];
                            temp = temp <<20;
                            temp |= findInLiteralTable(code[i][2],literalTable);
                            objectCode.push_back(intToHexString(temp));
                        }else{
                            int temp = objectCodeInt[i];
                            temp = temp<<20;
                            temp |= hexStringToDec(symbolTable.find(code[i][2])->second);
                            objectCode.push_back(intToHexString(temp));
                        }
                    }else if(code[i][1]=="FIX" || code[i][1]=="FLOAT" || code[i][1]=="HIO" || code[i][1]=="NORM" || code[i][1]=="SIO" || code[i][1]=="TIO" || code[i][1]=="FIX" ){
                        //handle format 1  (opcode)
                        objectCode.push_back(intToHexString(objectCodeInt[i]));

                    }else if(code[i][1]=="ADDR" || code[i][1]=="CLEAR" || code[i][1]=="COMPR" || code[i][1]=="DIVR" || code[i][1]=="MULR" || code[i][1]=="RMO" || code[i][1]=="SHIFTL" || code[i][1]=="SHIFTR" || code[i][1]=="SUBR" || code[i][1]=="SVC" || code[i][1]=="TIXR"){
                         //handle format 2 (opcode , registers)
                         objectCode.push_back(intToHexString(objectCodeInt[i]));

                    }else if(code[i][1]=="EQU"){//EQU

                        objectCode.push_back("-");

                    }else if(code[i][1]=="RESW" || code[i][1]=="RESB"){

                            objectCode.push_back("-");

                    }else if((code[i][2][0]=='X' || code[i][2][0]=='x')&& code[i][2].size()>=3){
                        int stringLength = code[i][2].substr(2,code[i][2].size()-3).size();
                        bool oddflag=false;

                        if(stringLength%2){
                            stringLength--;
                            oddflag=true;
                        }

                        for(int j=0;j<stringLength;j+=2){
                            objectCode.push_back(intToHexString(objectCodeInt[i++]));
                        }

                        if(oddflag){

                            objectCode.push_back(intToHexString(objectCodeInt[i]));
                        }

                    }else if((code[i][2][0]=='C' || code[i][2][0]=='c')&& code[i][2].size()>=3){

                        std::stringstream stream;
                        std::string temp, declaredValue;

                        int stringLength = code[i][2].substr(2,code[i][2].size()-3).size();
                        declaredValue = code[i][2].substr(2,code[i][2].size()-3);

                        bool oddflag=false;
                        int offset=0;

                        if(stringLength%3){

                            offset = stringLength%3;
                            oddflag=true;
                        }


                        for(int j=0;j<stringLength - offset;j+=3){
                            objectCode.push_back(intToHexString(objectCodeInt[i++]));
                        }

                        if(oddflag){
                            for(int j=stringLength-offset;j<stringLength;j++)
                                objectCode.push_back(intToHexString(objectCodeInt[i++]));
                        }

                    }else{
                         //handle format 3 (opcode , flags)
                        if(code[i][2][0]=='='){
                            int temp = objectCodeInt[i];
                            temp = temp <<12;
                            temp |=((findInLiteralTable(code[i][2],literalTable))-location[i+1]);
                            objectCode.push_back(intToHexString(temp));
                        }else{
                            int temp = objectCodeInt[i];
                            temp = temp<<12;
                            temp |= (hexStringToDec(symbolTable.find(code[i][2])->second)-location[i+1]);
                            objectCode.push_back(intToHexString(temp));
                        }

                    }
        }
    }
}
#endif