Alright, alright, alright.

So, the problem here was to take a single-threaded implementation of srt and making it
multithreaded.

1. The first step in solving this, really was to seperate the initialisation section of the code
from the part that could actually be multithreaded. I located a nested loop structure that
seemed to be going through each pixel of the image and performing some complex calculations on
it and then printing out the RGB values for that pixel.

I decided that that nested loop structure was the one I had to multithread. Now, I had to come
up with a way to visit each pixel, in a multithreaded manner, such that each pixel gets visited
once and only once, and there are no race conditions or any of the other multithreaded issues.

My first approach was to use a kernel-type approach that I borrowed from computer vision
methods. After attempting to think about it for a while however, I realised that a shape with
variable x and y, would be tricky (because finding a quadrilateral that always visits the
entire image, irrespective of nthreads is tricky). So I came to the conclusion that it would
be easier to do a rectangular shape that visits varying Xs, but always visits ALL the Ys.

The threads would decide their vertical rectangular region before beginning their processing
with the following formula, assuming tnum is an int in the half-open range [0, nthreads),
nthreads represents the actual number of threads, and width is the width of the image:

the startpoint would be: (tnum) * (width/nthreads)
the endpoint would be: (tnum+1) * (width/nthreads)

However, for widths that aren't fully divisible by nthreads, I would have to make the last
thread do the remainder of the processing, so to take care of that I conditionally assigned
endpoint as follows:

int endpoint = (tnum == nthreads - 1) ? width : (tnum+1) * (width/nthreads);

this ensures that the last thread always goes all the way up to the "width" of the image.

And then, the inner loop would go from y = 0  to y < height, thus ensuring that each pixel is
visited once, the threads split up their work and there are no race conditions, where diff.
threads are accessing the same data.

2. Some of the other issues were merely logistical. Such as, when i moved the nested loop to
the function that I was going to multithread, I also had to move a bunch of local variables
along with it to allow compilation. I also had to declare scene and nthreads globally so all the
threads could access it.

3. Lastly, to ensure data was written in the correct order, i had to remove the print statements
from the given loops, and store all the data in my own, globally defined triple-dim array, to
store pixel by pixel RGB values. I then used this array in the end, to print out each pixel's
RGB values in the correct order, independent of when each thread actually processed it.

4. Now, to analyse the time performance of my multithreaded implementation with diff.
number of threads:


time ./srt 1-test.ppm >1-test.ppm.tmp
real 0m48.020s
user 0m48.024s
sys  0m0.003s

time ./srt 2-test.ppm >2-test.ppm.tmp
real 0m24.900s
user 0m48.364s
sys  0m0.007s

time ./srt 4-test.ppm >4-test.ppm.tmp
real 0m16.447s
user 0m48.237s
sys  0m0.005s

time ./srt 8-test.ppm >8-test.ppm.tmp
real 0m10.551s
user 0m50.073s
sys  0m0.004s
mv 8-test.ppm.tmp 8-test.ppm

As is clearly evident, the amount of time taken by the program to complete the ray-tracing
decreases as the number of thread increases! The linearity of the decrease decreases, as
the number of threads increases, but that might have something to do with my approximation
where I tell the last thread to go all the way up to the width of the image.

As I kept increasing the num of threads however (8, 16, 32, etc), I noticed a seemingly
maximum time efficiency that the approach reached - of about 6 seconds. After about 32
threads, the performance gains were negligible even on doubling the nthreads. 

In fact, another interesting result of my approach that I noticed while trying my implementation
with random numbers, was that when I used >= 66 threads, the performance actually drops, because
(width/nthreads) rounds down to 1, which basically means the first 65 threads are doing 1 column
of work and then the 66th column does the remaining work. However, the performance gains are
noticeable until all the way up until 65 columns.
