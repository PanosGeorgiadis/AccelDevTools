#include "CyclicRotation.h"
#include "MaxProfit.h"
#include "realtest.h"

using namespace std;

int main(int argc, char *argv[]) {
  vector<int> test1 = {3, 8, 9, 7, 6};
  int K = 3;
  auto result = solution_cyclicRotation(test1, K);
  for (auto x : result) {
    cout << x << ' ';
  }

  /******************************************************/

  cout << "odd occurences of chars in: " << solution_realtest(28) << endl;

  /******************************************************/

  vector<int> test3 = {23171, 21011, 21123, 21366, 21013, 21367};
  auto strResult = solution_maxprofit(test1);
  cout << "Result: " << strResult << endl;

  return 0;
}