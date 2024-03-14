# Installation and Dependencies
The only dependency is ``eigen``, but the default command ``make`` of the makefile
is designed to work in the PACS environment, set up in the beginning of the
course. Additionally, there is the option ``make debug`` which
included additional debugging information (e.g. the taken descent directions
and information about the intermediate points).

For people with a mac, who installed ``eigen`` via homebrew, we provide
``make EIGEN_MAC=1`` respectively ``make debug EIGEN_MAC=1``.

TLDR:
```shell
make
```
does the job.

# Questions for the Tutor
- In the ``DescentDirectionAbstract`` class, I declared virtual functions
in order to override them. Unfortunately, the two methods behave a bit
diversely, which is why I was forced to provide a ``const`` and non-``const``
methods for each pair of methods.
Is this the standard procedure to solve this issue or is there a cleaner way
to achieve the same result?
- Also in ``DescentDirectionAbstract`` class, I return default values
in the ``get_dir()`` methods to avoid a compiler warning (non-void function
returning nothing). How can I do that in a better way, since this feels like
a trick and is probably hard to catch as a user.
- The typing of the objective function and its' gradient in ``main.cpp`` 
were super painful. What is the standard way to this? I could not find super
convincing results on the internet.

# Code Structure and Design Choices
In order to ensure the choice of the descent direction without loosing execution
speed, the gradient computation
and the step size algorithm at runtime, I experimented a bit with polymorphism,
abstracted classes and the factory method design pattern.

We provide all of the three linesearch algorithms, see ``StepSize<Name>.hpp``,
and since they are templates, everything is header-only. They are instantiated 
a run-time by the ``StepSizeFactory.hpp``.

The same is done for the descent direction, here are steepest descent and
momentum supported, but for the sake of simplicity, both of the classes and
the factory method are kept in the same file (I simply wanted to try everything).
To ensure, that we can instantiate the class before entering the generic descent
direction method, we encapsuled the zero-th gradient computation with a default
argument and set it later. Thus, the descent method is not coupled with the
filereader by any means.

We provide analytical and numerical derivatives which are directly located in the
main-file.

All options are parsed from the ``opti_options.dot`` file, by using 
``GetPot.hpp`` as described in the lecture.

# Personal Notes
## Possible Extensions
Currently, there are a couple of points missing, which would be required
to ship the code as a cleaner/full software product

- Setter and getter methods in the classes (not required for the challenge)
- Factory methods for the gradient computation
- Different files for the descent methods and the abstract factory
- Organize everything in a hierarchical folder structure as it is done 
for bigger projects
- etc.

## Learnings
- Polymorphism in C++ relies on dynamic dispatch, which means that the appropriate function to call is determined
    at runtime based on the actual type of the object being referred to.
    This mechanism requires using pointers or references to base class objects rather than the objects themselves.
    * Object Slicing: When you store a derived class object in a base class object, only the base class part of the
    object is retained. This is known as object slicing, and it leads to the loss of derived class functionality.
    Using pointers or references prevents object slicing because they directly refer to the derived class object.
    * Dynamic Dispatch: Polymorphic behavior relies on virtual function calls, which are resolved at runtime.
    When you call a virtual function through a base class pointer or reference, the actual function implementation
    of the derived class object is invoked. This is only possible when using pointers or references because
    the compiler can't determine the exact type of the object at compile time.
    * Memory Management: When dealing with polymorphic objects, it's common to use heap memory allocation (with new)
    and dynamic memory deallocation (with delete). Pointers are essential for managing objects on the heap,
    as they allow you to control the object's lifetime and avoid unnecessary copying.
- More complex examples would require a destructor in the base class and all derived classes to get rid of the
dynamics memory allocation
- Eigen 3.4.0 enables more convenient initialization of vectors and matrices
- Header-only libraries are super easy to include