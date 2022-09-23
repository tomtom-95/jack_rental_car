#include <iostream>
#include <map>

double dynamics(std::tuple<int,int> next_state, int reward, std::tuple<int,int> prev_state, int action);

double dynamics(std::tuple<int,int> next_state, int reward, std::tuple<int,int> prev_state, int action) {
  int wanna_return_to_A = 2;
  int wanna_take_from_A = 3;
  int wanna_return_to_B = 3;
  int wanna_take_from_B = 4;

  int taken_from_A = std::min(std::get<0>(prev_state) - action + wanna_return_to_A, wanna_take_from_A);
  int taken_from_B = std::min(std::get<1>(prev_state) + action + wanna_return_to_B, wanna_take_from_B);

  // now I can write the actual_next_state
  std::tuple<int,int> actual_next_state = {std::get<0>(prev_state) - taken_from_A, std::get<1>(prev_state) - taken_from_B};
  
  // and the actual reward
  int actual_reward = 10 * taken_from_A + 10 * taken_from_B -  2 * action;

  // in this simplified case all is deterministic, so I can assign porobability 1 to the only case that will happen and 0 if anything else happen
  if (next_state == actual_next_state && reward == actual_reward)
    return 1;
  else
    return 0;
}

int main(int argc, char *argv[]) {
  /*
  20 cars, every night I can move up to 5 cars between places at the cost of 2

  # cars returned to place A: 2
  # cars returned to place B: 3
  # cars taken from place A: 3
  # cars taken from place B: 4

  continuing finite MDP
  gamma = 0.9

  what are the states? (#cars at A, #cars at B)
  what are the actions? #cars to move from A to B (or B to A)
  what are the rewards? 10 for every car rented and -2 for every car moved

  Now that the problem is clear I can implement the policy iteration algorithm (pseudocode at page 80)
  -initialization: what data structure to use?
  -start from a random policy, do policy evaluation to find the value function
  -update the policy as greedy wrt the value funciton just calculated
  */

  // TODO: look here probably somethin wrong
  // state value function initialization
  struct state {
    int cars_at_A;
    int cars_at_B;
  };

  std::map<struct state, double> state_values;
  for (int i = 0; i != 20; ++i) {
    for (int j = 0; j != 20; ++j) {
      struct state stato;
      stato.cars_at_A = i;
      stato.cars_at_B = j;
      state_values[stato] = 0;
    }
  }

  std::map<std::tuple<int, int>, double> state_values;
  for (int i = 0; i != 20; ++i) {
    for (int j = 0; j != 20; ++j) {
        state_values[{i,j}] = 0;
    }
  }


  // policy initialization
  // <int, int, int> = state and action 
  std::map<std::tuple<int, int, int>, double> policy;
  for (int i = 0; i != 20; ++i) {
    for (int j = 0; j != 20; ++j) {
      for (int k = -5; k != 6; ++k) {
        policy[{i, j, k}] = 0;
      }
    }
  }

  return 0;
}
