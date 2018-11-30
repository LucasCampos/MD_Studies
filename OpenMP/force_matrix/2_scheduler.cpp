/*
 * serial.cpp
 * Copyright (C) 2018 Lucas Costa Campos <rmk236@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#include <iostream>
#include <fstream>
#include <chrono> 
#include "params.h"
class Particles {
    public:
        double *x, *y;
        double *ax, *ay;
        int N;

        Particles(int N) {
            x = new double[N];
            y = new double[N];
            ax = new double[N];
            ay = new double[N];
        }

        ~Particles() {
            delete[] ay;
            delete[] ax;
            delete[] x;
            delete[] y;
        }
};

double force_lj(double rij2) {

    const double _r2 = 1.0/rij2;
    const double _r6 = _r2*_r2*_r2;
    const double _r12 = _r6*_r6;

    return 24*_r2*(2*_r12-_r6);

}

double rot(double x, double box) {

    if(x > box)
        return x - box;
    else if(x < 0)
        return x + box;
    return x;
}

double rot_s(double x, double box) {

    if(x > box/2)
        return x - box;
    else if(x < -box/2)
        return x + box;
    return x;
}

int main() {

    Particles p(N);

    double *x_forces = new double[N*N];
    double *y_forces = new double[N*N];

#pragma omp parallel 
    {
        for(int i=0; i<n; i++) {
#pragma omp for simd schedule(static)
            for(int j=0; j<n; j++) {
                p.x[i*n + j] = i*box/n;
                p.y[i*n + j] = j*box/n + (i==j)*0.3*box/n;

                p.ax[i*n + j] = 0;
                p.ay[i*n + j] = 0;
            }
        }
    }

#pragma omp parallel 
    {
        for(int i=0; i<N-1; i++) {
#pragma omp for simd schedule(static)
            for(int j=i; j<N; j++) {
                x_forces[i*N + j] = 0;
                y_forces[i*N + j] = 0;

                x_forces[j*N + i] = 0;
                y_forces[j*N + i] = 0;
            }
        }
    }
    std::ofstream initial("initial.dat");
    for(int i=0; i<N; i++) {
        initial << p.x[i] << " " << p.y[i] << std::endl;
    }

    auto start = std::chrono::system_clock::now();
#pragma omp parallel 
    {
        for(int step = 0; step < steps; step++) {

#pragma omp for simd schedule(static)
            for(int i=0; i<N; i++) {
                p.x[i] += p.ax[i]*dt;
                p.y[i] += p.ay[i]*dt;

                p.x[i] = rot(p.x[i], box);
                p.y[i] = rot(p.y[i], box);

                p.ax[i] = 0;
                p.ay[i] = 0;
            }

#pragma omp for simd schedule(static)
            for(int i=0; i<N-1; i++) {
                for(int j=i+1; j<N; j++) {
                    const double dx = rot_s(p.x[j] - p.x[i], box);
                    const double dy = rot_s(p.y[j] - p.y[i], box);

                    const double r2 = dx*dx + dy*dy;

                    const double fij = force_lj(r2);

                    x_forces[i*N + j] = fij*dx;
                    y_forces[i*N + j] = fij*dy;

                    x_forces[j*N + i] = -fij*dx;
                    y_forces[j*N + i] = -fij*dy;
                }
            }
#pragma omp for simd schedule(static)
            for(int i=0; i<N; i++) {
                for(int j=0; j<N; j++) {
                    p.ax[i] += x_forces[i*N + j];
                    p.ay[i] += y_forces[i*N + j];
                }
            }
        }
    }
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cout << "Simulation finished in " << elapsed_seconds.count() << std::endl;

    std::ofstream out("final.dat");
    for(int i=0; i<N; i++) {
        out << p.x[i] << " " << p.y[i] << std::endl;
    }

    delete[] y_forces;
    delete[] x_forces;

}
