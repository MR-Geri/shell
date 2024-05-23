#ifndef __INTEGRATOR_H__
#define __INTEGRATOR_H__


#include <string>
#include <vector>
#include <stack>
#include <iostream>
#include <stdio.h>
#include <limits.h>
#include <unistd.h>
#include <string>

#define GetCurrentDir getcwd


 
using namespace std;

class preConnector;

struct parenLocation{
    int first;
    int last;
};

vector<int> findStrings(string base, string find);
bool foundString(string base, string find);
HeadConnector* integrate(vector <preConnector> bigVec);
HeadConnector* superIntegrate(vector <preConnector> bigVec);

#endif