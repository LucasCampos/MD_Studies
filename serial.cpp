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

    for(int i=0; i<n; i++) {
        for(int j=0; j<n; j++) {
            p.x[i*n + j] = i*box/n + (i==j)*0.5*box/n;;
            p.y[i*n + j] = j*box/n + (i==j)*0.5*box/n;

            p.ax[i*n + j] = 0;
            p.ay[i*n + j] = 0;
        }
    }

    std::ofstream initial("initial.dat");
    for(int i=0; i<N; i++) {
        initial << p.x[i] << " " << p.y[i] << std::endl;
    }

    auto start = std::chrono::system_clock::now();
    for(int step = 0; step < steps; step++) {

        for(int i=0; i<N; i++) {
            p.x[i] += p.ax[i]*dt;
            p.y[i] += p.ay[i]*dt;

            p.x[i] = rot(p.x[i], box);
            p.y[i] = rot(p.y[i], box);

            p.ax[i] = 0;
            p.ay[i] = 0;
        }

        for(int i=0; i<N-1; i++) {
            for(int j=i+1; j<N; j++) {
                const double dx = rot_s(p.x[j] - p.x[i], box);
                const double dy = rot_s(p.y[j] - p.y[i], box);

                const double r2 = dx*dx + dy*dy;

                const double fij = force_lj(r2);

                p.ax[i] += fij*dx;
                p.ay[i] += fij*dy;

                p.ax[j] -= fij*dx;
                p.ay[j] -= fij*dy;

            }
        }
    }
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cout << "Simulation finished in " << elapsed_seconds.count() << 's' << std::endl;

    std::ofstream out("final.dat");
    for(int i=0; i<N; i++) {
        out << p.x[i] << " " << p.y[i] << std::endl;
    }


}
