#ifndef __PARSERS_H__
#define __PARSERS_H__

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>


using namespace std;

// ; = 1
// || = 2
// && = 3

class Connector;

class paren{
    public:
        paren* prev = nullptr;
        paren* next = nullptr;
        Connector* parent = nullptr;
};

struct parenShading{
    int parent;
    int priority;
};

struct preConnector{
    string command = "";
    string argument = "";
    string connector = "";
    parenShading shade;
    paren* parentheses = nullptr;
};

string pythonicc_replace(string & original, const string replaceThis, const string replaceWith);
vector <preConnector> parse(string s);

#endif