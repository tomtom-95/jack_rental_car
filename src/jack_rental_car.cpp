#include <iostream>
#include <cmath>

#define MAX_CARS 20
#define MAX_ACTION 5
#define RENT_REWARD 10
#define MOVE_COST 2

int factorial(int k);
double poisson(double lambda, int k);
double cumulative_poisson(double lambda, int k);
double poisson_mod(double lambda, int k, int limit);
double rent_reward(int car, double lambda);

int factorial(int k) {
  int fact = 1;

  for (int i = 1; i < k + 1; i++) {
    fact *= i;
  }

  return fact;
}

double poisson(double lambda, int k) {
  if (k < 0 || k > 10) {
    return 0;
  }
  else {
    return (pow(lambda, k) * pow(M_E, -lambda)) / factorial(k);
  }
}

double cumulative_poisson(double lambda, int k) {
  double cumulative_prob = 0;
  double prob = 0;
  double eps = 0.001;

  do {
    prob = poisson(lambda, k++);
    cumulative_prob += prob;
  }
  while (prob > eps);

  return cumulative_prob;
}

double poisson_mod(double lambda, int k, int limit) {
  if (k == limit) {
    return cumulative_poisson(lambda, k);
  }
  else {
    return poisson(lambda, k);
  }
}

// expected reward from one place
double rent_reward(int n, double lambda) {
  double reward = 0;

  for (int rent = 1; rent < n + 1; rent++) {
    reward += rent * poisson_mod(lambda, rent, n);
  }
  reward *= RENT_REWARD;

  return reward;
}

int main(int argc, char *argv[]) {
  // poisson expectations for rent
  double lambda_a = 2;
  double lambda_b = 3;
  // poisson expectations for return
  double mu_a = 3;
  double mu_b = 4;

  // r(s,a)
  double expected_reward[MAX_CARS + 1][MAX_CARS + 1][2 * MAX_ACTION + 1] = {};
  for (int i = 0; i < MAX_CARS + 1; i++) {
    for (int j = 0; j < MAX_CARS + 1; j++) {
      for (int a = 0; a < 2 * MAX_ACTION + 1; a++) {
        int action = a - MAX_ACTION;
        if (i - action < 0 || j + action < 0) {
          expected_reward[i][j][a] = 0;
        }
        else {
          expected_reward[i][j][a] = rent_reward(i - action, lambda_a) +
                                      rent_reward(j + action, lambda_b) -
                                      MOVE_COST * abs(action);
        }
      }
    }
  }

  /*
  for (int i = 0; i < MAX_CARS + 1; i++) {
    for (int j = 0; j < MAX_CARS + 1; j++) {
      for (int a = 0; a < 2 * MAX_ACTION + 1; a++) {
        std::cout << expected_reward[i][j][a] << "  ";
      }
      std::cout << '\n';
    }
  }
  */

  // p(s'|s,a)
  double transition_prob_a[MAX_CARS + 1][MAX_CARS + 1][2 * MAX_ACTION + 1] = {};
  double transition_prob_b[MAX_CARS + 1][MAX_CARS + 1][2 * MAX_ACTION + 1] = {};

  for (int i = 0; i < MAX_CARS + 1; i++) {
    for (int j = 0; j < MAX_CARS + 1; j++) {
      for (int a = 0; a < 2 * MAX_ACTION + 1; a++) {
        int action = a - MAX_ACTION;
        if (j - action < 0) {
          transition_prob_a[i][j][a] = 0;
        }
        else {
          for (int rent = 0; rent < j - action + 1; rent++) {
            if (i - (j - action) + rent >= 0) {
              transition_prob_a[i][j][a] += poisson_mod(lambda_a, rent, j - action) * 
                                            poisson_mod(mu_a, i - (j - action) + rent, MAX_CARS - (j - action) + rent);
            }
          }
        }
        if (j + action < 0) {
          transition_prob_b[i][j][a] = 0;
        }
        else {
          for (int rent = 0; rent < j + action + 1; rent++) {
            if (i - (j + action) + rent >= 0) {
              transition_prob_b[i][j][a] += poisson_mod(lambda_b, rent, j + action) *
                                            poisson_mod(mu_b, i - (j + action) + rent, MAX_CARS - (j + action) + rent);
            }
          }
        }
      }
    }
  }

  // se le probabilità di transizione sono sensate: devo utilizzare 
  // expected_reward, transition_prob_a, trannsition_prob_b per risolvere il policy iteration

  /*
  for (int i = 0; i < MAX_CARS + 1; i++) {
    for (int j = 0; j < MAX_CARS + 1; j++) {
      std::cout << i << " " << j << ": ";
      for (int a = 0; a < 2 * MAX_ACTION + 1; a++) {
        std::cout << transition_prob_a[i][j][a] << "  ";
      }
      std::cout << '\n';
    }
  }
  */


  double state_values[MAX_CARS + 1][MAX_CARS + 1] = {};
  int policy[MAX_CARS + 1][MAX_CARS + 1] = {};
  
  // policy evaluation
  double delta = 0;
  double theta = 1;
  do {
    delta = 0;
    for (int i = 0; i < MAX_CARS + 1; i++) {
      for (int j = 0; j < MAX_CARS + 1; j++) {
        double old_value = state_values[i][j];
        double first_term = expected_reward[i][j][policy[i][j] + MAX_ACTION];
        double second_term = 0;
        for (int k = 0; k < MAX_CARS + 1; k++) {
          for (int q = 0; q < MAX_CARS + 1; q++) {
            second_term += transition_prob_a[k][i][policy[i][j] + MAX_ACTION] * 
                           transition_prob_b[q][j][policy[i][j] + MAX_ACTION] *
                           state_values[k][q];
          }
        }
        second_term *= 0.9;
        state_values[i][j] = first_term + second_term;
        delta = std::max(delta, abs(old_value - state_values[i][j]));
      }
    }
  }
  while (delta > theta);

  for (int i = 0; i < MAX_CARS + 1; i++) {
    for (int j = 0; j < MAX_CARS + 1; j++) {
      std::cout << state_values[i][j] << "  ";
    }
    std::cout << '\n';
  }

  // policy improvement
  bool policy_stable = true;
  do {
    policy_stable = true;
    for (int i = 0; i < MAX_CARS + 1; i++) {
      for (int j = 0; j < MAX_CARS + 1; j++) {
        int old_action = policy[i][j];
        policy[i][j] = argmax(state_values, expected_reward, transition_prob_a, transition_prob_b);
      }
    }
  }


  return 0;
}

// TODO: do not rush: think carefully this stuff
int argmax(double state_values[MAX_CARS + 1][MAX_CARS + 1]) {
  for (int i = 0; i < MAX_CARS + 1; i++) {
    for (int j = 0; j < MAX_CARS + 1; j++) {
      double old_value = state_values[i][j];
      double first_term = expected_reward[i][j][policy[i][j] + MAX_ACTION];
      double second_term = 0;
      for (int k = 0; k < MAX_CARS + 1; k++) {
        for (int q = 0; q < MAX_CARS + 1; q++) {
          second_term += transition_prob_a[k][i][policy[i][j] + MAX_ACTION] * 
                         transition_prob_b[q][j][policy[i][j] + MAX_ACTION] *
                         state_values[k][q];
        }
      }
      second_term *= 0.9;
      state_values[i][j] = first_term + second_term;
  
}
