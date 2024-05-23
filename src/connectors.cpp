#include "results.h"
#include "commands.h"
#include "connectors.h"
#include "parser.h"


using namespace std;

Result* HeadConnector::execute(Result* result){
    return this -> next -> execute(result);
}

Result* AnyConnector::execute(Result* result){
    switch(result -> getResult()){
        case -1:
            return result;
            break;
        default:
            Result* nextResult = this -> command -> execute();
            return this -> next -> execute(nextResult);

    }
    
}

Result* ExitCommand::execute(){return new ExitResult();}

Result* FailConnector::execute(Result* result){
    switch(result -> getResult()){
        case -1:
            return result;
            break;
        case 1:
            return this -> next -> execute(result);
            break;
        default:
            Result* nextResult = this -> command -> execute();
            return this -> next -> execute(nextResult);
    }
    
}

Result* PassConnector::execute(Result* result){
    switch(result -> getResult()){
        case -1:
            return result;
            break;
        case 0:
            return this -> next -> execute(result);
            break;
        default:
            Result* nextResult = this -> command -> execute();
            return this -> next -> execute(nextResult);
    }
    
}

Result* HeadConnector::execute(){
    Result* res =  this -> execute(new AbsoluteTrue());
    if(res -> getResult() == -1){
        this -> noExit = false;
    }
    return res;
};


ParenConnector::ParenConnector(Connector* next, paren* parentheses, string connector){
    this -> next = next;
    this -> parentheses = parentheses;
    this -> connector = connector;
}

Result* ParenConnector::execute(Result* result){
    if(result -> getResult() == -1){return result;};
    if(this -> connector == ""){throw __throw_logic_error;};
    if(this -> parentheses -> parent == nullptr){throw __throw_logic_error;};

    if(this -> connector == ";"){
        return this -> next -> execute(result);
    }
    else if(this -> connector == "&&"){
        switch(result -> getResult()){
            case 0:
                return this -> parentheses -> next -> parent -> execute(result);
                break;
            default:
                return this -> next -> execute(result);
        }
    }
    else if(this -> connector == "||"){
        switch(result -> getResult()){
            case 1:
                return this -> parentheses -> next -> parent -> execute(result);
                break;
            default:
                return this -> next -> execute(result);
        }
    }
}


