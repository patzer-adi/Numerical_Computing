
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

    double getRoot() const;
    int getIterations() const;
};

