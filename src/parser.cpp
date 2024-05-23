#include <string>
#include <vector>

#include "parser.h"

using namespace std;

bool isParen(string s){
    for(auto i : s){
        if (i == ')' or i == '('){
            return true;
        }
    }
    return false;
}

int isConnector(string s){
     vector<string> POSSIBLE_CONNECTORS{ ";", "||", "&&"} ;
     //If adding new connectors, please add to the END, and do not change the order
    if(s.back() == ';'){
        return 1;
    }

    for(int k = 0; k < POSSIBLE_CONNECTORS.size(); k++){
        if(s == POSSIBLE_CONNECTORS.at(k)){
            return k + 1;
        }
    }
    return 0;
    
}

string appendString(string baseString, string newString){
    if(baseString == ""){
        return newString;
    }else{
        return baseString + " " + newString;
    }
}

string pythonicc_replace(string & original, const string arg1, const string arg2){
    // pythonicc_replace("yeet the meat","yeet", "eat"))
    // output = "eat the meat"

    if(original.find(arg1) == string::npos){
        return original;
    }

    int size = arg1.size();
    original.replace(original.find(arg1),size,arg2);
    return original;
}

string pythonicc_replace_char(string & original, const char arg1, const string arg2){
    if(original.find(arg1) == string::npos){
        return original;
    }

    string returnString = "";
    string arg1Str;
    arg1Str.push_back(arg1);

    for(int i = 0; i < original.size(); i++){
        
        if(original.at(i) == arg1){
            for(int j = 0; j < arg2.size(); j++){
                returnString.push_back(arg2.at(j));
            }
        }else{
            returnString.push_back(original.at(i));
        }
    }


    return returnString;  
}

void pythonicc_replace_complete(string & original, const string arg1, const string arg2){
    while(original.find(arg1) != string::npos){
        pythonicc_replace(original, arg1, arg2);
    }
}

vector <preConnector> parse(string s) {


    pythonicc_replace_complete(s, "( ", "(");
    pythonicc_replace_complete(s, " )", ")");
    pythonicc_replace_complete(s, " (", "(");
    pythonicc_replace_complete(s, ") ", ")");

    s = pythonicc_replace_char(s, '(', " ( ");
    s = pythonicc_replace_char(s, ')', " ) ");
    

    istringstream ss(s);
    vector<string> tempList;
    vector <vector<string> > bigVec;
    if (s.length() == 0) { /* no string exists :( */
        vector<preConnector> empty;
        return empty;
    }
    while (ss) {
        string input;
        ss >> input;
        tempList.push_back(input);
    }


    string command = "";
    string args = "";
    string connector = "";

    bool comment = false;

    for(int i = 0; i< tempList.size(); i++){
        int connector_result = 0;
        string currentPhrase = tempList.at(i);
        if(currentPhrase == ""){
            continue;
        }
        if(currentPhrase == "(" or currentPhrase == ")"){ //If you find a paren
            if(comment == false){// ... and you are not in a comment ...
                if(command == ""){ //...and you have not found a command
                    vector<string> thisResult;
                    thisResult.push_back(currentPhrase);
                    thisResult.push_back("");
                    if(connector != ""){
                        thisResult.push_back(connector);
                    }else{
                        thisResult.push_back(";");//..make sure the integrator knows you found a paren
                    }
                    
                    bigVec.push_back(thisResult);
                    continue;
                }else{//...otherwise
                    currentPhrase = ";"; //...back up one and pretend that paran was a semicolon
                    i--;
                }
            }if(currentPhrase == "("){
                vector<string> newTemplist;
                for(int j = 0; j < tempList.size();j++){
                    if(j == i){
                        string constructNew = "(" + tempList.at(j+1);
                        newTemplist.push_back(constructNew);
                        j++;
                    }else{
                        newTemplist.push_back(tempList.at(j));
                    }
                }
                tempList = newTemplist;
                i--;
                continue;
            }else if(currentPhrase == ")"){
                args.push_back(')');
                continue;
                
            }

        }

        if(currentPhrase.front() == '"'){
            currentPhrase.erase(0, 1);
            comment = true;
        }else{ //If not a quote...
            if(comment==false){connector_result=isConnector(currentPhrase);}//...check for connectors
            if(comment == false and currentPhrase.front() == '#'){
                connector = ';';
                vector<string> thisResult; //...then you have finished the arguments, so return it
                thisResult.push_back(command);
                thisResult.push_back(args);
                thisResult.push_back(connector);
                bigVec.push_back(thisResult);
                break;
                }//... and if comment, treat as semicolon and stop parsing
        }


        if(currentPhrase.back() == '"'){//Check if end of quote
            comment = false;
            currentPhrase.pop_back();
        }

        if(command == "" and connector_result == 0){ //If vector is empty, then you have found a command that is not a connector, put it in
            command = currentPhrase;
        }
        else if(connector_result > 0){ //If you find a connector...
            if(connector_result == 1){ //...and it is a semicolon...
                if(currentPhrase.size() == 1){ //...that is freestanding
                    connector = currentPhrase; //...just add it to the connector index
                }
                else{ //...that is stuck to the end of an argument
                    args = appendString(args, currentPhrase.substr(0, currentPhrase.size()-1));
                    connector = currentPhrase.back(); //...rip it off
                }
            }
            else{//...and it is not a semicolon...
                connector = currentPhrase;//...it must be free standing
            }
            vector<string> thisResult; //...then you have finished the arguments, so return it
            thisResult.push_back(command);
            thisResult.push_back(args);
            thisResult.push_back(connector);
            bigVec.push_back(thisResult);
            command = "";
            args = "";
            connector = "";

        }else{ //If what you find is not a command nor a connector, it must be an argument, so add it to args
            args = appendString(args, currentPhrase);
        }
    }//If after looping through...

    if(connector == "" and command != ""){//...you have not found a command
        connector = ";";//...treat it as a semicolon
        vector<string> thisResult;
        thisResult.push_back(command);
        thisResult.push_back(args);
        thisResult.push_back(connector); 
        bigVec.push_back(thisResult);//...and make sure it is in the list
    }
    vector<preConnector> returnVec;
    int at_returnVec = -1;
    for(int i = 0; i < bigVec.size(); i++){
        preConnector thisItem = preConnector();
        if(isConnector(bigVec.at(i).at(2)) and bigVec.at(i).at(0) == ""){
            if(isParen(bigVec.at(i-1).at(0))){
                returnVec.at(at_returnVec).connector = bigVec.at(i).at(2);
            }else{
                throw __throw_logic_error;
            }

            
        }else{
            thisItem.command = bigVec.at(i).at(0);
            thisItem.argument = bigVec.at(i).at(1);
            thisItem.connector = bigVec.at(i).at(2);
            returnVec.push_back(thisItem);
            at_returnVec++;
        }

    }

    return returnVec;
}