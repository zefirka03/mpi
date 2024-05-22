#include <mpi.h>
#include "Application.h"


int main(int argc, char** argv) {
    int returned;
    int process_rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);
    
    if (process_rank == 0) {
        returned = Application::run_parent();
    }
    else {
        returned = Application::run_child(process_rank);
    }

    MPI_Finalize();

    return returned;
}