#include <iostream>
#include <cassert>
#include <cmath>

#define RUN_TEST 0 //1 - test on, 0 - test off

#define N 60001 //len of number

#define FL_P 0 //len of float part

#define BASE 10 //base system

using namespace std;

class InfNum {
    int sign = 1;
    int data[N] = { 0 };
    int size = 0;
    
    bool isZero() {
        for (int i = N - 1; i >= size; i--) {
            if (data[i] != 0) {
                return false;
            }
        }
        return true;
    }

    bool isOne() {
        for (int i = N - 2; i >= size; i--) {
            if (data[i] != 0 && i != N - 1 - FL_P) {
                return false;
            }
        }
        return data[N - 1 - FL_P] == 1;
    }

    void divByInt(InfNum &num, int intNum) {
        for (int i = 0; i < N -1 ; i++) {
            num.data[i + 1] += (num.data[i] % intNum) * BASE;
        	num.data[i] /= intNum;
        }
        num.data[N - 1] /= intNum;
        num.updateSize();
    }
    
    void swap(InfNum &a, InfNum &b) {
        InfNum c = a;
        a = b; b = c;
    }

 	static InfNum mult(InfNum &a, InfNum &b) {
        InfNum c;
        int iS, jS;

        for (int i = N - 1; i >= a.size; i--) {
			iS = N - 1 - i - FL_P;	
		
            for (int j = N - 1; j >= b.size; j--) {
            	jS = N - 1 - j - FL_P;
            	int index = N - 1 - (iS + jS + FL_P); 
                if (index >= 0) {
                    c.data[index] += a.data[i] * b.data[j];
                }
            }
           
        }
        c.fixSystem();
        c.sign = a.sign * b.sign;
        c.updateSize();
        return c;
    }
 
	void binarySearch(InfNum l, InfNum r, InfNum &x, InfNum &a, InfNum &b, InfNum f(InfNum &x, InfNum &y)) { //search x in [l; r]
		InfNum preX, res;
		int fR;
		
		do {
			preX = x;
			x = l + r; divByInt(x, 2);
			res = f(x, b);
			fR = compare(a, res);
			
			if (fR == 1) {
                l = x;
            } else {
                r = x;
            }
		} while (fR && compare(x, preX));
		return;
	}
	
    InfNum sum(InfNum &a, InfNum &b) {
        InfNum c;

        for (int i = N - 1; i >= min(a.size, b.size); i--) {
            c.data[i] = a.sign * a.data[i] + b.sign * b.data[i];
        }
        c.fixSystem();
        return c;
    }

    InfNum diff(InfNum &a, InfNum &b) {
    	InfNum c;

        for (int i = N - 1; i >= min(a.size, b.size); i--) {
            c.data[i] = a.sign * a.data[i] - b.sign * b.data[i];
        }
        c.fixSystem();
        return c;
    }

    void fixSystem() {
    	updateSize();
    	
		for (int i = N - 1; i > size - 1; i--) {
			if (data[i] > BASE - 1) {
				data[i - 1] += data[i] / BASE;
				data[i] %= BASE;
				
			} else if (data[i] < 0) {
				int ost = ceil((-1 * float(data[i])) / BASE);
				data[i] += BASE * ost;
				data[i - 1] -= ost;		
			}
		}
		if (data[size - 1] < 0) {
				sign *= -1;
			for (int i = size - 1; i < N; i++) {
				data[i] *= -1;
			}
			fixSystem();
		}
    }

    InfNum div(InfNum &a, InfNum &b) {
        int cmp = compare(a, b);
        InfNum x;

        if (a.isZero() || b.isZero()) {
            return zero();
        }
        if (cmp == 0) {
            return one();
        }
		if (b.isOne()) {
			return a; 
		}
        binarySearch(zero(), a, x, a, b, mult);
        return x;
    }

	void updateSize() {
		size = 0; 
		while (data[size] == 0) {
			size++;
		}
	}
	
	InfNum zero() {
        InfNum a;
        return a;
    }
	    
    InfNum one() {
        InfNum a;
        a.set("1");
        return a;
    }
	
    public:
    	static int compare(InfNum &a, InfNum &b) {
	        for (int i = min(a.size, b.size); i < N; i++) {
	            if (a.data[i] > b.data[i]) {
	                return 1;
	            } 
				if (a.data[i] < b.data[i]) {
	                return -1;
	            }
	    	}
	        return 0;
   		}
    
        int toInt() {
            int res = 0;

            for (int i = N - 1 - FL_P; i >= size; i--) {
                res += data[i] * pow(BASE, N - 1 - i - FL_P);
            }
            if (sign == -1) {
                res *= -1;
            }
            return res;
        }

        InfNum operator + (InfNum &num) {
            InfNum result = sum(*this, num);
            result.updateSize();
            return result;
        }

        InfNum operator - (InfNum &num) {
            InfNum result = diff(*this, num);
            result.updateSize();
            return result;
        }

        InfNum operator * (InfNum &num) {
            InfNum result = mult(*this, num);
            return result;
        }

        InfNum operator / (InfNum &num) {
            InfNum result = div(*this, num);
            result.updateSize();
            result.sign = this -> sign * num.sign;
            return result;
        }
        
        InfNum sqrt_l() {
        	if (sign == -1) return zero();
        	
        	InfNum x, one_obj = one(), r = *this + one_obj;
        
        	binarySearch(zero(), r, x, *this, x, mult);
        	return x;
		}

        char in() {
            string s;
            
            cin >> s;
            set(s);
        	return ' ';
        }

        void set(string s) {
            *this = zero();

            int i = 0;

            if (s[0] == '-') {
                i++;
          	    sign = -1;
            } else {
                sign = 1;
            }
            
            int k = 0, iS = -1;
            bool flag = false;
            
            for (int i = 0; i < s.size(); i++) {
            	if (s[i] == '.') {
            		flag = true;
            		iS = i;
				}
				if (flag) {
					if (s[i] != '.') {
						k++;
						data[N - 1 - (FL_P - k)] = s[i] - '0';
					}
				}
			}
			
			if (iS > -1)
			s.erase(iS);
			
			int len = s.size();
			
            for (i; i < len; i++) {
            	data[N - len + i - FL_P] = s[i] - '0';
            }  
            updateSize();
        }

        char out() {
        	bool flag = false;
        	
            if (sign == -1) {
                cout << "-";
            }
            
            int iEnd = N - 1;
            	
			while (data[iEnd] == 0 && iEnd > N - 1 - FL_P) {
				iEnd--;
			}
            
            for (int i = min(size, N - 1 - FL_P); i <= iEnd; i++) {
            	if (i == N - FL_P) {
            		cout << ".";
				}
                if (data[i] != 0 || i == N - FL_P) {
                    flag = true;
                }
                if (flag) {
                    cout << data[i];
                }
            }
            if (!flag) {
                cout << "0";
            }
            return ' ';
        }
};

void test() {
	cout << "Running test ..." << endl;
	
	InfNum a, b, one;

	one.set("1");
	a.set("-999");

	for (int aInt = -999; aInt <= 999; aInt++, a = a + one) {
		b.set("-999");
		for (int bInt = -999; bInt <= 999; bInt++, b = b + one) {
			assert(aInt + bInt== (a + b).toInt() && "+ error!");
			assert(aInt - bInt == (a - b).toInt() && "- error!");	
			assert(aInt  * bInt == (a * b).toInt() && "* error!");
			if (bInt != 0) {
				assert(aInt / bInt == (a / b).toInt() && "/ error!");
			}
		}
	}
	cout << "Test passed!" << endl;
	return;
}

int main() {
	
	if (RUN_TEST) {
		test();
	}

/*
    InfNum a, b, c;

    cout << "Enter num a: "; cout << a.in() << endl;
    

    cout << "Enter num b: "; cout << b.in() << endl;

    c = a + b;

    cout << "a + b = "; cout << c.out() << endl;

    //

    c = a - b;

    cout << "a - b = "; cout << c.out() << endl;

    //

    c = a * b;

    cout << "a * b = "; cout << c.out() << endl;

    //

    c = a / b;

    cout << "a / b = "; cout << c.out() << endl;
    
    //
    
    c = a.sqrt_l();

    cout << "sqrt(a) = "; cout << c.out() << endl;
    
    c = b.sqrt_l();

    cout << "sqrt(b) = "; cout << c.out() << endl;
*/

	InfNum a, b, c;

	a.in();
	b.in();
	
	c = a / b;

	c.out();
	
	//(b * c - a).out();
	
    return 0;
}