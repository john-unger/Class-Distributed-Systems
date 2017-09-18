/**
* 
* Data Parallelization
* by John Unger and David Thompson
* March 31, 2016
* 
* == SERIAL ==
* PI = 3.14195320  Time = 0.373572
* 
* == DATA PARALLEL ==
* PI = 3.14114400  Time = 0.419761
* PI = 3.14054240  Time = 0.219944
* PI = 3.14186560  Time = 0.119561
* PI = 3.14144440  Time = 0.067184
* PI = 3.14185040  Time = 0.067189
* PI = 3.14230360  Time = 0.067635
* 
* 
* As the data above shows threads increased, the faster the program was able to calculate 
* the value of pi. When there was only one thread given by the run script, 
* the code took the most time. This time increase happened most likely because of having to setup 
* thread-safe mechanisms, that were unneccessary for the serial version. 
* Once the script allowed for 8 threads, we came to a plateau in timing. 
* After 32 threads the communications became burdensome, which resulted in increased time.
* 
* Each thread executed implicitly, having the compiler split up the work between threads automatically. 
* An array was used to keep track of the number of hits. Once all darts have been thrown, 
* the array underwent a reduction giving the total.
* 
**/

use Time;
use Random;

config const numDarts = 10000000;

proc main ()
{
  var rng = new RandomStream(parSafe=true);

  var x : [1..numDarts] real;
  var y : [1..numDarts] real;
  var hits : [1..numDarts] int;

  const startTime = getCurrentTime();

//fillRandom(arr: [] eltType)  
  rng.fillRandom(x);
  rng.fillRandom(y);
 
  var totalCount : int = 0;
 
  //const startTime = getCurrentTime();
  forall i in 1..numDarts {
    if (x[i]*x[i] + y[i]*y[i] < 1.0) {
      hits[i] = 1;
    }
  }
 
  totalCount = + reduce hits;

  const stopTime = getCurrentTime();

  writeln("PI = ", "%{#.########}".format(totalCount * 4.0 / numDarts), "  ",
          "Time = ", stopTime - startTime);
}
