COMP9319 - Assignment 3

By Timothy Hale (z2273160)

* This README.txt is indended for Dr Raymond Wong.

* I had to abort using my "index file" code, because it had an intermintant bug that I couldn't fix. 
Therefore, I had to "refactor" my code to do the searching as it read the files. Most of the 
"index file" code is still there, but is disabled with hardcoded flags. This is why the code
is a bit "messy", so I appologise about that. I felt like I designed a pretty good index file, 
but I couldn't debug it in time, so I might finish it in the holidays.

* All non-english characters are filtered out and the remaining "words" are used "compared" to
the stored "search terms" after the filtering. I perform all types of the "searching" except
for "substring with space" and most of the test passed within 15 seconds (only 2 test failed).

* I only perform "substring searches" when an "exact search term" isn't found. The "phrase search" 
uses a type of "finite state machine" to keep track of which state it is in and what do to with
the next word detected. I only search for phrases that have a "single space" between the words,
as mentioned in the forums.

* Used the "total count" for each found "search word" for a given file, which is the same as
average frequencies, but I don't like doing "equality test" with floating point numbers.
When there are ties, they are sorted lexigraphically.

* I read the files in large fixed-sized "blocks" to ensure that I don't use more than the 12MB of 
memory.

* This code compiles on windows and linux. I do most of my "debugging" in visual studio before
testing it in Linux.