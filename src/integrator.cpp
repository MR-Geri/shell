#include <string>
#include <vector>
#include <stack>
#include <queue>
#include <iostream>
#include <stdio.h>
#include <limits.h>
#include <unistd.h>
#include <sstream>

#include "connectors.h"
#include "commands.h"
#include "integrator.h"
#include "parser.h"

#define GetCurrentDir getcwd


using namespace std;

vector<string> REDIRECTIONS = {"|"};


parenLocation findParens(vector <preConnector> given, int parenNumber){
    int first = -1;
    int last = -1;

    for(int i = 0; i < given.size(); i++){
        if(given.at(i).shade.parent == parenNumber){
            if(first == -1){
                first = i;
            }
            last = i;
        }
    }

    parenLocation returnval = parenLocation();
    returnval.first = first;
    returnval.last = last;
    return returnval;
}

Connector* makeConnector(string type, Command* com, Connector* next) {
    if (type == ";") {
        return new AnyConnector(next,com);
    }
    if (type == "&&") {
        return new PassConnector(next,com);
    }
    if (type == "||") {
        return new FailConnector(next,com);
    }
    return new AnyConnector(next, com);

}

vector<int> findChar(string givenStr, char findChar){
    vector<int> results;
    for(int i = 0; i < givenStr.size(); i++){
        if(givenStr[i] == findChar){
            results.push_back(i);
        }
    }
    return results;
}

vector<parenShading> constructShading(vector<preConnector> preConnectors){
    vector<int> lefts;
    vector<int> rights;
    vector<int> uniqueValues;

    vector<parenShading> returnShading;

    for(int i = 0;i < preConnectors.size(); i++){
        if(preConnectors.at(i).command == ")"){
            rights.push_back(i+1);
        } else if(preConnectors.at(i).command == "("){
            lefts.push_back(i+1);
        }
    }
    if(lefts.size() != rights.size()){
        parenShading failed = parenShading();
        failed.parent = -1;
        failed.priority = -1;
        returnShading.push_back(failed);
        return returnShading;
        }

    if(lefts.size() == 0){
        for(auto i: preConnectors){
            parenShading shade = parenShading();
            shade.parent = 0;
            shade.priority = 0;
            returnShading.push_back(shade);
        }
        return returnShading;
    }

    for(int i = 0; i < rights.at(rights.size()-1);i++){
        uniqueValues.push_back(0);
    }
    for(int i = 0; i < lefts.size(); i++){
        uniqueValues.at(lefts.at(i)) = i+1;
    }


    stack<int> stacc;
    int currentShading = 0;
    int depth = 0;
    for(int i = 0; i < preConnectors.size(); i++){
        parenShading shade = parenShading();
        if(lefts.size() > 0 and lefts.at(0) == i){
            if(currentShading != 0){
                stacc.push(currentShading);
            }
            depth++;
            currentShading = lefts.at(0);

            lefts.erase(lefts.begin(), lefts.begin()+1);
        }else if(rights.size() > 0 and rights.at(0) == i){
            if(stacc.empty()){
                depth = 0;
                currentShading = 0;
            }else{
                depth--;
                currentShading = stacc.top();
                stacc.pop();

            }
            rights.erase(rights.begin(), rights.begin()+1);
        }
        shade.parent = currentShading;
        shade.priority = depth;
        returnShading.push_back(shade);
    }

    for(int i = 0; i < returnShading.size(); i++){
        returnShading.at(i).parent = uniqueValues.at(returnShading.at(i).parent);
    }

    for(int i = 0; i < returnShading.size(); i++){
        if(preConnectors.at(i).command == "("){
            returnShading.at(i) = returnShading.at(i+1);
        }
    }

    return returnShading;
}

vector<int> findString(string base, string find){
    vector<int> returnVec;
    int foundPosition = base.find(find);
    while(foundPosition != string::npos)
    {
        returnVec.push_back(foundPosition);
        foundPosition = base.find(find,foundPosition+1);
    }
    return returnVec;
}

int findOneString(string base, string find){
    vector<int> founds = findString(base, find);
    switch(founds.size()){
        case 0:
            return 0;
        case 1:
            return founds.at(0);
        default:
            return founds.at(0); //How can there be two of the same redirectors in a command?
    }
}

bool foundString(string base, string find){
    vector<int> founds = findString(base, find);
    switch(founds.size()){
        case 0:
            return false;
            break;
        default:
            return true;
    }
}

queue<int> findStrings(string base, vector<string> finds){
    vector<vector<int> > redirectorLocations;
    vector<int> finalRedirectLocations;
    vector<int> setRedirectLocations;

    for(int i = 0; i < finds.size(); i++){
        redirectorLocations.push_back(findString(base, finds.at(i)));
    }
    for(int i = 0; i < redirectorLocations.size(); i++){
        for(int j = 0; j < redirectorLocations.at(i).size(); j++){
            int found = redirectorLocations.at(i).at(j);
            finalRedirectLocations.push_back(found);
        }
    }

    for(int i = 0; i < finalRedirectLocations.size(); i++){
        bool unique = true;
        for(int j = 0; j < setRedirectLocations.size(); j++){
            if(finalRedirectLocations.at(i) == finalRedirectLocations.at(j)){
                unique = false;
            }

        }
        if(unique){setRedirectLocations.push_back(finalRedirectLocations.at(i));};
    }
    sort(setRedirectLocations.begin(), setRedirectLocations.end());

    queue<int> returnQ;
    for(auto it : setRedirectLocations){
        returnQ.push(it);
    }
    return returnQ;
}

vector<string> commandParser(string argument){
    vector<string> commandParser;
    queue<int> tokenLocations = findStrings(argument, REDIRECTIONS);
    int nextToken = tokenLocations.front();
    tokenLocations.pop();
    int start = 0;
    for(int i = 0; i < argument.size(); i++){
        if(i == nextToken){
            string got = argument.substr(start, nextToken-start-1);
            commandParser.push_back(got);
            i += 2;
            if(tokenLocations.empty()){
                break;
            }
            nextToken = tokenLocations.front();
            tokenLocations.pop();
            start = i;

        }
    }
    if(nextToken+2 < argument.size()){
        string got = argument.substr(nextToken+2, argument.size());
        commandParser.push_back(got);
    }
    return commandParser;
}

Command* getCommand(string parsed) {
    int in_index = findOneString(parsed, "<");
    int out_index = findOneString(parsed, ">");
    int dub_out_index = findOneString(parsed, ">>");
    int flag_index = findOneString(parsed, " -");

    if(in_index > 0){  //cat < input

        string command = parsed.substr(0,in_index-1);
        command = pythonicc_replace(command, "< ", "");
        string file = parsed.substr(in_index, parsed.size()-1);
        file = pythonicc_replace(file, "< ", "");

        string flag = "";
        if(flag_index > 0){
            flag_index = findOneString(command, " -");
            flag = command.substr(flag_index, 3);
            flag = pythonicc_replace(flag, " ", "");
            command.erase(command.begin() + flag_index, command.begin() + flag_index + 3);
        }

        if(dub_out_index > 0){
            dub_out_index = findOneString(file, ">>");
            string newInFile = file.substr(0, dub_out_index-1);
            Command* in = new InRedir(command, newInFile, flag);
            string newOutFile = file.substr(dub_out_index+3, file.size());
            return new DubOutRedir(newOutFile, in);
        }
        else if (out_index > 0) { // cat < input > output
            out_index = findOneString(file, ">");
            string newInFile = file.substr(0, out_index-1);
            Command* in = new InRedir(command, newInFile, flag);
            string newOutFile = file.substr(out_index+2, file.size());
            return new OutRedir(newOutFile, in);
        }
        Command* in = new InRedir(command, file, flag);

        return in;
    }
    else if(dub_out_index > 0){
        string command = parsed.substr(0,out_index-1);
        string whole = pythonicc_replace(command, ">> ", "");
        int commandSplit = findOneString(whole, " ");
        command = whole.substr(0, commandSplit);
        string args = whole.substr(commandSplit+1, whole.size());

        string file = parsed.substr(out_index, parsed.size()-1);
        file = pythonicc_replace(file, ">> ", "");

        Command* com1 = new SysCommand(command, args);
        return new DubOutRedir(file, com1);
    }
    else if (out_index > 0){
        string command = parsed.substr(0,out_index-1);
        string whole = pythonicc_replace(command, "> ", "");
        int commandSplit = findOneString(whole, " ");
        command = whole.substr(0, commandSplit);
        string args = whole.substr(commandSplit+1, whole.size());

        string file = parsed.substr(out_index, parsed.size()-1);
        file = pythonicc_replace(file, "> ", "");

        Command* com1 = new SysCommand(command, args);
        return new OutRedir(file, com1);
    }
    else {
        char cwd[PATH_MAX];
        getcwd(cwd, sizeof(cwd));
        string command = parsed;
        string args=parsed;
        int firstSpace = parsed.find(' ');
        command.erase(firstSpace, command.size());
        args.erase(0, firstSpace+1);
        return new SysCommand(command, args);
        // istringstream ss(parsed);
        // ss >> command;
        // parsed.erase(parsed.begin(), parsed.begin() + command.length());
        // ss >> args;
        // while(ss) {
        //     ss >> temp;
        //     args = args + " " + temp;
        // }
        // if (command == "ls" && args == "") {
        //     args = cwd;
        // }
        // if (args.at(args.length()-1) == ' ')
        //     args.pop_back();

        // return new SysCommand(command, args);
    }

}


HeadConnector* integrate(vector <preConnector> bigVec) {
    reverse(bigVec.begin(),bigVec.end());

    TailConnector* tail = new TailConnector();
    string com1, argument;
    string outputfile = "";
    string outputfile_app = "";
    string inputfile = "";
    Command* command;
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));

    if (bigVec.size() == 0) { /* if there is nothing to integrate */
        HeadConnector* head = new HeadConnector(tail);
        return head;
    }

    Connector* next = tail;
    Connector* current;
    string connector;

    for (int i = 0; i < bigVec.size(); ++i) {
        com1 = bigVec.at(i).command;
        argument = bigVec.at(i).argument;

        if(i == bigVec.size()-1){
            connector = ";";
        }else{
            connector = bigVec.at(i+1).connector;
        }

        if (com1 == "ls" && argument == "") {
            argument = cwd;
            current = makeConnector(connector, (new SysCommand(com1, argument)), next);
        }
        else if(argument.find("|") != string::npos ){
          if (com1 == "exit") {
            com1 = "echo";
            argument = "100";
            current = makeConnector(connector, (new SysCommand(com1, argument)), next); /* conn2 -> next = connector */
            next = current;
            continue;
          }

          vector<string> parsed;
          argument = com1 + " " + argument;
          parsed = commandParser(argument);

          Command* base = getCommand(parsed.at(0));
          Command* currentCommand = base;
          for(int i = 1; i < parsed.size()-1; ++i) {
              // string end = parsed.at(parsed.size()-1);
              // if(end.find(">") != string::npos && (end.find(">>") == string::npos)){
              //     string ofile = end;
              //     ofile.erase(ofile.begin(), ofile.begin()+2);
              //     Command* endout = new OutRedir(ofile, currentCommand);
              //     currentCommand = endout;
              // }
              // else if(end.find(">>") != string::npos) {
              //     string dubofile = end;
              //     int index = findOneString(end,">>");
              //     end.erase(end.begin(), end.begin()+index+3);
              //     dubofile.erase(dubofile.begin()+index-1, dubofile.end());

              //     Command* pipeEdition = new PipeCommand(dubofile, currentCommand);
              //     currentCommand = pipeEdition;
              //     Command* endout = new DubOutRedir(end, currentCommand);
              //     currentCommand = endout;
              // }

              // else{
              //     Command* pipeEdition = new PipeCommand(parsed.at(i), currentCommand);
              //     currentCommand = pipeEdition;
              // }
              string current = parsed.at(i);
              currentCommand = new PipeCommand(current, currentCommand);
          }
          string lastCommand = parsed.at(parsed.size()-1);
          if(lastCommand.find(">") != string::npos or lastCommand.find(">>") != string::npos){
              string part1 = lastCommand;
              string part2 = lastCommand;
              if(lastCommand.find(">>") != string::npos){
                  int whereIs = findOneString(lastCommand, ">>");
                  part1.erase(whereIs-1, part1.size());
                  part2.erase(0, whereIs+3);
                  currentCommand = new PipeCommand(part1, currentCommand);
                  currentCommand = new DubOutRedir(part2, currentCommand);
              }else{
                  int whereIs = findOneString(lastCommand, ">");
                  part1.erase(whereIs-1, part1.size());
                  part2.erase(0, whereIs+2);
                  currentCommand = new PipeCommand(part1, currentCommand);
                  currentCommand = new DubOutRedir(part2, currentCommand);
              }
          }else{
              currentCommand = new PipeCommand(lastCommand, currentCommand);
          }

          current = makeConnector(connector, currentCommand, next);

        } else if(argument.find("<") != string::npos or argument.find(">") != string::npos or argument.find(">>") != string::npos){
            argument = com1 + " " + argument;
            Command* com = getCommand(argument);
            current = makeConnector(connector, com, next);
        } else if(com1 == "(" or com1 == ")"){
            if(com1 == ")"){
                connector = ";";
            }else if(i == bigVec.size()-1){
                connector == ";";
            }else{
                connector = bigVec.at(i+1).connector;
            }
            current = new ParenConnector(next, bigVec.at(i).parentheses, connector);
            bigVec.at(i).parentheses -> parent = current;
        } else if (com1 == "cd") {
            current = makeConnector(connector, (new CdCommand(com1, argument)), next);
        } else if (com1 == "exit") {
            current = makeConnector(connector, (new ExitCommand()), next);
        } else if (com1 == "test") {
            current = makeConnector(connector, (new TestCommand(com1, argument)), next);
        } else if (com1 != "" and com1.at(0) == '[') { // [argument]
            if (argument == "") {
                argument = com1;
                argument.erase(0,1);
                argument.pop_back();
                com1 = "test";
                current = makeConnector(connector, (new TestCommand(com1, argument)), next);
            }
            else if (com1 != "[") { // [-flag argument]
                argument = com1 + " " + argument;
                argument.erase(0,1);
                argument.pop_back();
                com1 = "test";
                current = makeConnector(connector, (new TestCommand(com1, argument)), next);
            }
            else if (com1 != ""){
                com1.replace(0,1,"test"); // [ argument ]
                argument.pop_back();
                argument.pop_back();
                current = makeConnector(connector, (new TestCommand(com1, argument)), next);
            }
        } else {
            current = makeConnector(connector, (new SysCommand(com1, argument)), next); /* conn2 -> next = connector */
        }
        // }
        next = current;
    }


    HeadConnector* head = new HeadConnector(current);
    return head;
}

void parenBuilder(vector <preConnector> & bigVec, int howManyParens){
    vector<preConnector> returnVec;

    for(int i = 1; i < howManyParens+1;i++){
        paren* first = nullptr;
        bool found = false;
        for(int j = 0; j < bigVec.size(); j++){
            if((bigVec.at(j).command == "(" or bigVec.at(j).command == ")") and bigVec.at(j).shade.parent == i){
                if(first == nullptr){
                    first = new paren();
                    bigVec.at(j).parentheses = first;
                }else{
                    bigVec.at(j).parentheses = new paren();
                    bigVec.at(j).parentheses -> prev = first;
                    first -> next = bigVec.at(j).parentheses;
                    found = true;
                }
            }
        }
        // if(! found){
        //     return returnVec;
        // }

    }

}

HeadConnector* superIntegrate(vector <preConnector> bigVec){
    vector<parenShading> shaders;
    shaders = constructShading(bigVec); //Get the location of all parans
    if(shaders.size() == 1 and shaders.at(0).priority == -1){ //Check for unmatched parans
        TailConnector* failTail = new TailConnector();
        HeadConnector* dummyHead = new HeadConnector(failTail);
        cout << "Error: Unmatched parentheses" << endl;
        return dummyHead;
    }



    if(shaders.size() != bigVec.size()){ //Shaders should be linked 1:1 with the input bigVec...
        throw __throw_logic_error;
    }

    for(int i = 0; i < bigVec.size(); i++){ //...therefore, match them with each other.
        bigVec.at(i).shade = shaders.at(i);
    }

    int parens = 0;
    int maxDepth = 0;

    vector<HeadConnector* > headVector;
    headVector.resize(parens+1, 0);

    vector<int> executionOrder;


    for(auto i : shaders){
        if(i.parent > parens){
            parens = i.parent; //How many parantheses do we have to deal with?
        }
        if(i.priority > maxDepth){
            maxDepth = i.priority; //How deep is the deepest parantheses
        }
    }

    parenBuilder(bigVec, parens);

    return integrate(bigVec);

}
