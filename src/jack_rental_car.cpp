#include <iostream>

int main(int arcg, char *argv[]) {
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

  // state value function initialization
  // now I must say that this map as 20*20=400 entries
  std::map<std::tuple<int, int>> state_values;
  // policy initialization
  // <int, int, int> = state and action 
  // now I must say that this map has 10*20*20=4000 entries
  std::map<std::tuple<int, int, int>, double>;

  // after initialization I can say how to implement policy iteration to compute the value function and change the policy accordingly

  return 0;
}
