#include "domain_splitting.h"
#include <naxos.h>

#include <cmath>

using namespace std;
using namespace naxos;

// inline NsInt
// round (const double x)
//{
//	return  ( ( x < 0.0 ) ?  x - 0.5  :  x + 0.5 );
//}

///  Returns a proper split value for the domain-splitting method, concerning
///  the domain of X.

///  It is assumed that it holds that:
///   \li
///   If \a greaterThan is \c false,
///    there is a constraint \a X < \a Y,
///
///   \li
///   else (if \a greaterThan is \c true),
///    there is a constraint \a X > \a Y.

double naxos::splitValueConstrLess(const NsIntVar X, const NsIntVar Y,
                                   const bool greaterThan)
{
        const double Xmin = ((greaterThan) ? -X.max() : X.min());
        const double Xmax = ((greaterThan) ? -X.min() : X.max());
        const double Ymin = ((greaterThan) ? -Y.max() : Y.min());
        const double Ymax = ((greaterThan) ? -Y.min() : Y.max());
        const double splitPercentage = 0.50;
        const double C = min(Xmax, Ymin - 1);
        const double Sum1SuppY = (C - Xmin + 1) * Y.size();
        const double Sum2SuppY = (Xmax - C) * (2.0 * Ymax - Xmax - C - 1) / 2;
        const double A = splitPercentage * (Sum1SuppY + Sum2SuppY);
        // cout << "Sum1SuppY=" << Sum1SuppY << "\n";
        // cout << "Sum2SuppY=" << Sum2SuppY << "\n";
        // cout << "A=" << A << "\n";
        if (Sum1SuppY >= A) {
                return (((greaterThan) ? -1 : 1) *
                        (Xmin - 1 + A / Y.size() + 0.5));
        }
        const double p = 1 - 2.0 * Ymax;
        const double q = (2.0 * Ymax - 1 - C) * C + 2 * (A - Sum1SuppY);
        const double D = p * p - 4 * q;
        assert_Ns(D >= 0, "splitValueConstrLess: Negative discriminant");
        // cout << ( (-p - sqrt(D))  /  2 ) << "\n";
        return (((greaterThan) ? -1 : 1) * ((-p - sqrt(D)) / 2 + 0.5));
}

///  Returns a proper split value for the domain-splitting method, concerning
///  the domain of X--when it holds that X != Y).

double naxos::splitValueConstrNeq(const NsIntVar X, const NsIntVar Y)
{
        const double splitPercentage = 0.50;
        double Sum1SuppY = 0, Sum2SuppY = 0, Sum3SuppY = 0;
        if (X.min() < Y.min())
                Sum1SuppY = min(Y.min() - 1, X.max()) - X.min() + 1.0;
        if ((Y.min() <= X.min() && X.min() <= Y.max()) ||
            (Y.min() <= X.max() && X.max() <= Y.max())) {
                Sum2SuppY =
                    (Y.size() - 1.0) / Y.size() *
                    (min(Y.max(), X.max()) - max(Y.min(), X.min()) + 1.0);
        }
        if (Y.max() < X.max())
                Sum3SuppY = X.max() - max(Y.max() + 1, X.min()) + 1.0;
        const double A = splitPercentage * (Sum1SuppY + Sum2SuppY + Sum3SuppY);
        // cout << "    \t" << ( min(Y.min()-1,X.max()) - X.min() + 1.0 ) <<
        // "\t" << ( min(Y.max(),X.max()) - max(Y.min(),X.min()) + 1.0 ) <<  "\t"
        // << ( X.max() - max(Y.max()+1,X.min()) + 1.0 ) << "\n";  cout <<
        // "Sum:\t" << Sum1SuppY <<  "\t" << Sum2SuppY <<  "\t" << Sum3SuppY <<
        // "\n";
        if (Sum1SuppY >= A)
                return (X.min() - 1 + A + 0.5);
        if (Sum1SuppY + Sum2SuppY >= A) {
                return (max(Y.min(), X.min()) - 1 +
                        (A - Sum1SuppY) * Y.size() / (Y.size() - 1.0) + 0.5);
        }
        return (max(Y.max() + 1, X.min()) - 1 + A - Sum1SuppY - Sum2SuppY +
                0.5);
}
