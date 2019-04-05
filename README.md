# DiffTool
Basically a fancy LCS (longest common subsequence) solver.

Algorithm here:
https://en.wikipedia.org/wiki/Longest_common_subsequence_problem

Very optimised: precalculations, pre-sizing, trimming of start and end, hashing of the input. 

Primary use was for evaluating the differences of an SVN diff / commit and spitting out whether a certain change had been made.
This was used to avoid a common issue occuring where if stats were modified (removed) it would cause an issue with Grandmasters as they 
store a hash of the stat name in their data file. Grandmasters have to updated (using another tool) so that the hash is changed to match
the new stat name.

Passes the stats file as a command argument and uses that to track differences from a cached version of the file. 
It extracts the differences and spits out whether an action should be made, or what was changed etc. 
Intended to be run as a build step daily by the person in charge of Grandmasters.
