 - [Previous section](Introduction.md)
 - [Contents](README.md)

---

# Error Handling

When we write code, error handling is the first thing to
take care of. In _Naxos Solver_ we should catch exceptions
of type `NsException`. This class is a `logic_error`
subclass, which inherits from `exception`. Therefore, it
suffices to catch `exception` instances; this base class has
a method `what()` that returns a string describing the error
occurred.

```C++
#include <naxos.h>
using namespace naxos;
using namespace std;

int main(void)
{
    try {

        // ... CODE OF THE PROGRAM ... //

    } catch (exception& exc) {
        cerr << exc.what() << "\n";
        return 1;
    } catch (...) {
        cerr << "Unknown exception\n";
        return 1;
    }
}
```

It is not a good programming practice to to use exceptions
inside the algorithms body. In most cases, exceptions should
“wrap” our programs.

---

 - [Next section](Variables.md)
