# Requirement
1. ROOT

# Quick Manual (Execute example file)
```
git clone https://github.com/fljs825/submet_ana.git/
cd beamtiming
root -l example.C
```
# Manual
1. Clone `BeamTiming.C` and `BeamTiming.h`
2. Create a new C script. (Namely, `vi test.C`)
3. In test.C,
   ```
   #include "BeamTiming.C"
   #include "BeamTiming.h"

   void test() {
     TString dir = "/path/to/directory";
     BeamTiming bt;
     bt.GetBeamTimings(dir);

     std::cout << bt.GetNbeams() <<std::endl;
   }
   ```
4. Then execute test.C
   ```
   root -l test.C
   ```
   Output is 
   ```
   8 // The number of beam found by the BeamTiming Finder
   ```
## Several useful methods
### 1. Print out beam peak timings
   Input : 
   ```
   for (int i = 0; i < bt.GetNbeams(); i++) {
      std::cout << "iBeams : " << i
      << " | counts : " << counts . at(i)
      << " | arising : " << arisings . at(i)
      << " | peak timing : " << timings . at(i)
      << " | falling : " << fallings.at(i)
      << std::endl;
   }
   ```
   Output :
   ```
   iBeams : 0 | counts : 3652 | arising : 225.591 | peak timing : 304 | falling : 388.724
   iBeams : 1 | counts : 4304 | arising : 701.998 | peak timing : 784 | falling : 864.083
   iBeams : 2 | counts : 3714 | arising : 1169.63 | peak timing : 1264 | falling : 1347.8
   iBeams : 3 | counts : 4114 | arising : 1653.86 | peak timing : 1744 | falling : 1821.8
   iBeams : 4 | counts : 3122 | arising : 2122.11 | peak timing : 2224 | falling : 2302.52
   iBeams : 5 | counts : 2710 | arising : 2598.95 | peak timing : 2704 | falling : 2784.57
   iBeams : 6 | counts : 2746 | arising : 3074.88 | peak timing : 3152 | falling : 3260.16
   iBeams : 7 | counts : 3304 | arising : 3553.99 | peak timing : 3632 | falling : 3730.79
   ```
      
### 2. Collect pulses within the beam time window in `myfile.root/ch0`
   Input : 
   ```
   TFile *file = new TFile("myfile.root", "READ");
   TTree *tree = (TTree *) file -> Get("ch0/pulse_ch0");
   TH1D *h1 = new TH1D("h1","h1", 1024/8, 0, 4096);

   double istime;
   tree -> SetBranchAddress("istime", &istime);
   for (int i = 0; i < tree -> GetEntries(); i++) {
      tree -> GetEntry(i);
      if ( bt.isBeamTiming(istime) ) { h1 -> Fill(istime); }
   }
   ```
   Output :

   <img width="746" alt="image" src="https://github.com/user-attachments/assets/38bc583a-43b3-419b-9109-60f90d90eba4" />


# Description ( To be updated... )
## Methods
### Class Declaration : BeamTiming bt;
- ### `void bt.GetBeamTimings(TString dir, const int filetype = 1, double threshold = 1, bool figures = false);`

-  `void bt.GetBeamTimings(...) searches the beam timings.

  - `TString dir` : path to the directory for a corresponding run

  - `const int filetype` :

    - `const int filetype = 0` : For the old version trees. (`istime` is invalid so that used `zctime1` to find pulse timings)
   
    - `const int filetype = 1` : For the current version pules-based trees. `istime` is valid.
   
    - `const int filetype = 2` : For the current version event-based trees.

  - `const int threshold` : adjust beam finding threshold. Recommended : 0.5 < threshold < 1.5.

  - `bool figures` : choose to save beam finding results as a png file. 

    - `bool figures = true` : save figures. 

    - `bool figures = false` : not save figures.

Example : 
```
BeamTiming bt;
bt.GetBeamTimings("/path/to/r00001/", 1, 1, false);
```

* GetBeamTimings() operates without some missing arguments.
Example :
```
BeamTiming bt;
bt.GetBeamTimings("/path/to/r00001"); // Default filetype : 1, threshold : 1, width : 5, save figures : false
```

- ### `std::vector<double> bt.GetTimings();`

  - returns peak timings of the beams as a vector.

Example :
```
std::vector<double> timings;
timings = bt.GetTimings();
cout << timings . at(0) << endl;
```
Gives the first beam's peak timing. 

- ### `std::vector<double> bt.GetCounts();`

returns the number of large pulses at a given pulse timing.

Example : 
```
std::vector<double> counts;
counts = bt.GetCounts();
cout << counts . at(0) << endl;
```
Gives the number of the first beam's large pulses. 

- ### `std::vector<double> bt.GetArisings();`

returns timing at $m - 5\sigma$

- ### `std::vector<double> bt.GetFallings();`

returns timing at $m + 5\sigma$

- ### `bool bt.isBeamTiming(time)`

If input time is in the beam time window, returns `true`. Else, returns `false`

Example :

  - Input : 

    ```
    time = 350; // Let t = 350 is in the beam time window
    cout << bt.isBeamTiming(time); << endl;
    ```

  - Output :

    ```
    1
    ```


