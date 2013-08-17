#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iterator>
#include <algorithm>

using std::string;

// Returns true if the string starts with the substring.
inline bool startswith(const string &s, const char *beg) {
    if (s.size()==0 && *beg != '\0') return false;
    for (size_t i=0; i<s.size(); ++i) {
        if (beg[i] == '\0') return true;
        if (s[i] != beg[i]) return false;
    }
    return true;
}

// Reads the next line of a file.
inline std::string read_line(std::fstream &fid) {
    std::string line;
    getline(fid, line);
    return line;
}

// Converts a string to type T.
template <typename T> inline T from_string(const std::string &s) {
    std::istringstream ss(s);
    T t;  ss >> t;
    if (ss.fail()) {
        std::cerr << "Bad string conversion: " << s
                  << ".  Terminating...\n";
        exit(1);
    }
    return t;
}

// Returns a copy of the string without trailing/preceding whitespace.
inline std::string trim(std::string s, std::string ws=" \t\n\r") {
    if (s.empty()) return s;
    size_t a=s.find_first_not_of(ws);
    size_t b = s.find_last_not_of(ws)+1;
    return s.substr(a, b-a);
}

// Splits a string like the python function.
inline std::vector<std::string> split(std::string s, std::string delims=" \t\n\r")
{
    std::vector<std::string> pieces;
    size_t begin=0, end=0;
    while (end != std::string::npos) {
        begin = s.find_first_not_of(delims, end);
        end   = s.find_first_of(delims, begin);
        if (begin != std::string::npos)
            pieces.push_back(s.substr(begin, end-begin));
    }
    return pieces;
}

// Returns a copy of the string in lowercase.
inline string lower(string s) {
    std::transform(s.begin(), s.end(), s.begin(),
            static_cast<int(*)(int)> (tolower));
    return s;
}

