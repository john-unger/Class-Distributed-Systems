/**
 * dht.c
 *
 * CS 470 Project 4
 *
 * Implementation for distributed hash table (DHT).
 *
 * Name: John Unger & David Thompson
 * Date: April 22, 2016
 */

#include "dht.h"
#include "mpi.h"
#include <pthread.h>

/*
 * private module variable: current process ID
 */
static int pid;
 
int provided;   //Variable holding the provided support level for mpi_thread_multiple.
int nprocs;     //Global variable for our number of processes.
int rank;       //Global variable holding rank of current process.
pthread_t serv; //Sever pthread.

//This function is what our server thread processes in an infinite loop until stop.
void* server_work()
{
    //Infinite loop.
    for(;;)
    {
        //Local variables.
        char *key;
        long value;
        //Create the space in memory for all the keys.
        key = (char *)malloc(sizeof(char)*50);

        // MPI RECV for our value variable.
        MPI_Recv(&value, 1, MPI_LONG, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        //Flag for infinite loop termination.
        // If the value is -1, it is a special case to shut down the server
        if (value == -1) 
        {
            printf("BREAKING\n");                                                   //Debug print statement
            return 0; //Leave this function.
        }

        //If not stop flag previously, continue recieving values for the keys. 
        MPI_Recv(key, 50, MPI_CHAR, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("HAVING Received\n");                                                //print debugger
        //Put key and value into our local hash table.
        local_put(key, value);

        //Free the memory of keys.
        free((char *)key);
    }
}

//Distributed hash table initialization and server thread creation.
int dht_init()
{
    //Initialize file storage with our local hash table.
    local_init();

    //Initialize our mpi threads with level of desired thread support and level of provided
    //thread support. 
    MPI_Init_thread(NULL, NULL, MPI_THREAD_MULTIPLE, &provided);
    //Check for the required level of support needed to run the program.
    if (provided != MPI_THREAD_MULTIPLE) {
        printf("ERROR: Cannot initialize MPI in THREAD_MULTIPLE mode.\n");
        exit(EXIT_FAILURE);
    }
    //Determines the size of the group associated with our communicator.
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    //Determines the rank of the calling process in the communicator.
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    //Create space in memory for our server thread. 
    serv = (pthread_t)malloc(sizeof(pthread_t));
    //Create a single pthread.
    pthread_create(&serv, NULL, server_work, NULL);
    
    //printf("Thread created\n");
    //pid = 0; //For the serial version, running only process 0.
    
    //Returns our process ID.
    return pid;
}

//Puts data onto our distributed hash table. 
void dht_put(const char *key, long value)
{
    //Set a local variable to the first letter in incoming key.
    /** Is this doing anything???!?! or does it get overwritten below?**/    
    int firstChar = key[0];
    
    //Set variable from ASCII code into an int using modulo.
    firstChar = (firstChar) % (nprocs);
    
    //printf("Firstchar: %d \t nprocs: %d\n", firstChar, nprocs);

    //Send the value to a server process given our converted char into integer. 
    MPI_Send(&value, 1, MPI_LONG, firstChar, 0, MPI_COMM_WORLD);
    
    //Sending the key to our server. Key, # of elements, datatype, destination, message tag, communicator.
    MPI_Send(key, (sizeof(key) / sizeof(key[0])), MPI_CHAR, firstChar, 1, MPI_COMM_WORLD);
    
    printf("Having Sent\n");                                            //Debug print statement.

    return; //Leave function.
}

//Distributed hash table destroyer.
void dht_destroy()
{
    //Local variable used as a destroy flag.
    long qtz = -1;
    printf("Termination message\n");                                    //debug statement.
    //Sends mpi kill message to server thread telling thread to stop infinite loop. 
    //Destroy flag, # of elements, datatype, destination, message tag, communicator.
    MPI_Send(&qtz, 1, MPI_LONG, rank, 0, MPI_COMM_WORLD);

    printf("About to JOIN\n");                                          //debug statement
    //Join the server thread back to main.
    pthread_join(serv, NULL);
    printf("JOINED\n");                                                 //debug statement
    
    //Free memory of pthread.
    //free((pthread_t *) serv);

    //Char array to hold sentences.
    char fn[64];
    //Creates a text file with a given process rank.
    snprintf(fn, 64, "dump-%d.txt", rank);
    //Opens file to write on.
    FILE *f = fopen(fn, "w");
    //Writes to the given file name, the process rank number.
    fprintf(f, "Process %d\n", rank);
    //Prints all pairs of keys and values of the current process from the temporary file f
    //onto our new text file.
    local_destroy(f);
    //Terminate MPI execution environment.
    MPI_Finalize();
    //Closes files stream.
    fclose(f);
}

