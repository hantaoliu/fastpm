#include <stdio.h>
#include <string.h>
#include <alloca.h>
#include <mpi.h>
#include <math.h>
#include <stdlib.h>

#include <fastpm/libfastpm.h>
#include <fastpm/io.h>
#include <fastpm/logging.h>

#include <bigfile.h>
#include <bigfile-mpi.h>

int main(int argc, char * argv[]) {

    MPI_Init(&argc, &argv);

    libfastpm_init();

    MPI_Comm comm = MPI_COMM_WORLD;

    fastpm_set_msg_handler(fastpm_default_msg_handler, comm, NULL);

    FastPMConfig * config = & (FastPMConfig) {
        .nc = 128,
        .boxsize = 128.,
        .alloc_factor = 2.0,
        .omega_m = 0.292,
        .vpminit = (VPMInit[]) {
            {.a_start = 0, .pm_nc_factor = 2},
            {.a_start = -1, .pm_nc_factor = 0},
        },
        .FORCE_TYPE = FASTPM_FORCE_FASTPM,
        .nLPT = 2.5,
        .COMPUTE_POTENTIAL = 1,
    };
    FastPMSolver solver[1];

    fastpm_solver_init(solver, config, comm);

    FastPMStore store[1];
    fastpm_store_init(store, 1024*1024, PACK_AEMIT | PACK_POS, FASTPM_MEMORY_HEAP);
    double r[] = {0, 1, 2, 3, 4, 5, 6, 7};
    double a[] = {0, 1, 2, 3, 4, 5, 6, 7};

    read_angular_grid(store, "healpix64", r, a, 8, 1, MPI_COMM_WORLD);

    write_snapshot(solver, store, "angulargrid", "", 1, NULL);

    fastpm_store_destroy(store);
    fastpm_solver_destroy(solver);
    libfastpm_cleanup();
    MPI_Finalize();
    return 0;
}

