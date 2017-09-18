use Time;
use Random;

config const numDarts = 10000000;

proc main ()
{
  var rng = new RandomStream(parSafe=false);

  var totalCount : int = 0;

  const startTime = getCurrentTime();
  for id in 1..numDarts {
    var x = rng.getNext();
    var y = rng.getNext();
    if (x*x + y*y < 1.0) {
      totalCount += 1;
    }
  }
  const stopTime = getCurrentTime();

  writeln("PI = ", "%{#.########}".format(totalCount * 4.0 / numDarts), "  ",
          "Time = ", stopTime - startTime);
}
