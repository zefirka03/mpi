#include <mpi.h>
#include "Application.h"


int main(int argc, char** argv) {
    int returned;
    int process_Rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_Rank);

    if (process_Rank == 0) {
        returned = Application::run_parent();
    }
    else {
        returned = Application::run_child(process_Rank);
    }

    MPI_Finalize();

    return returned;
}