
///////////////////////////////////////////////////////////////////////////////
// gfxalgebra_test.cpp
//
// Unit tests for code declared in gfxalgebra.hpp.
//
///////////////////////////////////////////////////////////////////////////////

#include <cmath> // for std::sqrt
#include <sstream> // for std::stringstream
#include <utility> // for std::move

#include "gtest/gtest.h"

#include "gfxalgebra.hpp"

TEST(GfxVectorProvidedCode, GfxVectorProvidedCode) {

  { // default constructor
    gfx::vector<int, 3> defaulted;
    EXPECT_TRUE(defaulted.is_zero());
  }

  { // copy constructor
    gfx::vector<int, 3> original{1, 2, 3}, copy(original);
    EXPECT_EQ(original, copy);
  }

  { // move constructor
    gfx::vector<int, 3> original{1, 2, 3}, copy(original), moved(std::move(copy));
    EXPECT_EQ(original, moved);
  }

  { // fill constructor
    gfx::vector<int, 3> filled(5), assigned;
    assigned[0] = assigned[1] = assigned[2] = 5;
    EXPECT_EQ(filled, assigned);
  }


  { // iterator constructor
    // sizes match
    std::vector<int> std_vector{1, 2, 3};
    gfx::vector<int, 3> initialized{1, 2, 3},
                        iterated(std_vector.begin(), std_vector.end());
    EXPECT_EQ(initialized, iterated);

    // iterator has _fewer_ elements, others default to zero
    gfx::vector<int, 5> longer(std_vector.begin(), std_vector.end());
    EXPECT_EQ(1, longer[0]);
    EXPECT_EQ(2, longer[1]);
    EXPECT_EQ(3, longer[2]);
    EXPECT_EQ(0, longer[3]);
    EXPECT_EQ(0, longer[4]);

    // iterator has _more_ elements, extras are ignored
    gfx::vector<int, 2> shorter(longer.begin(), longer.end());
    EXPECT_EQ(1, shorter[0]);
    EXPECT_EQ(2, shorter[1]);
  }

  { // initializer list constructor
    gfx::vector<int, 3> initialized{1, 2, 3}, assigned;
    assigned[0] = 1;
    assigned[1] = 2;
    assigned[2] = 3;
    EXPECT_EQ(assigned, initialized);
  }

  { // operator=
    gfx::vector<int, 3> original{1, 2, 3}, assigned;
    EXPECT_TRUE(assigned.is_zero());
    assigned = original;
    EXPECT_EQ(original, assigned);
  }

  { // operator[]
    gfx::vector<int, 3> v{1, 2, 3};
    EXPECT_EQ(1, v[0]);
    EXPECT_EQ(2, v[1]);
    EXPECT_EQ(3, v[2]);
    v[0] = 7;
    EXPECT_EQ(7, v[0]);
    EXPECT_EQ(2, v[1]);
    EXPECT_EQ(3, v[2]);
    v[1] = 8;
    EXPECT_EQ(7, v[0]);
    EXPECT_EQ(8, v[1]);
    EXPECT_EQ(3, v[2]);
    v[2] = 9;
    EXPECT_EQ(7, v[0]);
    EXPECT_EQ(8, v[1]);
    EXPECT_EQ(9, v[2]);
  }

  gfx::vector<int, 3> b{-4, 5, 6};

  { // operator<< (stream insert)
    std::stringstream ss;
    ss << b;
    auto str = ss.str();
    EXPECT_EQ("<-4, 5, 6>", str);
  }

  { // const iterator
    std::vector<int> v(b.begin(), b.end());
    ASSERT_EQ(3, v.size());
    EXPECT_EQ(-4, v[0]);
    EXPECT_EQ(5, v[1]);
    EXPECT_EQ(6, v[2]);
  }

  { // non-const iterator
    auto b_copy = b;
    for (auto& x : b_copy) {
      x = -x;
    }
    ASSERT_EQ(3, b_copy.dimension());
    EXPECT_EQ(4, b_copy[0]);
    EXPECT_EQ(-5, b_copy[1]);
    EXPECT_EQ(-6, b_copy[2]);
  }

  { // Size and indices.
    gfx::vector<int, 0> d0;
    EXPECT_EQ(0, d0.dimension());
    gfx::vector<int, 1> d1;
    EXPECT_EQ(1, d1.dimension());
    gfx::vector<int, 2> d2;
    EXPECT_EQ(2, d2.dimension());
    gfx::vector<int, 3> d3;
    EXPECT_EQ(3, d3.dimension());
    gfx::vector<int, 4> d4;
    EXPECT_EQ(4, d4.dimension());

    EXPECT_FALSE(d0.is_index(0));

    EXPECT_TRUE(d1.is_index(0));
    EXPECT_FALSE(d1.is_index(1));

    EXPECT_TRUE(d2.is_index(0));
    EXPECT_TRUE(d2.is_index(1));
    EXPECT_FALSE(d2.is_index(2));

    EXPECT_TRUE(d3.is_index(0));
    EXPECT_TRUE(d3.is_index(1));
    EXPECT_TRUE(d3.is_index(2));
    EXPECT_FALSE(d3.is_index(3));

    EXPECT_TRUE(d4.is_index(0));
    EXPECT_TRUE(d4.is_index(1));
    EXPECT_TRUE(d4.is_index(2));
    EXPECT_TRUE(d4.is_index(3));
    EXPECT_FALSE(d4.is_index(4));
  }

  // vector2, vector3, vector4
  {
    gfx::vector2<double> a;
    gfx::vector3<double> b;
    gfx::vector4<double> c;
  }
}

TEST(GfxVectorMiscOperators, GfxVectorProvidedCode) {

  { // operator==, operator!=
    gfx::vector<int, 3> zero, another_zero, twos(2);
    EXPECT_TRUE(zero == zero);
    EXPECT_TRUE(zero == another_zero);
    EXPECT_FALSE(zero == twos);
    EXPECT_TRUE(another_zero == zero);
    EXPECT_TRUE(another_zero == another_zero);
    EXPECT_FALSE(another_zero == twos);
    EXPECT_FALSE(twos == zero);
    EXPECT_FALSE(twos == another_zero);
    EXPECT_TRUE(twos == twos);

    EXPECT_FALSE(zero != zero);
    EXPECT_FALSE(zero != another_zero);
    EXPECT_TRUE(zero != twos);
    EXPECT_FALSE(another_zero != zero);
    EXPECT_FALSE(another_zero != another_zero);
    EXPECT_TRUE(another_zero != twos);
    EXPECT_TRUE(twos != zero);
    EXPECT_TRUE(twos != another_zero);
    EXPECT_FALSE(twos != twos);
  }

  gfx::vector<int, 3> a{1, 2, 3}, b{-4, 5, -6};

  { // operator+
    auto c = a + b;
    EXPECT_EQ(-3, c[0]);
    EXPECT_EQ(7, c[1]);
    EXPECT_EQ(-3, c[2]);
  }

  { // operator- (negation)
    auto c = -b;
    EXPECT_EQ(4, c[0]);
    EXPECT_EQ(-5, c[1]);
    EXPECT_EQ(6, c[2]);
  }

  { // operator- (subtraction)
    auto c = a - b;
    EXPECT_EQ(5, c[0]);
    EXPECT_EQ(-3, c[1]);
    EXPECT_EQ(9, c[2]);
  }

  { // operator* (scalar product)
    auto c = b * 10;
    EXPECT_EQ(-40, c[0]);
    EXPECT_EQ(50, c[1]);
    EXPECT_EQ(-60, c[2]);
  }

  { // operator/ (scalar division)
    gfx::vector<int, 3> c{100, 200, -30},
                        d = c / 10;
    EXPECT_EQ(10, d[0]);
    EXPECT_EQ(20, d[1]);
    EXPECT_EQ(-3, d[2]);
  }
}

TEST(GfxVectorDotAndCrossProduct, GfxVectorProvidedCode) {

  gfx::vector<int, 3> a{1, 2, 3}, b{-4, 5, -6};

  { // operator* (dot product)
    auto c = a * b;
    EXPECT_EQ( (1*-4) + (2*5) + (3*-6), c);
  }
  // cross product
  { // example from https://mathinsight.org/cross_product_examples
    gfx::vector<double, 3> lhs{3, -3, 1},
                           rhs{4, 9, 2},
                           expected{-15, -2, 39};
    EXPECT_TRUE(lhs.cross(rhs).approx_equal(expected, .01));
  }
  { // example from http://tutorial.math.lamar.edu/Classes/CalcII/CrossProduct.aspx
    gfx::vector<double, 3> lhs{0, 1, 1},
                           rhs{1, -1, 3},
                           expected{4, 1, -1};
    EXPECT_TRUE(lhs.cross(rhs).approx_equal(expected, .01));
  }
  { // example from https://www.mathsisfun.com/algebra/vectors-cross-product.html
    gfx::vector<double, 3> lhs{2, 3, 4},
                           rhs{5, 6, 7},
                           expected{-3, 6, -3};
    EXPECT_TRUE(lhs.cross(rhs).approx_equal(expected, .01));
  }
  { // example from https://math.oregonstate.edu/home/programs/undergrad/CalculusQuestStudyGuides/vcalc/crossprod/crossprod.html
    gfx::vector<double, 3> lhs{3, -2, -2},
                           rhs{-1, 0, 5},
                           expected{-10, -13, -2};
    EXPECT_TRUE(lhs.cross(rhs).approx_equal(expected, .01));
  }
}

TEST(GfxVectorConversions, GfxVectorProvidedCode) {
  // Converting to other types.
  { // grow and add zeroes
    gfx::vector<int, 2> small{1, 2};
    gfx::vector<int, 4> grown = small.grow<4>();
    ASSERT_EQ(4, grown.dimension());
    EXPECT_EQ(1, grown[0]);
    EXPECT_EQ(2, grown[1]);
    EXPECT_EQ(0, grown[2]);
    EXPECT_EQ(0, grown[3]);
  }
  { // grow and add non-zeroes
    gfx::vector<int, 2> small{1, 2};
    gfx::vector<int, 4> grown = small.grow<4>(5);
    ASSERT_EQ(4, grown.dimension());
    EXPECT_EQ(1, grown[0]);
    EXPECT_EQ(2, grown[1]);
    EXPECT_EQ(5, grown[2]);
    EXPECT_EQ(5, grown[3]);
  }
  { // shrink
    gfx::vector<int, 5> five{1, 2, 3, 4, 5};
    ASSERT_EQ(5, five.dimension());
    gfx::vector<int, 3> shrink_to_three = five.shrink<3>(),
                        expected_three{1, 2, 3};
    EXPECT_EQ(expected_three, shrink_to_three);
    gfx::vector<int, 1> shrink_to_one = five.shrink<1>(),
                        expected_one{1};
    ASSERT_EQ(expected_one, shrink_to_one);
  }
  { // subvector
    gfx::vector<int, 5> five{1, 2, 3, 4, 5};
    gfx::vector<int, 2> two_at_0{1, 2},
                        two_at_1{2, 3},
                        two_at_3{4, 5};
    // explicit start index
    EXPECT_EQ(two_at_0, five.subvector<2>(0));
    EXPECT_EQ(two_at_1, five.subvector<2>(1));
    // default start index
    EXPECT_EQ(two_at_0, five.subvector<2>());
    // subvector is entire thing
    EXPECT_EQ(five, five.subvector<5>());
    // subvector includes last index
    EXPECT_EQ(two_at_3, five.subvector<2>(3));
  }
  { // to_column_matrix, to_row_matrix
    gfx::vector<int, 3> vec{1, 2, 3};
    gfx::matrix<int, 1, 3> row{1, 2, 3};
    gfx::matrix<int, 3, 1> column{1, 2, 3};
    EXPECT_EQ(row, vec.to_row_matrix());
    EXPECT_EQ(column, vec.to_column_matrix());
  }
}

TEST(GfxVectorMiscFunctions, GfxVectorProvidedCode) {
  // approx_equal
  {
    gfx::vector<double, 3> x{1.0, 2.0, 3.0},
                           y{1.1, 2.0, 3.0},
                           z{1.0, 2.0, 3.1};
    EXPECT_TRUE(x.approx_equal(x, .01));
    EXPECT_FALSE(x.approx_equal(y, .01));
    EXPECT_FALSE(x.approx_equal(z, .03));
    EXPECT_TRUE(x.approx_equal(y, .2));
    EXPECT_TRUE(x.approx_equal(z, .2));
  }

  { // cross, is_unit, is_zero, magnitude, magnitude_squared, normalized
    const double delta = .01,
                 rad_one_third = std::sqrt(1.0 / 3.0),
                 rad_14 = std::sqrt(14.0);
    const gfx::vector<double, 3> zero,
                                 x{1.0, 0.0, 0.0},
                                 y{0.0, 1.0, 0.0},
                                 z{0.0, 0.0, 1.0},
                                 a{1.0, 2.0, 3.0},
                                 b{-2.0, 1.5, 6.0},
                                 c{rad_one_third, rad_one_third, rad_one_third};
    const double b_magnitude_squared = b[0]*b[0] + b[1]*b[1] + b[2]*b[2],
                 b_magnitude = std::sqrt(b_magnitude_squared);

    EXPECT_FALSE(zero.is_unit(delta));
    EXPECT_TRUE(x.is_unit(delta));
    EXPECT_TRUE(y.is_unit(delta));
    EXPECT_TRUE(z.is_unit(delta));
    EXPECT_FALSE(a.is_unit(delta));
    EXPECT_FALSE(b.is_unit(delta));
    EXPECT_TRUE(c.is_unit(delta));

    EXPECT_TRUE(zero.is_zero());
    EXPECT_FALSE(x.is_zero());
    EXPECT_FALSE(y.is_zero());
    EXPECT_FALSE(z.is_zero());
    EXPECT_FALSE(a.is_zero());
    EXPECT_FALSE(b.is_zero());
    EXPECT_FALSE(c.is_zero());

    EXPECT_TRUE(gfx::approx_equal(0.0, zero.magnitude_squared(), delta));
    EXPECT_TRUE(gfx::approx_equal(0.0, zero.magnitude(), delta));
    EXPECT_TRUE(gfx::approx_equal(1.0, x.magnitude_squared(), delta));
    EXPECT_TRUE(gfx::approx_equal(1.0, x.magnitude(), delta));
    EXPECT_TRUE(gfx::approx_equal(1.0, y.magnitude_squared(), delta));
    EXPECT_TRUE(gfx::approx_equal(1.0, y.magnitude(), delta));
    EXPECT_TRUE(gfx::approx_equal(1.0, z.magnitude_squared(), delta));
    EXPECT_TRUE(gfx::approx_equal(1.0, z.magnitude(), delta));
    EXPECT_TRUE(gfx::approx_equal(14.0, a.magnitude_squared(), delta));
    EXPECT_TRUE(gfx::approx_equal(rad_14, a.magnitude(), delta));
    EXPECT_TRUE(gfx::approx_equal(b_magnitude_squared, b.magnitude_squared(), delta));
    EXPECT_TRUE(gfx::approx_equal(b_magnitude, b.magnitude(), delta));
    EXPECT_TRUE(gfx::approx_equal(1.0, c.magnitude_squared(), delta));
    EXPECT_TRUE(gfx::approx_equal(1.0, c.magnitude(), delta));

    // unit vectors
    EXPECT_TRUE(x.normalized().approx_equal(x, delta));
    EXPECT_TRUE(y.normalized().approx_equal(y, delta));
    EXPECT_TRUE(z.normalized().approx_equal(z, delta));
    EXPECT_TRUE(c.normalized().approx_equal(c, delta));
    // non-unit vectors
    gfx::vector<double, 3> a_normalized{1.0 / rad_14, 2.0 / rad_14, 3.0 / rad_14};
    EXPECT_TRUE(a.normalized().approx_equal(a_normalized, delta));
    gfx::vector<double, 3> b_normalized{-2.0 / b_magnitude, 1.5 / b_magnitude, 6.0 / b_magnitude};
    EXPECT_TRUE(b.normalized().approx_equal(b_normalized, delta));
  }
  { // fill
    gfx::vector<int, 3> v{1, 2, 3};
    v.fill(0);
    EXPECT_TRUE(v.is_zero());
    v.fill(5);
    EXPECT_EQ(5, v[0]);
    EXPECT_EQ(5, v[1]);
    EXPECT_EQ(5, v[2]);
  }

  // other dimensions; almost all the tests above are 3D, so check that other
  // dimensions work too
  {
    gfx::vector<double, 2> lhs2{9, 8},
                           rhs2{31, 15},
                           difference2{9-31, 8-15};
    EXPECT_TRUE((lhs2 - rhs2).approx_equal(difference2, .01));
    gfx::vector<double, 7> lhs7{1, 2, 3, 4, 5, 6, 7},
                           rhs7{-2, -4, -6, -8, -10, -12, -99},
                           sum7{1-2, 2-4, 3-6, 4-8, 5-10, 6-12, 7-99};
    EXPECT_TRUE((lhs7 + rhs7).approx_equal(sum7, .01));
    gfx::vector<float, 5> lhs5{4, -2, 6, 3},
                          rhs5{2, 1.5, 9, -1};
    float dot5 = 4*2 + (-2)*1.5 + 6*9 + 3*(-1);
    EXPECT_TRUE(gfx::approx_equal(dot5, (lhs5 * rhs5), .01f));
    gfx::vector<double, 100> lhs100(33),
                             rhs100(19),
                             sum100(33+19);
    EXPECT_TRUE((lhs100 + rhs100).approx_equal(sum100, .01));
  }
}

TEST(GfxMatrixProvidedCode, GfxMatrixProvidedCode) {

  gfx::matrix<double, 3, 3> digits{1, 2, 3,
                                   4, 5, 6,
                                   7, 8, 9},

                            identity{1, 0, 0,
                                     0, 1, 0,
                                     0, 0, 1};

  { // default constructor
    gfx::matrix<int, 2, 2> defaulted;
    EXPECT_TRUE(defaulted.is_zero());
  }

  { // copy constructor
    gfx::matrix<int, 2, 2> original{1, 2, 3, 4}, copy(original);
    EXPECT_EQ(original, copy);
  }

  { // move constructor
    gfx::matrix<int, 2, 2> original{1, 2, 3, 4}, copy(original), moved(std::move(copy));
    EXPECT_EQ(original, moved);
  }

  { // fill constructor
    gfx::matrix<int, 2, 2> filled(5), assigned;
    assigned[0][0] = assigned[0][1] = assigned[1][0] = assigned[1][1] = 5;
    EXPECT_EQ(filled, assigned);
  }

  { // iterator constructor
    // sizes match
    std::vector<double> std_vector{1, 2, 3, 4, 5, 6, 7, 8, 9};
    gfx::matrix<double, 3, 3> iterated(std_vector.begin(), std_vector.end());
    EXPECT_EQ(iterated, digits);

    // iterator has _fewer_ elements, others default to zero
    gfx::matrix<double, 3, 3> partial(std_vector.begin(), std_vector.begin() + 5),
                              partial_expected{1, 2, 3, 4, 5, 0, 0, 0, 0};
    EXPECT_EQ(partial, partial_expected);

    // iterator has _more_ elements, extras are ignored
    gfx::matrix<double, 2, 2> overage(std_vector.begin(), std_vector.end()),
                              expected{1, 2, 3, 4};
    EXPECT_EQ(overage, expected);
  }

  { // initializer list constructor
    gfx::matrix<int, 2, 3> six{1, 3, 2, 4, 7, 8};
    EXPECT_EQ(1, six[0][0]);
    EXPECT_EQ(3, six[0][1]);
    EXPECT_EQ(2, six[0][2]);
    EXPECT_EQ(4, six[1][0]);
    EXPECT_EQ(7, six[1][1]);
    EXPECT_EQ(8, six[1][2]);
  }

  { // operator=
    gfx::matrix<double, 3, 3> assigned;
    assigned = digits;
    EXPECT_EQ(digits, assigned);
  }

  { // operator[] const
    EXPECT_EQ(1, digits[0][0]);
    EXPECT_EQ(2, digits[0][1]);
    EXPECT_EQ(3, digits[0][2]);
    EXPECT_EQ(4, digits[1][0]);
    EXPECT_EQ(5, digits[1][1]);
    EXPECT_EQ(6, digits[1][2]);
    EXPECT_EQ(7, digits[2][0]);
    EXPECT_EQ(8, digits[2][1]);
    EXPECT_EQ(9, digits[2][2]);
  }

  { // operator[] non-const
    auto copy(digits);
    copy[1][1] = 99;
    EXPECT_EQ(1, copy[0][0]);
    EXPECT_EQ(2, copy[0][1]);
    EXPECT_EQ(3, copy[0][2]);
    EXPECT_EQ(4, copy[1][0]);
    EXPECT_EQ(99, copy[1][1]);
    EXPECT_EQ(6, copy[1][2]);
    EXPECT_EQ(7, copy[2][0]);
    EXPECT_EQ(8, copy[2][1]);
    EXPECT_EQ(9, copy[2][2]);
  }

  { // operator<< (stream insert)
    std::stringstream ss;
    ss << identity;
    auto str = ss.str();
    EXPECT_EQ(std::string("[1 0 0]\n") + "[0 1 0]\n" + "[0 0 1]\n", str);
  }

  { // iterator, const
    gfx::matrix<double, 3, 3>::const_row_iterator it = digits.begin();
    gfx::vector<double, 3> first_row{1, 2, 3},
                           second_row{4, 5, 6},
                           third_row{7, 8, 9};
    EXPECT_EQ(first_row, *it++);
    EXPECT_EQ(second_row, *it++);
    EXPECT_EQ(third_row, *it++);
    EXPECT_EQ(digits.end(), it);
  }

  { // iterator, non-const
    auto modified = digits;
    gfx::matrix<double, 3, 3>::row_iterator it = modified.begin();
    (*it++)[1] = -1;
    (*it++)[1] = -2;
    (*it++)[1] = -3;
    EXPECT_EQ(it, modified.end());
    EXPECT_EQ(-1, modified[0][1]);
    EXPECT_EQ(-2, modified[1][1]);
    EXPECT_EQ(-3, modified[2][1]);
  }

  // height, width
  {
    EXPECT_EQ(3, digits.width());
    EXPECT_EQ(3, digits.height());
    gfx::matrix<int, 9, 2> m;
    EXPECT_EQ(9, m.height());
    EXPECT_EQ(2, m.width());
  }

  // is_column, is_row, is_row_column
  {
    gfx::matrix<double, 3, 2> m;

    EXPECT_TRUE(m.is_column(0));
    EXPECT_TRUE(m.is_column(1));
    EXPECT_FALSE(m.is_column(2));

    EXPECT_TRUE(m.is_row(0));
    EXPECT_TRUE(m.is_row(1));
    EXPECT_TRUE(m.is_row(0));
    EXPECT_FALSE(m.is_row(3));

    EXPECT_TRUE(m.is_row_column(0, 0));
    EXPECT_TRUE(m.is_row_column(2, 1));
    EXPECT_FALSE(m.is_row_column(3, 0));
    EXPECT_FALSE(m.is_row_column(0, 2));
  }

  { // is_same_size
    gfx::matrix<double, 4, 2> m1, m2;
    EXPECT_TRUE(digits.is_same_size(identity));
    EXPECT_TRUE(m1.is_same_size(m2));
    EXPECT_FALSE(digits.is_same_size(m1));
  }

  { // is_square
    gfx::matrix<double, 3, 2> m;
    EXPECT_FALSE(m.is_square());
    EXPECT_TRUE(digits.is_square());
  }

  // matrix2x2, matrix3x3, matrix4x4
  {
    gfx::matrix2x2<double> m2;
    EXPECT_EQ(2, m2.height());
    EXPECT_EQ(2, m2.width());

    gfx::matrix3x3<double> m3;
    EXPECT_EQ(3, m3.height());
    EXPECT_EQ(3, m3.width());

    gfx::matrix4x4<double> m4;
    EXPECT_EQ(4, m4.height());
    EXPECT_EQ(4, m4.width());
  }
}

TEST(GfxMatrixMiscOperators, GfxMatrixProvidedCode) {

  gfx::matrix<double, 3, 3> digits{1, 2, 3,
                                   4, 5, 6,
                                   7, 8, 9},

                            identity{1, 0, 0,
                                     0, 1, 0,
                                     0, 0, 1};

  { // operator==, operator!=
    auto copy(digits);

    EXPECT_TRUE(digits == digits);
    EXPECT_TRUE(identity == identity);
    EXPECT_TRUE(digits == copy);
    EXPECT_FALSE(digits == identity);

    EXPECT_FALSE(digits != digits);
    EXPECT_FALSE(identity != identity);
    EXPECT_FALSE(digits != copy);
    EXPECT_TRUE(digits != identity);
  }

  { // operator+
    gfx::matrix<double, 3, 3> sum = digits + identity,
                              expected{2, 2, 3,
                                       4, 6, 6,
                                       7, 8, 10};
    EXPECT_TRUE(sum.approx_equal(expected, .01));
  }

  { // operator-(), negate
    gfx::matrix<double, 3, 3> negated = -digits,
                              expected{-1, -2, -3,
                                       -4, -5, -6,
                                       -7, -8, -9};
    EXPECT_TRUE(negated.approx_equal(expected, .01));
  }

  { // operator-, subtraction
    gfx::matrix<double, 3, 3> difference = digits - identity,
                              expected{0, 2, 3,
                                       4, 4, 6,
                                       7, 8, 8};
    EXPECT_TRUE(difference.approx_equal(expected, .01));
  }

  { // operator*, matrix-scalar multiply
    gfx::matrix<double, 3, 3> product = digits * 3.0,
                              expected{3, 6, 9,
                                       12, 15, 18,
                                       21, 24, 27};
    EXPECT_TRUE(product.approx_equal(expected, .01));
  }

  { // operator/, matrix-scalar divide
    gfx::matrix<double, 3, 3> dividend = digits / 2.0,
                              expected{.5, 1, 1.5,
                                       2, 2.5, 3,
                                       3.5, 4, 4.5};
    EXPECT_TRUE(dividend.approx_equal(expected, .01));
  }
}

TEST(GfxMatrixMultiply, GfxMatrixProvidedCode) {
  { // operator*, matrix-matrix-multiply
    // example on page 92 of the textbook

    gfx::matrix<double, 3, 2> lhs{0, 1,
                                  2, 3,
                                  4, 5};
    gfx::matrix<double, 2, 4> rhs{6, 7, 8, 9,
                                  0, 1, 2, 3};
    gfx::matrix<double, 3, 4> expected{ 0,  1,  2,  3,
                                       12, 17, 22, 27,
                                       24, 33, 42, 51};
    EXPECT_TRUE(expected.approx_equal(lhs * rhs, .01));
  }
  {
    // example from https://www.mathsisfun.com/algebra/matrix-multiplying.html
    gfx::matrix<double, 2, 3> lhs{1, 2, 3,
                                  4, 5, 6};
    gfx::matrix<double, 3, 2> rhs{ 7,  8,
                                   9, 10,
                                  11, 12};
    gfx::matrix<double, 2, 2> expected{58, 64,
                                       139, 154};
    EXPECT_TRUE(expected.approx_equal(lhs * rhs, .01));
  }
}

TEST(GfxMatrixMiscFunctions, GfxMatrixProvidedCode) {

  gfx::matrix<double, 3, 3> digits{1, 2, 3,
                                   4, 5, 6,
                                   7, 8, 9},

                            identity{1, 0, 0,
                                     0, 1, 0,
                                     0, 0, 1};

  { // approx_equal
    EXPECT_TRUE(identity.approx_equal(identity, .01));
    EXPECT_TRUE(digits.approx_equal(digits, .01));
    EXPECT_FALSE(identity.approx_equal(digits, .01));
    EXPECT_TRUE(identity.approx_equal(digits, 10));
  }

  { // column_matrix
    gfx::matrix<double, 3, 1> column0{1, 4, 7},
                              column1{2, 5, 8},
                              column2{3, 6, 9};
    EXPECT_EQ(column0, digits.column_matrix(0));
    EXPECT_EQ(column1, digits.column_matrix(1));
    EXPECT_EQ(column2, digits.column_matrix(2));
  }

  { // column_vector
    gfx::vector<double, 3>    column0{1, 4, 7},
                              column1{2, 5, 8},
                              column2{3, 6, 9};
    EXPECT_EQ(column0, digits.column_vector(0));
    EXPECT_EQ(column1, digits.column_vector(1));
    EXPECT_EQ(column2, digits.column_vector(2));
  }

  // fill
  {
    auto filled = digits;
    EXPECT_EQ(filled, digits);
    filled.fill(4);
    EXPECT_EQ(4, filled[0][0]);
    EXPECT_EQ(4, filled[0][1]);
    EXPECT_EQ(4, filled[0][2]);
    EXPECT_EQ(4, filled[1][0]);
    EXPECT_EQ(4, filled[1][1]);
    EXPECT_EQ(4, filled[1][2]);
    EXPECT_EQ(4, filled[2][0]);
    EXPECT_EQ(4, filled[2][1]);
    EXPECT_EQ(4, filled[2][2]);
  }

  // identity
  {
    gfx::matrix<double, 4, 4> m = gfx::matrix<double, 4, 4>::identity();
    EXPECT_EQ(1, m[0][0]);
    EXPECT_EQ(0, m[0][1]);
    EXPECT_EQ(0, m[0][2]);
    EXPECT_EQ(0, m[0][3]);
    EXPECT_EQ(0, m[1][0]);
    EXPECT_EQ(1, m[1][1]);
    EXPECT_EQ(0, m[1][2]);
    EXPECT_EQ(0, m[1][3]);
    EXPECT_EQ(0, m[2][0]);
    EXPECT_EQ(0, m[2][1]);
    EXPECT_EQ(1, m[2][2]);
    EXPECT_EQ(0, m[2][3]);
    EXPECT_EQ(0, m[3][0]);
    EXPECT_EQ(0, m[3][1]);
    EXPECT_EQ(0, m[3][2]);
    EXPECT_EQ(1, m[3][3]);
  }

  { // is_zero
    EXPECT_FALSE(digits.is_zero());
    EXPECT_FALSE(identity.is_zero());

    gfx::matrix<double, 3, 3> z(0);
    EXPECT_TRUE(z.is_zero());

    z[2][2] = .0001;
    EXPECT_FALSE(z.is_zero());
  }

  { // row_matrix
    gfx::matrix<double, 1, 3> row0{1, 2, 3},
                              row1{4, 5, 6},
                              row2{7, 8, 9};
    EXPECT_EQ(row0, digits.row_matrix(0));
    EXPECT_EQ(row1, digits.row_matrix(1));
    EXPECT_EQ(row2, digits.row_matrix(2));
  }

  { // row_vector
    gfx::vector<double, 3>    row0{1, 2, 3},
                              row1{4, 5, 6},
                              row2{7, 8, 9};
    EXPECT_EQ(row0, digits.row_vector(0));
    EXPECT_EQ(row1, digits.row_vector(1));
    EXPECT_EQ(row2, digits.row_vector(2));
  }

  // transpose
  {
    auto t = digits.transpose();
    EXPECT_EQ(1, t[0][0]);
    EXPECT_EQ(4, t[0][1]);
    EXPECT_EQ(7, t[0][2]);
    EXPECT_EQ(2, t[1][0]);
    EXPECT_EQ(5, t[1][1]);
    EXPECT_EQ(8, t[1][2]);
    EXPECT_EQ(3, t[2][0]);
    EXPECT_EQ(6, t[2][1]);
    EXPECT_EQ(9, t[2][2]);
  }
}

TEST(GfxMatrixDeterminant, GfxMatrixProvidedCode) {

  // determinant
  { // 3x3 example on page 98 of textbook
    gfx::matrix<double, 3, 3> m{0, 1, 2, 3, 4, 5, 6, 7, 8};
    EXPECT_TRUE(gfx::approx_equal(0.0, m.determinant(), .01));
  }
	{ // 2x2 example from https://www.mathsisfun.com/algebra/matrix-determinant.html
    gfx::matrix<double, 2, 2> m{4, 6, 3, 8};
    EXPECT_TRUE(gfx::approx_equal(14.0, m.determinant(), .01));
	}
  { // 2x2 examples from https://www.chilimath.com/lessons/advanced-algebra/determinant-2x2-matrix/
    gfx::matrix<double, 2, 2> m1({1, 2, 3, 4});
    EXPECT_TRUE(gfx::approx_equal(-2.0, m1.determinant(), .01));

    gfx::matrix<double, 2, 2> m2({-5, -4, -2, -3});
    EXPECT_TRUE(gfx::approx_equal(7.0, m2.determinant(), .01));

    gfx::matrix<double, 2, 2> m3({-1, -2, 6, 3});
    EXPECT_TRUE(gfx::approx_equal(9.0, m3.determinant(), .01));

    gfx::matrix<double, 2, 2> m4({-4, 2, -8, 7});
    EXPECT_TRUE(gfx::approx_equal(-12.0, m4.determinant(), .01));
  }
  { // 3x3 example from https://www.mathsisfun.com/algebra/matrix-determinant.html
    gfx::matrix<double, 3, 3> m{6, 1, 1, 4, -2, 5, 2, 8, 7};
    EXPECT_TRUE(gfx::approx_equal(-306.0, m.determinant(), .01));
  }
  { // 3x3 examples from https://www.chilimath.com/lessons/advanced-algebra/determinant-3x3-matrix/
    gfx::matrix<double, 3, 3> m1{2, -3, 1, 2, 0, -1, 1, 4, 5};
    EXPECT_TRUE(gfx::approx_equal(49.0, m1.determinant(), .01));

    gfx::matrix<double, 3, 3> m2{1, 3, 2, -3, -1, -3, 2, 3, 1};
    EXPECT_TRUE(gfx::approx_equal(-15.0, m2.determinant(), .01));

    gfx::matrix<double, 3, 3> m3{-5, 0, -1, 1, 2, -1, -3, 4, 1};
    EXPECT_TRUE(gfx::approx_equal(-40.0, m3.determinant(), .01));
  }
}

TEST(GfxMatrixSolve, GfxMatrixProvidedCode) {

  gfx::matrix<double, 3, 3> digits{1, 2, 3,
                                   4, 5, 6,
                                   7, 8, 9},

                            identity{1, 0, 0,
                                     0, 1, 0,
                                     0, 0, 1};
  // solve
  // 2x2 examples from https://www.chilimath.com/lessons/advanced-algebra/cramers-rule-with-two-variables/
	// example 1
	{
    gfx::matrix<double, 2, 2> m{4, -3, 6, 5};
	  gfx::vector<double, 2> b{11, 7},
	                         expected{2, -1},
	                         got = m.solve(b);
    EXPECT_TRUE(got.approx_equal(expected, .01));
	}
	// example 2
	{
    gfx::matrix<double, 2, 2> m{3, 5, 1, 4};
	  gfx::vector<double, 2> b{-7, -14},
	                         expected{6, -5},
	                         got = m.solve(b);
    EXPECT_TRUE(got.approx_equal(expected, .01));
	}
	// example 3
	{
    gfx::matrix<double, 2, 2> m{1, -4, -1, 5};
	  gfx::vector<double, 2> b{-9, 11},
	                         expected{-1, 2},
	                         got = m.solve(b);
    EXPECT_TRUE(got.approx_equal(expected, .01));
	}
	// example 4
	{
    gfx::matrix<double, 2, 2> m{-2, 3, 3, -4};
	  gfx::vector<double, 2> b{-3, 5},
	                         expected{3, 1},
	                         got = m.solve(b);
    EXPECT_TRUE(got.approx_equal(expected, .01));
	}
	// example 5
	{
    gfx::matrix<double, 2, 2> m{5, 1, 3, -2};
	  gfx::vector<double, 2> b{-13, 0},
	                         expected{-2, -3},
	                         got = m.solve(b);
    EXPECT_TRUE(got.approx_equal(expected, .01));
	}
	// 3x3 examples from https://www.chilimath.com/lessons/advanced-algebra/cramers-rule-with-three-variables/
	// example 1
	{
    gfx::matrix<double, 3, 3> m{1, 2, 3, 3, 1, -3, -3, 4, 7};
	  gfx::vector<double, 3> b{-5, 4, -7},
	                         expected{-1, 1, -2},
	                         got = m.solve(b);
    EXPECT_TRUE(got.approx_equal(expected, .01));
	}
	// example 2
	{
    gfx::matrix<double, 3, 3> m{-2, -1, -3, 2, -3, 1, 2, 0, -3};
	  gfx::vector<double, 3> b{3, -13, -11},
	                         expected{-4, 2, 1},
	                         got = m.solve(b);
    EXPECT_TRUE(got.approx_equal(expected, .01));
	}
	// example 3
	{
    gfx::matrix<double, 3, 3> m{0, -1, -2, 1, 0, 3, 7, 1, 1};
	  gfx::vector<double, 3> b{-8, 2, 0},
	                         expected{-1, 6, 1},
	                         got = m.solve(b);
    EXPECT_TRUE(got.approx_equal(expected, .01));
	}
	// example 4
	{
    gfx::matrix<double, 3, 3> m{-2, 1, 1, -4, 2, -1, -6, -3, 1};
	  gfx::vector<double, 3> b{4, 8, 0},
	                         expected{-1, 2, 0},
	                         got = m.solve(b);
    EXPECT_TRUE(got.approx_equal(expected, .01));
	}
	// example 5
	{
    gfx::matrix<double, 3, 3> m{1, -8, 1, -1, 2, 1, 1, -1, 2};
	  gfx::vector<double, 3> b{4, 2, -1},
	                         expected{-3, -4.0/5.0, 3.0/5.0},
	                         got = m.solve(b);
    EXPECT_TRUE(got.approx_equal(expected, .01));
	}

  // submatrix
  {
    auto top_left = digits.submatrix<2, 2>();
    EXPECT_EQ(2, top_left.height());
    EXPECT_EQ(2, top_left.width());
    EXPECT_EQ(1, top_left[0][0]);
    EXPECT_EQ(2, top_left[0][1]);
    EXPECT_EQ(4, top_left[1][0]);
    EXPECT_EQ(5, top_left[1][1]);

    auto not_top_left = digits.submatrix<2, 2>(1, 1);
    EXPECT_EQ(2, not_top_left.height());
    EXPECT_EQ(2, not_top_left.width());
    EXPECT_EQ(5, not_top_left[0][0]);
    EXPECT_EQ(6, not_top_left[0][1]);
    EXPECT_EQ(8, not_top_left[1][0]);
    EXPECT_EQ(9, not_top_left[1][1]);

    auto explicit_row = digits.submatrix<2, 2>(1);
    EXPECT_EQ(2, explicit_row.height());
    EXPECT_EQ(2, explicit_row.width());
    EXPECT_EQ(4, explicit_row[0][0]);
    EXPECT_EQ(5, explicit_row[0][1]);
    EXPECT_EQ(7, explicit_row[1][0]);
    EXPECT_EQ(8, explicit_row[1][1]);

    auto not_square = digits.submatrix<3, 1>();
    EXPECT_EQ(3, not_square.height());
    EXPECT_EQ(1, not_square.width());
    EXPECT_EQ(1, not_square[0][0]);
    EXPECT_EQ(4, not_square[1][0]);
    EXPECT_EQ(7, not_square[2][0]);
  }
}
