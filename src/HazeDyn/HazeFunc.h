// HazeFunc.h
#ifndef HAZEFUNC_H

namespace HazeFunc {
	double tau(double lambda);
	double pi(double lambda);
	double eps(double lambda);
	double q(double lambda);

	double phi(double lambda);
	double y(double lambda);
	
	double lambdaTau(double tau);
	double lambdaPi(double pi);
	double lambdaEps(double eps);
	void lambdaQ(double q, double &l1, double &l2,
				 double epsilon, unsigned int MaxIter);
	
	void lambdaPhi(double phi, double &l1, double &l2,
				 double epsilon, unsigned int MaxIter);
	double lambdaY(double y, double epsilon, unsigned int MaxIter);
};

#define HAZEFUNC_H
#endif
