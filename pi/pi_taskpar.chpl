/**
* 
* Task Parallelization by John Unger and David Thompson
* March 31, 2016
* 
* == SERIAL ==
* PI = 3.14195320  Time = 0.373572
* 
* == TASK PARALLEL ==
* PI = 3.14122360  Time = 0.374349
* PI = 3.14156240  Time = 0.19979
* PI = 3.14096520  Time = 0.107765
* PI = 3.14163200  Time = 0.058368
* PI = 3.14241480  Time = 0.058684
* PI = 3.14211000  Time = 0.059526
* 
* 
* As the data above shows threads increased, the faster the program was able to calculate 
* the value of pi. When there was only one thread given by the run script, 
* the code took the most time. This time increase happened most likely because of having to setup 
* thread-safe mechanisms, that were unneccessary for the serial version. 
* Once the script allowed for 8 threads, we came to a plateau in timing. 
* After 8 threads the communications became burdensome, which resulted in increased time.
* 
* Since this code is written for task parallelization, the seeding of random numbers would have 
* resulted in each thread getting the same random number to work with. 
* We used a differing seed number for each thread, making each random number differ from thread 
* to thread. Similar to pThreads, each thread had a local copy of the count that hit the target. 
* At the end of the loop, we added the local to the global count, resulting in less synchronization 
* and faster timing.
* 
**/

use Time;
use Random;

config const numDarts = 10000000;
config const numThreads = 1;

proc main ()
{

  const startTime = getCurrentTime();
  var totalCount : sync int = 0;

  coforall td in 1..numThreads {
    var count : int = 0;
    var rng = new RandomStream(2*td+1, parSafe=false);

    for id in 1..(numDarts/numThreads) {
      var x = rng.getNext();
      var y = rng.getNext();
      if (x*x + y*y < 1.0) {
        count += 1;
      }
    }
    totalCount += count;
  }
  const stopTime = getCurrentTime();

  writeln("PI = ", "%{#.########}".format(totalCount * 4.0 / numDarts), "  ",
          "Time = ", stopTime - startTime);
}

