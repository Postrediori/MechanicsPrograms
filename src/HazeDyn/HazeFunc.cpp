// HazeFunc.cpp

#include <math.h>
#include "HazeFunc.h"

namespace HazeFunc {
	double k = 1.4;
	
	double tau(double lambda) {
		return (1.0-(k-1.0)*lambda*lambda/(k+1.0));
	}

	double pi(double lambda) {
		return pow(tau(lambda), k/(k-1.0));
	}

	double eps(double lambda) {
		return pow(tau(lambda), 1.0/(k-1.0));
	}

	double q(double lambda) {
		return (eps(lambda)*lambda*pow(2.0/(k+1.0),-1.0/(k-1.0)));
	}
	
	double dq(double lambda) {
		double q1, q2;
		q1 = pow(2.0/(k+1.0), -1.0/(k-1.0));
		q2 = pow(tau(lambda), 1.0/(k-1.0)-1.0);
		return (1.0-lambda*lambda)*q1*q2;
	}

	double phi(double lambda) {
		return (1.0/(lambda*lambda)+2.0*log(lambda));
	}
	
	double dphi(double lambda) {
		return 2.0*(1.0-1.0/(lambda*lambda))/lambda;
	}

	double y(double lambda) {
		return (q(lambda)/pi(lambda));
	}
	
	double dy(double lambda) {
        double A = (k-1.0)/(k+1.0);
        double B = 2.0/(k+1.0);
        double C = pow(B, (-1.0/(k-1.0)));
        return C*(1.0+2.0*A*lambda*lambda)/tau(lambda);
	}
	
	double lambdaTau(double tau) {
		return sqrt((k + 1.0) * (1.0 - tau) / (k - 1.0));
	}
	
	double lambdaPi(double pi) {
		return lambdaTau(pow(pi, (k - 1.0) / k));
	}
	
	double lambdaEps(double eps) {
		return lambdaTau(pow(eps, (k - 1.0)));
	}
	
	double lq(double qc, unsigned char side,
			  double epsilon, unsigned int MaxIter) {
		double xn, oldxn;
		unsigned int iters;
		
		if (side==1) xn = 0.5;
		else xn = 1.5;
		
		for (iters = 0; iters<MaxIter; iters++) {
			oldxn = xn;
			xn = oldxn - (q(oldxn) - qc) / dq(oldxn);
			if (fabs(xn - oldxn) < epsilon) break;
		}
		
		return xn;
	}
	
	void lambdaQ(double qc, double &l1, double &l2,
				   double epsilon, unsigned int MaxIter) {
		l1 = lq(qc, 1, epsilon, MaxIter);
		l2 = lq(qc, 2, epsilon, MaxIter);
	}
	
	double lphi(double phic, unsigned char side,
				double epsilon, unsigned int MaxIter) {
		double xn, oldxn;
		unsigned int iters;
		
		if (side==1) xn = 0.5;
		else xn = 1.5;
		
		for (iters = 0; iters<MaxIter; iters++) {
			oldxn = xn;
			xn = oldxn - (phi(oldxn) - phic) / dphi(oldxn);
			if (fabs(xn - oldxn) < epsilon) break;
		}
		
		return xn;
	}
	
	void lambdaPhi(double phi, double &l1, double &l2,
		double epsilon, unsigned int MaxIter) {
		l1 = lphi(phi, 1, epsilon, MaxIter);
		l2 = lphi(phi, 2, epsilon, MaxIter);
	}
	
	double lambdaY(double yc, double epsilon, unsigned int MaxIter) {
		double xn, oldxn;
		unsigned int iters;
		
		xn = 1.0;
		
		for (iters = 0; iters<MaxIter; iters++) {
			oldxn = xn;
			xn = oldxn - (y(oldxn) - yc) / dy(oldxn);
			if (fabs(xn - oldxn) < epsilon) break;
		}
		
		return xn;
	}
}

