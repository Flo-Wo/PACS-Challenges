You shoud use type alias to simplify typing.

Moreover, sometimes is is simpler to be less verbose even if imprecise. For example instead of

```cpp
template <int vec_size, typename vec_type>
Eigen::Matrix<vec_type, vec_size, 1> grad_obj_func(
    Eigen::Matrix<vec_type, vec_size, 1> const& x)
```
you can do

```cpp
auto grad_obj_func(auto const& x);
```
and let the compiler do the job.  Of course here you may be too generic. 

Since C++20 we can constrain the template function using concepts. In the Utilities of the 
examples I have created a concept to identify Eigen matricies. It is not foll proof, but if you use it you can write something of the form

```cpp
EigenMatrix auto grad_obj_func(EigenMatrix auto const& x);
```
which is more readable and less error prone.

Other comments are in the code.


