#include <iostream>
#include <vector>

int cars_num = 10;

double dynamics(int a_cars_nxt, int b_cars_nxt, int reward, int a_cars_prev, int b_cars_prev, int action);
double update_state_values(int a_cars, int b_cars, std::vector<std::vector<double>> state_values, std::vector<std::vector<int>> policy);

// dynamics for totally deterministic case
double dynamics(int a_cars_nxt, int b_cars_nxt, int reward, int a_cars_prev, int b_cars_prev, int action) {
  int a_cars_taken = std::min(a_cars_prev - action + 2, 3);
  int b_cars_taken = std::min(b_cars_prev + action + 3, 4);

  int a_cars = std::min(a_cars_prev - a_cars_taken + 2, cars_num);
  int b_cars = std::min(b_cars_prev - b_cars_taken + 3, cars_num);

  int real_reward = 10 * a_cars_taken + 10 * b_cars_taken - 2 * abs(action);

  return (a_cars == a_cars_nxt) && (b_cars == b_cars_nxt) && (real_reward == reward);
}

double update_state_values(int a_cars, int b_cars, std::vector<std::vector<double>> state_values, std::vector<std::vector<int>> policy) {
  double new_value = 0;

  for (int i = 0; i != cars_num; ++i) {
    for (int j = 0; j != cars_num; ++j) {
      for (int r = -10; r != 71; ++r) {
        new_value += dynamics(i, j, r, a_cars, b_cars, policy[i][j]) * (r + 0.9 * state_values[i][j]);
      }
    }
  }

  return new_value;
}

int main(int argc, char *argv[]) {
  std::vector<std::vector<double>> state_values(cars_num, std::vector<double>(cars_num));
  std::vector<std::vector<int>> policy(cars_num, std::vector<int>(cars_num));

  // policy evaluation
  double delta = 100;
  double theta = 1;
  while (delta > theta) {
    delta = 0;
    for (int i = 0; i != cars_num; ++i) {
      for (int j = 0; j != cars_num; ++j) {
        double temp_value = state_values[i][j];
        state_values[i][j] = update_state_values(i, j, state_values, policy);
        delta = std::max(delta, abs(temp_value - state_values[i][j]));
      }
    }
  }

  for (auto i: state_values) {
    for (auto j: i) {
      std::cout << j << std::endl;
    }
  }

  return 0;
}
