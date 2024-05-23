#ifndef __CONNECTORS_H__
#define __CONNECTORS_H__



#include <string>

using namespace std;

class Command;
class Result;


class Connector{
    
    protected:
        string type = "Abstract";
        Connector* next;
        Command* command;
    public:
        Connector(){};
        virtual Result* execute(Result*) = 0;
        
};


class FailConnector:public Connector{
    public:
        string type = "Fail";
        FailConnector(Connector* next, Command* command){this -> next = next; this -> command = command;};
        Result* execute(Result*);
};

class PassConnector:public Connector{
    public:
        string type = "Pass";
        PassConnector(Connector* next, Command* command){this -> next = next; this -> command = command;};
        Result* execute(Result*);
};

class AnyConnector:public Connector{
    public:
        string type = "Any";
        AnyConnector(Connector* next, Command* command){this -> next = next; this -> command = command;};
        Result* execute(Result*);
};
struct paren;
class ParenConnector:public Connector{
    public:
        paren* parentheses;
        string type = "Paren";
        string connector = "";
        ParenConnector(Connector*, paren*, string);
        Result* execute(Result*);
};


class HeadConnector:public Connector{
        
    public:
        bool noExit = true;
        string type = "Head";
        HeadConnector(Connector* next){this -> next = next;};
        HeadConnector(){};
        void setNext(Connector* next){this -> next = next;};
        Result* execute(Result* res);
        Result* execute();
        bool keepRunning(){
            return this -> noExit;
        }
};

class TailConnector:public Connector{
    public:
        string type = "Next";
        TailConnector(Connector* next){this -> next = next;};
        TailConnector(){};
        void setNext(Connector* next){this -> next = next;};
        Result* execute(Result* res){
            return res;
            //cout << res -> getResult() << endl;}; //Uncomment this if want to test
        }

};

#endif