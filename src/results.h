#ifndef __RESULTS_H__
#define __RESULTS_H__


class Result{
    protected:
        bool result;
    public:
        Result(){
            this -> result = false;
        }
        Result(bool result){
            this -> result = result;
        }
        virtual int getResult(){
            //-1 indicates exit, 0 indicates failed command, 1 indicates successful command, 2 indicates the next command must always be run
            if(this -> result){
                return 1;
            }
            else{
                return 0;
            }
        }
};

class ExitResult : public Result{
    public:
        ExitResult(){}
        int getResult(){
            return -1;
        }
};

class AbsoluteTrue : public Result{
    public:
        AbsoluteTrue(){}
        int getResult(){
            return 2;
        }
};


#endif