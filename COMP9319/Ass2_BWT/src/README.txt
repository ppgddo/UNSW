COMP9319 - Assignment 2

By Timothy Hale (z2273160)

* This README.txt is indended for Dr Raymond Wong.

* This is my final version for submission, but I have a few ideas about how to optimize it which I might do in my
  spare time.
  
* I use the "BackwardSearch" function to first find the string and for the -a or -r options I also use the 
  same function but with the "findIndex" flag set, so that it will simply travel back through the string 
  to find the first occurance of the open and closed square brackets that indicates the corresponding
  index. For the -n option, I simply return the value of (last - first + 1), which is how many total occurances
  were found.
  
* The "BackwardSearch" function uses a fixed sized array (size = 128) for the "Count" data and uses the  "RankOcc"
  function to get the "Rank/Occurance" data, which functionality depends on the size of the input BWT data. For 
  smaller files (~1MB), a "m_readDataFromFiles" flag is set to indicate that complete btw data is loaded into memory, 
  which is faster and doesn't go over the memory contraints. However, for larger files it will only read "blocks" of
  the input data and only store it in memory temporarily as needed for the current operation.
  
  Then for files less than roughly 15MB, the "Rank/Occurance" data is also stored in memory as an array which
  as Raymond suggested gives the current "Rank/Occurance" of a given character at set intervals. Therefore, to get 
  the exact "Rank/Occurance" value at a desired "row" of the BWT "matrix", you then iterate through the BWT data from
  the preceeding interval position and count the occurances up to the desired row. Then once the files are larger than
  the roughly 15MB, this array is saved to the binary index file and then read from the file as needed. Obviously,   
  reading data from a file is slower than from memory, so this option is only used when the index file can get large 
  enough such that the resulting "intervals" are small enough to compensate for the delays caused by the file I/O operations.