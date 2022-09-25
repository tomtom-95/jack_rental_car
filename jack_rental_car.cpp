// I AM A STUPID MORON WITH AN UGLY BUTT AND MY BUTT SMELL WHY CAN'T I FIGURE OUT SOMETHING SO SIMPLE???

#include <iostream>
#include <vector>

double dynamics(int cars_num, int next_a_cars, int next_b_cars, int reward, int prev_a_cars, int prev_b_cars, int action);
double update_state_value(const int cars_num, int a_cars, int b_cars, int policy[cars_num][cars_num], double state_value[cars_num][cars_num]);

// int argmax(int a_cars, int b_cars, double state_value[][]);

// p(s', r | s, a)
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

double update_state_value(const int cars_num, int a_cars, int b_cars, int policy[cars_num][cars_num], double state_value[cars_num][cars_num]) {
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

/*
int argmax(int cars_num, int a_cars, int b_cars, double state_value[][]) {
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
*/

// p(s', r | s, a)
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


double dynamics(int a_cars_nxt, int b_cars_nxt, int reward, int a_cars_prev, int b_cars_prev, int action) {
  return 0;
}

double update_state_value(int a_cars, int b_cars, std::vector<std::vector<double>> state_values, std::vector<std::vector<int>> policy) {
  double new_value = 0;
  for (int i = 0; i != state_values.size(); ++i) {
    for (int j = 0; j != stat)
  }


  for (auto i: state_values) {
    for (auto j: i) {
      for (int r = -10; r != 71; ++r) {
        new_value += dynamics()
      }
    }
  }



  for (int i = 0; i != cars_num; ++i) {
    for (int j = 0; j != cars_num; ++j) {
      for (int reward = -10; reward != 70; ++reward) {
        new_value += dynamics(i, j, reward, a_cars, b_cars, policy[a_cars][b_cars]) * (reward + 0.9 * state_value[i][j]);
      }
    }
  }
  return new_value;
}

int main(int argc, char *argv[]) {
  int cars_num = 5;

  std::vector<std::vector<double>> state_values(cars_num, std::vector<double>(cars_num));
  std::vector<std::vector<int>> policy(cars_num, std::vector<int>(cars_num));

  return 0;

  /*
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
          state_value[i][j] = update_state_value(cars_num, i, j, policy, state_value);
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
        // policy[i][j] = argmax(i, j, state_value);
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
  */
}
