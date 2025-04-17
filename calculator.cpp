// calculator.cpp
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

using namespace std;

// Function to check if a string is a valid double
bool isValidDouble(const string& s) {
    int i = 0, n = s.length();

    if (n == 0) return false;

    // Optional + or -
    if (s[i] == '+' || s[i] == '-') i++;

    bool hasDigits = false;
    while (i < n && isdigit(s[i])) {
        hasDigits = true;
        i++;
    }

    if (i < n && s[i] == '.') {
        i++;
        bool hasDecimalDigits = false;
        while (i < n && isdigit(s[i])) {
            hasDecimalDigits = true;
            i++;
        }
        if (!hasDigits && !hasDecimalDigits) return false;
        if (hasDigits && !hasDecimalDigits && s[n-1] == '.') return false; // e.g., "1."
    } else if (!hasDigits) {
        return false;
    }

    return i == n;
}

// Helper: Pad with zeros to equalize lengths
void padZeros(string& a, string& b) {
    int pointA = a.find('.') == string::npos ? a.length() : a.find('.');
    int pointB = b.find('.') == string::npos ? b.length() : b.find('.');

    int pre = max(pointA, pointB);
    int post = max(a.length() - pointA - (a.find('.') != string::npos),
                  b.length() - pointB - (b.find('.') != string::npos));

    if (a.find('.') == string::npos) a += ".0";
    if (b.find('.') == string::npos) b += ".0";

    while (a.find('.') < pre) a = "0" + a;
    while (b.find('.') < pre) b = "0" + b;

    while (a.length() - a.find('.') - 1 < post) a += "0";
    while (b.length() - b.find('.') - 1 < post) b += "0";
}

// Remove leading and trailing zeros
string trimZeros(const string& s) {
    string result = s;

    // Remove leading zeros
    int start = 0;
    while (start < result.size() && result[start] == '0' && result[start + 1] != '.')
        start++;
    result = result.substr(start);

    // Remove trailing zeros after decimal
    if (result.find('.') != string::npos) {
        while (result.back() == '0') result.pop_back();
        if (result.back() == '.') result += "0"; // retain .0 for whole numbers
    } else {
        result += ".0"; // append .0 to whole numbers
    }

    if (result.empty()) return "0.0";
    return result;
}

// Add two positive strings
string addStrings(const string& aRaw, const string& bRaw) {
    string a = aRaw, b = bRaw;
    padZeros(a, b);
    
    string result = "";
    int carry = 0;

    for (int i = a.length() - 1; i >= 0; --i) {
        if (a[i] == '.') {
            result += '.';
            continue;
        }

        int sum = (a[i] - '0') + (b[i] - '0') + carry;
        result += (sum % 10 + '0');
        carry = sum / 10;
    }

    if (carry) result += (carry + '0');
    reverse(result.begin(), result.end());
    return trimZeros(result);
}

// Compare absolute values of two positive normalized doubles
int compareAbs(const string& a, const string& b) {
    string aCopy = a, bCopy = b;
    padZeros(aCopy, bCopy);
    return aCopy.compare(bCopy);
}

// Subtract b from a (assuming a >= b and both are positive strings)
string subtractStrings(const string& aRaw, const string& bRaw) {
    string a = aRaw, b = bRaw;
    padZeros(a, b);
    string result = "";
    int borrow = 0;

    for (int i = a.length() - 1; i >= 0; --i) {
        if (a[i] == '.') {
            result += '.';
            continue;
        }

        int digitA = a[i] - '0' - borrow;
        int digitB = b[i] - '0';
        if (digitA < digitB) {
            digitA += 10;
            borrow = 1;
        } else {
            borrow = 0;
        }
        int diff = digitA - digitB;
        result += (diff + '0');
    }

    reverse(result.begin(), result.end());
    return trimZeros(result);
}

// Signed string addition and subtraction logic
string addSigned(const string& input, const string& offset) {
    string a = input;
    string b = offset;

    // Extract signs
    bool aNeg = false, bNeg = false;
    if (a[0] == '-') { aNeg = true; a = a.substr(1); }
    else if (a[0] == '+') { a = a.substr(1); }

    if (b[0] == '-') { bNeg = true; b = b.substr(1); }
    else if (b[0] == '+') { b = b.substr(1); }

    if (aNeg == bNeg) {
        string res = addStrings(a, b);
        return aNeg ? "-" + res : res;
    } else {
        // Determine which is larger in magnitude
        int cmp = compareAbs(a, b);
        if (cmp == 0) return "0.0";
        if (cmp > 0) {
            string res = subtractStrings(a, b);
            return aNeg ? "-" + res : res;
        } else {
            string res = subtractStrings(b, a);
            return bNeg ? "-" + res : res;
        }
    }
}

int main() {
    string filename;
    cout << "Enter input file name: ";
    cin >> filename;

    ifstream file(filename);
    if (!file) {
        cerr << "Could not open file." << endl;
        return 1;
    }

    string line;
    const string offset = "-123.456";

    while (getline(file, line)) {
        if (isValidDouble(line)) {
            cout << "Valid:   " << line << endl;
            string result = addSigned(line, offset);
            cout << "Result:  " << result << endl << "\n";
        } else {
            cout << "Invalid: " << line << endl << "\n";
        }
    }

    return 0;
}
