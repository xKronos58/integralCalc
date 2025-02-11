#include <iostream>
#include <sstream>
#include <stack>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <valarray>
#include <cmath>

using namespace std;
class Integration {
public:
    static string replace(string expr, char var, double val) {
        stringstream temp;
        size_t lastSS = 0;
        for (size_t i = 0; i < expr.length(); ++i) {
            if (expr[i] == var) {
                temp << expr.substr(lastSS, i - lastSS) << val;
                lastSS = i + 1;
            }
        }
        temp << expr.substr(lastSS);  // Append the remaining part of the string
        return temp.str();
    }


    static vector<string> Integrate(const string& expr, char var, double a, double b) {
        // formula int^a_b f(x) dx = {b-a}/{6}[f(a)+4f({a+b}/{2})+f(b)]
        vector<string> exprs;
        string formula = R"(\int^a_b\space f(x) dx=\frac{b-a}{6}[f(a)+4f(\frac{a+b}{2})+f(b)]\\\\)";
        string sup = R"(f(x)=\int^{)"+to_string(a)+"}_{"+to_string(b)+"}"+expr+" dx"+R"(\\\\)";
        string fa = replace(expr, var, a), fb = replace(expr, var, b), fab = replace(expr, var, (a+b)/2);
        std::cout << "fa: " << fa << ", fb: " << fb << ", fab: " << fab << std::endl;
        string step_one = R"(\int^a_bf(x)=\frac{)"+ to_string(b-a)+R"(}{6}\times[)"+fa+R"(+4\times)"+fab+fb+R"(]\\\\)";
        double evaluation = evaluate(to_string((b-a)/6) +"*("+fa+"+4*("+fab+")+"+fb+")");
        string eval = R"(\int^a_b f(x)=)" + to_string(evaluation) +R"(\\\\)";
        exprs.push_back(formula); exprs.push_back(sup); exprs.push_back(step_one); exprs.push_back(eval);

        return exprs;
    }

    static int precedence(char op) {
        if (op == '+' || op == '-') return 1;
        if (op == '*' || op == '/') return 2;
        if (op == '^') return 3; // Exponentiation has highest precedence
        return 0;
    }

// Perform arithmetic operations
    static double applyOp(double a, double b, char op) {
        switch (op) {
            case '+': return a + b;
            case '-': return a - b;
            case '*': return a * b;
            case '/': return b != 0 ? a / b : throw std::runtime_error("Division by zero");
            case '^': return pow(a, b);
        }
        return 0;
    }

    // Evaluate an expression given as a string
    static double evaluate(const std::string& expr) {
        std::stack<double> values;
        std::stack<char> ops;

        for (size_t i = 0; i < expr.length(); i++) {
            if (isspace(expr[i])) continue;

            if (isdigit(expr[i]) || (expr[i] == '-' && (i == 0 || expr[i - 1] == '('))) {
                double val = 0;
                size_t j = i;
                bool isNegative = (expr[i] == '-');
                if (isNegative) j++;
                while (j < expr.length() && (isdigit(expr[j]) || expr[j] == '.')) j++;
                val = std::stod(expr.substr(i, j - i));
                values.push(isNegative ? -val : val);
                i = j - 1;
            }
            else if (expr[i] == '(') {
                ops.push(expr[i]);
            }
            else if (expr[i] == ')') {
                while (!ops.empty() && ops.top() != '(') {
                    double b = values.top(); values.pop();
                    double a = values.top(); values.pop();
                    char op = ops.top(); ops.pop();
                    values.push(applyOp(a, b, op));
                }
                ops.pop();
            }
            else {  // Operator handling
                while (!ops.empty() && precedence(ops.top()) >= precedence(expr[i]) && expr[i] != '^') {
                    double b = values.top(); values.pop();
                    double a = values.top(); values.pop();
                    char op = ops.top(); ops.pop();
                    values.push(applyOp(a, b, op));
                }
                ops.push(expr[i]);
            }
        }

        while (!ops.empty()) {
            double b = values.top(); values.pop();
            double a = values.top(); values.pop();
            char op = ops.top(); ops.pop();
            values.push(applyOp(a, b, op));
        }

        return values.top();
    }
};

class renderer {
public:
    // Testing purposes
    static std::vector<std::string> generateExpressions(int count) {
        const char* ops[] = {"+", "-", "*", "/"};
        std::vector<std::string> expressions;
        for (int i = 0; i < count; ++i) {
            int num1 = rand() % 10 + 1;
            int num2 = rand() % 10 + 1;
            const char* op = ops[rand() % 4];
            expressions.push_back("\\(" + std::to_string(num1) + " " + op + " " + std::to_string(num2) + "\\)");
        }
        return expressions;
    }

    static void generateHTML(const std::vector<std::string>& expressions, const std::string& filename) {
        std::ofstream file(filename);
        if (!file) {
            std::cerr << "Error creating file!" << std::endl;
            return;
        }
        file << "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n";
        file << R"(   <link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/katex@0.16.21/dist/katex.min.css" integrity="sha384-zh0CIslj+VczCZtlzBcjt5ppRcsAmDnRem7ESsYwWwg3m/OaJ2l4x7YBZl9Kxxib" crossorigin="anonymous">)";
        file << R"(   <script defer src="https://cdn.jsdelivr.net/npm/katex@0.16.21/dist/katex.min.js" integrity="sha384-Rma6DA2IPUwhNxmrB/7S3Tno0YY7sFu9WSYMCuulLhIqYSGZ2gKCJWIqhBWqMQfh" crossorigin="anonymous"></script>)";
        file << R"(   <script defer src="https://cdn.jsdelivr.net/npm/katex@0.16.21/dist/contrib/auto-render.min.js" integrity="sha384-hCXGrW6PitJEwbkoStFjeJxv+fSOOQKOPbJxSfM6G5sWZjAyWhXiTIIAmQqnlLlh" crossorigin="anonymous"
            onload="renderMathInElement(document.body);"></script>)";
        file << "</head>\n<body>\n";
        file << R"(<script>
    document.addEventListener("DOMContentLoaded", function () {
        document.querySelectorAll("foreignObject span").forEach(span => {
            katex.render(span.textContent, span, { throwOnError: false });
        });
    });
</script>\n)";
        file << "<svg width=\"1000\" height=\"500\" xmlns=\"http://www.w3.org/2000/svg\">\n";

        int y = 50; // Initial vertical position
        for (const auto& expression : expressions) {
            file << "    <foreignObject x=\"20\" y=\"" << y << "\" width=\"800\" height=\"70\">";
            file << "        <div xmlns=\"http://www.w3.org/1999/xhtml\">";
            file << "            <span class=\"mathjax\">" << expression << "</span>\n";
            file << "        </div>\n";
            file << "    </foreignObject>\n";
            y += 60; // Increase spacing for next line
        }

        file << "</svg>\n</body>\n</html>";
        file.close();
    }
};

int main() {
    string expr;
    char var;
    double a, b;
    do {
        cout << "Please enter the expression:";
        getline(cin, expr);
        cout << "please enter the variable:";
        cin >> var;
        cout << "please input a:";
        cin >> a;
        cout << "please input b:";
        cin >> b;
        if (a < b) cout << "A must be greater than b";
    }
    while (expr.empty() || a < b);

    cout << expr << ", " << var << ", " << a << ", " << b << endl;

    renderer::generateHTML(Integration::Integrate(expr, var, a, b), "math_expressions.html");
    std::cout << "HTML file generated: math_expressions.html" << std::endl;

    return 0;
}

