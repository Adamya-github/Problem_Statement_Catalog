// main.cpp
#include <bits/stdc++.h>
#include "json.hpp"
using namespace std;
using json = nlohmann::json;

// --- String Big Integer helpers (non-negative integers in decimal) ---

string stripLeading(const string &s) {
    size_t i = 0;
    while (i < s.size() && s[i] == '0') ++i;
    if (i == s.size()) return "0";
    return s.substr(i);
}

string addStr(const string &a, const string &b) {
    int i = (int)a.size() - 1, j = (int)b.size() - 1, carry = 0;
    string res;
    while (i >= 0 || j >= 0 || carry) {
        int da = (i >= 0 ? a[i] - '0' : 0);
        int db = (j >= 0 ? b[j] - '0' : 0);
        int s = da + db + carry;
        res.push_back(char('0' + (s % 10)));
        carry = s / 10;
        --i; --j;
    }
    reverse(res.begin(), res.end());
    return stripLeading(res);
}

string mulStrSmall(const string &a, long long m) {
    if (m == 0) return "0";
    if (m == 1) return stripLeading(a);
    int n = a.size();
    long long carry = 0;
    string res;
    for (int i = n - 1; i >= 0; --i) {
        long long prod = (long long)(a[i] - '0') * m + carry;
        res.push_back(char('0' + (prod % 10)));
        carry = prod / 10;
    }
    while (carry) {
        res.push_back(char('0' + (carry % 10)));
        carry /= 10;
    }
    reverse(res.begin(), res.end());
    return stripLeading(res);
}

string mulStr(const string &a, const string &b) {
    string A = stripLeading(a), B = stripLeading(b);
    if (A == "0" || B == "0") return "0";
    int n = A.size(), m = B.size();
    vector<int> res(n + m, 0);
    for (int i = n - 1; i >= 0; --i) {
        for (int j = m - 1; j >= 0; --j) {
            int idx = i + j + 1;
            int mul = (A[i] - '0') * (B[j] - '0');
            res[idx] += mul;
        }
    }
    for (int i = n + m - 1; i > 0; --i) {
        res[i - 1] += res[i] / 10;
        res[i] %= 10;
    }
    string s;
    for (int d : res) s.push_back('0' + d);
    return stripLeading(s);
}

// Convert single character to integer
int digitVal(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'z') return 10 + (c - 'a');
    if (c >= 'A' && c <= 'Z') return 10 + (c - 'A');
    return -1;
}

// Convert string in base to decimal string
string baseToDecimalStr(const string &num, int base) {
    string val = "0";
    for (char c : num) {
        if (c == '_') continue;
        int d = digitVal(c);
        if (d < 0 || d >= base) {
            cerr << "Invalid digit '" << c << "' for base " << base << "\n";
            exit(1);
        }
        val = mulStrSmall(val, base);
        if (d != 0) val = addStr(val, to_string(d));
    }
    return stripLeading(val);
}

// --- Main Program ---
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    ifstream in("input.json");
    if (!in.is_open()) {
        cerr << "Error: cannot open input.json\n";
        return 1;
    }

    json j;
    try { in >> j; } 
    catch (const std::exception &e) {
        cerr << "Error parsing JSON: " << e.what() << "\n";
        return 1;
    }

    if (!j.contains("keys") || !j["keys"].contains("n") || !j["keys"].contains("k")) {
        cerr << "JSON missing keys.n or keys.k\n";
        return 1;
    }

    int n = j["keys"]["n"].get<int>();
    int k = j["keys"]["k"].get<int>();
    vector<string> rootsDec; // decimal strings
    vector<bool> isNegative;

    for (int i = 1; i <= n; ++i) {
        string idx = to_string(i);
        if (!j.contains(idx)) continue;
        if (!j[idx].contains("base") || !j[idx].contains("value")) continue;
        string baseStr = j[idx]["base"].get<string>();
        string valStr = j[idx]["value"].get<string>();
        bool neg = false;
        if (!valStr.empty() && valStr[0] == '-') { neg = true; valStr = valStr.substr(1); }
        int base = 10;
        try { base = stoi(baseStr); } catch(...) { base = 10; }
        string dec = baseToDecimalStr(valStr, base);
        rootsDec.push_back(dec);
        isNegative.push_back(neg);
    }

    if ((int)rootsDec.size() < k) k = (int)rootsDec.size(); // use available roots
    if ((int)rootsDec.size() > k) {
        rootsDec.resize(k);
        isNegative.resize(k);
    }

    // Multiply all roots
    string product = "1";
    int negCount = 0;
    for (int i = 0; i < k; ++i) {
        product = mulStr(product, rootsDec[i]);
        if (isNegative[i]) negCount++;
    }

    if (product == "0") { cout << "0\n"; return 0; }

    if (negCount % 2 != 0) cout << "-";
    cout << product << "\n";

    return 0;
}
