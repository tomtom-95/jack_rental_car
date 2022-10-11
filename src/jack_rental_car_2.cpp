#include <iostream>
#include <cmath>
#include <iomanip>

const int MAX_A_CARS = 20;
const int MAX_B_CARS = 20;

const int RENT_REWARD = 10;
const int MOVE_COST = 2;

const int MAX_A_MOVE = 5;
const int MAX_B_MOVE = 5;

const int lambda_rent_a = 3;
const int lambda_rent_b = 4;
const int lambda_ret_a = 3;
const int lambda_ret_b = 2;

struct rhs_params { 
  double R[MAX_A_CARS + 1][MAX_B_CARS + 1] = {};
  double P1[MAX_A_CARS + 1][MAX_A_CARS + 1] = {};
  double P1_t[MAX_A_CARS + 1][MAX_A_CARS + 1] = {};
  double V[MAX_A_CARS + 1][MAX_B_CARS + 1] = {}; 
  double P2[MAX_B_CARS + 1][MAX_B_CARS + 1] = {}; 
  double RHS[MAX_A_CARS + 1][MAX_B_CARS + 1] = {};
};

long long int factorial(int n) {
  long long int fact = 1;
  for (int i = 1; i < n + 1; ++i) {
    fact *= i;
  }
  return fact;
}

// Pr{X = n}
double poisson(int lambda, int n) {
  return (n < 0) ? 0 : std::pow(lambda, n) * std::pow(std::exp(1), -lambda) / factorial(n);
}

// Pr{X >= n}
double cumulative_poisson(int lambda, int n) {
  double cumulative_prob = 0;
  double eps = 0.000001;
  double prob = 1;
  while (prob > eps) {
    prob = poisson(lambda, n++);
    cumulative_prob += prob;
  }
  return cumulative_prob;
}

double mod_poisson(int lambda, int n, int lim) {
  return (n == lim) ? cumulative_poisson(lambda, n) : poisson(lambda, n);
}

void calculate_rhs(struct rhs_params& rhs_params) {
  double temp1[MAX_A_CARS + 1][MAX_B_CARS + 1] = {};
  for (int i = 0; i < MAX_A_CARS + 1; ++i) {
    for (int j = 0; j < MAX_B_CARS + 1; ++j) {
      for (int k = 0; k < MAX_A_CARS + 1; ++k) {
        temp1[i][j] += (rhs_params.P1_t)[i][k] * (rhs_params.V)[k][j];
      }
    }
  }

  double temp2[MAX_A_CARS + 1][MAX_B_CARS + 1] = {};
  for (int i = 0; i < MAX_A_CARS + 1; ++i) {
    for (int j = 0; j < MAX_B_CARS + 1; ++j) {
      for (int k = 0; k < MAX_B_CARS + 1; ++k) {
        temp2[i][j] += temp1[i][k] * (rhs_params.P2)[k][j];
      }
    }
  }

  for (int i = 0; i < MAX_A_CARS + 1; ++i) {
    for (int j = 0; j < MAX_B_CARS + 1; ++j) {
      (rhs_params.RHS)[i][j] = (rhs_params.R)[i][j] + 0.9 * temp2[i][j];
    }
  }
}


int main(void) {
  std::cout << std::fixed;
  std::cout << std::setprecision(0);

  struct rhs_params rhs_params;

  // R
  for (int i = 0; i < MAX_A_CARS + 1; ++i) {
    for (int j = 0; j < MAX_B_CARS + 1; ++j) {
      for (int a_cars = 0; a_cars < i + 1; ++a_cars) {
        (rhs_params.R)[i][j] += a_cars * mod_poisson(lambda_rent_a, a_cars, i);
      }
      for (int b_cars = 0; b_cars < j + 1; ++b_cars) {
        (rhs_params.R)[i][j] += b_cars * mod_poisson(lambda_rent_b, b_cars, j);
      }
      (rhs_params.R)[i][j] *= RENT_REWARD;
    }
  }
  
  // p(N1'|N1''): prob of starting with N1'' cars and ending with N1'
  for (int i = 0; i < MAX_A_CARS + 1; ++i) { // N'
    for (int j = 0; j < MAX_A_CARS + 1; ++j) { // N''
      for (int a_cars = 0; a_cars < j + 1; ++a_cars) { // cars rented
        (rhs_params.P1)[i][j] += mod_poisson(lambda_rent_a, a_cars, j) * 
                                 mod_poisson(lambda_ret_a, i - j + a_cars, MAX_A_CARS - j + a_cars);
      }
    }
  }

  // P1_t
  for (int i = 0; i < MAX_A_CARS + 1; ++i) {
    for (int j = 0; j < MAX_A_CARS + 1; ++j) {
      (rhs_params.P1_t)[i][j] = (rhs_params.P1)[j][i];
    }
  }

  // p(N2'|N2''): prov of starting with N2'' cars and ending with N2'
  for (int i = 0; i < MAX_B_CARS + 1; ++i) { // N'
    for (int j = 0; j < MAX_B_CARS + 1; ++j) { // N''
      for (int b_cars = 0; b_cars < j + 1; ++b_cars) {
        (rhs_params.P2)[i][j] += mod_poisson(lambda_rent_b, b_cars, j) * 
                                 mod_poisson(lambda_ret_b, i - j + b_cars, MAX_B_CARS - j + b_cars);
      }
    }
  }

  int policy[MAX_A_CARS + 1][MAX_B_CARS + 1] = {};

  bool policy_stable = false;
  while (policy_stable == false) {
    policy_stable = true;
    double delta = 1;
    double theta = 0.1;
    while (delta > theta) {
      delta = 0;
      double old_V[MAX_A_CARS + 1][MAX_B_CARS + 1] = {};
      for (int i = 0; i < MAX_A_CARS + 1; ++i) {
        for (int j = 0; j < MAX_B_CARS + 1; ++j) {
          old_V[i][j] = (rhs_params.V)[i][j];
        }
      }

      calculate_rhs(rhs_params);

      for (int i = 0; i < MAX_A_CARS + 1; ++i) {
        for (int j = 0; j < MAX_B_CARS + 1; ++j) {
          int a = policy[i][j];
          int i__ = i - a;
          int j__ = j + a;
          if (i__ >= 0 && i__ <= MAX_A_CARS) {
            if (j__ >= 0 && j__ <= MAX_B_CARS) {
              if (a >= 1) {
                (rhs_params.V)[i][j] = (rhs_params.RHS)[i__][j__] - MOVE_COST * abs(a - 1);
              }
              else {
                (rhs_params.V)[i][j] = (rhs_params.RHS)[i__][j__] - MOVE_COST * abs(a);
              }
            }
          }

          delta = std::max(delta, abs(old_V[i][j] - (rhs_params.V)[i][j]));
        }
      }
    }

    int old_policy[MAX_A_CARS + 1][MAX_B_CARS + 1] = {};
    for (int i = 0; i < MAX_A_CARS + 1; ++i) {
      for (int j = 0; j < MAX_B_CARS + 1; ++j) {
        old_policy[i][j] = policy[i][j];
      }
    }

    for (int i = 0; i < MAX_A_CARS + 1; ++i) {
      for (int j = 0; j < MAX_B_CARS + 1; ++j) {
        double max_value = 0;
        int max_action = 0;
        for (int action = -MAX_A_MOVE; action < MAX_B_MOVE + 1; ++action) {
          int i__ = i - action;
          int j__ = j + action;
          if (i__ >= 0 && i__ <= MAX_A_CARS) {
            if (j__ >= 0 && j__ <= MAX_B_CARS) {
              double value = 0;
              if (action >= 1) {
                value = (rhs_params.RHS)[i__][j__] - MOVE_COST * abs(action - 1);
              }
              else {
                value = (rhs_params.RHS)[i__][j__] - MOVE_COST * abs(action);
              }
              if (value > max_value) {
                max_value = value;
                max_action = action;
              }
            }
          }
        }

        policy[i][j] = max_action;
        if (old_policy[i][j] != policy[i][j]) {
          policy_stable = false;
        }
      }
    }
  }

  for (int i = 0; i < MAX_A_CARS + 1; ++i) {
    for (int j = 0; j < MAX_B_CARS + 1; ++j) {
      std::cout << (rhs_params.V)[i][j] << " ";
    }
    std::cout << '\n';
  }

  for (int i = 0; i < MAX_A_CARS + 1; ++i) {
    for (int j = 0; j < MAX_B_CARS + 1; ++j) {
      std::cout << policy[i][j] << " ";
    }
    std::cout << '\n';
  }

  return 0;
}
