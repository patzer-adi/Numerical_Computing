# OOP & C++ Deep Dive — Why Things Are The Way They Are

This document explains every OOP concept, C++ feature, and design decision in this
codebase. It answers the "why" behind every choice using real code from this library.

---

# PART 1: OOP CONCEPTS

---

## 1. ENCAPSULATION

### What it is

Encapsulation means a class **protects its internal state** and only exposes
what is necessary. It is NOT just about putting `private:` on variables.
True encapsulation means the class **guarantees invariants** — rules about
its internal data that can never be broken from outside.

### Where we use it

**Matrix class** — the core example:

```cpp
class Matrix {
protected:
  double **data;
  int rows;
  int cols;
```

`data`, `rows`, `cols` are `protected`. No outside code can directly set
`rows = -5` or `data = nullptr` without going through a method. The class
controls how data is allocated, resized, and freed.

### The invariant we guarantee

If `rows > 0` and `cols > 0`, then `data` points to a valid `rows × cols`
2D array. This is guaranteed by **every constructor** and **every method**
that modifies data:

```cpp
Matrix::Matrix(int r, int c) {
  rows = r;
  cols = c;
  data = new double *[rows];
  for (int i = 0; i < rows; i++) {
    data[i] = new double[cols];
    for (int j = 0; j < cols; j++)
      data[i][j] = 0.0;
  }
}
```

No constructor leaves `data` in a half-allocated state. If `r` and `c` are
given, the entire grid is allocated and zeroed. If default constructed,
everything is `nullptr` / `0`.

### Why `protected` and not `private`?

Because `SystemOfLinearEquationSolver` inherits from `Matrix` and needs
direct access to `data[][]` for performance. Making it `private` would
force every solver to call `getData(i, j)` in tight loops — an unnecessary
function call overhead when the solver IS-A matrix and already owns the data.

**Rule of thumb:**

- `private` — when no derived class should ever touch it
- `protected` — when derived classes in YOUR hierarchy need it
- `public` — when external code needs it

### Does `friend` break encapsulation?

**Yes, partially.** We use `friend` for stream operators:

```cpp
friend ostream &operator<<(ostream &out, const Matrix &m);
friend istream &operator>>(istream &in, Matrix &m);
```

This is a **controlled escape hatch**, not random access. We grant access
to exactly two functions, and those functions have a specific job: read/write
the matrix. Without `friend`, we could not write `cout << myMatrix` because
`operator<<` must be a non-member function (left operand is `ostream`, not
`Matrix`).

**Why not just use getters?** We could write:

```cpp
ostream &operator<<(ostream &out, const Matrix &m) {
  for (int i = 0; i < m.getRows(); i++)
    for (int j = 0; j < m.getCols(); j++)
      out << m.getData(i, j);
```

This works but is slower — `getData()` does bounds checking on every call.
The friend function accesses `m.data[i][j]` directly, which is safe because
the operator is part of the class's "trust circle."

### Does exposing `double* getRowPointer(int i)` break encapsulation?

**Yes.** This is a deliberate hole. The caller gets a raw pointer to internal
memory and can modify elements without the Matrix knowing. We accept this
risk specifically for CUDA memory transfers where performance matters:

```cpp
double *Matrix::getRowPointer(int i) {
  if (i < 0 || i >= rows)
    throw MatrixException("row index out of bounds in getRowPointer");
  return data[i];
}
```

The alternative would be copying data into a separate buffer for CUDA, which
is what we already do in the GPU codepath anyway. `getRowPointer` exists as
an optimization for specific use cases where the caller promises not to
corrupt the data.

**Takeaway:** Perfect encapsulation is a spectrum. Sometimes you make
calculated tradeoffs for performance. The key is making those tradeoffs
explicit and documented, not accidental.

---

## 2. ABSTRACTION

### What it is

Abstraction means exposing **what** a class does without exposing **how**.
The user of a class works with a simplified interface and does not need
to understand the internal algorithm.

### Where we use it

**SystemOfLinearEquationSolver** is the prime example:

```cpp
class SystemOfLinearEquationSolver : public Matrix {
public:
  virtual SolverResult solve(double *b, int n,
                             int maxIter = 10000,
                             double tol = 1e-10) = 0;
};
```

Every solver — Gaussian Elimination, Doolittle, Crout, Cholesky, Jacobi,
Seidel — has the **exact same interface**: give me a vector `b` and size
`n`, I will return a `SolverResult`. The caller writes:

```cpp
SolverResult res = solver.solve(b, n);
```

They don't care whether `solver` is doing Gaussian elimination with
partial pivoting, LU decomposition with forward/back substitution, or
iterative convergence. The **abstraction boundary** is `solve()`.

### What makes this a "good" abstraction?

1. **Minimal interface** — one method (`solve`) does the job
2. **Swappable** — change `Doolittle d;` to `Crout c;` and everything
   else stays identical
3. **Not too generic** — `solve()` takes `double* b` specifically because
   every solver needs a right-hand side vector
4. **Not too specific** — it doesn't expose LU-specific params like
   "which triangle has unit diagonal"

### Abstraction in the Interpolation hierarchy

Same pattern:

```cpp
class Interpolation {
  virtual double evaluate(double x) = 0;
};
```

Tomorrow if we add Newton interpolation, the caller writes:

```cpp
Newton newt(x, y, n);
double val = newt.evaluate(2.5);
```

Same interface, different algorithm underneath. The abstraction holds.

### Can an abstract class have a constructor?

**Yes.** Both `EigenSolver` and `Interpolation` have constructors:

```cpp
EigenSolver::EigenSolver(const Matrix &m) : mat(m) {
  if (!m.isSquare())
    throw MatrixException(...);
}
```

Even though you cannot create an `EigenSolver` object directly (it's
abstract), the constructor runs when a derived class like
`GershgorinAnalyzer` is constructed. The base constructor validates the
matrix is square — this is shared logic that every eigenvalue method needs.

### Can an abstract class have implemented functions?

**Yes.** `Interpolation` has two fully implemented `interpolate()` overloads.
These are shared algorithms that work for any interpolation method because
they call the virtual `evaluate()` which each derived class provides.

---

## 3. INHERITANCE

### What it is

Inheritance creates an "is-a" relationship. A derived class IS a base class
plus additional or specialized behavior.

### Where we use INHERITANCE (and why it's correct)

**Matrix → SystemOfLinearEquationSolver → GaussianElimination**

A `GaussianElimination` object IS-A `Matrix`. It holds matrix data in
`this->data` and operates on it. The solver literally IS the coefficient
matrix with extra behavior (the `solve()` method).

```cpp
class GaussianElimination : public SystemOfLinearEquationSolver {
public:
  SolverResult solve(double *b, int n, ...) override;
  double *solveWithPivoting(double *b, int n);
  double *solveWithoutPivoting(double *b, int n);
};
```

This is correct because:

- The solver stores the matrix `A` in its own `data[][]`
- Client code reads matrix data into the solver directly: `getMatrixInput(ge)`
- The solver modifies its own matrix during elimination

### Where we use INHERITANCE (Interpolation → Lagrange)

```cpp
class Lagrange : public Interpolation {
  double evaluate(double x) override;
};
```

This is correct because a `Lagrange` IS-AN `Interpolation`. It has all the
same data (xData, yData, numPoints) and all the same operations (interpolate,
loadData). It only adds the specific algorithm for `evaluate()`.

### Do we need virtual destructors?

**Yes. Always when using inheritance with polymorphism.**

```cpp
class Interpolation {
  virtual ~Interpolation();
};
```

Without `virtual ~Interpolation()`, if you write:

```cpp
Interpolation *p = new Lagrange(x, y, 5);
delete p;  // BUG: calls ~Interpolation(), NOT ~Lagrange()
```

`~Lagrange()` would never run. If Lagrange had its own dynamically allocated
memory, it would **leak**. With `virtual`, the correct destructor is called
through the vtable.

**What happens internally:**

1. Compiler creates a vtable for `Interpolation` with entry for `~Interpolation`
2. `Lagrange` gets its own vtable with entry for `~Lagrange`
3. When you `delete p`, the runtime looks up the vtable of the actual object
4. Finds `~Lagrange`, calls it first, then `~Interpolation` (chain)

### Liskov Substitution Principle (LSP)

**Rule:** If `D` inherits from `B`, then anywhere you use `B`, you should
be able to use `D` without breaking anything.

Our code satisfies this. `solveInterpolation(Interpolation &interp, ...)` accepts
any `Interpolation` subclass. If we pass a `Lagrange`, it works. If tomorrow
we pass a `Newton`, it works. The function never needs to know which subclass
it's dealing with.

---

## 4. POLYMORPHISM

### Runtime polymorphism (virtual functions)

We use this extensively. The key mechanism:

```cpp
SystemOfLinearEquationSolver &solver = someConcreteObject;
SolverResult res = solver.solve(b, n);  // calls correct override
```

The compiler does NOT know at compile time which `solve()` to call. At runtime,
it looks up the vtable of the actual object and dispatches to the correct
implementation.

### How vtable works internally

Every class with at least one virtual function gets a **vtable** — a table of
function pointers. Every object of that class gets a hidden **vptr** — a pointer
to its class's vtable.

```
Object layout (GaussianElimination):
+--------+
| vptr   | -----> vtable for GaussianElimination
| data   |          solve() -> GaussianElimination::solve
| rows   |          ~dtor   -> GaussianElimination::~dtor
| cols   |
+--------+
```

When you call `solver.solve(b, n)`:

1. Follow `solver.vptr` to find the vtable
2. Look up slot for `solve()`
3. Call the function pointer found there

**Cost:** One pointer indirection per virtual call. In tight loops (millions
of calls), this matters. In our case, `solve()` is called once per problem —
the cost is negligible.

### Why we DON'T use templates (compile-time polymorphism)

Templates would eliminate virtual dispatch overhead but they have downsides:

1. **All code must be in headers** — our `.cpp` files would not work
2. **No runtime swapping** — can't pick a solver based on user input
3. **Code bloat** — each template instantiation generates a full copy

Our menu literally does:

```cpp
case 7: handleDoolittle(); break;
case 8: handleCrout(); break;
```

The user picks at runtime. Templates can't do this.

### Can constructors be virtual?

**No.** When the constructor runs, the object isn't fully formed yet. There's
no vtable to look up because the object doesn't know its type yet. The vptr
is set during construction — base constructor sets vptr to base's vtable,
then derived constructor updates it to derived's vtable.

### What happens if virtual function is called in constructor?

The **base class version** is called, not the derived version. Because during
base construction, the vptr still points to the base vtable:

```
Construction order:
1. Allocate memory
2. Set vptr → Base vtable
3. Run Base constructor      ← virtual calls go to Base version
4. Set vptr → Derived vtable
5. Run Derived constructor   ← virtual calls go to Derived version
```

This is why we never call `solve()` or `evaluate()` inside constructors.

---

## 5. COMPOSITION (THE MOST IMPORTANT CONCEPT)

### What it is

Composition means a class **contains** another class as a member. It models
a "has-a" relationship, not "is-a".

### Where we use it (and WHY not inheritance)

**EigenSolver holds a Matrix reference:**

```cpp
class EigenSolver {
protected:
  const Matrix &mat;
};
```

**Interpolation uses Matrix for I/O** (in the Display workflow):

```cpp
void solveInterpolation(Interpolation &interp, const string &methodName) {
  Matrix xMat;
  getMatrixInput(xMat);  // uses Matrix I/O infrastructure
  // ... extract data into double* arrays ...
  interp.loadData(xArr, yArr, n);
}
```

### Why NOT inherit from Matrix?

An `Interpolation` is NOT a matrix. An `EigenSolver` is NOT a matrix.

- A matrix has rows, columns, addition, multiplication, determinant
- An interpolation has data points, a polynomial, evaluation at x
- An eigen solver has eigenvalue bounds, convergence criteria

If `Interpolation` inherited from `Matrix`, it would have methods like
`determinant()`, `inverse()`, `operator+` — all meaningless for interpolation.
This would violate the **Liskov Substitution Principle**: you could write
`interp.determinant()` which makes no semantic sense.

Compare with `GaussianElimination` inheriting from `Matrix` — a GE solver
DOES have rows, columns, and its data IS the coefficient matrix. The
`is-a` relationship holds.

### Composition is more flexible. Here's why:

With inheritance:

```
Matrix
  └── SystemOfLinearEquationSolver
        └── GaussianElimination
  └── Interpolation  ← WRONG: pollutes Matrix hierarchy
  └── EigenSolver    ← WRONG: same problem
```

With composition:

```
Matrix (standalone, clean)

SystemOfLinearEquationSolver (inherits Matrix ← correct is-a)
  └── GaussianElimination
  └── LU variants

Interpolation (USES Matrix via composition ← correct has-a)
  └── Lagrange

EigenSolver (USES Matrix via composition ← correct has-a)
  └── GershgorinAnalyzer
```

Each hierarchy is independent. Adding a new interpolation method doesn't
touch Matrix. Adding a new eigen method doesn't touch interpolation.
This is **loose coupling**.

### Ownership in composition

**EigenSolver holds a reference** (`const Matrix &mat`):

- Does NOT own the Matrix
- Matrix must outlive the EigenSolver
- EigenSolver cannot modify the Matrix (const reference)

**Interpolation owns its data** (`double *xData`, `double *yData`):

- Copies input arrays in constructor/loadData
- Deletes them in destructor
- Full ownership, full lifecycle control

These are deliberate decisions. EigenSolver only reads the matrix (const ref
is cheapest — no copy). Interpolation needs its own data because the original
arrays might go out of scope after construction.

---

## 6. ASSOCIATION

### What it is

Association describes how objects relate to each other: who knows about whom,
who owns whom, who outlives whom.

### Three levels

**Association** — objects know about each other but don't own each other.
Example: `EigenSolver` holds a `const Matrix&`. The Matrix exists independently
and could be used by multiple EigenSolvers simultaneously.

**Aggregation** — one object contains another but doesn't own it. Same as
association in C++ terms (reference/pointer without ownership).

**Composition (strong)** — one object contains and OWNS another. When the
parent dies, the child dies. Example: `Interpolation` owns `xData` and `yData`.
When `~Interpolation()` runs, those arrays are freed.

### The `SolverResult` ownership trap

```cpp
struct SolverResult {
  double *x;   // solution vector (caller must delete[])
  double **L;  // lower triangular (caller must freeLU())
  double **U;  // upper triangular
};
```

**Who owns this memory?** The comment says: "caller must delete[]". This is
**transfer of ownership**. The solver creates the result, but the caller
becomes the owner. If the caller forgets to call `delete[] res.x` and
`res.freeLU()`, memory leaks.

This is the cost of raw pointers. With `std::unique_ptr`, ownership would
be enforced by the type system. We use raw pointers for library consistency,
but we document ownership explicitly.

---

# PART 2: C++ FEATURES

---

## 1. POINTERS & DYNAMIC MEMORY

### Why `double**` and not `vector<vector<double>>`?

This was a deliberate choice for the entire library. Reasons:

1. **Heading toward a library** — raw pointers give maximum control over
   memory layout, alignment, and allocation strategy
2. **CUDA compatibility** — GPU memory transfers need contiguous raw pointers.
   `vector` adds indirection that complicates `cudaMemcpy`
3. **Educational purpose** — understanding manual memory management is
   fundamental to understanding C++
4. **Consistency** — every data structure in the library uses the same style:
   `double**`, `double*`, `GershgorinDisc*`. No mixing paradigms.

### Memory layout

```cpp
// Matrix::data is a double** (pointer to array of row pointers)
data -> [ptr0] -> [1.0, 2.0, 3.0]
        [ptr1] -> [4.0, 5.0, 6.0]
        [ptr2] -> [7.0, 8.0, 9.0]
```

Each row is a separate `new double[cols]` allocation. This means rows are NOT
contiguous in memory — row 0 might be at address 0x1000 and row 1 at 0x5000.

**Why not one contiguous block?** The `double**` approach allows pointer
swapping for row operations (pivoting):

```cpp
double *temp = result.data[i];
result.data[i] = result.data[bestRow];
result.data[bestRow] = temp;
```

This swaps two pointers — O(1). With contiguous memory, you'd need to copy
the entire row — O(n).

### Who owns the memory?

**Rule we follow throughout:**

- **Constructor allocates** → **Destructor frees**
- If a function returns `new[]` memory → caller must `delete[]`
- `SolverResult.x` is created by solver → freed by caller
- `Interpolation.xData` is created by Interpolation → freed by `~Interpolation`

### What about exceptions?

If an exception occurs between `new` and `delete`, memory leaks. Example:

```cpp
double *x = new double[n];
// ... code that might throw ...
delete[] x;  // never reached if throw happens
```

This is why RAII (Resource Acquisition Is Initialization) exists — wrap
resources in objects whose destructors run automatically. We don't use smart
pointers here for library consistency, but we are aware of the risk and keep
allocation/deallocation close together.

---

## 2. RULE OF 5

### When it applies

If a class **manages resources** (raw pointers, file handles, etc.), you MUST
implement all five special members:

1. **Destructor** — free resources
2. **Copy constructor** — deep copy
3. **Copy assignment** — deep copy + free old
4. **Move constructor** — steal resources from dying object
5. **Move assignment** — steal resources + free old

### Matrix implements all five

**Destructor:**

```cpp
Matrix::~Matrix() {
  if (data != nullptr) {
    for (int i = 0; i < rows; i++)
      delete[] data[i];
    delete[] data;
  }
}
```

**Copy constructor (deep copy):**

```cpp
Matrix::Matrix(const Matrix &other) {
  rows = other.rows;
  cols = other.cols;
  data = new double *[rows];
  for (int i = 0; i < rows; i++) {
    data[i] = new double[cols];
    for (int j = 0; j < cols; j++)
      data[i][j] = other.data[i][j];
  }
}
```

Why deep copy? Because the default (shallow) copy would copy the pointer
`data` but not the memory it points to. Both objects would point to the
same arrays. When one is destroyed, it frees the memory. The other now has
a **dangling pointer** — accessing it is undefined behavior.

**Move constructor (steal the guts):**

```cpp
Matrix::Matrix(Matrix &&other) noexcept {
  rows = other.rows;
  cols = other.cols;
  data = other.data;

  other.data = nullptr;
  other.rows = 0;
  other.cols = 0;
}
```

Move is called when the source is a **temporary** that's about to die anyway.
Instead of deep copying (expensive for large matrices), we steal the pointer
and null out the source. No allocation, no copying — O(1).

**When is move triggered?**

```cpp
Matrix createMatrix() {
  Matrix m(100, 100);
  return m;  // compiler uses move constructor (or NRVO)
}

Matrix A = createMatrix();  // move, not copy
```

Also explicit via `std::move`:

```cpp
Matrix B = std::move(A);  // A is now empty, B has the data
```

### Why `noexcept` on move operations?

STL containers (e.g., `vector<Matrix>`) only use move if it's marked `noexcept`.
If move can throw, the container falls back to copy for exception safety.

---

## 3. OPERATOR OVERLOADING

### What we overload and why

| Operator | Type | Why |
|----------|------|-----|
| `+, -, *` | member | natural matrix arithmetic |
| `* (scalar)` | member | `A * 2.0` |
| `==` | member | matrix comparison |
| `()` | member | element access `A(i,j)` |
| `=` | member | deep copy assignment |
| `<<, >>` | friend | stream I/O |

### Why is `operator<<` a friend and not a member?

Because the left operand is `ostream`, not `Matrix`:

```cpp
cout << myMatrix;
// is actually: operator<<(cout, myMatrix)
```

If `operator<<` were a member of `Matrix`, you'd write `myMatrix << cout` which
is backwards. The left operand determines which class the member belongs to.
Since we can't add members to `ostream`, we make it a free function. Since it
needs access to private `data`, `rows`, `cols` — it's a `friend`.

### The `operator()` design

We provide two overloads:

```cpp
double &operator()(int i, int j);       // non-const: A(1,2) = 5.0
double operator()(int i, int j) const;  // const: double v = A(1,2)
```

The non-const version returns a **reference** allowing assignment. The const
version returns a **copy** to prevent modification of const matrices.

Both do bounds checking and throw `MatrixException` if indices are invalid.
This preserves the invariant that no out-of-bounds access occurs.

---

## 4. CONST CORRECTNESS

### What it means

When a function promises not to modify the object, mark it `const`:

```cpp
int getRows() const;
bool isSquare() const;
Matrix transpose() const;
double determinant() const;
```

These methods can be called on `const Matrix` objects. Without `const`,
you couldn't pass a Matrix by `const` reference and call these methods.

### Why it matters for APIs

```cpp
EigenSolver::EigenSolver(const Matrix &m) : mat(m) { ... }
```

The `const Matrix &m` says: "I will NOT modify your matrix." This is a
**contract**. The caller knows the matrix is safe. If the EigenSolver tried
to call `m.setData(...)`, the compiler would reject it at compile time.

### Logical vs bitwise constness

**Bitwise:** no bits of the object change. All our `const` methods satisfy this.

**Logical:** the object appears unchanged from outside but may modify internal
caches. If we cached determinant results (`mutable double cachedDet`), the
method would modify `cachedDet` but still be logically const because the
mathematical state hasn't changed.

We don't use `mutable` currently but it would be the right tool if we added
caching.

---

## 5. VIRTUAL FUNCTIONS

### Where we use them

| Class | Virtual function | Purpose |
|-------|-----------------|---------|
| `SystemOfLinearEquationSolver` | `solve()` | each solver implements its own algorithm |
| `Interpolation` | `evaluate()` | each method computes polynomial differently |
| `EigenSolver` | `solve()`, `printAnalysis()` | each method has different output |
| `Matrix` | `~Matrix()` | correct cleanup in inheritance |

### Cost of vtable

Each object with virtual functions has a hidden `vptr` (8 bytes on 64-bit).
This is per-object, not per-class. For a `Matrix` with a 1000×1000 `double**`,
8 bytes of overhead is nothing.

Virtual dispatch costs one pointer indirection per call. Our hot paths
(matrix multiply inner loops) don't use virtual functions — they operate on
raw `double**` directly.

---

## 6. STATIC MEMBERS

### Where we use them

**`Matrix::inputMatrix(string label)`** — static factory method:

```cpp
static Matrix inputMatrix(string label);
```

**Why static?** Because it creates and returns a NEW Matrix. It doesn't operate
on an existing object. You call it as `Matrix::inputMatrix("Enter A")`.

**`SystemOfLinearEquationSolver::makeDiagDominant()`** — static utility:

```cpp
static bool makeDiagDominant(double **A, double *b, int n);
static bool checkDiagDominant(double **A, int n);
```

**Why static?** These operate on raw `double**` arrays, not on `this->data`.
Multiple solvers (Jacobi, Seidel) share this logic. Making it static means
it belongs to the class namespace but doesn't need an object instance.

### Can static functions be virtual?

**No.** Virtual dispatch requires an object (to find the vptr). Static functions
have no object. Static functions also can't access non-static members because
there's no `this` pointer.

---

## 7. STRUCTS

### Where we use them

**`GershgorinDisc`** — plain data holder:

```cpp
struct GershgorinDisc {
  int row;
  double center;
  double radius;
  double low;
  double high;
};
```

**`SolverResult`** — result container:

```cpp
struct SolverResult {
  double *x;
  int n;
  int iterations;
  bool converged;
  double error;
  bool dominanceAchieved;
  double **L;
  double **U;
  int luSize;
};
```

### Why struct instead of class?

In C++, `struct` and `class` are identical except for default access:

- `struct` — default `public`
- `class` — default `private`

We use `struct` for **plain data containers** that have no invariants to
protect. `GershgorinDisc` is just five numbers bundled together. There's
nothing to encapsulate — all fields are always valid.

We use `class` when there ARE invariants: Matrix guarantees `data` is
properly allocated, Interpolation guarantees `xData`/`yData` are consistent.

### POD vs non-POD

`GershgorinDisc` is a POD (Plain Old Data) type — no constructors, no virtual
functions, no inheritance. It can be safely `memcpy`'d, stored in arrays,
and treated like C-style data.

`SolverResult` is non-POD because it has a constructor and a member function
(`freeLU()`). But it's still used as a value type with no inheritance.

---

## 8. FILE HANDLING

### Where we use it

Matrix I/O:

```cpp
void Matrix::readFromFile(string filename);
void Matrix::saveToFile(string filename);
```

Interpolation output:

```cpp
void Interpolation::interpolate(int samples, bool saveToFile, string filename);
```

### Error handling

Every file operation checks for failure:

```cpp
ifstream fin(filename);
if (!fin)
  throw MatrixException("can't open file '" + filename + "'...");
```

Files are always closed explicitly (`fin.close()`, `fout.close()`). In
production-grade code, RAII handles this (ifstream destructor closes
automatically), but explicit close makes intent clear and flushes output.

### Does file handling belong in Matrix?

**Debatable.** Matrix handles both:

1. Mathematical operations (determinant, inverse, multiply)
2. File I/O (readFromFile, saveToFile)

This might violate Single Responsibility Principle (SRP). An alternative
design would have a separate `MatrixIO` utility class. We keep it in
Matrix for simplicity and because the I/O is tightly coupled with the
internal data format.

The utils layer (`Input.cpp`, `Display.cpp`) handles the more complex I/O
workflows (augmented matrices, system input, save prompts). The Matrix
class only handles basic read/write of its own data.

---

# PART 3: DESIGN CONCEPTS

---

## 1. STRATEGY PATTERN

### What it is

The Strategy Pattern lets you swap algorithms at runtime through a common
interface. Instead of giant `if-else` chains, each algorithm is its own class.

### Where we use it

**Solver hierarchy:**

```cpp
case 7:  handleDoolittle(); break;   // strategy 1
case 8:  handleCrout(); break;       // strategy 2
case 9:  handleCholesky(); break;    // strategy 3
case 10: handleJacobi(); break;      // strategy 4
```

Each solver is a different "strategy" for solving `Ax = b`. The workflow
function `solveIterative(SystemOfLinearEquationSolver &solver, ...)` doesn't
know or care which strategy it's using — it just calls `solver.solve(b, n)`.

**Interpolation hierarchy:**

Same pattern. `solveInterpolation(Interpolation &interp, ...)` accepts any
interpolation strategy. Today it's Lagrange. Tomorrow it's Newton. The
workflow code never changes.

**Eigen hierarchy:**

`EigenSolver` base with `GershgorinAnalyzer`. Each eigenvalue method is a
strategy. The mechanism for swapping is user input through the menu.

### vs if-else chains

Without strategy pattern:

```cpp
if (method == "doolittle") {
  // 100 lines of Doolittle code
} else if (method == "crout") {
  // 100 lines of Crout code
} else if (method == "cholesky") {
  // 100 lines of Cholesky code
}
```

Problems:

- One giant function with 300+ lines
- Adding a new method requires modifying existing code (violates Open/Closed)
- Can't reuse individual methods independently

With strategy pattern, each method is isolated in its own class/file.

---

## 2. INTERFACE-BASED DESIGN

### What it means

"Code to an interface, not an implementation." Functions should accept
abstract base types, not concrete types.

### Our code does this

```cpp
void solveIterative(SystemOfLinearEquationSolver &solver, const string &name);
void solveInterpolation(Interpolation &interp, const string &name);
```

These functions accept base class references. They work with ANY solver
or ANY interpolation method. The caller decides which concrete class to create.

### Why it enables extension

To add a new solver (say, Power Iteration):

1. Create `PowerIteration.hpp/cpp` inheriting from `SystemOfLinearEquationSolver`
2. Implement `solve()`
3. Add menu option

Steps 1-3 don't modify ANY existing code. `solveIterative()` already works
with the new class. This is the **Open/Closed Principle**: open for extension,
closed for modification.

---

## 3. MODULAR ARCHITECTURE

### Our module structure

```
include/    — class declarations (interfaces/contracts)
src/        — class implementations (hidden details)
app/        — application layer (Menu — wires everything together)
utils/      — I/O utilities (Input, Display — shared workflows)
```

### Loose coupling

Each module has minimal dependencies:

- `Lagrange.cpp` includes only `Lagrange.hpp` — nothing else
- `GershgorinAnalyzer.cpp` includes only `GershgorinAnalyzer.hpp` + standard lib
- `Menu.cpp` includes concrete headers (it's the composition root)
- `Display.cpp` includes base class headers (works polymorphically)

### Can you remove a module without breaking the system?

- Remove all LU files? The rest still compiles (just remove menu options)
- Remove Interpolation files? Same — Menu is the only place that references it
- Remove EigenSolver? Same pattern

This is because each feature area (solvers, interpolation, eigen) is a separate
subtree. The only coupling point is the Menu, which is intentional — it's
the composition root that assembles everything.

### Dependency direction

Dependencies flow **inward**:

```
Menu → Display → Solvers/Interpolation/Eigen → Matrix
```

Matrix depends on nothing (except standard library). Solvers depend on Matrix.
Display depends on solvers. Menu depends on everything (but nothing depends on
Menu). This is clean unidirectional dependency.

---

# PART 4: DESIGN DECISIONS UNDER SCRUTINY

---

## Where is encapsulation weakest?

`getRowPointer()` returns a raw pointer to internal data. Anyone with that
pointer can modify matrix elements without bounds checking. This exists for
CUDA performance.

## Where is abstraction leaking?

`SolverResult` exposes `double**` L and U matrices that the caller must
manually free with `freeLU()`. This is a manual resource management leak.
Ideally, `SolverResult` would own and clean up its own data with RAII.

## Which class is hardest to extend?

`Matrix` itself. Adding a new feature (like sparse matrix support) would
require modifying the core `double**` data structure, which would ripple
through every solver, every operation, everything. This is the cost of
choosing raw arrays — they're fast but rigid.

## What breaks first under scale?

For very large matrices (10000×10000), the `double**` approach wastes memory
on row pointers and loses cache locality. A contiguous `double*` with manual
`[i * cols + j]` indexing would be faster. The CUDA codepath already does this
conversion when it flattens for `cudaMemcpy`.

## Can this system handle memory failures?

If `new[]` throws `std::bad_alloc` during matrix construction, the constructor
is partially through allocation. Earlier rows are allocated but later ones
aren't. The destructor would try to free all `rows` entries, some of which
were never allocated. This is a potential bug with raw memory management that
smart pointers would prevent.

## Biggest design smell?

The `verify.cpp` file (verification suite) duplicates source file includes
in its own Makefile section. If a new source file is added, it must be added
to both SRCS lists. This is a maintenance burden.

---

# PART 5: QUICK-FIRE ANSWERS

| Question | Answer |
|----------|--------|
| Can we create object of abstract class? | No — has pure virtual functions |
| Can abstract class have constructor? | Yes — used by derived constructors |
| Does friend break encapsulation? | Yes — controlled escape hatch |
| Can constructor be virtual? | No — object not formed yet, no vtable |
| Can destructor be virtual? | Yes — MUST be for polymorphic bases |
| What if base destructor not virtual? | Undefined behavior, memory leak |
| Can virtual function be static? | No — needs object for vptr |
| Can we override private virtual? | Yes — but can't call directly |
| Shallow copy vs deep copy? | Shallow copies pointer, deep copies data |
| What does move solve? | Avoids expensive deep copy for temporaries |
| Which operators can't be overloaded? | `.`, `::`, `?:`, `sizeof` |
| Struct vs class difference? | Default access: struct=public, class=private |
| Can static access non-static? | No — no `this` pointer |
| What is size of empty class? | 1 byte (for unique address) |
| Can pure virtual have definition? | Yes — but class still abstract |
| Is friend inherited? | No — friendship is not transitive |
| Can friend be virtual? | No — friend is not a member |
