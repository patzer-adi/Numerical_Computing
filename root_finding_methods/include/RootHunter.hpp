
class RootHunter {
private:
    double left;
    double right;
    double tolerance;
    double root;
    int iterations;

    double f(double x);

public:
    RootHunter(double tol);

    bool findInterval(double start, double step, int maxSteps);
    void solve();

    double getRoot() ;
    int getIterations() ;
};

/*
 * class RootHunter {
 *
 * private:
 * double start;
 * double f(double x);
 *  double root;
 * int iterations;
 *
 *  double getRoot();
 int getIterations();

 public:
     void solve();

     };

 *
 * class bisection_method : public RootHunter
 * {
 * private:
 double left;
 double right;
 double tolerance;


 public:
     bisection_method(double tol);

     bool findInterval(double start, double step, int maxSteps);
     void solve();


 * }
 *
 * class Newton_Raphson : public RootHunter
 * {
 private:
     double f_dash(double x);

    public:
        void solve;
 *
 * }
 *
 */
