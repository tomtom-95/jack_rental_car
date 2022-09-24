#include <iostream>

int cars_num = 5;

double dynamics(int next_a_cars, int next_b_cars, int reward, int prev_a_cars, int prev_b_cars, int action);
double update_state_value(int a_cars, int b_cars, int policy[cars_num][cars_num], double state_value[cars_num][cars_num]);
int argmax(int a_cars, int b_cars, double state_value[cars_num][cars_num]);

// p(s', r | s, a)
//TODO: problem with my implementation (I think due to the policy definition) I do not explore other states and I find as best value function
double dynamics(int next_a_cars, int next_b_cars, int reward, int prev_a_cars, int prev_b_cars, int action) {
  int a_cars_taken = std::min(prev_a_cars - action + 2, 3);
  int b_cars_taken = std::min(prev_b_cars + action + 3, 4);

  int real_reward = 10 * a_cars_taken + 10 * b_cars_taken - 2 * abs(action);

  if ((next_a_cars == prev_a_cars - a_cars_taken + 2) &&
      (next_b_cars == prev_b_cars - b_cars_taken + 3) &&
      (reward == real_reward)) {
    return 1;
  }
  else {
    return 0;
  }
}

double update_state_value(int a_cars, int b_cars, int policy[cars_num][cars_num], double state_value[cars_num][cars_num]) {
  double new_value = 0;
  for (int i = 0; i != cars_num; ++i) {
    for (int j = 0; j != cars_num; ++j) {
      for (int reward = -10; reward != 70; ++reward) {
        new_value += dynamics(i, j, reward, a_cars, b_cars, policy[a_cars][b_cars]) * (reward + 0.9 * state_value[i][j]);
      }
    }
  }
  return new_value;
}

int argmax(int a_cars, int b_cars, double state_value[cars_num][cars_num]) {
  double value = 0;
  double max_value = 0;
  int max_action = 0;
  for (int a = -5 ; a != 6; ++a) {
    for (int i = 0; i != cars_num; ++i) {
      for (int j = 0; j != cars_num; ++j) {
        for (int reward = -10; reward != 70; ++reward) {
          value = dynamics(i, j, reward, a_cars, b_cars, a) * (reward + 0.9 * state_value[i][j]);
        }
      }
    }
    if (max_value < value) {
      max_value = value;
      max_action = a;
    }
  }
  return max_action; 
}

int main(int argc, char *argv[]) {
  double state_value[cars_num][cars_num] = {};
  int policy[cars_num][cars_num] = {};

  bool policy_stable = false;
  while (policy_stable == false) {
    // policy evaluation
    double delta = 10;
    double theta = 1;
    while (delta > theta) {
      delta = 0;
      for (int i = 0; i != cars_num; ++i) {
        for (int j = 0; j != cars_num; ++j) {
          double temp_value = state_value[i][j];
          state_value[i][j] = update_state_value(i, j, policy, state_value);
          std::cout << state_value[i][j] << std::endl;
          delta = std::max(delta, abs(temp_value - state_value[i][j]));
        }
      }
    }

    return 0;

    // policy improvement
    policy_stable = true;
    for (int i = 0; i != cars_num; ++i) {
      for (int j = 0; j != cars_num; ++j) {
        int old_action = policy[i][j];
        policy[i][j] = argmax(i, j, state_value);
        if (old_action != policy[i][j]) {
          policy_stable = false;
        }
      }
    }
  }

  for (int i = 0; i != cars_num; ++i) {
    for (int j = 0; j != cars_num; ++j) {
      std::cout << "state_value[" << i << "][" << j << "] = " << state_value[i][j] << std::endl;
      std::cout << "policy[" << i << "][" << j << "] = " << policy[i][j] << std::endl;
    }
  }
  
  return 0;
}
