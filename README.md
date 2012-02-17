###SMPL

Traditional TMP is an ugly beast.
Three primary issues are:
- Bad syntax
- Unnecesarily verbose
- Incomprehensible errors

The proposed library greatly improves on the first two, and hopefully
improves on the third.

Here's a short example of a traditional metafunction that computes the number
of elements in a container that satisfy a predicate, squares the result, and
then adds 5.

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

Compare to a concatenative approach:

struct foo:word<filter, length, square, int_<5>, add> {};