# agbalan2-tvitkin2-hfaroo9-aliceg3
Final Project


## Build/Run
Move into the `src` folder, run `make`, then run `./main` for the main program. Follow the instructions given to produce the output image or run the BFS or Floyd–Warshall algorithm.
**If you run into compilation issues where it can't find `*.o` files, run `make clean` then rerun the `make` command.**
To test, move into the `src` folder and run `make test`. Afterwards, run `./test`. 

To see the performance differences between the parallel and serial versions of the force directed graph output, first build the program by running `make` in `src`, then run the `graph.py` file by running `python graph.py`. An output image graphing the runtimes of 10 iterations of each version will then be outputted as `compare.png`.

## Sample Output
<p align="center">
  <img width="750" src="src/imgs/fdgOutput.png">
</p>

## Serial vs Parallel
<p align="center">
  <img src="src/imgs/compare.png">
</p>