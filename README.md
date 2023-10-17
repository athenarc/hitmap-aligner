In order to solve a sequence alignment problem with Hitmap, it is vital that we clarify the exact steps that a user needs to build the code correctly. 

In the bigger picture one has to generate the dataset, generate queries to be aligned, then build the index (Hash Table) and finally execute. However, to get more detailed, it is important to note that to attain proper execution of the code and potentially solving the problem, we make use of a Makefile. 

In that, someone can take a look at the dependences between the .cpp files and more specifically look at the targets. The targets of the Makefile are many since we are trying to build objective files out of every .cpp file. Though, The targets that we need to pay attention to for now are:

1) make all
2) make data_builder
3) make query_builder
4) make build_idx

Important: Data_builder, query_builder and build_idx require a "/bin" folder to exist in the "Aligner" catalog.

**Data_builder**

In order to solve a DNA sequence alignment problem you first need to have some type of reference sequence with which the query sequences will be compared and eventually aligned with. To generate such data you first need to make use of the target data_builder (make data_builder), this builder constructs the executable file out of dataset_creator.cpp with which we will be able to generate DNA reference sequences. The ".out" file is now located in the "/bin" folder. 

The next part in the procedure of data building is the execution of the executable file. "./data_builder.out" will fail to execute but will give you a detailed idea of the exact parameters (command line arguments) needed to run the code properly.

1) '-loc': The location where the data sequence files are going to be created (required)
    Here you just need to specify a path ending with the name of the folder that you want to create in which the DNA sequences will be saved in the form of ".d" files. If the final folder already exists there will be an error. 
    
    "-loc new" could be an example of such a value. In the "/bin" folder now lays a folder named "new"

2) '-dlen': The length of the data sequences to be created (required).
    Self-explainatory.

3) '-ab': The name of the alphabet to be used (required).
    Argument '-ab' can feel quite confusing. What alphabet? Do alphabets have names? And where is this alphabet? Well, the thing is, it doesn't exist. You need to build it yourself and you need to do that in a new folder named "Data". More specifically you need to define a ".conf" file at the '../../Data/Aligner/alphabets.conf' location and inside the configuration file include the name of the alphabeb eg. "ab_name" and at the line underneath type the vocabulary eg. ATCG for a DNA sequence. It doesn't really matter in which order you will type the letters they just needs to be A, T, C and G. At this stage it will not affect the execution but it will prove to be problematic when you query_builder will be built

4) '-dnum': The number of data sequences to be created (required)
    This will obviously determine the number of sequences to be generated but also the number of ".d" files that will consequently be created

**Query_builder**

The query_builder.out executable is responsible for generating the query sequences that will be checked with the reference sequences in the ".d" files. As with data_builder, query_builder takes some command line arguments that need to be clarified.

1) '-conf': The location where the configuration file resides (required)
    Following the idea of dat_builder, query_builder requires the configuration (".conf") file as well. This time the location doesnt matter since you just need to provide the path in the command line. What matters most is the content of the configuration file. 
    
    More specifically, the file should have 6 parameters: dfile, qnum, qlen, qthr, meas and inum.

    1) Dfile: The location of the ".d" file that its sequence will be used as a reference DNA sequence. 

    2) Qnum/qlen: Number of query sequences to be generated/length of each query sequence

    3) Qthr: Is the query threshold

    4) Meas: Distance Function

    5) Inum: Number of Iterations


    All the parameters should be in different lines and after their name, their value should be specified. 

2) '-mode': Two modes. 'datasub' (sequence is taken from the data sequence), 'rand' (random errors in seq)
   

**Build_idx**

Build_idx essentially constructs the indexer and the aligner of Hitmap which takes care of filtering, Bitset operations, windows operations, caclulationg Hamming/Edit distances, creating/storing/loading the index and also developing a lighter aligner with less attributes (class-wise). This becomes more clear if we take a look at HitmapAligner.cpp which is the core of build_idx.out. Moreover, we again need to provide some arguments just like in the previous executions. 

The .cpp file that we go through now is "build_idx.cpp".

1) '-d': The location of data seq file (required)
    It is essential to specify the exact path of a ".d" file containing a reference/data sequence.

2) '-c': The location of the index config file (required). 
    Just like before, we have to define a ".conf" configuration file containing:

    1) i: Index type. Can take the value of either "hitmap" or "hitmap2". The difference lies in the type of the aligner that the code will construct in these cases. In Hitmap2, the aligner is the "LightAligner" that we encountered in "HitmapAligner.cpp/.h".
    
    2) q: Query length
    
    3) r/e: Error ratio/Error threshold. These parameters are 2 different ways to declare how many "errors" you will allow matchings to have.  

    4) f: Fragments number. Given a query Q and an integer f, where 0 < f ≤ q, Q is divided into φ = floor(q/f) non-overlapping subsequences of length f, called fragments

    5) b: Hash table blocks number

    6) s: Bitset block size

    7) a: Alphabet

The format of the file should be: <param_name> <param_value> and there should be one pair in each line. 

**All**

"Make all" target produces an executable file in the "/bin" catalog named dn_aligner.out. Trying to execute this ".out" file we come across some more command line arguments most of which we have encountered already in previous command line parameters.

1) '-exp': Determines the location of the configuration file that contains the details of the experiment (required)
    This is nothing more than the configuration file we used for query_builder.out

2) '-mode': The mode of the runs: time_measurement, validation, time_measurement_verbose (required)
    *** need to clarify which values work ***

3) '-qthr': The query threshold (optional).

4)  '-idx': The index file (optional).
    We should specify the path of the file that was generated by the process of build_idx.out





    




