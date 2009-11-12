cmake_minimum_required (VERSION 2.6)
project (rl-learner) 

AUX_SOURCE_DIRECTORY(src SRCS)
add_executable (rl-learner ${SRCS})