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

const double gamma = 0.9;
const double theta = 0.1;

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


void calculate_R(struct rhs_params& rhs_params) {
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
}

void calculate_transition_prob(struct rhs_params& rhs_params) {
  // P1
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

  // P2
  for (int i = 0; i < MAX_B_CARS + 1; ++i) { // N'
    for (int j = 0; j < MAX_B_CARS + 1; ++j) { // N''
      for (int b_cars = 0; b_cars < j + 1; ++b_cars) {
        (rhs_params.P2)[i][j] += mod_poisson(lambda_rent_b, b_cars, j) * 
                                 mod_poisson(lambda_ret_b, i - j + b_cars, MAX_B_CARS - j + b_cars);
      }
    }
  }
}


void calculate_RHS(struct rhs_params& rhs_params) {
  // P1_t * V
  double temp1[MAX_A_CARS + 1][MAX_B_CARS + 1] = {};
  for (int i = 0; i < MAX_A_CARS + 1; ++i) {
    for (int j = 0; j < MAX_B_CARS + 1; ++j) {
      for (int k = 0; k < MAX_A_CARS + 1; ++k) {
        temp1[i][j] += (rhs_params.P1_t)[i][k] * (rhs_params.V)[k][j];
      }
    }
  }

  // R + 0.9 * P1_t * V * P2
  double temp2[MAX_A_CARS + 1][MAX_B_CARS + 1] = {};
  for (int i = 0; i < MAX_A_CARS + 1; ++i) {
    for (int j = 0; j < MAX_B_CARS + 1; ++j) {
      for (int k = 0; k < MAX_B_CARS + 1; ++k) {
        temp2[i][j] += temp1[i][k] * (rhs_params.P2)[k][j];
      }
      (rhs_params.RHS)[i][j] = (rhs_params.R)[i][j] + gamma * temp2[i][j];
    }
  }
}


int main(void) {
  struct rhs_params rhs_params;

  calculate_R(rhs_params); 
  calculate_transition_prob(rhs_params);

  int policy[MAX_A_CARS + 1][MAX_B_CARS + 1] = {};

  bool policy_stable = false;
  while(!policy_stable) {
    policy_stable = true;

    double delta = 1;
    while(delta > theta) {
      delta = 0;

      double old_V[MAX_A_CARS + 1][MAX_B_CARS + 1];
      for (int i = 0; i < MAX_A_CARS + 1; ++i) {
        for (int j = 0; j < MAX_B_CARS + 1; ++j) {
          old_V[i][j] = (rhs_params.V)[i][j];
        }
      }

      calculate_RHS(rhs_params);

      for (int i = 0; i < MAX_A_CARS + 1; ++i) {
        for (int j = 0; j < MAX_B_CARS + 1; ++j) {
          int action = policy[i][j];
          int i__ = i - action;
          int j__ = j + action;
          if (i__ >= 0 && i__ <= MAX_A_CARS && j__ >= 0 && j__ <= MAX_B_CARS) {
            (rhs_params.V)[i][j] = (rhs_params.RHS)[i__][j__] - MOVE_COST * abs(action);
          }
          delta = std::max(delta, abs(old_V[i][j] - (rhs_params.V)[i][j]));
        }
      }
    } // while (delta > theta)

    // policy improvement
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
          if (i__ >= 0 && i__ <= MAX_A_CARS && j__ >= 0 && j__ <= MAX_B_CARS) {
            double value = (rhs_params.RHS)[i__][j__] - MOVE_COST * abs(action);
            if (value > max_value) {
              max_value = value;
              max_action = action;
            }
          }
        }

        policy[i][j] = max_action;
        if (old_policy[i][j] != policy[i][j]) {
          policy_stable = false;
        }
      }
    }
  } // while(!policy_stable)

  /*
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
  */

  return 0;
}
