
#include <Fl/Fl.H>
#include <Fl/Fl_Window.H>
#include <Fl/Fl_Input.H>
#include <Fl/Fl_Output.H>
#include <Fl/Fl_Button.H>
#include <Fl/Fl_Tabs.H>
#include <Fl/Fl_Group.H>
#include <Fl/Fl_Choice.H>

#include <cstdio>
#include <cstring>
#include "HazeFunc.h"

Fl_Input *inLambda;
Fl_Button *btnCalc1;
Fl_Output *outTau, *outPi, *outEps, *outQ, *outPhi, *outY;

Fl_Choice *choiceFunc;
Fl_Input *inFunc;
Fl_Button *btnCalc2;
Fl_Output *outLambda1, *outLambda2;

static void Calc1_CB(Fl_Widget *, void *);
static void Calc2_CB(Fl_Widget *, void *);

int main(int argc, char **argv) {
    Fl_Window window(400, 500, "Haze Dynamics Calculator");
	window.begin();
	
	Fl_Tabs tabs(10, 10, 380, 480);
	
	// Group 1
	Fl_Group group1(10, 40, 380, 450, "vel->func");
	//group1.color(FL_DARK2);
	group1.color2(FL_DARK2);
	
	inLambda = new Fl_Input(150, 70, 150, 25, "lambda = ");
	inLambda->value("1.0");
	
	btnCalc1 = new Fl_Button(150, 105, 100, 30, "Calculate");
	
	outTau = new Fl_Output(150, 145, 150, 25, "tau = ");
	outPi = new Fl_Output(150, 175, 150, 25, "pi = ");
	outEps = new Fl_Output(150, 205, 150, 25, "eps = ");
	outQ = new Fl_Output(150, 235, 150, 25, "q = ");
	
	outPhi = new Fl_Output(150, 270, 150, 25, "phi = ");
	outY = new Fl_Output(150, 300, 150, 25, "y = ");
	
	group1.end();
	// Group 1 ends
	
	// Group 2
	Fl_Group group2(10, 40, 380, 450, "func->vel");
	group2.color2(FL_DARK2);
	
	choiceFunc = new Fl_Choice(50, 70, 75, 25);
	choiceFunc->add("tau");
	choiceFunc->add("pi");
	choiceFunc->add("eps");
	choiceFunc->add("q");
	choiceFunc->add("phi");
	choiceFunc->add("y");
	choiceFunc->value(0);
	
	inFunc = new Fl_Input(150, 70, 150, 25, " = ");
	inFunc->value("1.0");
	
	btnCalc2 = new Fl_Button(150, 105, 100, 30, "Calculate");
	
	outLambda1 = new Fl_Output(150, 145, 150, 25, "lambda1 = ");
	outLambda2 = new Fl_Output(150, 175, 150, 25, "lambda2 = ");
	
	group2.end();
	// Group 2 ends
	
	tabs.end();
	
	btnCalc1->callback(Calc1_CB);
	btnCalc2->callback(Calc2_CB);
	
	window.end();
	window.show(argc, argv);
    return Fl::run();
}

void Calc1_CB(Fl_Widget *, void *) {
	char buff[256];
	double lambda, x;
	
	strcpy(buff, inLambda->value());
	lambda = atof(buff);
	
	x = HazeFunc::tau(lambda);
	sprintf(buff, "%12.4f", x);
	outTau->value(buff);
	
	x = HazeFunc::pi(lambda);
	sprintf(buff, "%12.4f", x);
	outPi->value(buff);
	
	x = HazeFunc::eps(lambda);
	sprintf(buff, "%12.4f", x);
	outEps->value(buff);
	
	x = HazeFunc::q(lambda);
	sprintf(buff, "%12.4f", x);
	outQ->value(buff);
	
	x = HazeFunc::phi(lambda);
	sprintf(buff, "%12.4f", x);
	outPhi->value(buff);
	
	x = HazeFunc::y(lambda);
	sprintf(buff, "%12.4f", x);
	outY->value(buff);
}

void Calc2_CB(Fl_Widget *, void *) {
	char buff[256];
	// const char *func;
	int func;
	double x, l1, l2;
	
	strcpy(buff, inFunc->value());
	x = atof(buff);
	
	func = choiceFunc->value();
	if (func==0 /*strcmp(func, "tau")==0*/) {
		l1 = HazeFunc::lambdaTau(x);
		l2 = -1.0;
		
	} else if (func==1 /*strcmp(func, "pi")==0*/) {
		l1 = HazeFunc::lambdaPi(x);
		l2 = -1.0;
		
	} else if (func==2 /*strcmp(func, "eps")==0*/) {
		l1 = HazeFunc::lambdaEps(x);
		l2 = -1.0;
		
	} else if (func==3 /*strcmp(func, "q")==0*/) {
		HazeFunc::lambdaQ(x, l1, l2, 1e-6, 10000);
		
	} else if (func==4 /*strcmp(func, "phi")==0*/) {
		HazeFunc::lambdaPhi(x, l1, l2, 1e-6, 10000);
		
	} else if (func==5 /*strcmp(func, "y")==0*/) {
		l1 = HazeFunc::lambdaY(x, 1e-6, 10000);
		l2 = -1.;
		
	}
	
	sprintf(buff, "%12.4f", l1);
	outLambda1->value(buff);
	
	if (l2>0.0) sprintf(buff, "%12.4f", l2);
	else strcpy(buff, "");
	outLambda2->value(buff);
}

