/**
 * 数学計算サンプル
 * 標準ライブラリを使用して方程式の解を求めます
 * 
 * ビルド: gcc -o math_app.exe main.c -I../../include -lm
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

// ============================================
// 1次方程式: ax + b = 0
// ============================================
void solve_linear(double a, double b) {
    printf("\n=== 1次方程式: %.2fx + %.2f = 0 ===\n", a, b);
    
    if (fabs(a) < DBL_EPSILON) {
        if (fabs(b) < DBL_EPSILON) {
            printf("解: すべての実数（不定）\n");
        } else {
            printf("解: なし（不能）\n");
        }
    } else {
        double x = -b / a;
        printf("解: x = %.6f\n", x);
    }
}

// ============================================
// 2次方程式: ax² + bx + c = 0
// ============================================
void solve_quadratic(double a, double b, double c) {
    printf("\n=== 2次方程式: %.2fx² + %.2fx + %.2f = 0 ===\n", a, b, c);
    
    if (fabs(a) < DBL_EPSILON) {
        printf("（1次方程式として解きます）\n");
        solve_linear(b, c);
        return;
    }
    
    double discriminant = b * b - 4 * a * c;
    printf("判別式 D = %.6f\n", discriminant);
    
    if (discriminant > 0) {
        double x1 = (-b + sqrt(discriminant)) / (2 * a);
        double x2 = (-b - sqrt(discriminant)) / (2 * a);
        printf("解: x₁ = %.6f, x₂ = %.6f （異なる2つの実数解）\n", x1, x2);
    } else if (fabs(discriminant) < DBL_EPSILON) {
        double x = -b / (2 * a);
        printf("解: x = %.6f （重解）\n", x);
    } else {
        double real = -b / (2 * a);
        double imag = sqrt(-discriminant) / (2 * a);
        printf("解: x = %.6f ± %.6fi （虚数解）\n", real, imag);
    }
}

// ============================================
// ニュートン法で方程式 f(x) = 0 の解を求める
// ============================================
typedef double (*Function)(double);

double newton_method(Function f, Function df, double x0, double tol, int max_iter) {
    double x = x0;
    
    for (int i = 0; i < max_iter; i++) {
        double fx = f(x);
        double dfx = df(x);
        
        if (fabs(dfx) < DBL_EPSILON) {
            printf("  警告: 導関数が0に近くなりました\n");
            return x;
        }
        
        double x_new = x - fx / dfx;
        
        printf("  反復 %d: x = %.10f, f(x) = %.10e\n", i + 1, x_new, f(x_new));
        
        if (fabs(x_new - x) < tol) {
            return x_new;
        }
        
        x = x_new;
    }
    
    printf("  警告: 収束しませんでした\n");
    return x;
}

// 例: x³ - 2x - 5 = 0
double f1(double x) { return x * x * x - 2 * x - 5; }
double df1(double x) { return 3 * x * x - 2; }

// 例: sin(x) - x/2 = 0
double f2(double x) { return sin(x) - x / 2; }
double df2(double x) { return cos(x) - 0.5; }

// 例: e^x - 3x = 0
double f3(double x) { return exp(x) - 3 * x; }
double df3(double x) { return exp(x) - 3; }

// ============================================
// 二分法で方程式 f(x) = 0 の解を求める
// ============================================
double bisection_method(Function f, double a, double b, double tol, int max_iter) {
    if (f(a) * f(b) >= 0) {
        printf("  エラー: f(a)とf(b)が異符号である必要があります\n");
        return NAN;
    }
    
    double c;
    for (int i = 0; i < max_iter; i++) {
        c = (a + b) / 2;
        double fc = f(c);
        
        printf("  反復 %d: 区間 [%.6f, %.6f], 中点 = %.10f, f(c) = %.10e\n", 
               i + 1, a, b, c, fc);
        
        if (fabs(fc) < tol || (b - a) / 2 < tol) {
            return c;
        }
        
        if (f(a) * fc < 0) {
            b = c;
        } else {
            a = c;
        }
    }
    
    return c;
}

// ============================================
// 連立方程式（2元1次）をクラメルの公式で解く
// a1*x + b1*y = c1
// a2*x + b2*y = c2
// ============================================
void solve_linear_system_2x2(double a1, double b1, double c1,
                              double a2, double b2, double c2) {
    printf("\n=== 連立方程式 ===\n");
    printf("  %.2fx + %.2fy = %.2f\n", a1, b1, c1);
    printf("  %.2fx + %.2fy = %.2f\n", a2, b2, c2);
    
    double det = a1 * b2 - a2 * b1;
    
    if (fabs(det) < DBL_EPSILON) {
        printf("解: なし、または無数にある（係数行列の行列式が0）\n");
        return;
    }
    
    double x = (c1 * b2 - c2 * b1) / det;
    double y = (a1 * c2 - a2 * c1) / det;
    
    printf("行列式 D = %.6f\n", det);
    printf("解: x = %.6f, y = %.6f\n", x, y);
}

int main(void) {
    printf("=== 数学計算サンプル ===\n");

    // ============================================
    // 1次方程式
    // ============================================
    solve_linear(3, -6);      // 3x - 6 = 0 → x = 2
    solve_linear(0, 5);       // 0x + 5 = 0 → 不能
    
    // ============================================
    // 2次方程式
    // ============================================
    solve_quadratic(1, -5, 6);   // x² - 5x + 6 = 0 → x = 2, 3
    solve_quadratic(1, -4, 4);   // x² - 4x + 4 = 0 → x = 2 (重解)
    solve_quadratic(1, 2, 5);    // x² + 2x + 5 = 0 → 虚数解

    // ============================================
    // ニュートン法
    // ============================================
    printf("\n=== ニュートン法: x³ - 2x - 5 = 0 ===\n");
    double sol1 = newton_method(f1, df1, 2.0, 1e-10, 20);
    printf("解: x ≈ %.10f\n", sol1);
    printf("検算: f(%.10f) = %.10e\n", sol1, f1(sol1));

    printf("\n=== ニュートン法: sin(x) - x/2 = 0 ===\n");
    double sol2 = newton_method(f2, df2, 2.0, 1e-10, 20);
    printf("解: x ≈ %.10f\n", sol2);
    printf("検算: f(%.10f) = %.10e\n", sol2, f2(sol2));

    // ============================================
    // 二分法
    // ============================================
    printf("\n=== 二分法: e^x - 3x = 0 (区間[0,1]) ===\n");
    double sol3 = bisection_method(f3, 0.0, 1.0, 1e-10, 50);
    printf("解: x ≈ %.10f\n", sol3);
    printf("検算: f(%.10f) = %.10e\n", sol3, f3(sol3));

    // ============================================
    // 連立方程式
    // ============================================
    // 2x + 3y = 8
    // 4x - y = 2
    solve_linear_system_2x2(2, 3, 8, 4, -1, 2);  // x = 1, y = 2

    printf("\n=== 完了 ===\n");
    return 0;
}
