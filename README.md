#SMPL

**Note:** Development is done on dev branch

**Traditional TMP is an ugly beast.**

 - Bad syntax
 - Unnecesarily verbose
 - Incomprehensible errors

**SMPL greatly improves all 3 of these.**

Here's a short example of a traditional metafunction that computes the number
of elements in a container that satisfy a predicate, squares the result, and
then adds 5.

```cpp
template <typename Predicate, typename List>
struct foo
: add<
      square<
          typename length<
              typename filter<
                  Predicate, List
              >::type
           >::type
      >::type,
      int_<5>
  >
{};
```

Compare to a concatenative approach using SMPL:

```cpp
struct foo : word<filter, length, square, int_<5>, add> {};
```
